#include "../server/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

EndpointResponse* handle_upload(const RequestContext* request) {
    UploadedFile file;

    if (parse_multipart_file(request, &file) != 0) {
        return response_error(400, "Failed to parse file upload");
    }

    printf("Received file: %s (%zu bytes, %s)\n",
           file.filename, file.size, file.content_type);

    const char* ext = strrchr(file.filename, '.');
    if (!ext || strcmp(ext, ".mp3") != 0) {
        return response_error(400, "Only MP3 files are accepted");
    }

    printf("\n=== MP3 Buffer Ready ===\n");
    printf("Buffer: %p, Size: %zu bytes (%.2f MB)\n",
           (void*)file.data, file.size, file.size / (1024.0 * 1024.0));
    printf("First 16 bytes: ");
    for (int i = 0; i < 16 && i < (int)file.size; i++) {
        printf("%02X ", (unsigned char)file.data[i]);
    }
    printf("\n");
    printf("Usage: fwrite(file.data, 1, file.size, fp) to save to disk\n");
    printf("========================\n\n");

    char response[512];
    snprintf(response, sizeof(response),
        "{\"status\": \"success\", \"filename\": \"%s\", \"content_type\": \"%s\", \"size\": %zu, \"buffer_ready\": true}",
        file.filename, file.content_type, file.size);

    return response_json(200, response);
}

int main() {
    printf("Starting MP3 Upload Server...\n");

    if (server_init(8080) != 0) {
        printf("Failed to initialize server\n");
        return 1;
    }

    SERVER_POST("/upload", handle_upload);

    printf("Upload server ready!\n");
    printf("Test with:\n");
    printf("  curl -X POST -F \"file=@music.mp3\" http://localhost:8080/upload\n");

    server_start();

    return 0;
}

