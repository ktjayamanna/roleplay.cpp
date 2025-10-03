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

// TODO: Create global Server instance

// TODO: Implement server_init()

// TODO: Implement server_start()

// TODO: Implement server_stop()

// TODO: Implement handle_client()

