#ifndef EPOLL_H
#define EPOLL_H

#include "server/platform/fd.h"

#include <span>
#include <cstdint>
#include <cstddef>
#include <sys/epoll.h>

namespace server::platform {

  class Epoll {
  private:
    Fd fd_;

  public:
    Epoll();

    auto add(int fd, std::uint32_t events) -> void;
    auto modify(int fd, std::uint32_t events) -> void;
    auto remove(int fd) -> void;

    auto wait(std::span<epoll_event> events, int timeout_ms = -1) -> std::size_t;
  };

}

#endif
