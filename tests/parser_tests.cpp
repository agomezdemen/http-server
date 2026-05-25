#include "server/http/parser.hpp"

#include <cassert>

auto main() -> int
{
    const auto parser = http_server::http::Parser{};
    const auto result = parser.parse("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assert(result.request.has_value());
    assert(!result.error.has_value());
    assert(result.request->method == "GET");
    assert(result.request->target == "/");
    assert(result.request->version == "HTTP/1.1");
}
