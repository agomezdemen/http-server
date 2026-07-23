#include "../../include/server/http/request.h"
#include "../../include/server/http/http_types.h"

#include <vector>
#include <span>
#include <string>
#include <string_view>

namespace server::http {
  Request::Request(Method method, std::string target, Version version) noexcept : 
    headers_{}, body_{}, request_line_{RequestLine{.method = method, .target = std::move(target), .version = version}} {}

  Request::Request(RequestLine request_line) noexcept :
    headers_{}, body_{}, request_line_{std::move(request_line)} {}

  auto Request::set_header(std::string name, std::string value) -> void {
      
    for (auto& header : headers_) {
      if (header.name == name) {
        header.value = value;
        return;
      }
    }

    headers_.push_back(Header{.name = std::move(name), .value = std::move(value)});
  }

  auto Request::set_header(Header header) -> void {
    
    for (auto& c_header : headers_) {
      if (c_header.name == header.name) {
        c_header.value = std::move(header.value);
        return;
      }
    }

    headers_.push_back(std::move(header));
  }

  auto Request::set_body(std::string body) -> void {
    body_ = std::move(body);
  }
  
  auto Request::get_headers() const noexcept -> std::span<const Header> {
    return headers_;
  }

  auto Request::get_target() const noexcept -> std::string_view {
    return target_;
  }

  auto Request::get_body() const noexcept -> std::string_view {
    return body_;
  }

  auto Request::get_method() const noexcept -> const Method {
    return method_;
  }

  auto Request::get_version() const noexcept -> const Version {
    return version_;
  }
}
