export module cpputils.testing;

import std;

namespace cpputils::testing {
// Helper functions to extract specific vectors from types/reflections
template <typename T>
consteval auto getNonstaticDataMembers() {
  return std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current());
}

template <typename T>
consteval auto getMembers() {
  return std::meta::members_of(^^T, std::meta::access_context::current());
}

consteval auto getAnnotations(std::meta::info member) { return std::meta::annotations_of(member); }

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

// Different annotations recognized by the library
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
    throw Error("Assertion failed: expected " + std::to_string(expected) + ", got " +
                std::to_string(actual));
  }
}

// Finds and calls all tests in a suite
export template <typename T>
  requires(std::is_class_v<T>)
void test(T suite = {}) {
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

  // The BeforeAll function is run once before any tests, and if it fails, the entire suite is
  // aborted
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
          } catch (const std::exception& e) {
            std::cout << "Test " << std::meta::identifier_of(test)
                      << " failed with (uncaught) exception message: " << e.what() << '\n';
          } catch (...) {
            std::cout << "Test " << std::meta::identifier_of(test)
                      << " failed with unknown error\n";
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
}  // namespace cpputils::testing