import cpputils.testing;
import cpputils.memory;

import std;

template <typename T>
struct Allocator {
  std::size_t alloc_size;
  std::size_t dealloc_size;

  std::optional<T*> alloc(std::size_t size) {
    alloc_size += size;
    return new T[size / sizeof(T)];
  }

  void dealloc(T* ptr) {
    dealloc_size += sizeof(T);
    delete[] ptr;
  }
};

class MemoryTests {
 public:
  void simpleTest() {
    Allocator<int> allocator;
    cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 1);
  }
};

int main() {
  cpputils::testing::test(MemoryTests{});
  return 0;
}