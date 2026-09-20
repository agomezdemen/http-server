#include "server/http_server.h"
#include "server/http/parser.h"
#include "server/http/response.h"
#include "server/net/endpoint.h"
#include "server/net/tcp_connection.h"
#include "server/net/tcp_listener.h"
#include "server/platform/epoll.h"

#include <print>
#include <array>
#include <utility>
#include <string_view>

namespace server {
  
  HttpServer::HttpServer(net::Endpoint endpoint, http::Router router, int backlog) : connections_{}, listener_{std::move(endpoint), backlog}, router_{std::move(router)}, epoll_{} {
      epoll_.add(listener_.fd(), EPOLLIN);
  }

  auto HttpServer::handle_connection(ConnectionState& state) -> bool {
    auto parse_result = http::ParseStatus::need_more_data;
    std::array<char, 4096> buffer{};

    while (parse_result == http::ParseStatus::need_more_data) {
      const auto n = state.connection.read(buffer);
      if (!n.has_value()) {
        return true; // Preserve the parser until the next readable event.
      }
      if (*n == 0) {
        return false;
      }
      parse_result = state.parser.consume(std::string_view{buffer.data(), *n});
    }

    http::Response response{http::Status::internal_server_error};

    if(parse_result == http::ParseStatus::complete) {
      const auto request{state.parser.take_request()};

      if(!request.has_value())
        return false;

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

    state.output = response.to_string();
    state.write_offset = 0;
    epoll_.modify(state.connection.fd(), EPOLLOUT);
    return true;
  }

  auto HttpServer::run() -> void {
    std::println("Listening on {}", listener_.endpoint().to_string());
    std::array<epoll_event, 64> events{};

    while (true) {
      const auto count = epoll_.wait(events);
      for (std::size_t i = 0; i < count; ++i) {
        const int fd = events[i].data.fd;
        const auto flags = events[i].events;

        if (fd == listener_.fd()) {
          while (auto connection = listener_.accept()) {
            const int client_fd = connection->fd();
            connections_.try_emplace(client_fd, ConnectionState{std::move(*connection)});
            try {
              epoll_.add(client_fd, EPOLLIN);
            } catch (...) {
              connections_.erase(client_fd);
              throw;
            }
          }
          continue;
        }

        const auto it = connections_.find(fd);
        if (it == connections_.end()) {
          continue;
        }

        bool close_connection = false;
        try {
          auto& state = it->second;
          if ((flags & EPOLLERR) != 0) {
            close_connection = true;
          } else {
            if (state.output.empty() && (flags & (EPOLLIN | EPOLLHUP)) != 0) {
              close_connection = !handle_connection(state);
            }

            if (!close_connection && !state.output.empty()) {
              while (state.write_offset < state.output.size()) {
                const auto remaining = std::string_view{state.output}.substr(state.write_offset);
                const auto written = state.connection.write(remaining);
                if (!written.has_value()) {
                  break;
                }
                if (*written == 0) {
                  close_connection = true;
                  break;
                }
                state.write_offset += *written;
              }
              close_connection = close_connection || state.write_offset == state.output.size();
            }
            close_connection = close_connection || (flags & EPOLLHUP) != 0;
          }
        } catch (const std::exception& error) {
          std::println("connection error: {}", error.what());
          close_connection = true;
        }

        if (close_connection) {
          connections_.erase(it);
        }
      }
    }
  }
}
