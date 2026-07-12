#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <string>
#include "request.h"

namespace server::http {
  enum class ParseState {
    request_line,
    headers,
    body,
    complete,
    error
  };

  class RequestParser {
  private:
    std::string buffer_;
    ParseState state_{ParseState::request_line};

    std::optional<Request> request_;
    std::size_t expected_body_size_{0};

  public:
    [[nodiscard]] auto consume(std::string_view bytes) -> ParseResult;
    
    auto reset() -> void;
  };


}

#endif
