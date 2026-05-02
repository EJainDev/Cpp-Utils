import cpputils.testing;

import std;

using namespace cpputils::testing;

// Custom exception types for testing
class CustomException : public std::exception {
 public:
  const char* what() const noexcept override { return "Custom exception"; }
};

class DerivedCustomException : public CustomException {
 public:
  const char* what() const noexcept override { return "Derived custom exception"; }
};

struct TestSuite {
  // ===== Basic Assert Tests =====
  [[= Test{}]] void testAssertEqual() { assertEqual(5, 5); }

  [[= Test{}]] void testAssertNotEqual() { assertNotEqual(5, 3); }

  [[= Test{}]] void testAssertTrue() { assertTrue(true); }

  [[= Test{}]] void testAssertFalse() { assertFalse(false); }

  // ===== Comparison Assert Tests =====
  [[= Test{}]] void testAssertNear() { assertNear(1.0, 1.001, 0.01); }

  [[= Test{}]] void testAssertLess() { assertLess(1, 5); }

  [[= Test{}]] void testAssertLessEqual() { assertLessEqual(5, 5); }

  [[= Test{}]] void testAssertGreater() { assertGreater(5, 1); }

  [[= Test{}]] void testAssertGreaterEqual() { assertGreaterEqual(5, 5); }

  // ===== Container Assert Tests =====
  [[= Test{}]] void testAssertContains() {
    std::vector<int> v{1, 2, 3, 4, 5};
    assertContains(v, 3);
  }

  // ===== Exception Assert Tests =====
  [[= Test{}]] void testAssertThrows() {
    assertThrows<std::runtime_error>([]() { throw std::runtime_error("test"); });
  }

  [[= Test{}]] void testAssertThrowsMessage() {
    assertThrowsMessage<std::runtime_error>([]() { throw std::runtime_error("test error"); },
                                            "error");
  }

  [[= Test{}]] void testAssertThrowsExact() {
    assertThrowsExact<CustomException>([]() { throw CustomException(); });
  }

  // ===== Basic Expect Tests =====
  [[= Test{}]] void testExpectEqual() { expectEqual(5, 5); }

  [[= Test{}]] void testExpectNotEqual() { expectNotEqual(5, 3); }

  [[= Test{}]] void testExpectTrue() { expectTrue(true); }

  [[= Test{}]] void testExpectFalse() { expectFalse(false); }

  // ===== Comparison Expect Tests =====
  [[= Test{}]] void testExpectNear() { expectNear(1.0, 1.001, 0.01); }

  [[= Test{}]] void testExpectLess() { expectLess(1, 5); }

  [[= Test{}]] void testExpectLessEqual() { expectLessEqual(5, 5); }

  [[= Test{}]] void testExpectGreater() { expectGreater(5, 1); }

  [[= Test{}]] void testExpectGreaterEqual() { expectGreaterEqual(5, 5); }

  // ===== Container Expect Tests =====
  [[= Test{}]] void testExpectContains() {
    std::vector<int> v{1, 2, 3, 4, 5};
    expectContains(v, 3);
  }

  // ===== Exception Expect Tests =====
  [[= Test{}]] void testExpectThrows() {
    expectThrows<std::runtime_error>([]() { throw std::runtime_error("test"); });
  }

  [[= Test{}]] void testExpectThrowsMessage() {
    expectThrowsMessage<std::runtime_error>([]() { throw std::runtime_error("test error"); },
                                            "error");
  }

  [[= Test{}]] void testExpectThrowsExact() {
    expectThrowsExact<CustomException>([]() { throw CustomException(); });
  }

  // ===== Failure Tests =====
  // These tests verify that assertions fail correctly

  [[= Test{}]][[= Disabled{}]] void testAssertEqualFails() {
    // This should fail: assertEqual(5, 3);
  }

  [[= Test{}]][[= Disabled{}]] void testAssertNotEqualFails() {
    // This should fail: assertNotEqual(5, 5);
  }

  [[= Test{}]][[= Disabled{}]] void testAssertTrueFails() {
    // This should fail: assertTrue(false);
  }

  [[= Test{}]][[= Disabled{}]] void testAssertFalseFails() {
    // This should fail: assertFalse(true);
  }

  [[= Test{}]][[= Disabled{}]] void testAssertNearFails() {
    // This should fail: assertNear(1.0, 2.0, 0.5);
  }

  [[= Test{}]][[= Disabled{}]] void testAssertLessFails() {
    // This should fail: assertLess(5, 1);
  }

  [[= Test{}]][[= Disabled{}]] void testAssertContainsFails() {
    // This should fail: std::vector<int> v{1, 2, 3}; assertContains(v, 5);
  }

  [[= Test{}]][[= Disabled{}]] void testAssertThrowsFails() {
    // This should fail: assertThrows<std::runtime_error>([]() {});
  }

  [[= Test{}]][[= Disabled{}]] void testAssertThrowsExactFails() {
    // This should fail: assertThrowsExact<CustomException>([]() { throw
    // DerivedCustomException(); });
  }

  // ===== Edge Cases =====
  [[= Test{}]] void testAssertNearZero() { assertNear(0.0, 0.0, 0.0); }

  [[= Test{}]] void testAssertNearNegative() { assertNear(-1.5, -1.501, 0.01); }

  [[= Test{}]] void testAssertContainsString() {
    std::vector<std::string> v{"hello", "world"};
    assertContains(v, std::string("hello"));
  }

  [[= Test{}]] void testAssertThrowsWithMessage() {
    assertThrowsMessage<CustomException>([]() { throw CustomException(); }, "Custom");
  }
};

int main(int argc, char** argv) { return test<TestSuite>(argc, argv); }