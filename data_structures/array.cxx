export module cpputils.data_structures:array;

import :stream;
import std;

#ifdef HARDENED
#define BOUND_CHECK pre(index < N)
#else
#define BOUND_CHECK
#endif

namespace cpputils::data_structures {
template <typename T, int N>
class array {
 public:
  array(std::initializer_list<T> list) pre(list.size() <= N) {
    std::copy(list.begin(), list.end(), _data);
  }

  const T& operator[](int index) const noexcept BOUND_CHECK { return _data[index]; }
  T& operator[](int index) noexcept BOUND_CHECK { return _data[index]; }

 private:
  T _data[N];
};
}  // namespace cpputils::data_structures