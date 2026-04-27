import cpputils.data_structures;
import std;

using namespace cpputils::data_structures;

namespace {
struct BenchResult {
  std::string name;
  double best_ms;
  double ns_per_element;
  double million_elements_per_second;
  long long checksum;
};

int parse_int_arg(int argc, char** argv, std::string_view name, int default_value) {
  const std::string prefix = std::string(name) + '=';
  for (int i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (arg.starts_with(prefix)) {
      return std::stoi(std::string(arg.substr(prefix.size())));
    }
  }
  return default_value;
}

template <typename Fn>
BenchResult run_benchmark(std::string name, std::size_t elements, int passes, int trials, Fn&& fn) {
  constexpr int warmup_trials = 2;
  double best_ms = std::numeric_limits<double>::max();
  long long checksum = 0;

  for (int trial = 0; trial < warmup_trials + trials; ++trial) {
    const auto start = std::chrono::steady_clock::now();
    checksum = fn();
    const auto end = std::chrono::steady_clock::now();

    if (trial >= warmup_trials) {
      const double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
      best_ms = std::min(best_ms, elapsed_ms);
    }
  }

  const double total_elements = static_cast<double>(elements) * static_cast<double>(passes);
  const double ns_per_element = (best_ms * 1'000'000.0) / total_elements;
  const double million_elements_per_second = total_elements / best_ms / 1'000.0;

  return {std::move(name), best_ms, ns_per_element, million_elements_per_second, checksum};
}

vector<int> build_input(std::size_t elements) {
  vector<int> values(static_cast<int>(elements));
  for (std::size_t index = 0; index < elements; ++index) {
    values[static_cast<int>(index)] = static_cast<int>((index * 17u + 23u) % 1000u);
  }
  return values;
}

long long run_stream_api(const vector<int>& values, int passes) {
  long long checksum = 0;
  for (int pass = 0; pass < passes; ++pass) {
    auto pipeline = values.stream()
                        .filter([](const int& value) { return (value & 1) == 0; })
                        .map([](const int& value) { return value + 1; })
                        .map([](const int& value) { return value * 2; });

    std::optional<int> current = pipeline();
    while (current.has_value()) {
      checksum += current.value();
      current = pipeline();
    }
  }
  return checksum;
}

long long run_std_ranges(const vector<int>& values, int passes) {
  std::vector<int> standard_values;
  standard_values.reserve(static_cast<std::size_t>(values.size()));
  for (const int value : values) {
    standard_values.push_back(value);
  }

  long long checksum = 0;
  for (int pass = 0; pass < passes; ++pass) {
    auto pipeline = standard_values |
                    std::ranges::views::filter([](int value) { return (value & 1) == 0; }) |
                    std::ranges::views::transform([](int value) { return value + 1; }) |
                    std::ranges::views::transform([](int value) { return value * 2; });

    for (int value : pipeline) {
      checksum += value;
    }
  }
  return checksum;
}

void print_result(const BenchResult& result) {
  std::cout << std::left << std::setw(16) << result.name << std::right << std::setw(12)
            << std::fixed << std::setprecision(3) << result.best_ms << " ms  " << std::setw(10)
            << std::setprecision(2) << result.ns_per_element << " ns/elem  " << std::setw(10)
            << result.million_elements_per_second << " Melem/s  checksum=" << result.checksum
            << '\n';
}
}  // namespace

int main(int argc, char** argv) {
  const int element_count = parse_int_arg(argc, argv, "--size", 1 << 20);
  const int passes = parse_int_arg(argc, argv, "--passes", 50);
  const int trials = parse_int_arg(argc, argv, "--trials", 7);

  if (element_count <= 0 || passes <= 0 || trials <= 0) {
    std::cerr << "Expected positive values for --size, --passes, and --trials\n";
    return 1;
  }

  const vector<int> input = build_input(static_cast<std::size_t>(element_count));

  const BenchResult stream_result = run_benchmark("stream API", input.size(), passes, trials,
                                                  [&] { return run_stream_api(input, passes); });
  const BenchResult ranges_result = run_benchmark("std::ranges", input.size(), passes, trials,
                                                  [&] { return run_std_ranges(input, passes); });

  std::cout << "Benchmarking a filter + transform + transform pipeline over " << input.size()
            << " integers, repeated " << passes << " times per trial.\n";
  print_result(stream_result);
  print_result(ranges_result);

  if (stream_result.checksum != ranges_result.checksum) {
    std::cerr << "Checksum mismatch: stream API=" << stream_result.checksum
              << ", std::ranges=" << ranges_result.checksum << '\n';
    return 2;
  }

  const double speedup = ranges_result.best_ms / stream_result.best_ms;
  std::cout << "Speed ratio (std::ranges / stream API): " << std::fixed << std::setprecision(2)
            << speedup << "x\n";
  return 0;
}