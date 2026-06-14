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
         "Tuple.cxx"
         "Exceptions.cxx"
         "Asserts.cxx"
         "Expects.cxx"
         "DeathTest.cxx"
         "Test.cxx"
         "Posix.cxx")
target_link_libraries(cpputils_testing PUBLIC cpputils_refl)

add_library(cpputils::testing ALIAS cpputils_testing)

add_dependencies(cpputils cpputils_testing)

include(${PROJECT_SOURCE_DIR}/cmake/CppUtils/DiscoverTestsRunner.cmake)

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
