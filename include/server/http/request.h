#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <string_view>
#include <vector>
#include <span>

#include "http_types.h"

namespace server::http {
// Stores the structured pieces of an HTTP request once parsing is complete.
class Request {
 private:
  std::vector<Header> headers_;
  std::string body_;
  RequestLine request_line_ 

 public:
  explicit Request(Method method, std::string target, Version version) noexcept;
  explicit Request(RequestLine request_line) noexcept;

  auto set_header(std::string name, std::string value) -> void;
  auto set_header(Header header) -> void;
  auto set_body(std::string body) -> void;
  [[nodiscard]] auto get_headers() const noexcept -> std::span<const Header>;
  [[nodiscard]] auto get_target() const noexcept -> std::string_view;
  [[nodiscard]] auto get_body() const noexcept -> std::string_view;
  [[nodiscard]] auto get_method() const noexcept -> const Method;
  [[nodiscard]] auto get_version() const noexcept -> const Version;

  };
}  // namespace server::http

#endif
