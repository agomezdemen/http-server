#include "server/net/socket.hpp"

#include <utility>

namespace http_server::net {

Socket::Socket(Fd fd) noexcept : fd_{std::move(fd)} {}

auto Socket::fd() const noexcept -> int
{
    return fd_.get();
}

auto Socket::valid() const noexcept -> bool
{
    return fd_.valid();
}

} // namespace http_server::net
