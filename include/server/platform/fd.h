#ifndef FD_H
#define FD_H

namespace server::platform {

class Fd {
 private:
  int fd_;

 public:
  Fd() noexcept;
  explicit Fd(int fd) noexcept;

  Fd(Fd&& other) noexcept;
  Fd(const Fd&) = delete;

  auto operator=(const Fd&) -> Fd& = delete;
  auto operator=(Fd&& other) noexcept -> Fd&;

  ~Fd() noexcept;

  auto valid() const noexcept -> bool;
  auto get() const noexcept -> int;

  // Gives the raw descriptor back to the caller without closing it.
  auto release() noexcept -> int;
};

}  // namespace server::platform

#endif
