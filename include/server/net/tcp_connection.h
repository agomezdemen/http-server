#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <span>
#include <string_view>

#include "server/net/endpoint.h"
#include "server/platform/fd.h"

namespace server::net {

// Owns one accepted stream socket and exposes blocking read/write operations.
class TcpConnection {
 private:
  Endpoint peer_;  // Connected peer address.
  platform::Fd cfd_;         // Connected socket descriptor.

 public:
  TcpConnection(platform::Fd cfd, Endpoint peer);

  TcpConnection(const TcpConnection&) = delete;
  auto operator=(const TcpConnection&) -> TcpConnection& = delete;

  TcpConnection(TcpConnection&&) noexcept = default;
  auto operator=(TcpConnection&&) noexcept -> TcpConnection& = default;

  ~TcpConnection() noexcept = default;

  auto read(std::span<char> buffer) -> std::size_t;
  auto write(std::string_view data) -> std::size_t;
  auto write_all(std::string_view data) -> std::size_t;

  auto fd() const noexcept -> int;
  auto peer_endpoint() const noexcept -> const Endpoint&;
  auto valid() const noexcept -> bool;
};

}  // namespace server::net

#endif  // !TCP_CONNECTION_H
