#define _GNU_SOURCE
#include "websocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

static WebSocketClient clients[WS_MAX_CLIENTS];
static int next_client_id = 1;

// Base64 encoding for WebSocket handshake
static char* base64_encode(const unsigned char* input, int length) {
    BIO *bio, *b64;
    BUF_MEM *buffer_ptr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &buffer_ptr);

    char* result = malloc(buffer_ptr->length + 1);
    memcpy(result, buffer_ptr->data, buffer_ptr->length);
    result[buffer_ptr->length] = '\0';

    BIO_free_all(bio);
    return result;
}

char* ws_generate_accept_key(const char* client_key) {
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", client_key, WS_GUID);

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)combined, strlen(combined), hash);

    return base64_encode(hash, SHA_DIGEST_LENGTH);
}

const char* ws_get_websocket_key(const char* request) {
    const char* key_header = "Sec-WebSocket-Key: ";
    const char* key_start = strstr(request, key_header);
    if (!key_start) return NULL;

    key_start += strlen(key_header);
    const char* key_end = strstr(key_start, "\r\n");
    if (!key_end) return NULL;

    static char key[256];
    size_t key_len = key_end - key_start;
    if (key_len >= sizeof(key)) key_len = sizeof(key) - 1;
    
    strncpy(key, key_start, key_len);
    key[key_len] = '\0';
    
    return key;
}

int ws_is_upgrade_request(const char* request) {
    return (strstr(request, "Upgrade: websocket") != NULL ||
            strstr(request, "Upgrade: WebSocket") != NULL);
}

int ws_perform_handshake(int client_fd, const char* request) {
    const char* client_key = ws_get_websocket_key(request);
    if (!client_key) {
        fprintf(stderr, "No WebSocket key found in request\n");
        return -1;
    }

    char* accept_key = ws_generate_accept_key(client_key);
    
    char response[512];
    snprintf(response, sizeof(response),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "\r\n",
        accept_key);

    int result = write(client_fd, response, strlen(response));
    free(accept_key);

    return (result > 0) ? 0 : -1;
}

WebSocketFrame* ws_read_frame(int client_fd) {
    uint8_t header[2];
    ssize_t n = read(client_fd, header, 2);
    if (n <= 0) return NULL;

    WebSocketFrame* frame = calloc(1, sizeof(WebSocketFrame));
    frame->fin = (header[0] & 0x80) >> 7;
    frame->opcode = header[0] & 0x0F;
    frame->masked = (header[1] & 0x80) >> 7;
    frame->payload_length = header[1] & 0x7F;

    // Extended payload length
    if (frame->payload_length == 126) {
        uint8_t len[2];
        if (read(client_fd, len, 2) <= 0) {
            free(frame);
            return NULL;
        }
        frame->payload_length = (len[0] << 8) | len[1];
    } else if (frame->payload_length == 127) {
        uint8_t len[8];
        if (read(client_fd, len, 8) <= 0) {
            free(frame);
            return NULL;
        }
        frame->payload_length = 0;
        for (int i = 0; i < 8; i++) {
            frame->payload_length = (frame->payload_length << 8) | len[i];
        }
    }

    // Read mask if present
    if (frame->masked) {
        if (read(client_fd, frame->mask, 4) <= 0) {
            free(frame);
            return NULL;
        }
    }

    // Read payload
    if (frame->payload_length > 0) {
        frame->payload = malloc(frame->payload_length + 1);
        ssize_t total_read = 0;
        while (total_read < frame->payload_length) {
            ssize_t chunk = read(client_fd, frame->payload + total_read, 
                                frame->payload_length - total_read);
            if (chunk <= 0) {
                free(frame->payload);
                free(frame);
                return NULL;
            }
            total_read += chunk;
        }
        frame->payload[frame->payload_length] = '\0';

        // Unmask payload
        if (frame->masked) {
            for (uint64_t i = 0; i < frame->payload_length; i++) {
                frame->payload[i] ^= frame->mask[i % 4];
            }
        }
    }

    return frame;
}

int ws_send_frame(int client_fd, uint8_t opcode, const char* payload, size_t length) {
    uint8_t header[10];
    int header_len = 2;

    // FIN bit set, opcode
    header[0] = 0x80 | (opcode & 0x0F);

    // Payload length
    if (length < 126) {
        header[1] = length;
    } else if (length < 65536) {
        header[1] = 126;
        header[2] = (length >> 8) & 0xFF;
        header[3] = length & 0xFF;
        header_len = 4;
    } else {
        header[1] = 127;
        for (int i = 0; i < 8; i++) {
            header[9 - i] = (length >> (i * 8)) & 0xFF;
        }
        header_len = 10;
    }

    // Send header
    if (write(client_fd, header, header_len) <= 0) {
        return -1;
    }

    // Send payload
    if (length > 0 && payload) {
        if (write(client_fd, payload, length) <= 0) {
            return -1;
        }
    }

    return 0;
}

void ws_frame_free(WebSocketFrame* frame) {
    if (frame) {
        if (frame->payload) {
            free(frame->payload);
        }
        free(frame);
    }
}

int ws_send_text(WebSocketClient* client, const char* message) {
    if (!client || !client->is_active) return -1;
    return ws_send_frame(client->fd, WS_OPCODE_TEXT, message, strlen(message));
}

int ws_send_binary(WebSocketClient* client, const void* data, size_t length) {
    if (!client || !client->is_active) return -1;
    return ws_send_frame(client->fd, WS_OPCODE_BINARY, (const char*)data, length);
}

int ws_send_close(WebSocketClient* client) {
    if (!client || !client->is_active) return -1;
    return ws_send_frame(client->fd, WS_OPCODE_CLOSE, NULL, 0);
}

int ws_send_pong(WebSocketClient* client, const char* payload, size_t length) {
    if (!client || !client->is_active) return -1;
    return ws_send_frame(client->fd, WS_OPCODE_PONG, payload, length);
}

WebSocketClient* ws_client_create(int fd, const char* path) {
    for (int i = 0; i < WS_MAX_CLIENTS; i++) {
        if (!clients[i].is_active) {
            clients[i].fd = fd;
            clients[i].id = next_client_id++;
            clients[i].is_active = 1;
            strncpy(clients[i].path, path, sizeof(clients[i].path) - 1);
            clients[i].path[sizeof(clients[i].path) - 1] = '\0';
            return &clients[i];
        }
    }
    return NULL;
}

void ws_client_destroy(WebSocketClient* client) {
    if (client) {
        client->is_active = 0;
        client->fd = -1;
        client->id = 0;
    }
}

WebSocketClient* ws_get_client(int client_id) {
    for (int i = 0; i < WS_MAX_CLIENTS; i++) {
        if (clients[i].is_active && clients[i].id == client_id) {
            return &clients[i];
        }
    }
    return NULL;
}

