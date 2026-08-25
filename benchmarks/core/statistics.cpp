#include <span>
#include <cmath>
#include <chrono>
#include <ranges>
#include <vector>
#include <cstddef>
#include <algorithm>

#include "core/statistics.h"

namespace benchmark {

  auto mean(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double {
    if(samples.empty() || iterations == 0)
      return 0.0;

    const auto sample_count{samples.size()};
    double total{0.0};
    
    for(const auto sample : samples)
      total += static_cast<double>(sample.count());

    return total / static_cast<double>(sample_count) / static_cast<double>(iterations);
  }

  auto median(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double {
    if(samples.empty() || iterations == 0)
      return 0.0;

    std::vector<std::chrono::nanoseconds> copy{samples.begin(), samples.end()};
    std::ranges::sort(copy);

    const auto middle{copy.size() / 2};

    if(copy.size() % 2 != 0)
      return static_cast<double>(copy[middle].count()) / static_cast<double>(iterations);

    return (static_cast<double>(copy[middle - 1].count()) + static_cast<double>(copy[middle].count()))
      / 2.0
      / static_cast<double>(iterations);
  }

  auto standard_deviation(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double {
    if(samples.size() < 2 || iterations == 0)
      return 0.0;

    const auto avg{mean(samples, iterations)};
    double sum_of_squares{0.0};

    for(const auto sample : samples) {
      const auto value{static_cast<double>(sample.count()) / static_cast<double>(iterations)};
      const auto diff{value - avg};

      sum_of_squares += diff * diff;
    }

    const auto variance{sum_of_squares / static_cast<double>(samples.size() - 1)};

    return std::sqrt(variance);
  }

  auto percentile(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations, double percentile) -> double {
    if(samples.empty() || iterations == 0 || percentile < 0.0 || percentile > 1.0)
      return 0.0;

    std::vector<std::chrono::nanoseconds> sorted{samples.begin(), samples.end()};
    std::ranges::sort(sorted);

    const auto rank{static_cast<std::size_t>(
      std::ceil(percentile * static_cast<double>(sorted.size()))
    )};

    const auto index{rank == 0 ? 0 : rank - 1};

    return static_cast<double>(sorted[index].count()) / static_cast<double>(iterations);
  }


  auto calculate_statistics(const BenchmarkCaseResult& result) -> BenchmarkStatistics {
    if(result.samples.empty() || result.iterations == 0)
      return {};

    const auto mean_ns{mean(result.samples, result.iterations)};

    return BenchmarkStatistics{
      .mean = mean_ns,
      .median = median(result.samples, result.iterations),
      .min = static_cast<double>(std::min_element(result.samples.begin(), result.samples.end())->count())
        / static_cast<double>(result.iterations),
      .max = static_cast<double>(std::max_element(result.samples.begin(), result.samples.end())->count())
        / static_cast<double>(result.iterations),
      .standard_deviation = standard_deviation(result.samples, result.iterations),
      .p95 = percentile(result.samples, result.iterations, 0.95),
      .p99 = percentile(result.samples, result.iterations, 0.99),
      .operations_per_second = mean_ns > 0.0 ? 1'000'000'000.0 / mean_ns : 0.0
    };
  }
}
