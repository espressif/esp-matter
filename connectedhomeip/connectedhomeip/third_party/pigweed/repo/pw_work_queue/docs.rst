.. _module-pw_work_queue:

=============
pw_work_queue
=============
The ``pw_work_queue`` module contains utilities for deferring work to be
executed by another thread.

.. Warning::
  This module is still under construction, the API is not yet stable.

---------
WorkQueue
---------
The ``pw::work_queue::WorkQueue`` class enables threads and interrupts to
enqueue work as a ``pw::work_queue::WorkItem`` for execution by the work queue.

The entire API is thread and interrupt safe.

Queue Sizing
============
The number of outstanding work requests is limited based on the
``pw::work_queue::WorkQueue``'s internal queue size. This must be set
appropriately for the application by the user.

The queue size is set trough either through the size of the ``queue_storage``
buffer passed into the constructor or by using the templated
``pw::work_queue::WorkQueueWithBuffer`` helper.

.. Note:: While the queue is full, the queue will not accept further work.

Cooperative Thread Cancellation
===============================
The class is a ``pw::thread::ThreadCore``, meaning it should be executed as a
single thread. In order to facilitate clean shutdown, it provides a
``RequestStop()`` API for cooperative cancellation which should be invoked
before joining the thread.

.. Note:: Once stop has been requested the queue will no longer accept further
          work.

C++
===
.. cpp:class:: pw::work_queue::WorkQueue

  .. cpp:function:: Status PushWork(WorkItem work_item)

     Enqueues a work_item for execution by the work queue thread.

     Returns:

     * **Ok** - Success, entry was enqueued for execution.
     * **FailedPrecondition** - the work queue is shutting down, entries are no
       longer permitted.
     * **ResourceExhausted** - internal work queue is full, entry was not
       enqueued.

  .. cpp:function:: void CheckPushWork(WorkItem work_item)

     Queue work for execution. Crash if the work cannot be queued due to a
     full queue or a stopped worker thread.

     This call is recommended where possible since it saves error handling code
     at the callsite; and in many practical cases, it is a bug if the work
     queue is full (and so a crash is useful to detect the problem).

     **Precondition:** The queue must not overflow, i.e. be full.

     **Precondition:** The queue must not have been requested to stop, i.e. it
     must not be in the process of shutting down.

  .. cpp:function:: void RequestStop()

     Locks the queue to prevent further work enqueing, finishes outstanding
     work, then shuts down the worker thread.

     The WorkQueue cannot be resumed after stopping as the ThreadCore thread
     returns and may be joined. It must be reconstructed for re-use after
     the thread has been joined.

Example
-------

.. code-block:: cpp

  #include "pw_thread/detached_thread.h"
  #include "pw_work_queue/work_queue.h"

  pw::work_queue::WorkQueueWithBuffer<10> work_queue;

  pw::thread::Options& WorkQueueThreadOptions();
  void SomeLongRunningProcessing();

  void SomeInterruptHandler() {
    // Instead of executing the long running processing task in the interrupt,
    // the work_queue executes it on the interrupt's behalf.
    work_queue.CheckPushWork(SomeLongRunningProcessing);
  }

  int main() {
    // Start up the work_queue as a detached thread which runs forever.
    pw::thread::DetachedThread(WorkQueueThreadOptions(), work_queue);
  }

