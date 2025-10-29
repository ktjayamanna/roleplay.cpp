/*
 * server.c - Core server implementation
 * 
 * INSTRUCTIONS:
 * 1. Include necessary headers:
 *    - server.h
 *    - stdio.h, stdlib.h, string.h
 *    - unistd.h (for close())
 *    - sys/socket.h, netinet/in.h, arpa/inet.h (for socket operations)
 * 
 * 2. Create a global Server instance
 * 
 * 3. Implement server_init(int port):
 *    - Create a socket using socket(AF_INET, SOCK_STREAM, 0)
 *    - Set socket options with setsockopt() to allow address reuse
 *    - Bind the socket to the port using bind()
 *    - Start listening with listen()
 *    - Return 0 on success, -1 on error
 * 
 * 4. Implement server_start():
 *    - Set is_running flag to 1
 *    - Enter infinite loop while is_running is true
 *    - Accept connections with accept()
 *    - Call handle_client() for each connection
 *    - Close client socket after handling
 * 
 * 5. Implement server_stop():
 *    - Set is_running to 0
 *    - Close the server socket
 * 
 * 6. Implement handle_client(int client_fd):
 *    - Read the HTTP request (you can use a simple buffer)
 *    - Call handle_health_check() to generate response
 *    - Send response back to client
 * 
 * LEARNING GOALS:
 * - Understand socket programming basics
 * - Learn about network byte order and address structures
 * - Practice error handling with system calls
 * - Understand blocking I/O operations
 */

#define _GNU_SOURCE  // For strdup
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

// Internal server structure (hidden from library users)
typedef struct {
    int socket_fd;
    int port;
    int is_running;
} InternalServer;


// Global server instance (internal)
static InternalServer server;

// Forward declarations
static void handle_client(int client_fd);


// Initialize the server
int server_init(int port) {
    printf("Initializing server on port %d...\n", port);

    server.port = port;
    server.is_running = 0;

    // Initialize the endpoint system
    endpoint_system_init();

    // Create socket
    server.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server.socket_fd == -1) {
        perror("socket");
        return -1;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server.socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        return -1;
    }

    // Bind to address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server.socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return -1;
    }

    // Start listening
    if (listen(server.socket_fd, 10) == -1) {
        perror("listen");
        return -1;
    }

    printf("Server initialized successfully\n");
    return 0;
}

// TODO: Implement server_start()

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

// TODO: Implement server_stop()

void server_stop() {
    server.is_running = 0;
    close(server.socket_fd);
}

// Parse URL to separate path and query string
static void parse_url(const char* url, char* path, char* query_string) {
    const char* question_mark = strchr(url, '?');
    if (question_mark) {
        // Copy path part
        size_t path_len = question_mark - url;
        strncpy(path, url, path_len);
        path[path_len] = '\0';

        // Copy query string part
        strcpy(query_string, question_mark + 1);
    } else {
        // No query string
        strcpy(path, url);
        query_string[0] = '\0';
    }
}

char* handle_route(char* method, char* url) {
    char path[256];
    char query_string[512];

    // Parse URL into path and query string
    parse_url(url, path, query_string);

    // Dispatch to endpoint system
    EndpointResponse* endpoint_response = endpoint_dispatch(method, path, query_string, NULL, 0);

    if (endpoint_response) {
        // Build HTTP response using the existing http_build_response function
        HttpResponse* http_response = http_build_response(endpoint_response->status_code, endpoint_response->body);
        char* response_str = strdup(http_response->body);

        // Clean up
        endpoint_response_free(endpoint_response);
        free(http_response->body);
        free(http_response);

        return response_str;
    } else {
        // No endpoint found - return 404
        HttpResponse* http_response = http_build_response(404, "{\"error\": \"Endpoint not found\"}");
        char* response_str = strdup(http_response->body);
        free(http_response->body);
        free(http_response);
        return response_str;
    }
}

// TODO: Implement handle_client()

static void handle_client(int client_fd) {
    char buffer[1024];
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        return;
    }
    buffer[bytes_read] = '\0';
    char method[10], url[256];
    http_parse_request(buffer, method, url);
    char* response = handle_route(method, url);
    write(client_fd, response, strlen(response));
    free(response);
}

// ============================================================================
// LIBRARY API IMPLEMENTATION
// ============================================================================

// Convert method string to enum
static HttpMethod parse_method_string(const char* method_str) {
    if (strcmp(method_str, "GET") == 0) return HTTP_METHOD_GET;
    if (strcmp(method_str, "POST") == 0) return HTTP_METHOD_POST;
    if (strcmp(method_str, "PUT") == 0) return HTTP_METHOD_PUT;
    if (strcmp(method_str, "DELETE") == 0) return HTTP_METHOD_DELETE;
    return HTTP_METHOD_GET; // Default
}

// Simple endpoint handler for static responses
static EndpointResponse* simple_endpoint_handler(const RequestContext* request, const char* response_body, const char* content_type) {
    (void)request; // Suppress unused parameter warning
    return endpoint_create_response(200, response_body, content_type ? content_type : "application/json");
}

// Structure to hold simple endpoint data
typedef struct {
    char* response_body;
    char* content_type;
} SimpleEndpointData;

// Handler wrapper for simple endpoints
static EndpointResponse* simple_endpoint_wrapper(const RequestContext* request) {
    // This is a bit of a hack - we'll store the data in a global array
    // In a real implementation, you'd want a more sophisticated approach
    static SimpleEndpointData simple_endpoints[50];
    static int simple_endpoint_count = 0;

    // For now, just return a basic response
    // TODO: Implement proper data storage for simple endpoints
    (void)request;
    return endpoint_json_response(200, "{\"message\": \"Simple endpoint\"}");
}

// Register a simple endpoint
int server_register_simple(const char* path, const char* method, const char* response_body, const char* content_type) {
    printf("Registering simple endpoint: %s %s\n", method, path);

    HttpMethod http_method = parse_method_string(method);

    // For now, use a simple wrapper - in a full implementation you'd store the response data
    return endpoint_register(path, http_method, simple_endpoint_wrapper);
}

// Register an endpoint with custom handler
int server_register_handler(const char* path, const char* method, EndpointHandler handler) {
    printf("Registering custom endpoint: %s %s\n", method, path);

    HttpMethod http_method = parse_method_string(method);
    return endpoint_register(path, http_method, handler);
}

// Helper functions for request context
const char* request_get_param(const RequestContext* request, const char* param_name) {
    return endpoint_get_param(request, param_name);
}

int request_get_param_int(const RequestContext* request, const char* param_name, int default_value) {
    return endpoint_get_param_int(request, param_name, default_value);
}

const char* request_get_body(const RequestContext* request) {
    return request->body;
}

// Response creation helpers
EndpointResponse* response_json(int status_code, const char* json_body) {
    return endpoint_json_response(status_code, json_body);
}

EndpointResponse* response_text(int status_code, const char* text_body) {
    return endpoint_create_response(status_code, text_body, "text/plain");
}

EndpointResponse* response_error(int status_code, const char* error_message) {
    return endpoint_error_response(status_code, error_message);
}

