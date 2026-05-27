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
  Fd(int);                             // value constructor
  Fd(int &&);                          // constructor
  Fd(const int &) = delete;            // deleting copy constructor
  Fd &operator=(const int &) = delete; // delting copy assignment
  Fd &operator=(int &&);               // move assignment
  ~Fd();                               // constructor
};

#endif
