# Modern C++ Linux HTTP Server

A from-scratch HTTP/1.1 server written in modern C++ for Linux.

This project uses an HTTP server as a practical environment for studying Linux systems programming: sockets, file descriptors, process behavior, incremental network protocols, event-driven I/O, concurrency, diagnostics, and performance analysis.

The implementation begins as a small blocking server and is being developed incrementally toward a non-blocking, `epoll`-based, multithreaded architecture. Each stage is tested and benchmarked before additional complexity is introduced.

> **Status:** The blocking prototype can accept TCP connections, incrementally parse HTTP requests, and return valid HTTP responses. Automated end-to-end integration testing is the current development focus.

## Engineering Goals

This project is designed to develop and demonstrate the ability to:

* Build reliable software directly on Linux system interfaces
* Reason about file-descriptor ownership and resource lifetimes
* Handle partial reads, partial writes, disconnects, and malformed input
* Understand how application behavior maps to socket and kernel behavior
* Build automation for repeatable integration and stress testing
* Diagnose failures using Linux introspection tools
* Measure throughput, latency, CPU usage, allocation behavior, and system-call activity
* Progress from a simple implementation to an event-driven concurrent system
* Document failures, tradeoffs, and performance findings

## Current Capabilities

### Linux Networking

* Creates TCP sockets using Linux socket APIs
* Configures listening sockets with `SO_REUSEADDR`
* Binds and listens on an IPv4 endpoint
* Accepts incoming client connections
* Retrieves connected peer addresses with `getpeername`
* Reads request data without assuming that socket reads align with HTTP boundaries
* Retries interrupted operations where appropriate
* Handles partial socket writes through a `write_all` loop
* Prevents `SIGPIPE` during writes with `MSG_NOSIGNAL`
* Reports operating-system failures through `std::system_error`

### Resource Management

The project includes a move-only RAII wrapper for Linux file descriptors.

`platform::Fd` provides explicit ownership of an operating-system resource and ensures that:

* Owned descriptors are closed exactly once
* File descriptors cannot be accidentally copied
* Ownership can be transferred with move operations
* Moved-from objects no longer own the resource
* Resource cleanup occurs during normal execution and exception unwinding

This ownership model is used by the networking abstractions rather than exposing unmanaged integer descriptors throughout the codebase.

### HTTP Processing

* HTTP method and version representations
* HTTP status codes and reason phrases
* Request and response objects
* Header storage and replacement
* HTTP response serialization
* Request-line parsing
* Header-name and header-value validation
* `Content-Length` parsing
* Request-body parsing
* Incremental parsing across arbitrary input fragments
* Detection of incomplete and malformed requests

### Incremental Request Parser

`http::RequestParser` is implemented as a state machine with an internal byte buffer.

It does not assume that:

* One socket read contains one HTTP request
* A request line arrives in one read
* A header arrives in one read
* Header and body boundaries align with reads
* The complete body is immediately available

The parser preserves incomplete data between calls and transitions through request-line, header, body, completion, and error states.

HTTP grammar operations are separated from parser state management so they can be tested independently.

## Architecture

```text
Client
  |
  | TCP
  v
TcpListener
  |
  v
TcpConnection
  |
  | byte  v fragments
  v
RequestParser
  |
  v
Request
  |
  v
Request Handler / Router
  |
  v
Response
  |
  | serialized HTTP
  v
TcpConnection::write_all
```

### `platform::Fd`

Move-only ownership wrapper for Linux file descriptors.

### `net::Endpoint`

Represents an IP address and port independently from the underlying socket structures.

### `net::TcpListener`

Responsible for creating, configuring, binding, and listening on a TCP socket.

### `net::TcpConnection`

Represents an accepted client connection and provides read, write, and peer-inspection operations.

### `http::RequestParser`

Consumes arbitrary byte fragments and constructs a structured request when enough valid input has arrived.

### `http::Request`

Stores the request method, target, HTTP version, headers, and optional body.

### `http::Response`

Stores the response status, headers, and body and serializes them into HTTP wire format.

## Reliability and Failure Handling

The server is being developed around explicit failure cases rather than only the successful request path.

Current and planned test cases include:

* Empty socket reads
* Interrupted system calls
* Partial writes
* Client disconnects
* Incomplete request lines
* Headers split across several reads
* Bodies split across several reads
* Invalid methods and HTTP versions
* Invalid header names
* Invalid `Content-Length` values
* Bodies shorter than the declared length
* Multiple concurrent connections
* Slow or stalled clients
* Resource cleanup after failures

Errors are represented at the appropriate layer:

* Linux API failures use error codes and `std::system_error`
* HTTP grammar failures use structured parser errors
* Incomplete input is distinguished from invalid input
* Resource cleanup remains automatic during exceptions

## Testing

The project uses Catch2 and CTest.

Current test coverage includes:

* File-descriptor ownership and move behavior
* Listener and connection behavior
* HTTP request objects
* HTTP response serialization
* Request-line grammar
* Header grammar
* Header-name validation
* Incremental request parsing
* Invalid and incomplete requests

Run the test suite with:

```bash
ctest --preset prototype
```

Manual end-to-end testing is performed with tools such as `curl`:

```bash
curl -v http://127.0.0.1:8080/
```

Automated integration tests are being added to:

1. Start the server as a child process
2. Wait until the listening socket is ready
3. Connect over TCP
4. Send fragmented and complete HTTP requests
5. Validate the complete response
6. Detect crashes, hangs, and premature exits
7. Shut down the server cleanly
8. Run the scenario repeatedly to expose nondeterministic failures

## Linux Diagnostics

The server is intended to be inspectable using standard Linux tools.

Tools used or planned include:

* `strace` for system-call tracing
* `ss` for socket and TCP-state inspection
* `lsof` for file-descriptor inspection
* `/proc` for process and descriptor state
* `gdb` for debugging
* `perf` for CPU profiling and hardware performance counters
* AddressSanitizer for memory-safety errors
* UndefinedBehaviorSanitizer for undefined behavior
* ThreadSanitizer after concurrency is introduced
* `valgrind` where appropriate
* `wrk` for HTTP load generation

Performance findings and failure investigations will be recorded in the repository rather than only reporting final benchmark numbers.

## Performance Methodology

The blocking implementation will establish a correctness and performance baseline before non-blocking I/O is introduced.

Measurements will include:

* Requests per second
* Median latency
* Tail latency
* CPU utilization
* Context switches
* System calls per request
* Allocation frequency
* Memory usage
* Behavior as connection concurrency increases

Planned comparisons include:

* Blocking sockets versus non-blocking sockets
* Single-threaded versus multithreaded execution
* Level-triggered versus edge-triggered `epoll`
* Different read-buffer strategies
* Different work-distribution strategies
* Copying versus view-based parsing
* Behavior under slow-client and fragmented-request workloads

Benchmark commands, hardware details, compiler settings, and raw results will be included so measurements can be reproduced.

## Build

### Requirements

* Linux
* CMake
* Ninja or another supported CMake generator
* A compiler with C++23 support
* Catch2

### Configure and Build

```bash
cmake --preset prototype
cmake --build --preset prototype
```

### Run Tests

```bash
ctest --preset prototype
```

### Run the Server

Run the server executable and connect from another terminal:

```bash
curl -v http://127.0.0.1:8080/
```

## Development Roadmap

### Milestone 1: Reliable Blocking Server

* [x] Move-only file-descriptor ownership
* [x] TCP listener abstraction
* [x] TCP connection abstraction
* [x] Complete socket-write handling
* [x] HTTP request and response types
* [x] Response serialization
* [x] Request-line parsing
* [x] Header parsing and validation
* [x] Request-body parsing
* [x] Incremental request parser
* [x] Parser unit tests
* [x] Manual end-to-end testing
* [ ] Automated process-level integration tests
* [ ] Graceful server shutdown
* [ ] Initial blocking-server benchmark
* [ ] Initial system-call trace and analysis

### Milestone 2: Routing and File I/O

* [ ] Method-aware router
* [ ] Exact-path routing
* [ ] Static-file serving
* [ ] Safe path normalization
* [ ] Directory-traversal protection
* [ ] `404 Not Found` handling
* [ ] `405 Method Not Allowed` handling
* [ ] File-read error handling
* [ ] Routing integration tests
* [ ] File-I/O profiling

### Milestone 3: Non-Blocking Linux Event Loop

* [ ] Configure sockets with `O_NONBLOCK`
* [ ] `epoll`-based event loop
* [ ] Per-connection state
* [ ] Partial-read handling
* [ ] Partial-write queues
* [ ] Read and write readiness transitions
* [ ] Connection timeouts
* [ ] Peer-disconnect handling
* [ ] File-descriptor exhaustion handling
* [ ] Slow-client testing
* [ ] Concurrent-connection stress testing

### Milestone 4: Concurrency and Operations

* [ ] Worker thread pool
* [ ] Bounded work queues
* [ ] Thread-safe task scheduling
* [ ] Load shedding and resource limits
* [ ] Structured logging
* [ ] Runtime health statistics
* [ ] Graceful shutdown through Unix signals
* [ ] Automated diagnostic collection after failures
* [ ] ThreadSanitizer validation
* [ ] Fault-injection tests

### Milestone 5: Performance Investigation

* [ ] Publish reproducible `wrk` benchmarks
* [ ] Profile with `perf`
* [ ] Analyze system calls with `strace`
* [ ] Inspect cache and branch behavior
* [ ] Reduce unnecessary allocations
* [ ] Reduce unnecessary data copying
* [ ] Improve buffer reuse
* [ ] Evaluate memory-pooling strategies
* [ ] Measure context switches and scheduling behavior
* [ ] Investigate latency outliers
* [ ] Document unsuccessful optimizations and tradeoffs

### Milestone 6: Protocol and Security Features

* [ ] Persistent connections
* [ ] Configurable connection limits
* [ ] Configurable request-size limits
* [ ] Request timeouts
* [ ] Additional HTTP method handling
* [ ] TLS
* [ ] Fuzz testing
* [ ] Security review

## Current Limitations

This project is under active development and is not production-ready.

The current implementation does not yet provide:

* Non-blocking I/O
* `epoll`
* Concurrent request processing
* Persistent connections
* Request pipelining
* Chunked transfer decoding
* TLS
* Production-grade observability
* Comprehensive security hardening
* Complete HTTP/1.1 compliance

These limitations are documented explicitly so that completed functionality can be distinguished from planned work.

## Engineering Notes

The repository will include short technical notes covering topics such as:

* Why TCP reads cannot be treated as message boundaries
* File-descriptor ownership and move-only types
* Handling partial writes correctly
* Parser state-machine invariants
* Blocking versus non-blocking socket behavior
* `epoll` readiness semantics
* Debugging connection failures with `strace` and `ss`
* Measuring system calls per request
* Investigating latency outliers with `perf`
* Design decisions that were reconsidered after testing

The goal is to preserve the reasoning behind the implementation, including mistakes and failed approaches, rather than only presenting the final design.

## What This Project Demonstrates

* Linux command-line development
* Linux socket programming
* Operating-system resource management
* TCP and HTTP fundamentals
* Process-level integration testing
* Incremental protocol parsing
* State-machine design
* Failure-oriented testing
* Systems troubleshooting
* Performance measurement
* Event-driven architecture
* Concurrent software design
* Modern C++ ownership and type safety
* Technical communication and documentation
