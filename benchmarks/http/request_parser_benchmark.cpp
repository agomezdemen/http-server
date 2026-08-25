#include "core/benchmark_result.h"
#include "request_parser_benchmark.h"
#include "server/http/parser.h"

#include <cstddef>
#include <chrono>

namespace benchmark {
  RequestParserBenchmark::RequestParserBenchmark(ReqParserBenchCase bench_case, std::size_t iterations, std::size_t samples) : Benchmark{iterations, samples}, parsers_(iterations_), bench_case_{bench_case} {}

  auto RequestParserBenchmark::reset_parsers() -> void {
    for(auto& parser : parsers_) {
      parser.reset();
    }
  }

  auto RequestParserBenchmark::lifecycle() -> BenchmarkCaseResult {
    BenchmarkCaseResult result{};
    
    result.name = "lifecycle";
    result.iterations = iterations_;
    result.samples_count = samples_;
    result.samples.reserve(samples_);

    for(auto sample{0uz}; sample < samples_; ++sample) {
      const auto start{std::chrono::steady_clock::now()};

      for(auto i{0uz}; i < iterations_; ++i) {
        server::http::RequestParser parser{};
        (void)parser.consume(complete_request_);
      }

      const auto end{std::chrono::steady_clock::now()};

      const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};

      result.samples.push_back(total);
    }

    return result;
  }

  auto RequestParserBenchmark::complete_request() -> BenchmarkCaseResult {
    BenchmarkCaseResult result{};

    result.name = "complete_request";
    result.iterations = iterations_;
    result.samples_count = samples_;
    result.samples.reserve(samples_);

    for(auto sample{0uz}; sample < samples_; ++sample) {
      const auto start{std::chrono::steady_clock::now()};

      for(auto& parser : parsers_)
        (void)parser.consume(complete_request_);

      const auto end{std::chrono::steady_clock::now()};
      
      const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
      
      result.samples.push_back(total);

      reset_parsers();
    }

    return result;
  }

  auto RequestParserBenchmark::fragmented_request() -> BenchmarkCaseResult {
    BenchmarkCaseResult result{};

    result.name = "fragmented_request";
    result.iterations = iterations_;
    result.samples_count = samples_;
    result.samples.reserve(samples_);

    for(auto sample{0uz}; sample < samples_; ++sample) {
      const auto start{std::chrono::steady_clock::now()};

      for(auto& parser : parsers_)
        for(const auto chunk : fragmented_request_)
          (void)parser.consume(chunk);

      const auto end{std::chrono::steady_clock::now()};

      const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    
      result.samples.push_back(total);

      reset_parsers();
    }

    return result;
  }

  auto RequestParserBenchmark::byte_by_byte_request() -> BenchmarkCaseResult { 
    BenchmarkCaseResult result{};

    result.name = "byte_by_byte_request";
    result.iterations = iterations_;
    result.samples_count = samples_;
    result.samples.reserve(samples_);

    for(auto sample{0uz}; sample < samples_; ++sample) {
      const auto start{std::chrono::steady_clock::now()};
      
      for(auto& parser : parsers_)
        for(const auto& c : complete_request_)
          (void)parser.consume(std::string_view{&c, 1});
      
      const auto end{std::chrono::steady_clock::now()};

      const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    
      result.samples.push_back(total);

      reset_parsers();
    }

    return result;
  }

  auto RequestParserBenchmark::malformed_request() -> BenchmarkCaseResult {
    BenchmarkCaseResult result{};

    result.name = "malformed_request";
    result.iterations = iterations_;
    result.samples_count = samples_;
    result.samples.reserve(samples_);

    for(auto sample{0uz}; sample < samples_; ++sample) {
      const auto start{std::chrono::steady_clock::now()};

      for(auto& parser : parsers_)
        (void)parser.consume(malformed_request_);
      
      const auto end{std::chrono::steady_clock::now()};

      const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    
      result.samples.push_back(total);

      reset_parsers();
    }

    return result;
  }

  auto RequestParserBenchmark::request_with_body() -> BenchmarkCaseResult { 
    BenchmarkCaseResult result{};

    result.name = "request_with_body";
    result.iterations = iterations_;
    result.samples_count = samples_;
    result.samples.reserve(samples_);

    for(auto sample{0uz}; sample < samples_; ++sample) {
      const auto start{std::chrono::steady_clock::now()};

      for(auto& parser : parsers_)
        (void)parser.consume(request_with_body_);

      const auto end{std::chrono::steady_clock::now()};

      const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};

      result.samples.push_back(total);

      reset_parsers();
    }

    return result;
  }


  auto RequestParserBenchmark::many_headers() -> BenchmarkCaseResult {
    BenchmarkCaseResult result{};

    result.name = "many_headers";
    result.iterations = iterations_;
    result.samples_count = samples_;
    result.samples.reserve(samples_);

    for(auto sample{0uz}; sample < samples_; ++sample) {
      const auto start{std::chrono::steady_clock::now()};

      for(auto& parser : parsers_)
        (void)parser.consume(many_headers_request_);

      const auto end{std::chrono::steady_clock::now()};

      const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};

      result.samples.push_back(total);

      reset_parsers();
    }

    return result;

  }

  auto RequestParserBenchmark::large_body() -> BenchmarkCaseResult {
    BenchmarkCaseResult result{};

    result.name = "large_body";
    result.iterations = iterations_;
    result.samples_count = samples_;
    result.samples.reserve(samples_);

    for(auto sample{0uz}; sample < samples_; ++sample) {
      const auto start{std::chrono::steady_clock::now()};

      for(auto& parser : parsers_)
        (void)parser.consume(std::string_view{large_body_request_.data(), large_body_request_.size()});

      const auto end{std::chrono::steady_clock::now()};

      const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};

      result.samples.push_back(total);

      reset_parsers();
    }

    return result;

  }

  auto RequestParserBenchmark::fragmented_body() -> BenchmarkCaseResult {
    BenchmarkCaseResult result{};

    result.name = "fragmented_body";
    result.iterations = iterations_;
    result.samples_count = samples_;
    result.samples.reserve(samples_);

    for(auto sample{0uz}; sample < samples_; ++sample) {
      const auto start{std::chrono::steady_clock::now()};

      for(auto& parser : parsers_)
        for(const auto chunk : fragmented_body_request_)
          (void)parser.consume(chunk);

      const auto end{std::chrono::steady_clock::now()};

      const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};

      result.samples.push_back(total);

      reset_parsers();
    }

    return result;

  }

  auto RequestParserBenchmark::long_target() -> BenchmarkCaseResult {
    BenchmarkCaseResult result{};

    result.name = "long_target";
    result.iterations = iterations_;
    result.samples_count = samples_;
    result.samples.reserve(samples_);

    for(auto sample{0uz}; sample < samples_; ++sample) {
      const auto start{std::chrono::steady_clock::now()};

      for(auto& parser : parsers_)
        (void)parser.consume(long_target_request_);

      const auto end{std::chrono::steady_clock::now()};

      const auto total{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};

      result.samples.push_back(total);

      reset_parsers();
    }

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
        result.cases.push_back(many_headers());
        result.cases.push_back(large_body());
        result.cases.push_back(fragmented_body());
        result.cases.push_back(long_target());
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
      case ReqParserBenchCase::many_headers:
        result.cases.push_back(many_headers());
        break;
      case ReqParserBenchCase::large_body:
        result.cases.push_back(large_body());
        break;
      case ReqParserBenchCase::fragmented_body:
        result.cases.push_back(fragmented_body());
        break;
      case ReqParserBenchCase::long_target:
        result.cases.push_back(long_target());
        break;
    }

    return result;
  }

  auto RequestParserBenchmark::name() const -> std::string_view {
    return "Parser benchmark";
  }
}
