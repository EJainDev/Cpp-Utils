option(HARDENING "Enable hardening flags" ON)

add_library(data_structures SHARED)
target_sources(
  data_structures
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         ${CMAKE_CURRENT_SOURCE_DIR}
         FILES
         data_structures.cxx
         stream.cxx
         vector.cxx
         array.cxx)
if(HARDENING)
  target_compile_definitions(data_structures PRIVATE HARDENED)
endif()
