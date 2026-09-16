#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "server/http/router.h"
#include "server/net/endpoint.h"
#include "server/net/tcp_connection.h"
#include "server/net/tcp_listener.h"

namespace http = server::http;
namespace net = server::net;

namespace server {
  class HttpServer {
  private:
    net::TcpListener listener_;
    http::Router router_;

    auto handle_connection(net::TcpConnection connection) -> void;

  public:
    HttpServer(net::Endpoint endpoint, http::Router router, int backlog = 128);

    auto run() -> void;
  };
}

#endif
