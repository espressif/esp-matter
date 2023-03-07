.. _module-pw_sync:

=======
pw_sync
=======
The ``pw_sync`` module contains utilities for synchronizing between threads
and/or interrupts through signaling primitives and critical section lock
primitives.

.. Warning::
  This module is still under construction, the API is not yet stable.

.. Note::
  The objects in this module do not have an Init() style public API which is
  common in many RTOS C APIs. Instead, they rely on being able to invoke the
  native initialization APIs for synchronization primitives during C++
  construction.
  In order to support global statically constructed synchronization without
  constexpr constructors, the user and/or backend **MUST** ensure that any
  initialization required in your environment is done prior to the creation
  and/or initialization of the native synchronization primitives
  (e.g. kernel initialization).

--------------------------------
Critical Section Lock Primitives
--------------------------------
The critical section lock primitives provided by this module comply with
`BasicLockable <https://en.cppreference.com/w/cpp/named_req/BasicLockable>`_,
`Lockable <https://en.cppreference.com/w/cpp/named_req/Lockable>`_, and where
relevant
`TimedLockable <https://en.cppreference.com/w/cpp/named_req/TimedLockable>`_ C++
named requirements. This means that they are compatible with existing helpers in
the STL's ``<mutex>`` thread support library. For example `std::lock_guard <https://en.cppreference.com/w/cpp/thread/lock_guard>`_
and `std::unique_lock <https://en.cppreference.com/w/cpp/thread/unique_lock>`_ can be directly used.

Mutex
=====
The Mutex is a synchronization primitive that can be used to protect shared data
from being simultaneously accessed by multiple threads. It offers exclusive,
non-recursive ownership semantics where priority inheritance is used to solve
the classic priority-inversion problem.

The Mutex's API is C++11 STL
`std::mutex <https://en.cppreference.com/w/cpp/thread/mutex>`_ like,
meaning it is a
`BasicLockable <https://en.cppreference.com/w/cpp/named_req/BasicLockable>`_
and `Lockable <https://en.cppreference.com/w/cpp/named_req/Lockable>`_.

.. list-table::

  * - *Supported on*
    - *Backend module*
  * - FreeRTOS
    - :ref:`module-pw_sync_freertos`
  * - ThreadX
    - :ref:`module-pw_sync_threadx`
  * - embOS
    - :ref:`module-pw_sync_embos`
  * - STL
    - :ref:`module-pw_sync_stl`
  * - Baremetal
    - Planned
  * - Zephyr
    - Planned
  * - CMSIS-RTOS API v2 & RTX5
    - Planned

C++
---
.. cpp:class:: pw::sync::Mutex

  .. cpp:function:: void lock()

     Locks the mutex, blocking indefinitely. Failures are fatal.

     **Precondition:** The lock isn't already held by this thread. Recursive
     locking is undefined behavior.

  .. cpp:function:: bool try_lock()

     Tries to lock the mutex in a non-blocking manner.
     Returns true if the mutex was successfully acquired.

     **Precondition:** The lock isn't already held by this thread. Recursive
     locking is undefined behavior.

  .. cpp:function:: void unlock()

     Unlocks the mutex. Failures are fatal.

     **Precondition:** The mutex is held by this thread.


  .. list-table::

    * - *Safe to use in context*
      - *Thread*
      - *Interrupt*
      - *NMI*
    * - ``Mutex::Mutex``
      - ✔
      -
      -
    * - ``Mutex::~Mutex``
      - ✔
      -
      -
    * - ``void Mutex::lock``
      - ✔
      -
      -
    * - ``bool Mutex::try_lock``
      - ✔
      -
      -
    * - ``void Mutex::unlock``
      - ✔
      -
      -

Examples in C++
^^^^^^^^^^^^^^^
.. code-block:: cpp

  #include "pw_sync/mutex.h"

  pw::sync::Mutex mutex;

  void ThreadSafeCriticalSection() {
    mutex.lock();
    NotThreadSafeCriticalSection();
    mutex.unlock();
  }


Alternatively you can use C++'s RAII helpers to ensure you always unlock.

.. code-block:: cpp

  #include <mutex>

  #include "pw_sync/mutex.h"

  pw::sync::Mutex mutex;

  void ThreadSafeCriticalSection() {
    std::lock_guard lock(mutex);
    NotThreadSafeCriticalSection();
  }


C
-
The Mutex must be created in C++, however it can be passed into C using the
``pw_sync_Mutex`` opaque struct alias.

.. cpp:function:: void pw_sync_Mutex_Lock(pw_sync_Mutex* mutex)

  Invokes the ``Mutex::lock`` member function on the given ``mutex``.

.. cpp:function:: bool pw_sync_Mutex_TryLock(pw_sync_Mutex* mutex)

  Invokes the ``Mutex::try_lock`` member function on the given ``mutex``.

.. cpp:function:: void pw_sync_Mutex_Unlock(pw_sync_Mutex* mutex)

  Invokes the ``Mutex::unlock`` member function on the given ``mutex``.

.. list-table::

  * - *Safe to use in context*
    - *Thread*
    - *Interrupt*
    - *NMI*
  * - ``void pw_sync_Mutex_Lock``
    - ✔
    -
    -
  * - ``bool pw_sync_Mutex_TryLock``
    - ✔
    -
    -
  * - ``void pw_sync_Mutex_Unlock``
    - ✔
    -
    -

Example in C
^^^^^^^^^^^^
.. code-block:: cpp

  #include "pw_sync/mutex.h"

  pw::sync::Mutex mutex;

  extern pw_sync_Mutex mutex;  // This can only be created in C++.

  void ThreadSafeCriticalSection(void) {
    pw_sync_Mutex_Lock(&mutex);
    NotThreadSafeCriticalSection();
    pw_sync_Mutex_Unlock(&mutex);
  }

TimedMutex
==========
The TimedMutex is an extension of the Mutex which offers timeout and deadline
based semantics.

The TimedMutex's API is C++11 STL
`std::timed_mutex <https://en.cppreference.com/w/cpp/thread/timed_mutex>`_ like,
meaning it is a
`BasicLockable <https://en.cppreference.com/w/cpp/named_req/BasicLockable>`_,
`Lockable <https://en.cppreference.com/w/cpp/named_req/Lockable>`_, and
`TimedLockable <https://en.cppreference.com/w/cpp/named_req/TimedLockable>`_.

Note that the ``TimedMutex`` is a derived ``Mutex`` class, meaning that
a ``TimedMutex`` can be used by someone who needs the basic ``Mutex``. This is
in contrast to the C++ STL's
`std::timed_mutex <https://en.cppreference.com/w/cpp/thread/timed_mutex>`_.


.. list-table::

  * - *Supported on*
    - *Backend module*
  * - FreeRTOS
    - :ref:`module-pw_sync_freertos`
  * - ThreadX
    - :ref:`module-pw_sync_threadx`
  * - embOS
    - :ref:`module-pw_sync_embos`
  * - STL
    - :ref:`module-pw_sync_stl`
  * - Zephyr
    - Planned
  * - CMSIS-RTOS API v2 & RTX5
    - Planned

C++
---
.. cpp:class:: pw::sync::TimedMutex

  .. cpp:function:: void lock()

     Locks the mutex, blocking indefinitely. Failures are fatal.

     **Precondition:** The lock isn't already held by this thread. Recursive
     locking is undefined behavior.

  .. cpp:function:: bool try_lock()

     Tries to lock the mutex in a non-blocking manner.
     Returns true if the mutex was successfully acquired.

     **Precondition:** The lock isn't already held by this thread. Recursive
     locking is undefined behavior.


  .. cpp:function:: bool try_lock_for(const chrono::SystemClock::duration& timeout)

     Tries to lock the mutex. Blocks until specified the timeout has elapsed or
     the lock is acquired, whichever comes first.
     Returns true if the mutex was successfully acquired.

     **Precondition:** The lock isn't already held by this thread. Recursive
     locking is undefined behavior.

  .. cpp:function:: bool try_lock_until(const chrono::SystemClock::time_point& deadline)

     Tries to lock the mutex. Blocks until specified deadline has been reached
     or the lock is acquired, whichever comes first.
     Returns true if the mutex was successfully acquired.

     **Precondition:** The lock isn't already held by this thread. Recursive
     locking is undefined behavior.

  .. cpp:function:: void unlock()

     Unlocks the mutex. Failures are fatal.

     **Precondition:** The mutex is held by this thread.


  .. list-table::

    * - *Safe to use in context*
      - *Thread*
      - *Interrupt*
      - *NMI*
    * - ``TimedMutex::TimedMutex``
      - ✔
      -
      -
    * - ``TimedMutex::~TimedMutex``
      - ✔
      -
      -
    * - ``void TimedMutex::lock``
      - ✔
      -
      -
    * - ``bool TimedMutex::try_lock``
      - ✔
      -
      -
    * - ``bool TimedMutex::try_lock_for``
      - ✔
      -
      -
    * - ``bool TimedMutex::try_lock_until``
      - ✔
      -
      -
    * - ``void TimedMutex::unlock``
      - ✔
      -
      -

Examples in C++
^^^^^^^^^^^^^^^
.. code-block:: cpp

  #include "pw_chrono/system_clock.h"
  #include "pw_sync/timed_mutex.h"

  pw::sync::TimedMutex mutex;

  bool ThreadSafeCriticalSectionWithTimeout(
      const SystemClock::duration timeout) {
    if (!mutex.try_lock_for(timeout)) {
      return false;
    }
    NotThreadSafeCriticalSection();
    mutex.unlock();
    return true;
  }


Alternatively you can use C++'s RAII helpers to ensure you always unlock.

.. code-block:: cpp

  #include <mutex>

  #include "pw_chrono/system_clock.h"
  #include "pw_sync/timed_mutex.h"

  pw::sync::TimedMutex mutex;

  bool ThreadSafeCriticalSectionWithTimeout(
      const SystemClock::duration timeout) {
    std::unique_lock lock(mutex, std::defer_lock);
    if (!lock.try_lock_for(timeout)) {
      return false;
    }
    NotThreadSafeCriticalSection();
    return true;
  }



C
-
The TimedMutex must be created in C++, however it can be passed into C using the
``pw_sync_TimedMutex`` opaque struct alias.

.. cpp:function:: void pw_sync_TimedMutex_Lock(pw_sync_TimedMutex* mutex)

  Invokes the ``TimedMutex::lock`` member function on the given ``mutex``.

.. cpp:function:: bool pw_sync_TimedMutex_TryLock(pw_sync_TimedMutex* mutex)

  Invokes the ``TimedMutex::try_lock`` member function on the given ``mutex``.

.. cpp:function:: bool pw_sync_TimedMutex_TryLockFor(pw_sync_TimedMutex* mutex, pw_chrono_SystemClock_Duration timeout)

  Invokes the ``TimedMutex::try_lock_for`` member function on the given ``mutex``.

.. cpp:function:: bool pw_sync_TimedMutex_TryLockUntil(pw_sync_TimedMutex* mutex, pw_chrono_SystemClock_TimePoint deadline)

  Invokes the ``TimedMutex::try_lock_until`` member function on the given ``mutex``.

.. cpp:function:: void pw_sync_TimedMutex_Unlock(pw_sync_TimedMutex* mutex)

  Invokes the ``TimedMutex::unlock`` member function on the given ``mutex``.

.. list-table::

  * - *Safe to use in context*
    - *Thread*
    - *Interrupt*
    - *NMI*
  * - ``void pw_sync_TimedMutex_Lock``
    - ✔
    -
    -
  * - ``bool pw_sync_TimedMutex_TryLock``
    - ✔
    -
    -
  * - ``bool pw_sync_TimedMutex_TryLockFor``
    - ✔
    -
    -
  * - ``bool pw_sync_TimedMutex_TryLockUntil``
    - ✔
    -
    -
  * - ``void pw_sync_TimedMutex_Unlock``
    - ✔
    -
    -

Example in C
^^^^^^^^^^^^
.. code-block:: cpp

  #include "pw_chrono/system_clock.h"
  #include "pw_sync/timed_mutex.h"

  pw::sync::TimedMutex mutex;

  extern pw_sync_TimedMutex mutex;  // This can only be created in C++.

  bool ThreadSafeCriticalSectionWithTimeout(
      const pw_chrono_SystemClock_Duration timeout) {
    if (!pw_sync_TimedMutex_TryLockFor(&mutex, timeout)) {
      return false;
    }
    NotThreadSafeCriticalSection();
    pw_sync_TimedMutex_Unlock(&mutex);
    return true;
  }

RecursiveMutex
==============
``pw_sync`` provides ``pw::sync::RecursiveMutex``, a recursive mutex
implementation. At this time, this facade can only be used internally by
Pigweed.

InterruptSpinLock
=================
The InterruptSpinLock is a synchronization primitive that can be used to protect
shared data from being simultaneously accessed by multiple threads and/or
interrupts as a targeted global lock, with the exception of Non-Maskable
Interrupts (NMIs). It offers exclusive, non-recursive ownership semantics where
IRQs up to a backend defined level of "NMIs" will be masked to solve
priority-inversion.

This InterruptSpinLock relies on built-in local interrupt masking to make it
interrupt safe without requiring the caller to separately mask and unmask
interrupts when using this primitive.

Unlike global interrupt locks, this also works safely and efficiently on SMP
systems. On systems which are not SMP, spinning is not required but some state
may still be used to detect recursion.

The InterruptSpinLock is a
`BasicLockable <https://en.cppreference.com/w/cpp/named_req/BasicLockable>`_
and
`Lockable <https://en.cppreference.com/w/cpp/named_req/Lockable>`_.

.. list-table::

  * - *Supported on*
    - *Backend module*
  * - FreeRTOS
    - :ref:`module-pw_sync_freertos`
  * - ThreadX
    - :ref:`module-pw_sync_threadx`
  * - embOS
    - :ref:`module-pw_sync_embos`
  * - STL
    - :ref:`module-pw_sync_stl`
  * - Baremetal
    - Planned, not ready for use
  * - Zephyr
    - Planned
  * - CMSIS-RTOS API v2 & RTX5
    - Planned

C++
---
.. cpp:class:: pw::sync::InterruptSpinLock

  .. cpp:function:: void lock()

      Locks the spinlock, blocking indefinitely. Failures are fatal.

      **Precondition:** Recursive locking is undefined behavior.

  .. cpp:function:: bool try_lock()

      Tries to lock the spinlock in a non-blocking manner.
      Returns true if the spinlock was successfully acquired.

      **Precondition:** Recursive locking is undefined behavior.

  .. cpp:function:: void unlock()

     Unlocks the mutex. Failures are fatal.

     **Precondition:** The spinlock is held by the caller.

  .. list-table::

    * - *Safe to use in context*
      - *Thread*
      - *Interrupt*
      - *NMI*
    * - ``InterruptSpinLock::InterruptSpinLock``
      - ✔
      - ✔
      -
    * - ``InterruptSpinLock::~InterruptSpinLock``
      - ✔
      - ✔
      -
    * - ``void InterruptSpinLock::lock``
      - ✔
      - ✔
      -
    * - ``bool InterruptSpinLock::try_lock``
      - ✔
      - ✔
      -
    * - ``void InterruptSpinLock::unlock``
      - ✔
      - ✔
      -

Examples in C++
^^^^^^^^^^^^^^^
.. code-block:: cpp

  #include "pw_sync/interrupt_spin_lock.h"

  pw::sync::InterruptSpinLock interrupt_spin_lock;

  void InterruptSafeCriticalSection() {
    interrupt_spin_lock.lock();
    NotThreadSafeCriticalSection();
    interrupt_spin_lock.unlock();
  }


Alternatively you can use C++'s RAII helpers to ensure you always unlock.

.. code-block:: cpp

  #include <mutex>

  #include "pw_sync/interrupt_spin_lock.h"

  pw::sync::InterruptSpinLock interrupt_spin_lock;

  void InterruptSafeCriticalSection() {
    std::lock_guard lock(interrupt_spin_lock);
    NotThreadSafeCriticalSection();
  }


C
-
The InterruptSpinLock must be created in C++, however it can be passed into C using the
``pw_sync_InterruptSpinLock`` opaque struct alias.

.. cpp:function:: void pw_sync_InterruptSpinLock_Lock(pw_sync_InterruptSpinLock* interrupt_spin_lock)

  Invokes the ``InterruptSpinLock::lock`` member function on the given ``interrupt_spin_lock``.

.. cpp:function:: bool pw_sync_InterruptSpinLock_TryLock(pw_sync_InterruptSpinLock* interrupt_spin_lock)

  Invokes the ``InterruptSpinLock::try_lock`` member function on the given ``interrupt_spin_lock``.

.. cpp:function:: void pw_sync_InterruptSpinLock_Unlock(pw_sync_InterruptSpinLock* interrupt_spin_lock)

  Invokes the ``InterruptSpinLock::unlock`` member function on the given ``interrupt_spin_lock``.

.. list-table::

  * - *Safe to use in context*
    - *Thread*
    - *Interrupt*
    - *NMI*
  * - ``void pw_sync_InterruptSpinLock_Lock``
    - ✔
    - ✔
    -
  * - ``bool pw_sync_InterruptSpinLock_TryLock``
    - ✔
    - ✔
    -
  * - ``void pw_sync_InterruptSpinLock_Unlock``
    - ✔
    - ✔
    -

Example in C
^^^^^^^^^^^^
.. code-block:: cpp

  #include "pw_chrono/system_clock.h"
  #include "pw_sync/interrupt_spin_lock.h"

  pw::sync::InterruptSpinLock interrupt_spin_lock;

  extern pw_sync_InterruptSpinLock interrupt_spin_lock;  // This can only be created in C++.

  void InterruptSafeCriticalSection(void) {
    pw_sync_InterruptSpinLock_Lock(&interrupt_spin_lock);
    NotThreadSafeCriticalSection();
    pw_sync_InterruptSpinLock_Unlock(&interrupt_spin_lock);
  }

Thread Safety Lock Annotations
==============================
Pigweed's critical section lock primitives support Clang's thread safety
analysis extension for C++. The analysis is completely static at compile-time.
This is only supported when building with Clang. The annotations are no-ops when
using different compilers.

Pigweed provides the ``pw_sync/lock_annotations.h`` header file with macro
definitions to allow developers to document the locking policies of
multi-threaded code. The annotations can also help program analysis tools to
identify potential thread safety issues.

More information on Clang's thread safety analysis system can be found
`here <https://clang.llvm.org/docs/ThreadSafetyAnalysis.html>`_.

Enabling Clang's Analysis
-------------------------
In order to enable the analysis, Clang requires that the ``-Wthread-safety``
compilation flag be used. In addition, if any STL components like
``std::lock_guard`` are used, the STL's built in annotations have to be manually
enabled, typically by setting the ``_LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS``
macro.

If using GN, the ``pw_build:clang_thread_safety_warnings`` config is provided
to do this for you, when added to your clang toolchain definition's default
configs.

Why use lock annotations?
-------------------------
Lock annotations can help warn you about potential race conditions in your code
when using locks: you have to remember to grab lock(s) before entering a
critical section, yuou have to remember to unlock it when you leave, and you
have to avoid deadlocks.

Clang's lock annotations let you inform the compiler and anyone reading your
code which variables are guarded by which locks, which locks should or cannot be
held when calling which function, which order locks should be acquired in, etc.

Using Lock Annotations
----------------------
When referring to locks in the arguments of the attributes, you should
use variable names or more complex expressions (e.g. ``my_object->lock_``)
that evaluate to a concrete lock object whenever possible. If the lock
you want to refer to is not in scope, you may use a member pointer
(e.g. ``&MyClass::lock_``) to refer to a lock in some (unknown) object.

Annotating Lock Usage
^^^^^^^^^^^^^^^^^^^^^
.. cpp:function:: PW_GUARDED_BY(x)

  Documents if a shared field or global variable needs to be protected by a
  lock. ``PW_GUARDED_BY()`` allows the user to specify a particular lock that
  should be held when accessing the annotated variable.

  Although this annotation (and ``PW_PT_GUARDED_BY``, below) cannot be applied
  to local variables, a local variable and its associated lock can often be
  combined into a small class or struct, thereby allowing the annotation.

  Example:

  .. code-block:: cpp

    class Foo {
      Mutex mu_;
      int p1_ PW_GUARDED_BY(mu_);
      ...
    };

.. cpp:function:: PW_PT_GUARDED_BY(x)

  Documents if the memory location pointed to by a pointer should be guarded
  by a lock when dereferencing the pointer.

  Example:

  .. code-block:: cpp

    class Foo {
      Mutex mu_;
      int *p1_ PW_PT_GUARDED_BY(mu_);
      ...
    };

  Note that a pointer variable to a shared memory location could itself be a
  shared variable.

  Example:

  .. code-block:: cpp

    // `q_`, guarded by `mu1_`, points to a shared memory location that is
    // guarded by `mu2_`:
    int *q_ PW_GUARDED_BY(mu1_) PW_PT_GUARDED_BY(mu2_);

.. cpp:function:: PW_ACQUIRED_AFTER(...)
.. cpp:function:: PW_ACQUIRED_BEFORE(...)

  Documents the acquisition order between locks that can be held
  simultaneously by a thread. For any two locks that need to be annotated
  to establish an acquisition order, only one of them needs the annotation.
  (i.e. You don't have to annotate both locks with both ``PW_ACQUIRED_AFTER``
  and ``PW_ACQUIRED_BEFORE``.)

  As with ``PW_GUARDED_BY``, this is only applicable to locks that are shared
  fields or global variables.

  Example:

  .. code-block:: cpp

    Mutex m1_;
    Mutex m2_ PW_ACQUIRED_AFTER(m1_);

.. cpp:function:: PW_EXCLUSIVE_LOCKS_REQUIRED(...)
.. cpp:function:: PW_SHARED_LOCKS_REQUIRED(...)

  Documents a function that expects a lock to be held prior to entry.
  The lock is expected to be held both on entry to, and exit from, the
  function.

  An exclusive lock allows read-write access to the guarded data member(s), and
  only one thread can acquire a lock exclusively at any one time. A shared lock
  allows read-only access, and any number of threads can acquire a shared lock
  concurrently.

  Generally, non-const methods should be annotated with
  ``PW_EXCLUSIVE_LOCKS_REQUIRED``, while const methods should be annotated with
  ``PW_SHARED_LOCKS_REQUIRED``.

  Example:

  .. code-block:: cpp

    Mutex mu1, mu2;
    int a PW_GUARDED_BY(mu1);
    int b PW_GUARDED_BY(mu2);

    void foo() PW_EXCLUSIVE_LOCKS_REQUIRED(mu1, mu2) { ... }
    void bar() const PW_SHARED_LOCKS_REQUIRED(mu1, mu2) { ... }

.. cpp:function:: PW_LOCKS_EXCLUDED(...)

  Documents that the caller must not hold the given lock. This annotation is
  often used to prevent deadlocks. Pigweed's mutex implementation is not
  re-entrant, so a deadlock will occur if the function acquires the mutex a
  second time.

  Example:

  .. code-block:: cpp

    Mutex mu;
    int a PW_GUARDED_BY(mu);

    void foo() PW_LOCKS_EXCLUDED(mu) {
      mu.lock();
      ...
      mu.unlock();
    }

.. cpp:function:: PW_LOCK_RETURNED(...)

  Documents a function that returns a lock without acquiring it.  For example,
  a public getter method that returns a pointer to a private lock should
  be annotated with ``PW_LOCK_RETURNED``.

  Example:

  .. code-block:: cpp

    class Foo {
     public:
      Mutex* mu() PW_LOCK_RETURNED(mu) { return &mu; }

     private:
      Mutex mu;
    };

.. cpp:function:: PW_NO_LOCK_SAFETY_ANALYSIS()

   Turns off thread safety checking within the body of a particular function.
   This annotation is used to mark functions that are known to be correct, but
   the locking behavior is more complicated than the analyzer can handle.

Annotating Lock Objects
^^^^^^^^^^^^^^^^^^^^^^^
In order of lock usage annotation to work, the lock objects themselves need to
be annotated as well. In case you are providing your own lock or psuedo-lock
object, you can use the macros in this section to annotate it.

As an example we've annotated a Lock and a RAII ScopedLocker object for you, see
the macro documentation after for more details:

.. code-block:: cpp

  class PW_LOCKABLE("Lock") Lock {
   public:
    void Lock() PW_EXCLUSIVE_LOCK_FUNCTION();

    void ReaderLock() PW_SHARED_LOCK_FUNCTION();

    void Unlock() PW_UNLOCK_FUNCTION();

    void ReaderUnlock() PW_SHARED_TRYLOCK_FUNCTION();

    bool TryLock() PW_EXCLUSIVE_TRYLOCK_FUNCTION(true);

    bool ReaderTryLock() PW_SHARED_TRYLOCK_FUNCTION(true);

    void AssertHeld() PW_ASSERT_EXCLUSIVE_LOCK();

    void AssertReaderHeld() PW_ASSERT_SHARED_LOCK();
  };


  // Tag types for selecting a constructor.
  struct adopt_lock_t {} inline constexpr adopt_lock = {};
  struct defer_lock_t {} inline constexpr defer_lock = {};
  struct shared_lock_t {} inline constexpr shared_lock = {};

  class PW_SCOPED_LOCKABLE ScopedLocker {
    // Acquire lock, implicitly acquire *this and associate it with lock.
    ScopedLocker(Lock *lock) PW_EXCLUSIVE_LOCK_FUNCTION(lock)
        : lock_(lock), locked(true) {
      lock->Lock();
    }

    // Assume lock is held, implicitly acquire *this and associate it with lock.
    ScopedLocker(Lock *lock, adopt_lock_t) PW_EXCLUSIVE_LOCKS_REQUIRED(lock)
        : lock_(lock), locked(true) {}

    // Acquire lock in shared mode, implicitly acquire *this and associate it
    // with lock.
    ScopedLocker(Lock *lock, shared_lock_t) PW_SHARED_LOCK_FUNCTION(lock)
        : lock_(lock), locked(true) {
      lock->ReaderLock();
    }

    // Assume lock is held in shared mode, implicitly acquire *this and associate
    // it with lock.
    ScopedLocker(Lock *lock, adopt_lock_t, shared_lock_t)
        PW_SHARED_LOCKS_REQUIRED(lock) : lock_(lock), locked(true) {}

    // Assume lock is not held, implicitly acquire *this and associate it with
    // lock.
    ScopedLocker(Lock *lock, defer_lock_t) PW_LOCKS_EXCLUDED(lock)
        : lock_(lock), locked(false) {}

    // Release *this and all associated locks, if they are still held.
    // There is no warning if the scope was already unlocked before.
    ~ScopedLocker() PW_UNLOCK_FUNCTION() {
      if (locked)
        lock_->GenericUnlock();
    }

    // Acquire all associated locks exclusively.
    void Lock() PW_EXCLUSIVE_LOCK_FUNCTION() {
      lock_->Lock();
      locked = true;
    }

    // Try to acquire all associated locks exclusively.
    bool TryLock() PW_EXCLUSIVE_TRYLOCK_FUNCTION(true) {
      return locked = lock_->TryLock();
    }

    // Acquire all associated locks in shared mode.
    void ReaderLock() PW_SHARED_LOCK_FUNCTION() {
      lock_->ReaderLock();
      locked = true;
    }

    // Try to acquire all associated locks in shared mode.
    bool ReaderTryLock() PW_SHARED_TRYLOCK_FUNCTION(true) {
      return locked = lock_->ReaderTryLock();
    }

    // Release all associated locks. Warn on double unlock.
    void Unlock() PW_UNLOCK_FUNCTION() {
      lock_->Unlock();
      locked = false;
    }

    // Release all associated locks. Warn on double unlock.
    void ReaderUnlock() PW_UNLOCK_FUNCTION() {
      lock_->ReaderUnlock();
      locked = false;
    }

   private:
    Lock* lock_;
    bool locked_;
  };

.. cpp:function:: PW_LOCKABLE(name)

  Documents if a class/type is a lockable type (such as the ``pw::sync::Mutex``
  class). The name is used in the warning messages. This can also be useful on
  classes which have locking like semantics but aren't actually locks.

.. cpp:function:: PW_SCOPED_LOCKABLE()

  Documents if a class does RAII locking. The name is used in the warning
  messages.

  The constructor should use ``LOCK_FUNCTION()`` to specify the lock that is
  acquired, and the destructor should use ``UNLOCK_FUNCTION()`` with no
  arguments; the analysis will assume that the destructor unlocks whatever the
  constructor locked.

.. cpp:function:: PW_EXCLUSIVE_LOCK_FUNCTION()

  Documents functions that acquire a lock in the body of a function, and do
  not release it.

.. cpp:function:: PW_SHARED_LOCK_FUNCTION()

   Documents functions that acquire a shared (reader) lock in the body of a
   function, and do not release it.

.. cpp:function:: PW_UNLOCK_FUNCTION()

   Documents functions that expect a lock to be held on entry to the function,
   and release it in the body of the function.

.. cpp:function:: PW_EXCLUSIVE_TRYLOCK_FUNCTION(try_success)
.. cpp:function:: PW_SHARED_TRYLOCK_FUNCTION(try_success)

  Documents functions that try to acquire a lock, and return success or failure
  (or a non-boolean value that can be interpreted as a boolean).
  The first argument should be ``true`` for functions that return ``true`` on
  success, or ``false`` for functions that return `false` on success. The second
  argument specifies the lock that is locked on success. If unspecified, this
  lock is assumed to be ``this``.

.. cpp:function:: PW_ASSERT_EXCLUSIVE_LOCK()
.. cpp:function:: PW_ASSERT_SHARED_LOCK()

   Documents functions that dynamically check to see if a lock is held, and fail
   if it is not held.

-----------------------------
Critical Section Lock Helpers
-----------------------------

Virtual Lock Interfaces
=======================
Virtual lock interfaces can be useful when lock selection cannot be templated.

Why use virtual locks?
----------------------
Virtual locks enable depending on locks without templating implementation code
on the type, while retaining flexibility with respect to the concrete lock type.
Pigweed tries to avoid pushing policy on to users, and virtual locks are one way
to accomplish that without templating everything.

A case when virtual locks are useful is when the concrete lock type changes at
run time. For example, access to flash may be protected at run time by an
internal mutex, however at crash time we may want to switch to a no-op lock. A
virtual lock interface could be used here to minimize the code-size cost that
would occur otherwise if the flash driver were templated.

VirtualBasicLock
----------------
The ``VirtualBasicLock`` interface meets the
`BasicLockable <https://en.cppreference.com/w/cpp/named_req/BasicLockable>`_ C++
named requirement. Our critical section lock primitives offer optional virtual
versions, including:

* ``pw::sync::VirtualMutex``
* ``pw::sync::VirtualTimedMutex``
* ``pw::sync::VirtualInterruptSpinLock``

Borrowable
==========
The Borrowable is a helper construct that enables callers to borrow an object
which is guarded by a lock, enabling a containerized style of external locking.

Users who need access to the guarded object can ask to acquire a
``BorrowedPointer`` which permits access while the lock is held.

This class is compatible with locks which comply with
`BasicLockable <https://en.cppreference.com/w/cpp/named_req/BasicLockable>`_,
`Lockable <https://en.cppreference.com/w/cpp/named_req/Lockable>`_, and
`TimedLockable <https://en.cppreference.com/w/cpp/named_req/TimedLockable>`_
C++ named requirements.

By default the selected lock type is a ``pw::sync::VirtualBasicLockable``. If
this virtual interface is used, the templated lock parameter can be skipped.

External vs Internal locking
----------------------------
Before we explain why Borrowable is useful, it's important to understand the
trade-offs when deciding on using internal and/or external locking.

Internal locking is when the lock is hidden from the caller entirely and is used
internally to the API. For example:

.. code-block:: cpp

  class BankAccount {
   public:
    void Deposit(int amount) {
      std::lock_guard lock(mutex_);
      balance_ += amount;
    }

    void Withdraw(int amount) {
      std::lock_guard lock(mutex_);
      balance_ -= amount;
    }

    void Balance() const {
      std::lock_guard lock(mutex_);
      return balance_;
    }

   private:
    int balance_ PW_GUARDED_BY(mutex_);
    pw::sync::Mutex mutex_;
  };

Internal locking guarantees that any concurrent calls to its public member
functions don't corrupt an instance of that class. This is typically ensured by
having each member function acquire a lock on the object upon entry. This way,
for any instance, there can only be one member function call active at any
moment, serializing the operations.

One common issue that pops up is that member functions may have to call other
member functions which also require locks. This typically results in a
duplication of the public API into an internal mirror where the lock is already
held. This along with having to modify every thread-safe public member function
may results in an increased code size.

However, with the per-method locking approach, it is not possible to perform a
multi-method thread-safe transaction. For example, what if we only wanted to
withdraw money if the balance was high enough? With the current API there would
be a risk that money is withdrawn after we've checked the balance.

This is usually why external locking is used. This is when the lock is exposed
to the caller and may be used externally to the public API. External locking
can take may forms which may even include mixing internal and external locking.
In its most simplistic form it is an external lock used along side each
instance, e.g.:

.. code-block:: cpp

  class BankAccount {
   public:
    void Deposit(int amount) {
      balance_ += amount;
    }

    void Withdraw(int amount) {
      balance_ -= amount;
    }

    void Balance() const {
      return balance_;
    }

   private:
    int balance_;
  };

  pw::sync::Mutex bobs_account_mutex;
  BankAccount bobs_account PW_GUARDED_BY(bobs_account_mutex);

The lock is acquired before the bank account is used for a transaction. In
addition, we do not have to modify every public function and its trivial to
call other public member functions from a public member function. However, as
you can imagine instantiating and passing around the instances and their locks
can become error prone.

This is why ``Borrowable`` exists.

Why use Borrowable?
-------------------
``Borrowable`` offers code-size efficient way to enable external locking that is
easy and safe to use. It is effectively a container which holds references to a
protected instance and its lock which provides RAII-style access.

.. code-block:: cpp

  pw::sync::Mutex bobs_account_mutex;
  BankAccount bobs_account PW_GUARDED_BY(bobs_account_mutex);
  pw::sync::Borrowable<BankAccount, pw::sync::Mutex> bobs_acount(
      bobs_account, bobs_account_mutex);

This construct is useful when sharing objects or data which are transactional in
nature where making individual operations threadsafe is insufficient. See the
section on internal vs external locking tradeoffs above.

It can also offer a code-size and stack-usage efficient way to separate timeout
constraints between the acquiring of the shared object and timeouts used for the
shared object's API. For example, imagine you have an I2c bus which is used by
several threads and you'd like to specify an ACK timeout of 50ms. It'd be ideal
if the duration it takes to gain exclusive access to the I2c bus does not eat
into the ACK timeout you'd like to use for the transaction. Borrowable can help
you do exactly this if you provide access to the I2c bus through a
``Borrowable``.

C++
---
.. cpp:class:: template <typename GuardedType, typename Lock = pw::sync::VirtualBasicLockable> pw::sync::BorrowedPointer

  The BorrowedPointer is an RAII handle which wraps a pointer to a borrowed
  object along with a held lock which is guarding the object. When destroyed,
  the lock is released.

  This object is moveable, but not copyable.

  .. cpp:function:: GuardedType* operator->()

     Provides access to the borrowed object's members.

  .. cpp:function:: GuardedType& operator*()

     Provides access to the borrowed object directly.

     **Warning:** The member of pointer member access operator, operator->(), is
     recommended over this API as this is prone to leaking references. However,
     this is sometimes necessary.

     **Warning:** Be careful not to leak references to the borrowed object.

.. cpp:class:: template <typename GuardedType, typename Lock = pw::sync::VirtualBasicLockable> pw::sync::Borrowable

  .. cpp:function:: BorrowedPointer<GuardedType, Lock> acquire()

     Blocks indefinitely until the object can be borrowed. Failures are fatal.

  .. cpp:function:: std::optional<BorrowedPointer<GuardedType, Lock>> try_acquire()

     Tries to borrow the object in a non-blocking manner. Returns a
     BorrowedPointer on success, otherwise std::nullopt (nothing).

  .. cpp:function:: template <class Rep, class Period> std::optional<BorrowedPointer<GuardedType, Lock>> try_acquire_for(std::chrono::duration<Rep, Period> timeout)

     Tries to borrow the object. Blocks until the specified timeout has elapsed
     or the object has been borrowed, whichever comes first. Returns a
     BorrowedPointer on success, otherwise std::nullopt (nothing).

  .. cpp:function:: template <class Rep, class Period> std::optional<BorrowedPointer<GuardedType, Lock>> try_acquire_until(std::chrono::duration<Rep, Period> deadline)

     Tries to borrow the object. Blocks until the specified deadline has been
     reached or the object has been borrowed, whichever comes first. Returns a
     BorrowedPointer on success, otherwise std::nullopt (nothing).

Example in C++
^^^^^^^^^^^^^^

.. code-block:: cpp

  #include <chrono>

  #include "pw_bytes/span.h"
  #include "pw_i2c/initiator.h"
  #include "pw_status/try.h"
  #include "pw_status/result.h"
  #include "pw_sync/borrow.h"
  #include "pw_sync/mutex.h"

  class ExampleI2c : public pw::i2c::Initiator;

  pw::sync::VirtualMutex i2c_mutex;
  ExampleI2c i2c;
  pw::sync::Borrowable<ExampleI2c> borrowable_i2c(i2c, i2c_mutex);

  pw::Result<ConstByteSpan> ReadI2cData(ByteSpan buffer) {
    // Block indefinitely waiting to borrow the i2c bus.
    pw::sync::BorrowedPointer<ExampleI2c> borrowed_i2c =
        borrowable_i2c.acquire();

    // Execute a sequence of transactions to get the needed data.
    PW_TRY(borrowed_i2c->WriteFor(kFirstWrite, std::chrono::milliseconds(50)));
    PW_TRY(borrowed_i2c->WriteReadFor(kSecondWrite, buffer,
                                      std::chrono::milliseconds(10)));

    // Borrowed i2c pointer is returned when the scope exits.
    return buffer;
  }

InlineBorrowable
=================
``InlineBorrowable`` is a helper to simplify the common use case where an object
is wrapped in a ``Borrowable`` for its entire lifetime. The InlineBorrowable
owns the guarded object and the lock object.

InlineBorrowable has a separate parameter for the concrete lock type
that is instantiated and a (possibly virtual) lock interface type that is
referenced by users of the guarded object. The default lock is
``pw::sync::VirtualMutex`` and the default lock interface is
``pw::sync::VirtualBasicLockable``.

An InlineBorrowable is a Borrowable with the same guarded object and lock
interface types, and it can be passed directly to APIs that expect a Borrowable
reference.

Why use InlineBorrowable?
-------------------------
It is a safer and simpler way to guard an object for its entire lifetime. The
unguarded object is never exposed and doesn't need to be stored in a separate
variable or data member. The guarded object and its lock are guaranteed to have
the same lifetime, and the lock cannot be re-used for any other purpose.

Constructing objects in-place
-----------------------------
The guarded object and its lock are constructed in-place by the
InlineBorrowable, and any constructor parameters required by the object or
its lock must be passed through the InlineBorrowable constructor. There are
several ways to do this:

* Pass the parameters for the guarded object inline to the constructor. This is
  the recommended way to construct the object when the lock does not require any
  constructor parameters. Use the ``std::in_place`` marker to invoke the inline
  constructor.

  .. code-block:: cpp

    InlineBorrowable<Foo> foo(std::in_place, foo_arg1, foo_arg2);
    InlineBorrowable<std::array<int, 2>> foo_array(std::in_place, 1, 2);

* Pass the parameters inside tuples:

  .. code-block:: cpp

    InlineBorrowable<Foo> foo(std::forward_as_tuple(foo_arg1, foo_arg2));

    InlineBorrowable<Foo, MyLock> foo_lock(
        std::forward_as_tuple(foo_arg1, foo_arg2),
        std::forward_as_tuple(lock_arg1, lock_arg2));

  .. note:: This approach only supports list initialization starting with C++20.

* Use callables to construct the guarded object and lock object:

  .. code-block:: cpp

    InlineBorrowable<Foo> foo([&]{ return Foo{foo_arg1, foo_arg2}; });

    InlineBorrowable<Foo, MyLock> foo_lock(
        [&]{ return Foo{foo_arg1, foo_arg2}; }
        [&]{ return MyLock{lock_arg1, lock_arg2}; }

  .. note:: It is possible to construct and return objects that are not copyable
    or movable, thanks to mandatory copy ellision (return value optimization).

C++
---
.. cpp:class:: template <typename GuardedType, typename Lock = pw::sync::VirtualMutex, typename LockInterface = pw::sync::VirtualBasicLockable> InlineBorrowable

  Holds an instance of ``GuardedType`` and ``Lock``. Access to the members of
  ``GuardedType`` are protected by the ``Lock``.

  This class implements ``Borrowable<GuardedType, LockInterface>``.

  .. cpp:function:: template <typename... Args> constexpr explicit InlineBorrowable(std::in_place_t, Args&&... args)

    Construct the guarded object by providing its cosntructor parameters inline.
    The lock is constructed using its default constructor.

    This constructor supports list initialization for arrays, structs, and
    other objects such as ``std::array``.

  .. cpp:function:: template <typename... ObjectArgs, typename... LockArgs> constexpr explicit InlineBorrowable( std::tuple<ObjectArgs...>&& object_args, std::tuple<LockArgs...>&& lock_args)

    Construct the guarded object and lock by providing their construction
    parameters using separate tuples. The 2nd tuple can be ommitted to construct
    the lock using its default constructor.

  .. cpp:function:: template <typename ObjectConstructor, typename LockConstructor> constexpr explicit InlineBorrowable( const ObjectConstructor& object_ctor, const LockConstructor& lock_ctor)

    Construct the guarded object and lock by invoking the given callables. The
    2nd callable can be ommitted to construct the lock using its default
    constructor.

Example in C++
^^^^^^^^^^^^^^
.. code-block:: cpp

  #include <utility>

  #include "pw_bytes/span.h"
  #include "pw_i2c/initiator.h"
  #include "pw_status/result.h"
  #include "pw_sync/inline_borrowable.h"

  struct I2cOptions;

  class ExampleI2c : public pw::i2c::Initiator {
   public:
    ExampleI2c(int bus_id, I2cOptions options);
    // ...
  };

  int kBusId;
  I2cOptions opts;

  pw::sync::InlineBorrowable<ExampleI2c> i2c(std::in_place, kBusId, opts);

  pw::Result<ConstByteSpan> ReadI2cData(
    pw::sync::Borrowable<pw::i2c::Initiator>& initiator,
    ByteSpan buffer);

  pw::Result<ConstByteSpan> ReadData(ByteSpan buffer) {
    return ReadI2cData(i2c, buffer);
  }

--------------------
Signaling Primitives
--------------------
Native signaling primitives tend to vary more compared to critial section locks
across different platforms. For example, although common signaling primtives
like semaphores are in most if not all RTOSes and even POSIX, it was not in the
STL before C++20. Likewise many C++ developers are surprised that conditional
variables tend to not be natively supported on RTOSes. Although you can usually
build any signaling primitive based on other native signaling primitives, this
may come with non-trivial added overhead in ROM, RAM, and execution efficiency.

For this reason, Pigweed intends to provide some simpler signaling primitives
which exist to solve a narrow programming need but can be implemented as
efficiently as possible for the platform that it is used on.

This simpler but highly portable class of signaling primitives is intended to
ensure that a portability efficiency tradeoff does not have to be made up front.
Today this is class of simpler signaling primitives is limited to the
``pw::sync::ThreadNotification`` and ``pw::sync::TimedThreadNotification``.

ThreadNotification
==================
The ThreadNotification is a synchronization primitive that can be used to
permit a SINGLE thread to block and consume a latching, saturating
notification from multiple notifiers.

.. Note::
  Although only a single thread can block on a ThreadNotification at a time,
  many instances may be used by a single thread just like binary semaphores.
  This is in contrast to some native RTOS APIs, such as direct task
  notifications, which re-use the same state within a thread's context.

.. Warning::
  This is a single consumer/waiter, multiple producer/notifier API!
  The acquire APIs must only be invoked by a single consuming thread. As a
  result, having multiple threads receiving notifications via the acquire API
  is unsupported.

This is effectively a subset of the ``pw::sync::BinarySemaphore`` API, except
that only a single thread can be notified and block at a time.

The single consumer aspect of the API permits the use of a smaller and/or
faster native APIs such as direct thread signaling. This should be
backed by the most efficient native primitive for a target, regardless of
whether that is a semaphore, event flag group, condition variable, or something
else.

The ThreadNotification is initialized to being empty (latch is not set).

Generic BinarySemaphore-based Backend
-------------------------------------
This module provides a generic backend for ``pw::sync::ThreadNotification`` via
``pw_sync:binary_semaphore_thread_notification`` which uses a
``pw::sync::BinarySemaphore`` as the backing primitive. See
:ref:`BinarySemaphore <module-pw_sync-binary-semaphore>` for backend
availability.

Optimized Backend
-----------------
.. list-table::

  * - *Supported on*
    - *Optimized backend module*
  * - FreeRTOS
    - ``pw_sync_freertos:thread_notification``
  * - ThreadX
    - Not possible, use ``pw_sync:binary_semaphore_thread_notification``
  * - embOS
    - Not needed, use ``pw_sync:binary_semaphore_thread_notification``
  * - STL
    - Not planned, use ``pw_sync:binary_semaphore_thread_notification``
  * - Baremetal
    - Planned
  * - Zephyr
    - Planned
  * - CMSIS-RTOS API v2 & RTX5
    - Planned

C++
---
.. cpp:class:: pw::sync::ThreadNotification

  .. cpp:function:: void acquire()

     Blocks indefinitely until the thread is notified, i.e. until the
     notification latch can be cleared because it was set.

     Clears the notification latch.

     **IMPORTANT:** This should only be used by a single consumer thread.

  .. cpp:function:: bool try_acquire()

     Returns whether the thread has been notified, i.e. whether the notificion
     latch was set and resets the latch regardless.

     Clears the notification latch.

     Returns true if the thread was notified, meaning the the internal latch was
     reset successfully.

     **IMPORTANT:** This should only be used by a single consumer thread.

  .. cpp:function:: void release()

     Notifies the thread in a saturating manner, setting the notification latch.

     Raising the notification multiple time without it being acquired by the
     consuming thread is equivalent to raising the notification once to the
     thread. The notification is latched in case the thread was not waiting at
     the time.

     This is IRQ and thread safe.

  .. list-table::

    * - *Safe to use in context*
      - *Thread*
      - *Interrupt*
      - *NMI*
    * - ``ThreadNotification::ThreadNotification``
      - ✔
      -
      -
    * - ``ThreadNotification::~ThreadNotification``
      - ✔
      -
      -
    * - ``void ThreadNotification::acquire``
      - ✔
      -
      -
    * - ``bool ThreadNotification::try_acquire``
      - ✔
      -
      -
    * - ``void ThreadNotification::release``
      - ✔
      - ✔
      -

Examples in C++
^^^^^^^^^^^^^^^
.. code-block:: cpp

  #include "pw_sync/thread_notification.h"
  #include "pw_thread/thread_core.h"

  class FooHandler() : public pw::thread::ThreadCore {
   // Public API invoked by other threads and/or interrupts.
   void NewFooAvailable() {
     new_foo_notification_.release();
   }

   private:
    pw::sync::ThreadNotification new_foo_notification_;

    // Thread function.
    void Run() override {
      while (true) {
        new_foo_notification_.acquire();
        HandleFoo();
      }
    }

    void HandleFoo();
  }

TimedThreadNotification
=======================
The TimedThreadNotification is an extension of the ThreadNotification which
offers timeout and deadline based semantics.

The TimedThreadNotification is initialized to being empty (latch is not set).

.. Warning::
  This is a single consumer/waiter, multiple producer/notifier API!
  The acquire APIs must only be invoked by a single consuming thread. As a
  result, having multiple threads receiving notifications via the acquire API
  is unsupported.

Generic BinarySemaphore-based Backend
-------------------------------------
This module provides a generic backend for ``pw::sync::TimedThreadNotification``
via ``pw_sync:binary_semaphore_timed_thread_notification`` which uses a
``pw::sync::BinarySemaphore`` as the backing primitive. See
:ref:`BinarySemaphore <module-pw_sync-binary-semaphore>` for backend
availability.

Optimized Backend
-----------------
.. list-table::

  * - *Supported on*
    - *Backend module*
  * - FreeRTOS
    - ``pw_sync_freertos:timed_thread_notification``
  * - ThreadX
    - Not possible, use ``pw_sync:binary_semaphore_timed_thread_notification``
  * - embOS
    - Not needed, use ``pw_sync:binary_semaphore_timed_thread_notification``
  * - STL
    - Not planned, use ``pw_sync:binary_semaphore_timed_thread_notification``
  * - Zephyr
    - Planned
  * - CMSIS-RTOS API v2 & RTX5
    - Planned

C++
---
.. cpp:class:: pw::sync::TimedThreadNotification

  .. cpp:function:: void acquire()

     Blocks indefinitely until the thread is notified, i.e. until the
     notification latch can be cleared because it was set.

     Clears the notification latch.

     **IMPORTANT:** This should only be used by a single consumer thread.

  .. cpp:function:: bool try_acquire()

     Returns whether the thread has been notified, i.e. whether the notificion
     latch was set and resets the latch regardless.

     Clears the notification latch.

     Returns true if the thread was notified, meaning the the internal latch was
     reset successfully.

     **IMPORTANT:** This should only be used by a single consumer thread.

  .. cpp:function:: void release()

     Notifies the thread in a saturating manner, setting the notification latch.

     Raising the notification multiple time without it being acquired by the
     consuming thread is equivalent to raising the notification once to the
     thread. The notification is latched in case the thread was not waiting at
     the time.

     This is IRQ and thread safe.

  .. cpp:function:: bool try_acquire_for(chrono::SystemClock::duration timeout)

     Blocks until the specified timeout duration has elapsed or the thread
     has been notified (i.e. notification latch can be cleared because it was
     set), whichever comes first.

     Clears the notification latch.

     Returns true if the thread was notified, meaning the the internal latch was
     reset successfully.

     **IMPORTANT:** This should only be used by a single consumer thread.

  .. cpp:function:: bool try_acquire_until(chrono::SystemClock::time_point deadline)

     Blocks until the specified deadline time has been reached the thread has
     been notified (i.e. notification latch can be cleared because it was set),
     whichever comes first.

     Clears the notification latch.

     Returns true if the thread was notified, meaning the the internal latch was
     reset successfully.

     **IMPORTANT:** This should only be used by a single consumer thread.

  .. list-table::

    * - *Safe to use in context*
      - *Thread*
      - *Interrupt*
      - *NMI*
    * - ``TimedThreadNotification::TimedThreadNotification``
      - ✔
      -
      -
    * - ``TimedThreadNotification::~TimedThreadNotification``
      - ✔
      -
      -
    * - ``void TimedThreadNotification::acquire``
      - ✔
      -
      -
    * - ``bool TimedThreadNotification::try_acquire``
      - ✔
      -
      -
    * - ``bool TimedThreadNotification::try_acquire_for``
      - ✔
      -
      -
    * - ``bool TimedThreadNotification::try_acquire_until``
      - ✔
      -
      -
    * - ``void TimedThreadNotification::release``
      - ✔
      - ✔
      -

Examples in C++
^^^^^^^^^^^^^^^
.. code-block:: cpp

  #include "pw_sync/timed_thread_notification.h"
  #include "pw_thread/thread_core.h"

  class FooHandler() : public pw::thread::ThreadCore {
   // Public API invoked by other threads and/or interrupts.
   void NewFooAvailable() {
     new_foo_notification_.release();
   }

   private:
    pw::sync::TimedThreadNotification new_foo_notification_;

    // Thread function.
    void Run() override {
      while (true) {
        if (new_foo_notification_.try_acquire_for(kNotificationTimeout)) {
          HandleFoo();
        }
        DoOtherStuff();
      }
    }

    void HandleFoo();
    void DoOtherStuff();
  }

CountingSemaphore
=================
The CountingSemaphore is a synchronization primitive that can be used for
counting events and/or resource management where receiver(s) can block on
acquire until notifier(s) signal by invoking release.

Note that unlike Mutexes, priority inheritance is not used by semaphores meaning
semaphores are subject to unbounded priority inversions. Due to this, Pigweed
does not recommend semaphores for mutual exclusion.

The CountingSemaphore is initialized to being empty or having no tokens.

The entire API is thread safe, but only a subset is interrupt safe.

.. Note::
  If there is only a single consuming thread, we recommend using a
  ThreadNotification instead which can be much more efficient on some RTOSes
  such as FreeRTOS.

.. Warning::
  Releasing multiple tokens is often not natively supported, meaning you may
  end up invoking the native kernel API many times, i.e. once per token you
  are releasing!

.. list-table::

  * - *Supported on*
    - *Backend module*
  * - FreeRTOS
    - :ref:`module-pw_sync_freertos`
  * - ThreadX
    - :ref:`module-pw_sync_threadx`
  * - embOS
    - :ref:`module-pw_sync_embos`
  * - STL
    - :ref:`module-pw_sync_stl`
  * - Zephyr
    - Planned
  * - CMSIS-RTOS API v2 & RTX5
    - Planned

C++
---
.. cpp:class:: pw::sync::CountingSemaphore

  .. cpp:function:: void acquire()

     Decrements the internal counter by 1 or blocks indefinitely until it can.
     This is thread safe, but not IRQ safe.

  .. cpp:function:: bool try_acquire() noexcept

     Tries to decrement by the internal counter by 1 without blocking.
     Returns true if the internal counter was decremented successfully.
     This is thread and IRQ safe.

  .. cpp:function:: bool try_acquire_for(chrono::SystemClock::duration timeout)

     Tries to decrement the internal counter by 1. Blocks until the specified
     timeout has elapsed or the counter was decremented by 1, whichever comes
     first.
     Returns true if the internal counter was decremented successfully.
     This is thread safe, but not IRQ safe.

  .. cpp:function:: bool try_acquire_until(chrono::SystemClock::time_point deadline)

     Tries to decrement the internal counter by 1. Blocks until the specified
     deadline has been reached or the counter was decremented  by 1, whichever
     comes first.
     Returns true if the internal counter was decremented successfully.
     This is thread safe, but not IRQ safe.

  .. cpp:function:: void release(ptrdiff_t update = 1)

     Atomically increments the internal counter by the value of update.
     Any thread(s) waiting for the counter to be greater than 0, i.e.
     blocked in acquire, will subsequently be unblocked.
     This is thread and IRQ safe.

     **Precondition:** update >= 0

     **Precondition:** update <= max() - counter

  .. cpp:function:: static constexpr ptrdiff_t max() noexcept

     Returns the internal counter's maximum possible value.

  .. list-table::

    * - *Safe to use in context*
      - *Thread*
      - *Interrupt*
      - *NMI*
    * - ``CountingSemaphore::CountingSemaphore``
      - ✔
      -
      -
    * - ``CountingSemaphore::~CountingSemaphore``
      - ✔
      -
      -
    * - ``void CountingSemaphore::acquire``
      - ✔
      -
      -
    * - ``bool CountingSemaphore::try_acquire``
      - ✔
      - ✔
      -
    * - ``bool CountingSemaphore::try_acquire_for``
      - ✔
      -
      -
    * - ``bool CountingSemaphore::try_acquire_until``
      - ✔
      -
      -
    * - ``void CountingSemaphore::release``
      - ✔
      - ✔
      -
    * - ``void CountingSemaphore::max``
      - ✔
      - ✔
      - ✔

Examples in C++
^^^^^^^^^^^^^^^
As an example, a counting sempahore can be useful to run periodic tasks at
frequencies near or higher than the system clock tick rate in a way which lets
you detect whether you ever fall behind.

.. code-block:: cpp

  #include "pw_sync/counting_semaphore.h"
  #include "pw_thread/thread_core.h"

  class PeriodicWorker() : public pw::thread::ThreadCore {
   // Public API invoked by a higher frequency timer interrupt.
   void TimeToExecute() {
     periodic_run_semaphore_.release();
   }

   private:
    pw::sync::CountingSemaphore periodic_run_semaphore_;

    // Thread function.
    void Run() override {
      while (true) {
        size_t behind_by_n_cycles = 0;
        periodic_run_semaphore_.acquire(); // Wait to run until it's time.
        while (periodic_run_semaphore_.try_acquire()) {
          ++behind_by_n_cycles;
        }
        if (behind_by_n_cycles > 0) {
          PW_LOG_WARNING("Not keeping up, behind by %d cycles",
                         behind_by_n_cycles);
        }
        DoPeriodicWork();
      }
    }

    void DoPeriodicWork();
  }



.. _module-pw_sync-binary-semaphore:

BinarySemaphore
===============
BinarySemaphore is a specialization of CountingSemaphore with an arbitrary token
limit of 1. Note that that ``max()`` is >= 1, meaning it may be released up to
``max()`` times but only acquired once for those N releases.

Implementations of BinarySemaphore are typically more efficient than the
default implementation of CountingSemaphore.

The BinarySemaphore is initialized to being empty or having no tokens.

The entire API is thread safe, but only a subset is interrupt safe.

.. Note::
  If there is only a single consuming thread, we recommend using a
  ThreadNotification instead which can be much more efficient on some RTOSes
  such as FreeRTOS.


.. list-table::

  * - *Supported on*
    - *Backend module*
  * - FreeRTOS
    - :ref:`module-pw_sync_freertos`
  * - ThreadX
    - :ref:`module-pw_sync_threadx`
  * - embOS
    - :ref:`module-pw_sync_embos`
  * - STL
    - :ref:`module-pw_sync_stl`
  * - Zephyr
    - Planned
  * - CMSIS-RTOS API v2 & RTX5
    - Planned

C++
---
.. cpp:class:: pw::sync::BinarySemaphore

  .. cpp:function:: void acquire()

     Decrements the internal counter to 0 or blocks indefinitely until it can.
     This is thread safe, but not IRQ safe.

  .. cpp:function:: bool try_acquire() noexcept

     Tries to decrement by the internal counter to 0 without blocking.
     Returns true if the internal counter was decremented successfully.
     This is thread and IRQ safe.

  .. cpp:function:: bool try_acquire_for(chrono::SystemClock::duration timeout)

     Tries to decrement the internal counter to 0. Blocks until the specified
     timeout has elapsed or the counter was decremented to 0, whichever comes
     first.
     Returns true if the internal counter was decremented successfully.
     This is thread safe, but not IRQ safe.

  .. cpp:function:: bool try_acquire_until(chrono::SystemClock::time_point deadline)

     Tries to decrement the internal counter to 0. Blocks until the specified
     deadline has been reached or the counter was decremented to 0, whichever
     comes first.
     Returns true if the internal counter was decremented successfully.
     This is thread safe, but not IRQ safe.

  .. cpp:function:: void release()

     Atomically increments the internal counter by 1.
     Any thread(s) waiting for the counter to be greater than 0, i.e.
     blocked in acquire, will subsequently be unblocked.
     This is thread and IRQ safe.

     There exists an overflow risk if one releases more than max() times
     between acquires because many RTOS implementations internally
     increment the counter past one where it is only cleared when acquired.

     **Precondition:** 1 <= max() - counter

  .. cpp:function:: static constexpr ptrdiff_t max() noexcept

     Returns the internal counter's maximum possible value.

  .. list-table::

    * - *Safe to use in context*
      - *Thread*
      - *Interrupt*
      - *NMI*
    * - ``BinarySemaphore::BinarySemaphore``
      - ✔
      -
      -
    * - ``BinarySemaphore::~BinarySemaphore``
      - ✔
      -
      -
    * - ``void BinarySemaphore::acquire``
      - ✔
      -
      -
    * - ``bool BinarySemaphore::try_acquire``
      - ✔
      - ✔
      -
    * - ``bool BinarySemaphore::try_acquire_for``
      - ✔
      -
      -
    * - ``bool BinarySemaphore::try_acquire_until``
      - ✔
      -
      -
    * - ``void BinarySemaphore::release``
      - ✔
      - ✔
      -
    * - ``void BinarySemaphore::max``
      - ✔
      - ✔
      - ✔

Examples in C++
^^^^^^^^^^^^^^^
.. code-block:: cpp

  #include "pw_sync/binary_semaphore.h"
  #include "pw_thread/thread_core.h"

  class FooHandler() : public pw::thread::ThreadCore {
   // Public API invoked by other threads and/or interrupts.
   void NewFooAvailable() {
     new_foo_semaphore_.release();
   }

   private:
    pw::sync::BinarySemaphore new_foo_semaphore_;

    // Thread function.
    void Run() override {
      while (true) {
        if (new_foo_semaphore_.try_acquire_for(kNotificationTimeout)) {
          HandleFoo();
        }
        DoOtherStuff();
      }
    }

    void HandleFoo();
    void DoOtherStuff();
  }

Conditional Variables
=====================
``pw::sync::ConditionVariable`` provides a condition variable implementation
that provides semantics and an API very similar to `std::condition_variable
<https://en.cppreference.com/w/cpp/thread/condition_variable>`_ in the C++
Standard Library.
