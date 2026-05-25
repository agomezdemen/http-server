#pragma once

#include "server/net/fd.hpp"

#include <vector>

namespace http_server::net {

struct EpollEvent {
    int fd{-1};
    unsigned int events{0};
};

class Epoll {
public:
    Epoll() = default;

    [[nodiscard]] auto valid() const noexcept -> bool;
    [[nodiscard]] auto wait(int timeout_ms) -> std::vector<EpollEvent>;

private:
    Fd fd_{};
};

} // namespace http_server::net
