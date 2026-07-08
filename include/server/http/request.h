#ifndef REQUEST_H
#define REQUEST_H

#include "http_types.h"
#include <string>
#include <vector>

namespace server::http {
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
}

#endif
