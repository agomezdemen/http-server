#pragma once

#include "server/io/buffer.hpp"
#include "server/net/socket.hpp"

namespace http_server::server {

class Connection {
public:
    explicit Connection(net::Socket socket);

    [[nodiscard]] auto socket() const noexcept -> const net::Socket&;
    [[nodiscard]] auto input() const noexcept -> const io::Buffer&;
    [[nodiscard]] auto output() const noexcept -> const io::Buffer&;

private:
    net::Socket socket_;
    io::Buffer input_{};
    io::Buffer output_{};
};

} // namespace http_server::server
