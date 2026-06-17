OS Annotations
==============

Raw Source
----------

.. literalinclude:: ../../../examples/example04_os_annotations.cpp
   :language: cpp

Breaking It Down
----------------

Section 1 — **Single OS requirement**

.. code-block:: cpp

   [[= Test{}, = RequiresOS{OS::Linux}]]
   void linux_only() {
     assertEqual(1, 1);
     std::cout << "Running on Linux\n";
   }

The test runs **only** on Linux. On Windows or macOS it is silently skipped with a diagnostic message.

Section 2 — **Multiple OS requirement (OR logic)**

.. code-block:: cpp

   [[= Test{}, = RequiresOS{OS::Linux, OS::Windows}]]
   void linux_or_windows() {
     assertEqual(2, 1 + 1);
   }

Accepts multiple OS values. The test runs if the current OS matches **any** of them.

Section 3 — **DisallowOS (inverse)**

.. code-block:: cpp

   [[= Test{}, = DisallowOS{OS::Windows}]]
   void not_windows() {
     assertEqual(1, 1);
   }

The test is skipped on listed platforms, runs on all others.

Section 4 — **Disallowed with multiple platforms**

.. code-block:: cpp

   [[= Test{}, = DisallowOS{OS::Windows, OS::Mac}]]
   void linux_only_by_exclusion() {
     assertEqual(1, 1);
   }

Skipped on both Windows and macOS, runs on Linux.

Section 5 — **Mixed features**

.. code-block:: cpp

   [[= Test<"cross-platform math">{}, = RequiresOS{OS::Linux, OS::Mac, OS::Windows}]]
   void always_runs() {
     assertEqual(5, 2 + 3);
   }

OS annotations work alongside ``Test`` with a custom display name. The test has a custom name and runs on all platforms.
