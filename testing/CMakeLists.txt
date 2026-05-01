add_library(cpputils_testing SHARED)
target_sources(
  cpputils_testing
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES
         "Test.cxx")
