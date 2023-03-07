.. _module-pw_thread:

=========
pw_thread
=========
The ``pw_thread`` module contains utilities for thread creation and thread
execution.

.. Warning::
  This module is still under construction, the API is not yet stable.

---------------
Thread Sleeping
---------------
C++
===
.. cpp:function:: void pw::this_thread::sleep_for(chrono::SystemClock::duration sleep_duration)

   Blocks the execution of the current thread for at least the specified
   duration. This function may block for longer due to scheduling or resource
   contention delays.

   A sleep duration of 0 will at minimum yield, meaning it will provide a hint
   to the implementation to reschedule the execution of threads, allowing other
   threads to run.

   **Precondition:** This can only be called from a thread, meaning the
   scheduler is running.

.. cpp:function:: void pw::this_thread::sleep_until(chrono::SystemClock::time_point wakeup_time)

   Blocks the execution of the current thread until at least the specified
   time has been reached. This function may block for longer due to scheduling
   or resource contention delays.

   A sleep deadline in the past up to the current time will at minimum yield
   meaning it will provide a hint to the implementation to reschedule the
   execution of threads, allowing other threads to run.

   **Precondition:** This can only be called from a thread, meaning the
   scheduler is running.

Examples in C++
---------------
.. code-block:: cpp

  #include <chrono>

  #include "pw_chrono/system_clock.h"
  #include "pw_thread/sleep.h"

  using std::literals::chrono_literals::ms;

  void FunctionInvokedByThread() {
    pw::this_thread::sleep_for(42ms);
  }

  void AnotherFunctionInvokedByThread() {
    pw::this_thread::sleep_until(pw::chrono::SystemClock::now() + 42ms);
  }

C
=
.. cpp:function:: void pw_this_thread_SleepFor(pw_chrono_SystemClock_Duration sleep_duration)

   Invokes ``pw::this_thread::sleep_until(sleep_duration)``.

.. cpp:function:: void pw_this_thread_SleepUntil(pw_chrono_SystemClock_TimePoint wakeup_time)

   Invokes ``pw::this_thread::sleep_until(wakeup_time)``.


---------------
Thread Yielding
---------------
C++
===
.. cpp:function:: void pw::this_thread::yield() noexcept

   Provides a hint to the implementation to reschedule the execution of threads,
   allowing other threads to run.

   The exact behavior of this function depends on the implementation, in
   particular on the mechanics of the OS scheduler in use and the state of the
   system.

   **Precondition:** This can only be called from a thread, meaning the
   scheduler is running.

Example in C++
---------------
.. code-block:: cpp

  #include "pw_thread/yield.h"

  void FunctionInvokedByThread() {
    pw::this_thread::yield();
  }

C
=
.. cpp:function:: void pw_this_thread_Yield(void)

   Invokes ``pw::this_thread::yield()``.

---------------------
Thread Identification
---------------------
The class ``pw::thread::Id`` is a lightweight, trivially copyable class that
serves as a unique identifier of Thread objects.

Instances of this class may also hold the special distinct value that does
not represent any thread. Once a thread has finished, the value of its
Thread::id may be reused by another thread.

This class is designed for use as key in associative containers, both ordered
and unordered.

Although the current API is similar to C++11 STL
`std::thread::id <https://en.cppreference.com/w/cpp/thread/thread/id>`_, it is
missing the required hashing and streaming operators and may diverge further in
the future.

A thread's identification (``pw::thread::Id``) can be acquired only in C++ in
one of two ways:

1) Using the ``pw::thread::Thread`` handle's ``pw::thread::Id get_id() const``
   method.
2) While executing the thread using
   ``pw::thread::Id pw::this_thread::get_id() noexcept``.

.. cpp:function:: pw::thread::Id pw::this_thread::get_id() noexcept

   This is thread safe, not IRQ safe. It is implementation defined whether this
   is safe before the scheduler has started.


Example
=======
.. code-block:: cpp

  #include "pw_thread/id.h"

  void FunctionInvokedByThread() {
    const pw::thread::Id my_id = pw::this_thread::get_id();
  }


.. _module-pw_thread-thread-creation:

---------------
Thread Creation
---------------
The class ``pw::thread::Thread`` can represent a single thread of execution.
Threads allow multiple functions to execute concurrently.

The Thread's API is C++11 STL
`std::thread <https://en.cppreference.com/w/cpp/thread/thread>`_ like, meaning
the object is effectively a thread handle and not an object which contains the
thread's context. Unlike ``std::thread``, the API requires
``pw::thread::Options`` as an argument and is limited to only work with
``pw::thread::ThreadCore`` objects and functions which match the
``pw::thread::Thread::ThreadRoutine`` signature.

We recognize that the C++11's STL ``std::thread``` API has some drawbacks where
it is easy to forget to join or detach the thread handle. Because of this, we
offer helper wrappers like the ``pw::thread::DetachedThread``. Soon we will
extend this by also adding a ``pw::thread::JoiningThread`` helper wrapper which
will also have a lighter weight C++20 ``std::jthread`` like cooperative
cancellation contract to make joining safer and easier.

Threads may begin execution immediately upon construction of the associated
thread object (pending any OS scheduling delays), starting at the top-level
function provided as a constructor argument. The return value of the
top-level function is ignored. The top-level function may communicate its
return value by modifying shared variables (which may require
synchronization, see :ref:`module-pw_sync`)

Thread objects may also be in the state that does not represent any thread
(after default construction, move from, detach, or join), and a thread of
execution may be not associated with any thread objects (after detach).

No two Thread objects may represent the same thread of execution; Thread is
not CopyConstructible or CopyAssignable, although it is MoveConstructible and
MoveAssignable.

.. list-table::

  * - *Supported on*
    - *Backend module*
  * - FreeRTOS
    - :ref:`module-pw_thread_freertos`
  * - ThreadX
    - :ref:`module-pw_thread_threadx`
  * - embOS
    - :ref:`module-pw_thread_embos`
  * - STL
    - :ref:`module-pw_thread_stl`
  * - Zephyr
    - Planned
  * - CMSIS-RTOS API v2 & RTX5
    - Planned

Module Configuration Options
============================
The following configurations can be adjusted via compile-time configuration of
this module, see the
:ref:`module documentation <module-structure-compile-time-configuration>` for
more details.

.. c:macro:: PW_THREAD_CONFIG_LOG_LEVEL

  The log level to use for this module. Logs below this level are omitted.

Options
=======
The ``pw::thread::Options`` contains the parameters or attributes needed for a
thread to start.

Pigweed does not generalize options, instead we strive to give you full control
where we provide helpers to do this.

Options are backend specific and ergo the generic base class cannot be
directly instantiated.

The attributes which can be set through the options are backend specific
but may contain things like the thread name, priority, scheduling policy,
core/processor affinity, and/or an optional reference to a pre-allocated
Context (the collection of memory allocations needed for a thread to run).

Options shall NOT have an attribute to start threads as detached vs joinable.
All ``pw::thread::Thread`` instances must be explicitly ``join()``'d or
``detach()``'d through the run-time Thread API.

Note that if backends set ``PW_THREAD_JOINING_ENABLED`` to false, backends
may use native OS specific APIs to create native detached threads because the
``join()`` API would be compiled out. However, users must still explicitly
invoke ``detach()``.

Options must not contain any memory needed for a thread to run (TCB,
stack, etc.). The Options may be deleted or re-used immediately after
starting a thread.

Options subclass must contain non-default explicit constructor (parametrized or
not), e.g. ``constexpr Options() {}``. It is not enough to have them as
``= default`` ones, because C++17 considers subclasses like ``stl::Options`` as
aggregate classes if they have a default constructor and requires base class
constructor to be public (which is not the case for the ``thread::Options``) for
``Options{}`` syntax.

Please see the thread creation backend documentation for how their Options work.

Portable Thread Creation
========================
Due to the fact that ``pw::thread::Options`` cannot be created in portable code,
some extra work must be done in order to permit portable thread creation.
Namely, a reference to the portable ``pw::thread::Options`` base class interface
must be provided through a header or extern which points to an instantiation in
non-portable code.

This can be most easily done through a facade and set of backends. This approach
can be powerful; enabling multithreaded unit/integration testing which can run
on both the host and on a device with the device's exact thread options.

Alternatively, it can also be be injected at build time by instantiating backend
specific build rule which share the same common portable source file(s) but
select backend specific source files and/or dependencies which provide the
non-portable option instantiations.

As an example, let's say we want to create a thread on the host and on a device
running FreeRTOS. They could use a facade which contains a ``threads.h`` header
with the following contents:

.. code-block:: cpp

  // Contents of my_app/threads.h
  #pragma once

  #include "pw_thread/options.h"

  namespace my_app {

  const pw::thread::Options& HellowWorldThreadOptions();

  }  // namespace my_app

This could then be backed by two different backend implementations based on
the thread backend. For example for the STL the backend's ``stl_threads.cc``
source file may look something like:

.. code-block:: cpp

  // Contents of my_app/stl_threads.cc
  #include "my_app/threads.h"
  #include "pw_thread_stl/options.h"

  namespace my_app {

  const pw::thread::Options& HelloWorldThreadOptions() {
    static constexpr auto options = pw::thread::stl::Options();
    return options;
  }

  }  // namespace my_app

While for FreeRTOS the backend's ``freertos_threads.cc`` source file may look
something like:

.. code-block:: cpp

  // Contents of my_app/freertos_threads.cc
  #include "FreeRTOS.h"
  #include "my_app/threads.h"
  #include "pw_thread_freertos/context.h"
  #include "pw_thread_freertos/options.h"
  #include "task.h"

  namespace my_app {

  StaticContextWithStack<kHelloWorldStackWords> hello_world_thread_context;
  const pw::thread::Options& HelloWorldThreadOptions() {
    static constexpr auto options =
        pw::thread::freertos::Options()
            .set_name("HelloWorld")
            .set_static_context(hello_world_thread_context)
            .set_priority(kHelloWorldThreadPriority);
    return options;
  }

  }  // namespace my_app


Detaching & Joining
===================
The ``Thread::detach()`` API is always available, to let you separate the
thread of execution from the thread object, allowing execution to continue
independently.

The joining API, more specifically ``Thread::join()``, is conditionally
available depending on the selected backend for thread creation and how it is
configured. The backend is responsible for providing the
``PW_THREAD_JOINING_ENABLED`` macro through
``pw_thread_backend/thread_native.h``. This ensures that any users which include
``pw_thread/thread.h`` can use this macro if needed.

Please see the selected thread creation backend documentation for how to
enable joining if it's not already enabled by default.

.. Warning::
  A constructed ``pw::thread::Thread`` which represents a thread of execution
  must be EITHER detached or joined, else the destructor will assert!

DetachedThread
==============
To make it slightly easier and cleaner to spawn detached threads without having
to worry about thread handles, a wrapper ``DetachedThread()`` function is
provided which creates a ``Thread`` and immediately detaches it. For example
instead of:

.. code-block:: cpp

  Thread(options, foo).detach();

You can instead use this helper wrapper to:

.. code-block:: cpp

   DetachedThread(options, foo);

The arguments are directly forwarded to the Thread constructor and ergo exactly
match the Thread constuctor arguments for creating a thread of execution.


ThreadRoutine & ThreadCore
==========================
Threads must either be invoked through a
``pw::thread::Thread::ThreadRoutine``` style function or implement the
``pw::thread::ThreadCore`` interface.

.. code-block:: cpp

  namespace pw::thread {

  // This function may return.
  using Thread::ThreadRoutine = void (*)(void* arg);

  class ThreadCore {
   public:
    virtual ~ThreadCore() = default;

    // The public API to start a ThreadCore, note that this may return.
    void Start() { Run(); }

   private:
    // This function may return.
    virtual void Run() = 0;
  };

  }  // namespace pw::thread;


To use the ``pw::thread::Thread::ThreadRoutine``, your function must have the
following signature:

.. code-block:: cpp

  void example_thread_entry_function(void *arg);


To invoke a member method of a class a static lambda closure can be used
to ensure the dispatching closure is not destructed before the thread is
done executing. For example:

.. code-block:: cpp

  class Foo {
   public:
    void DoBar() {}
  };
  Foo foo;

  static auto invoke_foo_do_bar = [](void *void_foo_ptr) {
      //  If needed, additional arguments could be set here.
      static_cast<Foo*>(void_foo_ptr)->DoBar();
  };

  // Now use the lambda closure as the thread entry, passing the foo's
  // this as the argument.
  Thread thread(options, invoke_foo_do_bar, &foo);
  thread.detach();


Alternatively, the aforementioned ``pw::thread::ThreadCore`` interface can be
be implemented by an object by overriding the private
``void ThreadCore::Run();`` method. This makes it easier to create a thread, as
a static lambda closure or function is not needed to dispatch to a member
function without arguments. For example:

.. code-block:: cpp

  class Foo : public ThreadCore {
   private:
    void Run() override {}
  };
  Foo foo;

  // Now create the thread, using foo directly.
  Thread(options, foo).detach();

.. Warning::
  Because the thread may start after the pw::Thread creation, an object which
  implements the ThreadCore MUST meet or exceed the lifetime of its thread of
  execution!

----------------
Thread Iteration
----------------
C++
===
.. cpp:function:: Status ForEachThread(const ThreadCallback& cb)

   Calls the provided callback for each thread that has not been joined/deleted.

   This function provides a generalized subset of information that a TCB might
   contain to make it easier to introspect system state. Depending on the RTOS
   and its configuration, some of these fields may not be populated, so it is
   important to check that they have values before attempting to access them.

   **Warning:**  The function may disable the scheduler to perform
   a runtime capture of thread information.

-----------------------
Thread Snapshot Service
-----------------------
``pw_thread`` offers an optional RPC service library
(``:thread_snapshot_service``) that enables thread info capture of
running threads on a device at runtime via RPC. The service will guide
optimization of stack usage by providing an overview of thread information,
including thread name, stack bounds, and peak stack usage.

``ThreadSnapshotService`` currently supports peak stack usage capture for
all running threads (``ThreadSnapshotService::GetPeakStackUsage()``) as well as
for a specific thread, filtering by name
(``ThreadSnapshotService::GetPeakStackUsage(name=b"/* thread name */")``).
Thread information capture relies on the thread iteration facade which will
**momentarily halt your RTOS**, collect information about running threads, and
return this information through the service.

RPC service setup
=================
To expose a ``ThreadSnapshotService`` in your application, do the following:

1. Create an instance of ``pw::thread::proto::ThreadSnapshotServiceBuffer``.
   This template takes the number of expected threads, and uses it to properly
   size buffers required for a ``ThreadSnapshotService``. If no thread count
   argument is provided, this defaults to ``PW_THREAD_MAXIMUM_THREADS``.
2. Register the service with your RPC server.

For example:

.. code::

   #include "pw_rpc/server.h"
   #include "pw_thread/thread_snapshot_service.h"

   // Note: You must customize the RPC server setup; see pw_rpc.
   pw::rpc::Channel channels[] = {
    pw::rpc::Channel::Create<1>(&uart_output),
   };
   Server server(channels);

  // Thread snapshot service builder instance.
  pw::thread::proto::ThreadSnapshotServiceBuffer</*num threads*/>
      thread_snapshot_service;

   void RegisterServices() {
     server.RegisterService(thread_snapshot_service);
     // Register other services here.
   }

   void main() {
     // ... system initialization ...

     RegisterServices();

     // ... start your application ...
   }

.. c:macro:: PW_THREAD_MAXIMUM_THREADS

  The max number of threads to use by default for thread snapshot service.

.. cpp:function:: constexpr size_t RequiredServiceBufferSize(const size_t num_threads)

  Function provided through the service to calculate buffer sizing. If no
  argument ``num_threads`` is specified, the function will take ``num_threads``
  to be ``PW_THREAD_MAXIMUM_THREADS``.

.. attention::
    Some platforms may only support limited subsets of this service
    depending on RTOS configuration. **Ensure that your RTOS is configured
    properly before using this service.** Please see the thread iteration
    documentation for your backend for more detail on RTOS support.

-----------------------
pw_snapshot integration
-----------------------
``pw_thread`` provides some light, optional integration with pw_snapshot through
helper functions for populating a ``pw::thread::Thread`` proto. Some of these
are directly integrated into the RTOS thread backends to simplify the thread
state capturing for snapshots.

SnapshotStack()
===============
The ``SnapshotStack()`` helper captures stack metadata (stack pointer and
bounds) into a ``pw::thread::Thread`` proto. After the stack bounds are
captured, execution is passed off to the thread stack collection callback to
capture a backtrace or stack dump. Note that this function does NOT capture the
thread name: that metadata is only required in cases where a stack overflow or
underflow is detected.

Python processor
================
Threads captured as a Thread proto message can be dumped or further analyzed
using using ``pw_thread``'s Python module. This is directly integrated into
pw_snapshot's processor tool to automatically provide rich thread state dumps.

The ``ThreadSnapshotAnalyzer`` class may also be used directly to identify the
currently running thread and produce symbolized thread dumps.

.. Warning::
  Snapshot integration is a work-in-progress and may see significant API
  changes.
