/*
 * endpoint.c - Modular endpoint system implementation
 */

#define _GNU_SOURCE  // For strdup
#include "endpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global endpoint registry
RegisteredEndpoint endpoint_registry[MAX_ENDPOINTS];
int endpoint_count = 0;

// Initialize the endpoint system
void endpoint_system_init(void) {
    endpoint_count = 0;
    for (int i = 0; i < MAX_ENDPOINTS; i++) {
        endpoint_registry[i].is_active = 0;
    }
}

// Register a new endpoint
int endpoint_register(const char* path, HttpMethod method, EndpointHandler handler) {
    if (endpoint_count >= MAX_ENDPOINTS) {
        fprintf(stderr, "Error: Maximum number of endpoints reached\n");
        return -1;
    }
    
    // Find an empty slot
    int slot = -1;
    for (int i = 0; i < MAX_ENDPOINTS; i++) {
        if (!endpoint_registry[i].is_active) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        fprintf(stderr, "Error: No available endpoint slots\n");
        return -1;
    }
    
    // Register the endpoint
    strncpy(endpoint_registry[slot].path, path, MAX_PATH_LENGTH - 1);
    endpoint_registry[slot].path[MAX_PATH_LENGTH - 1] = '\0';
    endpoint_registry[slot].method = method;
    endpoint_registry[slot].handler = handler;
    endpoint_registry[slot].is_active = 1;
    
    endpoint_count++;
    printf("Registered endpoint: %s\n", path);
    return 0;
}

// Convert method string to enum
static HttpMethod parse_method(const char* method_str) {
    if (strcmp(method_str, "GET") == 0) return HTTP_METHOD_GET;
    if (strcmp(method_str, "POST") == 0) return HTTP_METHOD_POST;
    if (strcmp(method_str, "PUT") == 0) return HTTP_METHOD_PUT;
    if (strcmp(method_str, "DELETE") == 0) return HTTP_METHOD_DELETE;
    return HTTP_METHOD_GET; // Default
}

// Parse query string into parameters
static void parse_query_string(const char* query_string, RequestContext* context) {
    if (!query_string || strlen(query_string) == 0) {
        context->param_count = 0;
        return;
    }
    
    context->param_count = 0;
    char* query_copy = strdup(query_string);
    char* pair = strtok(query_copy, "&");
    
    while (pair && context->param_count < MAX_PARAMS) {
        char* equals = strchr(pair, '=');
        if (equals) {
            *equals = '\0';
            strncpy(context->params[context->param_count].name, pair, MAX_PARAM_LENGTH - 1);
            strncpy(context->params[context->param_count].value, equals + 1, MAX_PARAM_LENGTH - 1);
            context->params[context->param_count].name[MAX_PARAM_LENGTH - 1] = '\0';
            context->params[context->param_count].value[MAX_PARAM_LENGTH - 1] = '\0';
            context->param_count++;
        }
        pair = strtok(NULL, "&");
    }
    
    free(query_copy);
}

// Dispatch request to appropriate endpoint
EndpointResponse* endpoint_dispatch(const char* method_str, const char* path, const char* query_string, const char* body, int body_length) {
    return endpoint_dispatch_with_body(method_str, path, query_string, "", body, body_length);
}

// Dispatch request with content type to appropriate endpoint
EndpointResponse* endpoint_dispatch_with_body(const char* method_str, const char* path, const char* query_string, const char* content_type, const char* body, int body_length) {
    HttpMethod method = parse_method(method_str);

    // Find matching endpoint
    for (int i = 0; i < MAX_ENDPOINTS; i++) {
        if (endpoint_registry[i].is_active &&
            endpoint_registry[i].method == method &&
            strcmp(endpoint_registry[i].path, path) == 0) {

            // Build request context
            RequestContext context;
            context.method = method;
            strncpy(context.path, path, MAX_PATH_LENGTH - 1);
            context.path[MAX_PATH_LENGTH - 1] = '\0';
            context.body = (char*)body;
            context.body_length = body_length;
            strncpy(context.content_type, content_type ? content_type : "", sizeof(context.content_type) - 1);
            context.content_type[sizeof(context.content_type) - 1] = '\0';

            // Parse query parameters
            parse_query_string(query_string, &context);

            // Call the handler
            return endpoint_registry[i].handler(&context);
        }
    }

    // No matching endpoint found
    return endpoint_error_response(404, "Endpoint not found");
}

// Free endpoint response memory
void endpoint_response_free(EndpointResponse* response) {
    if (response) {
        if (response->body) {
            free(response->body);
        }
        if (response->content_type) {
            free(response->content_type);
        }
        free(response);
    }
}

// Create a new endpoint response
EndpointResponse* endpoint_create_response(int status_code, const char* body, const char* content_type) {
    EndpointResponse* response = malloc(sizeof(EndpointResponse));
    if (!response) return NULL;

    response->status_code = status_code;
    response->body = body ? strdup(body) : NULL;
    response->content_type = content_type ? strdup(content_type) : strdup("application/json");
    response->body_length = body ? strlen(body) : 0;


    return response;
}

// Get parameter value by name
const char* endpoint_get_param(const RequestContext* request, const char* param_name) {
    for (int i = 0; i < request->param_count; i++) {
        if (strcmp(request->params[i].name, param_name) == 0) {
            return request->params[i].value;
        }
    }
    return NULL;
}

// Get parameter as integer with default value
int endpoint_get_param_int(const RequestContext* request, const char* param_name, int default_value) {
    const char* value = endpoint_get_param(request, param_name);
    if (value) {
        return atoi(value);
    }
    return default_value;
}

// Helper for JSON responses
EndpointResponse* endpoint_json_response(int status_code, const char* json_body) {
    return endpoint_create_response(status_code, json_body, "application/json");
}

// Helper for error responses
EndpointResponse* endpoint_error_response(int status_code, const char* error_message) {
    char* json_body = malloc(256);
    snprintf(json_body, 256, "{\"error\": \"%s\"}", error_message);
    EndpointResponse* response = endpoint_create_response(status_code, json_body, "application/json");
    free(json_body); // endpoint_create_response makes a copy
    return response;
}

// This function creates a response containing binary data (or any raw data with known length)

EndpointResponse* endpoint_binary_response(int status_code, const void* data,
                                          size_t data_length, const char* content_type) {
    // STEP 1: Allocate EndpointResponse structure
      EndpointResponse* response = malloc(sizeof(EndpointResponse));
      if (!response) return NULL;

    // STEP 2: Set status code
      response->status_code = status_code;

    // STEP 3: Allocate and copy data (works for both binary and text)
      response->body = malloc(data_length);
      if (!response->body) { free(response); return NULL; }
      memcpy(response->body, data, data_length);  // memcpy works for everything!

    // STEP 4: Set body length
      response->body_length = data_length;

    // STEP 5: Set content type
      response->content_type = strdup(content_type);

    // STEP 6: Return the response
      return response;
}

// This function reads a file from disk and creates a binary response

EndpointResponse* endpoint_file_response(int status_code, const char* file_path) {
    // STEP 1: Open file in binary mode
      FILE* file = fopen(file_path, "rb");
      if (!file) {
          fprintf(stderr, "Error: Could not open file %s\n", file_path);
          return endpoint_error_response(404, "File not found");
      }

    // STEP 2: Get file size
      fseek(file, 0, SEEK_END);
      long file_size = ftell(file);
      fseek(file, 0, SEEK_SET);  // Rewind to beginning

    // STEP 3: Allocate buffer and read file
      char* file_data = malloc(file_size);
      if (!file_data) { fclose(file); return NULL; }
      size_t bytes_read = fread(file_data, 1, file_size, file);
      fclose(file);

    // STEP 4: Detect content type from file extension
      const char* content_type = "application/octet-stream";  // Default
      const char* ext = strrchr(file_path, '.');  // Find last '.'
      if (ext) {
          if (strcmp(ext, ".mp3") == 0) content_type = "audio/mpeg";
          else if (strcmp(ext, ".png") == 0) content_type = "image/png";
          else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) content_type = "image/jpeg";
          else if (strcmp(ext, ".gif") == 0) content_type = "image/gif";
          else if (strcmp(ext, ".pdf") == 0) content_type = "application/pdf";
          else if (strcmp(ext, ".txt") == 0) content_type = "text/plain";
          else if (strcmp(ext, ".html") == 0) content_type = "text/html";
      }

    // STEP 5: Create binary response using endpoint_binary_response()
      EndpointResponse* response = endpoint_binary_response(status_code, file_data, bytes_read, content_type);
      free(file_data);  // endpoint_binary_response makes a copy
      return response;
}

// Parse multipart/form-data file upload
int parse_multipart_file(const RequestContext* request, UploadedFile* file) {
    if (!request || !file || !request->body || request->body_length == 0) {
        return -1;
    }

    // Extract boundary from Content-Type header
    // Format: "multipart/form-data; boundary=----WebKitFormBoundary..."
    const char* boundary_start = strstr(request->content_type, "boundary=");
    if (!boundary_start) {
        return -1;
    }
    boundary_start += 9; // Skip "boundary="

    char boundary[256];
    snprintf(boundary, sizeof(boundary), "--%s", boundary_start);

    // Find first boundary
    const char* part_start = strstr(request->body, boundary);
    if (!part_start) {
        return -1;
    }
    part_start += strlen(boundary);

    // Skip to headers
    if (*part_start == '\r') part_start++;
    if (*part_start == '\n') part_start++;

    // Parse Content-Disposition header to get filename
    const char* disposition = strstr(part_start, "Content-Disposition:");
    if (!disposition) {
        return -1;
    }

    const char* filename_start = strstr(disposition, "filename=\"");
    if (filename_start) {
        filename_start += 10; // Skip 'filename="'
        const char* filename_end = strchr(filename_start, '"');
        if (filename_end) {
            size_t filename_len = filename_end - filename_start;
            if (filename_len >= sizeof(file->filename)) {
                filename_len = sizeof(file->filename) - 1;
            }
            strncpy(file->filename, filename_start, filename_len);
            file->filename[filename_len] = '\0';
        }
    }

    // Parse Content-Type header for the file
    const char* file_content_type = strstr(part_start, "Content-Type:");
    if (file_content_type) {
        file_content_type += 13; // Skip "Content-Type:"
        while (*file_content_type == ' ') file_content_type++;

        const char* ct_end = strstr(file_content_type, "\r\n");
        if (ct_end) {
            size_t ct_len = ct_end - file_content_type;
            if (ct_len >= sizeof(file->content_type)) {
                ct_len = sizeof(file->content_type) - 1;
            }
            strncpy(file->content_type, file_content_type, ct_len);
            file->content_type[ct_len] = '\0';
        }
    }

    // Find the blank line that separates headers from body
    const char* file_data_start = strstr(part_start, "\r\n\r\n");
    if (!file_data_start) {
        return -1;
    }
    file_data_start += 4; // Skip \r\n\r\n

    // Find the ending boundary
    // Use memmem instead of strstr because file data may contain null bytes
    char end_boundary[256];
    snprintf(end_boundary, sizeof(end_boundary), "\r\n%s", boundary);

    size_t search_len = request->body_length - (file_data_start - request->body);
    const char* file_data_end = memmem(file_data_start, search_len, end_boundary, strlen(end_boundary));

    if (!file_data_end) {
        // Try without \r\n prefix
        file_data_end = memmem(file_data_start, search_len, boundary, strlen(boundary));
        if (!file_data_end) {
            return -1;
        }
    }

    // Set file data and size
    file->data = file_data_start;
    file->size = file_data_end - file_data_start;

    return 0;
}
