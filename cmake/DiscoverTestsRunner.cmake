function(cpputils_discover_tests TARGET)
  # 1. Define the target location for our generated ctest manifest file
  set(GENERATED_TESTS_FILE
      "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_discovered_tests.cmake")

  # 1. Tell CTest to dynamically read this generated file at test time
  set_property(
    DIRECTORY
    APPEND
    PROPERTY TEST_INCLUDE_FILES "${GENERATED_TESTS_FILE}")

  # 1. Create a post-build step that passes the absolute target path to our
  #   script
  add_custom_command(
    TARGET ${TARGET}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -DTEST_EXECUTABLE=$<TARGET_FILE:${TARGET}>
      -DOUTPUT_FILE=${GENERATED_TESTS_FILE} -DTEST_TARGET=${TARGET} -P
      "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/DiscoverTests.cmake"
    COMMENT "Running custom test discovery for ${TARGET}..."
    VERBATIM)
endfunction()
