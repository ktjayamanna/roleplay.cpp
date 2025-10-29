#include "../server/server.h"
#include <stdio.h>
#include <stdlib.h>

// Pure business logic - just addition
int add_numbers(int a, int b) {
    return a + b;
}

// Endpoint handler for addition
EndpointResponse* handle_add(const RequestContext* request) {
    const char* a_str = request_get_param(request, "a");
    const char* b_str = request_get_param(request, "b");
    
    if (!a_str || !b_str) {
        return response_error(400, "Missing parameters 'a' and 'b'");
    }
    
    int a = atoi(a_str);
    int b = atoi(b_str);
    int result = add_numbers(a, b);
    
    char response[256];
    snprintf(response, sizeof(response), 
        "{\"operation\": \"add\", \"a\": %d, \"b\": %d, \"result\": %d}", 
        a, b, result);
    
    return response_json(200, response);
}

int main() {
    printf("Starting Simple Calculator (Addition Only)...\n");
    
    // Initialize server
    if (server_init(8080) != 0) {
        printf("Failed to initialize server\n");
        return 1;
    }
    
    // Register endpoints
    SERVER_GET("/add", handle_add);
    server_register_simple("/health", "GET", "{\"status\": \"ok\"}", "application/json");
    
    printf("Calculator ready! Try:\n");
    printf("  curl \"http://localhost:8080/add?a=5&b=3\"\n");
    printf("  curl \"http://localhost:8080/health\"\n");
    
    // Start serving
    server_start();
    
    return 0;
}
