#include "../../include/server/net/endpoint.h"
#include <string>
#include <utility>

Endpoint::Endpoint(std::string host, std::uint16_t port)
  : host_{std::move(host)}, port_{port} {}

auto Endpoint::host() const noexcept -> const std::string& {
  return host_;
}

auto Endpoint::port() const noexcept -> uint16_t {
  return port_;
}

auto Endpoint::to_string() const -> std::string {
  return host_ + ":" + std::to_string(port_);
}
