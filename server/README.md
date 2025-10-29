# Using the Server as a Library

```c
// calculator.c - Your main application
#include "server.h"

// Your pure business logic
int add(int a, int b) { return a + b; }

int main() {
    server_init(8080);
    SERVER_GET("/add", handle_add);  // Register your endpoint
    server_start();                  // Start serving!
    return 0;
}
```

## Complete Working Example

```c
#include "server.h"

// Pure business logic
int add_numbers(int a, int b) { return a + b; }
int multiply_numbers(int a, int b) { return a * b; }

// Endpoint handler
EndpointResponse* handle_add(const RequestContext* request) {
    const char* a_str = request_get_param(request, "a");
    const char* b_str = request_get_param(request, "b");
    
    if (!a_str || !b_str) {
        return response_error(400, "Missing parameters 'a' and 'b'");
    }
    
    int a = atoi(a_str);
    int b = atoi(b_str);
    int result = add_numbers(a, b);
    
    char response[256];
    snprintf(response, sizeof(response), 
        "{\"operation\": \"add\", \"a\": %d, \"b\": %d, \"result\": %d}", 
        a, b, result);
    
    return response_json(200, response);
}

int main() {
    server_init(8080);
    SERVER_GET("/add", handle_add);
    server_start();
    return 0;
}
```

## ✅ How to Test


```bash
# Addition
curl "http://localhost:8080/add?a=5&b=3"
# Returns: {"operation": "add", "a": 5, "b": 3, "result": 8}

# Multiplication  
curl "http://localhost:8080/multiply?a=6&b=7"
# Returns: {"operation": "multiply", "a": 6, "b": 7, "result": 42}

# Error handling
curl "http://localhost:8080/divide?a=10&b=0"
# Returns: {"error": "Division by zero is not allowed"}
```

## Application

### 1. Include the Library
```c
#include "server.h"
```

### 2. Initialize Server
```c
server_init(8080);  // Pick your port
```

### 3. Register Endpoints
```c
// Simple static responses
server_register_simple("/health", "GET", "{\"status\": \"ok\"}", NULL);

// Custom handlers with business logic
SERVER_GET("/add", handle_add);
SERVER_POST("/data", handle_post_data);
```

### 4. Start Serving
```c
server_start();  // Blocks and serves requests
```

## 📚 Library API Reference

### Core Functions
- `server_init(port)` - Initialize server
- `server_start()` - Start serving (blocks)
- `server_stop()` - Stop server

### Endpoint Registration
- `SERVER_GET(path, handler)` - Register GET endpoint
- `SERVER_POST(path, handler)` - Register POST endpoint  
- `server_register_simple(path, method, response, content_type)` - Static responses

### Request Helpers
- `request_get_param(request, "name")` - Get query parameter
- `request_get_param_int(request, "name", default)` - Get integer parameter
- `request_get_body(request)` - Get request body

### Response Helpers
- `response_json(status, json_body)` - JSON response
- `response_text(status, text_body)` - Plain text response
- `response_error(status, error_message)` - Error response

## 🏗️ Build Your App

### Simple Makefile
```makefile
# Your app Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Library files (include these)
LIB_SRCS = server/server.c server/http.c server/endpoint.c

# Your app files
APP_SRCS = myapp.c

# Build
myapp: $(LIB_SRCS:.c=.o) $(APP_SRCS:.c=.o)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
```

## Example

See `../examples/` for a complete working example:

```bash
# From examples directory
cd examples
make
./calculator

# Test endpoints
curl "http://localhost:8080/add?a=5&b=3"        # → {"result": 8}
curl "http://localhost:8080/health"             # → {"status": "ok"}
```