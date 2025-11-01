# Server Folder Overview

## 1. server.h - Main Library Header

Defines the public API for the HTTP server library. Key components:

**Enums & Structs:**
- `HttpMethod` - Enum for HTTP methods (GET, POST, PUT, DELETE)
- `RequestParam` - Holds query parameter name/value pairs
- `RequestContext` - Contains request data (method, path, body, parameters)
- `EndpointResponse` - Response structure (status code, body, content type)
- `EndpointHandler` - Function pointer type for endpoint handlers

**Core Functions:**
- `server_init(port)` - Initialize server on specified port
- `server_start()` - Start serving requests (blocking)
- `server_stop()` - Stop the server

**Endpoint Registration:**
- `server_register_handler()` - Register custom handler endpoints

**Helper Functions:**
- `request_get_param()` - Extract query parameters
- `request_get_param_int()` - Get parameter as integer
- `request_get_body()` - Get POST/PUT body
- `response_json()` - Create JSON response
- `response_text()` - Create plain text response
- `response_error()` - Create error response

---

## 2. server.c - Core Server Implementation

Implements the main server logic:

- **`server_init(port)`** - Sets up the server:
  - Creates a TCP socket
  - Sets socket options (SO_REUSEADDR)
  - Binds to the specified port
  - Starts listening for connections
  - Initializes the endpoint system

- **`server_start()`** - Main server loop:
  - Accepts incoming client connections
  - Calls `handle_client()` for each connection
  - Runs until `server_stop()` is called

- **`server_stop()`** - Shuts down the server:
  - Sets running flag to false
  - Closes the socket

- **`handle_client(client_fd)`** - Processes individual requests:
  - Reads HTTP request from socket
  - Parses method and URL
  - Routes to appropriate endpoint
  - Sends response back to client

- **`parse_url(url, path, query_string)`** - Splits URL into path and query string

- **`handle_route(method, url)`** - Routes requests to endpoints:
  - Parses URL
  - Dispatches to endpoint system
  - Builds HTTP response
  - Returns response string

- **`server_register_handler()`** - Endpoint registration wrapper that converts string method names to enums

- **Response helpers** - Wrapper functions that delegate to endpoint system

---

## 3. endpoint.h - Endpoint System Header

Defines the modular endpoint registration system:

- **`RegisteredEndpoint` struct** - Stores endpoint metadata (path, method, handler, active flag)
- **Global registry** - `endpoint_registry[]` array and `endpoint_count`
- **Core functions:**
  - `endpoint_system_init()` - Initialize registry
  - `endpoint_register()` - Register new endpoint
  - `endpoint_dispatch()` - Route request to handler
  - `endpoint_response_free()` - Free response memory
  - `endpoint_create_response()` - Create response object
  - `endpoint_get_param()` - Extract parameter
  - `endpoint_get_param_int()` - Extract integer parameter
  - `endpoint_json_response()` - Create JSON response
  - `endpoint_error_response()` - Create error response

---

## 4. endpoint.c - Endpoint System Implementation

Implements the modular endpoint system:

- **`endpoint_system_init()`** - Clears the endpoint registry

- **`endpoint_register(path, method, handler)`** - Registers a new endpoint:
  - Finds an empty slot in the registry
  - Stores path, HTTP method, and handler function pointer
  - Marks slot as active

- **`parse_method(method_str)`** - Converts "GET"/"POST" strings to enum values

- **`parse_query_string(query_string, context)`** - Parses query parameters:
  - Splits by `&` to get key=value pairs
  - Stores in request context

- **`endpoint_dispatch(method_str, path, query_string, body, body_length)`** - Routes requests:
  - Finds matching endpoint in registry
  - Builds RequestContext with parsed parameters
  - Calls the handler function
  - Returns 404 if no match found

- **`endpoint_response_free(response)`** - Frees allocated response memory

- **`endpoint_create_response(status_code, body, content_type)`** - Creates response object with allocated memory

- **`endpoint_get_param()` & `endpoint_get_param_int()`** - Parameter extraction helpers

- **`endpoint_json_response()` & `endpoint_error_response()`** - Response creation helpers

---

## 5. http.h - HTTP Protocol Header

Defines HTTP protocol structures and constants:

**Constants:**
- `HTTP_OK` (200)
- `HTTP_NOT_FOUND` (404)

**`HttpResponse` struct** - Contains status code, body, and body length

**Function prototypes:**
- `http_build_response()` - Build HTTP response string
- `http_parse_request()` - Parse HTTP request line

---

## 6. http.c - HTTP Protocol Implementation

Implements HTTP protocol handling:

- **`http_build_response(status_code, body)`** - Constructs HTTP response:
  - Allocates memory for response struct
  - Formats HTTP response with headers (Content-Type, Content-Length, Connection)
  - Returns complete HTTP response string

- **`http_parse_request(request, method, path)`** - Parses HTTP request:
  - Extracts HTTP method (GET, POST, etc.)
  - Extracts request path
  - Uses `sscanf()` for parsing

---

## Architecture Summary

The server is designed as a **modular library** with clear separation of concerns:

1. **HTTP Layer** (`http.h/c`) - Low-level HTTP protocol handling
2. **Endpoint Layer** (`endpoint.h/c`) - Modular endpoint registration and routing
3. **Server Layer** (`server.h/c`) - High-level server management and socket handling

This design allows applications to easily add new endpoints without modifying core server code.

---

## Memory Management Responsibilities

### Framework Responsibilities

The framework handles memory allocation and deallocation for:
- HTTP request parsing and processing
- Endpoint registry management
- Query parameter parsing
- Internal request context structures
- Response objects created by `response_*()` functions

### User Responsibilities

**Users are responsible for freeing dynamic memory in the following cases:**

#### 1. Temporary Allocations in Handlers

If you allocate memory while processing a request, you must free it:

```c
EndpointResponse* handle_post(const RequestContext* request) {
    const char* body = request_get_body(request);

    // If you allocate memory to process the body
    char* processed = malloc(strlen(body) + 1);
    strcpy(processed, body);

    // ... do processing ...

    free(processed);  // YOU must free your allocation

    return response_json(200, "{\"status\": \"ok\"}");
}
```

#### 2. Response Body Strings (Best Practice)

Use stack-allocated buffers when possible:

```c
EndpointResponse* handle_data(const RequestContext* request) {
    // ✓ CORRECT: Use stack-allocated buffer
    char response[256];
    snprintf(response, sizeof(response), "{\"data\": \"value\"}");
    return response_json(200, response);  // Framework copies the string
}
```

If you must use dynamic allocation:

```c
EndpointResponse* handle_complex(const RequestContext* request) {
    char* json = malloc(512);
    snprintf(json, 512, "{\"complex\": \"data\"}");
    EndpointResponse* resp = response_json(200, json);
    free(json);  // Free your temporary allocation
    return resp;  // Framework owns the response
}
```

### Memory Ownership Rules

| Object | Allocated By | Freed By | Notes |
|--------|--------------|----------|-------|
| Response body (from `response_*`) | Framework | Framework | Don't free the `EndpointResponse` |
| Request parameters | Framework | Framework | Don't free parameter strings |
| Request body | Framework | Framework | Don't free `request->body` |
| Temporary strings in handlers | User | User | Free any `malloc`'d strings you create |
| Response struct itself | Framework | Framework | Returned by `response_*` functions |

### Best Practices

1. **Use stack allocation when possible** - Avoid `malloc` for small, fixed-size buffers
2. **Let the framework own responses** - Don't free `EndpointResponse` objects
3. **Free your temporary allocations** - Any `malloc` you do, you must `free`
4. **Don't free framework-owned data** - Parameters, request body, etc.
5. **Use `snprintf` for safety** - Prevent buffer overflows with size limits

### What NOT to Free

```c
EndpointResponse* handle_params(const RequestContext* request) {
    const char* param = request_get_param(request, "name");
    // param is owned by the framework - do NOT free it

    const char* body = request_get_body(request);
    // body is owned by the framework - do NOT free it

    // If you need to keep a parameter, make a copy
    char* copy = malloc(strlen(param) + 1);
    strcpy(copy, param);
    // ... use copy ...
    free(copy);  // Free YOUR copy

    return response_json(200, "{\"ok\": true}");
}
```
