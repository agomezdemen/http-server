#include "../../include/server/http/request_grammar.h"

#include <expected>
#include <string_view>
#include "../../include/server/http/http_types.h"

namespace server::http {
 
  auto parse_request_line(std::string_view line) -> std::expected<RequestLine, GrammarError> {
    if(line.empty())
      return std::unexpected{GrammarError::empty_request_line}; 

    constexpr auto delimiter{' '};
    
    const auto space_1{line.find(delimiter)};
    if(space_1 == std::string_view::npos)
      return std::unexpected{GrammarError::missing_separator};


    auto method{method_from_string(line.substr(0uz, space_1))};
    if(method == Method::unknown)
      return std::unexpected{GrammarError::invalid_method};


    const auto space_2{line.find(delimiter, space_1 + 1)};
    if(space_2 == std::string_view::npos)
      return std::unexpected{GrammarError::missing_separator};

    
    auto target{line.substr(space_1 + 1, space_2 - space_1 - 1)};
    if(target.empty())
      return std::unexpected{GrammarError::empty_target};


    auto version{version_from_string(line.substr(space_2 + 1))};
    if(version == Version::unknown)
      return std::unexpected{GrammarError::invalid_version};


    return RequestLine{.method = method, 
                       .target = std::string{target},
                       .version = version};
  }

  auto parse_header(std::string_view header) -> std::expected<Header, GrammarError> {
    if(header.empty())
      return std::unexpected{GrammarError::empty_header};

    constexpr auto delimiter{':'};
    
    auto colon{header.find(delimiter)};
    if(colon == std::string_view::npos)
      return std::unexpected{GrammarError::missing_colon};


    auto name{header.substr(0uz, colon)};
    if(name.empty())
      return std::unexpected{GrammarError::empty_header_name};

    auto value{header.substr(colon + 1)};
    
    while (!value.empty() &&
           (value.front() == ' ' || value.front() == '\t')) {
        value.remove_prefix(1);
    }

    while (!value.empty() &&
           (value.back() == ' ' || value.back() == '\t')) {
        value.remove_suffix(1);
    }
    
    return Header{.name = std::string{name},
                  .value = std::string{value}};
  }

}
