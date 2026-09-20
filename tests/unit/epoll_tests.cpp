#include "server/platform/epoll.h"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <stdexcept>
#include <system_error>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

namespace platform = server::platform;
using namespace std::chrono_literals;

namespace {
struct Pair {
  platform::Fd reader;
  platform::Fd writer;
};

Pair make_pair() {
  int fds[2]{};
  REQUIRE(::socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, fds) == 0);
  return {platform::Fd{fds[0]}, platform::Fd{fds[1]}};
}

void make_ready(const Pair& pair) {
  const char byte = 'x';
  REQUIRE(::write(pair.writer.get(), &byte, 1) == 1);
}

// The timer interrupts the waiting thread. Eventually it also makes the socket
// readable, so a broken timeout retry loop still terminates.
volatile std::sig_atomic_t signal_count = 0;
volatile std::sig_atomic_t ready_after = 0;
volatile std::sig_atomic_t signal_writer = -1;

extern "C" void on_alarm(int) {
  const int saved_errno = errno;
  signal_count = signal_count + 1;
  if (signal_count == ready_after) {
    const char byte = 'x';
    const auto ignored = ::write(signal_writer, &byte, 1);
    (void)ignored;
  }
  errno = saved_errno;
}

class InterruptTimer {
 public:
  InterruptTimer(int writer, int ticks) {
    signal_count = 0;
    ready_after = ticks;
    signal_writer = writer;
    struct sigaction action{};
    action.sa_handler = on_alarm;
    ::sigemptyset(&action.sa_mask);
    if (::sigaction(SIGALRM, &action, &old_action_) == -1) {
      throw std::system_error(errno, std::generic_category(), "sigaction");
    }
    itimerval timer{};
    timer.it_value.tv_usec = 10000;
    timer.it_interval.tv_usec = 10000;
    if (::setitimer(ITIMER_REAL, &timer, &old_timer_) == -1) {
      const int error = errno;
      ::sigaction(SIGALRM, &old_action_, nullptr);
      throw std::system_error(error, std::generic_category(), "setitimer");
    }
  }

  ~InterruptTimer() {
    const itimerval stopped{};
    ::setitimer(ITIMER_REAL, &stopped, nullptr);
    ::sigaction(SIGALRM, &old_action_, nullptr);
    ::setitimer(ITIMER_REAL, &old_timer_, nullptr);
  }

  InterruptTimer(const InterruptTimer&) = delete;
  InterruptTimer& operator=(const InterruptTimer&) = delete;

 private:
  struct sigaction old_action_{};
  itimerval old_timer_{};
};
}  // namespace

TEST_CASE("Epoll add reports readable descriptors and their identity", "[unit][epoll]") {
  platform::Epoll epoll;
  const auto pair = make_pair();
  epoll.add(pair.reader.get(), EPOLLIN);
  std::array<epoll_event, 2> events{};
  REQUIRE(epoll.wait(events, 0) == 0);
  make_ready(pair);
  REQUIRE(epoll.wait(events, 100) == 1);
  REQUIRE(events[0].data.fd == pair.reader.get());
  REQUIRE((events[0].events & EPOLLIN) != 0);
}

TEST_CASE("Epoll rejects duplicate registrations", "[unit][epoll]") {
  platform::Epoll epoll;
  const auto pair = make_pair();
  epoll.add(pair.reader.get(), EPOLLIN);
  REQUIRE_THROWS_AS(epoll.add(pair.reader.get(), EPOLLIN), std::system_error);
}

TEST_CASE("Epoll modify changes watched readiness", "[unit][epoll]") {
  platform::Epoll epoll;
  const auto pair = make_pair();
  std::array<epoll_event, 1> events{};
  epoll.add(pair.reader.get(), EPOLLIN);
  REQUIRE(epoll.wait(events, 0) == 0);
  epoll.modify(pair.reader.get(), EPOLLOUT);
  REQUIRE(epoll.wait(events, 100) == 1);
  REQUIRE(events[0].data.fd == pair.reader.get());
  REQUIRE((events[0].events & EPOLLOUT) != 0);
  epoll.modify(pair.reader.get(), EPOLLIN);
  REQUIRE(epoll.wait(events, 0) == 0);
}

TEST_CASE("Epoll rejects modifying or removing unregistered descriptors", "[unit][epoll]") {
  platform::Epoll epoll;
  const auto pair = make_pair();
  REQUIRE_THROWS_AS(epoll.modify(pair.reader.get(), EPOLLIN), std::system_error);
  REQUIRE_THROWS_AS(epoll.remove(pair.reader.get()), std::system_error);
}

TEST_CASE("Epoll remove stops events without closing the descriptor", "[unit][epoll]") {
  platform::Epoll epoll;
  const auto pair = make_pair();
  std::array<epoll_event, 1> events{};
  epoll.add(pair.reader.get(), EPOLLIN);
  make_ready(pair);
  REQUIRE(epoll.wait(events, 100) == 1);
  epoll.remove(pair.reader.get());
  REQUIRE(epoll.wait(events, 0) == 0);
  REQUIRE(::fcntl(pair.reader.get(), F_GETFD) != -1);
  char byte{};
  REQUIRE(::read(pair.reader.get(), &byte, 1) == 1);
  REQUIRE(byte == 'x');
}

TEST_CASE("Epoll wait polls and expires finite timeouts", "[unit][epoll]") {
  platform::Epoll epoll;
  std::array<epoll_event, 1> events{};
  REQUIRE(epoll.wait(events, 0) == 0);
  const auto start = std::chrono::steady_clock::now();
  REQUIRE(epoll.wait(events, 20) == 0);
  REQUIRE(std::chrono::steady_clock::now() - start >= 20ms);
}

TEST_CASE("Epoll wait rejects an empty buffer", "[unit][epoll]") {
  platform::Epoll epoll;
  REQUIRE_THROWS_AS(epoll.wait({}, 0), std::invalid_argument);
}

TEST_CASE("Epoll wait respects a one element buffer with multiple ready sockets", "[unit][epoll]") {
  platform::Epoll epoll;
  const auto first = make_pair();
  const auto second = make_pair();
  epoll.add(first.reader.get(), EPOLLIN);
  epoll.add(second.reader.get(), EPOLLIN);
  make_ready(first);
  make_ready(second);
  std::array<epoll_event, 1> events{};
  REQUIRE(epoll.wait(events, 100) == 1);
  const int first_ready = events[0].data.fd;
  REQUIRE((first_ready == first.reader.get() || first_ready == second.reader.get()));
  REQUIRE((events[0].events & EPOLLIN) != 0);
  epoll.remove(first_ready);
  REQUIRE(epoll.wait(events, 100) == 1);
  REQUIRE(events[0].data.fd == (first_ready == first.reader.get() ? second.reader.get() : first.reader.get()));
  REQUIRE((events[0].events & EPOLLIN) != 0);
}

TEST_CASE("Epoll finite wait preserves its deadline across signals", "[unit][epoll]") {
  platform::Epoll epoll;
  const auto pair = make_pair();
  epoll.add(pair.reader.get(), EPOLLIN);
  std::array<epoll_event, 1> events{};
  InterruptTimer timer{pair.writer.get(), 100};
  const auto start = std::chrono::steady_clock::now();
  REQUIRE(epoll.wait(events, 100) == 0);
  REQUIRE(std::chrono::steady_clock::now() - start >= 100ms);
  REQUIRE(signal_count > 0);
}

TEST_CASE("Epoll indefinite wait retries signals until readiness", "[unit][epoll]") {
  platform::Epoll epoll;
  const auto pair = make_pair();
  epoll.add(pair.reader.get(), EPOLLIN);
  std::array<epoll_event, 1> events{};
  InterruptTimer timer{pair.writer.get(), 5};
  REQUIRE(epoll.wait(events) == 1);
  REQUIRE(signal_count >= 5);
  REQUIRE(events[0].data.fd == pair.reader.get());
  REQUIRE((events[0].events & EPOLLIN) != 0);
}
