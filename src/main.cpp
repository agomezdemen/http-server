#include <array>
#include <charconv>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>

#include "server/http/router.h"
#include "server/http_server.h"

namespace http = server::http;
namespace net = server::net;

auto main(int argc, char* argv[]) -> int {
  if(argc != 2)
    return 1;

  const std::string_view port_string{argv[1]};
  std::uint16_t port{};

  const auto [ptr, ec]{std::from_chars(port_string.data(),
                                      port_string.data() + port_string.size(),
                                      port)};

  if(ec != std::errc{} || ptr != port_string.data() + port_string.size() || port == 0)
    return 1;

  net::Endpoint ep{"127.0.0.1", port};
  http::Router router;

  if(!router.add_route(http::Method::get, "/",
    [](const http::Request&) -> http::Response {
      http::Response response{http::Status::ok};
      response.set_body("Hello world!");
      return response;}))
    return 1;

  if(!router.add_route(http::Method::get, "/health",
    [](const http::Request&) -> http::Response {
      http::Response response{http::Status::ok};
      response.set_body("OK");
      return response;}))
    return 1;


}
