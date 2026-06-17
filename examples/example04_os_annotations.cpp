/**
 * @example example04_os_annotations.cpp
 *
 * This example demonstrates OS annotations:
 * - @cpp:struct[RequiresOS] — run on specific platforms
 * - @cpp:struct[DisallowOS] — skip specific platforms
 *
 * See :doc:`/api/testing/annotations` for full documentation.
 */
#include <annotest_contract.h>

import annotest;

import std;

using namespace annotest;

/**
 * Section: Single OS requirement.
 * Each test runs only on its listed platform.
 */
struct SingleOSRequirement {
  [[= Test{}, = RequiresOS{OS::Linux}]]
  void linux_only() {
    assertEqual(1, 1);
    std::cout << "Running on Linux\n";
  }

  [[= Test{}, = RequiresOS{OS::Mac}]]
  void mac_only() {
    assertEqual(1, 1);
    std::cout << "Running on macOS\n";
  }

  [[= Test{}, = RequiresOS{OS::Windows}]]
  void windows_only() {
    assertEqual(1, 1);
    std::cout << "Running on Windows\n";
  }
};

/**
 * Section: Multiple OS requirement (OR logic).
 * Runs on Linux OR Windows.
 */
struct MultiOSRequirement {
  [[= Test{}, = RequiresOS{OS::Linux, OS::Windows}]]
  void linux_or_windows() {
    assertEqual(2, 1 + 1);
  }
};

/**
 * Section: DisallowOS — inverse of RequiresOS.
 * Skipped on listed platforms, runs on all others.
 */
struct DisallowedOS {
  [[= Test{}, = DisallowOS{OS::Windows}]]
  void not_windows() {
    assertEqual(1, 1);
  }

  [[= Test{}, = DisallowOS{OS::Windows, OS::Mac}]]
  void linux_only_by_exclusion() {
    assertEqual(1, 1);
  }
};

/**
 * Section: Mixing OS annotations with named tests.
 */
struct MixedFeatures {
  [[= Test<"cross-platform math">{}, = RequiresOS{OS::Linux, OS::Mac, OS::Windows}]]
  void always_runs() {
    assertEqual(5, 2 + 3);
  }

  [[= Test{}, = RequiresOS{OS::Linux}]]
  void linux_math() {
    assertNear(3.14159, 3.14159);
  }

  [[= Test{}, = DisallowOS{OS::Unknown}]]
  void known_platform() {
    assertTrue(true);
  }
};

int main(int argc, char** argv) {
  return test<SingleOSRequirement>(argc, argv);
}
