#include "server/net/tcp_connection.h"

#include <sys/socket.h>

#include <cerrno>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <optional>

namespace server::net {

TcpConnection::TcpConnection(platform::Fd cfd, Endpoint peer)
    : peer_{std::move(peer)}, cfd_{std::move(cfd)} {}

auto TcpConnection::read(std::span<char> buffer) -> std::optional<std::size_t> {
  if(buffer.empty()) {
    throw std::invalid_argument{"read buffer must not be empty"};
  }

  ssize_t n;
  do{
    n = ::recv(cfd_.get(), buffer.data(), buffer.size(), 0);
  }while(n == -1 && errno == EINTR);

  if (n == -1) {
    const auto err{errno};

    if(err == EAGAIN || err == EWOULDBLOCK) {
      return std::nullopt;
    }

    throw std::system_error{err, std::generic_category(), "recv failed"};
  }

  return static_cast<std::size_t>(n);
}

auto TcpConnection::write(std::string_view data) -> std::optional<std::size_t> {
  if (data.empty()) return 0;

  ssize_t n;
  do{
    n = ::send(cfd_.get(), data.data(), data.size(), MSG_NOSIGNAL);
  }while(n == -1 && errno == EINTR);

  if (n == -1) {
    const auto err{errno};

    if(err == EAGAIN || err == EWOULDBLOCK) {
      return std::nullopt;
    }

    throw std::system_error{err, std::generic_category(), "send failed"};
  }

  return static_cast<std::size_t>(n);
}

auto TcpConnection::fd() const noexcept -> int { return cfd_.get(); }

auto TcpConnection::peer_endpoint() const noexcept -> const Endpoint& { return peer_; }

auto TcpConnection::valid() const noexcept -> bool { return cfd_.valid(); }

}  // namespace server::net
