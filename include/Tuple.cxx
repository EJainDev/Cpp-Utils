export module annotest:tuple;

import std;

namespace annotest {
template <typename T>
consteval auto getNonstaticDataMembers() {
  return std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current());
}

template <std::size_t I>
struct member_name {
  static constexpr auto value = []() {
    if constexpr (I < 10) {
      return std::array<char, 3>{'m', static_cast<char>('0' + I), '\0'};
    } else {
      return std::array<char, 2>{'m', '\0'};  // Simplified for example
    }
  }();
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