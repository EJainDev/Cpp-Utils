.. _assertions_and_expectations:

Assertions and Expectations
===========================

.. cpp:namespace:: annotest

Assertions and expectations verify conditions during tests. They differ in how they handle failures:

.. list-table::
   :widths: 20 25 30 25
   :stub-columns: 1
   :header-rows: 1

   * - Category
     - Prefix
     - On Failure
     - Continues?
   * - **Assert**
     - ``assert*``
     - Throws :cpp:class:`Error`
     - Marks test **failed**, continues suite
   * - **Expect**
     - ``expect*``
     - Throws :cpp:class:`Abort`
     - **Aborts** current test, continues suite

**Use assertions** for conditions that, if false, mean the test result is invalid.
**Use expectations** for conditions that, if false, mean subsequent test code is meaningless, but the test result is still valid (e.g., setup failures).

Assertions
----------

Assertions verify that a condition holds. If an assertion fails, an :cpp:class:`Error` exception is thrown, the test case is marked as failed, and the library continues with the next test.

.. cpp:function:: void assertEqual(auto expected, auto actual)

    Asserts that ``expected`` and ``actual`` are equal (checked via ``!=``).

    .. code-block:: cpp

        assertEqual(4, 2 + 2);
        assertEqual(std::vector{1, 2}, std::vector{1, 2});

.. cpp:function:: void assertNotEqual(auto expected, auto actual)

    Asserts that ``expected`` and ``actual`` are not equal (checked via ``==``).

    .. code-block:: cpp

        assertNotEqual(3, 4);

.. cpp:function:: void assertTrue(auto value)

    Asserts that ``value`` is truthy.

    .. code-block:: cpp

        assertTrue(true);
        assertTrue(1 == 1);
        assertTrue(5 > 3);

.. cpp:function:: void assertFalse(auto value)

    Asserts that ``value`` is falsy.

    .. code-block:: cpp

        assertFalse(false);
        assertFalse(1 == 0);

.. cpp:function:: void assertNear(auto expected, auto actual, double tol = 0.001)

    Asserts that ``abs(expected - actual) <= tol``. Useful for floating-point comparisons.

    .. code-block:: cpp

        assertNear(3.14159, 3.14158, 1e-3);
        assertNear(5.0, 5.00001, 1e-4);

.. cpp:function:: void assertLess(auto a, auto b)

    Asserts that ``a < b``.

    .. code-block:: cpp

        assertLess(2, 5);

.. cpp:function:: void assertLessEqual(auto a, auto b)

    Asserts that ``a <= b``.

    .. code-block:: cpp

        assertLessEqual(5, 5);
        assertLessEqual(4, 5);

.. cpp:function:: void assertGreater(auto a, auto b)

    Asserts that ``a > b``.

    .. code-block:: cpp

        assertGreater(10, 1);

.. cpp:function:: void assertGreaterEqual(auto a, auto b)

    Asserts that ``a >= b``.

    .. code-block:: cpp

        assertGreaterEqual(5, 5);
        assertGreaterEqual(10, 5);

.. cpp:function:: void assertContains(auto const& container, auto const& value)

    Asserts that ``value`` exists in ``container`` (uses ``std::find``).

    .. code-block:: cpp

        std::vector<int> nums{1, 2, 3, 4, 5};
        assertContains(nums, 3);

.. cpp:function:: template <typename E = std::exception> void assertThrows(auto func)

    Asserts that ``func`` throws an exception of type ``E`` (or a derived type).

    .. code-block:: cpp

        assertThrows<std::runtime_error>([]() { throw std::runtime_error("error"); });
        assertThrows([]() { throw std::invalid_argument("bad"); });

.. cpp:function:: template <typename E> void assertThrowsExact(auto func)

    Asserts that ``func`` throws **exactly** type ``E`` (checked with ``typeid``).

    .. code-block:: cpp

        assertThrowsExact<std::runtime_error>([]() { throw std::runtime_error("exact"); });

.. cpp:function:: void assertNull(auto ptr)

    Asserts that ``ptr`` equals ``nullptr``.

    .. code-block:: cpp

        assertNull(nullptr);

.. cpp:function:: void assertDeath(auto func)

    Asserts that ``func`` causes the child process to exit abnormally (via ``fork`` / ``waitpid``). Unix and macOS only.

    .. code-block:: cpp

        assertDeath([]() { throw std::runtime_error("crash!"); });
        assertDeath([]() { std::abort(); });

.. cpp:function:: void assertContractViolation(Func f)

    Asserts that calling ``f`` triggers a C++26 contract violation.

    .. code-block:: cpp

        int safe_divide(int a, int b) pre(b > 0) { return a / b; }

        assertContractViolation([]() { safe_divide(10, 0); });  // Triggers pre(b > 0)

.. cpp:function:: void assertNoContractViolation(Func f)

    Asserts that calling ``f`` does **not** trigger a contract violation.

    .. code-block:: cpp

        assertNoContractViolation([]() { safe_divide(10, 2); });  // OK

Expectations
------------

Expectations verify conditions that are **not** critical failures — if an expectation fails, the test is aborted and the library skips the remaining lines in that test case. An :cpp:class:`Abort` exception is thrown instead of :cpp:class:`Error`.

.. cpp:function:: void expectEqual(auto expected, auto actual)

    Expects that ``expected`` and ``actual`` are equal.

    .. code-block:: cpp

        expectEqual(4, 2 + 2);

.. cpp:function:: void expectNotEqual(auto expected, auto actual)

    Expects that ``expected`` and ``actual`` are not equal.

    .. code-block:: cpp

        expectNotEqual(3, 4);

.. cpp:function:: void expectTrue(auto value)

    Expects that ``value`` is truthy.

.. cpp:function:: void expectFalse(auto value)

    Expects that ``value`` is falsy.

.. cpp:function:: void expectNear(auto expected, auto actual, auto tol)

    Expects that ``abs(expected - actual) <= tol``.

.. cpp:function:: void expectLess(auto a, auto b)

    Expects that ``a < b``.

.. cpp:function:: void expectLessEqual(auto a, auto b)

    Expects that ``a <= b``.

.. cpp:function:: void expectGreater(auto a, auto b)

    Expects that ``a > b``.

.. cpp:function:: void expectGreaterEqual(auto a, auto b)

    Expects that ``a >= b``.

.. cpp:function:: void expectContains(auto const& container, auto const& value)

    Expects that ``value`` exists in ``container``.

.. cpp:function:: template <typename E = std::exception> void expectThrows(auto func)

    Expects that ``func`` throws an exception of type ``E``.

.. cpp:function:: template <typename E> void expectThrowsExact(auto func)

    Expects that ``func`` throws exactly type ``E``.

.. cpp:function:: void expectNull(auto ptr)

    Expects that ``ptr`` equals ``nullptr``.

    .. code-block:: cpp

        expectNull(nullptr);

.. cpp:function:: void expectDeath(auto func)

    Expects that ``func`` causes the child process to exit abnormally. Unix and macOS only.

    .. code-block:: cpp

        expectDeath([]() { throw std::runtime_error("die"); });

.. cpp:function:: void expectContractViolation(Func f)

    Expects that calling ``f`` triggers a C++26 contract violation.

.. cpp:function:: void expectNoContractViolation(Func f)

    Expects that calling ``f`` does **not** trigger a contract violation.

.. cpp:function:: void assertThrowsExact(auto func)

    Asserts that ``func`` throws exactly type ``E`` (checked with ``typeid``).

.. cpp:function:: void expectThrowsExact(auto func)

    Expects that ``func`` throws exactly type ``E``.
