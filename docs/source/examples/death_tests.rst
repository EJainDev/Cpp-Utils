Death Tests
===========

Raw Source
----------

.. literalinclude:: ../../../examples/example05_death_tests.cpp
   :language: cpp

Breaking It Down
----------------

Section 1 — **assertDeath with exception-triggered death**

.. code-block:: cpp

   [[= Test{"death by exception"}]]
   void child_crashes() {
     assertDeath([]() { throw std::runtime_error("crash!"); });
   }

The lambda is executed in a child process. If the child throws or exits with non-zero status, ``assertDeath`` succeeds.

Section 2 — **assertDeath with std::abort()**

.. code-block:: cpp

   [[= Test{"death by abort"}]]
   void child_aborts() {
     assertDeath([]() { std::abort(); });
   }

Any abnormal exit — exception, ``abort()``, or signal — counts as death.

Section 3 — **expectDeath**

.. code-block:: cpp

   [[= Test{"expect death by crash"}]]
   void expect_child_crashes() {
     expectDeath([]() { throw std::runtime_error("oops"); });
   }

Behaves the same as ``assertDeath``, but throws :cpp:class:`Abort` instead of :cpp:class:`Error` on failure (aborts current test, continues suite).

Section 4 — **Surviving process — failure case**

.. code-block:: cpp

   [[= Test{"child survives — should fail"}]]
   void child_survives() {
     assertDeath([]() { return; });  // Expected to fail: child exits 0
   }

If the child exits normally (status 0), the death test **fails**. This demonstrates the negative case.
