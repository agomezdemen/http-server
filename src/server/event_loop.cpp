#include "server/server/event_loop.hpp"

namespace http_server::server {

void EventLoop::run_once()
{
    static_cast<void>(epoll_.wait(0));
}

} // namespace http_server::server
