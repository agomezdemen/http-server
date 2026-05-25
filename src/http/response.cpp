#include "server/http/response.hpp"

#include <utility>

namespace http_server::http {

auto reason_phrase(Status status) noexcept -> const char*
{
    switch (status) {
    case Status::ok:
        return "OK";
    case Status::bad_request:
        return "Bad Request";
    case Status::not_found:
        return "Not Found";
    case Status::internal_server_error:
        return "Internal Server Error";
    }

    return "Unknown";
}

Response::Response(Status status) : status_{status} {}

void Response::set_header(std::string name, std::string value)
{
    headers_.insert_or_assign(std::move(name), std::move(value));
}

void Response::set_body(std::string body)
{
    body_ = std::move(body);
}

auto Response::status() const noexcept -> Status
{
    return status_;
}

auto Response::serialize() const -> std::string
{
    auto output = std::string{"HTTP/1.1 "};
    output += std::to_string(static_cast<int>(status_));
    output += ' ';
    output += reason_phrase(status_);
    output += "\r\n";

    for (const auto& [name, value] : headers_) {
        output += name;
        output += ": ";
        output += value;
        output += "\r\n";
    }

    output += "Content-Length: ";
    output += std::to_string(body_.size());
    output += "\r\n\r\n";
    output += body_;
    return output;
}

} // namespace http_server::http
