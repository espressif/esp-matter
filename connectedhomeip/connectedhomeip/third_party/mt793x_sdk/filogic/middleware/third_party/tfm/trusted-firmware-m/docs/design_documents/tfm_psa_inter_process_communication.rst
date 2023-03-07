################################
TF-M Inter-Process Communication
################################

:Authors: Ken Liu, Mingyang Sun
:Organization: Arm Limited
:Contact: ken.liu@arm.com, mingyang.sun@arm.com
:Status: Accepted

***********
Terminology
***********

IPC - Inter-Process Communication

For more terminology please check Reference_ document.

***************
Design Overview
***************
IPC re-uses existed components in library model:

- SPM – for partition information and isolation actions
- Core – for exception handling

Extra components for implementing IPC:

- Memory pool
- Message manager
- Thread
- Synchronization objects
- PSA API

**********************
Implementation Details
**********************
Listed modules are all internal modules except PSA API. Prototypes and
definitions are not listed for internal modules in this document. For PSA
API definitions, check them in PSA Firmware Framework specification in the
reference chapter.

SPM and Core
============
SPM manages Secure Partition information. Enhancements need to be done in SPM
data structure for Secure Partition for IPC due to:

- IPC model requires each Secure Partition has its own stack area while
  isolation level 1 of library model makes all partition shares same stack
  pointer. This needs to be changed while implementing IPC.
- Multiple services are holding in same Secure Partition and each service
  has its own information like message queue, SID and priority.
- Changed information related manifest items need to be changed, too.

Modifications in Core:

- More SVC calls need to be added into list since PSA API are implemented as
  SVC calls in TF-M.
- New PendSV handler for thread scheduling.
- Arch-related context stacking and switching.

Memory Pool
===========
Handles of connection and messages for Secure Partition needs to be allocated
dynamically. A memory pool is provided in the system to handle dynamic
allocation. Each memory pool item contains below information:

- A list iterator to chain all of memory pool items.
- An information member to record information like size and types.
- The memory item body for caller usage.

A memory area needs to be provided in SPM for the memory pool. It could be an
array of memory areas defined in the linker script. Two chains are available to
manage the items: free chain and used chain. And an LRU (Last recent used)
mechanism is applied for fast seeking while item allocating and destroying.

Message Manager
===============
Message Manager handles message creating, pushing, retrieving and destroy. A
message contains below information:

- Message sender and destination
- Message status
- IO vectors for service
- 'psa_msg_t' for service

A checking needs to be performed in SPM before creating a message to detect if
a message with the same sender and destination is ongoing. This avoids repeat
messages are available in the queue.

Thread
======
Each Secure Partition has a thread as execution environment. Secure Partition
is defined statically in TF-M manifest, which indicates that a number of
threads are statically defined. Threads are chained in SPM and sorted with
its priority, and there is an extra indicator point to first running thread
with the highest priority. This helps fast seeking of running threads while
the scheduler is switching threads.

Thread context contains below information:

- Priority
- Status
- Stack pointer
- Stack pointer limitation
- Entry
- Parameter
- Entry return value
- Context
- List iterator

Thread API provides below functions:

- Thread creating and destroying
- Thread status retrieving and changing
- Current thread retrieving
- Thread context switching

PendSV exception in TF-M core is the place thread context APIs been called.
Before thread switching taking place, isolation status needs to be changed
based on Secure Partition change and current isolation level – a thread is a
member of partition which means thread switching caused a partition switching.

Synchronization API
===================
A first synchronization object is an event. This could be applied into event
waiting in the partition, and message response handling in IPC. The event
object contains below members:

- Owner thread who is waiting for this event
- Event status (Ready or Not-Ready)
- List iterator for synchronization objects management

Event API Limitation: could be waited by one thread only.

PSA API
=======
This chapter describes the PSA API in an implementation manner.

- API type: could be Client API and Service Partition API
- Block-able: Block-able API may block caller thread; Non-Block API does not
  block caller thread.
- Description: The functionality description and important comments.

.. code-block:: c

    uint32_t psa_framework_version(void);
    uint32_t psa_version(uint32_t sid);

- Client API
- Non-Block API
- These 2 functions are finally handled in SPM and return the framework version
  or version to the caller.

.. code-block:: c

    psa_handle_t psa_connect(uint32_t sid, uint32_t version);
    psa_status_t psa_call(psa_handle_t handle, int32_t type,
                          const psa_invec *in_vec, size_t in_len,
                          psa_outvec *out_vec, size_t out_len);
    void psa_close(psa_handle_t handle);

- Client API
- Block-able API
- These 3 APIs are implemented in the same manner and just different
  parameters. SPM converts each call into a corresponding message with a
  parameter in the message body and pushes the message into service queue to
  wait for the response. Scheduler switches to a specified thread (partition)
  and makes Secure Partition to have chance retrieving and process message.
  After a message response is returned to the caller, the waiting caller gets
  to go and get the result.

.. code-block:: c

    psa_signal_t psa_wait(psa_signal_t signal_mask, uint32_t timeout);

- Secure Partition API
- Block-able API
- This API blocks caller partition if there is no expected event for it. This
  function is implemented based on event API.

.. code-block:: c

    void psa_set_rhandle(psa_handle_t msg_handle, void *rhandle);
    psa_status_t psa_get(psa_signal_t signal, psa_msg_t *msg);
    size_t psa_read(psa_handle_t msg_handle, uint32_t invec_idx,
                    void *buffer, size_t num_bytes);
    size_t psa_skip(psa_handle_t msg_handle, uint32_t invec_idx,
                    size_t num_bytes);
    void psa_write(psa_handle_t msg_handle, uint32_t outvec_idx,
                   const void *buffer, size_t num_bytes);
    void psa_reply(psa_handle_t msg_handle, psa_status_t status);
    void psa_clear(void);
    void psa_eoi(psa_signal_t irq_signal);

- Secure Partition API
- Non-Block
- These APIs do not take the initiative to change caller status. They process
  data and return the processed data back to the caller.

.. code-block:: c

    void psa_notify(int32_t partition_id);

- Secure Partition API
- Non-Block
- This API sets DOORBELL bit in destination partition's event. This API does
  not take the initiative to change caller status.

.. code-block:: c

    void psa_panic(void);

- Secure Partition API
- Block-able API
- This function will terminate execution within the calling Secure Partition
  and will not return.

*********
Reference
*********

| `PSA Firmware Framework specification URL`_
| `Slides includes IPC basic introduction URL`_
| `IPC model implementation URL`_

.. _PSA Firmware Framework specification URL: https://pages.arm.com/psa-
  resources-ff.html?_ga=2.156169596.61580709.1542617040-1290528876.1541647333
.. _Slides includes IPC basic introduction URL: https://connect.linaro.org/
  resources/yvr18/sessions/yvr18-108/
.. _IPC model implementation URL: https://www.youtube.com/watch?v=6wEFoq49qUw

--------------

*Copyright (c) 2019, Arm Limited. All rights reserved.*
