#include "../../include/server/net/tcp_listener.h"
#include <arpa/inet.h>
#include <cerrno>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <system_error>
#include <utility>

TcpListener::TcpListener(Endpoint ep, int backlog)
    : sfd_{::socket(AF_INET, SOCK_STREAM, 0)}, ep_{std::move(ep)} {
  
  if(backlog <= 0)
    throw std::invalid_argument{"backlog must be positive"};

  if (!sfd_.valid()) {
    const int err{errno};
    throw std::system_error{err, std::generic_category(), "socket failed"};
  }

  int opt{1};

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

auto TcpListener::accept() -> Fd {
  const int client_fd{::accept(sfd_.get(), nullptr, nullptr)};

  if(client_fd == -1) {
    const int err{errno};
    throw std::system_error{err, std::generic_category(), "accept failed"};
  }

  return Fd{client_fd};
}

auto TcpListener::fd() const noexcept -> int {
  return sfd_.get();
}

auto TcpListener::endpoint() const noexcept -> const Endpoint& {
  return ep_;
}
