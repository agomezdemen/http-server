#ifndef HTTP_RESPONSE
#define HTTP_RESPONSE

#include <string>
#include <string_view>

enum class HttpStatus : int {
  ok = 200,
  bad_request = 400,
  not_found = 404,
  method_not_allowed = 405,
  internal_server_error = 500
};

class HttpResponse {
private:
  HttpStatus status_;
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
};

#endif
