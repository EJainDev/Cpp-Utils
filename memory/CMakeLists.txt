add_library(cpputils_memory SHARED)
target_sources(
  cpputils_memory
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES
         "Errors.cxx"
         "Memory.cxx"
         "allocators/SimpleAllocator.cxx"
         "allocators/SimplePool.cxx")

add_subdirectory(tests)
