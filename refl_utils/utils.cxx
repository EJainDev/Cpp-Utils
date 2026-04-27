export module cpputils.refl_utils;

import std;

export namespace cpputils {
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
}  // namespace cpputils