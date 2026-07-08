# Modern C++ HTTP Server

A small HTTP server project written in C++23 on top of Linux socket APIs. The
current codebase is focused on building the low-level pieces first: file
descriptor ownership, TCP endpoints, listeners, accepted connections, and HTTP
response serialization.

The executable currently accepts one connection at a time on `127.0.0.1:8080`,
prints the received request bytes, and writes a fixed `Hello world!` response.
The event-loop, parser, buffer, and higher-level server layers are still
scaffolded for future work.

## Goals

- Use RAII to make POSIX file descriptor ownership explicit.
- Keep socket code isolated from HTTP request and response code.
- Build each layer behind small, testable interfaces.
- Use CMake presets for prototype, debug, profile, and release builds.
- Grow toward an event-driven HTTP/1.1 server without hiding the Linux APIs.

## Project Layout

```text
modern-http-server/
├── include/
│   ├── platform/
│   └── server/
│       ├── http/
│       ├── io/
│       ├── net/
│       └── server/
├── src/
│   ├── http/
│   ├── io/
│   ├── net/
│   ├── platform/
│   └── server/
├── tests/
├── benchmarks/
├── cmake/
├── docs/
├── CMakeLists.txt
└── CMakePresets.json
```

## Implemented Pieces

- `Fd`: movable RAII wrapper for POSIX file descriptors.
- `Endpoint`: stores host and port values used by listener and connection code.
- `TcpListener`: creates, binds, listens, accepts, and reports the bound port.
- `TcpConnection`: reads, writes, and writes complete buffers to a peer socket.
- `server::http::Response`: serializes HTTP version, status, headers, content
  length, and body.
- `server::http` types: shared method, status, version, and header definitions.

## In Progress

- `server::http::Request` has its data model declared, but behavior is not
  implemented yet.
- HTTP parser, I/O buffer, epoll, server, event loop, and server connection
  files are present as scaffolding.
- `main.cpp` is still a simple blocking prototype, not the final event-loop
  architecture.
- Benchmarks are wired as an optional build area, but benchmark targets are not
  implemented yet.

## Build And Test

Configure and build the prototype preset:

```sh
cmake --preset prototype
cmake --build --preset prototype
```

Run the tests:

```sh
ctest --preset prototype
```

Available build presets:

- `prototype`: fast debug build with relaxed warnings.
- `debug`: strict debug build with warnings-as-errors and sanitizers.
- `profile`: optimized build with debug symbols.
- `release`: optimized release build with LTO enabled when supported.

## Formatting

The project uses `clang-format` with Google-derived C++ formatting settings.
Format C++ sources and headers with:

```sh
clang-format -i $(find include src tests -name '*.h' -o -name '*.cpp')
```

## Current Status

The prototype build currently compiles with C++23 and the active CTest suite
covers the implemented file descriptor, endpoint, listener, connection, and
HTTP response behavior.

Some tests open local sockets. If they fail with `Operation not permitted` in a
sandboxed environment, rerun them in an environment that allows loopback/socket
operations.

## Long-Term Direction

- Implement request parsing and request serialization support.
- Add nonblocking sockets and epoll-backed readiness handling.
- Move the prototype `main.cpp` flow into the server/event-loop layer.
- Support persistent HTTP/1.1 connections and better error responses.
- Add routing, static file serving, and benchmarking once the core runtime is
  stable.
