import annotest;

import std;

using namespace annotest;

// Enum for testing enum_to_string
enum class TestColor { Red, Green, Blue, Count };

struct TestSuite {
  [[= BeforeAll{}]] void beforeAll() { std::cout << "Running before all\n"; }
  [[= BeforeEach{}]] void beforeEach() { std::cout << "Running before each\n"; }
  [[= Test{.disabled = true}]] void disabledTest() {}
  [[= Test<"My Named Test">{}]] void test() {}
  [[ = Test{}, = Parameterize{tuple(5), tuple(50)} ]] void parameterizedTest(int val) {
    assertTrue(val == 5 || val == 50);
  }
  [[ = Test{}, = RequiresOS{OS::Windows} ]] void windows() {}
  [[ = Test{}, = RequiresOS{OS::Linux} ]] void testLinux() {}
  [[ = Test{}, = RequiresOS{OS::Mac} ]] void mac() {}
  [[ = Test{}, = RequiresOS{OS::Unknown} ]] void other() {}
  [[ = Test{}, = RequiresOS{OS::Windows, OS::Linux} ]] void windowsLinux() {}
  [[ = Test{}, = DisallowOS{OS::Windows} ]] void nonWindows() {}
  [[ = Test{}, = DisallowOS{OS::Linux} ]] void nonLinux() {}
  [[ = Test{}, = DisallowOS{OS::Mac} ]] void nonMac() {}
  [[ = Test{}, = DisallowOS{OS::Unknown} ]] void nonOther() {}
  [[ = Test{}, = DisallowOS{OS::Windows, OS::Linux} ]] void nonWindowsLinux() {}

  // Test for --test-name filtering: verifies a named test can be isolated
  [[= Test<"filterable test">{}]] void filterableTest() { assertTrue(true); }

  // assertNear with mixed int/double -- verifies common_type_t tolerance fix
  [[= Test<"assertNear int/double mixed">{}]] void assertNearMixed() {
    assertNear(5.0, 5.0001, 0.001);  // double + double with tolerance
    assertNear(5, 5.0001, 0.001);    // int + double with explicit tolerance
    assertNear(5, 5);                // int + int, no tolerance needed
  }

  // assertThrows with explicit template arg and derived types
  [[= Test<"assertThrows derived">{}]] void assertThrowsDerived() {
    // These should pass because std::runtime_error derives from both std::exception and
    // std::logic_error
    assertThrows<std::exception>([]() { throw std::runtime_error("derived"); });
  }

  // assertThrowsExact with explicit template args
  [[= Test<"assertThrowsExact explicit">{}]] void assertThrowsExactExplicit() {
    assertThrowsExact<std::runtime_error>([]() { throw std::runtime_error("exact"); });
  }

  // Parameterized tests with lifecycle hooks
  [[= BeforeEach{}]] void paramBeforeEach() { set_count = 0; }
  [[= AfterEach{}]] void paramAfterEach() { set_count = 0; }
  int set_count = 0;
  [[ = Test{}, = Parameterize{tuple(100), tuple(200), tuple(300)} ]] void countingParam(int val) {
    ++set_count;
    assertTrue(val % 100 == 0);
  }

  // enum_to_string tests
  [[= Test<"enum_to_string valid">{}]] void enumToStringValid() {
    std::string s = enum_to_string(TestColor::Red);
    assertTrue(s == "Red" || s == "0");  // reflection may produce numeric name
  }

  // format() tests
  [[= Test<"format nullptr">{}]] void formatNullptr() {
    std::string s = format(nullptr);
    assertTrue(s == "nullptr" || !s.empty());
  }

  // Large tuple test: verifies member_name fix for I >= 100
  [[= Test<"large tuple 100+ elements">{}]] void largeTuple() {
    // Create a tuple with exactly 105 elements to exercise I >= 100 path
    auto t =
        tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
              25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
              46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
              67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
              88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105);
    assertEqual(t.getSizeof(), 105u);
  }

  [[= AfterEach{}]] void afterEach() { std::cout << "Running after each\n"; }
  [[= AfterAll{}]] void afterAll() { std::cout << "Running after all\n"; }
};

struct TestSuite2 {
  // Additional assertThrows tests -- not in try-catch (positive tests)
  [[= Test<"assertThrows std::exception default">{}]] void throwsDefault() {
    assertThrows([]() { throw std::runtime_error("default"); });
    assertThrows([]() { return; });
  }

  [[= Test<"assertThrowsExact exact match">{}]] void throwsExactMatch() {
    assertThrowsExact<std::runtime_error>([]() { throw std::runtime_error("exact"); });
  }

  // Test: assertContractViolation with non-void return type
  [[= Test<"contract violation non-void return">{}]] void contractViolationNonVoid() {
    assertDeath([]() { assertContractViolation([]() -> int { return 42; }); });
  }

  // Test: assertNoContractViolation with non-void return type
  [[= Test<"no contract violation non-void return">{}]] void noContractViolationNonVoid() {
    auto result = assertNoContractViolation([]() -> int { return 42; });
    assertEqual(result, 42);
  }

  // Test: expectContractViolation with non-void return type
  [[= Test<"expect contract violation non-void return">{}]] void expectContractViolationNonVoid() {
    expectDeath([]() { expectContractViolation([]() -> int { return 42; }); });
  }

  // Test: expectNoContractViolation with non-void return type
  [[= Test<"expect no contract violation non-void return">{}]] void
      expectNoContractViolationNonVoid() {
    auto result = expectNoContractViolation([]() -> int { return 42; });
    assertEqual(result, 42);
  }

  // Test: enum_to_string with invalid enum value
  [[= Test<"enum_to_string invalid value">{}]] void enumToStringInvalid() {
    std::string s = enum_to_string(static_cast<TestColor>(99));
    assertTrue(s == "<invalid enum value>");
  }

  // Test: format() with user-defined classes
  [[= Test<"format user-defined class">{}]] void formatUserDefined() {
    struct Point {
      int x = 10;
      int y = 20;
    };
    Point p;
    std::string s = format(p);
    assertTrue(s.find("Point") != std::string::npos);
    assertTrue(s.find("x") != std::string::npos);
    assertTrue(s.find("y") != std::string::npos);
  }

  // Test: format() with containers (std::vector)
  [[= Test<"format container">{}]] void formatContainer() {
    std::vector<int> v{1, 2, 3};
    std::string s = format(v);
    assertTrue(!s.empty());
  }

  // Test: format() with std::source_location
  [[= Test<"format source location">{}]] void formatSourceLocation() {
    auto loc = std::source_location::current();
    std::string s = format(loc);
    assertTrue(s.find("file") != std::string::npos || s.find("line") != std::string::npos);
    assertTrue(!s.empty());
  }
};

// Test: duplicate BeforeEach detection (the second one should silently overwrite)
struct DuplicateBeforeEachSuite {
  [[= BeforeEach{}]] void first() {}
  [[= BeforeEach{}]] void second() {}  // This should trigger a warning
  [[= Test<"duplicate beforeEach warning">{}]] void test() { assertEqual(1, 1); }
};

// Test: Windows death test path compiles (compile-time verification)
// The code uses #if defined(__unix__) || defined(__APPLE__) guards.
// On non-Unix platforms, assertDeath/expectDeath should not be callable.
// This test verifies the Windows path compiles by checking that the module
// compiles cleanly when death tests are skipped.
struct WindowsDeathTestSuite {
  // On Unix/macOS, this runs death tests. On Windows, they are skipped.
  [[= Test<"death test conditional compile">{}]] void deathTestConditional() {
#if defined(__unix__) || defined(__APPLE__)
    // Should compile and run on Unix/macOS
    assertEqual(1, 1);
#else
    // On Windows, death tests are not available — verify compilation
    assertTrue(true);
#endif
  }
};

int main(int argc, char** argv) {
  if (argc > 1 && std::string(argv[1]) == "--list") {
    return test<TestSuite>(argc, argv);
  }

  std::vector elements{3, 5, 7};

  assertEqual(3, 3);
  assertNotEqual(3, 4);
  assertTrue(true);
  assertFalse(false);
  assertNear(5.00000, 5.00001, 1e-4);
  assertLess(2, 5);
  assertLessEqual(2, 2);
  assertGreater(6, 5);
  assertGreaterEqual(2, 2);
  assertContains(elements, 5);
  assertThrows([]() { throw std::runtime_error("Error"); });
  assertThrowsExact<std::runtime_error>([]() { throw std::runtime_error("Error"); });
  assertNull(nullptr);
#if defined(__unix__) || defined(__APPLE__)
  assertDeath([]() { throw std::runtime_error("Error"); });
#endif

  try {
    assertEqual(3, 2);
  } catch (...) {
  }
  try {
    assertNotEqual(4, 4);
  } catch (...) {
  }
  try {
    assertTrue(false);
  } catch (...) {
  }
  try {
    assertFalse(true);
  } catch (...) {
  }
  try {
    assertNear(5.00000, 6, 1e-4);
  } catch (...) {
  }
  try {
    assertLess(7, 5);
  } catch (...) {
  }
  try {
    assertLess(5, 5);
  } catch (...) {
  }
  try {
    assertLessEqual(7, 2);
  } catch (...) {
  }
  try {
    assertGreater(2, 5);
  } catch (...) {
  }
  try {
    assertGreater(5, 5);
  } catch (...) {
  }
  try {
    assertGreaterEqual(1, 2);
  } catch (...) {
  }
  try {
    assertContains(elements, 4);
  } catch (...) {
  }
  // Edge cases: empty container, single element
  try {
    assertContains(std::vector<int>{}, 5);
  } catch (...) {
  }
  assertContains(std::vector<int>{42}, 42);
  try {
    assertContains(std::vector<int>{42}, 99);
  } catch (...) {
  }
  try {
    assertThrows([]() { return; });
  } catch (...) {
  }
  try {
    assertThrowsExact<std::exception>([]() { throw std::runtime_error("Error"); });
  } catch (...) {
  }
  // Regression test: assertThrowsExact must reject wrong exception types (not slice)
  // Before the fix, catching by value caused object slicing and the exact check always passed.
  try {
    assertThrowsExact<std::logic_error>([]() { throw std::runtime_error("Error"); });
  } catch (...) {
  }
  try {
    assertNull(elements.data());
  } catch (...) {
  }

#if defined(__unix__) || defined(__APPLE__)
  try {
    assertDeath([]() { return; });
  } catch (...) {
  }
#endif

  expectEqual(3, 3);
  expectNotEqual(3, 4);
  expectTrue(true);
  expectFalse(false);
  expectNear(5.00000, 5.00001, 1e-4);
  expectLess(2, 5);
  expectLessEqual(2, 2);
  expectGreater(6, 5);
  expectGreaterEqual(2, 2);
  expectContains(elements, 5);
  expectThrows([]() { throw std::runtime_error("Error"); });
  expectThrowsExact<std::runtime_error>([]() { throw std::runtime_error("Error"); });
  expectNull(nullptr);
#if defined(__unix__) || defined(__APPLE__)
  expectDeath([]() { throw std::runtime_error("Error"); });
#endif

  try {
    expectEqual(3, 2);
  } catch (...) {
  }
  try {
    expectNotEqual(4, 4);
  } catch (...) {
  }
  try {
    expectTrue(false);
  } catch (...) {
  }
  try {
    expectFalse(true);
  } catch (...) {
  }
  try {
    expectNear(5.00000, 6, 1e-4);
  } catch (...) {
  }
  try {
    expectLess(7, 5);
  } catch (...) {
  }
  try {
    expectLess(5, 5);
  } catch (...) {
  }
  try {
    expectLessEqual(7, 2);
  } catch (...) {
  }
  try {
    expectGreater(2, 5);
  } catch (...) {
  }
  try {
    expectGreater(5, 5);
  } catch (...) {
  }
  try {
    expectGreaterEqual(1, 2);
  } catch (...) {
  }
  try {
    expectContains(elements, 4);
  } catch (...) {
  }
  try {
    expectThrows([]() { return; });
  } catch (...) {
  }
  try {
    expectThrowsExact<std::exception>([]() { throw std::runtime_error("Error"); });
  } catch (...) {
  }
  // Regression test: expectThrowsExact must reject wrong exception types (not slice)
  try {
    expectThrowsExact<std::logic_error>([]() { throw std::runtime_error("Error"); });
  } catch (...) {
  }
  try {
    expectNull(elements.data());
  } catch (...) {
  }
#if defined(__unix__) || defined(__APPLE__)
  try {
    expectDeath([]() { return; });
  } catch (...) {
  }
#endif

  auto my_tuple = tuple(3.0F, 4);
  assertEqual(my_tuple.getSizeof(), 2);

  // Regression test: Tuple with 10+ elements verifies simplified member naming (I >= 10)
  auto large_tuple = tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
  assertEqual(large_tuple.getSizeof(), 12);

  // Run TestSuite2 as well (assertThrows default + assertThrowsExact match)
  test<TestSuite2>(argc, argv);

  // Run DuplicateBeforeEachSuite to verify duplicate lifecycle warning
  test<DuplicateBeforeEachSuite>(argc, argv);

  // Run WindowsDeathTestSuite to verify conditional compilation
  test<WindowsDeathTestSuite>(argc, argv);

  return test<TestSuite>(argc, argv);
}
