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
  [[= Test{}]] void defaultConstructor() { cpputils::memory::Memory<int> mem; }

  [[= Test{}]][[= Parameterize<3, int>{tuple(1), tuple(5), tuple(10)}]] void basicSingleAlloc(
      int size = 1) {
    Allocator<int> allocator;
    {
      cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, size);
    }
    cpputils::testing::assertEqual(0, allocator.data->alloc_count);
  }

  [[= cpputils::testing::Test{}]][[= Parameterize<3, int>{tuple(1), tuple(5), tuple(10)}]] void
      forcedDealloc(int size = 1) {
    Allocator<int> allocator;
    {
      auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, size);
      mem->dealloc();
    }
    cpputils::testing::assertEqual(0, allocator.data->alloc_count);
  }
};

int main(int argc, char** argv) { return cpputils::testing::test<MemoryTests>(argc, argv); }