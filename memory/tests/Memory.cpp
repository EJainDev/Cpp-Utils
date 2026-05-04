import cpputils.testing;
import cpputils.memory;

import std;

using namespace cpputils::memory;
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
  [[= Test{}]] void defaultConstructor() {
    Memory<int> mem;
    assertEqual(static_cast<bool>(mem), mem.isValid());
    assertFalse(static_cast<bool>(mem));
    assertEqual(static_cast<int*>(mem), mem.get());
    assertNull(static_cast<int*>(mem));
    assertDeath([&]() { mem.dealloc(); });

    assertDeath([&]() { mem[0]; });
    assertDeath([&]() { mem.at(0); });

    [](const Memory<int>& mem) {
      assertDeath([&]() { mem[0]; });
      assertDeath([&]() { mem.at(0); });
    }(mem);
  }

  [[= Test{}]][[= Parameterize{tuple(1), tuple(5), tuple(10)}]] void basicSingleAlloc(
      int size = 1) {
    Allocator<int> allocator;
    {
      cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, size);
    }
    cpputils::testing::assertEqual(0, allocator.data->alloc_count);
  }

  [[= cpputils::testing::Test{}]][[= Parameterize{tuple(1), tuple(5), tuple(10)}]] void
      forcedDealloc(int size = 1) {
    Allocator<int> allocator;
    {
      auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, size);
      mem->dealloc();
    }
    cpputils::testing::assertEqual(0, allocator.data->alloc_count);
  }

  [[= Test{}]] void isValidAfterInit() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    cpputils::testing::assertEqual(true, mem->isValid());
  }

  [[= Test{}]] void isValidAfterDealloc() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    mem->dealloc();
    cpputils::testing::assertEqual(false, mem->isValid());
  }

  [[= Test{}]] void operatorBoolAfterInit() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    cpputils::testing::assertEqual(true, static_cast<bool>(*mem));
  }

  [[= Test{}]] void operatorBoolAfterDealloc() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    mem->dealloc();
    cpputils::testing::assertEqual(false, static_cast<bool>(*mem));
  }

  [[= Test{}]] void getReturnsNullptrWhenInvalid() {
    Allocator<int> allocator;
    cpputils::memory::Memory<int, decltype(allocator)> mem;
    cpputils::testing::assertEqual(nullptr, mem.get());
  }

  [[= Test{}]] void getReturnsValidPointerAfterInit() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    cpputils::testing::assertNotEqual(nullptr, mem->get());
  }

  [[= Test{}]] void implicitPointerConversion() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    int* ptr = *mem;
    cpputils::testing::assertNotEqual(nullptr, ptr);
  }

  [[= Test{}]][[= Parameterize<3, int>{tuple(1), tuple(5), tuple(10)}]] void elementAccessAt(
      int size = 1) {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, size);
    mem->at(0) = 42;
    cpputils::testing::assertEqual(42, mem->at(0));
  }

  [[= Test{}]][[= Parameterize<3, int>{tuple(1), tuple(5), tuple(10)}]] void elementAccessSubscript(
      int size = 1) {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, size);
    (*mem)[0] = 99;
    cpputils::testing::assertEqual(99, (*mem)[0]);
  }

  [[= Test{}]] void constElementAccessAt() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    mem->at(0) = 123;
    const auto& constMem = *mem;
    cpputils::testing::assertEqual(123, constMem.at(0));
  }

  [[= Test{}]] void constElementAccessSubscript() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    (*mem)[1] = 456;
    const auto& constMem = *mem;
    cpputils::testing::assertEqual(456, constMem[1]);
  }

  [[= Test{}]] void moveConstructor() {
    Allocator<int> allocator;
    auto mem1 = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    mem1->at(0) = 777;
    auto mem2 = std::move(*mem1);
    cpputils::testing::assertEqual(777, mem2.at(0));
  }

  [[= Test{}]] void moveAssignmentOperator() {
    Allocator<int> allocator;
    auto mem1 = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    auto mem2 = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 3);
    mem1->at(0) = 555;
    mem2 = std::move(*mem1);
    cpputils::testing::assertEqual(555, mem2->at(0));
  }

  [[= Test{}]] void multipleElementWrites() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 10);
    for (int i = 0; i < 10; ++i) {
      mem->at(i) = i * 2;
    }
    for (int i = 0; i < 10; ++i) {
      cpputils::testing::assertEqual(i * 2, mem->at(i));
    }
  }

  [[= Test{}]] void deallocClearsMemory() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    mem->dealloc();
    cpputils::testing::assertEqual(nullptr, mem->get());
  }

  [[= Test{}]] void allocatorCallCountInit() {
    Allocator<int> allocator;
    cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    cpputils::testing::assertEqual(1, allocator.data->alloc_count);
  }

  [[= Test{}]] void allocatorCallCountInitAndDealloc() {
    Allocator<int> allocator;
    auto mem = cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 5);
    cpputils::testing::assertEqual(1, allocator.data->alloc_count);
    mem->dealloc();
    cpputils::testing::assertEqual(0, allocator.data->alloc_count);
  }

  [[= Test{}]] void differentTypes() {
    Allocator<double> allocator;
    auto mem = cpputils::memory::Memory<double, decltype(allocator)>::init(allocator, 3);
    mem->at(0) = 3.14;
    cpputils::testing::assertEqual(3.14, mem->at(0));
  }

  [[= Test{}]] void defaultConstructorIsInvalid() {
    cpputils::memory::Memory<int> mem;
    cpputils::testing::assertEqual(false, mem.isValid());
  }
};

int main(int argc, char** argv) { return cpputils::testing::test<MemoryTests>(argc, argv); }