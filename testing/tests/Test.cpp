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
  [[= Test{}]] void AssertEqual() { assertEqual(5, 5); }

  [[= Test{}]] void AssertNotEqual() { assertNotEqual(5, 3); }

  [[= Test{}]] void AssertTrue() { assertTrue(true); }

  [[= Test{}]] void AssertFalse() { assertFalse(false); }

  // ===== Comparison Assert Tests =====
  [[= Test{}]] void AssertNear() { assertNear(1.0, 1.001, 0.01); }

  [[= Test{}]] void AssertLess() { assertLess(1, 5); }

  [[= Test{}]] void AssertLessEqual() { assertLessEqual(5, 5); }

  [[= Test{}]] void AssertGreater() { assertGreater(5, 1); }

  [[= Test{}]] void AssertGreaterEqual() { assertGreaterEqual(5, 5); }

  // ===== Container Assert Tests =====
  [[= Test{}]] void AssertContains() {
    std::vector<int> v{1, 2, 3, 4, 5};
    assertContains(v, 3);
  }

  // ===== Exception Assert Tests =====
  [[= Test{}]] void AssertThrows() {
    assertThrows<std::runtime_error>([]() { throw std::runtime_error("test"); });
  }

  [[= Test{}]] void AssertThrowsMessage() {
    assertThrowsMessage<std::runtime_error>([]() { throw std::runtime_error("test error"); },
                                            "error");
  }

  [[= Test{}]] void AssertThrowsExact() {
    assertThrowsExact<CustomException>([]() { throw CustomException(); });
  }

  // ===== Death Assert Tests =====
  [[= Test{}]] void AssertDeathOnAbort() {
    assertDeath([]() { std::abort(); });
  }

  [[= Test{}]] void AssertDeathOnAssertEqualFailure() {
    assertDeath([]() { assertEqual(1, 2); });
  }

  [[= Test{}]] void AssertDeathOnAssertNotEqualFailure() {
    assertDeath([]() { assertNotEqual(3, 3); });
  }

  [[= Test{}]] void AssertDeathOnAssertTrueFailure() {
    assertDeath([]() { assertTrue(false); });
  }

  [[= Test{}]] void AssertDeathOnAssertFalseFailure() {
    assertDeath([]() { assertFalse(true); });
  }

  [[= Test{}]] void AssertDeathOnAssertNearFailure() {
    assertDeath([]() { assertNear(1.0, 2.0, 0.1); });
  }

  [[= Test{}]] void AssertDeathOnAssertLessFailure() {
    assertDeath([]() { assertLess(5, 1); });
  }

  [[= Test{}]] void AssertDeathOnAssertLessEqualFailure() {
    assertDeath([]() { assertLessEqual(5, 4); });
  }

  [[= Test{}]] void AssertDeathOnAssertGreaterFailure() {
    assertDeath([]() { assertGreater(1, 5); });
  }

  [[= Test{}]] void AssertDeathOnAssertGreaterEqualFailure() {
    assertDeath([]() { assertGreaterEqual(4, 5); });
  }

  [[= Test{}]] void AssertDeathOnAssertContainsFailure() {
    assertDeath([]() {
      std::vector<int> values{1, 2, 3};
      assertContains(values, 4);
    });
  }

  [[= Test{}]] void AssertDeathOnAssertThrowsFailure() {
    assertDeath([]() { assertThrows<std::runtime_error>([]() {}); });
  }

  [[= Test{}]] void AssertDeathOnAssertThrowsMessageFailure() {
    assertDeath([]() {
      assertThrowsMessage<std::runtime_error>([]() { throw std::runtime_error("xyz"); },
                                              "expected");
    });
  }

  [[= Test{}]] void AssertDeathOnAssertThrowsExactFailure() {
    assertDeath(
        []() { assertThrowsExact<CustomException>([]() { throw DerivedCustomException(); }); });
  }

  // ===== Basic Expect Tests =====
  [[= Test{}]] void ExpectEqual() { expectEqual(5, 5); }

  [[= Test{}]] void ExpectNotEqual() { expectNotEqual(5, 3); }

  [[= Test{}]] void ExpectTrue() { expectTrue(true); }

  [[= Test{}]] void ExpectFalse() { expectFalse(false); }

  // ===== Comparison Expect Tests =====
  [[= Test{}]] void ExpectNear() { expectNear(1.0, 1.001, 0.01); }

  [[= Test{}]] void ExpectLess() { expectLess(1, 5); }

  [[= Test{}]] void ExpectLessEqual() { expectLessEqual(5, 5); }

  [[= Test{}]] void ExpectGreater() { expectGreater(5, 1); }

  [[= Test{}]] void ExpectGreaterEqual() { expectGreaterEqual(5, 5); }

  // ===== Container Expect Tests =====
  [[= Test{}]] void ExpectContains() {
    std::vector<int> v{1, 2, 3, 4, 5};
    expectContains(v, 3);
  }

  // ===== Exception Expect Tests =====
  [[= Test{}]] void ExpectThrows() {
    expectThrows<std::runtime_error>([]() { throw std::runtime_error("test"); });
  }

  [[= Test{}]] void ExpectThrowsMessage() {
    expectThrowsMessage<std::runtime_error>([]() { throw std::runtime_error("test error"); },
                                            "error");
  }

  [[= Test{}]] void ExpectThrowsExact() {
    expectThrowsExact<CustomException>([]() { throw CustomException(); });
  }

  // ===== Death Expect Tests =====
  [[= Test{}]] void ExpectDeathOnAbort() {
    expectDeath([]() { std::abort(); });
  }

  [[= Test{}]] void ExpectDeathOnExpectEqualFailure() {
    expectDeath([]() { expectEqual(1, 2); });
  }

  [[= Test{}]] void ExpectDeathOnExpectContainsFailure() {
    expectDeath([]() {
      std::vector<int> values{1, 2, 3};
      expectContains(values, 4);
    });
  }

  [[= Test{}]] void ExpectDeathOnExpectThrowsFailure() {
    expectDeath([]() { expectThrows<std::runtime_error>([]() {}); });
  }

  // ===== Failure Tests =====
  // These tests verify that assertions fail correctly

  [[= Test{}]][[= Disabled{}]] void AssertEqualFails() {
    // This should fail: assertEqual(5, 3);
  }

  [[= Test{}]][[= Disabled{}]] void AssertNotEqualFails() {
    // This should fail: assertNotEqual(5, 5);
  }

  [[= Test{}]][[= Disabled{}]] void AssertTrueFails() {
    // This should fail: assertTrue(false);
  }

  [[= Test{}]][[= Disabled{}]] void AssertFalseFails() {
    // This should fail: assertFalse(true);
  }

  [[= Test{}]][[= Disabled{}]] void AssertNearFails() {
    // This should fail: assertNear(1.0, 2.0, 0.5);
  }

  [[= Test{}]][[= Disabled{}]] void AssertLessFails() {
    // This should fail: assertLess(5, 1);
  }

  [[= Test{}]][[= Disabled{}]] void AssertContainsFails() {
    // This should fail: std::vector<int> v{1, 2, 3}; assertContains(v, 5);
  }

  [[= Test{}]][[= Disabled{}]] void AssertThrowsFails() {
    // This should fail: assertThrows<std::runtime_error>([]() {});
  }

  [[= Test{}]][[= Disabled{}]] void AssertThrowsExactFails() {
    // This should fail: assertThrowsExact<CustomException>([]() { throw
    // DerivedCustomException(); });
  }

  // ===== Edge Cases =====
  [[= Test{}]] void AssertNearZero() { assertNear(0.0, 0.0, 0.0); }

  [[= Test{}]] void AssertNearNegative() { assertNear(-1.5, -1.501, 0.01); }

  [[= Test{}]] void AssertContainsString() {
    std::vector<std::string> v{"hello", "world"};
    assertContains(v, std::string("hello"));
  }

  [[= Test{}]] void AssertThrowsWithMessage() {
    assertThrowsMessage<CustomException>([]() { throw CustomException(); }, "Custom");
  }
};

int main(int argc, char** argv) { return test<TestSuite>(argc, argv); }