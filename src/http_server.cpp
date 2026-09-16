#include "server/http_server.h"
#include "server/http/parser.h"
#include "server/http/response.h"
#include "server/net/endpoint.h"
#include "server/net/tcp_connection.h"
#include "server/net/tcp_listener.h"

#include <print>
#include <array>
#include <utility>
#include <string_view>

namespace server {
  
  HttpServer::HttpServer(net::Endpoint endpoint, http::Router router, int backlog) : listener_{std::move(endpoint), backlog}, router_{std::move(router)} {}

  auto HttpServer::handle_connection(net::TcpConnection connection) -> void {
    http::RequestParser parser{};
    auto parse_result{http::ParseStatus::need_more_data};
    auto peer_closed{false};

    while(parse_result == http::ParseStatus::need_more_data) {
      std::array<char, 4096> buffer{};

      const auto n{connection.read(buffer)};
      if(n == 0uz) {
        peer_closed = true;
        break;
      }

      const std::string_view bytes{buffer.data(), n};
      parse_result = parser.consume(bytes);
    }

    if(peer_closed)
      return;

    http::Response response{http::Status::internal_server_error};

    if(parse_result == http::ParseStatus::complete) {
      const auto request{parser.take_request()};

      if(!request.has_value())
        return;

      auto routed_response{router_.route(*request)};

      if(routed_response.has_value()) {
        response = std::move(*routed_response);
      } else {
        response = http::Response{http::Status::not_found};
        response.set_body("Not Found");
      }
    } else {
      response = http::Response{http::Status::bad_request};
      response.set_body("Bad Request");
    }

    const auto serialized{response.to_string()};
    // TODO: Queue the response and track partial writes in per-connection state.

    std::println("response size: {}", serialized.size());
  }

  auto HttpServer::run() -> void { 
    std::println("Listening on {}", listener_.endpoint().to_string());

    while(true) {
      auto connection{listener_.accept()};

      try {
        handle_connection(std::move(connection));
      } catch(const std::exception& e) {
        std::println("error: {}", e.what());
      } 
    }
  }

}
