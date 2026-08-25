#include "server/http/parser.h"
#include "server/http/request_grammar.h"

#include <charconv>
#include <algorithm>
#include <utility>

namespace server::http {

  auto RequestParser::process_request_line_state() -> ParseStatus {
    const auto buffer{remaining_buffer()};
    const auto line_end{buffer.find("\r\n")};

    if(line_end == std::string_view::npos)
      return ParseStatus::need_more_data;

    const auto line{buffer.substr(0uz, line_end)};
    const auto result{parse_request_line(line)};

    if(!result.has_value()) {
      state_ = ParseState::error;
      return ParseStatus::invalid;
    }

    request_ = Request{result->method,
                       result->target,
                       result->version};

    cursor_ += line_end + 2;
    state_ = ParseState::headers;

    return ParseStatus::progressed;
  }

  auto RequestParser::process_headers_state() -> ParseStatus {
    const auto buffer{remaining_buffer()};

    if(buffer.starts_with("\r\n")) {
      cursor_ += 2;

      if(!expected_body_size_ || *expected_body_size_ == 0uz) {
        state_ = ParseState::complete;
        return ParseStatus::complete;
      }

      state_ = ParseState::body;
      return ParseStatus::progressed;
    }

    const auto header_end{buffer.find("\r\n")};

    if(header_end == std::string_view::npos)
      return ParseStatus::need_more_data;

    const auto header{buffer.substr(0uz, header_end)};

    const auto result{parse_header(header)};

    if(!result.has_value()) {
      state_ = ParseState::error;
      return ParseStatus::invalid;
    }

    if(result->name == "Content-Length" || result->name == "content-length") {
      if(expected_body_size_.has_value() || result->value.empty()) {
        state_ = ParseState::error;
        return ParseStatus::invalid;
      }

      std::size_t body_size{};

      const auto first{result->value.data()};
      const auto last{first + result->value.size()};
      const auto [ptr, error]{std::from_chars(first, last, body_size)};

      if(error != std::errc{} || ptr != last) {
        state_ = ParseState::error;
        return ParseStatus::invalid;
      }

      expected_body_size_ = body_size;
    }

    request_->set_header(result->name, result->value);
    cursor_ += header_end + 2;

    return ParseStatus::progressed;
  }

  auto RequestParser::process_body_state() -> ParseStatus {
    const auto buffer{remaining_buffer()};
    const auto body_size{*expected_body_size_};

    if(buffer.size() < body_size)
      return ParseStatus::need_more_data;

    request_->set_body(std::string{buffer.substr(0uz, body_size)});
    cursor_ += body_size;

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

    return ParseStatus::invalid;
  }

  auto RequestParser::consume(std::string_view bytes) -> ParseStatus {
    buffer_ += bytes;

    while(true) {
      const auto status{process_current_state()};

      if(status != ParseStatus::progressed)
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
    cursor_ = 0uz;
    state_ = ParseState::request_line;
    request_ = std::nullopt;
    expected_body_size_ = std::nullopt;
  }

  auto RequestParser::remaining_buffer() const noexcept -> std::string_view {
    return std::string_view{buffer_}.substr(cursor_);
  }

  auto RequestParser::is_buffer_empty() const noexcept -> bool {
    return remaining_buffer().empty();
  }

  auto RequestParser::get_state() const noexcept -> ParseState {
    return state_;
  }
}
