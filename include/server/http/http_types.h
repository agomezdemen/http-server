#ifndef HTTP_TYPES_H
#define HTTP_TYPES_H

#include <string>
#include <string_view>

namespace server::http {
  // types
  enum class Method {
    get,
    head,
    options,
    trace,
    put,
    delete_,
    post,
    patch,
    connect,
    unknown
  };

  enum class Status : int {
    ok = 200,
    bad_request = 400,
    not_found = 404,
    method_not_allowed = 405,
    internal_server_error = 500
  };

  struct Header {
    std::string name;
    std::string value;
  };

  enum class Version {
    http_1_1,
    unknown
  };


  // to string functions
  constexpr auto to_string(Method method) noexcept -> std::string_view {
      switch(method) {
        case Method::get:
          return "GET";
        case Method::head:
          return "HEAD";
        case Method::options:
          return "OPTIONS";
        case Method::trace:
          return "TRACE";
        case Method::put:
          return "PUT";
        case Method::delete_:
          return "DELETE";
        case Method::post:
          return "POST";
        case Method::patch:
          return "PATCH";
        case Method::connect:
          return "CONNECT";
        case Method::unknown:
          return "UNKNOWN";
      }

      return "UNKNOWN";
    }

  constexpr auto to_string(Status status) noexcept -> std::string_view {
      switch (status) {
        case Status::ok:
          return "OK";
        case Status::bad_request:
          return "Bad Request";
        case Status::not_found:
          return "Not Found";
        case Status::method_not_allowed:
          return "Method Not Allowed";
        case Status::internal_server_error:
          return "Internal Server Error";
      }

      return "UNKNOWN";
    }


  constexpr auto to_string(Version version) noexcept -> std::string_view {
    if(version == Version::http_1_1)
        return "HTTP/1.1";

    return "UNKNOWN";
  }
  // from strings
  constexpr auto method_from_string(std::string_view str) noexcept -> Method {
      if(str == "GET")
        return Method::get;
      else if(str =="HEAD")
        return Method::head;
      else if(str == "OPTIONS")
        return Method::options;
      else if(str == "TRACE")
        return Method::trace;
      else if(str == "PUT")
        return Method::put;
      else if(str == "DELETE")
        return Method::delete_;
      else if(str == "POST")
        return Method::post;
      else if(str == "PATCH")
        return Method::patch;
      else if(str == "CONNECT")
        return Method::connect;

      return Method::unknown;
    }


  constexpr auto version_from_string(std::string_view version) noexcept -> Version {
    if(version == "HTTP/1.1")
      return Version::http_1_1;

    return Version::unknown;
  }

  // utility
  constexpr auto status_code(Status status) noexcept -> int {
    return static_cast<int>(status);
  }
}
#endif
