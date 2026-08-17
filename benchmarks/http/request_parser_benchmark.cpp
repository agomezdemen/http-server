#include "core/benchmark_result.h"
#include "request_parser_benchmark.h"
#include "server/http/parser.h"

#include <cstddef>
#include <chrono>

namespace benchmark {
  auto RequestParserBenchmark::measure_reset_cost() -> std::chrono::nanoseconds {
    const auto start{std::chrono::steady_clock::now()};

    for(auto i{0uz}; i < iterations_; ++i)
        parser_.reset();

    const auto end{std::chrono::steady_clock::now()};

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  }

  RequestParserBenchmark::RequestParserBenchmark(ReqParserBenchCase bench_case, std::size_t iterations) : Benchmark{iterations}, bench_case_{bench_case}, parser_{}, reset_time_{measure_reset_cost()} {}

  auto RequestParserBenchmark::lifecycle() -> BenchmarkCaseResult {
    const auto start{std::chrono::steady_clock::now()};

    for(auto i{0uz}; i < iterations_; ++i) {
      server::http::RequestParser parser{};
      (void)parser.consume(complete_request_);
      parser.reset();
    }

    const auto end{std::chrono::steady_clock::now()};  

    const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};

    BenchmarkCaseResult result{};

    result.name = "lifecycle";
    result.iterations = iterations_;
    result.total_time = total;

    return result;
  }

  auto RequestParserBenchmark::complete_request() -> BenchmarkCaseResult {
    const auto start{std::chrono::steady_clock::now()};

    for(auto i{0uz}; i < iterations_; ++i) {
      parser_.reset();
      (void)parser_.consume(complete_request_);
    }

    const auto end{std::chrono::steady_clock::now()};
    
    const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start) - reset_time_};

    BenchmarkCaseResult result{};

    result.name = "complete_request";
    result.iterations = iterations_;
    result.total_time = total;

    return result;
  }

  auto RequestParserBenchmark::fragmented_request() -> BenchmarkCaseResult {
    const auto start{std::chrono::steady_clock::now()};

    for(auto i{0uz}; i < iterations_; ++i) {
      parser_.reset();

      for(const auto chunk : fragmented_request_)
        (void)parser_.consume(chunk);

    }

    const auto end{std::chrono::steady_clock::now()};

    const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start) - reset_time_};

    BenchmarkCaseResult result{};

    result.name = "fragmented_request";
    result.iterations = iterations_;
    result.total_time = total;

    return result;
  }

  auto RequestParserBenchmark::byte_by_byte_request() -> BenchmarkCaseResult { 

    const auto start{std::chrono::steady_clock::now()};
    
    for(auto i{0uz}; i < iterations_; ++i) {
      parser_.reset();

      for(const auto& c : complete_request_)
        (void)parser_.consume(std::string_view{&c, 1});

    }
    
    const auto end{std::chrono::steady_clock::now()};

    const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start) - reset_time_};

    BenchmarkCaseResult result{};

    result.name = "byte_by_byte_request";
    result.iterations = iterations_;
    result.total_time = total;

    return result;
  }

  auto RequestParserBenchmark::malformed_request() -> BenchmarkCaseResult {
    const auto start{std::chrono::steady_clock::now()};

    for(auto i{0uz}; i < iterations_; ++i) {
      parser_.reset();
      (void)parser_.consume(malformed_request_);
    }
    const auto end{std::chrono::steady_clock::now()};

    const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start) - reset_time_};

    BenchmarkCaseResult result{};

    result.name = "malformed_request";
    result.iterations = iterations_;
    result.total_time = total;

    return result;
  }

  auto RequestParserBenchmark::request_with_body() -> BenchmarkCaseResult { 
    const auto start{std::chrono::steady_clock::now()};

    for(auto i{0uz}; i < iterations_; ++i) {
      parser_.reset();
      (void)parser_.consume(request_with_body_);
    }

    const auto end{std::chrono::steady_clock::now()};

    const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start) - reset_time_};

    BenchmarkCaseResult result{};

    result.name = "request_with_body";
    result.iterations = iterations_;
    result.total_time = total;

    return result;
  }

  auto RequestParserBenchmark::set_case(ReqParserBenchCase bench_case) noexcept -> void {
    bench_case_ = bench_case;
  }

  auto RequestParserBenchmark::run() -> BenchmarkResult {
    BenchmarkResult result{};
    result.name = std::string{name()};

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
