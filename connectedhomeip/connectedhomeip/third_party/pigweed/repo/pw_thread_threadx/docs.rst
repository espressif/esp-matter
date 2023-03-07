.. _module-pw_thread_threadx:

=================
pw_thread_threadx
=================
This is a set of backends for pw_thread based on ThreadX.

.. Warning::
  This module is still under construction, the API is not yet stable.

-----------------------
Thread Creation Backend
-----------------------
A backend for ``pw::thread::Thread`` is offered using ``tx_thread_create``.
Optional joining support is enabled via an ``TX_EVENT_FLAGS_GROUP`` in each
thread's context.

This backend permits users to start threads where contexts must be explicitly
allocated and passed in as an option. As a quick example, a detached thread can
be created as follows:

.. code-block:: cpp

  #include "pw_thread/detached_thread.h"
  #include "pw_thread_threadx/config.h"
  #include "pw_thread_threadx/context.h"
  #include "pw_thread_threadx/options.h"
  #include "tx_api.h"

  constexpr UINT kFooPriority =
      pw::thread::threadx::config::kDefaultPriority;
  constexpr ULONG kFooTimeSliceInterval =
      pw::thread::threadx::config::kDefaultTimeSliceInterval;
  constexpr size_t kFooStackSizeWords =
      pw::thread::threadx::config::kDefaultStackSizeWords;

  pw::thread::threadx::ContextWithStack<kFooStackSizeWords>
      example_thread_context;
  void StartExampleThread() {
    pw::thread::DetachedThread(
        pw::thread::threadx::Options()
            .set_name("example_thread")
            .set_priority(kFooPriority)
            .set_time_slice_interval(kFooTimeSliceInterval)
            .set_context(example_thread_context),
        example_thread_function);
  }

.. list-table::

  * - :ref:`module-pw_thread` Facade
    - Backend Target
    - Description
  * - ``pw_thread:id``
    - ``pw_thread_threadx:id``
    - Thread identification.
  * - ``pw_thread:yield``
    - ``pw_thread_threadx:yield``
    - Thread scheduler yielding.
  * - ``pw_thread:sleep``
    - ``pw_thread_threadx:sleep``
    - Thread scheduler sleeping.
  * - ``pw_thread:thread``
    - ``pw_thread_threadx:thread``
    - Thread creation.

Module Configuration Options
============================
The following configurations can be adjusted via compile-time configuration of
this module, see the
:ref:`module documentation <module-structure-compile-time-configuration>` for
more details.

.. c:macro:: PW_THREAD_THREADX_CONFIG_JOINING_ENABLED

  Whether thread joining is enabled. By default this is disabled.

  We suggest only enabling this when thread joining is required to minimize
  the RAM and ROM cost of threads.

  Enabling this grows the RAM footprint of every pw::thread::Thread as it adds
  a TX_EVENT_FLAGS_GROUP to every thread's pw::thread::threadx::Context. In
  addition, there is a minute ROM cost to construct and destroy this added
  object.

  PW_THREAD_JOINING_ENABLED gets set to this value.

.. c:macro:: PW_THREAD_THREADX_CONFIG_DEFAULT_STACK_SIZE_WORDS

  The default stack size in words. By default this uses the minimal ThreadX
  stack size.

.. c:macro:: PW_THREAD_THREADX_CONFIG_MAX_THREAD_NAME_LEN

  The maximum length of a thread's name, not including null termination. By
  default this is arbitrarily set to 15. This results in an array of characters
  which is this length + 1 bytes in every pw::thread::Thread's context.

.. c:macro:: PW_THREAD_THREADX_CONFIG_DEFAULT_TIME_SLICE_INTERVAL

  The round robin time slice tick interval for threads at the same priority.
  By default this is disabled as not all ports support this, using a value of 0
  ticks.

.. c:macro:: PW_THREAD_THREADX_CONFIG_MIN_PRIORITY

  The minimum priority level, this is normally based on the number of priority
  levels.

.. c:macro:: PW_THREAD_THREADX_CONFIG_DEFAULT_PRIORITY

  The default priority level. By default this uses the minimal ThreadX
  priority level, given that 0 is the highest priority.

.. c:macro:: PW_THREAD_THREADX_CONFIG_LOG_LEVEL

  The log level to use for this module. Logs below this level are omitted.

ThreadX Thread Options
======================
.. cpp:class:: pw::thread::threadx::Options

  .. cpp:function:: set_name(const char* name)

     Sets the name for the ThreadX thread, note that this will be deep copied
     into the context and may be truncated based on
     ``PW_THREAD_THREADX_CONFIG_MAX_THREAD_NAME_LEN``.

  .. cpp:function:: set_priority(UINT priority)

     Sets the priority for the ThreadX thread from 0 through 31, where a value
     of 0 represents the highest priority, see ThreadX tx_thread_create for
     more detail.

     **Precondition**: priority <= ``PW_THREAD_THREADX_CONFIG_MIN_PRIORITY``.

  .. cpp:function:: set_preemption_threshold(UINT preemption_threshold)

     Optionally sets the preemption threshold for the ThreadX thread from 0
     through 31.

     Only priorities higher than this level (i.e. lower number) are allowed to
     preempt this thread. In other words this allows the thread to specify the
     priority ceiling for disabling preemption. Threads that have a higher
     priority than the ceiling are still allowed to preempt while those with
     less than the ceiling are not allowed to preempt.

     Not setting the preemption threshold or explicitly specifying a value
     equal to the priority disables preemption threshold.

     Time slicing is disabled while the preemption threshold is enabled, i.e.
     not equal to the priority, even if a time slice interval was specified.

     The preemption threshold can be adjusted at run time, this only sets the
     initial threshold.

     **Precondition**: preemption_threshold <= priority

  .. cpp:function:: set_time_slice_interval(UINT time_slice_interval)

     Sets the number of ticks this thread is allowed to run before other ready
     threads of the same priority are given a chance to run.

     Time slicing is disabled while the preemption threshold is enabled, i.e.
     not equal to the priority, even if a time slice interval was specified.

     A value of ``TX_NO_TIME_SLICE`` (a value of 0) disables time-slicing of
     this thread.

     Using time slicing results in a slight amount of system overhead, threads
     with a unique priority should consider ``TX_NO_TIME_SLICE``.


  .. cpp:function:: set_context(pw::thread::embos::Context& context)

     Set the pre-allocated context (all memory needed to run a thread). Note
     that this is required for this thread creation backend! The Context can
     either be constructed with an externally provided ``pw::span<ULONG>``
     stack or the templated form of ``ContextWihtStack<kStackSizeWords`` can be
     used.

-----------------------------
Thread Identification Backend
-----------------------------
A backend for ``pw::thread::Id`` and ``pw::thread::get_id()`` is offerred using
``tx_thread_identify()``. It uses ``DASSERT`` to ensure that a thread is
executing via ``TX_THREAD_GET_SYSTEM_STATE()``.

--------------------
Thread Sleep Backend
--------------------
A backend for ``pw::thread::sleep_for()`` and ``pw::thread::sleep_until()`` is
offerred using ``tx_thread_sleep()`` if the duration is at least one tick, else
``tx_thread_relinquish()`` is used. It uses
``pw::this_thread::get_id() != thread::Id()`` to ensure it invoked only from a
thread.

--------------------
Thread Yield Backend
--------------------
A backend for ``pw::thread::yield()`` is offered using via
``tx_thread_relinquish()``. It uses
``pw::this_thread::get_id() != thread::Id()`` to ensure it invoked only from a
thread.

---------
Utilities
---------
``ForEachThread()``
===================
In cases where an operation must be performed for every thread,
``ForEachThread()`` can be used to iterate over all the created thread TCBs.
Note that it's only safe to use this while the scheduler is disabled.

An ``Aborted`` error status is returned if the provided callback returns
``false`` to request an early termination of thread iteration.

*Return values*

* ``Aborted``: The callback requested an early-termination of thread iteration.
* ``OkStatus``: The callback has been successfully run with every thread.

--------------------
Snapshot integration
--------------------
This ``pw_thread`` backend provides helper functions that capture ThreadX thread
state to a ``pw::thread::Thread`` proto.

``SnapshotThreads()``
=====================
``SnapshotThread()`` captures the thread name, state, and stack information for
the provided ThreadX TCB to a ``pw::thread::Thread`` protobuf encoder. To ensure
the most up-to-date information is captured, the stack pointer for the currently
running thread must be provided for cases where the running thread is being
captured. For ARM Cortex-M CPUs, you can do something like this:

.. Code:: cpp

  // Capture PSP.
  void* stack_ptr = 0;
  asm volatile("mrs %0, psp\n" : "=r"(stack_ptr));
  pw::thread::ProcessThreadStackCallback cb =
      [](pw::thread::proto::Thread::StreamEncoder& encoder,
         pw::ConstByteSpan stack) -> pw::Status {
    return encoder.WriteRawStack(stack);
  };
  pw::thread::threadx::SnapshotThread(my_thread, stack_ptr,
                                      snapshot_encoder, cb);

``SnapshotThreads()`` wraps the singular thread capture to instead captures
all created threads to a ``pw::thread::proto::SnapshotThreadInfo`` message.
This proto message overlays a snapshot, so it is safe to static cast a
``pw::snapshot::Snapshot::StreamEncoder`` to a
``pw::thread::proto::SnapshotThreadInfo::StreamEncoder`` when calling this
function.
