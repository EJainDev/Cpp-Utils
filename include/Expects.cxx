export module annotest:expects;

import :exceptions;

import std;

namespace annotest {
template <typename T>
consteval auto getNonstaticDataMembers() {
  return std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current());
}

template <typename E>
  requires std::is_enum_v<E>
constexpr std::string enum_to_string(E value) {
  template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^E))) {
    if (value == [:e:]) {
      return std::string(std::meta::identifier_of(e));
    }
  }

  return "<invalid enum value>";
}

template <typename T>
constexpr std::string format(const T& value) {
  if constexpr (std::is_class_v<T>) {
    std::string str;
    if constexpr (std::meta::has_identifier(^^T)) {
      str = std::string(std::meta::identifier_of(^^T)) + " { ";
    } else {
      str = "object { ";
    }
    static constexpr auto data_members = std::define_static_array(getNonstaticDataMembers<T>());
    template for (constexpr auto m : data_members) {
      str += std::string(std::meta::identifier_of(m)) + ": " + format(value.[:m:]) + ", ";
    }
    str += "}";
    return str;
  } else if constexpr (std::is_enum_v<T>) {
    return enum_to_string(value);
  } else if constexpr (std::is_same_v<T, std::source_location>) {
    return std::format("file {} {}:{} in function {}", value.file_name(), value.line(),
                       value.column(), value.function_name());
  } else {
    if constexpr (std::is_pointer_v<T>) {
      if (value == nullptr) {
        return "nullptr";
      } else {
        return std::format("{}", reinterpret_cast<std::size_t>(value));
      }
    } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
      return "nullptr";
    } else {
      return std::format("{}", value);
    }
  }
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
}  // namespace annotest