#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "server/http/router.h"
#include "server/http/parser.h"
#include "server/net/endpoint.h"
#include "server/net/tcp_connection.h"
#include "server/net/tcp_listener.h"
#include "server/platform/epoll.h"

#include <string>
#include <sys/socket.h>
#include <unordered_map>

namespace http = server::http;
namespace net = server::net;
namespace platform = server::platform;

namespace server {
  class HttpServer {
  private:
    struct ConnectionState {
      net::TcpConnection connection;
      http::RequestParser parser{};
      std::string output{};
      std::size_t write_offset{};
    };

    std::unordered_map<int, ConnectionState> connections_;
    net::TcpListener listener_;
    http::Router router_;
    platform::Epoll epoll_;

    // Returns false when the peer closes before completing a request.
    auto handle_connection(ConnectionState& state) -> bool;

  public:
    HttpServer(net::Endpoint endpoint, http::Router router, int backlog = SOMAXCONN);

    auto run() -> void;
  };
}

#endif
