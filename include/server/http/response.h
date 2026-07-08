#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <string_view>
#include <vector>
#include "http_types.h"

namespace http = server::http;

class HttpResponse {
private:
  http::Version version_;
  http::Status status_;
  std::vector<http::Header> headers_;
  std::string body_;

public:
  explicit HttpResponse(http::Status status, http::Version version = http::Version::http_1_1);
    
  auto set_header(std::string name, std::string value) -> void;
  auto set_body(std::string body) -> void;
  [[nodiscard]] auto to_string() const -> std::string;
};

#endif
