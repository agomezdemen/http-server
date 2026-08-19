#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <cstddef>
#include <string_view>
#include "benchmark_result.h"

namespace benchmark {
  class Benchmark {
  protected:
    std::size_t iterations_;
    std::size_t samples_;

  public:
    explicit Benchmark(std::size_t iterations, std::size_t samples) : iterations_{iterations}, samples_{samples} {}
    virtual ~Benchmark() = default;

    auto iterations() const noexcept -> std::size_t {
      return iterations_;
    }

    auto samples() const noexcept -> std::size_t {
      return samples_;
    }

    virtual auto run() -> BenchmarkResult = 0;
    virtual auto name() const -> std::string_view = 0;
  };
}

#endif
