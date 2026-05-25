#pragma once

namespace http_server::http {

enum class Status {
    ok = 200,
    bad_request = 400,
    not_found = 404,
    internal_server_error = 500,
};

[[nodiscard]] auto reason_phrase(Status status) noexcept -> const char*;

} // namespace http_server::http
