.. _module-pw_sync_threadx:

===============
pw_sync_threadx
===============
This is a set of backends for pw_sync based on ThreadX.

It is possible, if necessary, to use pw_sync_threadx without using the Pigweed
provided pw_chrono_threadx in case the ThreadX time API (``tx_time_get()``)) is
not available (i.e. ``TX_NO_TIMER`` is set). You are responsible for ensuring
that the chrono backend provided has counts which match the ThreadX tick based
API.

--------------------------------
Critical Section Lock Primitives
--------------------------------

Mutex & TimedMutex
==================
The ThreadX backend for the Mutex and TimedMutex use ``TX_MUTEX`` as the
underlying type. It is created using ``tx_mutex_create`` as part of the
constructors and cleaned up using ``tx_mutex_delete`` in the destructors.

InterruptSpinLock
=================
The ThreadX backend for InterruptSpinLock is backed by an ``enum class`` and
two ``UINT`` which permits these objects to detect accidental recursive locking
and unlocking contexts.

This object uses ``tx_interrupt_control`` to enable critical sections. In
addition, ``tx_thread_preemption_change`` is used to prevent accidental thread
context switches while the InterruptSpinLock is held by a thread.

.. Warning::
  This backend does not support SMP yet as there's no internal lock to spin on.

--------------------
Signaling Primitives
--------------------

ThreadNotification & TimedThreadNotification
============================================
The native ThreadX API does cover direct thread signaling and ergo we recommend
using the binary semaphore backends for ThreadNotifications:
- ``pw_sync:binary_semaphore_thread_notification_backend``
- ``pw_sync:binary_semaphore_timed_thread_notification_backend``

Background Information
----------------------
Although one may be tempted to use ``tx_thread_sleep`` and
``tx_thread_wait_abort`` to implement direct thread notifications with ThreadX,
this unfortunately cannot work. Between the blocking thread setting its
``TX_THREAD*`` handle and actually executing ``tx_thread_sleep`` there will
always exist a race condition. Another thread and/or interrupt may attempt
to invoke ``tx_thread_wait_abort`` before the blocking thread has executed
``tx_thread_sleep`` meaning the wait abort would fail.

BinarySemaphore & CountingSemaphore
===================================
The ThreadX backends for the BinarySemaphore and CountingSemaphore use
``TX_SEMAPHORE`` as the underlying type. It is created using
``tx_semaphore_create`` as part of the constructor and cleaned up using
``tx_semaphore_delete`` in the destructor.
