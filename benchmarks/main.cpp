#include "http/request_parser_benchmark.h"
#include "core/statistics.h"

#include <nlohmann/json.hpp>

#include <string_view>
#include <filesystem>
#include <charconv>
#include <cstddef>
#include <chrono>
#include <format>
#include <fstream>
#include <print>
#include <system_error>

auto main(int argc, char* argv[]) -> int {
  if(argc != 3) {
    std::println("Usage: benchmark <iterations> <samples>");
    return 1;
  }

  const std::string_view arg_1{argv[1]};
  const std::string_view arg_2{argv[2]};

  std::size_t iterations{};
  std::size_t samples{};

  const auto [iter_ptr, iter_ec]{std::from_chars(arg_1.data(), arg_1.data() + arg_1.size(), iterations)};

  if(iter_ec != std::errc{} || iter_ptr != arg_1.data() + arg_1.size()) {
    std::println("invalid iteration count");
    return 1;
  }

  const auto [smpl_ptr, smpl_ec]{std::from_chars(arg_2.data(), arg_2.data() + arg_2.size(), samples)};

  if(smpl_ec != std::errc{} || smpl_ptr != arg_2.data() + arg_2.size()) {
    std::println("invalid sample count");
    return 1;
  }

  if(iterations == 0 || samples == 0) {
    std::println("arguments must be greater than 0");
    return 1;
  }

  benchmark::RequestParserBenchmark rp_bench{benchmark::ReqParserBenchCase::all, iterations, samples};

  const auto result{rp_bench.run()};

  const auto now{std::chrono::system_clock::now()};
  const auto timestamp{std::format("{:%Y-%m-%d_%H-%M-%S}", now)};

  const std::filesystem::path output_dir{
    std::filesystem::path{HTTP_SERVER_ROOT} / "benchmarks" / "results"};

  std::filesystem::create_directories(output_dir);

  auto filename{output_dir / std::format("benchmark_{}.json", timestamp)};

  std::size_t suffix{1};

  while(std::filesystem::exists(filename))
    filename = output_dir / std::format("benchmark_{}_{}.json", timestamp, suffix++);

  nlohmann::json json{
    {"benchmark", result.name},
    {"timestamp", std::format("{:%Y-%m-%d %H:%M:%S}", now)},
    {"iterations", iterations},
    {"samples", samples},
    {"cases", nlohmann::json::array()}
  };

  std::println("{}", result.name);

  for(const auto& ca : result.cases) {
    const auto case_stats{benchmark::calculate_statistics(ca)};

    std::println("name: {}", ca.name);
    std::println("mean: {}", case_stats.mean);
    std::println("median: {}", case_stats.median);
    std::println("min: {}", case_stats.min);
    std::println("max: {}", case_stats.max);
    std::println("standard deviation: {}", case_stats.standard_deviation);
    std::println("p95: {}", case_stats.p95);
    std::println("p99: {}", case_stats.p99);
    std::println("operations per second: {}", case_stats.operations_per_second);

    json["cases"].push_back({
      {"name", ca.name},
      {"mean_ns", case_stats.mean},
      {"median_ns", case_stats.median},
      {"min_ns", case_stats.min},
      {"max_ns", case_stats.max},
      {"standard_deviation_ns", case_stats.standard_deviation},
      {"p95_ns", case_stats.p95},
      {"p99_ns", case_stats.p99},
      {"operations_per_second", case_stats.operations_per_second}
    });
  }

  std::ofstream output{filename};

  if(!output) {
    std::println("failed to open {}", filename.string());
    return 1;
  }

  output << json.dump(2);

  std::println("\nresults written to {}", filename.string());

  return 0;
}
