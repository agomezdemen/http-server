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
  Fd();                                // empty constructor
  explicit Fd(int);                   // value constructor
  Fd(Fd &&) noexcept;                 // constructor
  Fd(const Fd &) = delete;            // deleting copy constructor
  Fd &operator=(const Fd &) = delete; // delting copy assignment
  Fd &operator=(Fd &&) noexcept;      // move assignment
  ~Fd() noexcept;                               // constructor
  bool valid() const noexcept; // checks validity of the Fd object
  int get() const noexcept; // get sthe current fd handle
  int release() noexcept; // releases the resource
};

#endif
