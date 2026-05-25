#pragma once

#include "server/net/fd.hpp"

namespace http_server::net {

class Socket {
public:
    Socket() noexcept = default;
    explicit Socket(Fd fd) noexcept;

    [[nodiscard]] auto fd() const noexcept -> int;
    [[nodiscard]] auto valid() const noexcept -> bool;

private:
    Fd fd_{};
};

} // namespace http_server::net
