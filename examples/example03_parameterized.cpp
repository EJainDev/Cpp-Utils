/**
 * @example example03_parameterized.cpp
 *
 * This example demonstrates AnnoTest's parameterized testing:
 * - @cpp:struct[Parameterize] — struct-based parameter injection
 * - @cpp:func[tuple] — compile-time tuple builder
 *
 * See :doc:`/api/testing/annotations` for full documentation.
 */
#include <annotest_contract.h>

import annotest;

import std;

using namespace annotest;

/**
 * Section: Parameterize with single parameters.
 * Each tuple() call is one complete parameter set.
 */
struct ParameterizeBasic {
  [[ = Test{}, = Parameterize{tuple(2, 3), tuple(5, 7), tuple(-1, -3)} ]] void addition(int a,
                                                                                        int b) {
    assertEqual(a + b, a + b);
    assertTrue(a != 0 && b != 0);
  }

  [[ = Test{},
     = Parameterize{tuple(1.0), tuple(2.5), tuple(0.001)} ]] void floating_point(double x) {
    assertNear(x, x);
    assertTrue(x >= 0.0);
  }
};

/**
 * Section: Parameterized tests with lifecycle hooks.
 * BeforeEach/AfterEach fire around each parameter set.
 */
struct ParameterizeWithHooks {
  int set_count = 0;

  [[= BeforeEach{}]] void reset() { set_count = 0; }

  [[ = Test{}, = Parameterize{tuple(100), tuple(200), tuple(300)} ]] void counting(int val) {
    ++set_count;
    assertEqual(val, val);
    assertTrue(val % 100 == 0);
  }
};

int main(int argc, char** argv) { return test<ParameterizeBasic>(argc, argv); }
