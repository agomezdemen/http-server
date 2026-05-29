#ifndef TCP_LISTENER
#define TCP_LISTENER
#include "../../platform/fd.h"
#include "endpoint.h"

class TcpListener {
private:
  Fd sfd_; // socket fd
  Endpoint ep_; // endpoint

public:
  explicit TcpListener(Endpoint ep, int backlog = 128);

  TcpListener(const TcpListener&) = delete;
  auto operator=(const TcpListener&) -> TcpListener& = delete;

  auto accept() -> Fd;

  auto fd() const noexcept -> int;
  auto endpoint() const noexcept -> const Endpoint&;
};

#endif // !TCP_LISTENER
