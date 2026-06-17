/**
 * @example example05_death_tests.cpp
 *
 * This example demonstrates death tests:
 * - @cpp:func[assertDeath] — fail if child survives
 * - @cpp:func[expectDeath] — abort if child survives
 *
 * See :doc:`/api/testing/death` for full documentation.
 */
#include <annotest_contract.h>

import annotest;

import std;

using namespace annotest;

/**
 * Section: assertDeath with exception-triggered death.
 * Child throws → dies → test passes.
 */
struct DeathTestBasic {
#if defined(__unix__) || defined(__APPLE__)
  /**
   * Death via uncaught exception.
   * assertDeath succeeds because child exits non-zero.
   */
  [[= Test{"death by exception"}]]
  void child_crashes() {
    assertDeath([]() { throw std::runtime_error("crash!"); });
  }

  /**
   * Section: assertDeath with std::abort().
   */
  [[= Test{"death by abort"}]]
  void child_aborts() {
    assertDeath([]() { std::abort(); });
  }
#endif
};

/**
 * Section: expectDeath — same model, uses Abort instead of Error.
 */
struct ExpectDeathDemo {
#if defined(__unix__) || defined(__APPLE__)
  [[= Test{"expect death by crash"}]]
  void expect_child_crashes() {
    expectDeath([]() { throw std::runtime_error("oops"); });
  }
#endif
};

/**
 * Section: Surviving process — assertDeath failure case.
 * Child exits 0 → test fails.
 */
struct SurvivingProcess {
#if defined(__unix__) || defined(__APPLE__)
  [[= Test{"child survives — should fail"}]]
  void child_survives() {
    assertDeath([]() { return; });  // Expected to fail: child exits 0
  }
#endif
};

int main(int argc, char** argv) {
#if defined(__unix__) || defined(__APPLE__)
  return test<DeathTestBasic>(argc, argv);
#else
  std::cout << "Death tests are not available on this platform.\n";
  return 0;
#endif
}
