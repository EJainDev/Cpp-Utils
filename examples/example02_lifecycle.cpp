/**
 * @example example02_lifecycle.cpp
 *
 * This example demonstrates the test lifecycle annotations:
 * - @cpp:struct[BeforeAll] — runs once before all tests
 * - @cpp:struct[BeforeEach] — runs before each individual test
 * - @cpp:struct[AfterEach] — runs after each individual test
 * - @cpp:struct[AfterAll] — runs once after all tests complete
 *
 * These are useful for:
 * - Setting up shared state (BeforeAll / AfterAll)
 * - Resetting per-test fixtures (BeforeEach / AfterEach)
 *
 * The execution order is:
 * 1. BeforeAll()
 * 2. (BeforeEach(), test(), AfterEach()) × N tests
 * 3. AfterAll()
 */
import annotest;

import std;

using namespace annotest;

/**
 * A counter shared across the entire test suite.
 * Demonstrates that BeforeAll/AfterAll run only once.
 */
int call_count = 0;

struct LifecycleTests {
    /**
     * BeforeAll: called once, before the first test.
     * If this throws, the entire suite is aborted.
     */
    [[= BeforeAll{}]] void setUp() {
        call_count = 0;
        std::cout << "[BeforeAll] Setup shared resources\n";
    }

    /**
     * BeforeEach: called before every test.
     * Resets the counter for each test.
     */
    [[= BeforeEach{}]] void resetCounters() {
        call_count = 0;
    }

    /**
     * AfterEach: called after every test.
     * If this throws, only a warning is printed (suite continues).
     */
    [[= AfterEach{}]] void reportCounts() {
        std::cout << "[AfterEach] counter = " << call_count << "\n";
    }

    /**
     * AfterAll: called once, after the last test.
     * If this throws, only a warning is printed.
     */
    [[= AfterAll{}]] void tearDown() {
        std::cout << "[AfterAll] Cleaning up shared resources\n";
    }

    /**
     * Demonstrates BeforeEach runs before this test.
     */
    [[= Test{}]] void test_one() {
        ++call_count;
        assertEqual(1, call_count);
    }

    /**
     * Demonstrates BeforeEach runs before every test, including this one.
     */
    [[= Test{}]] void test_two() {
        ++call_count;
        ++call_count;
        assertEqual(2, call_count);
    }

    /**
     * Demonstrates AfterEach runs after this test.
     */
    [[= Test{}]] void test_three() {
        ++call_count;
        ++call_count;
        ++call_count;
        assertEqual(3, call_count);
    }
};

int main(int argc, char** argv) {
    return test<LifecycleTests>(argc, argv);
}
