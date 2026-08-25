#ifndef REQUEST_PARSER_BENCHMARK_H
#define REQUEST_PARSER_BENCHMARK_H

#include "core/benchmark.h"
#include "core/benchmark_result.h"
#include "server/http/parser.h"

#include <array>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace benchmark {
  namespace request_parser_benchmark_detail {
    static constexpr std::string_view large_body_header{
      "POST /upload HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "Content-Length: 65536\r\n"
      "\r\n"
    };

    static constexpr std::size_t large_body_size{64 * 1024};
    static constexpr std::size_t body_chunk_size{8 * 1024};

    static constexpr std::size_t large_body_request_size{large_body_header.size() + large_body_size};
    static constexpr std::size_t body_chunk_count{large_body_size / body_chunk_size};

    static constexpr auto make_large_body_request() -> std::array<char, large_body_request_size> {
      std::array<char, large_body_request_size> request{};

      for(auto i{0uz}; i < large_body_header.size(); ++i)
        request[i] = large_body_header[i];

      for(auto i{0uz}; i < large_body_size; ++i)
        request[large_body_header.size() + i] = static_cast<char>('a' + (i % 26));

      return request;
    }

    static constexpr auto large_body_request{make_large_body_request()};

    static constexpr auto make_fragmented_body_request() -> std::array<std::string_view, body_chunk_count + 1> {
      std::array<std::string_view, body_chunk_count + 1> chunks{};

      chunks[0] = std::string_view{large_body_request.data(), large_body_header.size()};

      for(auto i{0uz}; i < body_chunk_count; ++i) {
        chunks[i + 1] = std::string_view{
          large_body_request.data() + large_body_header.size() + (i * body_chunk_size),
          body_chunk_size
        };
      }

      return chunks;
    }

    static constexpr auto fragmented_body_request{make_fragmented_body_request()};
  }

  enum class ReqParserBenchCase : std::uint8_t {
    all,
    lifecycle,
    complete_request,
    fragmented_request,
    byte_by_byte_request,
    malformed_request,
    request_with_body,
    many_headers,
    large_body,
    fragmented_body,
    long_target
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

    static constexpr std::string_view many_headers_request_{
      "GET / HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "User-Agent: http-server-benchmark\r\n"
      "Accept: text/html,application/json\r\n"
      "Accept-Language: en-US,en;q=0.9\r\n"
      "Accept-Encoding: gzip, deflate\r\n"
      "Connection: keep-alive\r\n"
      "Cache-Control: no-cache\r\n"
      "Pragma: no-cache\r\n"
      "Content-Type: application/json\r\n"
      "X-Request-Id: 123456789abcdef\r\n"
      "X-Forwarded-For: 127.0.0.1\r\n"
      "X-Forwarded-Proto: https\r\n"
      "X-Client-Version: 1.0.0\r\n"
      "X-Trace-Id: abcdef0123456789\r\n"
      "X-Session-Id: benchmark-session\r\n"
      "X-Feature-Flag: enabled\r\n"
      "X-Region: us-central\r\n"
      "X-Environment: benchmark\r\n"
      "X-Priority: high\r\n"
      "X-Custom-Header: custom-value\r\n"
      "\r\n"
    };

    static constexpr std::string_view long_target_request_{
      "GET /api/v1/search/resources/items/benchmark/testing/path"
      "?query=modern-cpp-http-server"
      "&category=systems-programming"
      "&language=cpp"
      "&sort=performance"
      "&order=descending"
      "&limit=100"
      "&offset=1000"
      "&include=headers,body,metadata,statistics"
      "&benchmark=request-parser"
      "&architecture=x86-64"
      "&platform=linux"
      "&mode=stress-test"
      " HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "\r\n"
    };

    static constexpr auto large_body_request_{request_parser_benchmark_detail::large_body_request};
    static constexpr auto fragmented_body_request_{request_parser_benchmark_detail::fragmented_body_request};

    std::vector<server::http::RequestParser> parsers_;
    ReqParserBenchCase bench_case_;

    auto reset_parsers() -> void;
  
    auto lifecycle() -> BenchmarkCaseResult;
    auto complete_request() -> BenchmarkCaseResult;
    auto fragmented_request() -> BenchmarkCaseResult;
    auto byte_by_byte_request() -> BenchmarkCaseResult;
    auto malformed_request() -> BenchmarkCaseResult;
    auto request_with_body() -> BenchmarkCaseResult;
    auto many_headers() -> BenchmarkCaseResult;
    auto large_body() -> BenchmarkCaseResult;
    auto fragmented_body() -> BenchmarkCaseResult;
    auto long_target() -> BenchmarkCaseResult;

  public:
    explicit RequestParserBenchmark(ReqParserBenchCase bench_case, std::size_t iterations, std::size_t samples);
    auto set_case(ReqParserBenchCase bench_case) noexcept -> void;
    
    auto run() -> BenchmarkResult override;
    auto name() const -> std::string_view override;
  };
}

#endif
