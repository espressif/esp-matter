################################################################
Communication Prototype Between NSPE And SPE In Dual Core System
################################################################

:Authors: David Hu
:Organization: Arm Limited
:Contact: david.hu@arm.com
:Status: Accepted

************
Introduction
************

This document proposes a generic prototype of the communication between NSPE
(Non-secure Processing Environment) and SPE (Secure Processing Environment) in
TF-M on a dual core system.

The dual core system should satisfy the following requirements

- NSPE and SPE are properly isolated and protected following PSA
- An Arm M-profile core locates in SPE and acts as the Secure core
- An Inter-Processor Communication hardware module in system for communication
  between NSPE core and SPE core
- TF-M runs on the Secure core with platform specific drivers support.

Scope
=====

This design document focuses on the dual core communication design inside TF-M.
Some changes to TF-M core/Secure Partition Manager (SPM) are listed to support
the dual core communication. This document only discuss about the implementation
in TF-M Inter-Process Communication (IPC) model.
The TF-M non-secure interface library is tightly coupled with mailbox and RTOS
implementation. The related changes to TF-M non-secure interface library are not
discussed in detail in this document.

Some requirements to mailbox functionalities are defined in this document. The
detailed mailbox design or implementations is not specified in this document.
Please refer to mailbox dedicate documents.

Organization of the document
============================

- `Overall workflow in dual core communication`_ provides an overall workflow of
  dual core communication between NSPE and SPE.
- `Requirements on mailbox communication`_ lists general requirements of
  mailbox, from the perspective of dual core communication.
- `PSA client call handling flow in TF-M`_ discusses about the detailed sequence
  and key modules of handling PSA client call in TF-M.
- `Summary of changes to TF-M core/SPM`_ summarizes the potential changes to
  TF-M core/SPM to support dual core communication.

*******************************************
Overall workflow in dual core communication
*******************************************

The overall workflow in dual-core scenario can be described as follows

1. Non-secure application calls TF-M non-secure interface library to request
   Secure service. The TF-M non-secure interface library translates the Secure
   service into PSA Client calls.
2. TF-M non-secure interface library notifies TF-M of the PSA client call
   request, via mailbox. Proper generic mailbox APIs in HAL should be defined
   so that TF-M non-secure interface library can co-work with diverse platform
   specific Inter-Processor Communication implementations.
3. Inter-Processor Communication interrupt handler and mailbox handling in TF-M
   deal with the inbound mailbox event(s) and deliver the PSA client call
   request to TF-M SPM.
4. TF-M SPM processes the PSA client call request. The PSA client call is
   eventually handled in target Secure Partition or corresponding handler.
5. After the PSA Client call is completed, the return value is returned to NSPE
   via mailbox.
6. TF-M non-secure interface library fetches return value from mailbox.
7. The return value is returned to non-secure application.

The interfaces between NSPE app and TF-M NSPE interface library are unchanged
so the underlying platform specific details are transparent to NSPE
application.

Step 3 ~ step 5 are covered in `PSA client call handling flow in TF-M`_ in
detail.

*************************************
Requirements on mailbox communication
*************************************

The communication between NSPE and SPE relies on mailbox communication
implementation. The mailbox functionalities are eventually implemented by
platform specific Inter-Processor Communication drivers.
This section lists some general requirements on mailbox communication between
NSPE and SPE.

Data transferred between NPSE and SPE
=====================================

A mailbox message should contain the information and parameters of a PSA client
call. After SPE is notified by a mailbox event, SPE fetches the mailbox message
from NSPE for PSA Client call processing.
The mailbox design document should define the structure of the mailbox message.

The information and parameters of PSA Client call in the mailbox message include

- PSA Client API

- Parameters required in PSA Client call. The parameters can include the
  following, according to PSA client call type

  - Service ID (SID)
  - Handle
  - Request type
  - Input vectors and the lengths
  - Output vectors and the lengths
  - Requested version of secure service

- NSPE Client ID. Optional. The NSPE Client ID is required when NSPE RTOS
  enforces non-secure task isolation.

The mailbox design document can define additional members in mailbox message to
accomplish mailbox communication between NSPE and SPE.

When the PSA Client call is completed in TF-M, the return result, such as
PSA_SUCCESS or the handle, should be returned from SPE to NSPE via mailbox.

Mailbox synchronization between NSPE and SPE
============================================

Synchronization and protection between NSPE and SPE accesses to shared mailbox
objects and variables should be implemented.

When a core accesses shared mailbox objects or variables, proper mechanisms
should protect concurrent operations from the other core.

Support of multiple ongoing NS PSA client calls (informative)
=============================================================

Current TF-M implementation (git tag TF-Mv1.0-RC3) only supports single
outstanding PSA client call from NSPE.

If the support of multiple ongoing NS PSA client calls in TF-M is required
in dual-core systems, an optional queue can be maintained in TF-M core to store
multiple mailbox objects received from NSPE.
To identify NS PSA client calls, additional fields can be added in TF-M SPM
objects to store the NS PSA Client request identification.

Note that when just a single outstanding PSA client call is allowed, multiple
NSPE OS threads can run concurrently and call PSA client functions. The first
PSA client call will be processed first, and any other OS threads will be
blocked from submitting PSA client calls until the first is completed.

*************************************
PSA client call handling flow in TF-M
*************************************

This section provides more details about the flow of PSA client call handing in
TF-M.

The sequence of handling PSA Client call request in TF-M is listed as below

1. Platform specific Inter-Processor Communication interrupt handler is
   triggered after the mailbox event is asserted by NSPE. The interrupt handler
   should assert a PendSV.
2. In the top half of PendSV handler, the scheduler selects the next thread to
   run and executes normal context switch if necessary.
3. In the bottom half of PendSV handler, mailbox handling deals with the mailbox
   message(s) which contain(s) the PSA client call information and parameters.
   Then the PSA client call request is dispatched to dedicated PSA client call
   handler in TF-M SPM.
4. After the PSA client call is completed, the return value is transmitted to
   NSPE via mailbox.

Several key modules in the whole process are covered in detail in following
sections.

- `Inter-Processor Communication interrupt handler`_ discusses about the
  Inter-Processor Communication interrupt handler
- `TF-M Remote Procedure Call (RPC) module`_ introduces TF-M Remote Procedure
  Call module to support dual-core communication.
- `PendSV handler`_ specifies the mailbox tasks in PendSV handler.
- `Return value replying routine in TF-M`_ proposes the routine to reply the
  return value to NSPE.

Inter-Processor Communication interrupt handler
===============================================

Platform specific driver should implement the Inter-Processor Communication
interrupt handler to deal with the Inter-Processor Communication interrupt
asserted by NSPE.
The platform specific interrupt handler should complete the interrupt
operations, such as interrupt EOI or acknowledge.

The interrupt handler should call generic mailbox API(s) to check whether an
inbound mailbox event has occurred. Mailbox functions should be implemented in
platform vendor driver to check mailbox status in NSPE and distinguish spurious
events.
The interrupt handler should assert a PendSV after the inbound mailbox event is
confirmed. The triggered PendSV handler will deal with the mailbox events.

Platform specific driver should put Inter-Processor Communication interrupt into
a proper exception priority, according to system and application requirements.
The proper priority setting should guarantee that

- TF-M can respond to a PSA client call request in time according to system and
  application requirements.
- Other exceptions, which are more latency sensitive or require higher
  priorities, are not blocked by Inter-Processor Communication interrupt ISR.

The exception priority setting is IMPLEMENTATION DEFINED.

It is recommended to implement mailbox time-consuming operations in PendSV
handler, such as mailbox message copying from NSPE to SPE. This can decrease the
response latency for other interrupts, compared with putting all the mailbox
operations in interrupt handler. Refer to `PendSV handler`_ for more details of
PendSV handler.

In actual mailbox implementation, Inter-Processor Communication interrupt
handler can execute more mailbox processing, such as mailbox message parsing and
copying, to simplify the synchronization. It may increase the time consumption
in interrupt handling. Therefore, it might block the handling of other
interrupts whose priorities are lower or equal to Inter-Processor Communication
interrupt. As a result, it becomes more important to adjust the
Interrupt-Processor Communication interrupt priority in TF-M, according to
specific system and implementation requirements.

TF-M Remote Procedure Call (RPC) module
=======================================

This design brings up a concept of Remote Procedure Call module in TF-M.

The RPC module sits between TF-M SPM and mailbox implementation. The purpose of
RPC module is to decouple mailbox implementation and TF-M SPM and enhance the
generality of entire dual-core communication.

The RPC module provides a set of APIs to TF-M SPM to handle and reply PSA client
call from NSPE in dual-core scenario. Please refer to
`TF-M RPC definitions to TF-M SPM`_ for API details.
It hides the details of specific mailbox implementation from TF-M SPM. It avoids
modifying TF-M SPM to fit mailbox development and changes.
It can keep a unified PSA client call process in TF-M SPM in both single
Armv8-M scenario and dual core scenario.

The RPC module defines a set callback functions for mailbox implementation to
hook its specific mailbox operations. When TF-M SPM invokes RPC APIs to deal
with NSPE PSA client call, RPC module eventually calls the callbacks to execute
mailbox operations.
RPC module also defines a set of PSA client call handler APIs for mailbox
implementation. RPC specific client call handlers parse the PSA client call
parameters and invoke common TF-M PSA client call handlers. Please refer to
`TF-M RPC definitions for mailbox`_ for the details.

PendSV handler
==============

The mailbox handling should be added to PendSV handler in current TF-M single
Armv8-M implementation in IPC model. Mailbox handling processes the inbound
mailbox event(s) in the bottom half of PendSV handler. The top half of PendSV
contains the original scheduling.

Mailbox handling must be executed after the original scheduling to make sure
that when mailbox handling triggers the sleeping secure service, the status of
sleeping secure service has been updated in scheduling.

A compile flag can be defined to disable mailbox handling in PendSV handler in
single Armv8-M scenario during building.

This section only discusses about the mailbox handling in the bottom half of
PendSV handler. The TF-M scheduling and context switch should keep unchanged as
current single Armv8-M implementation.

Mailbox handling in bottom half of PendSV handler
-------------------------------------------------

PendSV handler should call RPC API ``tfm_rpc_client_call_handler()`` to check
and handle PSA client call request from NSPE. ``tfm_rpc_client_call_handler()``
invokes request handling callback function to eventually execute specific
mailbox message handling operations. The mailbox APIs are defined in mailbox
design document

The handling process in mailbox operation consists of the following steps.

1. If copy operations are not done in Inter-Processor Communication interrupt
   handler, the mailbox handling should fetch the mailbox message(s) containing
   PSA client call request from NSPE. Proper protection and synchronization
   should be implemented in mailbox to guarantee that the operations are not
   interfered by NSPE mailbox operations or Inter-Processor Communication
   interrupt handler. If a queue is maintained inside TF-M core, mailbox
   handling can fetch multiple mailbox messages together into the queue, to save
   the time of synchronization between two cores.

2. Mailbox handling parses the mailbox message copied in SPE and fetches the
   information of the PSA client call, including the PSA client call type.
   Additional checks can be executed to make sure that the mailbox message is
   valid. It may require additional information carried in the mailbox message.

3. The PSA client call request is dispatched to the dedicated TF-M RPC PSA
   client call handler. The PSA client call request is processed in the
   corresponding handler.

  - For ``psa_framework_version()`` and ``psa_version()``, the PSA client call
    can be completed in the handlers ``tfm_rpc_psa_framework_version()`` and
    ``tfm_rpc_psa_version()`` respectively.

  - For ``psa_connect()``, ``psa_call()`` and ``psa_close()``, the handlers
    ``tfm_rpc_psa_connect()``, ``tfm_rpc_psa_call()`` and
    ``tfm_rpc_psa_close()`` create the PSA message and trigger target Secure
    partition respectively. The target Secure partition will be woken up to
    handle the PSA message.

The dual-core scenario and single Armv8-M scenario in TF-M IPC implementation
should share the same PSA client call routines inside TF-M SPM. The current
handler definitions can be adjusted to be more generic for dual-core scenario
and single Armv8-M implementation. Please refer to
`Summary of changes to TF-M core/SPM`_ for details.

If there are multiple NSPE PSA client call requests pending, the mailbox
handling can process mailbox messages one by one. Mailbox handling can complete
all the mailbox messages processing before running to the bottom half of the
PendSV handler.

Implementation details in PendSV handler
----------------------------------------

Some more details should be taken care of in actual implementation.

- PendSV priority should be configured as low enough, to prevent blocking or
  preempting other latency sensitive interrupts.
- All the mailbox implementations inside PendSV handler must not directly
  execute context switch.
- To simplify the interrupt handling inside TF-M, the mailbox handling
  implementation inside PendSV handle should avoid triggering additional
  Inter-Processor Communication interrupts in TF-M, unless it is explicitly
  required in mailbox design.
- If Inter-Processor Communication interrupt handler and PendSV handler access
  shared mailbox objects, proper protection and synchronization should be
  implemented in both handlers. For example, the Inter-Processor Communication
  interrupt can be temporarily disabled on secure core while PendSV handler
  accesses mailbox objects in TF-M.

Return value replying routine in TF-M
=====================================

Diverse PSA client calls can be implemented with different return value replying
routines.

- `Replying routine for psa_framework_version() and psa_version()`_ describes
  the routine for ``psa_framework_version()`` and ``psa_version()``.
- `Replying routine for psa_connect(), psa_call() and psa_close()` describes the
  routine for ``psa_connect()``, ``psa_call()`` and ``psa_close()``.

Replying routine for psa_framework_version() and psa_version()
--------------------------------------------------------------

For ``psa_framework_version()`` and ``psa_version()``, the return value can be
directly returned from the dedicated TF-M RPC PSA client call handlers.
Therefore, the return value can be directly replied in mailbox handling process.

A compile flag should be defined to enable replying routine via mailbox in
dual-core scenario during building.

The mailbox reply functions must not trigger context switch inside PendSV
handler.

Replying routine for psa_connect(), psa_call() and psa_close()
--------------------------------------------------------------

For ``psa_connect()``, ``psa_call()`` and ``psa_close()``, the PSA client call
is completed in the target Secure Partition. The target Secure Partition calls
``psa_reply()`` to reply the return value to TF-M SPM. In the SVC handler of
``psa_reply()`` in TF-M SPM, TF-M SPM should call TF-M RPC API
``tfm_rpc_client_call_reply()`` to return the value to NSPE via mailbox.
``tfm_rpc_client_call_reply()`` invokes reply callbacks to execute specific
mailbox reply operations. The mailbox reply functions must not trigger context
switch inside SVC handler.

If an errors occurs in the handlers, the TF-M RPC handlers,
``tfm_rpc_psa_call()``, ``tfm_rpc_psa_connect()`` and ``tfm_rpc_psa_close()``,
may terminate and return the error, without triggering the target Secure
Partition. The mailbox implementation should return error code to NSPE.

A compile flag should be defined to enable replying routine via mailbox in
dual-core scenario during building.

***********************************
Summary of changes to TF-M core/SPM
***********************************

This section discusses the general changes related to NSPE and SPE
communication to current TF-M core/SPM implementations.

The detailed mailbox implementations are not covered in this section. Please
refer to mailbox related specific documents.
The platform specific implementations are also not covered in this section,
including the Inter-Processor Communication interrupt or its interrupt handler.

Common PSA client call handlers
===============================

Common PSA client call handlers should be extracted from current PSA client
call handlers implementation in TF-M.
Common PSA client call handlers are shared by both TF-M RPC module in dual-core
scenario and SVCall handlers in single Armv8-M scenario.

TF-M RPC module
===============

This section describes the TF-M RPC data types and APIs.

- `TF-M RPC definitions to TF-M SPM`_ lists the data types and APIs to be
  invoked by TF-M SPM.
- `TF-M RPC definitions for mailbox`_ lists the data types and APIs to be
  referred by mailbox implementation

TF-M RPC definitions to TF-M SPM
--------------------------------

TFM_RPC_SUCCESS
^^^^^^^^^^^^^^^

``TFM_RPC_SUCCESS`` is a general return value to indicate that the RPC operation
succeeds.

.. code-block:: c

  #define TFM_RPC_SUCCESS             (0)

TFM_RPC_INVAL_PARAM
^^^^^^^^^^^^^^^^^^^

``TFM_RPC_INVAL_PARAM`` is a return value to indicate that the input parameters
are invalid.

.. code-block:: c

  #define TFM_RPC_INVAL_PARAM         (INT32_MIN + 1)

TFM_RPC_CONFLICT_CALLBACK
^^^^^^^^^^^^^^^^^^^^^^^^^

Currently one and only one mailbox implementation is supported in dual core
communication. This flag indicates that callback functions from one mailbox
implementation are already registered and no more implementations are accepted.

.. code-block:: c

  #define TFM_RPC_CONFLICT_CALLBACK   (INT32_MIN + 2)

``tfm_rpc_client_call_handler()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TF-M PendSV handler calls this function to handle NSPE PSA client call request.

.. code-block:: c

  void tfm_rpc_client_call_handler(void);

Usage
~~~~~

``tfm_rpc_client_call_handler()`` invokes callback function ``handle_req()`` to
execute specific mailbox handling.
Please note that ``tfm_rpc_client_call_handler()`` doesn't return the status of
underlying mailbox handling.

``tfm_rpc_client_call_reply()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TF-M ``psa_reply()`` handler calls this function to reply PSA client call return
result to NSPE.

.. code-block:: c

  void tfm_rpc_client_call_reply(const void *owner, int32_t ret);

Parameters
~~~~~~~~~~

+-----------+--------------------------------------------------------------+
| ``owner`` | A handle to identify the owner of the PSA client call return |
|           | value.                                                       |
+-----------+--------------------------------------------------------------+
| ``ret``   | PSA client call return result value.                         |
+-----------+--------------------------------------------------------------+

Usage
~~~~~

``tfm_rpc_client_call_reply()`` invokes callback function ``reply()`` to execute
specific mailbox reply.
Please note that ``tfm_rpc_client_call_reply()`` doesn't return the status of
underlying mailbox reply process.

TF-M RPC definitions for mailbox
--------------------------------

PSA client call parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^

This data structure holds the parameters used in a PSA client call. The
parameters are passed from non-secure core to secure core via mailbox.

.. code-block:: c

  struct client_call_params_t {
      uint32_t        sid;
      psa_handle_t    handle;
      int32_t         type;
      const psa_invec *in_vec;
      size_t          in_len;
      psa_outvec      *out_vec;
      size_t          out_len;
      uint32_t        version;
  };

Mailbox operations callbacks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This structures contains the callback functions for specific mailbox operations.

.. code-block:: c

  struct tfm_rpc_ops_t {
      void (*handle_req)(void);
      void (*reply)(const void *owner, int32_t ret);
  };

``tfm_rpc_register_ops()``
^^^^^^^^^^^^^^^^^^^^^^^^^^

This function registers underlying mailbox operations into TF-M RPC callbacks.

.. code-block:: c

  int32_t tfm_rpc_register_ops(const struct tfm_rpc_ops_t *ops_ptr);

Parameters
~~~~~~~~~~

+-------------+----------------------------------------------+
| ``ops_ptr`` | Pointer to the specific operation structure. |
+-------------+----------------------------------------------+

Return
~~~~~~

+----------------------+-----------------------------------------+
| ``TFM_RPC_SUCCESS``  | Operations are successfully registered. |
+----------------------+-----------------------------------------+
| ``Other error code`` | Fail to register operations.            |
+----------------------+-----------------------------------------+

Usage
~~~~~

Mailbox should register TF-M RPC callbacks during mailbox initialization, before
enabling secure services for NSPE.

Currently one and only one underlying mailbox communication implementation is
allowed in runtime.

``tfm_rpc_unregister_ops()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This function unregisters underlying mailbox operations from TF-M RPC callbacks.

.. code-block:: c

  void tfm_rpc_unregister_ops(void);

Usage
~~~~~

Currently one and only one underlying mailbox communication implementation is
allowed in runtime.

``tfm_rpc_psa_framework_version()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TF-M RPC handler for psa_framework_version().

.. code-block:: c

  uint32_t tfm_rpc_psa_framework_version(void);

Return
~~~~~~

+-------------+---------------------------------------------------------+
| ``version`` | The version of the PSA Framework implementation that is |
|             | providing the runtime services.                         |
+-------------+---------------------------------------------------------+

Usage
~~~~~

``tfm_rpc_psa_framework_version()`` invokes common ``psa_framework_version()``
handler in TF-M.

``tfm_rpc_psa_version()``
^^^^^^^^^^^^^^^^^^^^^^^^^

TF-M RPC handler for psa_version().

.. code-block:: c

  uint32_t tfm_rpc_psa_version(const struct client_call_params_t *params,
                               bool ns_caller);

Parameters
~~~~~~~~~~

+---------------+-----------------------------------+
| ``params``    | Base address of parameters.       |
+---------------+-----------------------------------+
| ``ns_caller`` | Whether the caller is non-secure. |
+---------------+-----------------------------------+

Return
~~~~~~

+----------------------+------------------------------------------------------+
| ``PSA_VERSION_NONE`` | The RoT Service is not implemented, or the caller is |
|                      | not permitted to access the service.                 |
+----------------------+------------------------------------------------------+
| ``> 0``              | The minor version of the implemented RoT Service.    |
+----------------------+------------------------------------------------------+

Usage
~~~~~

``tfm_rpc_psa_version()`` invokes common ``psa_version()`` handler in TF-M.
The parameters in params should be prepared before calling
``tfm_rpc_psa_version()``.

``tfm_rpc_psa_connect()``
^^^^^^^^^^^^^^^^^^^^^^^^^

TF-M RPC handler for ``psa_connect()``.

.. code-block:: c

  psa_status_t tfm_rpc_psa_connect(const struct client_call_params_t *params,
                                   bool ns_caller);

Parameters
~~~~~~~~~~

+---------------+-----------------------------------+
| ``params``    | Base address of parameters.       |
+---------------+-----------------------------------+
| ``ns_caller`` | Whether the caller is non-secure. |
+---------------+-----------------------------------+

Return
~~~~~~

+-------------------------+---------------------------------------------------+
| ``PSA_SUCCESS``         | Success.                                          |
+-------------------------+---------------------------------------------------+
| ``PSA_CONNECTION_BUSY`` | The SPM cannot make the connection at the moment. |
+-------------------------+---------------------------------------------------+
| ``Does not return``     | The RoT Service ID and version are not supported, |
|                         | or the caller is not permitted to access the      |
|                         | service.                                          |
+-------------------------+---------------------------------------------------+

Usage
~~~~~

``tfm_rpc_psa_connect()`` invokes common ``psa_connect()`` handler in TF-M.
The parameters in params should be prepared before calling
``tfm_rpc_psa_connect()``.

``tfm_rpc_psa_call()``
^^^^^^^^^^^^^^^^^^^^^^

TF-M RPC handler for ``psa_call()``.

.. code-block:: c

  psa_status_t tfm_rpc_psa_call(const struct client_call_params_t *params,
                                bool ns_caller);

Parameters
~~~~~~~~~~

+---------------+-----------------------------------+
| ``params``    | Base address of parameters.       |
+---------------+-----------------------------------+
| ``ns_caller`` | Whether the caller is non-secure. |
+---------------+-----------------------------------+

Return
~~~~~~

+---------------------+---------------------------------------------+
| ``PSA_SUCCESS``     | Success.                                    |
+---------------------+---------------------------------------------+
| ``Does not return`` | The call is invalid, or invalid parameters. |
+---------------------+---------------------------------------------+

Usage
~~~~~

``tfm_rpc_psa_call()`` invokes common ``psa_call()`` handler in TF-M.
The parameters in params should be prepared before calling
``tfm_rpc_psa_call()``.

``tfm_rpc_psa_close()``
^^^^^^^^^^^^^^^^^^^^^^^

TF-M RPC ``psa_close()`` handler

.. code-block:: c

  void tfm_rpc_psa_close(const struct client_call_params_t *params,
                         bool ns_caller);

Parameters
~~~~~~~~~~

+---------------+-----------------------------------+
| ``params``    | Base address of parameters.       |
+---------------+-----------------------------------+
| ``ns_caller`` | Whether the caller is non-secure. |
+---------------+-----------------------------------+

Return
~~~~~~

+---------------------+---------------------------------------------+
| ``void``            | Success.                                    |
+---------------------+---------------------------------------------+
| ``Does not return`` | The call is invalid, or invalid parameters. |
+---------------------+---------------------------------------------+

Usage
~~~~~

``tfm_rpc_psa_close()`` invokes common ``psa_close()`` handler in TF-M.
The parameters in params should be prepared before calling
``tfm_rpc_psa_close()``.

Other modifications
===================

The following mandatory changes are also required.

- One or more compile flag(s) should be defined to select corresponding
  execution routines in dual-core scenario or single Armv8-M scenario during
  building.
- PendSV priority should be configured in TF-M initialization.

Some optional changes or optimizations are listed below.

- The PSA client call handlers of ``psa_connect()``, ``psa_call()`` and
  ``psa_close()`` can be optimized to skip asserting PendSV in dual-core
  scenario.

----------------

Copyright (c) 2019-2020 Arm Limited. All Rights Reserved.
Copyright (c) 2020 Cypress Semiconductor Corporation
