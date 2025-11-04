#define _GNU_SOURCE
#include "../server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define TEST_PORT 9998
#define TEST_HOST "127.0.0.1"

static pthread_t server_thread;
static int test_iterations = 100;

// Helper: Send HTTP request
static int send_simple_request(const char* request) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TEST_PORT);
    inet_pton(AF_INET, TEST_HOST, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }

    write(sock, request, strlen(request));
    
    char buffer[4096];
    read(sock, buffer, sizeof(buffer));
    
    close(sock);
    return 0;
}

// Test handlers that allocate memory
static EndpointResponse* handle_json_response(const RequestContext* req) {
    return response_json(200, "{\"status\":\"ok\"}");
}

static EndpointResponse* handle_text_response(const RequestContext* req) {
    return response_text(200, "Hello, World!");
}

static EndpointResponse* handle_error_response(const RequestContext* req) {
    return response_error(404, "Not found");
}

static EndpointResponse* handle_with_params(const RequestContext* req) {
    const char* name = request_get_param(req, "name");
    int age = request_get_param_int(req, "age", 0);
    
    char response[256];
    snprintf(response, sizeof(response), "{\"name\":\"%s\",\"age\":%d}", 
             name ? name : "unknown", age);
    return response_json(200, response);
}

static EndpointResponse* handle_post_body(const RequestContext* req) {
    const char* body = request_get_body(req);
    
    // Allocate and process body
    if (body) {
        char* processed = malloc(strlen(body) + 100);
        snprintf(processed, strlen(body) + 100, "{\"received\":\"%s\"}", body);
        EndpointResponse* resp = response_json(200, processed);
        free(processed);
        return resp;
    }
    
    return response_json(200, "{\"received\":null}");
}

static EndpointResponse* handle_binary_data(const RequestContext* req) {
    // Allocate binary data
    size_t size = 1024;
    unsigned char* data = malloc(size);
    for (size_t i = 0; i < size; i++) {
        data[i] = i % 256;
    }
    
    EndpointResponse* resp = malloc(sizeof(EndpointResponse));
    resp->status_code = 200;
    resp->body = data;
    resp->body_length = size;
    resp->content_type = strdup("application/octet-stream");
    return resp;
}

static EndpointResponse* handle_large_allocation(const RequestContext* req) {
    // Test large memory allocation
    size_t size = 50000;
    char* large = malloc(size);
    memset(large, 'X', size - 1);
    large[size - 1] = '\0';
    
    EndpointResponse* resp = malloc(sizeof(EndpointResponse));
    resp->status_code = 200;
    resp->body = large;
    resp->body_length = size - 1;
    resp->content_type = strdup("text/plain");
    return resp;
}

// Server thread
static void* server_thread_func(void* arg) {
    server_start();
    return NULL;
}

// Memory leak tests
static void test_repeated_json_responses() {
    printf("TEST: Repeated JSON responses (%d iterations)... ", test_iterations);
    
    for (int i = 0; i < test_iterations; i++) {
        const char* request = "GET /json HTTP/1.1\r\nHost: localhost\r\n\r\n";
        send_simple_request(request);
    }
    
    printf("DONE\n");
}

static void test_repeated_text_responses() {
    printf("TEST: Repeated text responses (%d iterations)... ", test_iterations);
    
    for (int i = 0; i < test_iterations; i++) {
        const char* request = "GET /text HTTP/1.1\r\nHost: localhost\r\n\r\n";
        send_simple_request(request);
    }
    
    printf("DONE\n");
}

static void test_repeated_error_responses() {
    printf("TEST: Repeated error responses (%d iterations)... ", test_iterations);
    
    for (int i = 0; i < test_iterations; i++) {
        const char* request = "GET /error HTTP/1.1\r\nHost: localhost\r\n\r\n";
        send_simple_request(request);
    }
    
    printf("DONE\n");
}

static void test_repeated_params() {
    printf("TEST: Repeated requests with params (%d iterations)... ", test_iterations);
    
    for (int i = 0; i < test_iterations; i++) {
        const char* request = "GET /params?name=test&age=25 HTTP/1.1\r\nHost: localhost\r\n\r\n";
        send_simple_request(request);
    }
    
    printf("DONE\n");
}

static void test_repeated_post_body() {
    printf("TEST: Repeated POST with body (%d iterations)... ", test_iterations);
    
    for (int i = 0; i < test_iterations; i++) {
        const char* request = 
            "POST /post HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 20\r\n"
            "\r\n"
            "{\"test\":\"message\"}";
        send_simple_request(request);
    }
    
    printf("DONE\n");
}

static void test_repeated_binary() {
    printf("TEST: Repeated binary responses (%d iterations)... ", test_iterations);
    
    for (int i = 0; i < test_iterations; i++) {
        const char* request = "GET /binary HTTP/1.1\r\nHost: localhost\r\n\r\n";
        send_simple_request(request);
    }
    
    printf("DONE\n");
}

static void test_repeated_large_allocation() {
    printf("TEST: Repeated large allocations (%d iterations)... ", test_iterations);
    
    for (int i = 0; i < test_iterations; i++) {
        const char* request = "GET /large HTTP/1.1\r\nHost: localhost\r\n\r\n";
        send_simple_request(request);
    }
    
    printf("DONE\n");
}

static void test_repeated_404() {
    printf("TEST: Repeated 404 responses (%d iterations)... ", test_iterations);
    
    for (int i = 0; i < test_iterations; i++) {
        const char* request = "GET /nonexistent HTTP/1.1\r\nHost: localhost\r\n\r\n";
        send_simple_request(request);
    }
    
    printf("DONE\n");
}

static void test_mixed_requests() {
    printf("TEST: Mixed request types (%d iterations)... ", test_iterations);
    
    const char* requests[] = {
        "GET /json HTTP/1.1\r\nHost: localhost\r\n\r\n",
        "GET /text HTTP/1.1\r\nHost: localhost\r\n\r\n",
        "GET /params?name=test&age=30 HTTP/1.1\r\nHost: localhost\r\n\r\n",
        "POST /post HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\n{\"a\":\"b\"}",
        "GET /binary HTTP/1.1\r\nHost: localhost\r\n\r\n",
    };
    
    for (int i = 0; i < test_iterations; i++) {
        send_simple_request(requests[i % 5]);
    }
    
    printf("DONE\n");
}

int main(int argc, char** argv) {
    printf("=== Memory Leak Tests ===\n");
    printf("Run with: valgrind --leak-check=full --show-leak-kinds=all ./test_memory_leaks\n\n");
    
    if (argc > 1) {
        test_iterations = atoi(argv[1]);
        printf("Using %d iterations\n\n", test_iterations);
    }
    
    // Initialize server
    if (server_init(TEST_PORT) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }
    
    // Register test endpoints
    SERVER_GET("/json", handle_json_response);
    SERVER_GET("/text", handle_text_response);
    SERVER_GET("/error", handle_error_response);
    SERVER_GET("/params", handle_with_params);
    SERVER_POST("/post", handle_post_body);
    SERVER_GET("/binary", handle_binary_data);
    SERVER_GET("/large", handle_large_allocation);
    
    // Start server in background thread
    pthread_create(&server_thread, NULL, server_thread_func, NULL);
    sleep(1); // Give server time to start
    
    // Run memory leak tests
    test_repeated_json_responses();
    test_repeated_text_responses();
    test_repeated_error_responses();
    test_repeated_params();
    test_repeated_post_body();
    test_repeated_binary();
    test_repeated_large_allocation();
    test_repeated_404();
    test_mixed_requests();
    
    printf("\n=== All tests completed ===\n");
    printf("Check valgrind output for memory leaks\n");
    
    // Cleanup
    server_stop();
    
    return 0;
}

