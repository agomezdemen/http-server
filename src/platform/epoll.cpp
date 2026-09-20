#include "server/platform/epoll.h"

#include <cerrno>
#include <chrono>
#include <limits>
#include <stdexcept>
#include <system_error>

namespace server::platform {

  Epoll::Epoll() : fd_{::epoll_create1(EPOLL_CLOEXEC)} {
    if(!fd_.valid()) {
      throw std::system_error(errno, std::generic_category(), "epoll_create1 failed");
    }
  }

  auto Epoll::add(int fd, std::uint32_t events) -> void {
    epoll_event event{};
    event.events = events;
    event.data.fd = fd;

    auto result{::epoll_ctl(fd_.get(), EPOLL_CTL_ADD, fd, &event)};

    if(result == -1) {
      throw std::system_error(errno, std::generic_category(), "failed to add");
    }
  }

  auto Epoll::modify(int fd, std::uint32_t events) -> void {
    epoll_event event{};
    event.events = events;
    event.data.fd = fd;

    auto result{::epoll_ctl(fd_.get(), EPOLL_CTL_MOD, fd, &event)};

    if(result == -1) {
      throw std::system_error(errno, std::generic_category(), "failed to modify");
    }
  }

  auto Epoll::remove(int fd) -> void {
    auto result{::epoll_ctl(fd_.get(), EPOLL_CTL_DEL, fd, nullptr)};

    if(result == -1) {
      throw std::system_error(errno, std::generic_category(), "failed to remove");
    }
  }

  auto Epoll::wait(std::span<epoll_event> events, int timeout_ms) -> std::size_t {
    if (events.empty() || events.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
      throw std::invalid_argument("invalid epoll event buffer size");
    }

    using Clock = std::chrono::steady_clock;
    const auto deadline = Clock::now() + std::chrono::milliseconds{timeout_ms > 0 ? timeout_ms : 0};
    int remaining_ms = timeout_ms;

    while (true) {
      const int result = ::epoll_wait(fd_.get(), events.data(), static_cast<int>(events.size()), remaining_ms);

      if (result >= 0) {
        return static_cast<std::size_t>(result);
      }

      const int error = errno;
      if (error != EINTR) {
        throw std::system_error(error, std::generic_category(), "wait failed");
      }

      if (timeout_ms < 0) {
        continue;
      }

      const auto now = Clock::now();
      if (now >= deadline) {
        return 0;
      }

      remaining_ms = static_cast<int>(std::chrono::ceil<std::chrono::milliseconds>(deadline - now).count());
    }
  }
}
