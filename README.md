# Modern C++ HTTP Server

A high-performance HTTP server written in modern C++ using Linux networking primitives. The project is currently focused on building a reliable single-threaded event-driven server using nonblocking sockets and epoll. The long-term goal is to evolve the server into a more complete systems project with strong architecture, testing, benchmarking, and eventually multithreading.

## Goals

The first goal of this project is to build a clean single-threaded HTTP server from the ground up using modern C++ and low-level Linux APIs.

Core goals:

- Use RAII to safely manage system resources.
- Use nonblocking sockets for network I/O.
- Use epoll for readiness-based event handling.
- Keep HTTP parsing separate from socket and event-loop logic.
- Build a small but testable architecture.
- Establish separate build configurations for prototyping, debugging, profiling, and release builds.

## Architecture Overview

The server is organized into several layers:

Application layer:
- Starts the server.
- Handles top-level configuration.
- Keeps main.cpp small.

Server layer:
- Owns the main server runtime.
- Manages the event loop.
- Tracks active client connections.

Networking layer:
- Wraps low-level Linux file descriptors.
- Manages sockets, listeners, and epoll.
- Keeps raw system calls isolated from higher-level server logic.

I/O layer:
- Provides buffer abstractions for reading from and writing to sockets.
- Helps handle partial reads and partial writes.

HTTP layer:
- Represents HTTP requests and responses.
- Parses raw request bytes into structured request objects.
- Serializes response objects into bytes that can be written to the client.

## Project Layout


```text
modern-http-server/
├── include/
│   └── http_server/
│       ├── net/
│       ├── io/
│       ├── http/
│       └── server/
├── src/
│   ├── main.cpp
│   ├── net/
│   ├── io/
│   ├── http/
│   └── server/
├── tests/
├── benchmarks/
├── docs/
├── cmake/
├── CMakeLists.txt
└── CMakePresets.json
```


```
```
## Main Components

### File Descriptor Management

The networking layer uses an RAII file descriptor wrapper to ensure that file descriptors are closed automatically. This helps prevent resource leaks and makes ownership clear.

### Listener

The listener owns the server socket. It is responsible for binding to an endpoint, listening for incoming clients, and accepting new connections.

### Epoll Wrapper

The epoll wrapper isolates Linux epoll operations from the rest of the server. It handles registering, modifying, removing, and waiting on file descriptor events.

### Event Loop

The event loop is the core of the single-threaded server. It waits for readiness events and dispatches work to the appropriate part of the server.

It handles:

- new incoming connections
- readable client sockets
- writable client sockets
- closed or failed connections

### Connection

A connection represents one active client. It owns the client socket, read buffer, write buffer, and HTTP parsing state.

### HTTP Parser

The parser is responsible for turning raw bytes into an HTTP request. It is designed to eventually support incremental parsing so that requests do not need to arrive in one complete read.

### HTTP Response

The response layer builds valid HTTP responses and serializes them into bytes that can be written back to the client.

## Build Configurations

This project uses CMake presets for different development modes.

Available build presets:

- prototype: fast development build with relaxed warnings
- debug: strict debug build with sanitizers
- profile: optimized build with debug symbols for profiling
- release: optimized release build

Example usage:

cmake --preset prototype
cmake --build --preset prototype

cmake --preset debug
cmake --build --preset debug

cmake --preset profile
cmake --build --preset profile

cmake --preset release
cmake --build --preset release

## Current Status

The project is currently in the early MVP stage.

Initial milestones:

- Set up project structure
- Set up CMake build presets
- Implement RAII file descriptor wrapper
- Implement listening socket
- Implement epoll wrapper
- Implement single-threaded event loop
- Return a hardcoded HTTP response
- Add basic HTTP request parsing
- Add tests for parser and response serialization
- Add benchmarks for request throughput

## Long-Term Direction

After the single-threaded server is reliable, future work may include:

- persistent connections
- more complete HTTP/1.1 support
- configurable routing
- static file serving
- better error handling
- benchmarking against existing servers
- multithreaded event loop design
- custom memory and buffer optimizations
