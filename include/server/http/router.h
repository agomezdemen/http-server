#ifndef HTTP_ROUTER_H
#define HTTP_ROUTER_H

#include "http_types.h"
#include "request.h"
#include "response.h"

#include <compare>
#include <concepts>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

namespace server::http {
  template<typename F>
  concept RouteHandler =
    std::invocable<F&, const Request&> &&
    std::same_as<std::invoke_result_t<F&, const Request&>, Response>;


  class Router {
  private:
    using Handler = std::function<Response(const Request&)>;

    struct RouteKey {
      Method method;
      std::string path;
    
      auto operator<=>(const RouteKey&) const = default;
    };

    std::map<RouteKey, Handler> routes_;

    auto add_route_impl(Method method, std::string path, Handler handler) -> bool;

  public:
    template<RouteHandler F>
    auto add_route(Method method, std::string path, F&& handler) -> bool {
      return add_route_impl(method, std::move(path), Handler{std::forward<F>(handler)});
    }

    auto route(const Request& request) const -> std::optional<Response>;

  };
}

#endif
