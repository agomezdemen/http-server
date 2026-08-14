#ifndef REQUEST_PARSER_BENCHMARK_H
#define REQUEST_PARSER_BENCHMARK_H

#include "core/benchmark.h"
#include "core/benchmark_result.h"
#include "server/http/parser.h"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace benchmark {
  enum class ReqParserBenchCase : std::uint8_t {
    all,
    lifecycle,
    complete_request,
    fragmented_request,
    byte_by_byte_request,
    malformed_request,
    request_with_body
  };

  class RequestParserBenchmark final : public Benchmark {
  private:
    static constexpr std::string_view complete_request_ {
      "GET / HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "\r\n"
    };

    static constexpr std::array<std::string_view, 4> fragmented_request_{
      "GET / HTTP/1.1\r\n",
      "Host: local",
      "host\r\n",
      "\r\n"
    };

    static constexpr std::string_view malformed_request_ {
      "REI f http6\n\r"
      "something: 3920.53059.23\n\r"
    };

    static constexpr std::string_view request_with_body_ {
      "GET / HTTP/1.1\r\n"
      "Content-Length: 12\r\n"
      "\r\n"
      "Hello World!"
    };

    ReqParserBenchCase bench_case_;
    server::http::RequestParser parser_; 

    auto lifecycle() -> BenchmarkCaseResult;
    auto complete_request() -> BenchmarkCaseResult;
    auto fragmented_request() -> BenchmarkCaseResult;
    auto byte_by_byte_request() -> BenchmarkCaseResult;
    auto malformed_request() -> BenchmarkCaseResult;
    auto request_with_body() -> BenchmarkCaseResult;

  public:
    explicit RequestParserBenchmark(ReqParserBenchCase bench_case, std::size_t iterations);
    auto set_case(ReqParserBenchCase bench_case) noexcept -> void;
    
    auto run() -> BenchmarkResult override;
    auto name() const -> std::string_view override;
  };
}

#endif
