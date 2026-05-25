#pragma once

#include "server/net/endpoint.hpp"
#include "server/server/event_loop.hpp"

namespace http_server::server {

struct Config {
    net::Endpoint endpoint{};
};

class Server {
public:
    explicit Server(Config config = {});

    [[nodiscard]] auto config() const noexcept -> const Config&;
    void run_once();

private:
    Config config_{};
    EventLoop event_loop_{};
};

} // namespace http_server::server
