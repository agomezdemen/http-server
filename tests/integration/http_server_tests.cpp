#include "server_process.h"
#include "http_client.h"

#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include <cstdint>
#include <chrono>

using test::integration::ServerProcess; 
using test::integration::HttpClient;

TEST_CASE("server starts and listens", "[integration][process]") {
  constexpr std::uint16_t port{18080};

  ServerProcess server{HTTP_SERVER_EXECUTABLE, port};

  REQUIRE(server.wait_until_ready(std::chrono::seconds{1}));
}

TEST_CASE("server responds to a valid GET request", "[integration][server]") {
  constexpr std::uint16_t port{18080};

  ServerProcess server{HTTP_SERVER_EXECUTABLE, port};
  REQUIRE(server.wait_until_ready(std::chrono::seconds{1}));

  HttpClient client{"127.0.0.1", port};
  
  constexpr std::string_view request{
    "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"};

  const auto response{client.send_request(request)};

  REQUIRE(response.starts_with("HTTP/1.1 200 OK\r\n"));
  REQUIRE(response.find("Content-Length: 12\r\n") != std::string::npos);
  REQUIRE(response.ends_with("\r\n\r\nHello world!"));
}

TEST_CASE("server handles multiple sequential connections", "[integration][server]") {
  constexpr std::uint16_t port{18080};

  ServerProcess server{HTTP_SERVER_EXECUTABLE, port};
  REQUIRE(server.wait_until_ready(std::chrono::seconds{1}));

  constexpr std::string_view request{
    "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"};

  for(auto iteration{0}; iteration < 5; ++iteration) {
    INFO("connection: " << iteration);

    HttpClient client{"127.0.0.1", port};
    const auto response{client.send_request(request)};

    REQUIRE(response.starts_with("HTTP/1.1 200 OK\r\n"));
    REQUIRE(response.find("Content-Length: 12\r\n") != std::string::npos);
    REQUIRE(response.ends_with("\r\n\r\nHello world!"));
  }
}

TEST_CASE("server responds with bad request to malformed HTTP", "[integration][server]") {
  constexpr std::uint16_t port{18080};

  ServerProcess server{HTTP_SERVER_EXECUTABLE, port};
  REQUIRE(server.wait_until_ready(std::chrono::seconds{1}));

  HttpClient client{"127.0.0.1", port};

  constexpr std::string_view request{"INVALID REQUEST\r\n\r\n"};
  const auto response{client.send_request(request)};

  REQUIRE(response.starts_with("HTTP/1.1 400 Bad Request\r\n"));
  REQUIRE(response.find("Content-Length: 11\r\n") != std::string::npos);
  REQUIRE(response.ends_with("\r\n\r\nBad Request"));
}
