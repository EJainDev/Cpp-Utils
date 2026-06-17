/**
 * @example example01_basic.cpp
 *
 * This example demonstrates the most basic AnnoTest usage:
 * - Writing a minimal test suite with @cpp:struct[Test] annotations
 * - Calling @cpp:func[Test::test]() to run the suite
 *
 * Build & run:
 *   g++ -std=c++26 -freflection -fmodules -fmodule-map=. -x c++-module example01_basic.cpp -o
 * example01_basic
 *   ./example01_basic
 */
import annotest;
import annotest.contracts;

import std;

using namespace annotest;

/**
 * A test suite is just a plain class (or struct).
 * Each function annotated with @cpp:struct[Test] becomes a test case.
 */
struct BasicTests {
  /**
   * The simplest possible test: no arguments, no setup.
   * The name is derived from the function name.
   */
  [[= Test{}]] void addition() { assertEqual(4, 2 + 2); }

  /**
   * Tests can verify any condition using the assert functions.
   */
  [[= Test{}]] void boolean_logic() {
    assertTrue(true);
    assertFalse(false);
    assertTrue(1 == 1);
    assertFalse(5 < 3);
  }

  /**
   * Test with a custom display name via the template parameter.
   */
  [[= Test<"subtraction works">{}]] void math() { assertEqual(10, 15 - 5); }
};

int main(int argc, char** argv) {
  /**
   * The @cpp:func[Test::test]() function runs all tests in the suite.
   * It takes the class type as a template parameter.
   *
   * By default it returns 0 if all pass, 1 if any fail.
   */
  return test<BasicTests>(argc, argv);
}
