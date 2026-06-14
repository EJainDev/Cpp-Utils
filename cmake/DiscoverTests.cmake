# MyCustomDiscovery.cmake This script executes post-build, NOT during
# configuration.

# 1. Execute the binary using its full absolute path
execute_process(
  COMMAND "${TEST_EXECUTABLE}" --list
  OUTPUT_VARIABLE test_output
  OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULT_VARIABLE result)

# 1. Convert text output into an iterable list of lines
string(REPLACE "\n" ";" line_list "${test_output}")

# 1. Initialize/overwrite the generated test registration file
file(WRITE "${OUTPUT_FILE}"
     "# Generated dynamically by custom test discovery\n")

set(CURRENT_SUITE "")

foreach(line IN LISTS line_list)
  # Strip leading/trailing spaces to analyze clean content
  string(STRIP "${line}" clean_line)

  if(clean_line STREQUAL "")
    continue()
  endif()

  # Detect Suite line (Ends with a dot, e.g., "Suite1.")
  if(clean_line MATCHES "\\.$")
    # Extract the suite name by stripping the trailing dot
    string(REGEX REPLACE "\\.$" "" CURRENT_SUITE "${clean_line}")
  else()
    # Ensure we have a valid suite context before processing tests
    if(CURRENT_SUITE STREQUAL "")
      message(
        WARNING "Skipping test case '${clean_line}' found before a valid suite."
      )
      continue()
    endif()

    # Construct a clean test identifier for CTest (e.g., Suite1.Name)
    set(FULL_TEST_NAME "${CURRENT_SUITE}.${clean_line}")

    # Append individual test registrations to the target file. This passes
    # --run-suite and --run-test to your binary. Change args if needed!
    file(
      APPEND "${OUTPUT_FILE}"
      "add_test(\"${FULL_TEST_NAME}\" \"${TEST_EXECUTABLE}\" --suite-name \"${CURRENT_SUITE}\" --test-name \"${clean_line}\")\n"
    )
  endif()
endforeach()
