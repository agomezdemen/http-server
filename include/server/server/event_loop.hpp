#pragma once

#include "server/net/epoll.hpp"

namespace http_server::server {

class EventLoop {
public:
    void run_once();

private:
    net::Epoll epoll_{};
};

} // namespace http_server::server
