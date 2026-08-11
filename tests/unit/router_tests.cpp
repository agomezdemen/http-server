#include <catch2/catch_test_macros.hpp>

#include "server/http/router.h"
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
