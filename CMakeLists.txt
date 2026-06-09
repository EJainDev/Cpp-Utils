cmake_minimum_required(VERSION 4.3)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
set(CMAKE_CXX_MODULE_STD 1)

project(
  CppUtils
  VERSION 0.1.0.0
  LANGUAGES C CXX)

option(REFL_LIBRARY "Build the reflection utility library" ON)
option(TESTING_LIBRARY "Build the testing library" ON)
option(ENABLE_TESTING "Enable building and execution of tests" OFF)
option(BUILD_EXAMPLES "Build example applications" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(TESTING_LIBRARY)
  set(REFL_LIBRARY ON)
endif()

if(ENABLE_TESTING)
  include(CTest)

  option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

  if(ENABLE_COVERAGE)
    find_program(GCOV_PATH gcov)

    if(GCOV_PATH)
      set(CTEST_COVERAGE_COMMAND "${GCOV_PATH}")
    endif()

    add_compile_options(-O0 -g --coverage)
    add_link_options(--coverage)
  endif()
endif()

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -freflection")

add_library(cpputils INTERFACE)
add_library(cpputils::cpputils ALIAS cpputils)

# Sub-libraries
if(REFL_LIBRARY)
  add_subdirectory(refl)
endif()

if(TESTING_LIBRARY)
  add_subdirectory(testing)
endif()

if(REFL_LIBRARY)
  target_link_libraries(cpputils INTERFACE cpputils_refl)
endif()

if(TESTING_LIBRARY)
  target_link_libraries(cpputils INTERFACE cpputils_testing)
endif()

set(CPPUTILS_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/CppUtils")

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CppUtilsConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/CppUtilsConfig.cmake"
  INSTALL_DESTINATION "${CPPUTILS_INSTALL_CMAKEDIR}")

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/CppUtilsConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion)

install(
  TARGETS cpputils
  EXPORT cpputils_targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(
  EXPORT cpputils_targets
  NAMESPACE cpputils::
  DESTINATION "${CPPUTILS_INSTALL_CMAKEDIR}")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/CppUtilsConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/CppUtilsConfigVersion.cmake"
        DESTINATION "${CPPUTILS_INSTALL_CMAKEDIR}")
