/*
This is the RAII wrapper for file descriptors.
Both the copy constructor and copy assignment are disabled for safety. 
*/

#ifndef FD_H
#define FD_H

class Fd {
private:
  int fd_;

public:
  Fd() noexcept;                                // empty constructor
  explicit Fd(int) noexcept;                   // value constructor
  Fd(Fd &&) noexcept;                 // constructor
  Fd(const Fd &) = delete;            // deleting copy constructor
  auto operator=(const Fd &) -> Fd& = delete; // delting copy assignment
  auto operator=(Fd &&) noexcept -> Fd&;      // move assignment
  ~Fd() noexcept;                               // destructor
  auto valid() const noexcept -> bool; // checks validity of the Fd object
  auto get() const noexcept -> int; // get sthe current fd handle
  auto release() noexcept -> int; // releases the resource
};

#endif
