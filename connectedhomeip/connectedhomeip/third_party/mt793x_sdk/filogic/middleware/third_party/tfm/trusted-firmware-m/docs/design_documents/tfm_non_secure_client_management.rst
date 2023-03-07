############################
Non-secure Client Management
############################

:Author: Miklos Balint
:Organization: Arm Limited
:Contact: Miklos Balint <miklos.balint@arm.com>
:Status: Accepted

***********
Terminology
***********

**Secure service call**: request to a secure partition by a secure or non-secure
client thread

**Secure function call**: any function call from NSPE to SPE

**TrustZone (TZ) API**: the set of functions defined by CMSIS for RTOS secure
context management

**Client ID**: the identifier defining a single entity within the system,
determining its access policies for any given secure assets

*************************
Assumptions, restrictions
*************************

This design considers as its baseline the current operation of TF-M: an
operating mode where at any given time only a single non-secure access is
permitted to call a secure service.

If a non-secure RTOS/bare-metal application does not use the API calls defined
in this design, that non-secure application is still able to use secure services
using a single, default non-secure client context. That remains a supported use
case and use of this API is optional and is only needed if multiple access
policies and/or concurrent secure contexts initiated by non-secure threads are
required.

Investigation is ongoing to address the option of enabling multiple concurrent
calls by non-secure threads without the use of the context management API below.

******
Issues
******

The topics being discussed in this document:

- NS client/thread awareness in TF-M Core
- "Known client" list

Improvements, alternatives, investigations

- Concurrent secure service requests
- NS to S priority inheritance
- NS privilege to be derived from CONTROL_NS register

**************
Design details
**************

NS thread awareness in TF-M Core
================================

Description
-----------

TrustZone context management API defines a set of secure function calls from NS
RTOS handler mode to TF-M Core to get notification of context switch.

While CMSIS context management can be used to directly expose secure context
management to the non-secure OS, TF-M has a proprietary implementation: the
context management API is used to get notification of NS context switches and
to track various non-secure clients.

.. _`API definition`:

API definition
--------------

TZ_MemoryId_t data type
^^^^^^^^^^^^^^^^^^^^^^^

TZ Memory ID identifies an allocated memory slot.

TF-M usage
""""""""""

``TZ_MemoryId_t`` is used for an index into an array containing active NS client
IDs. The memory ID is required by CMSIS to be a positive integer, so it is
mapped to the array index by being decremented by 1.

Signature
"""""""""

.. code-block:: c

    typedef uint32_t TZ_MemoryId_t;

Context management initialization: TZ_InitContextSystem_S
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Initialize secure context memory system.

Return value
""""""""""""

This function returns execution status: 1 for success, 0 for error.

TF-M usage
""""""""""

This function call is used to identify a non-secure RTOS that has TZ context
management capabilities, as this function is expected to be called before any
other TZ API function is used.

Signature
""""""""""

.. code-block:: c

    uint32_t TZ_InitContextSystem_S (void);

Context allocation: TZ_AllocModuleContext_S
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Allocate context memory for calling secure software modules in TrustZone.

Parameters
""""""""""

``module`` [input]: identifies software modules called from non-secure mode

Return value
""""""""""""

``value != 0`` TrustZone memory slot identifier
``value == 0`` no memory available or internal error

TF-M usage
""""""""""

This function is used to identify a new non-secure thread that may be identified
as a client in the non-secure domain. The ``module`` parameter is unused. The
returned ``TZ_MemoryId_t`` value is the index in the ``NsClientIdList`` array
where the client ID for the newly allocated context is stored.

Signature
"""""""""

.. code-block:: c

    TZ_MemoryId_t TZ_AllocModuleContext_S (TZ_ModuleId_t module);

Context freeing: TZ_FreeModuleContext_S
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Free context memory that was previously allocated with TZ_AllocModuleContext_S

Parameters
""""""""""

``id`` [input]: TrustZone memory slot identifier

Return value
""""""""""""

Execution status (1: success, 0: error)

TF-M usage
""""""""""

This function indicates that a non-secure client is inactive, meaning that any
subsequent references to the client ID are considered erroneous. In effect, the
client ID indexed by ``(id – 1)`` is cleared and the memory slot flagged as
free.

Signature
"""""""""

.. code-block:: c

    uint32_t TZ_FreeModuleContext_S (TZ_MemoryId_t id);

Context activation: TZ_LoadContext_S
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Load secure context (called on RTOS thread context switch)

Parameters
""""""""""

``id`` [input]: TrustZone memory slot identifier

Return value
""""""""""""

Execution status (1: success, 0: error)

TF-M usage
""""""""""

The client ID indexed by ``(id – 1)`` becomes the active NS client. Any
subsequent secure service requests coming from non-secure domain will be
associated with this client ID.

Signature
"""""""""

.. code-block:: c

    uint32_t TZ_LoadContext_S (TZ_MemoryId_t id);

Context deactivation: TZ_StoreContext_S
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Store secure context (called on RTOS thread context switch)

Parameters
""""""""""

``id`` [input]: TrustZone memory slot identifier

Return value
""""""""""""

Execution status (1: success, 0: error)

TF-M usage
""""""""""

The client ID indexed by ``(id – 1)`` becomes inactive. Any subsequent secure
service requests coming from non-secure domain will be invalid until a new NS
context is loaded.

Signature
"""""""""

.. code-block:: c

    uint32_t TZ_StoreContext_S (TZ_MemoryId_t id);

Security implications (to be assessed separately if needed)
-----------------------------------------------------------

If NS RTOS / NS handler mode is compromised, NS clients’ data can be disclosed
to unauthorised non-secure actors, as it’s not in the scope of TF-M to guarantee
non-secure client isolation. Support for this API is only an enabler for a
non-secure RTOS feature.

Vulnerabilities of the NS handler mode cannot and will not lead to disclosure of
assets owned by secure entities to non-secure actors after the introduction of
this feature as a malicious NS handler can only ever assume the identity of
another non-secure client and cannot elevate its access privileges to those of
secure clients.

Known client list
=================

Description
-----------

A different – but related – API to that defined by CMSIS is proposed in this
design to register a specific client ID to the active non-secure thread.

The purpose of this API is to provide non-secure privileged code with the
ability to associate the active non-secure context with a pre-defined identity.
This enables the application of a pre-set access policy on the secure side to be
applied to the non-secure thread.

Use cases
---------

It is valid for non-secure privileged code to only support the TF-M-specific API
defined below and not the CMSIS TZ API defined previously. In this case the
single non-secure client is still able to access resources based on a
pre-defined access policy in secure services without relying on the default
non-secure identity configured in TF-M.

If used in conjunction with the TZ API, this function can provide a means to
assign and identify multiple non-secure client IDs based on the active context,
overriding TF-M’s default non-secure client identity assignment policy.

API definition
--------------

NS RTOS client registration API – secure function calls from NS handler mode to
TF-M Core to associate a “known” Client ID to the active non-secure thread.

Register specific client ID: ``tfm_register_client_id``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Assign client ID to the current TZ context.

**Note**: This function must be called from handler mode so that TF-M can verify
that it was sent by a privileged entity.

This function call must follow all TZ_AllocModuleContext_S function calls to
override the default NS client IDs allocated by TF-M.

Secure and non-secure client IDs are allocated from different ranges (negative
IDs for non-secure clients, positive for secure clients). The function call is
rejected if called with a secure ID.

Parameters
""""""""""

``ns_client_id`` [input]: The client ID to be assigned to the current context

Return value
""""""""""""

``TFM_SUCCESS`` (0) if the client ID assigned successfully, a non-zero error
code in case of error.

Signature
"""""""""

.. code-block:: c

    enum tfm_status_e tfm_register_client_id (int32_t ns_client_id);

********************
Implementation notes
********************

Option to reduce required context switch notifications
======================================================

According to TrustZone API definition ``TZ_StoreContext_S()`` is to be called
"at thread context switch after running a thread" and ``TZ_LoadContext_S`` "at
thread context switch before running a thread". The API definition does not
define the course of action to be taken if two ``TZ_LoadContext_S()`` calls are
made without an interleaving StoreContext.

The proposal for TF-M is to accept this as a valid scenario where the second
``TZ_LoadContext_S()`` call is taken to imply a ``TZ_StoreContext_S()`` with
the previous active Memory_Id.

This assumption does not alter the intended use of ``TZ_StoreContext_S()``,
which remains a valid call with the behaviour as defined in the
`API definition`_ section above.

******************************************
Investigations, improvements, alternatives
******************************************

Concurrent secure service requests
==================================

If there are concurrent services requests, TF-M needs to identify the client for
each request and should make their corresponding context available in the secure
domain. Client ID needs to be associated with the secure service request so that
a NS context switch does not break client identification.

If a non-secure client is blocked on an asynchronous secure service completion,
the NS TFM library must provide a semaphore the NS thread can wait on, whereby
NS RTOS can schedule a different context.

Should a secure service completion happen for an inactive NS context, a
notification mechanism needs to be created to activate the given NS context.

The proposal is for the NS TFM library to include a NS IRQ handler for a
reserved interrupt signal. The ISR would identify the context to be activated
and release the corresponding semaphore.

NS to S priority inheritance
============================

Whether or not NS thread priorities should be influencing secure service
prioritization needs to be analysed. It is raised as a topic of discussion and
is not detailed in this document further at this stage.

NS privilege check for secure function calls
============================================

Non-secure privilege can be derived from CONTROL_NS instead of requiring NS to
call context management veneers in handler mode. This can be a more generic
approach, but implications are to be investigated.

**********
References
**********

Description of the TZ API:
https://www.keil.com/pack/doc/CMSIS/Core/html/group__context__trustzone__functions.html

*Copyright (c) 2019-2020, Arm Limited. All rights reserved.*