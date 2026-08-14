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
    std::size_t samples;

    std::chrono::nanoseconds total_time;

    double mean_ns;
    double median_ns;
    double min_ns;
    double max_ns;
    double standard_deviation_ns;

    double p95_ns;
    double p99_ns;

    double operations_per_second;
  };

  struct BenchmarkResult {
    std::string name;
    std::vector<BenchmarkCaseResult> cases;
  };
}

#endif
