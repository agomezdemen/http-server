#include "../../include/server/http/response.h"

#include <string>

namespace server::http {

Response::Response(Status status, Version version)
    : headers_{}, body_{}, status_{status}, version_{version} {}

auto Response::set_header(std::string name, std::string value) -> void {
  // Header names are treated as exact matches for now.
  for (auto& header : headers_) {
    if (header.name == name) {
      header.value = value;
      return;
    }
  }

  headers_.push_back(Header{.name = std::move(name), .value = std::move(value)});
}

auto Response::set_body(std::string body) -> void { body_ = std::move(body); }

auto Response::to_string() const -> std::string {
  auto response{std::string{}};

  // Serialize the start line first, followed by user headers and body metadata.
  response += server::http::to_string(version_);
  response += ' ';
  response += std::to_string(static_cast<int>(status_));
  response += ' ';
  response.append(server::http::to_string(status_));
  response += "\r\n";

  for (const auto& header : headers_) {
    response += header.name;
    response += ": ";
    response += header.value;
    response += "\r\n";
  }

  response += "Content-Length: ";
  response += std::to_string(body_.size());
  response += "\r\n";

  response += "\r\n";
  response += body_;

  return response;
}

}  // namespace server::http
