# Language Split: C vs C++ for Roleplay.cpp

## C Components (Justified by Performance & Simplicity)

### 1. HTTP Server Core (`server/`)
**Language:** C
**Justification:**
- Socket programming is pure POSIX C API
- No need for classes/objects - simple request/response structs suffice
- Minimal dependencies = faster compilation, smaller binary
- Direct memory control for zero-copy operations
- Current implementation already proven in C

### 2. HTTP Protocol Layer (`server/http.c`)
**Language:** C
**Justification:**
- String parsing with `strstr`, `sscanf` is straightforward in C
- Fixed-size buffers and simple structs are sufficient
- No polymorphism needed - just parse and build responses
- Performance-critical path benefits from C's simplicity

### 3. Endpoint Routing (`server/endpoint.c`)
**Language:** C
**Justification:**
- Array-based endpoint registry is simple and fast
- Function pointers provide sufficient abstraction
- No need for inheritance or virtual methods
- Linear search is acceptable for <100 endpoints

### 4. Configuration Parser
**Language:** C
**Justification:**
- File I/O with `fopen`/`fgets` is simple
- Key-value parsing doesn't need complex data structures
- Minimal state machine for INI/TOML parsing
- No need for exceptions - return codes work fine

### 5. Memory Management Utilities
**Language:** C
**Justification:**
- Custom allocators (pool, arena) are C idioms
- Direct `malloc`/`free` control for profiling
- No hidden allocations from C++ containers
- Explicit ownership model via conventions
- Pinned memory allocation for CUDA transfers

### 6. IPC Layer (Pipes/Shared Memory)
**Language:** C
**Justification:**
- POSIX IPC APIs are C-based
- Simple producer-consumer patterns
- Lock-free ring buffers for audio streaming
- Direct system call access

---

## C++ Components (Justified by Complexity Management)

### 1. AI Model Integration (`src/ai/`)
**Language:** C++
**Justification:**
- **llama.cpp** is C++ - native integration required
- **whisper.cpp** is C++ - native integration required
- RAII for model lifecycle (load/unload) prevents leaks
- Exception handling for model loading failures
- C++ templates for generic tensor operations
- **Cannot reasonably rewrite these in C**

### 2. Conversation State Manager
**Language:** C++
**Justification:**
- `std::vector` for dynamic message history (safer than manual realloc)
- `std::unordered_map` for session lookup (hash table without manual implementation)
- `std::string` prevents buffer overflows in message handling
- Smart pointers (`std::unique_ptr`) for session ownership
- **Complexity of manual C implementation outweighs C++ overhead**

### 3. Thread Pool & Work Queue
**Language:** C++
**Justification:**
- `std::thread` for portable threading
- `std::mutex` + `std::condition_variable` for thread-safe queue
- RAII for automatic lock management (`std::lock_guard`)
- Exception safety in multi-threaded context
- Thread affinity control for CPU pinning
- **Manual C implementation is error-prone and complex**

### 4. WebSocket Handler
**Language:** C++
**Justification:**
- **Required for real-time audio streaming**
- State machine complexity (handshake, framing, ping/pong)
- Async I/O with `epoll`/`kqueue` integration
- Frame parsing/building cleaner with C++ abstractions
- Use **uWebSockets** (C++) or **Boost.Beast** for proven implementation
- **C implementation would be 3x more code and bug-prone**

### 5. JSON Serialization/Deserialization
**Language:** C++ (with library)
**Justification:**
- Use **simdjson** (C++) for parsing - 2-5x faster than manual C
- Type-safe JSON construction vs manual `sprintf`
- Automatic escaping and validation
- **Manual C JSON handling is tedious and bug-prone**

---

## CUDA Components (C/C++ + CUDA C)

### 1. Audio Preprocessing Kernels (`src/audio/cuda/`)
**Language:** CUDA C
**Justification:**
- **Mel-spectrogram computation** - 10-20x speedup on GPU
- Parallel FFT for audio feature extraction
- Batch processing multiple audio chunks simultaneously
- Use **cuFFT** library for optimized transforms
- **High gain: 50ms → 5ms preprocessing time**

### 2. CUDA Memory Manager (`src/audio/cuda/memory.c`)
**Language:** C (with CUDA runtime)
**Justification:**
- Pinned memory allocation for fast CPU↔GPU transfers
- Memory pool for reusing GPU buffers (avoid allocation overhead)
- Zero-copy buffers for streaming audio
- Simple C API: `cuda_alloc_pinned()`, `cuda_free_pinned()`
- **C is sufficient - just wrapper around cudaMallocHost()**

### 3. Audio Ring Buffer (GPU-aware) (`src/audio/ringbuf.c`)
**Language:** C
**Justification:**
- Lock-free circular buffer for audio samples
- Direct GPU memory pointers for zero-copy
- Producer (audio input) / Consumer (CUDA kernel) pattern
- Atomic operations for thread safety
- **C is ideal for low-level lock-free data structures**

---

## Assembly Components (Only High-Gain Optimizations)

### 1. Audio Resampling (x86-64 AVX2)
**Language:** C with inline assembly / intrinsics
**Justification:**
- **Resample 48kHz → 16kHz** for Whisper input
- AVX2 SIMD: process 8 samples in parallel
- **Gain: 3-5x faster than scalar C** (15ms → 3ms for 1s audio)
- Use intrinsics first (`_mm256_*`), assembly only if needed
- **High gain justifies complexity**

### 2. WebSocket Frame Masking (x86-64)
**Language:** C with inline assembly
**Justification:**
- XOR masking of WebSocket payload (required by spec)
- Process 32 bytes per iteration with AVX2
- **Gain: 4-6x faster** for large audio frames (100KB+)
- Critical path for real-time audio streaming
- **High gain justifies assembly**

### 3. Fast Memory Copy for Audio Buffers (ARM NEON / x86 AVX)
**Language:** C with intrinsics
**Justification:**
- Copy audio samples between ring buffers
- NEON/AVX: 128-bit aligned copies
- **Gain: 2-3x faster than memcpy** for small buffers (<4KB)
- **Moderate gain - use only if profiling shows bottleneck**

**Assembly Decision Rule:**
- **Use if:** >3x speedup AND in critical path (audio/WebSocket)
- **Skip if:** <2x speedup OR not in hot path
- **Always:** Provide C fallback for portability

---

## Hybrid Components (C Interface, C++ Implementation)

### 1. Model Wrapper (`src/ai/model_wrapper.cpp`)
**Interface:** C
**Implementation:** C++
**Justification:**
- Expose C API for use by C server code
- Internally use C++ for llama.cpp/whisper.cpp integration
- CUDA context management for GPU acceleration
- Example:
  ```c
  // C header (model_wrapper.h)
  typedef struct ModelHandle ModelHandle;
  ModelHandle* model_load(const char* path, int gpu_id);
  char* model_infer(ModelHandle* model, const char* prompt);
  void model_free(ModelHandle* model);
  ```
  ```cpp
  // C++ implementation (model_wrapper.cpp)
  extern "C" {
      ModelHandle* model_load(const char* path, int gpu_id) {
          cudaSetDevice(gpu_id);
          return reinterpret_cast<ModelHandle*>(new LlamaModel(path));
      }
  }
  ```

### 2. Session Manager (`src/core/session_manager.cpp`)
**Interface:** C
**Implementation:** C++
**Justification:**
- C server calls simple C functions
- C++ manages complex state internally
- Thread-safe session storage with mutexes
- Best of both worlds: simple API, robust implementation

### 3. WebSocket Manager (`src/websocket/ws_manager.cpp`)
**Interface:** C
**Implementation:** C++
**Justification:**
- C server registers WebSocket upgrade handler
- C++ handles WebSocket protocol complexity
- Async I/O with epoll for thousands of connections
- Audio streaming over WebSocket frames

---

## Decision Matrix

| Component | Language | Key Reason |
|-----------|----------|------------|
| HTTP Server | C | POSIX sockets, zero-copy, simplicity |
| Endpoint Routing | C | Function pointers sufficient |
| Config Parser | C | Simple file I/O, no complex state |
| Memory Allocators | C | Direct control, CUDA pinned memory |
| IPC Layer | C | Lock-free ring buffers, POSIX APIs |
| Audio Ring Buffer | C | Lock-free, GPU-aware pointers |
| **AI Model Integration** | **C++** | **llama.cpp/whisper.cpp are C++** |
| **Conversation State** | **C++** | **STL containers safer than manual** |
| **Thread Pool** | **C++** | **RAII + exception safety** |
| **WebSocket Handler** | **C++** | **State machine + async I/O** |
| **JSON Handling** | **C++** | **simdjson 2-5x faster** |
| **CUDA Kernels** | **CUDA C** | **Mel-spectrogram 10-20x speedup** |
| **CUDA Memory Mgr** | **C** | **Simple wrapper around CUDA runtime** |
| **Audio Resampling** | **C + AVX2** | **3-5x speedup (assembly justified)** |
| **WebSocket Masking** | **C + AVX2** | **4-6x speedup (assembly justified)** |

---

## Build Strategy

### Compilation
```makefile
# C files
CFLAGS = -std=c11 -O3 -Wall -Wextra -march=native -mavx2
%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

# C++ files
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -march=native
%.o: %.cpp
	g++ $(CXXFLAGS) -c $< -o $@

# CUDA files
NVCCFLAGS = -O3 -arch=sm_75 --use_fast_math
%.o: %.cu
	nvcc $(NVCCFLAGS) -c $< -o $@

# Link with C++ (for stdlib + CUDA runtime)
roleplay: server.o http.o audio.o model.o session.o ws.o cuda_kernels.o
	g++ -o $@ $^ -lpthread -lm -lcudart -lcufft -luv
```

### Directory Structure
```
src/
├── server/              # Pure C (HTTP server core)
│   ├── server.c
│   ├── http.c
│   └── endpoint.c
├── audio/               # C + CUDA
│   ├── ringbuf.c        # Lock-free ring buffer (C)
│   ├── resample.c       # AVX2 resampling (C + intrinsics)
│   └── cuda/
│       ├── memory.c     # CUDA memory manager (C wrapper)
│       └── preprocess.cu # Mel-spectrogram kernels (CUDA C)
├── websocket/           # C++ (WebSocket protocol)
│   ├── ws_manager.cpp   # WebSocket handler
│   ├── ws_manager.h     # C interface
│   └── frame_mask.c     # AVX2 masking (C + intrinsics)
├── ai/                  # C++ (model integration)
│   ├── model_wrapper.cpp
│   └── wrapper.h        # C interface
├── core/                # C++ (session, threading)
│   ├── session_manager.cpp
│   ├── thread_pool.cpp
│   └── session.h        # C interface
├── utils/               # Pure C (config, memory)
│   ├── config.c
│   └── allocator.c
└── json/                # C++ (simdjson)
    └── json_handler.cpp
```

### Thread Architecture
```
Main Thread (C)
├── HTTP Accept Loop
└── WebSocket Accept Loop

Worker Threads (C++)
├── Thread 1: Text inference (llama.cpp + CUDA)
├── Thread 2: Audio inference (whisper.cpp + CUDA)
├── Thread 3: Audio preprocessing (CUDA kernels)
└── Thread 4-N: WebSocket I/O (epoll/kqueue)

CUDA Streams
├── Stream 0: Text model inference
├── Stream 1: Audio model inference
└── Stream 2: Audio preprocessing (async)
```

---

## Performance Targets

| Component | Baseline (C) | Optimized | Technique |
|-----------|--------------|-----------|-----------|
| Audio resample | 15ms | 3ms | AVX2 SIMD |
| Mel-spectrogram | 50ms | 5ms | CUDA kernels |
| WebSocket mask | 12ms | 2ms | AVX2 XOR |
| JSON parse | 8ms | 2ms | simdjson |
| **Total latency** | **~500ms** | **<100ms** | **Combined** |

---

## Summary

**C for:** HTTP server, lock-free buffers, CUDA wrappers, config parsing
**C++ for:** AI models, WebSocket, threading, JSON, session management
**CUDA for:** Audio preprocessing (mel-spectrogram, FFT)
**Assembly for:** Audio resampling, WebSocket masking (>3x gains only)

**Ratio:** ~60% C, ~30% C++, ~8% CUDA, ~2% Assembly (by line count)
**Philosophy:** C by default, C++ for complexity, CUDA for parallelism, Assembly for proven hot paths.

