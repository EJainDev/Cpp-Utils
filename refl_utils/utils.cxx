export module cpputils.refl_utils;

import std;

namespace cpputils::refl_utils {
export template <typename T>
consteval auto getNonstaticDataMembers() {
  return std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current());
}

export template <typename T>
consteval auto getMembers() {
  return std::meta::members_of(^^T, std::meta::access_context::current());
}

export consteval auto getAnnotations(std::meta::info member) {
  return std::meta::annotations_of(member);
}

export template <typename E>
  requires std::is_enum_v<E>
constexpr std::string enum_to_string(E value) {
  template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^E))) {
    if (value == [:e:]) {
      return std::string(std::meta::identifier_of(e));
    }
  }

  return "<invalid enum value>";
}

export template <typename T>
std::string format(const T& value) {
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
}  // namespace cpputils::refl_utils