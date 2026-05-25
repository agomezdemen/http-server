#include "server/http/parser.hpp"

#include <utility>

namespace http_server::http {

auto Parser::parse(std::string_view bytes) const -> ParseResult
{
    const auto line_end = bytes.find("\r\n");
    if (line_end == std::string_view::npos) {
        return {.request = std::nullopt, .error = ParseError::incomplete};
    }

    const auto request_line = bytes.substr(0, line_end);
    const auto first_space = request_line.find(' ');
    const auto second_space = first_space == std::string_view::npos
                                  ? std::string_view::npos
                                  : request_line.find(' ', first_space + 1);

    if (first_space == std::string_view::npos || second_space == std::string_view::npos) {
        return {.request = std::nullopt, .error = ParseError::invalid_request_line};
    }

    auto request = Request{};
    request.method = std::string{request_line.substr(0, first_space)};
    request.target = std::string{request_line.substr(first_space + 1, second_space - first_space - 1)};
    request.version = std::string{request_line.substr(second_space + 1)};

    return {.request = std::move(request), .error = std::nullopt};
}

} // namespace http_server::http
