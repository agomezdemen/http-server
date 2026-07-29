#include <array>
#include <iostream>
#include <string>
#include <string_view>

#include "../include/server/http/parser.h"
#include "../include/server/http/response.h"
#include "../include/server/net/endpoint.h"
#include "../include/server/net/tcp_connection.h"
#include "../include/server/net/tcp_listener.h"

namespace http = server::http;

auto main() -> int {
  try {
    Endpoint ep{"127.0.0.1", 8080};
    TcpListener listener{ep};

    std::cout << "Listening on " << ep.to_string() << '\n';

    while(true) {
      auto conn{listener.accept()};

      http::RequestParser parser{};
      auto parse_result{http::ParseStatus::need_more_data};
      auto peer_closed{false};

      while(parse_result == http::ParseStatus::need_more_data) {
        std::array<char, 4096> buffer{};

        const auto n{conn.read(buffer)};
        if(n == 0uz) {
          peer_closed = true;
          break;
        }

        const std::string_view bytes{buffer.data(), n};
        parse_result = parser.consume(bytes);
      }

      if(peer_closed)
        continue;

      http::Response response{http::Status::ok};

      if(parse_result == http::ParseStatus::complete) {
        const auto request{parser.take_request()};

        if(!request.has_value())
          continue;

        response.set_body("Hello world!");
      } else {
        response = http::Response{http::Status::bad_request};
        response.set_body("Bad Request");
      }

      const auto serialized{response.to_string()};
      const auto bytes_written{conn.write_all(serialized)};

      std::cout << "response size: " << serialized.size() << '\n';
      std::cout << "bytes written: " << bytes_written << '\n';
    }
  } catch(const std::exception& e) {
    std::cerr << "error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
