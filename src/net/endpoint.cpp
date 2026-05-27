#include "../../include/server/net/endpoint.h"
#include <utility>

explicit Endoint(std::string host, std::uint16_t port) 
  : host_{std::move(host)}, port_{port} {}

auto host() const noexcept -> const std::string& {
  return host_;
}

auto port() const noexcept -> uint16_t {
  return port_;
}

auto to_string() const -> std::string {
  return host_ + ":" + std::to_string(port_);
}

