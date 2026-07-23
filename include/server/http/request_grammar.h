#ifndef HTTP_REQUEST_GRAMMAR_H
#define HTTP_REQUEST_GRAMMAR_H

#include <string_view>
#include <expected>
#include <cstdint>

#include "http_types.h"

namespace server::http {
  
  enum class GrammarError : std::uint8_t {
    empty_request_line,
    invalid_method,
    missing_separator,
    empty_target,
    invalid_target,
    invalid_version,
    missing_colon,
    empty_header,
    invalid_header_name,
    invalid_header_value,
  };

  [[nodiscard]]
  auto parse_request_line(std::string_view line) -> std::expected<RequestLine, GrammarError>;

  [[nodiscard]]
  auto parse_header(std::string_view header) -> std::expected<Header, GrammarError>;

  [[nodiscard]]
  auto is_valid_header_name(std::string_view name) noexcept -> bool;
  
}

#endif
