import cpputils.testing;
import cpputils.memory;

import std;

template <typename T>
struct Allocator {
  int alloc_count;
  int dealloc_count;

  ~Allocator() { cpputils::testing::assertEqual(alloc_count, dealloc_count); }

  std::optional<T*> alloc(std::size_t size) {
    ++alloc_count;
    return {nullptr};
  }

  void dealloc(T* ptr) { ++dealloc_count; }
};

class MemoryTests {
 public:
  [[= cpputils::testing::Test()]] void simpleTest() {
    Allocator<int> allocator;
    cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 1);
  }
};

int main() {
  cpputils::testing::test(MemoryTests{});
  return 0;
}