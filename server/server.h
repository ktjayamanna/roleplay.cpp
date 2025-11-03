// server.h - Core server header

#ifndef SERVER_H
#define SERVER_H

// Maximum number of parameters in a request
#define MAX_PARAM_LENGTH 128
#define MAX_PARAMS 10
#define MAX_PATH_LENGTH 256

// HTTP methods supported
typedef enum {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE
} HttpMethod;

// Structure to hold request parameters (query params, URL params, etc.)
typedef struct {
    char name[MAX_PARAM_LENGTH];
    char value[MAX_PARAM_LENGTH];
} RequestParam;

// Request context passed to endpoint handlers
typedef struct RequestContext {
    HttpMethod method;
    char path[MAX_PATH_LENGTH];
    char* body;                    // Request body (for POST/PUT)
    int body_length;
    RequestParam params[MAX_PARAMS]; // Query parameters and URL parameters
    int param_count;

    // TODO: PHASE 2 - Add request content type tracking
    // Add the following field to track incoming content type:
    // - char content_type[128];   // Content-Type header from request (e.g., "application/octet-stream")
    //
    // RATIONALE:
    // - Handlers need to know if incoming data is JSON, binary, multipart, etc.
    // - This allows proper parsing of request body based on content type
    // - Future: Can be used for multipart/form-data file upload parsing
} RequestContext;

// Response structure returned by endpoint handlers
typedef struct EndpointResponse {
    int status_code;
    void* body;                    // Response body (handler must allocate) - void* to handle both text and binary
    char* content_type;           // Content type (e.g., "application/json", "audio/mpeg", "image/png")
    size_t body_length;            // Length of body in bytes (works for both text and binary)
} EndpointResponse;

// Function pointer type for endpoint handlers
// Your handler receives request context and returns a response
typedef EndpointResponse* (*EndpointHandler)(const RequestContext* request);

// ============================================================================
// CORE SERVER FUNCTIONS
// ============================================================================

// Initialize the server on the specified port
// Returns 0 on success, -1 on error
int server_init(int port);

// Start the server (blocks until server_stop() is called)
// Returns 0 on success, -1 on error
int server_start(void);

// Stop the server (call from signal handler or another thread)
void server_stop(void);

// ============================================================================
// ENDPOINT REGISTRATION FUNCTIONS
// ============================================================================

// Register an endpoint with a custom handler function
int server_register_handler(const char* path, const char* method, EndpointHandler handler);

// ============================================================================
// HELPER FUNCTIONS FOR ENDPOINT HANDLERS
// ============================================================================

// Get a query parameter value by name (returns NULL if not found)
const char* request_get_param(const RequestContext* request, const char* param_name);

// Get a query parameter as integer with default value
int request_get_param_int(const RequestContext* request, const char* param_name, int default_value);

// Get the request body (for POST/PUT requests)
const char* request_get_body(const RequestContext* request);

// Create a JSON response
EndpointResponse* response_json(int status_code, const char* json_body);

// Create a plain text response
EndpointResponse* response_text(int status_code, const char* text_body);

// Create an error response
EndpointResponse* response_error(int status_code, const char* error_message);

// ============================================================================
// CONVENIENCE MACROS
// ============================================================================

// Quick endpoint registration macros
#define SERVER_GET(path, handler) server_register_handler(path, "GET", handler)
#define SERVER_POST(path, handler) server_register_handler(path, "POST", handler)

#endif
