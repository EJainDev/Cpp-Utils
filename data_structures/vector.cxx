export module cpputils.data_structures:vector;

import :stream;

#ifdef HARDENED
#define BOUND_CHECK pre(index < _size)
#else
#define BOUND_CHECK
#endif

namespace cpputils::data_structures {
export template <typename T>
class vector {
 public:
  // For STL compatibility
  class Iterator {
   public:
    // These traits allow the iterator to work with <algorithm> functions
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    Iterator(pointer ptr) : m_ptr(ptr) {}

    // Dereference operators
    reference operator*() const { return *m_ptr; }
    pointer operator->() { return m_ptr; }

    // Increment / Decrement
    Iterator& operator++() {
      m_ptr++;
      return *this;
    }
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    Iterator& operator--() {
      m_ptr--;
      return *this;
    }
    Iterator operator--(int) {
      Iterator tmp = *this;
      --(*this);
      return tmp;
    }

    // Element access
    reference operator[](difference_type n) { return m_ptr[n]; }

    // Arithmetic
    Iterator& operator+=(difference_type n) {
      m_ptr += n;
      return *this;
    }
    Iterator& operator-=(difference_type n) {
      m_ptr -= n;
      return *this;
    }
    Iterator operator+(difference_type n) const { return Iterator(m_ptr + n); }
    Iterator operator-(difference_type n) const { return Iterator(m_ptr - n); }
    difference_type operator-(const Iterator& other) const { return m_ptr - other.m_ptr; }

    // Comparison
    bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }
    bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }
    bool operator<(const Iterator& other) const { return m_ptr < other.m_ptr; }
    bool operator>(const Iterator& other) const { return m_ptr > other.m_ptr; }
    bool operator<=(const Iterator& other) const { return m_ptr <= other.m_ptr; }
    bool operator>=(const Iterator& other) const { return m_ptr >= other.m_ptr; }

   private:
    pointer m_ptr;
  };

  vector() = default;

  ~vector() { delete[] _data; }

  vector(std::initializer_list<T> list)
      : _data(new T[list.size()]), _size(list.size()), _capacity(list.size()) {
    std::copy(list.begin(), list.end(), _data);
  }
  vector(int size) : _data(new T[size]), _size(size), _capacity(size) {}

  template <StreamPart Init>
  vector(Init init) : _data(new T[4]), _size(0), _capacity(4) {
    auto val = init();
    while (val.has_value()) {
      add(val.value());
      val = init();
    }
  }

  const T& operator[](int index) const noexcept BOUND_CHECK { return _data[index]; }
  T& operator[](int index) noexcept BOUND_CHECK { return _data[index]; }

  void add(T val) {
    if (_size >= _capacity) {
      if (_capacity == 0) {
        _capacity = 1;
      } else {
        _capacity *= 2;
      }
      T* new_data = new T[_capacity];
      std::move(_data, _data + _size, new_data);
      delete[] _data;
      _data = new_data;
    }
    _data[_size++] = val;
  }

  int size() const { return _size; }

  auto stream() const {
    auto get = [](T* data, int idx) -> const T& { return data[idx]; };
    return StreamStart<T, decltype(get)>{std::move(get), _data, _size};
  }

  // For STL compatibility
  Iterator begin() { return Iterator(_data); }
  Iterator end() { return Iterator(_data + _size); }
  Iterator begin() const { return Iterator(_data); }
  Iterator end() const { return Iterator(_data + _size); }

 private:
  T* _data;
  int _size;
  int _capacity;
};
}  // namespace cpputils::data_structures