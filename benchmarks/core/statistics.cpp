#include <span>
#include <cmath>
#include <chrono>
#include <ranges>
#include <vector>
#include <cstddef>
#include <algorithm>

namespace benchmark {

  auto mean(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double {
    if(samples.empty() || iterations == 0)
      return 0.0;

    const auto sample_count{samples.size()};
    double total{0.0};
    
    for(const auto sample : samples)
      total += sample.count();

    return total / static_cast<double>(sample_count) / static_cast<double>(iterations);
  }

  auto median(std::span<const std::chrono::nanoseconds> samples, std::size_t iterations) -> double {
    if(samples.empty() || iterations == 0)
      return 0.0;

    std::vector<std::chrono::nanoseconds> copy{samples.begin(), samples.end()};

    const auto middle{copy.begin() + copy.size() / 2};

    if(copy.size() % 2 != 0)
      return static_cast<double>(middle->count()) / static_cast<double>(iterations);

    const auto lower{std::max_element(copy.begin(), middle)};

    return (static_cast<double>(lower->count()) + static_cast<double>(middle->count()))
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
    if(samples.empty() || iterations == 0)
      return 0.0;

    std::vector<std::chrono::nanoseconds> sorted{samples.begin(), samples.end()};
    std::ranges::sort(sorted);

    const auto rank{static_cast<std::size_t>(
      std::ceil((percentile / 100.0) * static_cast<double>(sorted.size()))
    )};

    const auto index{rank - 1};

    return static_cast<double>(sorted[index].count()) / static_cast<double>(iterations);
  }

}
