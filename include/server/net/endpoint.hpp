#pragma once

#include <cstdint>
#include <string>

namespace http_server::net {

struct Endpoint {
    std::string address{"0.0.0.0"};
    std::uint16_t port{8080};
};

} // namespace http_server::net
