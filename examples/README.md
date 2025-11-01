# Calculator Addition Demo

This is a simple example demonstrating how to use the C HTTP Server Library to create a web API.

## What This Example Shows

The calculator implements a basic **addition service** that demonstrates:

- **Library Import** - How to include the server library from an external project
- **Business Logic** - Pure C function for addition (`add_numbers()`)
- **Endpoint Handler** - Converting C function to HTTP endpoint
- **Parameter Handling** - Extracting query parameters from requests
- **JSON Responses** - Returning structured data
- **Error Handling** - Validating input and returning appropriate errors

## Files

- `calculator.c` - Main application demonstrating library usage
- `Makefile` - Build configuration showing how to link with the library
- `README.md` - This documentation

## How It Works

### 1. Import the Library
```c
#include "../server/server.h"  // Import from parent server/ directory
```

### 2. Pure Business Logic
```c
// Your core function - no HTTP knowledge needed
int add_numbers(int a, int b) {
    return a + b;
}
```

### 3. HTTP Endpoint Handler
```c
EndpointResponse* handle_add(const RequestContext* request) {
    // Extract parameters
    const char* a_str = request_get_param(request, "a");
    const char* b_str = request_get_param(request, "b");
    
    // Validate input
    if (!a_str || !b_str) {
        return response_error(400, "Missing parameters 'a' and 'b'");
    }
    
    // Call business logic
    int result = add_numbers(atoi(a_str), atoi(b_str));
    
    // Return JSON response
    char response[256];
    snprintf(response, sizeof(response), 
        "{\"operation\": \"add\", \"a\": %d, \"b\": %d, \"result\": %d}", 
        atoi(a_str), atoi(b_str), result);
    
    return response_json(200, response);
}
```

### 4. Server Setup
```c
int main() {
    server_init(8080);                    // Initialize on port 8080
    SERVER_GET("/add", handle_add);       // Register endpoint
    server_start();                       // Start serving
    return 0;
}
```

## Building and Running

```bash
# Build the calculator
make

# Run the server
./calculator
```

## Testing the API

```bash
# Addition endpoint
curl "http://localhost:8080/add?a=5&b=3"
# Returns: {"operation": "add", "a": 5, "b": 3, "result": 8}

# Error handling
curl "http://localhost:8080/add?a=5"
# Returns: {"error": "Missing parameters 'a' and 'b'"}

# Large numbers
curl "http://localhost:8080/add?a=1000&b=2000"
# Returns: {"operation": "add", "a": 1000, "b": 2000, "result": 3000}
```