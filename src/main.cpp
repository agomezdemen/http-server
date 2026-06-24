#include "../include/server/net/tcp_listener.h"
#include "../include/server/net/tcp_connection.h"
#include "../include/server/net/endpoint.h"

#include <array>
#include <iostream>
#include <string_view>

auto main() -> int {
  try {
    Endpoint ep{"127.0.0.1", 8080};
    TcpListener listener{ep};

    std::cout << "Listening on " << ep.to_string() << '\n';

    while(true) {
      auto conn{listener.accept()};

      std::array<char, 4096> buffer{};
      const auto n{conn.read(buffer)};

      std::string_view request{buffer.data(), n};

      std::cout << "Request:\n" << request << '\n';

      constexpr std::string_view response {
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 12\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Hello world!\n"
      };

      const auto bytes_written{conn.write(response)};

      std::cout << "response size: " << response.size() << '\n';
      std::cout << "bytes written: " << bytes_written << '\n';
    }

  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}

