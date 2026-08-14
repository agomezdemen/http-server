#include "http/request_parser_benchmark.h"
#include <print>

auto main() -> int {
  benchmark::RequestParserBenchmark rp_bench{benchmark::ReqParserBenchCase::all, 100};

  const auto result{rp_bench.run()};

  std::println("{}", result.name);

  for(const auto& ca : result.cases) {
    const auto average_ns{static_cast<double>(ca.total_time.count() / ca.iterations)};

    std::println("name: {}", ca.name);
    std::println("iterations: {}", ca.iterations);
    std::println("total time: {} ns", ca.total_time.count());
    std::println("average: {} ns\n", average_ns);
  }

  return 0;
}
