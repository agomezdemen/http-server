#include "server/net/listener.hpp"

#include <utility>

namespace http_server::net {

Listener::Listener(Endpoint endpoint) : endpoint_{std::move(endpoint)} {}

auto Listener::endpoint() const noexcept -> const Endpoint&
{
    return endpoint_;
}

auto Listener::socket() const noexcept -> const Socket&
{
    return socket_;
}

auto Listener::accept() -> std::optional<Socket>
{
    return std::nullopt;
}

} // namespace http_server::net
