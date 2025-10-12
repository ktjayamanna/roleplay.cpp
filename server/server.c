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

// TODO: Add your includes here
#include "server.h"
#include "http.h"
#include "routes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// TODO: Create global Server instance
Server server;


// TODO: Implement server_init()
int server_init(int port) {
    server.port = port;
    server.is_running = 0;
    server.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server.socket_fd == -1) {
        perror("socket");
        return -1;
    }
    int opt = 1;
    if (setsockopt(server.socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server.socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return -1;
    }
    if (listen(server.socket_fd, 10) == -1) {
        perror("listen");
        return -1;
    }
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

// TODO: Implement handle_client()

void handle_client(int client_fd) {
    char buffer[1024];
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        return;
    }
    buffer[bytes_read] = '\0';
    char method[10], path[100];
    http_parse_request(buffer, method, path);
    char* response = handle_route(method, path);
    write(client_fd, response, strlen(response));
    free(response);
}

