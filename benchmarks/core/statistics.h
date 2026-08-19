#ifndef STATISTICS_H
#define STATISTICS_H

#include <span>
#include <chrono>
#include <cstddef>

namespace benchmark {
  auto mean(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double;
  auto median(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double;
  auto standard_deviation(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double;
  auto percentile(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations, double percentile) -> double;
}

#endif
