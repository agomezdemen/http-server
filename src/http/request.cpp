#include "../../include/server/http/request.h"
#include "../../include/server/http/http_types.h"

namespace server::http {
  explicit Request(Method method, std::string target, Version version) noexcept : 
    headers_{}, target_{std::move(target)}, body_{}, method_{method}, version_{version} {}

  auto set_header(std::string name, std::string value) -> void {
      
    for (auto& header : headers_) {
      if (header.name == name) {
        header.value = value;
        return;
      }
    }

    headers_.push_back(Header{.name = std::move(name), .value = std::move(value)});
  }

  auto set_body(std::string body) -> void {
    body_ = std::move(body);
  }
  
  auto get_headers() noexcept -> std::span<const Header> {
    return headers_;
  }

  auto get_target() noexcept -> std::string_view {
    return target_;
  }

  auto get_body() noexcept -> std::string_view {
    return body_;
  }

  auto get_method() noexcept -> Method {
    return method_;
  }

  auto get_version() noexcept -> Version {
    return version_;
  }
}
