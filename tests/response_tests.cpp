#include "server/http/response.hpp"

#include <cassert>
#include <string>

auto main() -> int
{
    auto response = http_server::http::Response{http_server::http::Status::ok};
    response.set_header("Content-Type", "text/plain");
    response.set_body("hello");

    const auto serialized = response.serialize();

    assert(serialized.find("HTTP/1.1 200 OK\r\n") == 0U);
    assert(serialized.find("Content-Type: text/plain\r\n") != std::string::npos);
    assert(serialized.find("Content-Length: 5\r\n") != std::string::npos);
    assert(serialized.ends_with("\r\n\r\nhello"));
}
