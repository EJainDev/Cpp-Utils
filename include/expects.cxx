export module annotest:expects;

import :exceptions;
import :utils;
import :contracts;

import std;

namespace annotest {
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

export template <typename T, typename U, typename K = std::common_type_t<T, U>>
void expectNear(T expected, U actual, K tol = static_cast<K>(1e-3)) {
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

export void expectNull(auto ptr) {
  if (ptr != nullptr) {
    throw Abort("Expectation failed: expected nullptr, got " + format(ptr));
  }
}

export template <typename Func>
void expectContractViolation(Func f) {
  if constexpr (!std::is_void_v<std::invoke_result_t<Func>>) {
    auto result = f();
    if (!annotest::contract_violation_occurred) {
      throw Abort("Expectation failed: expected contract violation, but it was not detected");
    }
    return;
  }
  f();
  if (!annotest::contract_violation_occurred) {
    throw Abort("Expectation failed: expected contract violation, but it was not detected");
  }
}

export template <typename Func>
auto expectNoContractViolation(Func f) {
  if constexpr (!std::is_void_v<std::invoke_result_t<Func>>) {
    auto result = f();
    if (annotest::contract_violation_occurred) {
      throw Abort("Expectation failed: expected no contract violation, but one was detected");
    }
    return result;
  }
  f();
  if (annotest::contract_violation_occurred) {
    throw Abort("Expectation failed: expected no contract violation, but one was detected");
  }
}
}  // namespace annotest