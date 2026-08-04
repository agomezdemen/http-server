#include <catch2/catch_test_macros.hpp>

#include "server/http/http_types.h"
#include "server/http/request.h"

namespace http = server::http;

TEST_CASE("Request is initialized with the provided method target and version", "[unit][request]") {
  const http::Request request{http::Method::get, "/", http::Version::http_1_1};
  
  const auto headers{request.get_headers()};
  const auto target{request.get_target()};
  const auto body{request.get_body()};
  const auto method{request.get_method()};
  const auto version{request.get_version()};

  REQUIRE(headers.empty());
  REQUIRE(target == "/");
  REQUIRE(body.empty());
  REQUIRE(method == http::Method::get);
  REQUIRE(version == http::Version::http_1_1);
}

TEST_CASE("Request can add a header", "[unit][request]") {
  http::Request request{http::Method::get, "/", http::Version::http_1_1};

  request.set_header("Host", "localhost");

  const auto headers{request.get_headers()};

  REQUIRE(headers.size() == 1);
  REQUIRE(headers[0].name == "Host");
  REQUIRE(headers[0].value == "localhost");
}

TEST_CASE("Setting an existing header updates its value", "[unit][request]") {
  http::Request request{http::Method::get, "/", http::Version::http_1_1};
  
  request.set_header("Content-Type", "text/plain");
  request.set_header("Content-Type", "application/json");

  const auto headers{request.get_headers()};

  REQUIRE(headers.size() == 1);
  REQUIRE(headers[0].name == "Content-Type");
  REQUIRE(headers[0].value == "application/json");
}


TEST_CASE("Request can add a header with a header type", "[unit][request]") {
  http::Request request{http::Method::get, "/", http::Version::http_1_1};

  request.set_header(http::Header{"Host", "localhost"});

  const auto headers{request.get_headers()};

  REQUIRE(headers.size() == 1);
  REQUIRE(headers[0].name == "Host");
  REQUIRE(headers[0].value == "localhost");
}

TEST_CASE("Setting an existing header updates its value with a header type", "[unit][request]") {
  http::Request request{http::Method::get, "/", http::Version::http_1_1};
  
  request.set_header(http::Header{"Content-Type", "text/plain"});
  request.set_header(http::Header{"Content-Type", "application/json"});

  const auto headers{request.get_headers()};

  REQUIRE(headers.size() == 1);
  REQUIRE(headers[0].name == "Content-Type");
  REQUIRE(headers[0].value == "application/json");
}

TEST_CASE("Request can store a body", "[unit][request]") {
  http::Request request{http::Method::post, "/", http::Version::http_1_1};

  request.set_body(R"({"name":"John"})");

  const auto body{request.get_body()};

  REQUIRE(body == R"({"name":"John"})");
} 
