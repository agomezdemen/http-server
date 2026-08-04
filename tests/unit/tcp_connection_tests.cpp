#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <span>
#include <string_view>
#include <system_error>
#include <utility>

#include "server/net/endpoint.h"
#include "server/net/tcp_connection.h"
#include "server/platform/fd.h"

namespace net = server::net;
namespace platform = server::platform;

namespace {

// Socket pairs let connection tests exercise real send/recv behavior locally.
auto make_socket_pair() -> std::pair<platform::Fd, platform::Fd> {
  int sockets[2]{-1, -1};

  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
    throw std::system_error{errno, std::generic_category(), "socketpair failed"};
  }

  return {platform::Fd{sockets[0]}, platform::Fd{sockets[1]}};
}

auto fake_peer() -> net::Endpoint {
  // TcpConnection stores peer metadata even when the socket pair is local.
  return net::Endpoint{"127.0.0.1", 8080};
}

}  // namespace

TEST_CASE("TcpConnection can read bytes sent by peer", "[unit][tcp_connection]") {
  auto [conn_fd, peer_fd] = make_socket_pair();

  net::TcpConnection conn{std::move(conn_fd), fake_peer()};

  constexpr std::string_view msg{"hello"};

  REQUIRE(::send(peer_fd.get(), msg.data(), msg.size(), 0) == static_cast<ssize_t>(msg.size()));

  std::array<char, 16> buffer{};

  const auto bytes_read{conn.read(buffer)};

  REQUIRE(bytes_read == msg.size());
  REQUIRE(std::string_view{buffer.data(), bytes_read} == msg);
}

TEST_CASE("TcpConnection can write bytes to peer", "[unit][tcp_connection]") {
  auto [conn_fd, peer_fd] = make_socket_pair();

  net::TcpConnection conn{std::move(conn_fd), fake_peer()};

  constexpr std::string_view msg{"hello from connection"};

  const auto bytes_written{conn.write(msg)};

  REQUIRE(bytes_written == msg.size());

  std::array<char, 64> buffer{};

  const auto bytes_read{::recv(peer_fd.get(), buffer.data(), buffer.size(), 0)};

  REQUIRE(bytes_read == static_cast<ssize_t>(msg.size()));
  REQUIRE(std::string_view{buffer.data(), static_cast<std::size_t>(bytes_read)} == msg);
}

TEST_CASE("TcpConnection read returns 0 when peer closes connection", "[unit][tcp_connection]") {
  net::TcpConnection conn{platform::Fd{-1}, fake_peer()};

  std::array<char, 16> buffer{};

  REQUIRE_THROWS_AS(conn.read(buffer), std::system_error);
}

TEST_CASE("TcpConnection write throws when fd is invalid", "[unit][tcp_connection]") {
  net::TcpConnection conn{platform::Fd{-1}, fake_peer()};

  REQUIRE_THROWS_AS(conn.write("hello"), std::system_error);
}

TEST_CASE("TcpConnection write_all writes complete message", "[unit][tcp_connection]") {
  auto [client_fd, server_fd] = make_socket_pair();

  net::TcpConnection conn{std::move(server_fd), net::Endpoint{"127.0.0.1", 0}};

  constexpr std::string_view message{"Hello from write_all"};

  const auto bytes_written{conn.write_all(message)};

  REQUIRE(bytes_written == message.size());

  std::array<char, 64> buffer{};
  const auto bytes_read{::read(client_fd.get(), buffer.data(), buffer.size())};

  REQUIRE(bytes_read == static_cast<ssize_t>(message.size()));
  REQUIRE(std::string_view{buffer.data(), message.size()} == message);
}

TEST_CASE("TcpConnection write_all with empty data writes zero bytes", "[unit][tcp_connection]") {
  auto [client_fd, server_fd] = make_socket_pair();

  net::TcpConnection conn{std::move(server_fd), net::Endpoint{"127.0.0.1", 0}};

  REQUIRE(conn.write_all("") == 0);
}
