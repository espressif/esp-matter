.. _module-pw_sync_baremetal:

=================
pw_sync_baremetal
=================
This is a set of backends for pw_sync that works on baremetal targets. It is not
ready for use, and is under construction.

.. note::
  All constructs in this baremetal backend do not support hardware
  multi-threading (SMP, SMT, etc).

.. warning::
  It does not perform interrupt masking or disable global interrupts. This is
  not safe to use yet!

-----------------
InterruptSpinLock
-----------------
The interrupt spin-lock implementation makes a single attempt to acquire the
lock and asserts if it is unavailable. It does not perform interrupt masking or
disable global interrupts.

-----
Mutex
-----
The mutex implementation makes a single attempt to acquire the lock and asserts
if it is unavailable.

--------------
RecursiveMutex
--------------
The recursive mutex implementation counts the number of lock and unlock calls
and asserts if the mutex is unlocked too many times or destroyed while locked.
Note that recursive mutexes are not available for general use in Pigweed.
