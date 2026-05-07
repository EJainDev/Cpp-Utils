export module cpputils.memory;

import std;

export import :SimpleAllocator;
export import :SimplePool;
export import :Errors;

namespace cpputils::memory {
/**
 * @brief RAII wrapper for allocator-backed memory of type T
 *
 * @details Owns a contiguous block allocated by the provided allocator and
 * releases it on destruction. Copy is disabled; moves transfer ownership.
 *
 * @tparam T The element type managed by this wrapper
 * @tparam Allocator The allocator type used to allocate and deallocate memory
 */
export template <typename T, typename Allocator = allocators::SimpleAllocator<T>>
class Memory {
 public:
  /**
   * @brief Constructs an empty, invalid Memory instance
   */
  Memory() = default;

  Memory(const Memory&) = delete;
  Memory& operator=(const Memory&) = delete;

  Memory(Memory&& other) = default;
  Memory& operator=(Memory&& other) = default;

  /**
   * @brief Releases owned memory, if any
   */
  ~Memory() {
    if (_ptr != nullptr) {
      _allocator.dealloc(_ptr);
    }
  }

  /**
   * @brief Allocates and constructs a Memory instance using the allocator
   *
   * @param allocator The allocator instance to use
   * @param count The number of elements to allocate (must be greater than 0)
   * @return std::optional<Memory> A Memory instance on success, std::nullopt on failure
   */
  static std::optional<Memory> init(Allocator allocator, long long count) pre(count > 0) {
    return allocator.alloc(sizeof(T) * count).transform([&](T* data) {
      return Memory(data, count, allocator);
    });
  }

  /**
   * @brief Releases owned memory and makes this instance invalid
   */
  void dealloc() pre(_ptr != nullptr) {
    _allocator.dealloc(_ptr);
    _ptr = nullptr;
  }

  /**
   * @brief Checks whether this instance currently owns memory
   */
  bool isValid() const { return _ptr != nullptr; }
  /**
   * @brief Implicit validity check
   */
  operator bool() const { return _ptr != nullptr; }

  /**
   * @brief Returns the raw pointer to the allocated memory
   */
  T* const get() const { return _ptr; }
  /**
   * @brief Implicit conversion to raw pointer
   */
  operator T* const() const { return _ptr; }

  /**
   * @brief Bounds-checked element access
   */
  T& at(long long idx) pre(idx >= 0 && idx < _size) { return _ptr[idx]; }
  /**
   * @brief Unchecked element access with precondition
   */
  T& operator[](long long idx) pre(idx >= 0 && idx < _size) { return _ptr[idx]; }
  /**
   * @brief Bounds-checked element access (const)
   */
  const T& at(long long idx) const pre(idx >= 0 && idx < _size) { return _ptr[idx]; }
  /**
   * @brief Unchecked element access with precondition (const)
   */
  const T& operator[](long long idx) const pre(idx >= 0 && idx < _size) { return _ptr[idx]; }

 private:
  Memory(T* data, long long size, Allocator allocator) pre(size > 0) pre(data != nullptr)
      : _ptr(data), _size(size), _allocator(allocator) {}

  T* _ptr = nullptr;
  long long _size = 0;
  Allocator _allocator{};
};
}  // namespace cpputils::memory