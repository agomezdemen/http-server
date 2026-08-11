#include <catch2/catch_test_macros.hpp>

#include "server/http/router.h"
#include "server/http/request.h"
#include "server/http/response.h"
#include "server/http/http_types.h"

namespace http = server::http;

TEST_CASE("Router matches a registered route", "[unit][router]") {
  http::Router router;

  REQUIRE(router.add_route(http::Method::get, "/health", 
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));

  http::Request request{http::Method::get, "/health", http::Version::http_1_1};

  const auto response{router.route(request)};

  REQUIRE(response.has_value());
}

TEST_CASE("Unknown path returns nullopt", "[unit][router]") {
  http::Router router;

  REQUIRE(router.add_route(http::Method::get, "/health", 
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));

  http::Request request{http::Method::get, "/user", http::Version::http_1_1};

  const auto response{router.route(request)};

  REQUIRE_FALSE(response.has_value());
}

TEST_CASE("Same path, wrong method does not match", "[unit][router]") {
  http::Router router;
  
  REQUIRE(router.add_route(http::Method::get, "/health", 
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));
  
  http::Request request{http::Method::post, "/health", http::Version::http_1_1};

  const auto response{router.route(request)};

  REQUIRE_FALSE(response.has_value());
}

TEST_CASE("Different methods can share the same path", "[unit][router]") {
  http::Router router;

  REQUIRE(router.add_route(http::Method::get, "/users", 
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));

  REQUIRE(router.add_route(http::Method::post, "/users", 
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));

  http::Request request_1{http::Method::get, "/users", http::Version::http_1_1};
  http::Request request_2{http::Method::post, "/users", http::Version::http_1_1};
  
  const auto response_1{router.route(request_1)};
  const auto response_2{router.route(request_2)};

  REQUIRE(response_1.has_value());
  REQUIRE(response_2.has_value());
}

TEST_CASE("Duplicate method + path is rejected", "[unit][router]") {
  http::Router router;

  REQUIRE(router.add_route(http::Method::get, "/health", 
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));

  REQUIRE_FALSE(router.add_route(http::Method::get, "/health", 
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));
}

TEST_CASE("Duplicate registration preserves the original handler", "[unit][router]") {
  http::Router router;
 

  REQUIRE(router.add_route(http::Method::get, "/health", 
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));

  REQUIRE_FALSE(router.add_route(http::Method::get, "/health", 
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::internal_server_error, http::Version::http_1_1};
        }));

  http::Request request{http::Method::get, "/health", http::Version::http_1_1};
  
  const auto response{router.route(request)};

  REQUIRE(response.has_value());

  REQUIRE(response->to_string() ==
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 0\r\n"
        "\r\n");
}

TEST_CASE("The correct handler's response is returned", "[unit][router]") {
  http::Router router;

  REQUIRE(router.add_route(http::Method::get, "/users",
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));

  REQUIRE(router.add_route(http::Method::post, "/users",
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::created, http::Version::http_1_1};
        }));

  http::Request request_1{http::Method::get, "/users", http::Version::http_1_1};
  http::Request request_2{http::Method::post, "/users", http::Version::http_1_1};

  const auto response_1{router.route(request_1)};
  const auto response_2{router.route(request_2)};

  REQUIRE(response_1.has_value());
  REQUIRE(response_2.has_value());
 
  REQUIRE(response_1->to_string() ==
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 0\r\n"
        "\r\n");

  REQUIRE(response_2->to_string() ==
        "HTTP/1.1 201 Created\r\n"
        "Content-Length: 0\r\n"
        "\r\n");
}

TEST_CASE("Handler receives the original request", "[unit][router]") {
  http::Router router;

  const http::Request* received_request{nullptr};

  REQUIRE(router.add_route(http::Method::get, "/health",
        [&received_request](const http::Request& request) -> http::Response {
          received_request = &request;
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));

  http::Request request{http::Method::get, "/health", http::Version::http_1_1};

  const auto response{router.route(request)};

  REQUIRE(response.has_value());
  REQUIRE(received_request == &request);
}

TEST_CASE("Request returns query string", "[unit][request]") {
  http::Request request{http::Method::get, "/search?q=test", http::Version::http_1_1};

  REQUIRE(request.get_query() == "q=test");
}

TEST_CASE("Request returns empty query when target has no query string", "[unit][request]") {
  http::Request request{http::Method::get, "/search", http::Version::http_1_1};

  REQUIRE(request.get_query().empty());
}

TEST_CASE("Router ignores query string when matching path", "[unit][router]") {
  http::Router router;

  REQUIRE(router.add_route(http::Method::get, "/search",
        [](const http::Request&) -> http::Response {
          return http::Response{http::Status::ok, http::Version::http_1_1};
        }));

  http::Request request{http::Method::get, "/search?q=test", http::Version::http_1_1};

  const auto response{router.route(request)};

  REQUIRE(response.has_value());
}
