#include "../../include/server/net/tcp_listener.h"
#include <arpa/inet.h>
#include <cerrno>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <utility>

TcpListener::TcpListener(Endpoint ep, int backlog)
    : ep_{std::move(ep)}, sfd_{::socket(AF_INET, SOCK_STREAM, 0)} {
  
  if(backlog <= 0) {
    throw std::invalid_argument{"backlog must be positive"};
  }

  if (!sfd_.valid()) {
    const int err{errno};
    throw std::system_error{err, std::generic_category(), "socket failed"};
  }

  int opt{1};

  // Allow quick restarts while tests and local dev builds reuse the same address.
  if (::setsockopt(
          sfd_.get(),
          SOL_SOCKET,
          SO_REUSEADDR,
          &opt,
          static_cast<socklen_t>(sizeof(opt))) == -1) {
    const int err{errno};
    throw std::system_error{err, std::generic_category(), "setsockopt failed"};
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(ep_.port());

  // For now the listener accepts IPv4 literals only. Name resolution can be added later.
  const int ip_result{
      ::inet_pton(AF_INET, ep_.host().c_str(), &addr.sin_addr)
  };

  if (ip_result == 0) {
    throw std::runtime_error{"invalid IPv4 address"};
  }

  if (ip_result == -1) {
    const int err{errno};
    throw std::system_error{err, std::generic_category(), "inet_pton failed"};
  }

  if (::bind(
          sfd_.get(),
          reinterpret_cast<const sockaddr*>(&addr),
          static_cast<socklen_t>(sizeof(addr))) == -1) {
    const int err{errno};
    throw std::system_error{err, std::generic_category(), "bind failed"};
  }

  if (::listen(sfd_.get(), backlog) == -1) {
    const int err{errno};
    throw std::system_error{err, std::generic_category(), "listen failed"};
  }
}

auto TcpListener::accept() -> TcpConnection {
  sockaddr_in client_addr{};
  socklen_t client_len{sizeof(client_addr)};

  const auto raw_client_fd{
      ::accept(sfd_.get(), reinterpret_cast<sockaddr*>(&client_addr), &client_len)};

  if (raw_client_fd == -1) {
    const auto err{errno};
    throw std::system_error{err, std::generic_category(), "accept failed"};
  }

  Fd client_fd{raw_client_fd};

  char ip_buffer[INET_ADDRSTRLEN]{};

  if (::inet_ntop(AF_INET, &client_addr.sin_addr, ip_buffer, sizeof(ip_buffer)) ==
      nullptr) {
    const auto err{errno};
    ::close(client_fd.release());
    throw std::system_error{err, std::generic_category(), "inet_ntop failed"};
  }

  Endpoint peer{
      std::string{ip_buffer},
      ntohs(client_addr.sin_port)
  };

  return TcpConnection{Fd{std::move(client_fd)}, std::move(peer)};
}


auto TcpListener::fd() const noexcept -> int {
  return sfd_.get();
}

auto TcpListener::endpoint() const noexcept -> const Endpoint& {
  return ep_;
}

auto TcpListener::bound_port() const -> unsigned short {
  sockaddr_in addr{};
  socklen_t addr_len{sizeof(addr)};

  // When the requested port is 0, getsockname tells us which port the OS chose.
  if (::getsockname(sfd_.get(), reinterpret_cast<sockaddr*>(&addr), &addr_len) == -1) {
    const int err{errno};
    throw std::system_error{err, std::generic_category(), "getsockname failed"};
  }

  return ntohs(addr.sin_port);
}

auto TcpListener::valid() const noexcept -> bool {
  return sfd_.valid();
}
