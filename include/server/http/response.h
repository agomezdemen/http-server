#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <vector>

#include "server/http/http_types.h"

namespace server::http {
// Builds an HTTP response and serializes it into bytes for the socket layer.
class Response {
 private:
  std::vector<Header> headers_;
  std::string body_;
  Status status_;
  Version version_;

 public:
  explicit Response(Status status, Version version = Version::http_1_1);

  auto set_header(std::string name, std::string value) -> void;
  auto set_body(std::string body) -> void;
  [[nodiscard]] auto to_string() const -> std::string;
};
}  // namespace server::http

#endif
