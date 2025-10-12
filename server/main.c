/*
 * main.c - Entry point for the web server
 * 
 * INSTRUCTIONS:
 * 1. Include necessary headers (server.h, stdio.h, stdlib.h)
 * 2. Define the port number (8080)
 * 3. In main():
 *    - Print a startup message
 *    - Call server_init() to initialize the server
 *    - Call server_start() to start listening for connections
 *    - Handle any errors appropriately
 *    - Return 0 on success
 * 
 * LEARNING GOALS:
 * - Understand program entry points in C
 * - Learn about return codes and error handling
 * - Practice calling functions from other modules
 */

// TODO: Add your includes here
#include <server.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: Define PORT constant (8080)

int main(int argc, char *argv[]) {
    // TODO: Print startup message
    printf("Server starting on port 8080...\n");
    
    // TODO: Initialize server
    if (server_init(8080) == -1) {
        return 1;
    }
    
    // TODO: Start server (this should block and run forever)
    server_start();
    server_stop();
    printf("Server stopped\n");
    
    // TODO: Handle cleanup (if server stops)
    // No cleanup needed for this basic example
    
    
    return 0;
}

