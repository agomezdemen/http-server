#pragma once

#include "server/http/status.hpp"

#include <string>
#include <unordered_map>

namespace http_server::http {

class Response {
public:
    explicit Response(Status status = Status::ok);

    void set_header(std::string name, std::string value);
    void set_body(std::string body);

    [[nodiscard]] auto status() const noexcept -> Status;
    [[nodiscard]] auto serialize() const -> std::string;

private:
    Status status_{Status::ok};
    std::unordered_map<std::string, std::string> headers_{};
    std::string body_{};
};

} // namespace http_server::http
