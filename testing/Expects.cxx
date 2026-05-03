module;

#if defined(__unix__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

export module cpputils.testing:expects;

import :exceptions;

import cpputils.refl_utils;

import std;

using namespace cpputils::refl_utils;

namespace cpputils::testing {
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
}  // namespace cpputils::testing