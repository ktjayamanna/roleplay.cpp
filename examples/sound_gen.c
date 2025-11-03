#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../server/server.h"
#include "../server/endpoint.h"

EndpointResponse* serve_music(const RequestContext* request) {
    return endpoint_file_response(200, "music.mp3");
}

EndpointResponse* serve_json(const RequestContext* request) {
    // Test that text responses still work!
    return endpoint_json_response(200, "{\"message\": \"Hello, World!\"}");
}

int main() {
    if (server_init(8080) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }
    server_register_handler("/music.mp3", "GET", serve_music);
    server_register_handler("/api/hello", "GET", serve_json);
    printf("Server running on http://localhost:8080\n");
    printf("  - Binary: http://localhost:8080/music.mp3\n");
    printf("  - JSON:   http://localhost:8080/api/hello\n");
    server_start();
    return 0;
}
