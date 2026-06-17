Full Suite Example
==================

Raw Source
----------

.. literalinclude:: ../../../examples/example07_full_suite.cpp
   :language: cpp

Breaking It Down
----------------

Section 1 — **The class under test**

.. code-block:: cpp

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

A minimal stack with C++26 ``pre()`` contracts on ``pop()`` and ``top()``.

Section 2 — **Suite-level lifecycle hooks**

.. code-block:: cpp

   int total_pushes = 0;
   std::vector<Stack> all_stacks;

   [[= BeforeAll{}]] void before_all() {
     total_pushes = 0;
     std::cout << "[BeforeAll] Initializing test fixture\n";
   }

   [[= AfterAll{}]] void after_all() {
     std::cout << "[AfterAll] Total pushes across all tests: " << total_pushes << "\n";
   }

Shared state declared as struct members. ``BeforeAll``/``AfterAll`` run once for the entire suite.

Section 3 — **Per-test lifecycle hooks**

.. code-block:: cpp

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

BeforeEach resets a Stack fixture with 3 elements. AfterEach saves it for later inspection.

Section 4 — **Basic tests with named and disabled cases**

.. code-block:: cpp

   [[= Test{"stack is not empty after push"}]]
   void basic_not_empty() {
     assertFalse(fixture.empty());
     assertEqual(3, fixture.size());
   }

   [[= Test<"named test with custom display name">{}]]
   void top_element() {
     assertEqual(3, fixture.top());
   }

   [[= Test{.disabled = true}]]
   void skipped_test() {
     assertEqual(0, 1);  // This never runs
   }

Custom names via template, ``.disabled`` flag for skipped tests.

Section 5 — **All assert functions in one test**

.. code-block:: cpp

   [[= Test{"all assert functions"}]]
   void all_asserts() {
     assertEqual(3, 3);
     assertNotEqual(3, 4);
     assertTrue(true);
     assertFalse(false);
     assertNear(3.14159, 3.14158, 1e-3);
     assertLess(1, 5);
     assertLessEqual(5, 5);
     assertGreater(10, 1);
     assertGreaterEqual(10, 10);
     std::vector<int> nums{1, 2, 3, 4, 5};
     assertContains(nums, 3);
     assertNull(nullptr);
   }

Demonstrates every assert function: equality, boolean, floating point, comparison, container, and null.

Section 6 — **All expect functions in one test**

.. code-block:: cpp

   [[= Test{"all expect functions"}]]
   void all_expects() {
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

Same as above but uses ``expect*`` functions (throw :cpp:class:`Abort`).

Section 7 — **Exception assertions**

.. code-block:: cpp

   [[= Test{"throws specific exception"}]]
   void throws_runtime_error() {
     assertThrows<std::runtime_error>([]() { throw std::runtime_error("test"); });
   }

   [[= Test{"throws exact exception"}]]
   void throws_exact() {
     assertThrowsExact<std::runtime_error>([]() { throw std::runtime_error("exact"); });
   }

``assertThrows<E>`` catches ``E`` and derived types. ``assertThrowsExact<E>`` requires exact match via ``typeid``.

Section 8 — **Parameterized tests**

.. code-block:: cpp

   [[= Test{"parameterized push/pop"}, = Parameterize{
        tuple(1, 1), tuple(100, 100), tuple(-50, -50)
   }]]
   void push_pop(int push_val, int expect_pop) {
     Stack s;
     s.push(push_val);
     assertEqual(expect_pop, s.pop());
     assertTrue(s.empty());
   }

Two-parameter tuple sets injected in order. Three sub-runs.

Section 9 — **OS annotations**

.. code-block:: cpp

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

Single and multi-platform OS annotations combined with named tests.

Section 10 — **Death tests**

.. code-block:: cpp

   [[= Test{"child process dies"}]]
   void death_by_exception() {
     assertDeath([]() { throw std::runtime_error("die!"); });
   }

   [[= Test{"expect child dies"}]]
   void expect_death() {
     expectDeath([]() { std::abort(); });
   }

Unix/macOS only. assertDeath uses Error; expectDeath uses Abort.

Section 11 — **Contract testing**

.. code-block:: cpp

   [[= Test{"contract violation on bad input"}]]
   void contract_violation() {
     assertContractViolation([]() {
       int x = safe_divide(10, 0);
       (void)x;
     });
   }

   [[= Test{"no contract violation on valid input"}]]
   void contract_ok() {
     assertNoContractViolation([]() {
       int x = safe_divide(10, 2);
       assertEqual(5, x);
     });
   }

Verifies that C++26 contract pre/post conditions fire (or don't) as expected.
