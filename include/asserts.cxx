export module annotest:asserts;

import :exceptions;
import :contracts;
import :utils;

import std;

namespace annotest {
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

export void assertNear(auto expected, auto actual, double tol = 0.001) {
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

export template <typename E>
void assertThrowsExact(auto func) {
  try {
    func();
    throw Error("Assertion failed: expected exception, none thrown");
  } catch (const E& e) {
    if (typeid(e) != typeid(E)) {
      throw Error(std::string("Assertion failed: expected exact exception type ") +
                  typeid(E).name() + ", got " + typeid(e).name());
    }
  }
}

export void assertNull(auto ptr) {
  if (ptr != nullptr) {
    throw Error("Assertion failed: expected nullptr, got " + format(ptr));
  }
}

export template <typename Func>
auto assertContractViolation(Func f) {
  if constexpr (!std::is_same_v<decltype(f()), void>) {
    auto result = f();
    if (!annotest::contract_violation_occurred) {
      throw Error("Assertion failed: expected contract violation, but it was not detected");
    }
    return result;
  }
  f();
  if (!annotest::contract_violation_occurred) {
    throw Error("Assertion failed: expected contract violation, but it was not detected");
  }
}

export template <typename Func>
auto assertNoContractViolation(Func f) {
  if constexpr (!std::is_same_v<decltype(f()), void>) {
    auto result = f();
    if (annotest::contract_violation_occurred) {
      throw Error("Assertion failed: expected no contract violation, but one was detected");
    }
    return result;
  }

  f();
  if (annotest::contract_violation_occurred) {
    throw Error("Assertion failed: expected no contract violation, but one was detected");
  }
}
}  // namespace annotest