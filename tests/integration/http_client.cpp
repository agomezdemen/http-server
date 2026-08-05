#include "http_client.h"
#include <sys/socket.h>
#include <system_error>
#include <arpa/inet.h>
#include <stdexcept>
#include <unistd.h>
#include <cerrno>
#include <string>
#include <array>

namespace test::integration {

  HttpClient::HttpClient(std::string_view host, std::uint16_t port) {
    const auto fd{::socket(AF_INET, SOCK_STREAM, 0)};

    if(fd == -1)
      throw std::system_error{errno, std::generic_category(), "socket failed"};

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    const std::string host_str{host};

    if(::inet_pton(AF_INET, host_str.c_str(), &address.sin_addr) != 1) {
      ::close(fd);
      throw std::invalid_argument{"invalid IPv4 address"};
    }

    if(::connect(fd, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == -1) {
      const auto error{errno};
      ::close(fd);
      throw std::system_error{error, std::generic_category(), "connect failed"};
    }

    sfd_ = fd;
  }

  HttpClient::~HttpClient() {
    if(sfd_ != -1)
      ::close(sfd_);
  }

  auto HttpClient::send_request(std::string_view request) -> std::string {
    send_all(request);

    if(::shutdown(sfd_, SHUT_WR) == -1)
      throw std::system_error{errno, std::generic_category(), "shutdown failed"};

    return receive_all();
  }

  auto HttpClient::send_all(std::string_view bytes) -> void {
    std::size_t total_sent{};

    while(total_sent < bytes.size()) {
      const auto sent{::send(sfd_, bytes.data() + total_sent, bytes.size() - total_sent, MSG_NOSIGNAL)};

      if(sent == -1) {
        if(errno == EINTR)
          continue;

        throw std::system_error{errno, std::generic_category(), "send failed"};
      }

      if(sent == 0)
        throw std::runtime_error{"send made no progress"};

      total_sent += static_cast<std::size_t>(sent);
    }
  }

  auto HttpClient::receive_all() -> std::string {
    std::string response{};
    std::array<char, 4096> buffer{};

    while(true) {
      const auto received{::recv(sfd_, buffer.data(), buffer.size(), 0)};

      if(received == 0)
        break;

      if(received == -1) {
        if(errno == EINTR)
          continue;

        throw std::system_error{errno, std::generic_category(), "recv failed"};
      }

      response.append(buffer.data(), static_cast<std::size_t>(received));
    }

    return response;
  }

}
