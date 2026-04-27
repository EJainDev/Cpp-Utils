export module cpputils.data_structures:stream;

import std;

export namespace cpputils::data_structures {

struct FakeStreamPart {
  void operator()() const {}
};

template <typename T>
concept StreamPart = requires(T t) { t(); };

template <typename T, typename Fn, StreamPart Child>
struct Filter;

template <typename T, typename Fn, StreamPart Child>
struct Map;

template <typename T, typename GetFn>
struct StreamStart;

template <typename T, typename Fn, StreamPart Child>
struct Filter {
  Fn filter_fn;
  Child child;

  std::optional<T> operator()() {
    std::optional<T> val = child();
    while (val) {
      if (filter_fn(*val)) {
        return val;
      }
      val = child();
    }
    return std::nullopt;
  }

  template <typename PredicateFn>
  // requires std::invocable<PredicateFn, const T&> &&
  //          std::convertible_to<std::invoke_result_t<PredicateFn, const T&>, bool>
  auto filter(PredicateFn filter_fn) {
    return Filter<T, std::decay_t<PredicateFn>, Filter>{std::move(filter_fn), std::move(*this)};
  }

  template <typename MapFn>
  // requires std::invocable<MapFn, const T&> &&
  //          std::convertible_to<std::invoke_result_t<MapFn, const T&>, T>
  auto map(MapFn map_fn) {
    return Map<T, std::decay_t<MapFn>, Filter>{std::move(map_fn), std::move(*this)};
  }
};

template <typename T, typename Fn, StreamPart Child>
struct Map {
  Fn map_fn;
  Child child;

  std::optional<T> operator()() { return child().transform(map_fn); }

  template <typename PredicateFn>
  // requires std::invocable<PredicateFn, const T&> &&
  //          std::convertible_to<std::invoke_result_t<PredicateFn, const T&>, bool>
  auto filter(PredicateFn filter_fn) {
    return Filter<T, std::decay_t<PredicateFn>, Map>{std::move(filter_fn), std::move(*this)};
  }

  template <typename MapFn>
  // requires std::invocable<MapFn, const T&> &&
  //          std::convertible_to<std::invoke_result_t<MapFn, const T&>, T>
  auto map(MapFn next_map_fn) {
    return Map<T, std::decay_t<MapFn>, Map>{std::move(next_map_fn), std::move(*this)};
  }
};

template <typename T, typename GetFn, typename Child = int>
struct StreamStart {
  GetFn get;
  T* data;
  int size;
  int idx = 0;

  std::optional<T> operator()() {
    if (idx < size) {
      return get(data, idx++);
    }
    return std::nullopt;
  }

  template <typename PredicateFn>
  // requires std::invocable<PredicateFn, const T&> &&
  //          std::convertible_to<std::invoke_result_t<PredicateFn, const T&>, bool>
  auto filter(PredicateFn filter_fn) {
    return Filter<T, std::decay_t<PredicateFn>, StreamStart>{std::move(filter_fn),
                                                             std::move(*this)};
  }

  template <typename MapFn>
  // requires std::invocable<MapFn, const T&> &&
  //          std::convertible_to<std::invoke_result_t<MapFn, const T&>, T>
  auto map(MapFn map_fn) {
    return Map<T, std::decay_t<MapFn>, StreamStart>{std::move(map_fn), std::move(*this)};
  }
};

}  // namespace cpputils::data_structures