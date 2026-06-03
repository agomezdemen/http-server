#include "../include/server/net/tcp_listener.h"
#include "../include/server/net/endpoint.h"
#include "../include/platform/fd.h"
#include <arpa/inet.h>
#include <catch2/catch_test_macros.hpp>
#include <cerrno>
#include <netinet/in.h>
#include <string_view>
#include <sys/socket.h>
#include <type_traits>
#include <unistd.h>
#include <utility>

namespace {

sockaddr_in loopback_addr(unsigned short port) {
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  REQUIRE(::inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) == 1);

  return addr;
}

Fd connected_client(const TcpListener& listener) {
  Fd client_fd{::socket(AF_INET, SOCK_STREAM, 0)};
  REQUIRE(client_fd.valid());

  const sockaddr_in addr{loopback_addr(listener.bound_port())};

  REQUIRE(::connect(
        client_fd.get(),
        reinterpret_cast<const sockaddr*>(&addr),
        static_cast<socklen_t>(sizeof(addr))) != -1);

  return client_fd;
}

} // namespace

TEST_CASE("Constructor succeeds with valid localhost endpoint") {
  Endpoint ep{"127.0.0.1", 0};
  TcpListener listener{ep};

  REQUIRE(listener.valid());
}

TEST_CASE("Invalid backlog throws") {
  Endpoint ep{"127.0.0.1", 0};

  REQUIRE_THROWS_AS(
      ([&] { TcpListener listener{ep, 0}; }()),
      std::invalid_argument);

  REQUIRE_THROWS_AS(
      ([&] { TcpListener listener{ep, -1}; }()),
      std::invalid_argument);

}

TEST_CASE("Invalid IPv4 address throws") {
  Endpoint ep_1{"999.999.999.999", 0};
  Endpoint ep_2{"not an ip", 0};
  Endpoint ep_3{"localhost", 0};

  // Host names are not resolved yet; the listener only accepts IPv4 literals.
  REQUIRE_THROWS_AS(
      [&] { TcpListener{ep_1}; }(),
      std::runtime_error);
  
  REQUIRE_THROWS_AS(
      [&] { TcpListener{ep_2}; }(),
      std::runtime_error);
  
  REQUIRE_THROWS_AS(
      [&] { TcpListener{ep_3}; }(),
      std::runtime_error);

}

TEST_CASE("Binding to the same port twice fails") {
  Endpoint ep{"127.0.0.1", 0};
  TcpListener listener_1{ep};

  // Use the real port chosen for listener_1, since port 0 means "pick one".
  Endpoint bound_ep{"127.0.0.1", listener_1.bound_port()};

  REQUIRE_THROWS_AS(
      ([&] { TcpListener listener_2{bound_ep}; }()),
      std::system_error);

}

TEST_CASE("Accept returns a valid client") {
  Endpoint ep{"127.0.0.1", 0};
  TcpListener listener{ep};

  Fd client_fd{connected_client(listener)};

  Fd accepted_fd{listener.accept()};

  REQUIRE(accepted_fd.valid());
}


TEST_CASE("Accepted socket can communicate") {
  Endpoint ep{"127.0.0.1", 0};
  TcpListener listener{ep};

  Fd client_fd{connected_client(listener)};

  Fd accepted_fd{listener.accept()};

  REQUIRE(accepted_fd.valid());

  // Check both directions so the accepted descriptor is proven to be a live socket.
  ::write(client_fd.get(), "hello", 5);

  char buffer[6]{};
  auto bytes_read = ::read(accepted_fd.get(), buffer, 5);
  
  REQUIRE(bytes_read == 5);
  REQUIRE(std::string_view{buffer, 5} == "hello");

  ::write(accepted_fd.get(), "world", 5);

  char response[6]{};
  auto response_read = ::read(client_fd.get(), response, 5);

  REQUIRE(response_read == 5);
  REQUIRE(std::string_view{response, 5} == "world");
}

TEST_CASE("Move behavior works indirectly") {
  Endpoint ep{"127.0.0.1", 0};
  TcpListener init_listener{ep};

  TcpListener moved_listener{std::move(init_listener)};

  REQUIRE_FALSE(init_listener.valid());
  REQUIRE(moved_listener.valid());

  init_listener = std::move(moved_listener);

  REQUIRE(init_listener.valid());
  REQUIRE_FALSE(moved_listener.valid());
}

TEST_CASE("Not copy constructible") {
  static_assert(!std::is_copy_constructible_v<TcpListener>);
  static_assert(!std::is_copy_assignable_v<TcpListener>);
}
