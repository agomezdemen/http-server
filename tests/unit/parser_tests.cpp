#include <catch2/catch_test_macros.hpp>
#include <string_view>

#include "server/http/parser.h"

using server::http::ParseStatus;
using server::http::RequestParser;

TEST_CASE("Complete GET request in one call", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n\r\n"};

  RequestParser parser{};

  const auto result{parser.consume(request)};
  REQUIRE(result == ParseStatus::complete);
}

TEST_CASE("Request line split between multiple calls", "[unit][parser]") {
  constexpr std::string_view bytes_1{"GET "};
  constexpr std::string_view bytes_2{"/ HTT"};
  constexpr std::string_view bytes_3{"P/1.1"};
  constexpr std::string_view bytes_4{"\r\n"};

  RequestParser parser{};

  const auto result_1{parser.consume(bytes_1)};
  REQUIRE(result_1 == ParseStatus::need_more_data);

  const auto result_2{parser.consume(bytes_2)};
  REQUIRE(result_2 == ParseStatus::need_more_data);

  const auto result_3{parser.consume(bytes_3)};
  REQUIRE(result_3 == ParseStatus::need_more_data);

  const auto result_4{parser.consume(bytes_4)};
  REQUIRE(result_4 == ParseStatus::need_more_data);
}

TEST_CASE("Header split between calls", "[unit][parser]") {
  constexpr std::string_view request_line{"GET / HTTP/1.1\r\n"};
  constexpr std::string_view header_1{"Accept-Language"};
  constexpr std::string_view header_2{": en\r\n"};
  constexpr std::string_view eoh{"\r\n"};

  RequestParser parser{};

  const auto request_line_result{parser.consume(request_line)};
  REQUIRE(request_line_result == ParseStatus::need_more_data);

  const auto result_1{parser.consume(header_1)};
  REQUIRE(result_1 == ParseStatus::need_more_data);

  const auto result_2{parser.consume(header_2)};
  REQUIRE(result_2 == ParseStatus::need_more_data);

  const auto result_3{parser.consume(eoh)};
  REQUIRE(result_3 == ParseStatus::complete);
}

TEST_CASE("Carriage return and newline split between calls", "[unit][parser]") {
  constexpr std::string_view bytes_1{"GET / HTTP/1.1\r"};
  constexpr std::string_view bytes_2{"\n"};

  RequestParser parser{};

  const auto result_1{parser.consume(bytes_1)};
  REQUIRE(result_1 == ParseStatus::need_more_data);

  const auto result_2{parser.consume(bytes_2)};
  REQUIRE(result_2 == ParseStatus::need_more_data);
}

TEST_CASE("Body split between multiple calls", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n"
                                     "Content-Length: 10\r\n"
                                     "\r\n"};
  constexpr std::string_view body_1{"hello"};
  constexpr std::string_view body_2{"hello"};

  RequestParser parser{};

  const auto request_result{parser.consume(request)};
  REQUIRE(request_result == ParseStatus::need_more_data);

  const auto result_1{parser.consume(body_1)};
  REQUIRE(result_1 == ParseStatus::need_more_data);

  const auto result_2{parser.consume(body_2)};
  REQUIRE(result_2 == ParseStatus::complete);
}

TEST_CASE("Request split at every possible byte position", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n\r\n"};

  for(auto split{1uz}; split < request.length(); ++split) {
    CAPTURE(split);

    RequestParser parser{};

    const auto first_result{parser.consume(request.substr(0uz, split))};
    REQUIRE(first_result == ParseStatus::need_more_data);

    const auto second_result{parser.consume(request.substr(split))};
    REQUIRE(second_result == ParseStatus::complete);
  }
}

TEST_CASE("Multiple headers", "[unit][parser]") {
  constexpr std::string_view request_line{"GET / HTTP/1.1\r\n"};
  constexpr std::string_view header_1{"Host: awesome.com\r\n"};
  constexpr std::string_view header_2{"Accept-Language: en\r\n\r\n"};

  RequestParser parser{};

  const auto request_line_result{parser.consume(request_line)};
  REQUIRE(request_line_result == ParseStatus::need_more_data);

  const auto result_1{parser.consume(header_1)};
  REQUIRE(result_1 == ParseStatus::need_more_data);

  const auto result_2{parser.consume(header_2)};
  REQUIRE(result_2 == ParseStatus::complete);
}

TEST_CASE("Content-Length: 0", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n"
                                     "Content-Length: 0\r\n"
                                     "\r\n"};

  RequestParser parser{};

  const auto result{parser.consume(request)};
  REQUIRE(result == ParseStatus::complete);
}

TEST_CASE("Content-Length: 5 with a space", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n"
                                     "Content-Length: 5\r\n"
                                     "\r\n"
                                     "hello"};

  RequestParser parser{};

  const auto result{parser.consume(request)};
  REQUIRE(result == ParseStatus::complete);
}

TEST_CASE("Lowercase content-length", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n"
                                     "content-length: 5\r\n"
                                     "\r\n"
                                     "hello"};

  RequestParser parser{};

  const auto result{parser.consume(request)};
  REQUIRE(result == ParseStatus::complete);
}

TEST_CASE("Conflicting duplicate Content-Length produces invalid", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n"
                                     "Content-Length: 0\r\n"};
  constexpr std::string_view duplicate{"Content-Length: 5\r\n"
                                       "\r\n"};

  RequestParser parser{};

  const auto request_result{parser.consume(request)};
  REQUIRE(request_result == ParseStatus::need_more_data);

  const auto result{parser.consume(duplicate)};
  REQUIRE(result == ParseStatus::invalid);
}

TEST_CASE("Body shorter than Content-Length returns need_more_data", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n"
                                     "Content-Length: 5\r\n"
                                     "\r\n"
                                     "1234"};

  RequestParser parser{};

  const auto result{parser.consume(request)};
  REQUIRE(result == ParseStatus::need_more_data);
}

TEST_CASE("Extra bytes after the body remain unconsumed", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n"
                                     "Content-Length: 5\r\n"
                                     "\r\n"
                                     "hellopoeinpoeir"};

  RequestParser parser{};

  const auto result{parser.consume(request)};
  REQUIRE(result == ParseStatus::complete);

  REQUIRE_FALSE(parser.is_buffer_empty());
}

TEST_CASE("Malformed request line produces invalid", "[unit][parser]") {
  constexpr std::string_view request_line{"Got . http3.5\r\n"};

  RequestParser parser{};

  const auto result{parser.consume(request_line)};
  REQUIRE(result == ParseStatus::invalid);
}

TEST_CASE("Malformed header produces invalid", "[unit][parser]") {
  constexpr std::string_view request_line{"GET / HTTP/1.1\r\n"};
  constexpr std::string_view header{"bad header ; pwe844\r\n"};

  RequestParser parser{};

  const auto request_line_result{parser.consume(request_line)};
  REQUIRE(request_line_result == ParseStatus::need_more_data);

  const auto result{parser.consume(header)};
  REQUIRE(result == ParseStatus::invalid);
}

TEST_CASE("take_request() before completion returns nullopt", "[unit][parser]") {
  RequestParser parser{};

  const auto request{parser.take_request()};
  REQUIRE_FALSE(request.has_value());
}

TEST_CASE("take_request() after completion returns the request", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n\r\n"};

  RequestParser parser{};

  const auto result{parser.consume(request)};
  REQUIRE(result == ParseStatus::complete);

  const auto final_request{parser.take_request()};
  REQUIRE(final_request.has_value());
}

TEST_CASE("A second take_request() returns nullopt", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n\r\n"};

  RequestParser parser{};

  const auto result{parser.consume(request)};
  REQUIRE(result == ParseStatus::complete);

  const auto final_request{parser.take_request()};
  REQUIRE(final_request.has_value());

  const auto second{parser.take_request()};
  REQUIRE_FALSE(second.has_value());
}

TEST_CASE("reset() allows another request to be parsed", "[unit][parser]") {
  constexpr std::string_view request{"GET / HTTP/1.1\r\n\r\n"};

  RequestParser parser{};

  const auto first_result{parser.consume(request)};
  REQUIRE(first_result == ParseStatus::complete);

  const auto first_request{parser.take_request()};
  REQUIRE(first_request.has_value());

  parser.reset();

  const auto second_result{parser.consume(request)};
  REQUIRE(second_result == ParseStatus::complete);

  const auto second_request{parser.take_request()};
  REQUIRE(second_request.has_value());
}
