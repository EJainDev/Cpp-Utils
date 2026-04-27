cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -freflection") # This is so that the std
                                                       # module also compiles
                                                       # with full C++26 support
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD 1)

project(CppUtils LANGUAGES C CXX)

add_subdirectory(refl_utils)
add_subdirectory(data_structures)

add_executable(test test.cpp)
target_link_libraries(test PRIVATE data_structures)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark PRIVATE data_structures)
