#include <catch2/catch_test_macros.hpp>

#include "server/http/http_types.h"
#include "server/http/request_grammar.h"

namespace http = server::http;

TEST_CASE("Empty line returns empty request line error") {
  const auto result{http::parse_request_line("")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::empty_request_line);
}

TEST_CASE("Missing separator returns missing separator error") {
  SECTION("First separator missing") {
    const auto result_1{http::parse_request_line("GET")};

    REQUIRE_FALSE(result_1.has_value());
    CHECK(result_1.error() == http::GrammarError::missing_separator);
  }

  SECTION("Second separator missing") {
    const auto result_2{http::parse_request_line("GET /")};

    REQUIRE_FALSE(result_2.has_value());
    CHECK(result_2.error() == http::GrammarError::missing_separator);
  }
}

TEST_CASE("Invalid method returns invalid method error") {
  const auto result{http::parse_request_line("HELLO / HTTP/1.1")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::invalid_method);
}

TEST_CASE("Empty target returns empty target error") {
  const auto result{http::parse_request_line("GET  ")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::empty_target);
}

TEST_CASE("Invalid version returns invalid version error") {
  const auto result{http::parse_request_line("GET / ht48")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::invalid_version);
}

TEST_CASE("Request line is parsed correctly") {
  const auto result{http::parse_request_line("GET / HTTP/1.1")};

  REQUIRE(result.has_value());
  CHECK(result->method == http::Method::get);
  CHECK(result->target == "/");
  CHECK(result->version == http::Version::http_1_1);
}


TEST_CASE("Invalid header name returns false") {
  auto valid{http::is_valid_header_name("  []}")};

  REQUIRE_FALSE(valid);
}

TEST_CASE("Valid header name returns true") {
  const auto valid{http::is_valid_header_name("Name!#$%^&|")};

  REQUIRE(valid);
}

TEST_CASE("Empty header returns empty header error") {
  const auto result{http::parse_header("")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::empty_header);
}

TEST_CASE("Header with missing colon returns missing colon error") {
  const auto result{http::parse_header("Name Value")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::missing_colon);
}

TEST_CASE("Invalid header name returns invalid header name error") {
  const auto result{http::parse_header("[]: Value")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::invalid_header_name);
}

TEST_CASE("Header is parsed correctly") {
  SECTION("Ideal header parse") {
    const auto result_1{http::parse_header("Host: 127.0.0.1")};

    REQUIRE(result_1.has_value());
    CHECK(result_1->name == "Host");
    CHECK(result_1->value == "127.0.0.1");
  }

  SECTION("Parsing header with value that needs trimming") {
    const auto result_2{http::parse_header("Host:     127.0.0.1          ")};

    REQUIRE(result_2.has_value());
    CHECK(result_2->name == "Host");
    CHECK(result_2->value == "127.0.0.1");
  }
}

TEST_CASE("Empty header name returns invalid header name error") {
  const auto result{http::parse_header(": value")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::invalid_header_name);
}

TEST_CASE("Empty header value is parsed correctly") {
  const auto result{http::parse_header("Name:")};

  REQUIRE(result.has_value());
  CHECK(result->name == "Name");
  CHECK(result->value.empty());
}

TEST_CASE("Tabs surrounding header value are trimmed") {
  const auto result{http::parse_header("Name:\t\tvalue\t\t")};

  REQUIRE(result.has_value());
  CHECK(result->name == "Name");
  CHECK(result->value == "value");
}

TEST_CASE("Colon inside header value is preserved") {
  const auto result{http::parse_header("Time: 12:30")};

  REQUIRE(result.has_value());
  CHECK(result->name == "Time");
  CHECK(result->value == "12:30");
}

TEST_CASE("All valid header name symbols are accepted") {
  CHECK(http::is_valid_header_name("!#$%&'*+-.^_`|~"));
}

TEST_CASE("Header names containing control characters are rejected") {
  CHECK_FALSE(http::is_valid_header_name("Bad\nName"));
  CHECK_FALSE(http::is_valid_header_name("Bad\rName"));
  CHECK_FALSE(http::is_valid_header_name("Bad\tName"));

  constexpr char name_with_null[]{
      'B', 'a', 'd', '\0', 'N', 'a', 'm', 'e'
  };

  CHECK_FALSE(http::is_valid_header_name(
      std::string_view{name_with_null, sizeof(name_with_null)}));
}

TEST_CASE("Header name containing non-ASCII byte is rejected") {
  constexpr char non_ascii_name[]{
      'N', 'a', 'm', 'e', static_cast<char>(0x80)
  };

  CHECK_FALSE(http::is_valid_header_name(
      std::string_view{non_ascii_name, sizeof(non_ascii_name)}));
}

TEST_CASE("Extra space before target returns empty target error") {
  const auto result{http::parse_request_line("GET  / HTTP/1.1")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::empty_target);
}

TEST_CASE("Extra space before version returns invalid version error") {
  const auto result{http::parse_request_line("GET /  HTTP/1.1")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::invalid_version);
}

TEST_CASE("Trailing space after version returns invalid version error") {
  const auto result{http::parse_request_line("GET / HTTP/1.1 ")};

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error() == http::GrammarError::invalid_version);
}
