# roleplay.cpp

## Project Overview

This project implements a high-performance, local AI voice agent using C/C++ for the backend REST API and Next.js for the frontend interface. The system is designed as a progressive learning vehicle for mastering C/C++ fundamentals and advanced systems programming concepts while building a practical, production-quality application.

### System Architecture

```
Next.js Frontend ←→ HTTP/WebSocket ←→ C/C++ REST API ←→ AI Components
                                           ├── llama.cpp (LLM)
                                           ├── whisper.cpp (ASR)
                                           └── Piper TTS
```

## Functional Requirements

- **Chat Interface**: Real-time text-based conversation with AI
- **Voice Mode**: Speech-to-text input and text-to-speech output
- **Low Latency**: Sub-second response times for voice interactions
- **Local Processing**: 100% offline operation with GPU acceleration
- **Single User**: Optimized for personal use with minimal resource overhead

## Technical Requirements

- **Target Platforms**: Linux and macOS with GPU support
- **Deployment**: Single Docker container for easy distribution
- **Frontend**: Next.js with TypeScript
- **Backend**: C/C++ REST API server
- **AI Models**: LLaMA 3 8B, Whisper, Piper TTS

## Learning Path Integration

This project is structured to progressively apply C/C++ and systems programming concepts as you advance through your curriculum.

### Phase 1: C Language Fundamentals

**Concepts Applied:**
- Variables, functions, control structures
- Basic I/O operations
- String manipulation
- Command-line argument processing

**Implementation Opportunities:**
1. **Basic HTTP Server**: Implement a minimal HTTP server that can parse GET/POST requests and return JSON responses
   - Apply string functions (`strlen`, `strcpy`, `strstr`) for header parsing
   - Use control structures for request routing
   - Practice function organization and modular design

2. **Configuration System**: Build a config file parser for server settings
   - File I/O operations for reading configuration
   - String processing for key-value pair parsing
   - Error handling with return codes

3. **Process Orchestration**: Create a simple process manager
   - Use `system()` calls to launch llama.cpp/whisper.cpp
   - Command-line argument construction
   - Basic error checking and process status monitoring

**Deliverable**: A basic REST API that can accept text input, call llama.cpp, and return AI responses via HTTP.

### Phase 2: C Pointers and Memory Management

**Concepts Applied:**
- Pointer arithmetic and dereferencing
- Dynamic memory allocation (`malloc`, `free`)
- Memory layout understanding
- Buffer management

**Implementation Opportunities:**
1. **Dynamic Request Handling**: Replace fixed-size buffers with dynamic allocation
   - Allocate request buffers based on `Content-Length` header
   - Implement proper cleanup to prevent memory leaks
   - Use pointer arithmetic for efficient string operations

2. **Inter-Process Communication**: Replace `system()` calls with pipes
   - Create bidirectional pipes to communicate with AI processes
   - Implement buffered I/O for streaming responses
   - Use pointers to manage multiple concurrent connections

3. **Conversation Memory**: Build an in-memory conversation store
   - Linked list implementation for chat history
   - Dynamic string allocation for variable-length messages
   - Memory pool optimization for frequent allocations

4. **Audio Buffer Management**: Handle raw audio data streams
   - Circular buffers for real-time audio processing
   - Pointer-based audio sample manipulation
   - Zero-copy buffer sharing between processes

**Deliverable**: A more sophisticated API server with proper memory management, IPC-based AI integration, and conversation state tracking.

### Phase 3: Bit Manipulation and Low-Level Operations

**Concepts Applied:**
- Bitwise operations and bit fields
- Binary data formats
- Endianness handling
- Performance optimization

**Implementation Opportunities:**
1. **Audio Format Processing**: Parse and manipulate audio file headers
   - WAV header parsing using bit manipulation
   - Sample rate conversion algorithms
   - Audio compression/decompression helpers

2. **Binary Protocol Design**: Create efficient communication protocols
   - Pack multiple flags into single bytes using bit fields
   - Implement custom binary serialization for AI model inputs
   - Network byte order conversion for cross-platform compatibility

3. **Performance Optimization**: Low-level optimizations for hot paths
   - Bit manipulation for fast string hashing
   - SIMD-style operations using bitwise tricks
   - Memory alignment optimizations

4. **Custom Data Encoding**: Efficient data representation
   - Implement base64 encoding/decoding for binary data transport
   - Create compressed message formats for network efficiency
   - Use bit manipulation for fast checksum calculations

**Deliverable**: An optimized API server with custom binary protocols, efficient audio processing, and performance-tuned data handling.

### Phase 4: Advanced Data Structures

**Concepts Applied:**
- Complex data structure implementation
- Algorithm optimization
- Tree and graph structures
- Hash tables and maps

**Implementation Opportunities:**
1. **Advanced Session Management**: Implement sophisticated state tracking
   - Hash table for O(1) session lookup
   - Tree structures for conversation indexing
   - Priority queues for request scheduling

2. **Intelligent Caching**: Build multi-level caching systems
   - LRU cache implementation for frequent queries
   - Bloom filters for negative cache lookups
   - B-tree structures for persistent conversation storage

3. **Connection Pool Management**: Advanced networking structures
   - Connection pool with load balancing
   - Request routing trees for different endpoints
   - Rate limiting using token bucket algorithms

4. **Model Management**: Efficient AI model loading and switching
   - Reference counting for shared model instances
   - Graph structures for model dependency tracking
   - Memory-mapped file handling for large models

**Deliverable**: A production-ready API server with advanced data structures, intelligent caching, and sophisticated resource management.

## Systems Programming Integration

### ARM Assembly and Low-Level Optimization

**Application Areas:**
1. **Audio Processing Acceleration**
   - Use ARM NEON instructions for audio sample processing
   - Implement custom audio filters using SIMD
   - Optimize matrix operations for AI model preprocessing

2. **Custom Memory Allocators**
   - Write assembly-optimized allocation routines
   - Implement stack-based allocators for request handling
   - Create pool allocators tuned for AI workload patterns

3. **Performance Critical Paths**
   - Hand-optimize JSON parsing using assembly
   - Implement fast string search algorithms
   - Create optimized hash functions for session management

### Linux Device Drivers and Kernel Programming

**Application Areas:**
1. **GPU Memory Management**
   - Direct GPU memory allocation for model storage
   - Custom memory mapping for zero-copy AI inference
   - Implement GPU memory pools for efficient usage

2. **Real-Time Audio Processing**
   - Custom audio device drivers for minimal latency
   - Kernel-level audio buffer management
   - Interrupt-driven audio sample processing

3. **System Resource Optimization**
   - Custom scheduler integration for AI workloads
   - Memory management hooks for large model loading
   - System call optimization for high-frequency operations

### Advanced Kernel Facilities

**Application Areas:**
1. **High-Performance IPC**
   - Shared memory segments for zero-copy AI model sharing
   - Semaphores and mutexes for process synchronization
   - Message queues for asynchronous AI task processing

2. **Real-Time Guarantees**
   - Real-time process scheduling for voice interactions
   - Memory locking to prevent AI model swapping
   - CPU affinity management for consistent performance

3. **Advanced Networking**
   - Kernel bypass networking for ultra-low latency
   - Custom network protocols optimized for AI data
   - Zero-copy network I/O implementation

## Docker Container Implementation

### Container Architecture Benefits
- **Dependency Management**: Bundle all AI models and libraries
- **Cross-Platform Deployment**: Run consistently across different host systems
- **Resource Isolation**: Control CPU/GPU/memory allocation
- **Easy Distribution**: Single container image for end users

### C/C++ Specific Considerations
1. **Multi-Stage Build**: Compile C/C++ components in build stage, copy binaries to runtime stage
2. **GPU Access**: Configure container for GPU passthrough (NVIDIA Docker, ROCm)
3. **Shared Libraries**: Manage dynamic linking for AI libraries within container
4. **Signal Handling**: Proper shutdown and cleanup in containerized environment

### Learning Opportunities in Docker Context
1. **Container Optimization**: Apply systems knowledge to minimize image size
2. **Resource Management**: Use cgroups and namespaces understanding from kernel programming
3. **Security**: Apply privilege separation and capability management
4. **Networking**: Implement container-to-container communication using advanced networking concepts

## Implementation Roadmap

### Milestone 1: Basic HTTP Server (C Fundamentals)
**Timeline**: 2-3 weeks
- Implement basic HTTP request/response handling
- Create simple REST endpoints (`/chat`, `/health`)
- Integrate with llama.cpp via `system()` calls
- Basic error handling and logging

**Learning Focus**: Core C syntax, string manipulation, basic I/O

### Milestone 2: Advanced Server Architecture (Pointers + Memory)
**Timeline**: 3-4 weeks
- Replace system calls with proper IPC (pipes/sockets)
- Implement dynamic memory management for requests
- Add conversation state tracking
- Build connection handling with proper cleanup

**Learning Focus**: Pointer manipulation, dynamic allocation, buffer management

### Milestone 3: Performance Optimization (Bit Manipulation)
**Timeline**: 2-3 weeks
- Add binary protocol support for audio data
- Implement custom serialization formats
- Optimize memory layout and data structures
- Add performance monitoring and profiling

**Learning Focus**: Binary data handling, performance optimization, low-level operations

### Milestone 4: Production Features (Data Structures)
**Timeline**: 4-5 weeks
- Implement advanced caching and session management
- Add concurrent request handling with queuing
- Build sophisticated error recovery and failover
- Create comprehensive logging and monitoring

**Learning Focus**: Complex algorithms, concurrent programming, production reliability

### Milestone 5: Systems Integration (Assembly + Kernel)
**Timeline**: 6-8 weeks
- Add assembly-optimized audio processing
- Implement custom memory allocators
- Build kernel-level optimizations
- Create real-time scheduling for voice processing

**Learning Focus**: Low-level optimization, kernel programming, real-time systems

## Technical Deep Dives

### HTTP Server Implementation
```c
// Example: Applying pointer concepts in HTTP parsing
typedef struct {
    char* method;       // Pointer to method string
    char* path;         // Pointer to URL path
    char* headers;      // Pointer to headers block
    size_t content_len; // Content length for body allocation
    char* body;         // Dynamically allocated body
} http_request_t;
```

### Process Communication
```c
// Example: IPC with AI processes using pipes
typedef struct {
    pid_t pid;          // Process ID
    int stdin_fd;       // Pipe to process stdin
    int stdout_fd;      // Pipe from process stdout
    char* buffer;       // Dynamic response buffer
    size_t buffer_size; // Current buffer allocation
} ai_process_t;
```

### Memory Management Strategy
- **Request Pools**: Pre-allocate request structures for common sizes
- **Buffer Reuse**: Maintain buffer pools to avoid frequent allocation
- **Model Caching**: Memory-map AI models for instant access
- **Garbage Collection**: Implement reference counting for shared resources

### Performance Considerations
- **Zero-Copy Operations**: Minimize memory copies in data pipeline
- **Lock-Free Queues**: Handle concurrent requests without blocking
- **CPU Affinity**: Pin threads to specific cores for consistent latency
- **Memory Prefetching**: Optimize cache usage for AI model access

## Docker Container Specifications

### Base Image Strategy
```dockerfile
# Multi-stage build for C/C++ compilation
FROM nvidia/cuda:12.0-devel-ubuntu22.04 as builder
# Build C/C++ components with GPU support

FROM nvidia/cuda:12.0-runtime-ubuntu22.04 as runtime
# Copy compiled binaries and AI models
```

### Resource Requirements
- **Memory**: 16GB+ for LLaMA 3 8B model
- **GPU**: NVIDIA GPU with 8GB+ VRAM or Apple Silicon Mac
- **Storage**: 10GB+ for all AI models and dependencies
- **CPU**: 4+ cores recommended for concurrent processing

### Container Features
1. **Health Checks**: Implement comprehensive health monitoring
2. **Graceful Shutdown**: Proper signal handling for container lifecycle
3. **Volume Mounts**: Support for external model storage
4. **Environment Configuration**: Runtime configuration via environment variables

## Learning Outcomes

By completing this project, you will have practical experience with:

**C/C++ Programming:**
- Production-quality server development
- Memory management in complex applications
- Network programming and protocol implementation
- Integration with external libraries and processes

**Systems Programming:**
- Low-level performance optimization
- Kernel-level resource management
- Real-time system design
- Hardware-software integration

**DevOps and Deployment:**
- Containerized application development
- Cross-platform compilation and deployment
- Performance tuning and monitoring
- Production reliability patterns

## Getting Started

1. **Environment Setup**: Install Docker, NVIDIA Container Runtime, and development tools
2. **Model Download**: Acquire LLaMA 3 8B, Whisper, and Piper TTS models
3. **Basic Implementation**: Start with Phase 1 HTTP server
4. **Progressive Enhancement**: Add features as you master each concept area
5. **Performance Tuning**: Apply advanced systems concepts for optimization

## Success Metrics

- **Latency**: Sub-500ms response time for text chat, sub-2s for voice
- **Memory Usage**: Efficient memory utilization under 8GB total
- **Concurrent Users**: Handle 10+ simultaneous connections
- **Reliability**: 99.9% uptime with proper error recovery
- **Code Quality**: Clean, maintainable C/C++ following best practices
