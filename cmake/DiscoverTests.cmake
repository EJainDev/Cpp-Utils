# Execute the testing binary in list mode
# Note: Test names containing semicolons, equals signs, or other CMake-special
# characters may cause malformed add_test() calls. Use only alphanumeric,
# underscore, hyphen, and space characters in test names.
execute_process(
  COMMAND "${TEST_EXECUTABLE}" --list
  OUTPUT_VARIABLE test_output
  OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULT_VARIABLE result)

# Basic parsing into a list
string(REPLACE "\n" ";" line_list "${test_output}")

# Create test file
file(WRITE "${OUTPUT_FILE}"
     "# Generated dynamically by custom test discovery\n")

set(CURRENT_SUITE "")

foreach(line IN LISTS line_list)
  # Parse output
  string(STRIP "${line}" clean_line)

  if(clean_line STREQUAL "")
    continue()
  endif()

  if(clean_line MATCHES "\\.$")
    string(REGEX REPLACE "\\.$" "" CURRENT_SUITE "${clean_line}")
  else()
    if(CURRENT_SUITE STREQUAL "")
      message(
        WARNING "Skipping test case '${clean_line}' found before a valid suite."
      )
      continue()
    endif()

    # Construct a clean test identifier for CTest (e.g., Suite1.Name)
    set(FULL_TEST_NAME "${CURRENT_SUITE}.${clean_line}")

    # Append individual test registrations to the target file
    file(
      APPEND "${OUTPUT_FILE}"
      "add_test(\"${FULL_TEST_NAME}\" \"${TEST_EXECUTABLE}\" --test-name \"${FULL_TEST_NAME}\")\n"
    )
  endif()
endforeach()
