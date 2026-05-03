cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD 1)

project(CppUtils LANGUAGES C CXX)

option(ENABLE_TESTING "Enable building and execution of tests" OFF)

if(ENABLE_TESTING)
  find_program(GCOV_PATH gcov)
  if(GCOV_PATH)
    set(CTEST_COVERAGE_COMMAND "${GCOV_PATH}")
  endif()

  include(CTest)

  find_package(GTest REQUIRED)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(--coverage)
    add_link_options(--coverage)

    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g -O0")
  endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -freflection")

add_subdirectory(refl_utils)
add_subdirectory(testing)
add_subdirectory(memory)

# add_executable(test test.cpp) target_link_libraries(test PRIVATE
# data_structures)

# add_executable(benchmark benchmark.cpp) target_link_libraries(benchmark
# PRIVATE data_structures)
