export module cpputils.testing;

import std;

namespace cpputils::testing {
class Error : std::exception {
 public:
  explicit Error(std::string message) : message_(std::move(message)) {}

  const std::string& message() const { return message_; }

 private:
  std::string message_;
};

export struct BeforeEach {};
export struct Test {};
export struct AfterEach {};

template <typename T>
consteval auto getMembers() {
  return std::meta::members_of(^^T, std::meta::access_context::unprivileged());
}

consteval auto getAnnotations(std::meta::info member) { return std::meta::annotations_of(member); }

template <typename T>
consteval auto getTests() {
  static constexpr auto members = std::define_static_array(getMembers<T>());

  bool has_before_each = false;
  std::meta::info before_func;
  std::vector<std::meta::info> tests;
  std::vector<std::function<void(const T&)>> test_funcs;
  bool has_after_each = false;
  std::meta::info after_func;

  template for (constexpr auto m : members) {
    if constexpr (std::meta::is_function(m)) {
      static constexpr auto annotations = std::define_static_array(getAnnotations(m));
      template for (constexpr auto a : annotations) {
        if constexpr (std::meta::type_of(a) == ^^Test) {
          tests.push_back(m);
          break;
        } else if constexpr (std::meta::type_of(a) == ^^BeforeEach) {
          before_func = m;
          has_before_each = true;
        } else if constexpr (std::meta::type_of(a) == ^^AfterEach) {
          after_func = m;
          has_after_each = true;
        }
      }
    }
  }

  return std::tuple<bool, std::meta::info, std::vector<std::meta::info>, bool, std::meta::info>(
      has_before_each, before_func, tests, has_after_each, after_func);
}

export void assertEqual(auto expected, auto actual) {
  if (expected != actual) {
    throw Error("Assertion failed: expected " + std::to_string(expected) + ", got " +
                std::to_string(actual));
  }
}

export template <typename T>
  requires(std::is_class_v<T>)
void test(T suite) {
  static constexpr auto result = getTests<T>();
  static constexpr auto tests = std::get<2>(result);

  template for (constexpr auto test : tests) {
    std::cout << "Running test: " << std::meta::identifier_of(test) << '\n';
    if (std::get<0>(result)) {
      try {
        suite.[:std::get<1>(result):]();
      } catch (...) {
        std::cout << "BeforeEach function failed for test " << std::meta::identifier_of(test)
                  << ", skipping...\n";
        continue;  // Skip the test if setup fails
      }
    }
    try {
      suite.[:test:]();
      std::cout << "Test " << std::meta::identifier_of(test) << " passed\n";
    } catch (const Error& e) {
      std::cout << "Test " << std::meta::identifier_of(test)
                << " failed with error: " << e.message() << '\n';
    } catch (const std::exception& e) {
      std::cout << "Test " << std::meta::identifier_of(test)
                << " failed with exception message: " << e.what() << '\n';
    } catch (...) {
      std::cout << "Test " << std::meta::identifier_of(test) << " failed with unknown error\n";
    }
    if (std::get<3>(result)) {
      try {
        suite.[:std::get<4>(result):]();
      } catch (...) {
        std::cout << "AfterEach function failed for test " << std::meta::identifier_of(test)
                  << '\n';
      }
    }
  }
}
}  // namespace cpputils::testing