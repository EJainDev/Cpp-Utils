module;

#if defined(__unix__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

export module cpputils.testing;

import cpputils.refl_utils;

import std;

using namespace cpputils::refl_utils;

namespace cpputils::testing {
// Functions for implementing the tuple type for parameterized tests
template <std::size_t I>
struct member_name {
  static constexpr auto value = []() {
    if constexpr (I < 10) {
      return std::array<char, 3>{'m', static_cast<char>('0' + I), '\0'};
    } else {
      return std::array<char, 2>{'m', '\0'};  // Simplified for example
    }
  }();
};

template <typename... Ts, std::size_t... Is>
consteval auto make_specs(std::index_sequence<Is...>) {
  // Explicitly typing the vector prevents CTAD failure on empty packs
  return std::vector<std::meta::info>{
      std::meta::data_member_spec(^^Ts, {
                                            .name = member_name<Is>::value.data()})...};
}

export template <typename... Ts>
struct Tuple {
  struct storage;

  consteval {
    auto storage_meta = ^^storage;

    // Generate the specs using an index sequence to keep 'I' constant
    auto specs = make_specs<Ts...>(std::make_index_sequence<sizeof...(Ts)>{});

    std::meta::define_aggregate(storage_meta, specs);
  }

  static constexpr auto nsdms = std::define_static_array(getNonstaticDataMembers<storage>());

  storage s;

  static consteval auto getSizeof() -> std::size_t { return sizeof...(Ts); }
};

export template <typename... Ts>
Tuple(Ts...) -> Tuple<Ts...>;

export consteval auto tuple(auto... args) { return Tuple<decltype(args)...>{.s = {args...}}; }

// Helper function to call a test with all the parameters in a tuple
template <std::size_t N, typename F>
constexpr auto with_indices(const F f) -> decltype(auto) {
  return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> decltype(auto) {
    return f(std::integral_constant<std::size_t, Is>{}...);
  }(std::make_index_sequence<N>{});
}

// An exception class for test failures, to distinguish from other exceptions. It does not inherit
// std::exception to prevent the user from catching it as frequently
class Error {
 public:
  explicit Error(std::string message) : message_(std::move(message)) {}

  const std::string& message() const { return message_; }

 private:
  std::string message_;
};

// An exception class for abort messages that should not be treated as test failures (e.g. failed
// setup)
class Abort {
 public:
  explicit Abort(std::string message) : message_(std::move(message)) {}

  const std::string& message() const { return message_; }

 private:
  std::string message_;
};

namespace detail {
struct DeathResult {
  bool died;
  std::string detail;
};

#if defined(__unix__) || defined(__APPLE__)
template <typename F>
DeathResult runDeathTest(F&& func) {
  const pid_t pid = fork();

  if (pid < 0) {
    return {false, "fork() failed"};
  }

  if (pid == 0) {
    try {
      func();
      std::_Exit(0);
    } catch (...) {
      std::_Exit(1);
    }
  }

  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    return {false, "waitpid() failed"};
  }

  if (WIFSIGNALED(status)) {
    return {true, "terminated by signal " + std::to_string(WTERMSIG(status))};
  }

  if (WIFEXITED(status)) {
    const int exit_code = WEXITSTATUS(status);
    if (exit_code != 0) {
      return {true, "exited with status " + std::to_string(exit_code)};
    }
    return {false, "exited normally with status 0"};
  }

  return {false, "terminated with unknown status"};
}
#endif
}  // namespace detail

// Different annotations recognized by the library
export template <typename T = int>
struct BeforeEach {};
export template <typename T = int>
struct Test {};
export template <typename T = int>
struct DeathTest {};
export template <typename T = int>
struct AfterEach {};
export template <typename T = int>
struct BeforeAll {};
export template <typename T = int>
struct AfterAll {};
export template <typename T = int>
struct Disabled {};

export template <int N, typename... TupleArgs>
  requires(N > 0)
struct Parameterize {
  using TupleType = Tuple<TupleArgs...>;
  TupleType parameters[N];
};

// Gets all required information from the testing class
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

// Assert statements that can be called
export void assertEqual(auto expected, auto actual) {
  if (expected != actual) {
    throw Error("Assertion failed: expected " + format(expected) + ", got " + format(actual));
  }
}

export void assertNotEqual(auto expected, auto actual) {
  if (expected == actual) {
    throw Error("Assertion failed: expected not equal to " + format(expected) + ", got " +
                format(actual));
  }
}

export void assertTrue(auto value) {
  if (!value) {
    throw Error("Assertion failed: expected true, got false for " + format(value));
  }
}

export void assertFalse(auto value) {
  if (value) {
    throw Error("Assertion failed: expected false, got true for " + format(value));
  }
}

export void assertNear(auto expected, auto actual, auto tol) {
  if (!(std::abs(expected - actual) <= tol)) {
    throw Error("Assertion failed: expected " + format(expected) + " ≈ " + format(actual) +
                " within " + format(tol));
  }
}

export void assertLess(auto a, auto b) {
  if (!(a < b)) {
    throw Error("Assertion failed: expected " + format(a) + " < " + format(b));
  }
}

export void assertLessEqual(auto a, auto b) {
  if (!(a <= b)) {
    throw Error("Assertion failed: expected " + format(a) + " <= " + format(b));
  }
}

export void assertGreater(auto a, auto b) {
  if (!(a > b)) {
    throw Error("Assertion failed: expected " + format(a) + " > " + format(b));
  }
}

export void assertGreaterEqual(auto a, auto b) {
  if (!(a >= b)) {
    throw Error("Assertion failed: expected " + format(a) + " >= " + format(b));
  }
}

export void assertContains(auto const& container, auto const& value) {
  if (std::find(std::begin(container), std::end(container), value) == std::end(container)) {
    throw Error("Assertion failed: container does not contain value " + format(value));
  }
}

export template <typename E = std::exception>
void assertThrows(auto func) {
  try {
    func();
    throw Error("Assertion failed: expected exception, none thrown");
  } catch (const E&) {
    return;
  } catch (...) {
    throw Error("Assertion failed: thrown exception type did not match expected");
  }
}

export template <typename E = std::exception>
void assertThrowsMessage(auto func, std::string_view msg_substr) {
  try {
    func();
    throw Error("Assertion failed: expected exception, none thrown");
  } catch (const E& e) {
    if (std::string_view(e.what()).find(msg_substr) == std::string_view::npos) {
      throw Error("Assertion failed: exception message does not contain " +
                  std::string(msg_substr));
    }
  } catch (...) {
    throw Error("Assertion failed: thrown exception type did not match expected");
  }
}

export template <typename E>
void assertThrowsExact(auto func) {
  try {
    func();
    throw Error("Assertion failed: expected exception, none thrown");
  } catch (...) {
    auto ep = std::current_exception();
    try {
      std::rethrow_exception(ep);
    } catch (const E& e) {
      if (typeid(e) != typeid(E)) {
        throw Error(std::string("Assertion failed: expected exact exception type ") +
                    typeid(E).name() + ", got " + typeid(e).name());
      }
      return;  // exact match
    } catch (...) {
      throw Error("Assertion failed: thrown exception type did not match expected");
    }
  }
}

export void assertDeath(auto func) {
#if defined(__unix__) || defined(__APPLE__)
  auto result = detail::runDeathTest(std::move(func));
  if (!result.died) {
    throw Error("Assertion failed: expected process death, child " + result.detail);
  }
#else
  (void)func;
  throw Error("Assertion failed: assertDeath is only supported on Linux/Unix");
#endif
}

// Expect statements that can be called
export void expectEqual(auto expected, auto actual) {
  if (expected != actual) {
    throw Abort("Expectation failed: expected " + format(expected) + ", got " + format(actual));
  }
}

export void expectNotEqual(auto expected, auto actual) {
  if (expected == actual) {
    throw Abort("Expectation failed: expected not equal to " + format(expected) + ", got " +
                format(actual));
  }
}

export void expectTrue(auto value) {
  if (!value) {
    throw Abort("Expectation failed: expected true, got false for " + format(value));
  }
}

export void expectFalse(auto value) {
  if (value) {
    throw Abort("Expectation failed: expected false, got true for " + format(value));
  }
}

export void expectNear(auto expected, auto actual, auto tol) {
  if (!(std::abs(expected - actual) <= tol)) {
    throw Abort("Expectation failed: expected " + format(expected) + " ≈ " + format(actual) +
                " within " + format(tol));
  }
}

export void expectLess(auto a, auto b) {
  if (!(a < b)) {
    throw Abort("Expectation failed: expected " + format(a) + " < " + format(b));
  }
}

export void expectLessEqual(auto a, auto b) {
  if (!(a <= b)) {
    throw Abort("Expectation failed: expected " + format(a) + " <= " + format(b));
  }
}

export void expectGreater(auto a, auto b) {
  if (!(a > b)) {
    throw Abort("Expectation failed: expected " + format(a) + " > " + format(b));
  }
}

export void expectGreaterEqual(auto a, auto b) {
  if (!(a >= b)) {
    throw Abort("Expectation failed: expected " + format(a) + " >= " + format(b));
  }
}

export void expectContains(auto const& container, auto const& value) {
  if (std::find(std::begin(container), std::end(container), value) == std::end(container)) {
    throw Abort("Expectation failed: container does not contain value " + format(value));
  }
}

export template <typename E = std::exception>
void expectThrows(auto func) {
  try {
    func();
    throw Abort("Expectation failed: expected exception, none thrown");
  } catch (const E&) {
    return;
  } catch (...) {
    throw Abort("Expectation failed: thrown exception type did not match expected");
  }
}

export template <typename E = std::exception>
void expectThrowsMessage(auto func, std::string_view msg_substr) {
  try {
    func();
    throw Abort("Expectation failed: expected exception, none thrown");
  } catch (const E& e) {
    if (std::string_view(e.what()).find(msg_substr) == std::string_view::npos) {
      throw Abort("Expectation failed: exception message does not contain " +
                  std::string(msg_substr));
    }
  } catch (...) {
    throw Abort("Expectation failed: thrown exception type did not match expected");
  }
}

export template <typename E>
void expectThrowsExact(auto func) {
  try {
    func();
    throw Abort("Expectation failed: expected exception, none thrown");
  } catch (...) {
    auto ep = std::current_exception();
    try {
      std::rethrow_exception(ep);
    } catch (const E& e) {
      if (typeid(e) != typeid(E)) {
        throw Abort(std::string("Expectation failed: expected exact exception type ") +
                    typeid(E).name() + ", got " + typeid(e).name());
      }
      return;  // exact match
    } catch (...) {
      throw Abort("Expectation failed: thrown exception type did not match expected");
    }
  }
}

export void expectDeath(auto func) {
#if defined(__unix__) || defined(__APPLE__)
  auto result = detail::runDeathTest(std::move(func));
  if (!result.died) {
    throw Abort("Expectation failed: expected process death, child " + result.detail);
  }
#else
  (void)func;
  throw Abort("Expectation failed: expectDeath is only supported on Linux/Unix");
#endif
}

// Finds and calls all tests in a suite
export template <typename T>
  requires(std::is_class_v<T>)
int test(int argc, char** argv, T suite = {}) {
  int status_code = 0;

  static constexpr auto result = getTests<T>();

  // Manually unpack the tuple
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

  std::string test_name;

  // Check mode
  std::vector<std::string> args(argv, argv + argc);
  for (const auto& arg : args) {
    if (arg == "--list") {
      std::cout << std::meta::identifier_of(^^T) << ".\n";
      template for (constexpr auto test : tests) {
        std::cout << "  " << std::meta::identifier_of(test) << '\n';
      }
      return 0;
    } else {
      test_name = arg;
    }
  }

  // The BeforeAll function is run once before any tests, and if it fails, the entire suite is
  // aborted
  if constexpr (has_before_all) {
    std::cout << "Running BeforeAll setup for suite " << std::meta::identifier_of(^^T) << '\n';
    try {
      suite.[:before_all_func:]();
    } catch (const std::exception& e) {
      std::cout << "BeforeAll setup failed with exception: " << e.what() << ", aborting test suite "
                << std::meta::identifier_of(^^T) << '\n';
      return 1;  // Abort the entire test suite if BeforeAll setup fails
    } catch (...) {
      std::cout << "BeforeAll setup failed with unknown error, aborting test suite "
                << std::meta::identifier_of(^^T) << '\n';
      return 1;  // Abort the entire test suite if BeforeAll setup fails
    }
  }

  template for (constexpr auto test : tests) {
    if (std::string(std::meta::identifier_of(test)) != test_name && !test_name.empty()) {
      continue;
    }

    bool disabled = false;
    static constexpr auto annotations = std::define_static_array(getAnnotations(test));

    bool parameterized = false;

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

    template for (constexpr auto a : annotations) {
      constexpr auto t = std::meta::template_of(std::meta::type_of(a));

      // Parameterized tests are run in a loop for each set of parameters so they must use a
      // separate calling mechanism
      if constexpr (t == ^^Parameterize) {
        parameterized = true;

        static constexpr auto template_args =
            std::define_static_array(std::meta::template_arguments_of(std::meta::type_of(a)));
        static constexpr auto num_sets = [:template_args[0]:];

        std::cout << "Running parameterized test " << std::meta::identifier_of(test) << " with "
                  << num_sets << " parameter sets\n";

        static constexpr auto param_members = std::define_static_array(
            getNonstaticDataMembers<
                decltype(std::meta::extract<
                             typename[:std::meta::substitute(^^Parameterize, template_args):]>(a)
                             .parameters[0]
                             .s)>());

        for (const auto param :
             std::meta::extract<typename[:std::meta::substitute(^^Parameterize, template_args):]>(a)
                 .parameters) {
          if constexpr (has_before_each) {
            try {
              suite.[:before_each_func:]();
            } catch (...) {
              std::cout << "BeforeEach function failed for test " << std::meta::identifier_of(test)
                        << ", skipping...\n";
              continue;  // Skip the test if setup fails
            }
          }

          template for (constexpr auto m : param_members) {
            std::cout << "\t- {" << std::to_string(param.s.[:m:]);
          }
          std::cout << "} --";

          try {
            auto start = std::chrono::system_clock::now();
            with_indices<param.getSizeof()>(
                [&](auto... Is) { return suite.[:test:](param.s.[:param_members[Is]:]...); });
            auto end = std::chrono::system_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            std::cout << " passed in " << duration.count() / 1'000'000.0 << " ms\n";
          } catch (const Error& e) {
            std::cout << "Test " << std::meta::identifier_of(test)
                      << " failed with error: " << e.message() << '\n';
            status_code = 1;
          } catch (const Abort& e) {
            std::cout << "Test " << std::meta::identifier_of(test)
                      << " aborted with message: " << e.message() << '\n';
          } catch (const std::exception& e) {
            std::cout << "Test " << std::meta::identifier_of(test)
                      << " failed with (uncaught) exception message: " << e.what() << '\n';
            status_code = 1;
          } catch (...) {
            std::cout << "Test " << std::meta::identifier_of(test)
                      << " failed with unknown error\n";
            status_code = 1;
          }

          if constexpr (has_after_each) {
            try {
              suite.[:after_each_func:]();
            } catch (...) {
              std::cout << "AfterEach function failed for test " << std::meta::identifier_of(test)
                        << '\n';
            }
          }
        }
      }
    }

    if (parameterized) {
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
      status_code = 1;
    } catch (const Abort& e) {
      std::cout << "Test " << std::meta::identifier_of(test)
                << " aborted with message: " << e.message() << '\n';
    } catch (const std::exception& e) {
      std::cout << "Test " << std::meta::identifier_of(test)
                << " failed with (uncaught) exception message: " << e.what() << '\n';
      status_code = 1;
    } catch (...) {
      std::cout << "Test " << std::meta::identifier_of(test) << " failed with unknown error\n";
      status_code = 1;
    }

    if constexpr (has_after_each) {
      try {
        suite.[:after_each_func:]();
      } catch (...) {
        std::cout << "AfterEach function failed for test " << std::meta::identifier_of(test)
                  << '\n';
      }
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

  return status_code;
}
}  // namespace cpputils::testing