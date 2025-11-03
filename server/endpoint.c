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

    // TODO: PHASE 1 - Set body_length for text responses
    // Add this line after setting content_type:
    //   response->body_length = body ? strlen(body) : 0;
    //
    // NOTE: This function is for TEXT responses (uses strdup/strlen)
    // For binary data, use endpoint_binary_response() instead

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

// TODO: PHASE 1 - Implement endpoint_binary_response()
// This function creates a response containing binary data (or any raw data with known length)
//
// EndpointResponse* endpoint_binary_response(int status_code, const void* data,
//                                           size_t data_length, const char* content_type) {
//     // STEP 1: Allocate EndpointResponse structure
//     //   EndpointResponse* response = malloc(sizeof(EndpointResponse));
//     //   if (!response) return NULL;
//
//     // STEP 2: Set status code
//     //   response->status_code = status_code;
//
//     // STEP 3: Allocate and copy data (works for both binary and text)
//     //   response->body = malloc(data_length);
//     //   if (!response->body) { free(response); return NULL; }
//     //   memcpy(response->body, data, data_length);  // memcpy works for everything!
//
//     // STEP 4: Set body length
//     //   response->body_length = data_length;
//
//     // STEP 5: Set content type
//     //   response->content_type = strdup(content_type);
//
//     // STEP 6: Return the response
//     //   return response;
// }

// TODO: PHASE 1 - Implement endpoint_file_response()
// This function reads a file from disk and creates a binary response
//
// EndpointResponse* endpoint_file_response(int status_code, const char* file_path) {
//     // STEP 1: Open file in binary mode
//     //   FILE* file = fopen(file_path, "rb");
//     //   if (!file) {
//     //       fprintf(stderr, "Error: Could not open file %s\n", file_path);
//     //       return endpoint_error_response(404, "File not found");
//     //   }
//
//     // STEP 2: Get file size
//     //   fseek(file, 0, SEEK_END);
//     //   long file_size = ftell(file);
//     //   fseek(file, 0, SEEK_SET);  // Rewind to beginning
//
//     // STEP 3: Allocate buffer and read file
//     //   char* file_data = malloc(file_size);
//     //   if (!file_data) { fclose(file); return NULL; }
//     //   size_t bytes_read = fread(file_data, 1, file_size, file);
//     //   fclose(file);
//
//     // STEP 4: Detect content type from file extension
//     //   const char* content_type = "application/octet-stream";  // Default
//     //   const char* ext = strrchr(file_path, '.');  // Find last '.'
//     //   if (ext) {
//     //       if (strcmp(ext, ".mp3") == 0) content_type = "audio/mpeg";
//     //       else if (strcmp(ext, ".png") == 0) content_type = "image/png";
//     //       else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) content_type = "image/jpeg";
//     //       else if (strcmp(ext, ".gif") == 0) content_type = "image/gif";
//     //       else if (strcmp(ext, ".pdf") == 0) content_type = "application/pdf";
//     //       else if (strcmp(ext, ".txt") == 0) content_type = "text/plain";
//     //       else if (strcmp(ext, ".html") == 0) content_type = "text/html";
//     //   }
//
//     // STEP 5: Create binary response using endpoint_binary_response()
//     //   EndpointResponse* response = endpoint_binary_response(status_code, file_data, bytes_read, content_type);
//     //   free(file_data);  // endpoint_binary_response makes a copy
//     //   return response;
// }
