#include "server/server/server.hpp"

#include <utility>

namespace http_server::server {

Server::Server(Config config) : config_{std::move(config)} {}

auto Server::config() const noexcept -> const Config&
{
    return config_;
}

void Server::run_once()
{
    event_loop_.run_once();
}

} // namespace http_server::server
