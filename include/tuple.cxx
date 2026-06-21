export module annotest:tuple;

import std;

import :utils;

namespace annotest {

template <std::size_t I>
struct member_name {
  // NOTE: This template only handles indices up to 9999. For I >= 10000 the generated names
  // will overflow the character array and produce undefined behavior. When using tuple() with
  // more than 9999 elements, consider splitting into multiple smaller tuples. This should never be
  // the case. Might want to consider why you're creating a test with 9999+ parameters...
  static constexpr auto internal_value() {
    if constexpr (I < 10) {
      return std::array<char, 3>{'m', static_cast<char>('0' + I), '\0'};
    } else if constexpr (I < 100) {
      return std::array<char, 4>{'m', static_cast<char>('0' + I / 10),
                                 static_cast<char>('0' + I % 10), '\0'};
    } else if constexpr (I < 1000) {
      return std::array<char, 5>{'m', static_cast<char>('0' + I / 100),
                                 static_cast<char>('0' + (I / 10) % 10),
                                 static_cast<char>('0' + I % 10), '\0'};
    } else {
      return std::array<char, 7>{'m',
                                 static_cast<char>('0' + I / 1000),
                                 static_cast<char>('0' + (I / 100) % 10),
                                 static_cast<char>('0' + (I / 10) % 10),
                                 static_cast<char>('0' + I % 10),
                                 '\0'};
    }
  }

  static constexpr auto value = member_name<I>::internal_value();
};

template <typename... Ts, std::size_t... Is>
consteval auto make_specs(std::index_sequence<Is...>) {
  // Explicitly typing the vector prevents CTAD failure on empty packs
  return std::vector<std::meta::info>{
      std::meta::data_member_spec(^^Ts, {
                                            .name = member_name<Is>::value.data()})...};
}

template <typename... Ts>
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

template <typename... Ts>
Tuple(Ts...) -> Tuple<Ts...>;

export consteval auto tuple(auto... args) { return Tuple<decltype(args)...>{.s = {args...}}; }
}  // namespace annotest