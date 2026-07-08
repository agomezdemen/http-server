#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <vector>

#include "http_types.h"

namespace server::http {
// Stores the structured pieces of an HTTP request once parsing is complete.
class Request {
 private:
  std::vector<Header> headers_;
  std::string target_;
  std::string body_;
  Method method_;
  Version version_;

 public:
  explicit Request(Method method, std::string target, Version version) noexcept;

  auto set_header(std::string name, std::string value) -> void;
  auto set_body(std::string body) -> void;
};
}  // namespace server::http

#endif
