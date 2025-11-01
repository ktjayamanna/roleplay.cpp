/*
 * endpoint.h - Modular endpoint system header
 * 
 * This file defines a modular endpoint system that allows registering
 * HTTP endpoints without modifying the core server code.
 * 
 * DESIGN PRINCIPLES:
 * - Function pointers for handler registration
 * - Standardized request/response interface
 * - Easy parameter extraction from URLs and query strings
 * - Memory management handled by the framework
 */

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "http.h"
#include "server.h"  // Get the type definitions from server.h

// Maximum number of endpoints that can be registered
#define MAX_ENDPOINTS 50

// Structure to store registered endpoints
typedef struct {
    char path[MAX_PATH_LENGTH];
    HttpMethod method;
    EndpointHandler handler;
    int is_active;                // Flag to indicate if this slot is in use
} RegisteredEndpoint;

// Global endpoint registry
extern RegisteredEndpoint endpoint_registry[MAX_ENDPOINTS];
extern int endpoint_count;

// Core endpoint system functions
void endpoint_system_init(void);
int endpoint_register(const char* path, HttpMethod method, EndpointHandler handler);
EndpointResponse* endpoint_dispatch(const char* method_str, const char* path, const char* query_string, const char* body, int body_length);
void endpoint_response_free(EndpointResponse* response);

// Utility functions for handlers
EndpointResponse* endpoint_create_response(int status_code, const char* body, const char* content_type);
const char* endpoint_get_param(const RequestContext* request, const char* param_name);
int endpoint_get_param_int(const RequestContext* request, const char* param_name, int default_value);

// Helper functions for common response types
EndpointResponse* endpoint_json_response(int status_code, const char* json_body);
EndpointResponse* endpoint_error_response(int status_code, const char* error_message);

// TODO: PHASE 1 - Declare binary response helper functions
// Add function declarations for binary file support:
//
// EndpointResponse* endpoint_binary_response(int status_code, const void* data,
//                                           size_t data_length, const char* content_type);
// DESCRIPTION: Create a response with binary data (images, audio, etc.)
// PARAMETERS:
//   - status_code: HTTP status (usually 200)
//   - data: Pointer to binary data buffer
//   - data_length: Size of data in bytes
//   - content_type: MIME type (e.g., "image/png", "audio/mpeg", "application/octet-stream")
// RETURNS: EndpointResponse with is_binary=1 and body_length set
// MEMORY: Makes a copy of data, caller can free original after function returns
//
// EndpointResponse* endpoint_file_response(int status_code, const char* file_path);
// DESCRIPTION: Read a file from disk and create a binary response
// PARAMETERS:
//   - status_code: HTTP status (usually 200)
//   - file_path: Path to file on disk
// RETURNS: EndpointResponse with file contents, or NULL on error
// MEMORY: Allocates memory for file contents, automatically detects content type from extension
// NOTE: Returns NULL if file doesn't exist or can't be read
//
// IMPLEMENTATION HINTS:
// - Use fopen() with "rb" mode for binary files
// - Use fseek()/ftell() to get file size
// - Use malloc() to allocate buffer for file contents
// - Use memcpy() instead of strcpy() for binary data
// - Set is_binary=1 and body_length=actual_size

#endif
