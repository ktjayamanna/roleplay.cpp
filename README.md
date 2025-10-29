# roleplay.cpp

## Project Overview

This project implements a high-performance, local AI voice agent using C/C++ for the backend REST API and Next.js for the frontend interface. The system is designed as a progressive learning vehicle for mastering C/C++ fundamentals and advanced systems programming concepts while building a practical, production-quality application.

## Quick Start

### HTTP Server Library

The project includes a reusable C HTTP server library in the `server/` directory. See `examples/` for a complete calculator demo showing how to use the library.

```bash
cd examples
make
./calculator

# Test the API
curl "http://localhost:8080/add?a=5&b=3"
```

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

## Capabilities

- Real‑time conversational agent with streaming text and voice
- 100% local inference; optional GPU acceleration
- Low‑latency, resource‑efficient single‑user service

## Component Architecture

- C service exposing HTTP/WebSocket endpoints and streaming
- AI adapters communicating with llama.cpp, whisper.cpp, Piper via pipes/sockets
- Audio pipeline with ring buffers and WAV parser
- State & caching: open‑addressing session store, LRU cache, token bucket limiter
- Observability hooks: structured logs and metrics endpoints

## Design Traceability (Competency → Implementation)

- Memory management → dynamic request/response buffers, pool allocators, zero‑copy paths
- Concurrency & IPC → nonblocking pipes/sockets, streaming chunked responses, connection manager
- Binary & bit‑level handling → WAV header parser, bitfield flags byte, endianness helpers
- Data structures → hash table (sessions), LRU cache, ring/rate/priority queues
- Reliability → strict input validation, timeouts, graceful shutdown, signal handlers
- Performance → preallocation, cache‑friendly layouts, CPU affinity hints

## Core Modules

- http/: request parsing, routing, chunked streaming
- ai/: process orchestration, pipes, back‑pressure
- audio/: WAV parsing, ring buffer, format conversion stubs
- core/: session hash table, LRU cache, token bucket
- utils/: growable buffer, logging, assertions
- include/: public headers with stable ABI between modules

## Engineering Notes

- Memory safety: ASan/Valgrind clean for sustained runs
- Input validation: reject malformed HTTP early, bounded parsing
- Graceful lifecycle: SIGTERM handling, draining requests, metrics flush
- Portability: gcc/clang, Linux/macOS; minimal dependencies

## Security & Reliability

- Strict bounds checks; integer overflow guards
- Authentication hook ready (e.g., token header) though single‑user by default
- Rate limiting and per‑IP quotas via token bucket
- Structured logging with request IDs; metrics endpoint for SLOs

## Why this is relevant to systems engineering

- Demonstrates ownership of a low‑latency service with clear SLIs/SLOs
- Shows competency in memory management, IPC, and binary protocols
- Highlights practical performance work (preallocation, zero‑copy, ring buffers)
- Exhibits reliability discipline: graceful shutdown, back‑pressure, limits

## Containerization

- Multi‑stage Docker build (builder/runtime) with minimal runtime surface
- GPU passthrough supported (NVIDIA/ROCm)
- Healthcheck, signal handling, and volume mounts for models

## Roadmap (high level)

1. HTTP server and health endpoint
2. Chat endpoint wired to AI adapter (mock → llama.cpp)
3. Dynamic buffers, streaming responses, conversation state
4. Session hash table, LRU cache, rate limiting
5. Audio path: ring buffer + WAV parser
6. Observability and hardening (timeouts, shutdown)