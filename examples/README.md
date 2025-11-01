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

The Makefile uses a simple swap-and-build approach. Change `APP_NAME` to build different applications.

### Quick Start

```bash
# Build the default app (calculator)
make

# Build a specific app by swapping APP_NAME
make APP_NAME=sound_gen

# Run the built app
make run

# Run with a specific app
make APP_NAME=calculator run

# Run tests
make APP_NAME=calculator test

# Clean all build artifacts
make clean
```

### Available Commands

| Command | Description |
|---------|-------------|
| `make` | Build default app (calculator) |
| `make APP_NAME=<name>` | Build specific app |
| `make run` | Run the built application |
| `make test` | Run tests for the application |
| `make clean` | Remove all build artifacts |
| `make help` | Show all available commands |

### Build Directory

All build artifacts (`.o` files and `.exe` files) are stored in the `build/` directory:
- `build/calculator.exe` - Compiled calculator application
- `build/sound_gen.exe` - Compiled sound generator application
- `build/*.o` - Intermediate object files

This keeps the main directory clean with only source files.

### Adding New Examples

To add a new example application:

1. Create your source file (e.g., `my_app.c`) in the `examples/` directory
2. Build it:
   ```bash
   make APP_NAME=my_app
   ```
3. Run it:
   ```bash
   make run
   ```

That's it! No need to modify the Makefile. All executables are built with `.exe` extension in the `build/` folder.

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