/**
 * @example example07_full_suite.cpp
 *
 * This example demonstrates a **comprehensive test suite** that combines
 * ALL AnnoTest features in a single, realistic example:
 *
 * 1. @cpp:struct[Test] — basic test cases with named and anonymous tests
 * 2. @cpp:struct[Test<Name>] — custom test display names
 * 3. @cpp:struct[Test{.disabled}] — skipped tests
 * 4. @cpp:struct[BeforeEach] / @cpp:struct[AfterEach] — per-test lifecycle hooks
 * 5. @cpp:struct[BeforeAll] / @cpp:struct[AfterAll] — suite-level lifecycle hooks
 * 6. @cpp:struct[Parameterize] — parameterized tests with @cpp:func[tuple]
 * 7. @cpp:enum-class[OS] — @cpp:struct[RequiresOS] and @cpp:struct[DisallowOS]
 * 8. @cpp:func[assertEqual], @cpp:func[assertNotEqual], etc. — all assert functions
 * 9. @cpp:func[expectEqual], @cpp:func[expectNotEqual], etc. — all expect functions
 * 10. @cpp:func[assertThrows] / @cpp:func[expectThrows] — exception assertions
 * 11. @cpp:func[assertDeath] / @cpp:func[expectDeath] — death tests (Unix/macOS only)
 * 12. @cpp:struct[RequiresOS] with multiple platforms
 * 13. @cpp:func[assertContractViolation] — contract testing
 *
 * This is a realistic integration test that you might see in a production codebase.
 *
 * Build & run:
 *   g++ -std=c++26 -freflection -fmodules -fmodule-map=. -x c++-module example07_full_suite.cpp -o example07_full_suite
 *   ./example07_full_suite
 */
#include <annotest_contract.h>

import annotest;

import std;

using namespace annotest;

/**
 * A simple stack class under test — demonstrates real-world usage patterns.
 */
struct Stack {
  std::vector<int> data;

  void push(int value) { data.push_back(value); }

  [[nodiscard]] int pop() pre(!empty()) {
    int value = data.back();
    data.pop_back();
    return value;
  }

  [[nodiscard]] bool empty() const { return data.empty(); }

  [[nodiscard]] int size() const { return static_cast<int>(data.size()); }

  [[nodiscard]] int top() pre(!empty()) { return data.back(); }
};

/**
 * Comprehensive test suite for Stack, exercising every AnnoTest feature.
 */
struct FullSuite {
  // ─── Suite-level lifecycle ───────────────────────────────────────────
  int total_pushes = 0;
  std::vector<Stack> all_stacks;

  [[= BeforeAll{}]] void before_all() {
    total_pushes = 0;
    std::cout << "[BeforeAll] Initializing test fixture\n";
  }

  [[= AfterAll{}]] void after_all() {
    std::cout << "[AfterAll] Total pushes across all tests: " << total_pushes << "\n";
  }

  // ─── Per-test lifecycle ──────────────────────────────────────────────
  Stack fixture;

  [[= BeforeEach{}]] void setup() {
    fixture = Stack{};
    fixture.push(1);
    fixture.push(2);
    fixture.push(3);
  }

  [[= AfterEach{}]] void teardown() {
    all_stacks.push_back(std::move(fixture));
  }

  // ─── Basic tests ─────────────────────────────────────────────────────
  [[= Test{"stack is not empty after push"}]]
  void basic_not_empty() {
    assertFalse(fixture.empty());
    assertEqual(3, fixture.size());
  }

  [[= Test{"named test with custom display name"}]]
  void top_element() {
    assertEqual(3, fixture.top());
  }

  [[= Test{.disabled = true}]]
  void skipped_test() {
    assertEqual(0, 1);  // This never runs
  }

  // ─── Assert functions ────────────────────────────────────────────────
  [[= Test{"all assert functions"}]]
  void all_asserts() {
    // Equality
    assertEqual(3, 3);
    assertNotEqual(3, 4);

    // Boolean
    assertTrue(true);
    assertFalse(false);

    // Floating point
    assertNear(3.14159, 3.14158, 1e-3);

    // Comparison
    assertLess(1, 5);
    assertLessEqual(5, 5);
    assertGreater(10, 1);
    assertGreaterEqual(10, 10);

    // Container
    std::vector<int> nums{1, 2, 3, 4, 5};
    assertContains(nums, 3);

    // Null
    assertNull(nullptr);
  }

  // ─── Expect functions ────────────────────────────────────────────────
  [[= Test{"all expect functions"}]]
  void all_expects() {
    // These use Abort instead of Error — test continues on failure
    expectEqual(3, 3);
    expectNotEqual(3, 4);
    expectTrue(true);
    expectFalse(false);
    expectNear(3.14159, 3.14158, 1e-3);
    expectLess(1, 5);
    expectLessEqual(5, 5);
    expectGreater(10, 1);
    expectGreaterEqual(10, 10);

    std::vector<int> nums{1, 2, 3, 4, 5};
    expectContains(nums, 3);
    expectNull(nullptr);
  }

  // ─── Exception assertions ────────────────────────────────────────────
  [[= Test{"throws specific exception"}]]
  void throws_runtime_error() {
    assertThrows<std::runtime_error>([]() { throw std::runtime_error("test"); });
  }

  [[= Test{"throws exact exception"}]]
  void throws_exact() {
    assertThrowsExact<std::runtime_error>([]() { throw std::runtime_error("exact"); });
  }

  [[= Test{"expect exception"}]]
  void expect_throws() {
    expectThrows<std::runtime_error>([]() { throw std::runtime_error("test"); });
    expectThrowsExact<std::runtime_error>([]() { throw std::runtime_error("exact"); });
  }

  // ─── Parameterized tests ─────────────────────────────────────────────
  [[= Test{"parameterized push/pop"}, = Parameterize{
       tuple(1, 1), tuple(100, 100), tuple(-50, -50)
  }]]
  void push_pop(int push_val, int expect_pop) {
    Stack s;
    s.push(push_val);
    assertEqual(expect_pop, s.pop());
    assertTrue(s.empty());
  }

  [[= Test{"parameterized size"}, = Parameterize{
       tuple(0), tuple(1), tuple(10)
  }]]
  void push_n(int n) {
    Stack s;
    for (int i = 0; i < n; ++i) {
      s.push(i);
    }
    assertEqual(n, s.size());
  }

  // ─── OS annotations ──────────────────────────────────────────────────
  [[= Test{"runs only on Linux"}, = RequiresOS{OS::Linux}]]
  void linux_test() {
    assertEqual(1, 1);
  }

  [[= Test{"runs only on Linux or Mac"}, = RequiresOS{OS::Linux, OS::Mac}]]
  void linux_or_mac() {
    assertEqual(1, 1);
  }

  [[= Test{"skips on Windows"}, = DisallowOS{OS::Windows}]]
  void not_windows() {
    assertTrue(true);
  }

  // ─── Death tests (Unix/macOS only) ───────────────────────────────────
#if defined(__unix__) || defined(__APPLE__)
  [[= Test{"child process dies"}]]
  void death_by_exception() {
    assertDeath([]() { throw std::runtime_error("die!"); });
  }

  [[= Test{"expect child dies"}]]
  void expect_death() {
    expectDeath([]() { std::abort(); });
  }
#endif

  // ─── Contract testing ────────────────────────────────────────────────
  [[= Test{"contract violation on bad input"}]]
  void contract_violation() {
    assertContractViolation([]() {
      int x = safe_divide(10, 0);
      (void)x;  // Suppress unused warning
    });
  }

  [[= Test{"no contract violation on valid input"}]]
  void contract_ok() {
    assertNoContractViolation([]() {
      int x = safe_divide(10, 2);
      assertEqual(5, x);
    });
  }
};

int main(int argc, char** argv) {
  return test<FullSuite>(argc, argv);
}
