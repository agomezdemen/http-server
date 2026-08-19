#ifndef BENCHMARK_RESULT_H
#define BENCHMARK_RESULT_H

#include <string>
#include <cstddef>
#include <vector>
#include <chrono>

namespace benchmark {
    struct BenchmarkCaseResult {
    std::string name;

    std::size_t iterations;
    std::size_t samples_count;

    std::chrono::nanoseconds total_time;

    std::vector<std::chrono::nanoseconds> samples;
  };

  struct BenchmarkResult {
    std::string name;
    std::vector<BenchmarkCaseResult> cases;
  };
}

#endif
