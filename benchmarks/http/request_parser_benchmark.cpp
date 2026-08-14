#include "core/benchmark_result.h"
#include "request_parser_benchmark.h"
#include "server/http/parser.h"

#include <cstddef>
#include <chrono>

namespace benchmark {
  RequestParserBenchmark::RequestParserBenchmark(ReqParserBenchCase bench_case, std::size_t iterations) : Benchmark{iterations}, bench_case_{bench_case}, parser_{} {}

  auto RequestParserBenchmark::lifecycle() -> BenchmarkCaseResult {
    std::chrono::nanoseconds total{};

    for(auto i{0uz}; i < iterations_; ++i) {
      const auto start{std::chrono::steady_clock::now()};
      {
        server::http::RequestParser parser{};
        parser.consume(complete_request_);
        parser.reset();
      }
      const auto end{std::chrono::steady_clock::now()};
      
      total += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }

    return BenchmarkCaseResult{
      .name = "lifecycle",
      .iterations = iterations_,
      .total_time = total
    };
  }

  auto RequestParserBenchmark::complete_request() -> BenchmarkCaseResult {
    std::chrono::nanoseconds total{};

    for(auto i{0uz}; i < iterations_; ++i) {
      parser_.reset();

      const auto start{std::chrono::steady_clock::now()};
      parser_.consume(complete_request_);
      const auto end{std::chrono::steady_clock::now()};

      total += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }
    
    return BenchmarkCaseResult{
      .name = "complete_request",
      .iterations = iterations_,
      .total_time = total
    };
  }

  auto RequestParserBenchmark::fragmented_request() -> BenchmarkCaseResult {
    std::chrono::nanoseconds total{}; 

    for(auto i{0uz}; i < iterations_; ++i) {
      parser_.reset();

      const auto start{std::chrono::steady_clock::now()};
      for(const auto chunk : fragmented_request_)
        parser_.consume(chunk);
      const auto end{std::chrono::steady_clock::now()};

      total += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }

    return BenchmarkCaseResult{
      .name = "fragmented_request",
      .iterations = iterations_,
      .total_time = total
    };
  }

  auto RequestParserBenchmark::byte_by_byte_request() -> BenchmarkCaseResult {
    std::chrono::nanoseconds total{};
    
    for(auto i{0uz}; i < iterations_; ++i) {
      parser_.reset();

      const auto start{std::chrono::steady_clock::now()};
      for(const auto& c : complete_request_)
        parser_.consume(std::string_view{&c, 1});
      const auto end{std::chrono::steady_clock::now()};

      total += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }

    return BenchmarkCaseResult{
      .name = "byte_by_byte_request",
      .iterations = iterations_,
      .total_time = total
    };
  }

  auto RequestParserBenchmark::malformed_request() -> BenchmarkCaseResult {
    std::chrono::nanoseconds total{};

    for(auto i{0uz}; i < iterations_; ++i) {
      parser_.reset();

      const auto start{std::chrono::steady_clock::now()};
      parser_.consume(malformed_request_);
      const auto end{std::chrono::steady_clock::now()};

      total += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }

    return BenchmarkCaseResult{
      .name = "malformed_request",
      .iterations = iterations_,
      .total_time = total
    };
  }

  auto RequestParserBenchmark::request_with_body() -> BenchmarkCaseResult {
    std::chrono::nanoseconds total{};
   
    for(auto i{0uz}; i < iterations_; ++i) {
      parser_.reset();

      const auto start{std::chrono::steady_clock::now()};
      parser_.consume(request_with_body_);
      const auto end{std::chrono::steady_clock::now()};

      total += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }

    return BenchmarkCaseResult{
      .name = "request_with_body",
      .iterations = iterations_,
      .total_time = total
    };
  }

  auto RequestParserBenchmark::set_case(ReqParserBenchCase bench_case) noexcept -> void {
    bench_case_ = bench_case;
  }

  auto RequestParserBenchmark::run() -> BenchmarkResult {
    BenchmarkResult result{.name = std::string{name()}};

    switch(bench_case_) {
      case ReqParserBenchCase::all:
        result.cases.push_back(lifecycle());
        result.cases.push_back(complete_request());
        result.cases.push_back(fragmented_request());
        result.cases.push_back(byte_by_byte_request());
        result.cases.push_back(malformed_request());
        result.cases.push_back(request_with_body());
        break;
      case ReqParserBenchCase::lifecycle:
        result.cases.push_back(lifecycle());
        break;
      case ReqParserBenchCase::complete_request:
        result.cases.push_back(complete_request());
        break;
      case ReqParserBenchCase::fragmented_request:
        result.cases.push_back(fragmented_request());
        break;
      case ReqParserBenchCase::byte_by_byte_request:
        result.cases.push_back(byte_by_byte_request());
        break;
      case ReqParserBenchCase::malformed_request:
        result.cases.push_back(malformed_request());
        break;
      case ReqParserBenchCase::request_with_body:
        result.cases.push_back(request_with_body());
        break;
    }

    return result;
  }

  auto RequestParserBenchmark::name() const -> std::string_view {
    return "Parser benchmark";
  }
}
