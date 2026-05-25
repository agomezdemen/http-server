#pragma once

#include "server/net/endpoint.hpp"
#include "server/net/socket.hpp"

#include <optional>

namespace http_server::net {

class Listener {
public:
    Listener() = default;
    explicit Listener(Endpoint endpoint);

    [[nodiscard]] auto endpoint() const noexcept -> const Endpoint&;
    [[nodiscard]] auto socket() const noexcept -> const Socket&;
    [[nodiscard]] auto accept() -> std::optional<Socket>;

private:
    Endpoint endpoint_{};
    Socket socket_{};
};

} // namespace http_server::net
