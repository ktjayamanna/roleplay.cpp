#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "server.h"
#include <stddef.h>
#include <stdint.h>

#define WS_MAX_CLIENTS 50
#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// WebSocket opcodes
typedef enum {
    WS_OPCODE_CONTINUATION = 0x0,
    WS_OPCODE_TEXT = 0x1,
    WS_OPCODE_BINARY = 0x2,
    WS_OPCODE_CLOSE = 0x8,
    WS_OPCODE_PING = 0x9,
    WS_OPCODE_PONG = 0xA
} WebSocketOpcode;

// WebSocket frame structure
typedef struct {
    uint8_t fin;
    uint8_t opcode;
    uint8_t masked;
    uint64_t payload_length;
    uint8_t mask[4];
    char* payload;
} WebSocketFrame;

// WebSocket handshake
int ws_perform_handshake(int client_fd, const char* request);
char* ws_generate_accept_key(const char* client_key);

// Frame handling
WebSocketFrame* ws_read_frame(int client_fd);
int ws_send_frame(int client_fd, uint8_t opcode, const char* payload, size_t length);
void ws_frame_free(WebSocketFrame* frame);

// High-level send functions
int ws_send_text(WebSocketClient* client, const char* message);
int ws_send_binary(WebSocketClient* client, const void* data, size_t length);
int ws_send_close(WebSocketClient* client);
int ws_send_pong(WebSocketClient* client, const char* payload, size_t length);

// Client management
WebSocketClient* ws_client_create(int fd, const char* path);
void ws_client_destroy(WebSocketClient* client);
WebSocketClient* ws_get_client(int client_id);

// Utility
int ws_is_upgrade_request(const char* request);
const char* ws_get_websocket_key(const char* request);

#endif

