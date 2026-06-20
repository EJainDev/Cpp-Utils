export module annotest;

export import :tuple;
export import :contracts;
export import :exceptions;
export import :asserts;
export import :expects;
export import :death_test;
export import :posix;
export import :utils;

import std;

namespace annotest {
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
  bool has_duplicate_each = false;
  bool has_duplicate_all = false;

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
          if (before_each_func.has_value()) {
            has_duplicate_each = true;
          }
          before_each_func = m;
        } else if constexpr (t == ^^AfterEach) {
          if (after_each_func.has_value()) {
            has_duplicate_each = true;
          }
          after_each_func = m;
        } else if constexpr (t == ^^BeforeAll) {
          if (before_all_func.has_value()) {
            has_duplicate_all = true;
          }
          before_all_func = m;
        } else if constexpr (t == ^^AfterAll) {
          if (after_all_func.has_value()) {
            has_duplicate_all = true;
          }
          after_all_func = m;
        }
      }
    }
  }

  return std::tuple(before_all_func, before_each_func, std::define_static_array(tests),
                    after_each_func, after_all_func, has_duplicate_each, has_duplicate_all);
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

bool runBeforeEach(const auto& func) {
  try {
    func();
  } catch (const std::exception& e) {
    std::cout << "BeforeEach function failed with exception: " << e.what() << '\n';
    return false;
  } catch (...) {
    std::cout << "BeforeEach function failed with unknown error\n";
    return false;
  }
  return true;
}

void runAfterEach(const auto& func) {
  try {
    func();
  } catch (const std::exception& e) {
    std::cout << "AfterEach function failed with exception: " << e.what() << '\n';
  } catch (...) {
    std::cout << "AfterEach function failed with unknown error\n";
  }
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
  static constexpr auto has_duplicate_each = std::get<5>(result);
  static constexpr auto has_duplicate_all = std::get<6>(result);
  static constexpr auto size = tests.size();

  std::string test_name;

  // Check mode
  std::vector<std::string> args(argv + 1, argv + argc);
  int i = 0;
  while (i < static_cast<int>(args.size())) {
    const auto& arg = args[i];
    if (arg == "--list") {
      std::cout << std::meta::identifier_of(^^T) << ".\n";
      template for (constexpr auto test_info : tests) {
        constexpr auto current_test_name = test_info.name;
        std::cout << "  " << current_test_name << '\n';
      }
      return 0;
    } else if (arg == "--test-name" && i + 1 < static_cast<int>(args.size())) {
      std::string candidate = args[i + 1];
      std::string full_name = std::string(std::meta::identifier_of(^^T)) + ".";
      // Accept either the full qualified name "Suite.test_name" or just the test name "test_name"
      if (candidate.starts_with(full_name)) {
        test_name = candidate.substr(full_name.size());
      } else {
        constexpr auto starts_with_idx = std::string(std::meta::identifier_of(^^T)).size() + 1;
        if (candidate.size() > static_cast<std::size_t>(starts_with_idx)) {
          test_name = candidate.substr(starts_with_idx);
        }
      }
    }
    ++i;
  }

  // Warn about duplicate lifecycle annotations
  if (has_duplicate_each) {
    std::cout << "[AnnoTest Warning] Duplicate BeforeEach/AfterEach annotation detected in '"
              << std::meta::identifier_of(^^T) << "'. Only the last occurrence will be used.\n";
  }
  if (has_duplicate_all) {
    std::cout << "[AnnoTest Warning] Duplicate BeforeAll/AfterAll annotation detected in '"
              << std::meta::identifier_of(^^T) << "'. Only the last occurrence will be used.\n";
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
    contract_violation_occurred = false;
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
    bool os_message_printed = false;

    template for (constexpr auto a : annotations) {
      constexpr auto t = std::meta::template_of(std::meta::type_of(a));

      // OS checks (RequiresOS / DisallowOS)
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

      // Print OS skip message exactly once per test
      if ((osRequirementFailed || osDisallowed) && !os_message_printed) {
        os_message_printed = true;
        if (osRequirementFailed) {
          std::cout << "Skipping test " << current_test_name << " because the current OS "
                    << enum_to_string(os) << " does not match the required OS of "
                    << enum_to_string(requiredOS) << '\n';
        } else if (osDisallowed) {
          std::cout << "Skipping test " << current_test_name << " because the current OS "
                    << enum_to_string(os) << " is disallowed.\n";
        }
      }

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
            if (!runBeforeEach([&suite]() { suite.[:*before_each_func:](); })) {
              continue;
            }
          }

          template for (constexpr auto m : param_members) {
            std::cout << "\t- {" << std::to_string(param.s.[:m:]);
          }
          std::cout << "} -- ";

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
            runAfterEach([&suite]() { suite.[:*after_each_func:](); });
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
        if (!runBeforeEach([&suite]() { suite.[:*before_each_func:](); })) {
          continue;
        }
      }

      try {
        auto start = std::chrono::system_clock::now();
        suite.[:test:]();
        auto end = std::chrono::system_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        std::cout << "Passed in " << duration.count() / 1'000'000.0 << " ms\n";
      } catch (const Error& e) {
        std::cout << "Failed with error: " << e.message() << '\n';
        status_code = 1;
      } catch (const Abort& e) {
        std::cout << "Aborted with message: " << e.message() << '\n';
      } catch (const std::exception& e) {
        std::cout << "Failed with (uncaught) exception message: " << e.what() << '\n';
        status_code = 1;
      } catch (...) {
        std::cout << "Failed with unknown error\n";
        status_code = 1;
      }

      if constexpr (after_each_func) {
        runAfterEach([&suite]() { suite.[:*after_each_func:](); });
      }
    } else {
      if (!parameterized) {
        std::cout << "Warning: Test " << current_test_name
                  << " did not execute because it has required arguments that were not given via '"
                     "Parameterized' annotation.\n";
      }
    }

    if (!test_name.empty() && std::string(current_test_name) == test_name) {
      break;
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

}  // namespace annotest
