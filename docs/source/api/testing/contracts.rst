.. _contract_handling:

Contract Handling
=================

.. cpp:namespace:: annotest

C++26 contracts are runtime checks enforced at function entry (`pre`), function exit (`post`), or inline (`contract_assert`). AnnoTest provides contract assertion helpers and a hook for custom violation handling.

.. note:: C++26 contracts are **compiler builtin** features — they are part of the C++26 standard. GCC 16 implements the full contract runtime API.

Contract Builtin Statements
---------------------------

Three contract builtins are available:

.. code-block:: cpp

    int foo(int x) pre(x > 0) pre(x < 100) post(r: r > 0) {
        contract_assert(x != 50);  // inline invariant
        return -x;
    }

.. list-table::
   :widths: 25 25 50
   :stub-columns: 1
   :header-rows: 1

   * - Statement
     - Placement
     - Purpose
   * - ``pre(boolean_expr)``
     - Function signature (before ``post``)
     - Validates parameters on function entry. Multiple clauses allowed.
   * - ``contract_assert(boolean_expr)``
     - Inline in function body
     - Asserts an invariant at runtime. Replaces ``assert()`` — no user message.
   * - ``post(r : boolean_expr)``
     - Function signature (after ``pre()``)
     - Validates return value on exit. ``r`` is user-chosen, binds to return value.

Example:

.. code-block:: cpp

    // Pre: parameter must be positive
    // Post: return value must be positive
    int safe_divide(int a, int b) pre(a > 0) pre(b > 0) post(r: r > 0) {
        return a / b;
    }

Testing Contracts
-----------------

See the `example gallery <examples/index.html>`__ for usage examples. For the full function signatures, see Assertions.

See also :ref:`getting_started`.

Contract Handler Header
------------------------

Include the following header to install AnnoTest's default contract violation handler:

.. code-block:: cpp

   #include <annotest_contract.h>

When a contract is violated, the handler:

1. Extracts the violation location (file, line, function).
2. Sets :cpp:var:`annotest::contract_violation_occurred` to ``true``.
3. Calls a :cpp:var:`custom_handler` if one has been registered.

Enforcement Modes
-----------------

C++26 contracts support four enforcement modes (configured per-executable at compile time):

.. list-table::
   :widths: 25 45 30
   :stub-columns: 1
   :header-rows: 1

   * - Mode
     - Behavior
     - Use case
   * - ``ignore``
     - Removes all contract statements at compile time.
     - Production (max performance)
   * - ``observe``
     - Calls handler (if present), then **continues** execution. Slight runtime overhead.
     - Production with slight overhead
   * - ``enforce``
     - Calls handler (if present), then calls ``std::terminate``.
     - Default. For debugging.
   * - ``quick_enforce``
     - Calls ``std::terminate`` directly, **without** calling the handler.
     - Debugging without handler overhead

Global State
------------

The ``annotest:contracts`` module exports two global variables:

.. cpp:var:: bool contract_violation_occurred

    Tracks whether any contract violation has occurred during test execution.
    The library checks this flag after assert/expect calls to avoid double-reporting.

.. cpp:var:: std::atomic<void (*)(const std::contracts::contract_violation&)> custom_handler

    A callable function pointer for custom contract violation handling.
    Set to ``nullptr`` by default. When non-null, the default handler delegates
    to this function after setting :cpp:var:`contract_violation_occurred` to ``true``.

Example:

.. code-block:: cpp

   #include <annotest_contract.h>

   void my_handler(const std::contracts::contract_violation& v) {
       std::cerr << "Contract violated at "
                 << v.location().file_name()
                 << ":" << v.location().line_number() << "\n";
   }

   int main() {
       annotest::custom_handler = my_handler;
       return test<MySuite>(__argc, __argv);
   }

See also :ref:`getting_started`.
