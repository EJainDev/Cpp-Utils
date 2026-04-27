add_library(refl_utils SHARED)
target_sources(
  refl_utils
  PUBLIC FILE_SET
         public_modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         ${CMAKE_CURRENT_SOURCE_DIR}
         FILES
         utils.cxx)
