Contract Testing
================

Raw Source
----------

.. literalinclude:: ../../../examples/example06_contracts.cpp
   :language: cpp

Breaking It Down
----------------

Section 1 — **assertContractViolation on division by zero**

.. code-block:: cpp

   [[= Test{"contract triggered on bad input"}]]
   void division_by_zero() {
     assertContractViolation([]() {
       safe_divide(10, 0);  // Triggers pre(b > 0)
     });
   }

Wraps the violating code in a lambda. If no contract fires, the assertion fails.

Section 2 — **assertContractViolation on a different contract**

.. code-block:: cpp

   [[= Test{"contract triggered on negative"}]]
   void negative_input() {
     assertContractViolation([]() {
       safe_divide(-5, 3);  // Triggers pre(a > 0)
     });
   }

The ``pre(a > 0)`` clause fires when ``a`` is negative.

Section 3 — **assertNoContractViolation**

.. code-block:: cpp

   [[= Test{"no contract on valid input"}]]
   void valid_input() {
     assertNoContractViolation([]() {
       safe_divide(10, 2);  // All contracts pass, returns 5
     });
   }

If a contract **does** fire, the assertion fails.

Section 4 — **expectContractViolation**

.. code-block:: cpp

   [[= Test{"expect contract violation"}]]
   void expect_violation() {
     expectContractViolation([]() {
       safe_divide(10, 0);  // Triggers pre(b > 0)
     });
     // If we reach here, the expectation was met
     assertEqual(1, 1);
   }

Same as ``assertContractViolation`` but uses :cpp:class:`Abort` instead of :cpp:class:`Error`.

Section 5 — **expectNoContractViolation**

.. code-block:: cpp

   [[= Test{"expect no contract violation"}]]
   void expect_no_violation() {
     expectNoContractViolation([]() {
       safe_divide(6, 2);  // Returns 3 — all contracts pass
     });
   }

Same as ``assertNoContractViolation`` but uses :cpp:class:`Abort`.
