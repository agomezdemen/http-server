# Architecture Notes

This project is organized around a small set of boundaries that should stay stable while the implementation evolves.

## Layers

- `net`: Linux file descriptors, sockets, listeners, and readiness notification.
- `io`: byte buffers and partial read/write support.
- `http`: request/response data structures, parsing, and serialization.
- `server`: connection lifecycle, event loop dispatch, and runtime configuration.

## Current Constraint

The first production-quality milestone is a single-threaded HTTP/1.1 server. Multithreading, TLS, routing frameworks, and static-file serving should wait until the single-threaded core is correct, tested, and benchmarked.

## Near-Term Vertical Slice

1. Create a listening socket.
2. Register it with epoll.
3. Accept one or more client sockets.
4. Read bytes into a connection buffer.
5. Parse one HTTP request.
6. Serialize one HTTP response.
7. Write the response and close the connection.
