# Modern C++ HTTP Server

[![CI](https://github.com/agomezdemen/http-server/actions/workflows/ci.yml/badge.svg)](https://github.com/agomezdemen/http-server/actions/workflows/ci.yml)

A from-scratch HTTP/1.1 server written in modern C++23 for Linux.

The project focuses on systems programming, explicit resource ownership, incremental protocol parsing, TCP networking, automated process-level testing, and eventually scalable nonblocking I/O with `epoll`.

## Current Status

The project currently implements a single-threaded blocking HTTP server capable of accepting TCP connections, incrementally parsing HTTP requests, and returning serialized HTTP responses.

Current milestone: **blocking HTTP server MVP**

- Linux TCP socket abstraction
- RAII file-descriptor ownership
- Incremental HTTP/1.1 request parser
- Request and response types
- HTTP response serialization
- Valid request and malformed-request handling
- Multiple sequential client connections
- Catch2 unit and process-level integration tests
- CTest test discovery
- GitHub Actions continuous integration

Routing, nonblocking I/O, multithreading, and TLS are planned milestones.

## Features

### Networking

- RAII-managed POSIX file descriptors
- IPv4 endpoint representation
- TCP listener abstraction
- TCP connection abstraction
- Complete-write handling for partial `send()` operations
- Peer endpoint inspection
- Graceful handling of clients that disconnect before sending a complete request

### HTTP

- HTTP method parsing
- HTTP version parsing
- Request-line validation
- Header parsing and validation
- Case-insensitive `Content-Length` handling
- Incremental parsing across multiple socket reads
- Request-body parsing based on `Content-Length`
- Detection of conflicting duplicate `Content-Length` headers
- HTTP response serialization with status line, headers, and body
- Automatic `Content-Length` generation
- `200 OK` and `400 Bad Request` responses

### Testing

The test suite includes both unit tests and black-box process-level integration tests.

Unit tests cover:

- File-descriptor ownership and move semantics
- TCP listener and connection behavior
- Request and response types
- HTTP grammar validation
- Incremental parser state transitions
- Fragmented requests
- Malformed requests
- Request bodies and `Content-Length`

Integration tests launch the real server executable with `fork()` and `exec()`, communicate with it through real TCP sockets, and terminate and reap the child process after each test.

Current integration coverage includes:

- Server startup and readiness
- Valid `GET` request returning `200 OK`
- Malformed request returning `400 Bad Request`
- Multiple sequential client connections

## Architecture

Production code is compiled into the reusable `server_core` library.

```text
server_core
├── server::platform
│   └── Fd
├── server::net
│   ├── Endpoint
│   ├── TcpListener
│   └── TcpConnection
└── server::http
    ├── Request
    ├── Response
    ├── RequestParser
    └── HTTP grammar and protocol types

http_server
└── links server_core

unit tests
├── Catch2
└── link server_core

integration tests
├── Catch2
├── launch the real http_server executable
└── communicate through TCP
```

The integration-test harness contains two primary components:

- `ServerProcess`, which manages the server process lifecycle
- `HttpClient`, which sends raw HTTP requests and receives raw responses

## Requirements

- Linux
- CMake 3.28 or newer
- A compiler with C++23 support
- Catch2 3
- Ninja when using the provided presets

## Build

Configure the prototype build:

```bash
cmake --preset prototype
```

Build the project:

```bash
cmake --build --preset prototype
```

## Run

Start the server by passing a port:

```bash
./out/build/prototype/http_server 8080
```

Send a request from another terminal:

```bash
curl -v http://127.0.0.1:8080/
```

The current server returns:

```text
Hello world!
```

## Test

Run the complete suite:

```bash
ctest --preset prototype --output-on-failure
```

Run only the process-level integration tests:

```bash
ctest --preset prototype -R "^integration:" --output-on-failure
```

Repeatedly run the integration suite using fresh test and server processes:

```bash
ctest --preset prototype -R "^integration:" --repeat until-fail:10 --output-on-failure
```

## Roadmap

### Milestone 1 — Blocking HTTP Server

- [x] RAII file-descriptor wrapper
- [x] TCP listener and connection abstractions
- [x] HTTP request and response types
- [x] Incremental HTTP parser
- [x] Unit tests
- [x] Process-level integration tests
- [x] Continuous integration
- [ ] Request routing
- [ ] Routing integration tests
- [ ] Blocking-server benchmark

### Milestone 2 — Nonblocking Event Loop

- [ ] Nonblocking sockets
- [ ] `epoll`-based event loop
- [ ] Per-connection parser and output state
- [ ] Partial-read and partial-write handling
- [ ] Connection timeouts
- [ ] Load and latency benchmarks

### Milestone 3 — Concurrency

- [ ] Worker thread pool
- [ ] Work distribution
- [ ] Thread-safe connection and task management
- [ ] Contention and scalability benchmarks

### Milestone 4 — TLS

- [ ] TLS connection abstraction
- [ ] Certificate configuration
- [ ] HTTPS integration tests
- [ ] TLS performance measurements

### Milestone 5 — Performance and Design Review

- [ ] Profiling and bottleneck analysis
- [ ] Allocation reduction
- [ ] Compile-time computation where appropriate
- [ ] `constexpr`, `noexcept`, and move-semantics review
- [ ] Architecture and design-pattern review

## Design Goals

The project intentionally avoids relying on an existing HTTP or networking framework. Its purpose is to explore the implementation details behind:

- Linux socket programming
- Process management
- Incremental protocol parsing
- Resource ownership
- Nonblocking I/O
- Event-driven server architecture
- Concurrency
- Performance measurement

Correctness and testability are established before introducing additional concurrency and performance complexity.

## Current Limitations

The current milestone is intentionally minimal:

- Blocking I/O
- Single-threaded request handling
- One request per connection
- No routing yet
- No persistent HTTP connections
- No chunked transfer encoding
- No `epoll`
- No TLS
- Not intended for production use
