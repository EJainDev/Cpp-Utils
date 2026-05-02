export module cpputils.testing;

import std;

namespace cpputils::testing {
export class Error : std::exception {
 public:
  explicit Error(std::string message) : message_(std::move(message)) {}

  const std::string& message() const { return message_; }

 private:
  std::string message_;
};

export template <typename T = int>
struct BeforeEach {};
export template <typename T = int>
struct Test {};
export template <typename T = int>
struct AfterEach {};
export template <typename T = int>
struct BeforeAll {};
export template <typename T = int>
struct AfterAll {};
export template <typename T = int>
struct Disabled {};

export template <typename T>
consteval auto getMembers() {
  return std::meta::members_of(^^T, std::meta::access_context::current());
}

export consteval auto getAnnotations(std::meta::info member) {
  return std::meta::annotations_of(member);
}

template <typename T>
consteval auto getTests() {
  static constexpr auto members = std::define_static_array(getMembers<T>());
  static constexpr auto size = members.size();

  bool has_before_all = false;
  std::meta::info before_all_func;
  bool has_before_each = false;
  std::meta::info before_func;
  std::vector<std::meta::info> tests;
  bool has_after_each = false;
  std::meta::info after_func;
  bool has_after_all = false;
  std::meta::info after_all_func;

  template for (constexpr auto m : members) {
    if constexpr (std::meta::has_identifier(m)) {
      static constexpr auto annotations = std::define_static_array(getAnnotations(m));
      template for (constexpr auto a : annotations) {
        constexpr auto t = std::meta::template_of(std::meta::type_of(a));

        if constexpr (t == ^^Test) {
          tests.push_back(m);
        } else if constexpr (t == ^^BeforeEach) {
          before_func = m;
          has_before_each = true;
        } else if constexpr (t == ^^AfterEach) {
          after_func = m;
          has_after_each = true;
        } else if constexpr (t == ^^BeforeAll) {
          before_all_func = m;
          has_before_all = true;
        } else if constexpr (t == ^^AfterAll) {
          after_all_func = m;
          has_after_all = true;
        }
      }
    }
  }

  return std::tuple<bool, std::meta::info, bool, std::meta::info, std::span<const std::meta::info>,
                    bool, std::meta::info, bool, std::meta::info, std::size_t>(
      has_before_all, before_all_func, has_before_each, before_func,
      std::define_static_array(tests), has_after_each, after_func, has_after_all, after_all_func,
      size);
}

export void assertEqual(auto expected, auto actual) {
  if (expected != actual) {
    throw Error("Assertion failed: expected " + std::to_string(expected) + ", got " +
                std::to_string(actual));
  }
}

export template <typename T>
  requires(std::is_class_v<T>)
void test(T suite = {}) {
  static constexpr auto result = getTests<T>();

  // Manually unpack the tuple for better readability
  static constexpr auto has_before_all = std::get<0>(result);
  static constexpr auto before_all_func = std::get<1>(result);
  static constexpr auto has_before_each = std::get<2>(result);
  static constexpr auto before_each_func = std::get<3>(result);
  static constexpr auto tests = std::get<4>(result);
  static constexpr auto has_after_each = std::get<5>(result);
  static constexpr auto after_each_func = std::get<6>(result);
  static constexpr auto has_after_all = std::get<7>(result);
  static constexpr auto after_all_func = std::get<8>(result);
  static constexpr auto size = tests.size();

  if constexpr (has_before_all) {
    std::cout << "Running BeforeAll setup for suite " << std::meta::identifier_of(^^T) << '\n';
    try {
      suite.[:before_all_func:]();
    } catch (const std::exception& e) {
      std::cout << "BeforeAll setup failed with exception: " << e.what() << ", aborting test suite "
                << std::meta::identifier_of(^^T) << '\n';
      return;  // Abort the entire test suite if BeforeAll setup fails
    } catch (...) {
      std::cout << "BeforeAll setup failed with unknown error, aborting test suite "
                << std::meta::identifier_of(^^T) << '\n';
      return;  // Abort the entire test suite if BeforeAll setup fails
    }
  }

  std::cout << "Running " << size << " tests in suite " << std::meta::identifier_of(^^T) << '\n';

  template for (constexpr auto test : tests) {
    bool disabled = false;
    static constexpr auto annotations = std::define_static_array(getAnnotations(test));

    template for (constexpr auto a : annotations) {
      constexpr auto t = std::meta::template_of(std::meta::type_of(a));

      if constexpr (t == ^^Disabled) {
        std::cout << "Skipping disabled test: " << std::meta::identifier_of(test) << '\n';
        disabled = true;
        break;
      }
    }

    if (disabled) {
      continue;
    }

    std::cout << "Running test: " << std::meta::identifier_of(test) << '\n';
    if constexpr (has_before_each) {
      try {
        suite.[:before_each_func:]();
      } catch (...) {
        std::cout << "BeforeEach function failed for test " << std::meta::identifier_of(test)
                  << ", skipping...\n";
        continue;  // Skip the test if setup fails
      }
    }

    try {
      auto start = std::chrono::system_clock::now();
      suite.[:test:]();
      auto end = std::chrono::system_clock::now();

      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
      std::cout << "Test " << std::meta::identifier_of(test) << " passed in "
                << duration.count() / 1'000'000.0 << " ms\n";
    } catch (const Error& e) {
      std::cout << "Test " << std::meta::identifier_of(test)
                << " failed with error: " << e.message() << '\n';
    } catch (const std::exception& e) {
      std::cout << "Test " << std::meta::identifier_of(test)
                << " failed with (uncaught) exception message: " << e.what() << '\n';
    } catch (...) {
      std::cout << "Test " << std::meta::identifier_of(test) << " failed with unknown error\n";
    }

    if constexpr (has_after_each) {
      try {
        suite.[:after_each_func:]();
      } catch (...) {
        std::cout << "AfterEach function failed for test " << std::meta::identifier_of(test)
                  << '\n';
      }
    }

    if constexpr (has_after_all) {
      std::cout << "Running AfterAll teardown for suite " << std::meta::identifier_of(^^T) << '\n';
      try {
        suite.[:after_all_func:]();
      } catch (const std::exception& e) {
        std::cout << "AfterAll teardown failed with exception: " << e.what() << '\n';
      } catch (...) {
        std::cout << "AfterAll teardown failed with unknown error\n";
      }
    }
  }
}
}  // namespace cpputils::testing