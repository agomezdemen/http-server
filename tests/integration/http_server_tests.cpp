#include "server_process.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <cstdint>
#include <chrono>

TEST_CASE("server starts and listens", "[integration][process]") {
  constexpr std::uint16_t port{18080};

  ServerProcess server{std::filesystem::path{HTTP_SERVER_EXECUTABLE}, port};

  REQUIRE(server.wait_until_ready(std::chrono::seconds{1}));
}


