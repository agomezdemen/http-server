#include "server/server/connection.hpp"

#include <utility>

namespace http_server::server {

Connection::Connection(net::Socket socket) : socket_{std::move(socket)} {}

auto Connection::socket() const noexcept -> const net::Socket&
{
    return socket_;
}

auto Connection::input() const noexcept -> const io::Buffer&
{
    return input_;
}

auto Connection::output() const noexcept -> const io::Buffer&
{
    return output_;
}

} // namespace http_server::server
