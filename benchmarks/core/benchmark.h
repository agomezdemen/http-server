#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <cstddef>
#include <string_view>
#include "benchmark_result.h"

namespace benchmark {
  class Benchmark {
  protected:
    std::size_t iterations_;

  public:
    explicit Benchmark(std::size_t iterations) : iterations_{iterations} {}
    virtual ~Benchmark() = default;

    auto set_iterations(std::size_t iterations) noexcept -> void {
      iterations_ = iterations;
    }

    auto iterations() const noexcept -> std::size_t {
      return iterations_;
    }

    virtual auto run() -> BenchmarkResult = 0;
    virtual auto name() const -> std::string_view = 0;
  };
}

#endif
