import cpputils.memory;

#include <gtest/gtest.h>

template <typename T>
struct Allocator {
  int alloc_count;
  int dealloc_count;

  ~Allocator() { EXPECT_EQ(alloc_count, dealloc_count); }

  std::optional<T*> alloc(size_t size) {
    ++alloc_count;
    return {nullptr};
  }

  void dealloc(T* ptr) { ++dealloc_count; }
};

TEST(MemoryTests, Init) {
  Allocator<int> allocator;
  cpputils::memory::Memory<int>::init(allocator, 1);
}