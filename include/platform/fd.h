#ifndef FD_H
#define FD_H

// Owns one POSIX file descriptor and closes it when the wrapper goes away.
// The handle is movable, but not copyable, so ownership stays clear.
class Fd {
private:
  int fd_;

public:
  Fd() noexcept;
  explicit Fd(int fd) noexcept;

  Fd(Fd &&other) noexcept;
  Fd(const Fd &) = delete;

  auto operator=(const Fd &) -> Fd& = delete;
  auto operator=(Fd &&other) noexcept -> Fd&;

  ~Fd() noexcept;

  auto valid() const noexcept -> bool;
  auto get() const noexcept -> int;

  // Gives the raw descriptor back to the caller without closing it.
  auto release() noexcept -> int;
};

#endif
