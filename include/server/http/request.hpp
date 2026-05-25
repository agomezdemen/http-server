#pragma once

#include <string>
#include <unordered_map>

namespace http_server::http {

struct Request {
    std::string method{};
    std::string target{};
    std::string version{};
    std::unordered_map<std::string, std::string> headers{};
    std::string body{};
};

} // namespace http_server::http
