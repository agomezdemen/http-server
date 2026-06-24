#include "../../include/server/net/tcp_connection.h"

#include <cerrno>
#include <sys/socket.h>
#include <system_error>

TcpConnection::TcpConnection(Fd cfd, Endpoint peer) : cfd_{std::move(cfd)}, peer_{std::move(peer)} {}

auto TcpConnection::read(std::span<char> buffer) -> std::size_t {
  auto n{::recv(cfd_.get(), buffer.data(), buffer.size(), 0)};

  if(n == -1) {
    const auto err{errno};
    throw std::system_error{err, std::generic_category(), "recv failed"};
  }

  return static_cast<std::size_t>(n);
}

auto TcpConnection::write(std::string_view data) -> std::size_t {
  if(data.empty())
    return 0;

  auto n{::send(cfd_.get(), data.data(), data.size(), MSG_NOSIGNAL)};

  if(n == -1) {
    const auto err{errno};
    throw std::system_error{err, std::generic_category(), "send failed"};
  }

  return static_cast<std::size_t>(n);
}

auto TcpConnection::write_all(std::string_view data) -> std::size_t {
  auto total_written{0uz};

  while (!data.empty()) {
    const auto n{
        ::send(cfd_.get(), data.data(), data.size(), MSG_NOSIGNAL)};

    if (n == -1) {
      const auto err{errno};
      throw std::system_error{err, std::generic_category(), "send failed"};
    }

    if (n == 0)
      throw std::runtime_error{"send wrote 0 bytes"};

    const auto bytes_written{static_cast<std::size_t>(n)};

    total_written += bytes_written;
    data.remove_prefix(bytes_written);
  }

  return total_written;
}

auto TcpConnection::fd() const noexcept -> int {
  return cfd_.get();
}

auto TcpConnection::peer_endpoint() const noexcept -> const Endpoint& {
  return peer_;
}

auto TcpConnection::valid() const noexcept -> bool {
  return cfd_.valid();
}
