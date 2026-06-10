export module cpputils.testing;

export import :tuple;
export import :exceptions;
export import :asserts;
export import :expects;
export import :death_test;
export import :posix;

import cpputils.refl;

import std;

using namespace cpputils::refl;

namespace cpputils::testing {
// Helper function to call a test with all the parameters in a tuple
template <std::size_t N, typename F>
constexpr auto with_indices(const F f) -> decltype(auto) {
  return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> decltype(auto) {
    return f(std::integral_constant<std::size_t, Is>{}...);
  }(std::make_index_sequence<N>{});
}

export enum class OS {
  Windows,
  Mac,
  Linux,
  Unknown,
};

export template <typename T = int>
struct BeforeEach {};

template <std::size_t N>
struct FixedString {
  char value[N];

  constexpr FixedString(const char (&str)[N]) {
    for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
  }

  static constexpr bool isEmpty() { return N == 1; }
};

export template <FixedString Name = "">
struct Test {
  bool disabled = false;
  static constexpr auto name = Name.value;

  constexpr auto isEmpty() const { return Name.isEmpty(); }
};

export template <typename T = int>
struct AfterEach {};

export template <typename T = int>
struct BeforeAll {};

export template <typename T = int>
struct AfterAll {};

struct InternalTest {
  std::meta::info test;
  const char* name;
  bool disabled;
};

export template <int N, typename... TupleArgs>
  requires(N > 0)
struct Parameterize {
  using TupleType = Tuple<TupleArgs...>;
  TupleType parameters[N];
};

export template <int N, typename... Args>
  requires(N > 0 && (std::same_as<Args, std::meta::info> && ...))
struct ParameterizeTemplate {
  using TupleType = Tuple<Args...>;
  TupleType parameters[N];
};

export template <int N>
struct RequiresOS {
  OS os[N];
};

export template <int N>
struct DisallowOS {
  OS os[N];
};

template <typename... Ts>
RequiresOS(Ts...) -> RequiresOS<sizeof...(Ts)>;

template <typename... Ts>
DisallowOS(Ts...) -> DisallowOS<sizeof...(Ts)>;

template <typename... Args, typename... Rest>
Parameterize(Tuple<Args...>, Rest...) -> Parameterize<1 + (int)sizeof...(Rest), Args...>;

template <typename... Args, typename... Rest>
ParameterizeTemplate(Tuple<Args...>, Rest...)
    -> ParameterizeTemplate<1 + (int)sizeof...(Rest), Args...>;

// Gets all required information from the testing class
template <typename T>
consteval auto getTests() {
  static constexpr auto members = std::define_static_array(getMembers<T>());
  static constexpr auto size = members.size();

  std::optional<std::meta::info> before_all_func;
  std::optional<std::meta::info> before_each_func;
  std::vector<InternalTest> tests;
  std::optional<std::meta::info> after_each_func;
  std::optional<std::meta::info> after_all_func;

  template for (constexpr auto m : members) {
    if constexpr (std::meta::has_identifier(m)) {
      static constexpr auto annotations = std::define_static_array(getAnnotations(m));
      template for (constexpr auto a : annotations) {
        constexpr auto t = std::meta::template_of(std::meta::type_of(a));

        if constexpr (t == ^^Test) {
          static constexpr auto template_args =
              std::define_static_array(std::meta::template_arguments_of(std::meta::type_of(a)));

          auto test_info =
              std::meta::extract<typename[:std::meta::substitute(^^Test, template_args):]>(a);
          std::string str_test_name;
          if (!test_info.isEmpty()) {
            str_test_name = test_info.name;
          }
          const char* final_test_name =
              !test_info.isEmpty() ? std::define_static_string(str_test_name)
                                   : std::define_static_string(std::meta::identifier_of(m));
          tests.emplace_back(m, final_test_name, test_info.disabled);
        } else if constexpr (t == ^^BeforeEach) {
          before_each_func = m;
        } else if constexpr (t == ^^AfterEach) {
          after_each_func = m;
        } else if constexpr (t == ^^BeforeAll) {
          before_all_func = m;
        } else if constexpr (t == ^^AfterAll) {
          after_all_func = m;
        }
      }
    }
  }

  return std::tuple(before_all_func, before_each_func, std::define_static_array(tests),
                    after_each_func, after_all_func);
}

template <std::meta::info func>
consteval bool notHasRequiredParameter() {
  static constexpr auto args = std::define_static_array(std::meta::parameters_of(func));
  template for (constexpr auto arg : args) {
    if constexpr (!std::meta::has_default_argument(arg)) {
      return false;
    }
  }

  return true;
}

export template <typename T>
  requires std::is_class_v<T>
int test(int argc, char** argv, T suite = {}) {
#if defined(__linux__)
  static constexpr auto os = OS::Linux;
#elif defined(_WIN32)
  static constexpr auto os = OS::Windows;
#elif defined(__APPLE__)
  static constexpr auto os = OS::Mac;
#else
  static constexpr auto os = OS::Unknown;
#endif

  int status_code = 0;

  static constexpr auto result = getTests<T>();

  // Manually unpack the tuple
  static constexpr auto before_all_func = std::get<0>(result);
  static constexpr auto before_each_func = std::get<1>(result);
  static constexpr auto tests = std::get<2>(result);
  static constexpr auto after_each_func = std::get<3>(result);
  static constexpr auto after_all_func = std::get<4>(result);
  static constexpr auto size = tests.size();

  std::string test_name;

  // Check mode
  std::vector<std::string> args(argv + 1, argv + argc);
  for (const auto& arg : args) {
    if (arg == "--list") {
      std::cout << std::meta::identifier_of(^^T) << ".\n";
      template for (constexpr auto test_info : tests) {
        constexpr auto current_test_name = test_info.name;
        std::cout << "  " << current_test_name << '\n';
      }
      return 0;
    } else {
      test_name = arg;
    }
  }

  // The BeforeAll function is run once before any tests, and if it fails, the entire suite is
  // aborted
  if constexpr (before_all_func) {
    std::cout << "Running BeforeAll setup for suite " << std::meta::identifier_of(^^T) << '\n';
    try {
      suite.[:*before_all_func:]();
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

  template for (constexpr auto test_info : tests) {
    constexpr auto test = test_info.test;
    constexpr auto current_test_name = test_info.name;
    constexpr auto disabled = test_info.disabled;
    if (!test_name.empty() && std::string(current_test_name) != test_name) {
      continue;
    }

    if constexpr (disabled) {
      std::cout << "Skipping disabled test: " << current_test_name << '\n';
      continue;
    }

    bool osRequirementFailed = false;
    bool osDisallowed = false;
    OS requiredOS = OS::Unknown;
    static constexpr auto annotations = std::define_static_array(getAnnotations(test));

    bool parameterized = false;

    template for (constexpr auto a : annotations) {
      constexpr auto t = std::meta::template_of(std::meta::type_of(a));

      if constexpr (t == ^^RequiresOS) {
        static constexpr auto template_args =
            std::define_static_array(std::meta::template_arguments_of(std::meta::type_of(a)));

        auto osDetail =
            std::meta::extract<typename[:std::meta::substitute(^^RequiresOS, template_args):]>(a)
                .os;
        for (int i = 0; i < sizeof(osDetail) / sizeof(OS); ++i) {
          if (os != osDetail[i]) {
            osRequirementFailed = true;
            requiredOS = osDetail[i];
            break;
          }
        }
      } else if constexpr (t == ^^DisallowOS) {
        static constexpr auto template_args =
            std::define_static_array(std::meta::template_arguments_of(std::meta::type_of(a)));

        auto osDetail =
            std::meta::extract<typename[:std::meta::substitute(^^DisallowOS, template_args):]>(a)
                .os;
        for (int i = 0; i < sizeof(osDetail) / sizeof(OS); ++i) {
          if (os == osDetail[i]) {
            osDisallowed = true;
            break;
          }
        }
      }
    }

    if (osRequirementFailed) {
      std::cout << "Skipping test " << current_test_name << " because the current OS "
                << enum_to_string(os) << " does not match the required OS of "
                << enum_to_string(requiredOS) << '\n';
    } else if (osDisallowed) {
      std::cout << "Skipping test " << current_test_name << " because the current OS "
                << enum_to_string(os) << " is disallowed.\n";
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

        std::cout << "Running parameterized test " << current_test_name << " with " << num_sets
                  << " parameter sets\n";

        static constexpr auto param_members = std::define_static_array(
            getNonstaticDataMembers<
                decltype(std::meta::extract<
                             typename[:std::meta::substitute(^^Parameterize, template_args):]>(a)
                             .parameters[0]
                             .s)>());

        for (const auto param :
             std::meta::extract<typename[:std::meta::substitute(^^Parameterize, template_args):]>(a)
                 .parameters) {
          if constexpr (before_each_func) {
            try {
              suite.[:*before_each_func:]();
            } catch (...) {
              std::cout << " BeforeEach function failed\n";
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
            std::cout << " failed with error: " << e.message() << '\n';
            status_code = 1;
          } catch (const Abort& e) {
            std::cout << " aborted with message: " << e.message() << '\n';
          } catch (const std::exception& e) {
            std::cout << " failed with (uncaught) exception message: " << e.what() << '\n';
            status_code = 1;
          } catch (...) {
            std::cout << " failed with unknown error\n";
            status_code = 1;
          }

          if constexpr (after_each_func) {
            try {
              suite.[:*after_each_func:]();
            } catch (...) {
              std::cout << " AfterEach function failed\n";
            }
          }
        }
      }
    }

    if (parameterized) {
      continue;
    }

    if constexpr (notHasRequiredParameter<test>()) {
      std::cout << "Running test: " << current_test_name << '\n';
      if constexpr (before_each_func) {
        try {
          suite.[:*before_each_func:]();
        } catch (...) {
          std::cout << "BeforeEach function failed for test " << current_test_name
                    << ", skipping...\n";
          continue;  // Skip the test if setup fails
        }
      }

      try {
        auto start = std::chrono::system_clock::now();
        suite.[:test:]();
        auto end = std::chrono::system_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        std::cout << "Test " << current_test_name << " passed in " << duration.count() / 1'000'000.0
                  << " ms\n";
      } catch (const Error& e) {
        std::cout << "Test " << current_test_name << " failed with error: " << e.message() << '\n';
        status_code = 1;
      } catch (const Abort& e) {
        std::cout << "Test " << current_test_name << " aborted with message: " << e.message()
                  << '\n';
      } catch (const std::exception& e) {
        std::cout << "Test " << current_test_name
                  << " failed with (uncaught) exception message: " << e.what() << '\n';
        status_code = 1;
      } catch (...) {
        std::cout << "Test " << current_test_name << " failed with unknown error\n";
        status_code = 1;
      }

      if constexpr (after_each_func) {
        try {
          suite.[:*after_each_func:]();
        } catch (...) {
          std::cout << "AfterEach function failed for test " << current_test_name << '\n';
        }
      }
    } else {
      if (!parameterized) {
        std::cout << "Warning: Test " << current_test_name
                  << " did not execute because it has required arguments that were not given via "
                     "'Paremterized' annotation.\n";
      }
    }
  }

  if constexpr (after_all_func) {
    std::cout << "Running AfterAll teardown for suite " << std::meta::identifier_of(^^T) << '\n';
    try {
      suite.[:*after_all_func:]();
    } catch (const std::exception& e) {
      std::cout << "AfterAll teardown failed with exception: " << e.what() << '\n';
    } catch (...) {
      std::cout << "AfterAll teardown failed with unknown error\n";
    }
  }

  return status_code;
}
}  // namespace cpputils::testing