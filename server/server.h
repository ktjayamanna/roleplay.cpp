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
} RequestContext;

// Response structure returned by endpoint handlers
typedef struct EndpointResponse {
    int status_code;
    char* body;                    // Response body (handler must allocate)
    char* content_type;           // Content type (optional, defaults to application/json)
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

// Register a simple endpoint that returns static content
// method: "GET", "POST", "PUT", "DELETE"
// content_type: "application/json", "text/plain", etc. (optional, defaults to application/json)
int server_register_simple(const char* path, const char* method, const char* response_body, const char* content_type);

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
#define SERVER_SIMPLE_GET(path, response) server_register_simple(path, "GET", response, NULL)

#endif
