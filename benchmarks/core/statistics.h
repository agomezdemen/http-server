#ifndef STATISTICS_H
#define STATISTICS_H

#include "benchmark_result.h"

#include <span>
#include <chrono>
#include <cstddef>

namespace benchmark {
  struct BenchmarkStatistics {
    double mean;
    double median;
    double min;
    double max;
    double standard_deviation;
    double p95;
    double p99;
    double operations_per_second;
  };

  auto mean(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double;
  auto median(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double;
  auto standard_deviation(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double;
  auto percentile(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations, double percentile) -> double;

  auto calculate_statistics(const BenchmarkCaseResult& result) -> BenchmarkStatistics;

}

#endif
