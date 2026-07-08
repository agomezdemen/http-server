#ifndef TCP_LISTENER_H
#define TCP_LISTENER_H
#include "../../platform/fd.h"
#include "endpoint.h"
#include "tcp_connection.h"

// Owns a listening TCP socket for one endpoint.
// Accepted clients are returned as Fd objects so ownership stays explicit.
class TcpListener {
 private:
  Endpoint ep_;
  Fd sfd_;

 public:
  explicit TcpListener(Endpoint ep, int backlog = 128);

  TcpListener(const TcpListener&) = delete;
  auto operator=(const TcpListener&) -> TcpListener& = delete;
  TcpListener(TcpListener&&) noexcept = default;
  auto operator=(TcpListener&&) noexcept -> TcpListener& = default;

  auto accept() -> TcpConnection;

  auto fd() const noexcept -> int;
  auto endpoint() const noexcept -> const Endpoint&;

  // Useful when binding to port 0 and letting the OS pick an open port.
  auto bound_port() const -> unsigned short;

  auto valid() const noexcept -> bool;
};

#endif  // !TCP_LISTENER
