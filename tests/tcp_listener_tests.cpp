#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cerrno>
#include <span>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>

#include "../include/platform/fd.h"
#include "../include/server/net/endpoint.h"
#include "../include/server/net/tcp_connection.h"
#include "../include/server/net/tcp_listener.h"

namespace {

// Build a loopback sockaddr for client sockets that connect to test listeners.
auto loopback_addr(unsigned short port) -> sockaddr_in {
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  REQUIRE(::inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) == 1);

  return addr;
}

auto connected_client(const TcpListener& listener) -> Fd {
  // Connect a real client socket so listener.accept() can return immediately.
  Fd client_fd{::socket(AF_INET, SOCK_STREAM, 0)};
  REQUIRE(client_fd.valid());

  const sockaddr_in addr{loopback_addr(listener.bound_port())};

  REQUIRE(::connect(client_fd.get(), reinterpret_cast<const sockaddr*>(&addr),
                    static_cast<socklen_t>(sizeof(addr))) != -1);

  return client_fd;
}

}  // namespace

TEST_CASE("Constructor succeeds with valid localhost endpoint") {
  Endpoint ep{"127.0.0.1", 0};
  TcpListener listener{ep};

  REQUIRE(listener.valid());
}

TEST_CASE("Invalid backlog throws") {
  Endpoint ep{"127.0.0.1", 0};

  REQUIRE_THROWS_AS(([&] { TcpListener listener{ep, 0}; }()), std::invalid_argument);

  REQUIRE_THROWS_AS(([&] { TcpListener listener{ep, -1}; }()), std::invalid_argument);
}

TEST_CASE("Invalid IPv4 address throws") {
  Endpoint ep_1{"999.999.999.999", 0};
  Endpoint ep_2{"not an ip", 0};
  Endpoint ep_3{"localhost", 0};

  // Host names are not resolved yet; the listener only accepts IPv4 literals.
  REQUIRE_THROWS_AS([&] { TcpListener{ep_1}; }(), std::runtime_error);

  REQUIRE_THROWS_AS([&] { TcpListener{ep_2}; }(), std::runtime_error);

  REQUIRE_THROWS_AS([&] { TcpListener{ep_3}; }(), std::runtime_error);
}

TEST_CASE("Binding to the same port twice fails") {
  Endpoint ep{"127.0.0.1", 0};
  TcpListener listener_1{ep};

  // Use the real port chosen for listener_1, since port 0 means "pick one".
  Endpoint bound_ep{"127.0.0.1", listener_1.bound_port()};

  REQUIRE_THROWS_AS(([&] { TcpListener listener_2{bound_ep}; }()), std::system_error);
}

TEST_CASE("Accept returns a usable TcpConnection") {
  Endpoint ep{"127.0.0.1", 0};
  TcpListener listener{ep};

  Fd client_fd{connected_client(listener)};

  TcpConnection accepted_conn{listener.accept()};

  const auto bytes_written{accepted_conn.write("x")};
  REQUIRE(bytes_written == 1);

  std::array<char, 2> response{};
  const auto bytes_read{::read(client_fd.get(), response.data(), 1)};

  REQUIRE(bytes_read == 1);
  REQUIRE(std::string_view{response.data(), 1} == "x");
}

TEST_CASE("Accepted TcpConnection can communicate") {
  Endpoint ep{"127.0.0.1", 0};
  TcpListener listener{ep};

  Fd client_fd{connected_client(listener)};

  TcpConnection accepted_conn{listener.accept()};

  // Client -> accepted connection
  REQUIRE(::write(client_fd.get(), "hello", 5) == 5);

  std::array<char, 6> buffer{};
  const auto bytes_read{accepted_conn.read(std::span<char>{buffer.data(), 5})};

  REQUIRE(bytes_read == 5);
  REQUIRE(std::string_view{buffer.data(), 5} == "hello");

  // Accepted connection -> client
  const auto bytes_written{accepted_conn.write("world")};
  REQUIRE(bytes_written == 5);

  std::array<char, 6> response{};
  const auto response_read{::read(client_fd.get(), response.data(), 5)};

  REQUIRE(response_read == 5);
  REQUIRE(std::string_view{response.data(), 5} == "world");
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
