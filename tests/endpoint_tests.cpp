#include "../include/server/net/endpoint.h"
#include <catch2/catch_test_macros.hpp>
#include <type_traits>
#include <utility>

// Endpoint is intentionally small: it only stores the user-facing address data.
TEST_CASE("Endpoint constructs with host and port") {
  Endpoint ep{"127.0.0.1", 8080};

  REQUIRE(ep.host() == "127.0.0.1");
  REQUIRE(ep.port() == 8080);
}

TEST_CASE("Endpoint is not default constructible") {
  static_assert(!std::is_default_constructible_v<Endpoint>);
}

TEST_CASE("Endpoint is copyable") {
  Endpoint ep_1{"127.0.0.1", 8080};

  Endpoint ep_2{ep_1};

  REQUIRE(ep_1.host() == "127.0.0.1"); 
  REQUIRE(ep_2.host() == "127.0.0.1");
  REQUIRE(ep_1.port() == 8080);
  REQUIRE(ep_2.port() == 8080);
}

TEST_CASE("Endpoint is movable") {
  Endpoint ep_1{"127.0.0.1", 8080};

  Endpoint ep_2{std::move(ep_1)};

  REQUIRE(ep_2.host() == "127.0.0.1");
  REQUIRE(ep_2.port() == 8080);
}
