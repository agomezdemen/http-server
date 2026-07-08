#include "../../include/server/http/response.h"
#include <string>

namespace http = server::http;

HttpResponse::HttpResponse(http::Status status, http::Version version) : version_{version}, status_{status}, headers_{}, body_{} {}
  
auto HttpResponse::set_header(std::string name, std::string value) -> void {
  for(auto& header : headers_) {
    if(header.name == name) {
      header.value = value;
      return;
    }
  }

  headers_.push_back(http::Header{
      .name = std::move(name), 
      .value = std::move(value)});
}

auto HttpResponse::set_body(std::string body) -> void {
  body_ = std::move(body);
}

auto HttpResponse::to_string() const -> std::string {
  auto response{std::string{}};

  response += http::to_string(version_);
  response += ' ';
  response += std::to_string(static_cast<int>(status_));
  response += ' ';
  response.append(http::to_string(status_));
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
