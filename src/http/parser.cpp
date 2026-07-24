#include "../../include/server/http/parser.h"

#include <charconv>
#include <algorithm>
#include <utility>

namespace server::http {

  auto RequestParser::process_request_line_state() -> ParseStatus {
    const auto line_end{buffer_.find("\r\n")};
    if(line_end == std::string::npos)
      return ParseStatus::need_more_data;

    const std::string_view line{buffer_.data(), line_end};
    const auto result{parse_request_line(line)};

    if(!result.has_value()) {
      state_ = ParseState::error;
      return ParseStatus::invalid;
    }

    request_ = Request{result->method,
                       result->target,
                       result->version};

    buffer_.erase(0uz, line_end + 2);
    state_ = ParseState::headers;
   
    return ParseStatus::progress;
  }

  auto RequestParser::process_headers_state() -> ParseStatus {
    if(buffer_.starts_with("\r\n")) {
      buffer_.erase(0uz, 2uz);

      if(!expected_body_size_ || *expected_body_size_ == 0uz) {
        state_ = ParseState::complete;
        return ParseStatus::complete;
      }

      state_ = ParseState::body;
      return ParseStatus::progress;
    }

    const auto header_end{buffer_.find("\r\n")};
    if(header_end == std::string::npos)
      return ParseStatus::need_more_data;

    const std::string_view header{buffer_.data(), header_end};

    const auto result{parse_header(header)};
    if(!result.has_value()) {
      state_ = ParseState::error;
      return ParseStatus::invalid;
    }

    if(result->name == "Content-Length") {
      if(expected_body_size_.has_value() || result->value.empty()) {
        state_ = ParseState::error;
        return ParseState::invalid;
      }
      
      std::size_t body_size{};

      const auto first{result->value.data()};
      const auto last{first + result->value.size()};
      const auto [ptr, error]{std::from_chars(first, last, body_size)};

      if(error != std::errc{} || ptr != last) {
        state_ = ParseState::error;
        return ParseStatus::invalid;
      }

      *expected_body_size_ = body_size;
    }

    request_->set_header(result->name, result->value);
    buffer_.erase(0uz, header_end + 2uz);
    
    return ParseStatus::progress;
  }

  auto RequestParser::process_body_state() -> ParseStatus {
    const auto body_size{*expected_body_size_};

    if(buffer_.size() < body_size)
      return ParseStatus::need_more_data;

    request_->set_body(buffer_.substr(0uz, body_size));
    buffer_.erase(0uz, expected_body_size_);

    state_ = ParseState::complete;
    return ParseStatus::complete;
  }

  auto RequestParser::process_current_state() -> ParseStatus {
    switch(state_) {
      case ParseState::request_line:
        return process_request_line_state();
      case ParseState::headers:
        return process_headers_state();
      case ParseState::body:
        return process_body_state(); 
      case ParseState::complete:
        return ParseStatus::complete; 
      case ParseState::error:
        return ParseStatus::invalid; 
    }
  }

  auto RequestParser::consume(std::string_view bytes) -> ParseStatus {
    buffer_ += bytes;

    while(true) {
      const auto status{process_current_state()};

      if(status != ParseStatus::progress)
        return status;
    }
  }

  auto RequestParser::take_request() -> std::optional<Request> {
    if(state_ != ParseState::complete)
      return std::nullopt;

    return std::exchange(request_, std::nullopt);
  }

  auto RequestParser::reset() -> void {
    buffer_.clear();
    state_ = ParseState::request_line; 
    request_ = std::nullopt;
    expected_body_size_ = std::nullopt; 
  }
}
