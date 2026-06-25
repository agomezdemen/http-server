#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <string_view>
#include <vector>

enum class HttpStatus : int {
  ok = 200,
  bad_request = 400,
  not_found = 404,
  method_not_allowed = 405,
  internal_server_error = 500
};

struct Header {
  std::string name;
  std::string value;
};

class HttpResponse {
private:
  HttpStatus status_;
  std::vector<Header> headers_;
  std::string body_;

  static constexpr auto reason_phrase(HttpStatus status) noexcept -> std::string_view {
    switch (status) {
      case HttpStatus::ok:
        return "OK";
      case HttpStatus::bad_request:
        return "Bad Request";
      case HttpStatus::not_found:
        return "Not Found";
      case HttpStatus::method_not_allowed:
        return "Method Not Allowed";
      case HttpStatus::internal_server_error:
        return "Internal Server Error";
    }

    return "Unknown";
  }

public:
  explicit HttpResponse(HttpStatus status);
    
  auto set_header(std::string name, std::string value) -> void;
  auto set_body(std::string body) -> void;
  [[nodiscard]] auto to_string() const -> std::string;
};

#endif
