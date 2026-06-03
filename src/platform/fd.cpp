#include "../../include/platform/fd.h"
#include <unistd.h>

Fd::Fd() noexcept : fd_{-1} {}

Fd::Fd(int input_fd) noexcept : fd_{input_fd} {}

Fd::Fd(Fd &&other) noexcept : fd_{other.fd_} {
  // Leave the moved-from object empty so only one wrapper closes the fd.
  other.fd_ = -1;
}

auto Fd::operator=(Fd &&other) noexcept -> Fd& {
  if(this != &other){
    if(fd_ != -1) {
      ::close(fd_);
    }

    fd_ = other.fd_;
    other.fd_ = -1;
  }

  return *this;
}

Fd::~Fd() noexcept {
  if(fd_ != -1) {
    ::close(fd_);
  }
}

auto Fd::valid() const noexcept -> bool {
  return (fd_ != -1);
}

auto Fd::get() const noexcept -> int {
  return fd_;
}

auto Fd::release() noexcept -> int {
  int fd_copy = fd_;
  fd_ = -1;
  return fd_copy;
}
