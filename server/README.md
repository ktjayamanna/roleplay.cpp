# C Web Server

A from-scratch web server implementation in C with no external libraries.

## Project Structure

```
server/
├── main.c          # Entry point
├── server.h/c      # Core server socket handling
├── http.h/c        # HTTP protocol parsing and response building
├── routes.h/c      # Route handlers (health check, etc.)
├── Makefile        # Build configuration
└── README.md       # This file
```

## Getting Started

### Using VS Code Dev Container (Recommended)

1. Open this project in VS Code
2. Install the "Dev Containers" extension
3. Press `F1` and select "Dev Containers: Reopen in Container"
4. Wait for the container to build
5. Open a terminal in VS Code (it will be inside the container)
6. Navigate to the server directory: `cd server`
7. Build and run: `make run`


## Implementation Order

Follow this order to implement the server:

1. **Start with headers** - Define your interfaces first

## Implementation Order

Follow this order to implement the server:

1. **Start with headers** - Define your interfaces first
   - `server.h` - Define the Server struct and function prototypes
   - `http.h` - Define HTTP constants and response structure
   - `routes.h` - Define route handler prototypes

2. **Implement HTTP utilities** - Build the foundation
   
   - `server.h` - Define the Server struct and function prototypes
   - `http.h` - Define HTTP constants and response structure
   - `routes.h` - Define route handler prototypes

2. **Implement HTTP utilities** - Build the foundation
   - `http.c` - Implement response building and request parsing
   - `routes.c` - Implement the health check handler

3. **Implement server core** - The main logic
   - `server.c` - Implement socket creation, binding, listening, and accepting

4. **Wire it together** - Connect all the pieces
   - `main.c` - Initialize and start the server

## Building and Running

```bash
# Compile the server
make

# Run the server
make run

# In another terminal, test the health check
make test
# Or manually:
curl http://localhost:8080/health

# Clean build artifacts
make clean

# Debug with gdb
make debug

# Check for memory leaks
make valgrind
```

## Expected Behavior

When working correctly:

```bash
$ ./webserver
Server starting on port 8080...
Server listening on port 8080

# In another terminal:
$ curl http://localhost:8080/health
{"status": "ok"}

$ curl -i http://localhost:8080/health
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 16
Connection: close

{"status": "ok"}
```

## Tips

### Socket Programming Concepts
- **Socket**: An endpoint for network communication
- **bind()**: Associates a socket with an address and port
- **listen()**: Marks socket as passive (ready to accept connections)
- **accept()**: Blocks until a client connects, returns new socket for that client

### HTTP Protocol Basics
- HTTP is text-based
- Request format: `METHOD /path HTTP/1.1\r\n[headers]\r\n\r\n[body]`
- Response format: `HTTP/1.1 STATUS_CODE STATUS_TEXT\r\n[headers]\r\n\r\n[body]`
- `\r\n` is CRLF (Carriage Return + Line Feed)

### Memory Management
- Always `free()` what you `malloc()`
- Use `valgrind` to check for leaks
- Be careful with string operations (buffer overflows!)

## Common Issues and Solutions

### "Address already in use"
The port is still bound from a previous run. Wait a few seconds or use `setsockopt()` with `SO_REUSEADDR`.

### Segmentation Fault
- Check for NULL pointers before dereferencing
- Ensure buffers are large enough
- Use `gdb` to find the exact line: `make debug`

### Connection Refused
- Make sure the server is running
- Check that you're using the correct port
- Verify the port is exposed in Docker

## Next Steps

After getting the health check working, you can extend the server with:
- [ ] More routes (e.g., `/api/status`, `/api/info`)
- [ ] POST request handling
- [ ] Query parameter parsing
- [ ] Request logging
- [ ] Multi-threading for concurrent connections
- [ ] Static file serving


