#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <optional>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include "request.h"

namespace server::http {
  enum class ParseState : std::uint8_t {
    request_line,
    headers,
    body,
    complete,
    error
  };

  enum class ParseStatus : std::uint8_t {
    complete,
    need_more_data,
    invalid
  };

  class RequestParser {
  private:
    std::string buffer_;
    ParseState state_{ParseState::request_line};

    std::optional<Request> request_;
    std::optional<std::size_t> expected_body_size_;

    auto process_request_line_state() -> ParseStatus;
    auto process_headers_state() -> ParseStatus;
    auto process_body_state() -> ParseStatus;
    auto process_current_state() -> ParseStatus;

  public:
    [[nodiscard]] auto consume(std::string_view bytes) -> ParseStatus;
    
    [[nodiscard]] auto take_request() -> std::optional<Request>;

    auto reset() -> void;
  };
}

#endif
