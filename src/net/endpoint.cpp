#include "server/net/endpoint.h"

#include <string>
#include <utility>

namespace server::net {

// Move the host string in so callers can pass either a literal or an owned string.
Endpoint::Endpoint(std::string host, std::uint16_t port) : host_{std::move(host)}, port_{port} {}

auto Endpoint::host() const noexcept -> const std::string& { return host_; }

auto Endpoint::port() const noexcept -> uint16_t { return port_; }

auto Endpoint::to_string() const -> std::string {
  // Used for logs and diagnostics; socket code still reads host and port separately.
  return host_ + ":" + std::to_string(port_);
}

}  // namespace server::net
