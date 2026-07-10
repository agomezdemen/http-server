#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <string_view>
#include <vector>

#include "http_types.h"

namespace server::http {
// Stores the structured pieces of an HTTP request once parsing is complete.
class Request {
 private:
  std::vector<Header> headers_;
  std::string target_;
  std::string body_;
  Method method_;
  Version version_;

 public:
  explicit Request(Method method, std::string target, Version version);

  auto set_header(std::string name, std::string value) -> void;
  auto set_body(std::string body) -> void;
  [[nodiscard]] const auto& get_headers() noexcept -> std::span<const Header>;
  [[nodiscard]] const auto& get_target() noexcept -> std::string_view;
  [[nodiscard]] const auto& get_body() noexcept -> std::string_view;
  [[nodiscard]] const auto& get_method() noexcept -> const Method;
  [[nodiscard]] const auto& get_version() noexcept -> const Version;

  };
}  // namespace server::http

#endif
