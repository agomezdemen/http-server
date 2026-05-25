#include "server/net/epoll.hpp"

namespace http_server::net {

auto Epoll::valid() const noexcept -> bool
{
    return fd_.valid();
}

auto Epoll::wait(int /*timeout_ms*/) -> std::vector<EpollEvent>
{
    return {};
}

} // namespace http_server::net
