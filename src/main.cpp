#include "server/server/server.hpp"

auto main() -> int
{
    auto server = http_server::server::Server{};
    server.run_once();
    return 0;
}
