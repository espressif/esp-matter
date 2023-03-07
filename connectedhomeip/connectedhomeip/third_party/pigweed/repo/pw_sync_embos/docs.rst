.. _module-pw_sync_embos:

=============
pw_sync_embos
=============
This is a set of backends for pw_sync based on embOS v4.

--------------------------------
Critical Section Lock Primitives
--------------------------------

Mutex & TimedMutex
==================
The embOS v4 backend for the Mutex and TimedMutex use ``OS_RSEMA`` as the
underlying type. It is created using ``OS_CreateRSema`` as part of the
constructors and cleaned up using ``OS_DeleteRSema`` in the destructors.

InterruptSpinLock
=================
The embOS v4 backend for InterruptSpinLock is backed by a ``bool`` which permits
these objects to detect accidental recursive locking.

This object uses ``OS_IncDI`` and ``OS_DecRI`` to mask interrupts which enables
the critical section. In addition, ``OS_SuspendAllTasks`` and
``OS_ResumeAllSuspendedTasks`` are used to to prevent accidental thread context
switches while the InterruptSpinLock is locked.

--------------------
Signaling Primitives
--------------------

ThreadNotification & TimedThreadNotification
============================================
The native embOS v4 implementation of its semaphores (``OS_CSEMA``) is very
efficient, ergo we recommend using the binary semaphore backends for
ThreadNotifications:
- ``pw_sync:binary_semaphore_thread_notification_backend``
- ``pw_sync:binary_semaphore_timed_thread_notification_backend``

BinarySemaphore & CountingSemaphore
===================================
The embOS v4 backends for the BinarySemaphore and CountingSemaphore use
``OS_CSEMA`` as the underlying type. It is created using ``OS_CreateCSema`` as
part of the constructor and cleaned up using ``OS_DeleteCSema`` in the
destructor.
