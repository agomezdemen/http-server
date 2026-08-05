#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string_view>
#include <cstdint>
#include <string>


namespace test::integration {
  class HttpClient {
  private:
    int sfd_{-1};

    auto send_all(std::string_view bytes) -> void;
    auto receive_all() -> std::string;

  public:
    HttpClient(std::string_view host, std::uint16_t port);
    ~HttpClient();

    auto send_request(std::string_view request) -> std::string;

    HttpClient(const HttpClient& client) = delete;
    auto operator=(const HttpClient& client) -> HttpClient& = delete;

    HttpClient(HttpClient&& client) = delete;
    auto operator=(HttpClient&& client) -> HttpClient& = delete;
  };
}

#endif
