#include "../include/server/http/response.h"
#include "../include/server/http/http_types.h"

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <string>
#include <string_view>

namespace http = server::http;

namespace {

auto count_occurrences(std::string_view text, std::string_view target) -> std::size_t {
  if (target.empty()) {
    return 0;
  }

  std::size_t count{};
  std::size_t pos{};

  while ((pos = text.find(target, pos)) != std::string_view::npos) {
    ++count;
    pos += target.size();
  }

  return count;
}

}

TEST_CASE("Default OK response has correct status line") {
  http::Response res{http::Status::ok};

  const auto out{res.to_string()};

  REQUIRE(out.starts_with("HTTP/1.1 200 OK\r\n"));
}

TEST_CASE("Response serializes the configured HTTP version") {
  http::Response res{http::Status::ok, http::Version::unknown};

  const auto out{res.to_string()};

  REQUIRE(out.starts_with("UNKNOWN 200 OK\r\n"));
}

TEST_CASE("Status codes serialize with correct status phrases") {
  REQUIRE(http::Response{http::Status::ok}.to_string().starts_with(
      "HTTP/1.1 200 OK\r\n"));

  REQUIRE(http::Response{http::Status::bad_request}.to_string().starts_with(
      "HTTP/1.1 400 Bad Request\r\n"));

  REQUIRE(http::Response{http::Status::not_found}.to_string().starts_with(
      "HTTP/1.1 404 Not Found\r\n"));

  REQUIRE(http::Response{http::Status::method_not_allowed}.to_string().starts_with(
      "HTTP/1.1 405 Method Not Allowed\r\n"));

  REQUIRE(http::Response{http::Status::internal_server_error}.to_string().starts_with(
      "HTTP/1.1 500 Internal Server Error\r\n"));
}

TEST_CASE("Body is serialized after CRLF CRLF separator") {
  http::Response res{http::Status::ok};
  res.set_body("Hello world!");

  const auto out{res.to_string()};

  REQUIRE(out.contains("\r\n\r\nHello world!"));
}

TEST_CASE("Content-Length matches body size") {
  http::Response res{http::Status::ok};
  res.set_body("Hello world!");

  const auto out{res.to_string()};

  REQUIRE(out.contains("Content-Length: 12\r\n"));
}

TEST_CASE("Empty body has Content-Length zero") {
  http::Response res{http::Status::ok};

  const auto out{res.to_string()};

  REQUIRE(out.contains("Content-Length: 0\r\n"));
}

TEST_CASE("Custom headers are serialized") {
  http::Response res{http::Status::ok};
  res.set_header("Content-Type", "text/plain");

  const auto out{res.to_string()};

  REQUIRE(out.contains("Content-Type: text/plain\r\n"));
}

TEST_CASE("Multiple different headers are serialized") {
  http::Response res{http::Status::ok};

  res.set_header("Content-Type", "text/plain");
  res.set_header("Connection", "close");

  const auto out{res.to_string()};

  REQUIRE(out.contains("Content-Type: text/plain\r\n"));
  REQUIRE(out.contains("Connection: close\r\n"));
}

TEST_CASE("set_header replaces existing header value") {
  http::Response res{http::Status::ok};

  res.set_header("Content-Type", "text/plain");
  res.set_header("Content-Type", "application/json");

  const auto out{res.to_string()};

  REQUIRE(out.contains("Content-Type: application/json\r\n"));
  REQUIRE_FALSE(out.contains("Content-Type: text/plain\r\n"));
}

TEST_CASE("Complete response serializes exactly") {
  http::Response res{http::Status::ok};

  res.set_header("Content-Type", "text/plain");
  res.set_body("Hello");

  const auto out{res.to_string()};

  REQUIRE(out ==
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/plain\r\n"
          "Content-Length: 5\r\n"
          "\r\n"
          "Hello");
}

TEST_CASE("Headers end before body begins") {
  http::Response res{http::Status::ok};

  res.set_header("Content-Type", "text/plain");
  res.set_body("abc");

  const auto out{res.to_string()};

  const auto separator{out.find("\r\n\r\n")};
  REQUIRE(separator != std::string::npos);

  const auto body_pos{out.find("abc")};
  REQUIRE(body_pos != std::string::npos);
  REQUIRE(body_pos > separator);
}

TEST_CASE("Content-Length handles body with null bytes") {
  http::Response res{http::Status::ok};

  const std::string body{"abc\0def", 7};
  res.set_body(body);

  const auto out{res.to_string()};

  REQUIRE(out.contains("Content-Length: 7\r\n"));
  REQUIRE(out.ends_with(body));
}

TEST_CASE("Response uses CRLF line endings") {
  http::Response res{http::Status::ok};

  res.set_body("Hi");

  const auto out{res.to_string()};

  REQUIRE(out.contains("HTTP/1.1 200 OK\r\n"));
  REQUIRE(out.contains("\r\n\r\nHi"));
}

TEST_CASE("to_string does not duplicate Content-Length across calls") {
  http::Response res{http::Status::ok};

  res.set_body("Hello");

  const auto first{res.to_string()};
  const auto second{res.to_string()};

  constexpr std::string_view target{"Content-Length: 5\r\n"};

  REQUIRE(first.contains(target));
  REQUIRE(second.contains(target));

  REQUIRE(count_occurrences(second, target) == 1);
}

TEST_CASE("Content-Length updates when body changes") {
  http::Response res{http::Status::ok};

  res.set_body("Hello");
  const auto first{res.to_string()};

  res.set_body("Hello world!");
  const auto second{res.to_string()};

  REQUIRE(first.contains("Content-Length: 5\r\n"));
  REQUIRE(second.contains("Content-Length: 12\r\n"));
  REQUIRE_FALSE(second.contains("Content-Length: 5\r\n"));
}
