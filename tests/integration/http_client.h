#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string_view>
#include <cstdint>
#include <string>

auto send_http_request(std::uint16_t port, std::string_view request) -> std::string;

#endif
