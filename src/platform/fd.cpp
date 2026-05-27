#include "../../include/platform/fd.h"
#include <unistd.h>

Fd::Fd() {
  fd_ = -1;
}

Fd::Fd(int input_fd) : fd_{input_fd} {}

Fd::Fd(Fd &&other) noexcept : fd_{other.fd_} {
  other.fd_ = -1;
}

auto Fd::operator=(Fd &&other) noexcept -> Fd& {
  if(this != &other){
    if(fd_ != -1)
      ::close(fd_);
    fd_ = other.fd_;
    other.fd_ = -1;
  }

  return *this;
}

Fd::~Fd() noexcept {
  if(fd_ != -1) {
    ::close(fd_);
    fd_ = -1;
  }
}

bool Fd::valid() const noexcept {
  return (fd_ != -1);
}

int Fd::get() const noexcept {
  return fd_;
}

int Fd::release() noexcept {
  int fd_copy = fd_;
  fd_ = -1;
  return fd_copy;
}
