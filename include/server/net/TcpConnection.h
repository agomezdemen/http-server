#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "../../platform/fd.h"
#include "endpoint.h"

class TcpConnection {
private:
  Fd sfd_;
  Endpoint ep_;

public:

};

#endif // !TCP_CONNECTION_H
