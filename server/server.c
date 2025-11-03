#define _GNU_SOURCE
#include "server.h"
#include "http.h"
#include "endpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    int socket_fd;
    int port;
    int is_running;
} InternalServer;

static InternalServer server;
static void handle_client(int client_fd);

int server_init(int port) {
    printf("Initializing server on port %d...\n", port);

    server.port = port;
    server.is_running = 0;

    endpoint_system_init();

    server.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server.socket_fd == -1) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(server.socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt SO_REUSEADDR");
        close(server.socket_fd);
        return -1;
    }

    if (setsockopt(server.socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        perror("setsockopt SO_REUSEPORT");
        close(server.socket_fd);
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    printf("Attempting to bind to port %d (socket_fd=%d)...\n", port, server.socket_fd);
    if (bind(server.socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server.socket_fd);
        return -1;
    }
    printf("Successfully bound to port %d\n", port);

    if (listen(server.socket_fd, 10) == -1) {
        perror("listen");
        return -1;
    }

    printf("Server initialized successfully\n");
    return 0;
}

int server_start() {
    server.is_running = 1;
    while (server.is_running) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server.socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }
        handle_client(client_fd);
        close(client_fd);
    }
    return 0;
}

void server_stop() {
    server.is_running = 0;
    close(server.socket_fd);
}

static void parse_url(const char* url, char* path, char* query_string) {
    const char* question_mark = strchr(url, '?');
    if (question_mark) {
        size_t path_len = question_mark - url;
        strncpy(path, url, path_len);
        path[path_len] = '\0';
        strcpy(query_string, question_mark + 1);
    } else {
        strcpy(path, url);
        query_string[0] = '\0';
    }
}

HttpResponse* handle_route_with_body(char* method, char* url, const char* content_type, char* body, int body_length) {
    char path[256];
    char query_string[512];

    parse_url(url, path, query_string);

    EndpointResponse* endpoint_response = endpoint_dispatch_with_body(method, path, query_string, content_type, body, body_length);

    if (endpoint_response) {
        HttpResponse* http_response = http_build_binary_response(
            endpoint_response->status_code,
            endpoint_response->body,
            endpoint_response->body_length,
            endpoint_response->content_type
        );

        endpoint_response_free(endpoint_response);
        return http_response;
    } else {
        const char* error_body = "{\"error\": \"Endpoint not found\"}";
        HttpResponse* http_response = http_build_binary_response(404, error_body, strlen(error_body), "application/json");
        return http_response;
    }
}

static void handle_client(int client_fd) {
    char header_buffer[4096];
    ssize_t bytes_read = read(client_fd, header_buffer, sizeof(header_buffer) - 1);
    if (bytes_read <= 0) {
        return;
    }
    header_buffer[bytes_read] = '\0';

    char method[10], url[256];
    http_parse_request(header_buffer, method, url);

    char content_type[128];
    http_get_content_type(header_buffer, content_type, sizeof(content_type));
    int content_length = http_get_content_length(header_buffer);

    const char* body_in_buffer = http_find_body(header_buffer);
    char* body = NULL;
    int body_length = 0;

    if (content_length > 0 && body_in_buffer) {
        int body_already_read = bytes_read - (body_in_buffer - header_buffer);

        body = malloc(content_length);
        if (!body) {
            return;
        }

        memcpy(body, body_in_buffer, body_already_read);

        int remaining = content_length - body_already_read;
        if (remaining > 0) {
            int total_read = 0;
            while (total_read < remaining) {
                ssize_t additional = read(client_fd, body + body_already_read + total_read, remaining - total_read);
                if (additional <= 0) {
                    free(body);
                    return;
                }
                total_read += additional;
            }
        }
        body_length = content_length;
    }

    HttpResponse* response = handle_route_with_body(method, url, content_type, body, body_length);
    write(client_fd, response->body, response->body_length);

    free(response->body);
    free(response);
    if (body) {
        free(body);
    }
}

static HttpMethod parse_method_string(const char* method_str) {
    if (strcmp(method_str, "GET") == 0) return HTTP_METHOD_GET;
    if (strcmp(method_str, "POST") == 0) return HTTP_METHOD_POST;
    if (strcmp(method_str, "PUT") == 0) return HTTP_METHOD_PUT;
    if (strcmp(method_str, "DELETE") == 0) return HTTP_METHOD_DELETE;
    return HTTP_METHOD_GET;
}

int server_register_handler(const char* path, const char* method, EndpointHandler handler) {
    printf("Registering custom endpoint: %s %s\n", method, path);

    HttpMethod http_method = parse_method_string(method);
    return endpoint_register(path, http_method, handler);
}

const char* request_get_param(const RequestContext* request, const char* param_name) {
    return endpoint_get_param(request, param_name);
}

int request_get_param_int(const RequestContext* request, const char* param_name, int default_value) {
    return endpoint_get_param_int(request, param_name, default_value);
}

const char* request_get_body(const RequestContext* request) {
    return request->body;
}

EndpointResponse* response_json(int status_code, const char* json_body) {
    return endpoint_json_response(status_code, json_body);
}

EndpointResponse* response_text(int status_code, const char* text_body) {
    return endpoint_create_response(status_code, text_body, "text/plain");
}

EndpointResponse* response_error(int status_code, const char* error_message) {
    return endpoint_error_response(status_code, error_message);
}

