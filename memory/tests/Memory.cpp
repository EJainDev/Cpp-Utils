import cpputils.testing;
import cpputils.memory;

import std;

using namespace cpputils::testing;

template <typename T>
struct Allocator {
  struct Data {
    int alloc_count;
  };

  std::shared_ptr<Data> data = std::make_shared<Data>();

  std::optional<T*> alloc(std::size_t size) {
    ++data->alloc_count;
    return new T[size / sizeof(T)];
  }

  void dealloc(T* ptr) {
    --data->alloc_count;
    delete[] ptr;
  }
};

class MemoryTests {
 public:
  [[= Test{}]][[= Parameterize<3, int>{Tuple<int>{1}, Tuple<int>{5}, Tuple<int>{10}}]] void
      basicSingleAlloc(int size = 1) {
    Allocator<int> allocator;
    {
      cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, size);
    }
    cpputils::testing::assertEqual(0, allocator.data->alloc_count);
  }

  [[= cpputils::testing::Test{}]] void forcedDealloc() {
    Allocator<int> allocator;
    {
      auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 1);
      mem->dealloc();
    }
    cpputils::testing::assertEqual(0, allocator.data->alloc_count);
  }
};

int main() {
  cpputils::testing::test(MemoryTests{});
  return 0;
}