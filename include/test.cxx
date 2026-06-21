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

export template <int N, typename... TupleArgs>
  requires(N > 0)
struct Parameterize {
  using TupleType = Tuple<TupleArgs...>;
  TupleType parameters[N];
};

template <typename... Args, typename... Rest>
Parameterize(Tuple<Args...>, Rest...) -> Parameterize<1 + (int)sizeof...(Rest), Args...>;

export template <typename... Args>
struct ParameterizeTemplate {};

template <typename... Pairs>
struct ParameterizeMatrix {
  Tuple<Pairs...> sets;
};

export template <typename... Types>
struct Pair {
  template <typename Values>
  struct InitI {
    Values params;
  };

  template <typename... ValueTuples>
  static constexpr auto InitM(ValueTuples... values) {
    return Tuple{InitI<ValueTuples>{.params = values}...};
  }
};

export template <typename... PairTuples>
consteval auto dualParameterize(PairTuples... pair_tuples) {
  return ParameterizeMatrix{.sets = tuple_cat(pair_tuples...)};
}

export template <int N>
struct RequiresOS {
  OS os[N];
};

template <typename... Ts>
RequiresOS(Ts...) -> RequiresOS<sizeof...(Ts)>;

export template <int N>
struct DisallowOS {
  OS os[N];
};

template <typename... Ts>
DisallowOS(Ts...) -> DisallowOS<sizeof...(Ts)>;
struct InternalTest {
  std::meta::info test;
  std::meta::info raw_test;
  const char* name;
  bool disabled;
  bool parameterized;
  int num_parameterizations;
};

// Gets all required information from the testing class
template <typename T>
consteval auto getTests() {
  static constexpr auto members = std::define_static_array(
      std::meta::members_of(^^T, std::meta::access_context::unprivileged()));
  static constexpr auto size = members.size();

  std::optional<std::meta::info> before_all_func;
  std::optional<std::meta::info> before_each_func;
  std::optional<std::meta::info> after_each_func;
  std::optional<std::meta::info> after_all_func;
  std::vector<InternalTest> tests;

  template for (constexpr auto m : members) {
    bool test_found = false;
    bool lifecycle_found = false;

    if constexpr (std::meta::has_identifier(m) &&
                  (std::meta::is_function(m) || std::meta::is_template(m))) {
      static constexpr auto member =
          (std::meta::is_function(m)
               ? m
               : std::meta::substitute(m,
                                       std::define_static_array(std::vector<std::meta::info>{})));
      static constexpr auto annotations =
          std::define_static_array(std::meta::annotations_of(member));
      InternalTest current_test;

      template for (constexpr auto a : annotations) {
        constexpr auto t = std::meta::template_of(std::meta::type_of(a));

        if constexpr (t == ^^Test) {
          if (lifecycle_found) {
            throw std::logic_error(
                std::string("Cannot have a Test annotation on a BeforeEach method. Method name: ") +
                std::define_static_string(std::meta::identifier_of(m)));
          }

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
          test_found = true;

          current_test = InternalTest{.test = member,
                                      .raw_test = m,
                                      .name = final_test_name,
                                      .disabled = test_info.disabled,
                                      .parameterized = false,
                                      .num_parameterizations = 0};
        } else if constexpr (t == ^^BeforeEach) {
          if (before_each_func) {
            throw std::logic_error(
                std::string("Duplicate BeforeEach annotation detected. First one at ") +
                std::define_static_string(std::meta::identifier_of(*before_each_func)) +
                " and second one at " + std::define_static_string(std::meta::identifier_of(m)));
          } else if (test_found) {
            throw std::logic_error(
                std::string("BeforeEach cannot be declared on a Test method. Method name: ") +
                std::define_static_string(std::meta::identifier_of(m)));
          }
          before_each_func = m;
        } else if constexpr (t == ^^AfterEach) {
          if (after_each_func) {
            throw std::logic_error(
                std::string("Duplicate AfterEach annotation detected. First one at ") +
                std::define_static_string(std::meta::identifier_of(*after_each_func)) +
                " and second one at " + std::define_static_string(std::meta::identifier_of(m)));
          } else if (test_found) {
            throw std::logic_error(
                std::string("AfterEach cannot be declared on a Test method. Method name: ") +
                std::define_static_string(std::meta::identifier_of(m)));
          }
          after_each_func = m;
        } else if constexpr (t == ^^BeforeAll) {
          if (before_all_func) {
            throw std::logic_error(
                std::string("Duplicate BeforeAll annotation detected. First one at ") +
                std::define_static_string(std::meta::identifier_of(*before_all_func)) +
                " and second one at " + std::define_static_string(std::meta::identifier_of(m)));
          } else if (test_found) {
            throw std::logic_error(
                std::string("BeforeAll cannot be declared on a Test method. Method name: ") +
                std::define_static_string(std::meta::identifier_of(m)));
          }
          before_all_func = m;
        } else if constexpr (t == ^^AfterAll) {
          if (after_all_func) {
            throw std::logic_error(
                std::string("Duplicate AfterAll annotation detected. First one at ") +
                std::define_static_string(std::meta::identifier_of(*after_all_func)) +
                " and second one at " + std::define_static_string(std::meta::identifier_of(m)));
          } else if (test_found) {
            throw std::logic_error(
                std::string("AfterAll cannot be declared on a Test method. Method name: ") +
                std::define_static_string(std::meta::identifier_of(m)));
          }
          after_all_func = m;
        } else if constexpr (t == ^^Parameterize) {
          current_test.parameterized = true;
          static constexpr auto template_args =
              std::define_static_array(std::meta::template_arguments_of(std::meta::type_of(a)));
          current_test.num_parameterizations = [:template_args[0]:];
        } else if constexpr (t == ^^ParameterizeTemplate) {
          current_test.parameterized = true;

          constexpr auto parameterize_args =
              std::define_static_array(std::meta::template_arguments_of(std::meta::type_of(a)));
          constexpr auto total_args = parameterize_args.size();
          constexpr auto template_parameters =
              std::define_static_array(std::meta::template_arguments_of(member));
          constexpr auto args_per_batch = template_parameters.size();
          current_test.num_parameterizations = total_args / args_per_batch;
        } else if constexpr (t == ^^ParameterizeMatrix) {
          constexpr auto parameterize_matrix = std::meta::extract<
              typename[:std::meta::substitute(
                            ^^ParameterizeMatrix,
                            std::define_static_array(
                                std::meta::template_arguments_of(std::meta::type_of(a)))):]>(a);
          current_test.parameterized = true;

          current_test.num_parameterizations = parameterize_matrix.sets.getSizeof();
        }
      }

      if (test_found) {
        tests.emplace_back(current_test);
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

bool runBeforeEach(const auto& func) {
  try {
    func();
  } catch (const std::exception& e) {
    std::println("BeforeEach function failed with exception: {}", e.what());
    return false;
  } catch (...) {
    std::println("BeforeEach function failed with unknown error");
    return false;
  }
  return true;
}

void runAfterEach(const auto& func) {
  try {
    func();
  } catch (const std::exception& e) {
    std::println("AfterEach function failed with exception: {}", e.what());
  } catch (...) {
    std::println("AfterEach function failed with unknown error");
  }
}

consteval auto createBatch(const auto& v, const auto start, const auto args_per_batch) {
  std::vector<std::meta::info> batch;
  for (std::size_t j = 0; j < args_per_batch; ++j) {
    batch.push_back(v[start + j]);
  }
  return std::define_static_array(batch);
}

consteval auto createArgBatchesIterable(const auto args_per_batch, const auto num_batches) {
  std::vector<int> iterable;
  for (std::size_t i = 0; i < num_batches; ++i) {
    iterable.push_back(i * args_per_batch);
  }
  return std::define_static_array(iterable);
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

  int verbosity = 1;

  // Check mode
  std::vector<std::string> args(argv + 1, argv + argc);
  int i = 0;
  while (i < static_cast<int>(args.size())) {
    const auto& arg = args[i];
    if (arg == "--list") {
      std::println("{}.", std::meta::identifier_of(^^T));
      template for (constexpr auto test_info : tests) {
        constexpr auto current_test_name = test_info.name;
        constexpr auto parameterized = test_info.parameterized;
        constexpr auto num_parameterizations = test_info.num_parameterizations;
        if constexpr (!parameterized) {
          std::println("  {}", current_test_name);
        } else {
          for (int i = 0; i < num_parameterizations; ++i) {
            std::println("  {}.{}", current_test_name, i + 1);
          }
        }
      }
      return 0;
    } else if (arg == "--test-name" && i + 1 < static_cast<int>(args.size())) {
      if (test_name != "") {
        std::println("Warning: Multiple test name arguments provided. Using the last one: {}",
                     args[i + 1]);
      }
      test_name = args[i + 1];
    } else if (arg.starts_with("-v")) {
      verbosity = arg.size() - 1;  // Number of 'v's indicates verbosity level
    }
    ++i;
  }
  std::println("Running test suite {} with verbosity level {}", std::meta::identifier_of(^^T),
               verbosity);

  // The BeforeAll function is run once before any tests, and if it fails, the entire suite is
  // aborted
  if constexpr (before_all_func) {
    constexpr auto func = before_all_func.value();
    std::print("Running BeforeAll setup for suite {}\n", std::meta::identifier_of(^^T));
    try {
      suite.[:func:]();
    } catch (const std::exception& e) {
      std::println("BeforeAll setup failed with exception: {}, aborting test suite {}", e.what(),
                   std::meta::identifier_of(^^T));
      return 1;  // Abort the entire test suite if BeforeAll setup fails
    } catch (...) {
      std::println("BeforeAll setup failed with unknown error, aborting test suite {}",
                   std::meta::identifier_of(^^T));
      return 1;  // Abort the entire test suite if BeforeAll setup fails
    }
  }

  template for (constexpr auto test_info : tests) {
    contract_violation_occurred = false;  // Reset contract flag before each test

    constexpr auto test = test_info.test;
    constexpr auto raw_test = test_info.raw_test;
    constexpr auto current_test_name = test_info.name;
    constexpr auto disabled = test_info.disabled;
    constexpr auto parameterized = test_info.parameterized;

    // Get the test to run
    int parameterize_target_idx = -1;
    if constexpr (!parameterized) {
      if (!test_name.empty() && std::string(current_test_name) != test_name) {
        continue;
      }
    } else {
      if (!test_name.empty() &&
          std::string(current_test_name) != test_name.substr(0, test_name.find_last_of('.'))) {
        continue;
      } else if (!test_name.empty()) {
        parameterize_target_idx = std::stoi(test_name.substr(test_name.find_last_of('.') + 1)) - 1;
      }
    }

    // Skip if disabled
    if constexpr (disabled) {
      std::println("Skipping disabled test: {}", current_test_name);
      continue;
    }

    bool osRequirementFailed = false;
    bool osDisallowed = false;
    OS requiredOS = OS::Unknown;
    static constexpr auto annotations = std::define_static_array(std::meta::annotations_of(test));

    bool os_message_printed = false;

    // Check for OS level requirements
    template for (constexpr auto a : annotations) {
      bool stop = false;

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
            std::println("Current OS {} does not match required OS {}, skipping test {}",
                         enum_to_string(os), enum_to_string(osDetail[i]), current_test_name);
            stop = true;
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
            std::println("Current OS {} is disallowed for test {}, skipping", enum_to_string(os),
                         current_test_name);
            stop = true;
            break;
          }
        }
      }

      if (stop) {
        break;
      }
    }

    // Check for parameterization
    template for (constexpr auto a : annotations) {
      constexpr auto t = std::meta::template_of(std::meta::type_of(a));

      // Regular, value parameterization
      if constexpr (t == ^^Parameterize) {
        static constexpr auto template_args =
            std::define_static_array(std::meta::template_arguments_of(std::meta::type_of(a)));
        static constexpr auto num_sets = [:template_args[0]:];

        std::println("Running parameterized test {} with {} parameter sets", current_test_name,
                     num_sets);

        static constexpr auto param_members = std::define_static_array(
            getNonstaticDataMembers<
                decltype(std::meta::extract<
                             typename[:std::meta::substitute(^^Parameterize, template_args):]>(a)
                             .parameters[0]
                             .s)>());

        int parameterize_idx = 0;
        std::chrono::nanoseconds total_duration(0);
        int passed = 0;
        for (const auto param :
             std::meta::extract<typename[:std::meta::substitute(^^Parameterize, template_args):]>(a)
                 .parameters) {
          if (parameterize_target_idx != -1 && parameterize_idx != parameterize_target_idx) {
            ++parameterize_idx;
            continue;
          }
          ++parameterize_idx;

          contract_violation_occurred = false;

          if constexpr (before_each_func) {
            if (verbosity > 1) {
              std::println("\tRunning setup function {}",
                           std::meta::identifier_of(*before_each_func));
            }
            if (!runBeforeEach([&suite]() { suite.[:*before_each_func:](); })) {
              continue;
            }
          }

          std::print("\t (");
          int i = 0;
          template for (constexpr auto m : param_members) {
            std::print("{}", param.s.[:m:]);
            ++i;
            if (i < param_members.size() - 1) {
              std::print(", ");
            }
          }
          std::print(") -- ");

          try {
            auto start = std::chrono::steady_clock::now();
            with_indices<param.getSizeof()>(
                [&](auto... Is) { return suite.[:test:](param.s.[:param_members[Is]:]...); });
            auto end = std::chrono::steady_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            total_duration += duration;

            if (contract_violation_occurred) {
              throw Error("Contract violation occurred during test execution");
            }

            std::println("passed in {:.3f} ms", duration.count() / 1'000'000.0);
            ++passed;
          } catch (const Error& e) {
            std::println("failed with error {}", e.message());
            status_code = 1;
          } catch (const std::exception& e) {
            std::println("failed with (uncaught) exception message: {}", e.what());
            status_code = 1;
          } catch (...) {
            std::println("failed with unknown error\n");
            status_code = 1;
          }

          if constexpr (after_each_func) {
            std::println("\tRunning teardown function {}",
                         std::meta::identifier_of(*after_each_func));
            runAfterEach([&suite]() { suite.[:*after_each_func:](); });
          }

          if (parameterize_target_idx != -1) {
            break;
          }
        }

        std::println("Finished in {:.3f} ms with {}/{} tests passed",
                     total_duration.count() / 1'000'000.0, passed, num_sets);
      } else if constexpr (t == ^^ParameterizeTemplate) {
        constexpr auto parameterize_args =
            std::define_static_array(std::meta::template_arguments_of(std::meta::type_of(a)));
        constexpr auto total_args = parameterize_args.size();
        constexpr auto template_parameters =
            std::define_static_array(std::meta::template_arguments_of(test));
        constexpr auto args_per_batch = template_parameters.size();
        constexpr auto num_sets = total_args / args_per_batch;
        static constexpr auto arg_batches_iterable =
            createArgBatchesIterable(args_per_batch, num_sets);

        std::println("Running parameterized template test \"{}\" with {} parameter sets:",
                     current_test_name, num_sets);

        int parameterize_idx = 0;
        std::chrono::nanoseconds total_duration(0);
        int passed = 0;

        template for (constexpr auto batch_iter : arg_batches_iterable) {
          contract_violation_occurred = false;

          if (parameterize_target_idx != -1 && parameterize_idx != parameterize_target_idx) {
            ++parameterize_idx;
            continue;
          }
          ++parameterize_idx;

          static constexpr auto batch = createBatch(parameterize_args, batch_iter, args_per_batch);

          if constexpr (before_each_func) {
            if (verbosity > 1) {
              std::println("\tRunning setup function {}",
                           std::meta::identifier_of(*before_each_func));
            }
            if (!runBeforeEach([&suite]() { suite.[:*before_each_func:](); })) {
              continue;
            }
          }

          std::print("\t <");
          int idx = 0;
          static constexpr auto max_idx = batch.size() - 1;
          template for (constexpr auto param : batch) {
            std::print("{}", std::meta::display_string_of(param));
            if (idx < max_idx) {
              std::print(", ");
            }
            ++idx;
          }
          std::print("> -- ");

          try {
            auto start = std::chrono::system_clock::now();
            constexpr auto expanded_test = std::meta::substitute(raw_test, batch);
            suite.[:expanded_test:]();
            auto end = std::chrono::system_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            total_duration += duration;

            if (contract_violation_occurred) {
              throw Error("Contract violation occurred during test execution");
            }

            std::println("passed in {:.3f} ms", duration.count() / 1'000'000.0);
            ++passed;
          } catch (const Error& e) {
            std::println("failed with error: {}", e.message());
            status_code = 1;
          } catch (const std::exception& e) {
            std::println("failed with (uncaught) exception message: {}", e.what());
            status_code = 1;
          } catch (...) {
            std::println("failed with unknown error");
            status_code = 1;
          }

          if constexpr (after_each_func) {
            if (verbosity > 1) {
              std::println("\tRunning teardown function {}",
                           std::meta::identifier_of(*after_each_func));
            }
            runAfterEach([&suite]() { suite.[:*after_each_func:](); });
          }

          if (parameterize_target_idx != -1) {
            break;
          }
        }

        std::println("Finished in {:.3f} ms with {}/{} tests passed",
                     total_duration.count() / 1'000'000.0, passed, num_sets);
      } else if constexpr (t == ^^ParameterizeMatrix) {
        constexpr auto parameterize_matrix =
            std::meta::extract<typename[:std::meta::substitute(^^ParameterizeMatrix,
                                                               std::define_static_array(
                                                                   std::meta::template_arguments_of(
                                                                       std::meta::type_of(a)))):]>(
                a);
        constexpr auto num_sets = parameterize_matrix.sets.getSizeof();

        std::println("Running parameterized template and parameter test \"{}\" with {} sets:",
                     current_test_name, num_sets);

        static constexpr auto member_pairs = std::define_static_array(
            getNonstaticDataMembers<decltype(parameterize_matrix.sets.s)>());

        std::chrono::nanoseconds total_duration(0);
        int passed = 0;
        int parameterize_idx = 0;

        template for (constexpr auto pair : member_pairs) {
          if (parameterize_target_idx != -1 && parameterize_idx != parameterize_target_idx) {
            ++parameterize_idx;
            continue;
          }
          ++parameterize_idx;

          if constexpr (before_each_func) {
            if (verbosity > 1) {
              std::println("\tRunning setup function {}",
                           std::meta::identifier_of(*before_each_func));
            }
            if (!runBeforeEach([&suite]() { suite.[:*before_each_func:](); })) {
              continue;
            }
          }

          try {
            constexpr auto parent_pair = std::meta::parent_of(std::meta::type_of(pair));
            constexpr auto expanded_test = std::meta::substitute(
                raw_test, std::define_static_array(std::meta::template_arguments_of(parent_pair)));
            static constexpr auto template_args =
                std::define_static_array(std::meta::template_arguments_of(parent_pair));

            constexpr auto physical_pair = parameterize_matrix.sets.s.[:pair:];
            static constexpr auto param_members = std::define_static_array(
                getNonstaticDataMembers<decltype(physical_pair.params.s)>());

            std::print("\t <");
            int idx = 0;
            static constexpr auto max_idx = template_args.size() - 1;
            template for (constexpr auto param : template_args) {
              std::print("{}", std::meta::display_string_of(param));
              if (idx < max_idx) {
                std::print(", ");
              }
              ++idx;
            }
            std::print(">");
            std::print("(");
            int i = 0;
            template for (constexpr auto m : param_members) {
              std::print("{}", physical_pair.params.s.[:m:]);
              if (i < param_members.size() - 1) {
                std::print(", ");
              }
              ++i;
            }
            std::print(") -- ");

            auto start = std::chrono::system_clock::now();
            with_indices<physical_pair.params.getSizeof()>([&](auto... Is) {
              return suite.[:expanded_test:](physical_pair.params.s.[:param_members[Is]:]...);
            });

            auto end = std::chrono::system_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            total_duration += duration;

            if (contract_violation_occurred) {
              throw Error("Contract violation occurred during test execution");
            }

            std::println("passed in {:.3f} ms", duration.count() / 1'000'000.0);
            ++passed;
          } catch (const Error& e) {
            std::println("failed with error: {}", e.message());
            status_code = 1;
          } catch (const std::exception& e) {
            std::println("failed with (uncaught) exception message: {}", e.what());
            status_code = 1;
          } catch (...) {
            std::println("failed with unknown error");
            status_code = 1;
          }

          if constexpr (after_each_func) {
            if (verbosity > 1) {
              std::println("\tRunning teardown function {}",
                           std::meta::identifier_of(*after_each_func));
            }
            runAfterEach([&suite]() { suite.[:*after_each_func:](); });
          }

          if (parameterize_target_idx != -1) {
            break;
          }
        }

        std::println("Finished in {:.3f} ms with {}/{} tests passed",
                     total_duration.count() / 1'000'000.0, passed, num_sets);
      }
    }

    if constexpr (parameterized) {
      continue;
    }

    if constexpr (notHasRequiredParameter<test>()) {
      std::println("Running test: {}", current_test_name);
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

        if (contract_violation_occurred) {
          throw Error("Contract violation occurred during test execution");
        }

        std::println("Passed in {:.3f} ms", duration.count() / 1'000'000.0);
      } catch (const Error& e) {
        std::println("Failed with error: {}", e.message());
        status_code = 1;
      } catch (const std::exception& e) {
        std::println("Failed with (uncaught) exception message: {}", e.what());
        status_code = 1;
      } catch (...) {
        std::println("Failed with unknown error");
        status_code = 1;
      }

      if constexpr (after_each_func) {
        runAfterEach([&suite]() { suite.[:*after_each_func:](); });
      }
    } else {
      if (!parameterized) {
        std::println(
            "Warning: Test {} did not execute because it has required arguments that were not "
            "given via 'Parameterized' annotation.",
            current_test_name);
      }
    }

    if (!test_name.empty() && std::string(current_test_name) == test_name) {
      break;
    }
  }

  if constexpr (after_all_func) {
    constexpr auto func = after_all_func.value();
    std::println("Running AfterAll teardown for suite {}", std::meta::identifier_of(^^T));
    try {
      suite.[:func:]();
    } catch (const std::exception& e) {
      std::println("AfterAll teardown failed with exception: {}", e.what());
    } catch (...) {
      std::println("AfterAll teardown failed with unknown error");
    }
  }

  return status_code;
}

}  // namespace annotest
