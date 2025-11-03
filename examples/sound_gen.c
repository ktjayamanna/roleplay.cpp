#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../server/server.h"
#include "../server/endpoint.h"

EndpointResponse* serve_music(const RequestContext* request) {
    return endpoint_file_response(200, "music.mp3");
}

EndpointResponse* serve_json(const RequestContext* request) {
    return endpoint_json_response(200, "{\"message\": \"Hello, World!\"}");
}

int main() {
    int port = 8888;
    if (server_init(port) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }
    server_register_handler("/ezio_family", "GET", serve_music);
    server_register_handler("/api/hello", "GET", serve_json);
    printf("Server running on http://localhost:%d\n", port);
    printf("  - Binary: http://localhost:%d/ezio_family\n", port);
    printf("  - JSON:   http://localhost:%d/api/hello\n", port);
    server_start();
    return 0;
}
