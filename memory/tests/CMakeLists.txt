add_executable(MemoryTests Memory.cpp)
target_link_libraries(MemoryTests PRIVATE cpputils_memory GTest::gtest_main)
