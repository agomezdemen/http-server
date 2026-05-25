#pragma once

#include "server/http/request.hpp"

#include <optional>
#include <string_view>

namespace http_server::http {

enum class ParseError {
    incomplete,
    invalid_request_line,
};

struct ParseResult {
    std::optional<Request> request{};
    std::optional<ParseError> error{};
};

class Parser {
public:
    [[nodiscard]] auto parse(std::string_view bytes) const -> ParseResult;
};

} // namespace http_server::http
