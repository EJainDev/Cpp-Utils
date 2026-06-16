.. cpp:namespace:: annotest

Assertions
----------

Assertions verify that a condition holds. If an assertion fails, an :cpp:class:`Error` exception is thrown, the test case is marked as failed, and the library continues with the next test.

.. cpp:function:: void assertEqual(auto expected, auto actual)

    Asserts that ``expected`` and ``actual`` are equal (checked via ``!=``).

.. cpp:function:: void assertNotEqual(auto expected, auto actual)

    Asserts that ``expected`` and ``actual`` are not equal (checked via ``==``).

.. cpp:function:: void assertTrue(auto value)

    Asserts that ``value`` is truthy.

.. cpp:function:: void assertFalse(auto value)

    Asserts that ``value`` is falsy.

.. cpp:function:: void assertNear(auto expected, auto actual, auto tol = 1e-3f)

    Asserts that ``abs(expected - actual) <= tol``.

.. cpp:function:: void assertLess(auto a, auto b)

    Asserts that ``a < b``.

.. cpp:function:: void assertLessEqual(auto a, auto b)

    Asserts that ``a <= b``.

.. cpp:function:: void assertGreater(auto a, auto b)

    Asserts that ``a > b``.

.. cpp:function:: void assertGreaterEqual(auto a, auto b)

    Asserts that ``a >= b``.

.. cpp:function:: void assertContains(auto const& container, auto const& value)

    Asserts that ``value`` exists in ``container`` (uses ``std::find``).

.. cpp:function:: template <typename E = std::exception> void assertThrows(auto func)

    Asserts that ``func`` throws an exception of type ``E``.

.. cpp:function:: template <typename E> void assertThrowsExact(auto func)

    Asserts that ``func`` throws exactly type ``E`` (checked with ``typeid``).

.. cpp:function:: void assertNull(auto ptr)

    Asserts that ``ptr`` equals ``nullptr``.

.. cpp:function:: void assertDeath(auto func)

    Asserts that ``func`` causes the child process to exit abnormally (via ``fork`` / ``waitpid``). Unix and macOS only.

Expectations
------------

Expectations verify conditions that are **not** critical failures — if an expectation fails, the test is aborted and the library skips the remaining lines in that test case. An :cpp:class:`Abort` exception is thrown instead of :cpp:class:`Error`.

.. cpp:function:: void expectEqual(auto expected, auto actual)

    Expects that ``expected`` and ``actual`` are equal.

.. cpp:function:: void expectNotEqual(auto expected, auto actual)

    Expects that ``expected`` and ``actual`` are not equal.

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

.. cpp:function:: void expectDeath(auto func)

    Expects that ``func`` causes the child process to exit abnormally. Unix and macOS only.