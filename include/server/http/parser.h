#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <optional>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

#include "server/http/request.h"

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
    progressed,
    need_more_data,
    invalid
  };

  class RequestParser {
  private:
    std::string buffer_;
    std::size_t cursor_{0uz};
    ParseState state_{ParseState::request_line};

    std::optional<Request> request_;
    std::optional<std::size_t> expected_body_size_;

    auto process_request_line_state() -> ParseStatus;
    auto process_headers_state() -> ParseStatus;
    auto process_body_state() -> ParseStatus;
    auto process_current_state() -> ParseStatus;
    auto remaining_buffer() const noexcept -> std::string_view;

  public:
    RequestParser() = default;

    [[nodiscard]] auto consume(std::string_view bytes) -> ParseStatus;
    [[nodiscard]] auto take_request() -> std::optional<Request>;
    [[nodiscard]] auto is_buffer_empty() const noexcept -> bool;

    auto get_state() const noexcept -> ParseState;
    auto reset() -> void;
  };
}

#endif
