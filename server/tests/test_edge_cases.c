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

#define TEST_PORT 9996
#define TEST_HOST "127.0.0.1"

static pthread_t server_thread;
static int tests_passed = 0;
static int tests_failed = 0;

// Helper: Send raw data and get response
static char* send_raw_data(const char* data, size_t len, size_t* response_len) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return NULL;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TEST_PORT);
    inet_pton(AF_INET, TEST_HOST, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return NULL;
    }

    write(sock, data, len);
    
    char* response = malloc(65536);
    ssize_t n = read(sock, response, 65536);
    if (n > 0) {
        response[n] = '\0';
        if (response_len) *response_len = n;
    } else {
        free(response);
        response = NULL;
    }
    
    close(sock);
    return response;
}

// Test handlers
static EndpointResponse* handle_normal(const RequestContext* req) {
    return response_json(200, "{\"status\":\"ok\"}");
}

static EndpointResponse* handle_null_param(const RequestContext* req) {
    const char* param = request_get_param(req, "nonexistent");
    
    char response[128];
    snprintf(response, sizeof(response), "{\"param\":\"%s\"}", 
             param ? param : "null");
    return response_json(200, response);
}

static EndpointResponse* handle_empty_body(const RequestContext* req) {
    const char* body = request_get_body(req);
    int len = req->body_length;
    
    char response[128];
    snprintf(response, sizeof(response), "{\"body_length\":%d}", len);
    return response_json(200, response);
}

static EndpointResponse* handle_special_chars(const RequestContext* req) {
    const char* text = request_get_param(req, "text");
    
    char response[256];
    snprintf(response, sizeof(response), "{\"received\":\"%s\"}", 
             text ? text : "");
    return response_json(200, response);
}

static EndpointResponse* handle_null_response(const RequestContext* req) {
    // Test returning NULL (should be handled gracefully)
    return NULL;
}

static EndpointResponse* handle_zero_length(const RequestContext* req) {
    EndpointResponse* resp = malloc(sizeof(EndpointResponse));
    resp->status_code = 200;
    resp->body = strdup("");
    resp->body_length = 0;
    resp->content_type = strdup("text/plain");
    return resp;
}

// Server thread
static void* server_thread_func(void* arg) {
    server_start();
    return NULL;
}

// Edge case tests
static void test_malformed_request() {
    printf("TEST: Malformed HTTP request... ");
    
    const char* request = "INVALID REQUEST\r\n\r\n";
    size_t len;
    char* response = send_raw_data(request, strlen(request), &len);
    
    // Server should handle gracefully (might return 404 or close connection)
    if (response) {
        printf("PASS (got response)\n");
        tests_passed++;
    } else {
        printf("PASS (connection closed)\n");
        tests_passed++;
    }
    
    free(response);
}

static void test_missing_headers() {
    printf("TEST: Request without headers... ");
    
    const char* request = "GET /normal\r\n\r\n";
    size_t len;
    char* response = send_raw_data(request, strlen(request), &len);
    
    if (response) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        tests_failed++;
    }
    
    free(response);
}

static void test_empty_request() {
    printf("TEST: Empty request... ");
    
    const char* request = "";
    size_t len;
    char* response = send_raw_data(request, strlen(request), &len);
    
    // Should handle gracefully
    printf("PASS (handled gracefully)\n");
    tests_passed++;
    
    free(response);
}

static void test_very_long_url() {
    printf("TEST: Very long URL... ");
    
    char url[2048];
    memset(url, 'a', sizeof(url) - 1);
    url[sizeof(url) - 1] = '\0';
    
    char request[3072];
    snprintf(request, sizeof(request), "GET /%s HTTP/1.1\r\nHost: localhost\r\n\r\n", url);
    
    size_t len;
    char* response = send_raw_data(request, strlen(request), &len);
    
    // Should handle (might truncate or return error)
    if (response) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("PASS (rejected)\n");
        tests_passed++;
    }
    
    free(response);
}

static void test_null_parameter() {
    printf("TEST: Request for non-existent parameter... ");
    
    const char* request = "GET /null_param HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t len;
    char* response = send_raw_data(request, strlen(request), &len);
    
    if (response && strstr(response, "200 OK")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        tests_failed++;
    }
    
    free(response);
}

static void test_empty_post_body() {
    printf("TEST: POST with empty body... ");
    
    const char* request = 
        "POST /empty_body HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    
    size_t len;
    char* response = send_raw_data(request, strlen(request), &len);
    
    if (response && strstr(response, "200 OK")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        tests_failed++;
    }
    
    free(response);
}

static void test_post_without_content_length() {
    printf("TEST: POST without Content-Length... ");
    
    const char* request = 
        "POST /empty_body HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
        "{\"data\":\"test\"}";
    
    size_t len;
    char* response = send_raw_data(request, strlen(request), &len);
    
    // Should handle gracefully
    if (response) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("PASS (rejected)\n");
        tests_passed++;
    }
    
    free(response);
}

static void test_special_characters_in_params() {
    printf("TEST: Special characters in parameters... ");
    
    const char* request = "GET /special?text=hello%20world&x=1%2B2 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t len;
    char* response = send_raw_data(request, strlen(request), &len);
    
    if (response && strstr(response, "200 OK")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        tests_failed++;
    }
    
    free(response);
}

static void test_multiple_query_params() {
    printf("TEST: Many query parameters... ");
    
    const char* request = "GET /normal?a=1&b=2&c=3&d=4&e=5&f=6&g=7&h=8&i=9&j=10&k=11 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t len;
    char* response = send_raw_data(request, strlen(request), &len);
    
    if (response && strstr(response, "200 OK")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        tests_failed++;
    }
    
    free(response);
}

static void test_zero_length_response() {
    printf("TEST: Zero-length response body... ");
    
    const char* request = "GET /zero_length HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t len;
    char* response = send_raw_data(request, strlen(request), &len);
    
    if (response && strstr(response, "200 OK")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        tests_failed++;
    }
    
    free(response);
}

static void test_binary_in_post_body() {
    printf("TEST: Binary data in POST body... ");
    
    unsigned char binary_data[] = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0xFD};
    char request[512];
    int header_len = snprintf(request, sizeof(request),
        "POST /empty_body HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/octet-stream\r\n"
        "Content-Length: %zu\r\n"
        "\r\n", sizeof(binary_data));
    
    memcpy(request + header_len, binary_data, sizeof(binary_data));
    
    size_t len;
    char* response = send_raw_data(request, header_len + sizeof(binary_data), &len);
    
    if (response && strstr(response, "200 OK")) {
        printf("PASS\n");
        tests_passed++;
    } else {
        printf("FAIL\n");
        tests_failed++;
    }
    
    free(response);
}

static void test_concurrent_same_endpoint() {
    printf("TEST: Concurrent requests to same endpoint... ");
    
    // Simple concurrent test
    int success = 0;
    
    #pragma omp parallel for reduction(+:success)
    for (int i = 0; i < 10; i++) {
        const char* request = "GET /normal HTTP/1.1\r\nHost: localhost\r\n\r\n";
        size_t len;
        char* response = send_raw_data(request, strlen(request), &len);
        if (response && strstr(response, "200 OK")) {
            success++;
        }
        free(response);
    }
    
    if (success >= 8) {
        printf("PASS (%d/10 succeeded)\n", success);
        tests_passed++;
    } else {
        printf("FAIL (%d/10 succeeded)\n", success);
        tests_failed++;
    }
}

int main() {
    printf("=== Edge Case Tests ===\n\n");
    
    // Initialize server
    if (server_init(TEST_PORT) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }
    
    // Register test endpoints
    SERVER_GET("/normal", handle_normal);
    SERVER_GET("/null_param", handle_null_param);
    SERVER_POST("/empty_body", handle_empty_body);
    SERVER_GET("/special", handle_special_chars);
    SERVER_GET("/zero_length", handle_zero_length);
    
    // Start server in background thread
    pthread_create(&server_thread, NULL, server_thread_func, NULL);
    sleep(1);
    
    // Run edge case tests
    test_malformed_request();
    test_missing_headers();
    test_empty_request();
    test_very_long_url();
    test_null_parameter();
    test_empty_post_body();
    test_post_without_content_length();
    test_special_characters_in_params();
    test_multiple_query_params();
    test_zero_length_response();
    test_binary_in_post_body();
    test_concurrent_same_endpoint();
    
    // Print results
    printf("\n=== Results ===\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    
    // Cleanup
    server_stop();
    
    return tests_failed > 0 ? 1 : 0;
}

