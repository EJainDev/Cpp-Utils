cmake_minimum_required(VERSION 4.3)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
set(CMAKE_CXX_MODULE_STD 1)

project(
  AnnoTest
  VERSION 0.1.0.0
  LANGUAGES C CXX)

option(BUILD_TESTS "Enable building of tests" OFF)
option(BUILD_EXAMPLES "Build example applications" OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(BUILD_TESTS)
  include(CTest)
  enable_testing()
endif()

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -freflection")

# Library

add_library(annotest STATIC)
target_sources(
  annotest
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "${CMAKE_CURRENT_SOURCE_DIR}/include"
         FILES
         "${CMAKE_CURRENT_SOURCE_DIR}/include/Tuple.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/Exceptions.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/Asserts.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/Expects.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/DeathTest.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/Test.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/Posix.cxx")
target_compile_features(annotest PUBLIC cxx_std_26)

# Installation configuration

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DiscoverTestsRunner.cmake)

install(
  TARGETS annotest
  EXPORT annotest_targets
  FILE_SET modules
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/annotest
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(CPPUTILS_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/AnnoTest")

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/AnnoTestConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/AnnoTestConfig.cmake"
  INSTALL_DESTINATION "${CPPUTILS_INSTALL_CMAKEDIR}")

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/AnnoTestConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion)

install(
  EXPORT annotest_targets
  NAMESPACE annotest::
  DESTINATION "${CPPUTILS_INSTALL_CMAKEDIR}")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DiscoverTests.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DiscoverTestsRunner.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/AnnoTest")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/AnnoTestConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/AnnoTestConfigVersion.cmake"
        DESTINATION "${CPPUTILS_INSTALL_CMAKEDIR}")

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
