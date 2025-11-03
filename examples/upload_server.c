#include "../server/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Endpoint handler for MP3 file upload
EndpointResponse* handle_upload(const RequestContext* request) {
    // Parse the uploaded file from multipart/form-data
    UploadedFile file;

    if (parse_multipart_file(request, &file) != 0) {
        return response_error(400, "Failed to parse file upload");
    }

    // Log the upload
    printf("Received file: %s (%zu bytes, %s)\n",
           file.filename, file.size, file.content_type);

    // Validate it's an MP3 file (check filename extension)
    const char* ext = strrchr(file.filename, '.');
    if (!ext || strcmp(ext, ".mp3") != 0) {
        return response_error(400, "Only MP3 files are accepted");
    }

    // Build response with file info
    char response[512];
    snprintf(response, sizeof(response),
        "{\"status\": \"success\", \"filename\": \"%s\", \"content_type\": \"%s\", \"size\": %zu}",
        file.filename, file.content_type, file.size);

    return response_json(200, response);
}

int main() {
    printf("Starting MP3 Upload Server...\n");
    
    // Initialize server
    if (server_init(8080) != 0) {
        printf("Failed to initialize server\n");
        return 1;
    }
    
    // Register upload endpoint
    SERVER_POST("/upload", handle_upload);
    
    printf("Upload server ready!\n");
    printf("Test with:\n");
    printf("  curl -X POST -F \"file=@music.mp3\" http://localhost:8080/upload\n");
    
    // Start serving
    server_start();
    
    return 0;
}

