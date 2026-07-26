#include <catch2/catch_test_macros.hpp>

#include "../include/server/http/parser.h"

TEST_CASE("Complete GET request in one call") {}

TEST_CASE("Request line split between multiple calls") {}

TEST_CASE("Header split between calls") {}

TEST_CASE("Carraige return and new line split between calls") {}

TEST_CASE("Body split between multiple calls") {}

TEST_CASE("Request split at every possible byte position") {}

TEST_CASE("Multiple headers") {}

TEST_CASE("No body") {}

TEST_CASE("Content-Length: 0") {}

TEST_CASE("Normal Content-Length: 5 with a space") {}

TEST_CASE("Lowercase content-length") {}

TEST_CASE("Invalid and duplicate Content-Length") {}

TEST_CASE("Body shorter than Content-Length returns need_more_data") {}

TEST_CASE("Body exactly matching Content-Length completes") {}

TEST_CASE("Extra bytes after the body remain unconsumed") {}

TEST_CASE("Malformed request line and malformed header produce invalid") {}

TEST_CASE("take_request() before completion returns nullopt") {}

TEST_CASE("take_request() after completion returns the request") {}

TEST_CASE("A second take_requst returns nullopt") {}

TEST_CASE("reset() allows another request to be parsed") {}

