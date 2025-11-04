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
#include <assert.h>

#define TEST_PORT 9999
#define TEST_HOST "127.0.0.1"

// Test state
static int tests_passed = 0;
static int tests_failed = 0;
static pthread_t server_thread;

// Helper: Send HTTP request and get response
static char* send_http_request(const char* request, size_t* response_len) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return NULL;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TEST_PORT);
    inet_pton(AF_INET, TEST_HOST, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return NULL;
    }

    // Send request
    ssize_t sent = write(sock, request, strlen(request));
    if (sent < 0) {
        perror("write");
        close(sock);
        return NULL;
    }

    // Read response (allocate larger buffer for large responses)
    size_t buffer_size = 200000;
    char* response = malloc(buffer_size);
    ssize_t total = 0;
    ssize_t n;

    while ((n = read(sock, response + total, buffer_size - total - 1)) > 0) {
        total += n;
        // Check if we've read the full response
        if (total > 4 && strstr(response, "\r\n\r\n")) {
            // For responses with Content-Length, check if we got it all
            char* cl_header = strstr(response, "Content-Length: ");
            if (cl_header) {
                int content_length = atoi(cl_header + 16);
                char* body = strstr(response, "\r\n\r\n");
                if (body) {
                    body += 4;
                    int body_received = total - (body - response);
                    if (body_received >= content_length) {
                        break;
                    }
                }
            }
        }
    }
    
    response[total] = '\0';
    if (response_len) *response_len = total;
    
    close(sock);
    return response;
}

// Test handlers
static EndpointResponse* handle_get_hello(const RequestContext* req) {
    return response_json(200, "{\"message\":\"hello\"}");
}

static EndpointResponse* handle_get_with_params(const RequestContext* req) {
    const char* name = request_get_param(req, "name");
    int age = request_get_param_int(req, "age", 0);
    
    char response[256];
    snprintf(response, sizeof(response), "{\"name\":\"%s\",\"age\":%d}", 
             name ? name : "unknown", age);
    return response_json(200, response);
}

static EndpointResponse* handle_post_echo(const RequestContext* req) {
    const char* body = request_get_body(req);
    char response[512];
    snprintf(response, sizeof(response), "{\"received\":\"%s\"}", body ? body : "");
    return response_json(200, response);
}

static EndpointResponse* handle_binary_data(const RequestContext* req) {
    // Return some binary data
    unsigned char data[] = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD};
    EndpointResponse* resp = malloc(sizeof(EndpointResponse));
    resp->status_code = 200;
    resp->body = malloc(sizeof(data));
    memcpy(resp->body, data, sizeof(data));
    resp->body_length = sizeof(data);
    resp->content_type = strdup("application/octet-stream");
    return resp;
}

static EndpointResponse* handle_large_response(const RequestContext* req) {
    // Generate a large response to test memory handling
    size_t size = 100000;
    char* large_data = malloc(size);
    memset(large_data, 'A', size - 1);
    large_data[size - 1] = '\0';
    
    EndpointResponse* resp = malloc(sizeof(EndpointResponse));
    resp->status_code = 200;
    resp->body = large_data;
    resp->body_length = size - 1;
    resp->content_type = strdup("text/plain");
    return resp;
}

// Server thread
static void* server_thread_func(void* arg) {
    server_start();
    return NULL;
}

// Test functions
static void test_get_request() {
    printf("TEST: GET request... ");
    
    const char* request = "GET /hello HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t len;
    char* response = send_http_request(request, &len);
    
    if (response && strstr(response, "200 OK") && strstr(response, "hello")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        if (response) printf("Response: %s\n", response);
        tests_failed++;
    }
    
    free(response);
}

static void test_get_with_query_params() {
    printf("TEST: GET with query parameters... ");
    
    const char* request = "GET /params?name=John&age=30 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t len;
    char* response = send_http_request(request, &len);
    
    if (response && strstr(response, "200 OK") && 
        strstr(response, "John") && strstr(response, "30")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        if (response) printf("Response: %s\n", response);
        tests_failed++;
    }
    
    free(response);
}

static void test_post_request() {
    printf("TEST: POST request with body... ");
    
    const char* request = 
        "POST /echo HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 15\r\n"
        "\r\n"
        "{\"test\":\"data\"}";
    
    size_t len;
    char* response = send_http_request(request, &len);
    
    if (response && strstr(response, "200 OK") && strstr(response, "received")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        if (response) printf("Response: %s\n", response);
        tests_failed++;
    }
    
    free(response);
}

static void test_binary_response() {
    printf("TEST: Binary response... ");
    
    const char* request = "GET /binary HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t len;
    char* response = send_http_request(request, &len);
    
    if (response && strstr(response, "200 OK") && 
        strstr(response, "application/octet-stream")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        tests_failed++;
    }
    
    free(response);
}

static void test_large_response() {
    printf("TEST: Large response (100KB)... ");
    
    const char* request = "GET /large HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t len;
    char* response = send_http_request(request, &len);
    
    if (response && strstr(response, "200 OK") && len > 100000) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL (received %zu bytes)\n", len);
        tests_failed++;
    }
    
    free(response);
}

static void test_404_not_found() {
    printf("TEST: 404 Not Found... ");
    
    const char* request = "GET /nonexistent HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t len;
    char* response = send_http_request(request, &len);
    
    if (response && strstr(response, "404")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        tests_failed++;
    }
    
    free(response);
}

static void test_multiple_requests() {
    printf("TEST: Multiple sequential requests... ");
    
    int success = 1;
    for (int i = 0; i < 10; i++) {
        const char* request = "GET /hello HTTP/1.1\r\nHost: localhost\r\n\r\n";
        size_t len;
        char* response = send_http_request(request, &len);
        
        if (!response || !strstr(response, "200 OK")) {
            success = 0;
            free(response);
            break;
        }
        free(response);
    }
    
    if (success) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        tests_failed++;
    }
}

int main() {
    printf("=== HTTP Endpoint Tests ===\n\n");
    
    // Initialize server
    if (server_init(TEST_PORT) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }
    
    // Register test endpoints
    SERVER_GET("/hello", handle_get_hello);
    SERVER_GET("/params", handle_get_with_params);
    SERVER_POST("/echo", handle_post_echo);
    SERVER_GET("/binary", handle_binary_data);
    SERVER_GET("/large", handle_large_response);
    
    // Start server in background thread
    pthread_create(&server_thread, NULL, server_thread_func, NULL);
    sleep(1); // Give server time to start
    
    // Run tests
    test_get_request();
    test_get_with_query_params();
    test_post_request();
    test_binary_response();
    test_large_response();
    test_404_not_found();
    test_multiple_requests();
    
    // Print results
    printf("\n=== Results ===\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    
    // Cleanup
    server_stop();
    
    return tests_failed > 0 ? 1 : 0;
}

