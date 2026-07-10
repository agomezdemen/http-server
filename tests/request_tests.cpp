#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <string>
#include <string_view>

#include "../include/server/http/request.h"
#include "../include/server/http/http_types.h"

using http = server::http;

TEST_CASE("") {
  http::Request request{http::Method::get, "/", http::Version::http_1_1};
}
