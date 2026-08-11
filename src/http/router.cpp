#include "server/http/router.h"

namespace server::http {
  auto Router::add_route_impl(Method method, std::string path, Handler handler) -> bool {
    RouteKey key{method, std::move(path)};

    return routes_.try_emplace(std::move(key), std::move(handler)).second;
  }

  auto Router::route(const Request& request) const -> std::optional<Response> {
    RouteKey key{request.get_method(), std::string{request.get_path()}};

    const auto route_it{routes_.find(key)};

    if(route_it == routes_.end())
      return std::nullopt;
    
    return std::invoke(route_it->second, request);
  }  
}
