Lifecycle Hooks
===============

Raw Source
----------

.. literalinclude:: ../../../examples/example02_lifecycle.cpp
   :language: cpp

Breaking It Down
----------------

Section 1 — **BeforeAll**

.. code-block:: cpp

   [[= BeforeAll{}]] void setUp() {
     call_count = 0;
     std::cout << "[BeforeAll] Setup shared resources\n";
   }

Runs **once** before the first test. If it throws, the entire suite aborts.

Section 2 — **BeforeEach**

.. code-block:: cpp

   [[= BeforeEach{}]] void resetCounters() { call_count = 0; }

Runs before **every** test. Resets per-test fixture state.

Section 3 — **AfterEach**

.. code-block:: cpp

   [[= AfterEach{}]] void reportCounts() {
     std::cout << "[AfterEach] counter = " << call_count << "\n";
   }

Runs after **every** test. If it throws, only a warning is printed (suite continues).

Section 4 — **AfterAll**

.. code-block:: cpp

   [[= AfterAll{}]] void tearDown() {
     std::cout << "[AfterAll] Cleaning up shared resources\n";
   }

Runs **once** after the last test. If it throws, only a warning is printed.

Section 5 — **Tests use the hook state**

.. code-block:: cpp

   [[= Test{}]] void test_one() {
     ++call_count;
     assertEqual(1, call_count);
   }

Each test runs with a fresh reset by ``BeforeEach``, so the counter is always 1 at this point.

Execution order:

.. code-block:: text

   BeforeAll()
     BeforeEach() → test_one() → AfterEach()
     BeforeEach() → test_two() → AfterEach()
     BeforeEach() → test_three() → AfterEach()
   AfterAll()
