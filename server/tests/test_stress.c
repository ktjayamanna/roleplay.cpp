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
#include <time.h>

#define TEST_PORT 9997
#define TEST_HOST "127.0.0.1"
#define NUM_CONCURRENT_CLIENTS 20
#define REQUESTS_PER_CLIENT 50

static pthread_t server_thread;
static int successful_requests = 0;
static int failed_requests = 0;
static pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

// Helper: Send HTTP request
static int send_request_and_verify(const char* request, const char* expected) {
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
    
    char buffer[65536];
    ssize_t n = read(sock, buffer, sizeof(buffer) - 1);
    buffer[n > 0 ? n : 0] = '\0';
    
    close(sock);
    
    if (n > 0 && (expected == NULL || strstr(buffer, expected))) {
        return 0;
    }
    return -1;
}

// Test handlers
static EndpointResponse* handle_fast(const RequestContext* req) {
    return response_json(200, "{\"status\":\"ok\"}");
}

static EndpointResponse* handle_slow(const RequestContext* req) {
    // Simulate slow processing
    usleep(10000); // 10ms
    return response_json(200, "{\"status\":\"slow\"}");
}

static EndpointResponse* handle_compute(const RequestContext* req) {
    // Simulate CPU-intensive work
    int sum = 0;
    for (int i = 0; i < 100000; i++) {
        sum += i;
    }
    
    char response[128];
    snprintf(response, sizeof(response), "{\"result\":%d}", sum);
    return response_json(200, response);
}

static EndpointResponse* handle_large_payload(const RequestContext* req) {
    // Return large payload
    size_t size = 10000;
    char* data = malloc(size);
    memset(data, 'A', size - 1);
    data[size - 1] = '\0';
    
    EndpointResponse* resp = malloc(sizeof(EndpointResponse));
    resp->status_code = 200;
    resp->body = data;
    resp->body_length = size - 1;
    resp->content_type = strdup("text/plain");
    return resp;
}

static EndpointResponse* handle_post_large(const RequestContext* req) {
    const char* body = request_get_body(req);
    int body_len = req->body_length;
    
    char response[128];
    snprintf(response, sizeof(response), "{\"received\":%d}", body_len);
    return response_json(200, response);
}

// Server thread
static void* server_thread_func(void* arg) {
    server_start();
    return NULL;
}

// Client thread for concurrent testing
typedef struct {
    int client_id;
    int num_requests;
} ClientThreadArg;

static void* client_thread_func(void* arg) {
    ClientThreadArg* carg = (ClientThreadArg*)arg;
    int local_success = 0;
    int local_fail = 0;
    
    for (int i = 0; i < carg->num_requests; i++) {
        const char* request = "GET /fast HTTP/1.1\r\nHost: localhost\r\n\r\n";
        
        if (send_request_and_verify(request, "200 OK") == 0) {
            local_success++;
        } else {
            local_fail++;
        }
        
        // Small random delay
        usleep(rand() % 1000);
    }
    
    pthread_mutex_lock(&stats_mutex);
    successful_requests += local_success;
    failed_requests += local_fail;
    pthread_mutex_unlock(&stats_mutex);
    
    free(carg);
    return NULL;
}

// Stress tests
static void test_concurrent_connections() {
    printf("TEST: Concurrent connections (%d clients, %d requests each)... ", 
           NUM_CONCURRENT_CLIENTS, REQUESTS_PER_CLIENT);
    
    successful_requests = 0;
    failed_requests = 0;
    
    pthread_t clients[NUM_CONCURRENT_CLIENTS];
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Spawn client threads
    for (int i = 0; i < NUM_CONCURRENT_CLIENTS; i++) {
        ClientThreadArg* arg = malloc(sizeof(ClientThreadArg));
        arg->client_id = i;
        arg->num_requests = REQUESTS_PER_CLIENT;
        pthread_create(&clients[i], NULL, client_thread_func, arg);
    }
    
    // Wait for all clients
    for (int i = 0; i < NUM_CONCURRENT_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("DONE\n");
    printf("  Success: %d, Failed: %d, Time: %.2fs\n", 
           successful_requests, failed_requests, elapsed);
    printf("  Throughput: %.2f req/s\n", 
           (successful_requests + failed_requests) / elapsed);
}

static void test_rapid_sequential() {
    printf("TEST: Rapid sequential requests (1000 requests)... ");
    
    int success = 0;
    int fail = 0;
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < 1000; i++) {
        const char* request = "GET /fast HTTP/1.1\r\nHost: localhost\r\n\r\n";
        if (send_request_and_verify(request, "200 OK") == 0) {
            success++;
        } else {
            fail++;
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("DONE\n");
    printf("  Success: %d, Failed: %d, Time: %.2fs\n", success, fail, elapsed);
}

static void test_large_payloads() {
    printf("TEST: Large payload responses (100 requests)... ");
    
    int success = 0;
    for (int i = 0; i < 100; i++) {
        const char* request = "GET /large HTTP/1.1\r\nHost: localhost\r\n\r\n";
        if (send_request_and_verify(request, "200 OK") == 0) {
            success++;
        }
    }
    
    printf("DONE (Success: %d/100)\n", success);
}

static void test_large_post_bodies() {
    printf("TEST: Large POST bodies (50 requests)... ");
    
    // Create large POST body
    size_t body_size = 50000;
    char* body = malloc(body_size);
    memset(body, 'X', body_size - 1);
    body[body_size - 1] = '\0';
    
    char* request = malloc(body_size + 512);
    snprintf(request, body_size + 512,
        "POST /post_large HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "\r\n%s", body_size - 1, body);
    
    int success = 0;
    for (int i = 0; i < 50; i++) {
        if (send_request_and_verify(request, "200 OK") == 0) {
            success++;
        }
    }
    
    free(body);
    free(request);
    
    printf("DONE (Success: %d/50)\n", success);
}

static void test_endpoint_limit() {
    printf("TEST: Endpoint registration limit... ");
    
    // Try to register many endpoints
    int registered = 0;
    for (int i = 0; i < 150; i++) {
        char path[64];
        snprintf(path, sizeof(path), "/endpoint_%d", i);
        
        if (server_register_handler(path, "GET", handle_fast) == 0) {
            registered++;
        } else {
            break;
        }
    }
    
    printf("DONE (Registered: %d endpoints)\n", registered);
}

static void test_mixed_load() {
    printf("TEST: Mixed workload (fast/slow/compute, 500 requests)... ");
    
    const char* requests[] = {
        "GET /fast HTTP/1.1\r\nHost: localhost\r\n\r\n",
        "GET /slow HTTP/1.1\r\nHost: localhost\r\n\r\n",
        "GET /compute HTTP/1.1\r\nHost: localhost\r\n\r\n",
    };
    
    int success = 0;
    for (int i = 0; i < 500; i++) {
        if (send_request_and_verify(requests[i % 3], "200 OK") == 0) {
            success++;
        }
    }
    
    printf("DONE (Success: %d/500)\n", success);
}

int main() {
    printf("=== Stress Tests ===\n\n");
    
    srand(time(NULL));
    
    // Initialize server
    if (server_init(TEST_PORT) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }
    
    // Register test endpoints
    SERVER_GET("/fast", handle_fast);
    SERVER_GET("/slow", handle_slow);
    SERVER_GET("/compute", handle_compute);
    SERVER_GET("/large", handle_large_payload);
    SERVER_POST("/post_large", handle_post_large);
    
    // Start server in background thread
    pthread_create(&server_thread, NULL, server_thread_func, NULL);
    sleep(1); // Give server time to start
    
    // Run stress tests
    test_rapid_sequential();
    test_concurrent_connections();
    test_large_payloads();
    test_large_post_bodies();
    test_mixed_load();
    test_endpoint_limit();
    
    printf("\n=== All stress tests completed ===\n");
    
    // Cleanup
    server_stop();
    
    return 0;
}

