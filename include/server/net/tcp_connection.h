#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "../../platform/fd.h"
#include "endpoint.h"

#include <span>
#include <string_view>

class TcpConnection {
private:
  Fd cfd_; // connection fd
  Endpoint peer_; // connected peer

public:
  TcpConnection(Fd cfd, Endpoint peer);

  TcpConnection(const TcpConnection&) = delete;
  auto operator=(const TcpConnection&) -> TcpConnection& = delete;

  TcpConnection(TcpConnection&&) noexcept = default;
  auto operator=(TcpConnection&&) noexcept -> TcpConnection& = default;
  
  ~TcpConnection() noexcept = default;

  auto read(std::span<char> buffer) -> std::size_t;
  auto write(std::string_view data) -> std::size_t;

  auto fd() const noexcept -> int;
  auto peer_endpoint() const noexcept -> const Endpoint&;
  auto valid() const noexcept -> bool;
};

#endif // !TCP_CONNECTION_H
