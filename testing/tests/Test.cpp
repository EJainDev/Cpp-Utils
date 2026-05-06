import cpputils.testing;

import std;

using namespace cpputils::testing;

struct TestSuite {
  [[= BeforeAll{}]] void beforeAll() { std::cout << "Running before all\n"; }
  [[= BeforeEach{}]] void beforeEach() { std::cout << "Running before each\n"; }
  [[ = Test{}, = Disabled{} ]] void disabledTest() {}
  [[= Test{}]] void test() {}
  [[= Parameterize{tuple(5), tuple(50)}]] void parameterizedTest(int val) {
    assertTrue(val == 5 || val == 50);
  }
  [[= Test{}]][[= RequiresOS{OS::Windows}]] void windows() {}
  [[= Test{}]][[= RequiresOS{OS::Linux}]] void testLinux() {}
  [[= Test{}]][[= RequiresOS{OS::Mac}]] void mac() {}
  [[= Test{}]][[= RequiresOS{OS::Unknown}]] void other() {}
  [[= Test{}]][[= RequiresOS{OS::Windows, OS::Linux}]] void windowsLinux() {}
  [[= Test{}]][[= DisallowOS{OS::Windows}]] void nonWindows() {}
  [[= Test{}]][[= DisallowOS{OS::Linux}]] void nonLinux() {}
  [[= Test{}]][[= DisallowOS{OS::Mac}]] void nonMac() {}
  [[= Test{}]][[= DisallowOS{OS::Unknown}]] void nonOther() {}
  [[= Test{}]][[= DisallowOS{OS::Windows, OS::Linux}]] void nonWindowsLinux() {}

  [[= AfterEach{}]] void afterEach() { std::cout << "Running after each\n"; }
  [[= AfterAll{}]] void afterAll() { std::cout << "Running after all\n"; }
};

int main(int argc, char** argv) {
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
  try {
    assertThrows([]() { return; });
  } catch (...) {
  }
  try {
    assertThrowsExact<std::exception>([]() { throw std::runtime_error("Error"); });
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

  return test<TestSuite>(argc, argv);
}