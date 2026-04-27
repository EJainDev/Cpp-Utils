export module cpputils.data_structures:stream;

import std;

export namespace cpputils::data_structures {

struct StreamEnd {};

struct FakeStreamPart {
  void operator()() const {}
};

template <typename T>
concept StreamPart = requires(T t) { t(); };

template <typename T, typename Fn, StreamPart Child>
struct ForEach;

template <typename T, typename Fn, StreamPart Child>
struct Filter;

template <typename T, typename Fn, StreamPart Child>
struct Map;

template <typename T, typename GetFn>
struct StreamStart;

template <typename T, typename Fn, StreamPart Child>
struct ForEach {
  Fn for_each_fn;
  Child child;

  T next() const {
    auto val = child.next();
    for_each_fn(val);
    return val;
  }

  std::optional<T> operator()() const {
    try {
      return next();
    } catch (const StreamEnd&) {
      return std::nullopt;
    }
  }

  auto forEach(std::ostream& os, char sep) const {
    auto action = [&os, sep](const T& val) { os << val << sep; };
    return ForEach<T, decltype(action), ForEach<T, Fn, Child>>{std::move(action), *this};
  }

  auto forEach(std::ostream& os, const std::string& sep) const {
    auto action = [&os, sep_copy = std::string(sep)](const T& val) { os << val << sep_copy; };
    return ForEach<T, decltype(action), ForEach<T, Fn, Child>>{std::move(action), *this};
  }

  template <typename ActionFn>
    requires std::invocable<ActionFn, const T&>
  auto forEach(ActionFn action) const {
    return ForEach<T, std::decay_t<ActionFn>, ForEach<T, Fn, Child>>{std::move(action), *this};
  }

  template <typename PredicateFn>
    requires std::invocable<PredicateFn, const T&> &&
             std::convertible_to<std::invoke_result_t<PredicateFn, const T&>, bool>
  auto filter(PredicateFn filter_fn) const {
    return Filter<T, std::decay_t<PredicateFn>, ForEach<T, Fn, Child>>{std::move(filter_fn), *this};
  }

  template <typename MapFn>
    requires std::invocable<MapFn, const T&> &&
             std::convertible_to<std::invoke_result_t<MapFn, const T&>, T>
  auto map(MapFn map_fn) const {
    return Map<T, std::decay_t<MapFn>, ForEach<T, Fn, Child>>{std::move(map_fn), *this};
  }
};

template <typename T, typename Fn, StreamPart Child>
struct Filter {
  Fn filter_fn;
  Child child;

  T next() const {
    while (true) {
      auto val = child.next();
      if (filter_fn(val)) {
        return val;
      }
    }
  }

  std::optional<T> operator()() const {
    try {
      return next();
    } catch (const StreamEnd&) {
      return std::nullopt;
    }
  }

  auto forEach(std::ostream& os, char sep) const {
    auto action = [&os, sep](const T& val) { os << val << sep; };
    return ForEach<T, decltype(action), Filter<T, Fn, Child>>{std::move(action), *this};
  }

  auto forEach(std::ostream& os, const std::string& sep) const {
    auto action = [&os, sep_copy = std::string(sep)](const T& val) { os << val << sep_copy; };
    return ForEach<T, decltype(action), Filter<T, Fn, Child>>{std::move(action), *this};
  }

  template <typename ActionFn>
    requires std::invocable<ActionFn, const T&>
  auto forEach(ActionFn action) const {
    return ForEach<T, std::decay_t<ActionFn>, Filter<T, Fn, Child>>{std::move(action), *this};
  }

  template <typename PredicateFn>
    requires std::invocable<PredicateFn, const T&> &&
             std::convertible_to<std::invoke_result_t<PredicateFn, const T&>, bool>
  auto filter(PredicateFn filter_fn) const {
    return Filter<T, std::decay_t<PredicateFn>, Filter<T, Fn, Child>>{std::move(filter_fn), *this};
  }

  template <typename MapFn>
    requires std::invocable<MapFn, const T&> &&
             std::convertible_to<std::invoke_result_t<MapFn, const T&>, T>
  auto map(MapFn map_fn) const {
    return Map<T, std::decay_t<MapFn>, Filter<T, Fn, Child>>{std::move(map_fn), *this};
  }
};

template <typename T, typename Fn, StreamPart Child>
struct Map {
  Fn map_fn;
  Child child;

  T next() const { return map_fn(child.next()); }

  std::optional<T> operator()() const {
    try {
      return next();
    } catch (const StreamEnd&) {
      return std::nullopt;
    }
  }

  auto forEach(std::ostream& os, char sep) const {
    auto action = [&os, sep](const T& val) { os << val << sep; };
    return ForEach<T, decltype(action), Map<T, Fn, Child>>{std::move(action), *this};
  }

  auto forEach(std::ostream& os, const std::string& sep) const {
    auto action = [&os, sep_copy = std::string(sep)](const T& val) { os << val << sep_copy; };
    return ForEach<T, decltype(action), Map<T, Fn, Child>>{std::move(action), *this};
  }

  template <typename ActionFn>
    requires std::invocable<ActionFn, const T&>
  auto forEach(ActionFn action) const {
    return ForEach<T, std::decay_t<ActionFn>, Map<T, Fn, Child>>{std::move(action), *this};
  }

  template <typename PredicateFn>
    requires std::invocable<PredicateFn, const T&> &&
             std::convertible_to<std::invoke_result_t<PredicateFn, const T&>, bool>
  auto filter(PredicateFn filter_fn) const {
    return Filter<T, std::decay_t<PredicateFn>, Map<T, Fn, Child>>{std::move(filter_fn), *this};
  }

  template <typename MapFn>
    requires std::invocable<MapFn, const T&> &&
             std::convertible_to<std::invoke_result_t<MapFn, const T&>, T>
  auto map(MapFn next_map_fn) const {
    return Map<T, std::decay_t<MapFn>, Map<T, Fn, Child>>{std::move(next_map_fn), *this};
  }
};

template <typename T, typename GetFn>
struct StreamStart {
  GetFn get;
  int size;
  mutable int idx = 0;

  T next() const {
    if (idx < size) {
      return get(idx++);
    }
    throw StreamEnd{};
  }

  std::optional<T> operator()() const {
    try {
      return next();
    } catch (const StreamEnd&) {
      return std::nullopt;
    }
  }

  auto forEach(std::ostream& os, char sep) const {
    auto action = [&os, sep](const T& val) { os << val << sep; };
    return ForEach<T, decltype(action), StreamStart<T, GetFn>>{std::move(action), *this};
  }

  auto forEach(std::ostream& os, const std::string& sep) const {
    auto action = [&os, sep_copy = std::string(sep)](const T& val) { os << val << sep_copy; };
    return ForEach<T, decltype(action), StreamStart<T, GetFn>>{std::move(action), *this};
  }

  template <typename ActionFn>
    requires std::invocable<ActionFn, const T&>
  auto forEach(ActionFn action) const {
    return ForEach<T, std::decay_t<ActionFn>, StreamStart<T, GetFn>>{std::move(action), *this};
  }

  template <typename PredicateFn>
    requires std::invocable<PredicateFn, const T&> &&
             std::convertible_to<std::invoke_result_t<PredicateFn, const T&>, bool>
  auto filter(PredicateFn filter_fn) const {
    return Filter<T, std::decay_t<PredicateFn>, StreamStart<T, GetFn>>{std::move(filter_fn), *this};
  }

  template <typename MapFn>
    requires std::invocable<MapFn, const T&> &&
             std::convertible_to<std::invoke_result_t<MapFn, const T&>, T>
  auto map(MapFn map_fn) const {
    return Map<T, std::decay_t<MapFn>, StreamStart<T, GetFn>>{std::move(map_fn), *this};
  }
};

}  // namespace cpputils::data_structures