Parameterized Tests
===================

Raw Source
----------

.. literalinclude:: ../../../examples/example03_parameterized.cpp
   :language: cpp

Breaking It Down
----------------

Section 1 — **Parameterize with single parameters**

.. code-block:: cpp

   [[= Test{}, = Parameterize{tuple(2, 3), tuple(5, 7), tuple(-1, -3)}]]
   void addition(int a, int b) {
     assertEqual(a + b, a + b);
     assertTrue(a != 0 && b != 0);
   }

Each ``tuple()`` call is a complete parameter set. The parameters are injected **in order** into the test function's arguments. Three sub-runs: ``(2,3)``, ``(5,7)``, and ``(-1,-3)``.

Section 2 — **Parameterize with multiple parameters**

.. code-block:: cpp

   [[= Test{}, = Parameterize{tuple(1.0), tuple(2.5), tuple(0.001)}]]
   void floating_point(double x) {
     assertNear(x, x);
     assertTrue(x >= 0.0);
   }

Each tuple creates one sub-run with a single value.

Section 3 — **ParameterizeTemplate with typed params**

.. code-block:: cpp

   [[= Test{}, = ParameterizeTemplate{tuple(10), tuple(20), tuple(100)}]]
   void multiples(int n) {
     assertTrue(n % 10 == 0);
     assertGreater(n, 0);
   }

``ParameterizeTemplate`` passes **typed** tuples instead of raw values. Useful when parameters have complex types.

Section 4 — **Parameterized tests with hooks**

.. code-block:: cpp

   [[= BeforeEach{}]] void reset() { set_count = 0; }

   [[= Test{}, = Parameterize{tuple(100), tuple(200), tuple(300)}]]
   void counting(int val) {
     ++set_count;
     assertEqual(val, val);
     assertTrue(val % 100 == 0);
   }

BeforeEach runs before **each parameter set**, not each test. AfterEach runs after each set too. The counter resets to 0 before each of the 3 parameter sets.
