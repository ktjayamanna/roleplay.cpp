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

### Just‑in‑Time Topic → Build Map

Use this map to ensure you build exactly what you just learned. After you finish a topic, do the matching mini‑build before moving on.

- C syntax: variables, control flow, functions
  - Build: A CLI echo tool with argv parsing (src/utils/), return codes for errors
  - Done when: it handles unknown flags and prints a helpful usage string

- Strings and arrays
  - Build: HTTP request line parser that extracts method and path (src/http/)
  - Done when: malformed requests don’t crash; adds unit test in tests/test_http_parser.c

- File I/O (fopen/fgets/fread), error handling (errno)
  - Build: INI/TOML‑style config loader for server settings (src/core/config.c)
  - Done when: missing keys fall back to defaults; tests cover malformed lines

- Structs and modular design (headers, .c files)
  - Build: Define http_request_t/http_response_t headers in include/
  - Done when: the server compiles without exposing internal fields across modules

- Processes and the OS (system(), exit codes)
  - Build: Minimal process launcher that shells out to a mock AI (src/ai/launcher.c)
  - Done when: you capture stdout and return it as the HTTP body for /chat

- Pointers 101 (addresses, dereference, pointer arithmetic)
  - Build: Replace fixed buffers with a malloc’d body sized by Content‑Length
  - Done when: Valgrind shows 0 leaks after 2 minutes of requests

- Dynamic memory APIs (malloc/calloc/realloc/free)
  - Build: Growable buffer type (vector<char>) for request/response assembly (src/utils/buf.c)
  - Done when: reallocation preserves content; unit tests pass

- Linked lists and ownership
  - Build: Conversation history as an intrusive singly linked list
  - Done when: you can append/prune N messages and free everything cleanly

- Pipes/sockets & IPC
  - Build: Replace system() with bidirectional pipes to the AI process
  - Done when: you stream partial responses over chunked HTTP without blocking

- Ring buffers and circular queues
  - Build: Audio ring buffer for Whisper input (src/audio/ringbuf.c)
  - Done when: producer/consumer tests show no overwrites at target rates

- Bitwise ops, masks, and bitfields
  - Build: Pack server flags (streaming, gzip, auth) into a byte sent in response headers
  - Done when: unit tests verify set/clear/toggle paths across all flags

- Endianness and binary formats
  - Build: WAV header parser (src/audio/wav.c) with host/network byte‑order conversions
  - Done when: golden test files round‑trip; invalid headers rejected with clear errors

- Checksums and simple encodings
  - Build: Base64 encoder/decoder + fast rolling hash for headers
  - Done when: vectors from RFC test files match exactly

- Hash tables and maps
  - Build: Open‑addressing hash table for session store (src/core/hashtab.c)
  - Done when: collision stress tests keep load factor under target and lookups O(1) avg

- Queues/priority queues
  - Build: Token bucket rate limiter + priority queue for requests
  - Done when: enforced limits remain accurate at 1–100 rps in tests

- CMake/Make, static analysis, and testing
  - Build: Makefile targets: make test, make asan, make tsan, make bench
  - Done when: CI (local) runs unit tests and Valgrind/ASan cleanly

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


### Performance Considerations
- **Zero-Copy Operations**: Minimize memory copies in data pipeline
- **Lock-Free Queues**: Handle concurrent requests without blocking
- **CPU Affinity**: Pin threads to specific cores for consistent latency
- **Memory Prefetching**: Optimize cache usage for AI model access

## Success Metrics

- **Latency**: Sub-500ms response time for text chat, sub-2s for voice
- **Memory Usage**: Efficient memory utilization under 8GB total
- **Reliability**: 99.9% uptime with proper error recovery
- **Code Quality**: Clean, maintainable C/C++ following best practices

## Debugging and Profiling

### Development Tools
- **GDB**: For debugging C/C++ applications
- **Valgrind**: Memory error detection and profiling
- **Perf**: Linux performance analysis
- **AddressSanitizer**: Runtime error detection
- **Clang-tidy**: Static code analysis

### Performance Monitoring
- **System Metrics**: CPU, memory, and GPU utilization
- **Application Metrics**: Request latency, throughput, error rates
- **AI Model Metrics**: Inference time, model loading time
- **Network Metrics**: Connection count, bandwidth usage

## Contributing Guidelines

### Code Style
- Follow Linux kernel coding style for C code
- Use consistent naming conventions (snake_case for functions/variables)
- Include comprehensive comments for complex algorithms
- Maintain clean separation between modules

### Git Workflow
- Use feature branches for new development
- Write descriptive commit messages
- Include tests with new features
- Update documentation for API changes

### Pull Request Process
1. Ensure all tests pass
2. Run static analysis tools
3. Update relevant documentation
4. Request code review from maintainers

## Troubleshooting

### Common Issues

#### Build Problems
```bash
# Missing dependencies
sudo apt install build-essential cmake pkg-config

# Compiler errors with GPU libraries
export CUDA_PATH=/usr/local/cuda
export LD_LIBRARY_PATH=$CUDA_PATH/lib64:$LD_LIBRARY_PATH
```

#### Runtime Issues
```bash
# Memory allocation failures
ulimit -v unlimited  # Remove virtual memory limits
echo 'vm.overcommit_memory=1' | sudo tee -a /etc/sysctl.conf

# GPU access problems
sudo usermod -a -G docker $USER  # Add user to docker group
sudo systemctl restart docker     # Restart Docker daemon
```

#### Performance Issues
- **High Memory Usage**: Check for memory leaks with Valgrind
- **Slow Response Times**: Profile with perf and optimize hot paths
- **GPU Utilization**: Monitor with nvidia-smi, ensure proper CUDA setup

### Debug Mode
```bash
# Compile with debug symbols
make DEBUG=1

# Run with debugging tools
gdb ./roleplay_server
valgrind --leak-check=full ./roleplay_server
```

## Milestone Tracking

### Phase 1 Checklist: C Fundamentals ✓/❌
- [ ] Basic HTTP request parsing
- [ ] Simple GET/POST endpoint handling
- [ ] JSON response generation
- [ ] Configuration file parsing
- [ ] Process execution with system() calls
- [ ] Basic error handling and logging
- [ ] Command-line argument processing

### Phase 2 Checklist: Pointers & Memory ✓/❌
- [ ] Dynamic memory allocation for requests
- [ ] Pointer-based string manipulation
- [ ] Inter-process communication with pipes
- [ ] Linked list for conversation history
- [ ] Memory leak detection and prevention
- [ ] Buffer overflow protection
- [ ] Proper cleanup and resource management

### Phase 3 Checklist: Bit Manipulation ✓/❌
- [ ] Audio header parsing with bit operations
- [ ] Binary protocol implementation
- [ ] Custom serialization formats
- [ ] Performance optimization with bitwise ops
- [ ] Endianness handling
- [ ] SIMD-style optimizations
- [ ] Base64 encoding/decoding

### Phase 4 Checklist: Data Structures ✓/❌
- [ ] Hash table for session management
- [ ] LRU cache implementation
- [ ] Priority queue for request scheduling
- [ ] B-tree for persistent storage
- [ ] Connection pooling
- [ ] Rate limiting algorithms
- [ ] Advanced error recovery

### Phase 5 Checklist: Systems Programming ✓/❌
- [ ] ARM assembly optimizations
- [ ] Custom memory allocators
- [ ] Kernel module integration
- [ ] Real-time scheduling
- [ ] GPU memory management
- [ ] Zero-copy operations
- [ ] Performance profiling and tuning
