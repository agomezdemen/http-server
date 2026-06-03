#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <string>
#include <cstdint>

// Stores the address the server should bind or connect to.
// Host is kept as text so parsing can happen at the socket boundary.
class Endpoint {
private:
  std::string host_;
  uint16_t port_;
public:
  Endpoint() = delete;
  Endpoint(std::string host, std::uint16_t port);
  const std::string& host() const noexcept;
  uint16_t port() const noexcept;
  std::string to_string() const;

};

#endif // !ENDPOINT_H
