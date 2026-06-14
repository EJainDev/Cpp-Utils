add_library(cpputils_testing STATIC)
target_sources(
  cpputils_testing
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES
         "${CMAKE_CURRENT_SOURCE_DIR}/Tuple.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/Exceptions.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/Asserts.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/Expects.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/DeathTest.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/Test.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/Posix.cxx")
target_link_libraries(cpputils_testing PUBLIC cpputils_refl)
target_compile_features(cpputils_testing PUBLIC cxx_std_26)

add_library(cpputils::testing ALIAS cpputils_testing)

add_dependencies(cpputils cpputils_testing)

include(${PROJECT_SOURCE_DIR}/cmake/DiscoverTestsRunner.cmake)

install(
  TARGETS cpputils_testing
  EXPORT cpputils_targets
  FILE_SET modules
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpputils/testing
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()
