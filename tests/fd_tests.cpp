#include "../include/platform/fd.h"
#include <catch2/catch_test_macros.hpp>
#include <cerrno>
#include <fcntl.h>
#include <type_traits>
#include <unistd.h>
#include <utility>

namespace {
  
  struct Pipe {
    int read_end;
    int write_end;
  };

  Pipe make_pipe() {
    int fds[2] {};

    REQUIRE(::pipe(fds) == 0);

    return Pipe{
      .read_end = fds[0],
      .write_end = fds[1],
    };
  }

  bool fd_is_closed(int fd) {
    errno = 0;
    const int result = ::fcntl(fd, F_GETFD);

    return result == -1 && errno == EBADF;
  }

  bool fd_is_open(int fd) {
    return !fd_is_closed(fd);
  }

}


TEST_CASE("Default constructed Fd is invalid") {
  Fd fd{};

  REQUIRE_FALSE(fd.valid());
  REQUIRE(fd.get() == -1);
}

TEST_CASE("Fd constructed from raw fd owns the fd") {
  const auto pipe = make_pipe();

  {
    Fd fd{pipe.read_end};

    REQUIRE(fd.valid());
    REQUIRE(fd.get() == pipe.read_end);
    REQUIRE(fd_is_open(pipe.read_end));
  }

  REQUIRE(fd_is_closed(pipe.read_end));

  REQUIRE(::close(pipe.write_end) == 0);
}

TEST_CASE("Fd destructor closes owned fd") {
  const auto pipe = make_pipe();
  const int raw_fd = pipe.read_end;

  {
    Fd fd{raw_fd};

    REQUIRE(fd.valid());
    REQUIRE(fd_is_open(raw_fd));
  }

  REQUIRE(fd_is_closed(raw_fd));
  REQUIRE(::close(pipe.write_end) == 0);
}

TEST_CASE("Fd release returns fd and invalidates wrapper without closing fd") {
  const auto pipe = make_pipe();
  
  Fd fd{pipe.read_end};
  
  const int released_fd = fd.release();

  REQUIRE(released_fd == pipe.read_end);
  REQUIRE_FALSE(fd.valid());
  REQUIRE(fd.get() == -1);
  REQUIRE(fd_is_open(released_fd));

  REQUIRE(::close(released_fd) == 0);
  REQUIRE(::close(pipe.write_end) == 0);
}

TEST_CASE("Fd move constructor transfers ownership") {
    const auto pipe = make_pipe();
    const int raw_fd = pipe.read_end;

    Fd original{raw_fd};
    Fd moved{std::move(original)};

    REQUIRE_FALSE(original.valid());
    REQUIRE(original.get() == -1);

    REQUIRE(moved.valid());
    REQUIRE(moved.get() == raw_fd);
    REQUIRE(fd_is_open(raw_fd));

    REQUIRE(::close(pipe.write_end) == 0);
}

TEST_CASE("Fd move assignment transfers ownership") {
    const auto pipe = make_pipe();
    const int raw_fd = pipe.read_end;

    Fd source{raw_fd};
    Fd destination{};

    destination = std::move(source);

    REQUIRE_FALSE(source.valid());
    REQUIRE(source.get() == -1);

    REQUIRE(destination.valid());
    REQUIRE(destination.get() == raw_fd);
    REQUIRE(fd_is_open(raw_fd));

    REQUIRE(::close(pipe.write_end) == 0);
}

TEST_CASE("Fd move assignment closes destination's old fd") {
    const auto old_pipe = make_pipe();
    const auto new_pipe = make_pipe();

    const int old_fd = old_pipe.read_end;
    const int new_fd = new_pipe.read_end;

    Fd destination{old_fd};
    Fd source{new_fd};

    destination = std::move(source);

    REQUIRE_FALSE(source.valid());
    REQUIRE(source.get() == -1);

    REQUIRE(destination.valid());
    REQUIRE(destination.get() == new_fd);

    REQUIRE(fd_is_closed(old_fd));
    REQUIRE(fd_is_open(new_fd));

    REQUIRE(::close(old_pipe.write_end) == 0);
    REQUIRE(::close(new_pipe.write_end) == 0);
}

TEST_CASE("Fd self move assignment does not break ownership") {
    const auto pipe = make_pipe();
    const int raw_fd = pipe.read_end;

    Fd fd{raw_fd};

    fd = std::move(fd);

    REQUIRE(fd.valid());
    REQUIRE(fd.get() == raw_fd);
    REQUIRE(fd_is_open(raw_fd));

    REQUIRE(::close(pipe.write_end) == 0);
}

TEST_CASE("Fd has correct copy and move properties") {
    static_assert(!std::is_copy_constructible_v<Fd>);
    static_assert(!std::is_copy_assignable_v<Fd>);

    static_assert(std::is_move_constructible_v<Fd>);
    static_assert(std::is_move_assignable_v<Fd>);

    static_assert(std::is_nothrow_move_constructible_v<Fd>);
    static_assert(std::is_nothrow_move_assignable_v<Fd>);
}
