.. _module-pw_thread_freertos:

==================
pw_thread_freertos
==================
This is a set of backends for pw_thread based on FreeRTOS.

.. Warning::
  This module is still under construction, the API is not yet stable.

-----------------------
Thread Creation Backend
-----------------------
A backend for ``pw::thread::Thread`` is offered using ``xTaskCreateStatic()``.
Optional dynamic allocation for threads is supported using ``xTaskCreate()``.
Optional joining support is enabled via an ``StaticEventGroup_t`` in each
thread's context.

This backend always permits users to start threads where static contexts are
passed in as an option. As a quick example, a detached thread can be created as
follows:

.. code-block:: cpp

  #include "FreeRTOS.h"
  #include "pw_thread/detached_thread.h"
  #include "pw_thread_freertos/config.h"
  #include "pw_thread_freertos/context.h"
  #include "pw_thread_freertos/options.h"

  constexpr UBaseType_t kFooPriority =
      pw::thread::freertos::config::kDefaultPriority;
  constexpr size_t kFooStackSizeWords =
      pw::thread::freertos::config::kDefaultStackSizeWords;

  pw::thread::freertos::StaticContextWithStack<kFooStackSizeWords>
      example_thread_context;
  void StartExampleThread() {
    pw::thread::DetachedThread(
        pw::thread::freertos::Options()
            .set_name("static_example_thread")
            .set_priority(kFooPriority)
            .set_static_context(example_thread_context),
        example_thread_function);
  }

Alternatively when ``PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED`` is
enabled, dynamic thread allocation can be used. The above example could instead
be done as follows:

.. code-block:: cpp

  #include "FreeRTOS.h"
  #include "pw_thread/detached_thread.h"
  #include "pw_thread_freertos/config.h"
  #include "pw_thread_freertos/context.h"
  #include "pw_thread_freertos/options.h"

  constexpr UBaseType_t kFooPriority =
      pw::thread::freertos::config::kDefaultPriority;
  constexpr size_t kFooStackSizeWords =
      pw::thread::freertos::config::kDefaultStackSizeWords;

  void StartExampleThread() {
    pw::thread::DetachedThread(
        pw::thread::freertos::Options()
            .set_name("dyanmic_example_thread")
            .set_priority(kFooPriority)
            .set_stack_size(kFooStackSizeWords),
        example_thread_function)
  }


Module Configuration Options
============================
The following configurations can be adjusted via compile-time configuration of
this module, see the
:ref:`module documentation <module-structure-compile-time-configuration>` for
more details.

.. c:macro:: PW_THREAD_FREERTOS_CONFIG_JOINING_ENABLED

  Whether thread joining is enabled. By default this is disabled.

  We suggest only enabling this when thread joining is required to minimize
  the RAM and ROM cost of threads.

  Enabling this grows the RAM footprint of every ``pw::thread::Thread`` as it
  adds a ``StaticEventGroup_t`` to every thread's
  ``pw::thread::freertos::Context``. In addition, there is a minute ROM cost to
  construct and destroy this added object.

  ``PW_THREAD_JOINING_ENABLED`` gets set to this value.

.. c:macro:: PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED

  Whether dynamic allocation for threads (stacks and contexts) is enabled. By
  default this matches the FreeRTOS configuration on whether dynamic
  allocations are enabled. Note that static contexts **must** be provided if
  dynamic allocations are disabled.

.. c:macro:: PW_THREAD_FREERTOS_CONFIG_DEFAULT_STACK_SIZE_WORDS

   The default stack size in words. By default this uses the minimal FreeRTOS
   stack size based on ``configMINIMAL_STACK_SIZE``.

.. c:macro:: PW_THREAD_FREERTOS_CONFIG_DEFAULT_PRIORITY

   The default thread priority. By default this uses the minimal FreeRTOS
   priority level above the idle priority (``tskIDLE_PRIORITY + 1``).

.. c:macro:: PW_THREAD_FREERTOS_CONFIG_MAXIMUM_PRIORITY

  The maximum thread priority. By default this uses the value below the
  number of priorities defined by the FreeRTOS configuration
  (``configMAX_PRIORITIES - 1``).

.. c:macro:: PW_THREAD_FREERTOS_CONFIG_LOG_LEVEL

  The log level to use for this module. Logs below this level are omitted.

FreeRTOS Thread Options
=======================
.. cpp:class:: pw::thread::freertos::Options

  .. cpp:function:: set_name(const char* name)

    Sets the name for the FreeRTOS task, note that this will be truncated
    based on ``configMAX_TASK_NAME_LEN``. This is deep copied by FreeRTOS into
    the task's task control block (TCB).

  .. cpp:function:: set_priority(UBaseType_t priority)

    Sets the priority for the FreeRTOS task. This must be a value between
    ``0`` to ``PW_THREAD_FREERTOS_CONFIG_MAXIMUM_PRIORITY``. Higher priority
    values have a higher priority.

    Note that the idle task priority, ``tskIDLE_PRIORITY``, is fixed to ``0``.
    See the `FreeRTOS documentation on the idle task
    <https://www.freertos.org/RTOS-idle-task.html>`_ for more details.

    Precondition: This must be <= PW_THREAD_FREERTOS_CONFIG_MAXIMUM_PRIORITY.

  .. cpp:function:: set_stack_size(size_t size_words)

    Set the stack size in words for a dynamically thread.

    This is only available if
    ``PW_THREAD_FREERTOS_CONFIG_DYNAMIC_ALLOCATION_ENABLED`` is enabled.

    Precondition: size_words must be >= ``configMINIMAL_STACK_SIZE``

  .. cpp:function:: set_static_context(pw::thread::freertos::Context& context)

    Set the pre-allocated context (all memory needed to run a thread). The
    ``StaticContext`` can either be constructed with an externally provided
    ``pw::span<StackType_t>`` stack or the templated form of
    ``StaticContextWithStack<kStackSizeWords>`` can be used.


-----------------------------
Thread Identification Backend
-----------------------------
A backend for ``pw::thread::Id`` and ``pw::thread::get_id()`` is offered using
``xTaskGetCurrentTaskHandle()``. It uses ``DASSERT`` to ensure that it is not
invoked from interrupt context and if possible that the scheduler has started
via ``xTaskGetSchedulerState()``.

------------------------
Thread Iteration Backend
------------------------
``pw_thread_freertos_TSKTCB_BACKEND`` to be configured
properly and ``pw_third_party_freertos_DISABLE_TASKS_STATICS`` to be enabled.
To allow for peak stack usage measurement, the FreeRTOS config
``INCLUDE_uxTaskGetStackHighWaterMark`` should also be enabled.

--------------------
Thread Sleep Backend
--------------------
A backend for ``pw::thread::sleep_for()`` and ``pw::thread::sleep_until()`` is
offerred using ``vTaskDelay()`` if the duration is at least one tick, else
``taskYIELD()`` is used. It uses ``pw::this_thread::get_id() != thread::Id()``
to ensure it invoked only from a thread.

--------------------
Thread Yield Backend
--------------------
A backend for ``pw::thread::yield()`` is offered using via ``taskYIELD()``.
It uses ``pw::this_thread::get_id() != thread::Id()`` to ensure it invoked only
from a thread.

---------
Utilities
---------
``ForEachThread()``
===================
In cases where an operation must be performed for every thread,
``ForEachThread()`` can be used to iterate over all the created thread TCBs.
Note that it's only safe to use this while the scheduler and interrupts are
disabled.

Calling this before the scheduler has started, via ``vTaskStartScheduler()``, is
non-fatal but will result in no action and a ``FailedPrecondition`` error code.

An ``Aborted`` error status is returned if the provided callback returns
``false`` to request an early termination of thread iteration.

*Return values*

* ``FailedPrecondition``: Returned when ``ForEachThread()`` is run before the OS
  has been initialized.
* ``Aborted``: The callback requested an early-termination of thread iteration.
* ``OkStatus``: The callback has been successfully run with every thread.

.. Note:: This uses an unsupported method to iterate the threads in a more
   efficient manner while also supporting interrupt contexts. This requires
   linking against internal statics from the FreeRTOS kernel,
   :ref:`pw_third_party_freertos_DISABLE_TASKS_STATICS <third_party-freertos_disable_task_statics>`
   must be used.

--------------------
Snapshot integration
--------------------
This ``pw_thread`` backend provides helper functions that capture FreeRTOS
thread state to a ``pw::thread::Thread`` proto.

FreeRTOS tskTCB facade
======================
Unfortunately FreeRTOS entirely hides the contents of the TCB inside of
``Source/tasks.c``, but it's necessary for snapshot processing in order to
access the stack limits from interrupt contexts. For this reason, FreeRTOS
snapshot integration relies on the ``pw_thread_freertos:freertos_tsktcb`` facade
to provide the ``tskTCB`` definition. By default, a header will automatically be
generated from FreeRTOS's ``tasks.c`` file to work around this limitation.

In the event that the automatic header generation is incompatible with your
version of FreeRTOS, ``pw_thread_freertos_FREERTOS_TSKTCB_BACKEND`` must be
configured to point to a source set that provides the ``struct tskTCB``
definition through ``pw_thread_freertos_backend/freertos_tsktcb.h``. The facade
asserts that this definition matches the size of FreeRTOS's ``StaticTask_T``
which is the public opaque TCB type.

``SnapshotThreads()``
=====================
``SnapshotThread()`` captures the thread name, state, and stack information for
the provided TCB to a ``pw::thread::Thread`` protobuf encoder. To ensure
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
  pw::thread::threadx::SnapshotThread(my_thread, thread_state, stack_ptr,
                                      snapshot_encoder, cb);

``SnapshotThreads()`` wraps the singular thread capture to instead captures
all created threads to a ``pw::thread::proto::SnapshotThreadInfo`` message
which also captures the thread state for you. This proto
message overlays a snapshot, so it is safe to static cast a
``pw::snapshot::Snapshot::StreamEncoder`` to a
``pw::thread::proto::SnapshotThreadInfo::StreamEncoder`` when calling this
function.

.. Note:: ``SnapshotThreads()`` is only safe to use this while the scheduler and
   interrupts are disabled as it relies on ``ForEachThread()``.

Thread Stack Capture
--------------------
Snapshot attempts to capture as much of the thread stack state as possible,
however it can be limited by the FreeRTOS configuration.

The ``stack_start_ptr`` can only be provided if the ``portSTACK_GROWTH`` is < 0,
i.e. the stack grows down, when ``configRECORD_STACK_HIGH_ADDRESS`` is enabled.

The ``stack_pointer_est_peak`` can only be provided when
``config_USE_TRACE_FACILITY`` and/or ``INCLUDE_uxTaskGetStackHighWaterMark`` are
enabled and ``stack_start_ptr``'s requirements above are met.
