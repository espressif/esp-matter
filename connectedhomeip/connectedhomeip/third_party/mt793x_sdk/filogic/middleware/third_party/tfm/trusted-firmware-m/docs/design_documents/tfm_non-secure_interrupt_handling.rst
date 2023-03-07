#############################
Non-Secure Interrupt Handling
#############################

:Author: Mate Toth-Pal
:Organization: Arm Limited
:Contact: Mate Toth-Pal <mate.toth-pal@arm.com>
:Status: Accepted


*****
Terms
*****

==========  ================================================
Term        Meaning
==========  ================================================
AIRCR       Application Interrupt and Reset Control Register
AIRCR.PRIS  PRIoritize Secure exceptions
ISR         Interrupt Service Routine
NS          Non-Secure
NSPM        Non-Secure Partition Manager
SAU         Security Attribution Unit
SPM         Secure Partition Manager
TF-M        Trusted Firmware-M
==========  ================================================

************
Introduction
************

In the current design it is possible to use Non-secure interrupts, however the
Non-secure interrupts cannot pre-empt Secure service execution. TF-M core
achieves this by making the following configurations:

1. ``AIRCR.PRIS`` is set to 1 during TF-M core initialisation. This
   de-prioritizes Non-secure exceptions compared to Secure exceptions, so that
   they cannot interupt Secure Handler mode. the ``AIRCR.PRIS`` bit remains set
   during TF-M run. The bit is set in the function

.. code-block:: c

    static int32_t tfm_core_set_secure_exception_priorities(void);

.. Note::

    Setting ``AIRCR.PRIS`` in itself doesn't prevent NS interrupts to pre-empt
    Secure Thread mode when it runs on normal priority i.e., 256.

2. On Secure service entry ``PRIMASK_NS`` is set, to boost the Non-secure
   execution priority so that all NS interrupts are masked. This is done in the
   ``TFM_NS_EXC_DISABLE()`` macro called from

.. code-block:: c

    static int32_t tfm_start_partition(struct tfm_sfn_req_s *desc_ptr, uint32_t excReturn)

.. Note::

    '2.' is only in Library model.

In the below chapters a design is proposed to enable Non-secure interrupts to
pre-empt Secure Thread mode.

**********************************
Limitations of the proposed design
**********************************

Library model
=============

The proposed design keeps the Secure lock in place, which means Non-secure code
can do a single Secure service call, all further calls to Secure services will
be rejected until the first Secure service call returns.

IPC model
=========

The PSA client API can only be entered once. All the functions in the client API
(``psa_framework_version``, ``psa_version``, ``psa_connect``, ``psa_call``,
``psa_close``) are part of the same critical section.

Non-secure software
===================

The Non-secure API functions that wraps the Secure service veneers (either
Library or IPC model) should continue to use the NS locking mechanism currently
implemented by calling ``tfm_ns_lock_dispatch(...)``.

If any of the Non-secure software decides to bypass the locking mechanism, then
concurrent access of veneer functions is detected by TF-M, and NS software
execution is halted.

.. Note::

    This makes denial of service attack possible by a malicious NS application

***************************************************
Enabling NS interrupts to pre-empt Secure execution
***************************************************

To enable NS interrupts, 2) described in chapter 'Current design' must be turned
off. (For details see implementation notes)

When a Non-secure interrupt is triggered during Secure code execution, and the
ISR have to be executed based on the priority settings, the hardware saves the
current execution context on the current Secure stack, and clears the general
purpose registers, to prevents data leakage. After that the NS ISR starts
execution.

When the Non-secure ISR returns with the EXC_RETURN value provided to it in the
link register, the context is fetched from the Secure stack, and the Secure code
continues execution.

If TF-M is used with a single threaded NS software, the mechanisms provided by
the HW is enough to maintain the consistency of the system, and keep the
secrets.

However if the NS software is allowed to change execution context during an
interrupt (e.g an NS operating system schedules another thread during a SysTick
interrupt), then the Secure code can return execution to an NS thread, with the
context of a different thread. So extra measures needs to be introduced in TF-M
to prevent this.

For IPC model
=============

In the current implementation there is no locking mechanism on the Secure side
that would prevent the Non-secure code to enter psa_client functions multiple
times. (For the Library model the ``tfm_secure_lock global`` variable is used
for this purpose). Note, that the ``tfm_ns_lock_dispatch(...)`` function that
is used by the NS service API implementations to prevent Secure services to be
called simultaneously can be bypassed by a malicious Non-secure application, so
a Secure side locking mechanism have to be implemented.

When an NS client calls a PSA client API function, the client ID of the calling
NS context have to be saved, and execution can only return to NS if the current
scheduled NS thread is the one that did the call.

For Library model
=================

As currently there is no scheduling in the Library model, the calls follow each
other just like in an ordinary function call scheme. Then when the original
Secure service that was called from the NS code is about to return, it has to
check for the current NS client ID, and only return if it is the same as the one
saved on Secure service entry from NS. If the ID's don't match, the Secure side
waits so that NS OS can do context switch.

Common measures
===============

Exception priorities
--------------------

The priority of the Secure SVC and the Secure faults must be higher than any
Secure exception in the system. Once this is done, the following fixme can be
removed from ``secure_fw\core\tfm_core.c``:
``tfm_core_set_secure_exception_priorities``:

.. code-block:: c

    /* FixMe: Explicitly set secure fault and Secure SVC priority to highest */

.. note::

    The priority of PendSV Is set to be the lowest priority Secure interrupt,
    but still higher than the maximum possible NS execution priority when
    ``AIRCR.PRIS`` is set.

NSPM
----

If the Non-secure software allows the use of multiple threads, it needs to use
the NSPM feature of TF-M. It is expected, that all the NS context that use
Secure services have a unique client ID, and the other contexts, that don't use
Secure service need to have a client ID that doesn't match with any of the
client IDs of the Secure service calling contexts.

In other words, for all the *cs(0)*, *cs(1)*, ..., *cs(n)* NS contexts that use
Secure services and for all *cn(0)*, *cn(1)*, ..., *cn(m)* NS contexts that
don't use Secure service (where *n* > 0, *m* >= 0):

- *cs(i).client_id* != *cs(j).client_id* (where 0 <= *i* < *j* <= *n*)
- *cs(i).client_id* != *cn(j).client_id* (where 0 <= *i* <= *n* and 0 <= *j*
  <= *m*)

Entering from Non-secure to Secure
----------------------------------

The Secure code can be entered through the following gateways:

1. NSPM related functions (``TZ_<operation>(...)``,
   ``tfm_register_client_id(...)``)

   These functions are expected to be called from Handler mode. The execution
   priority, after the execution crosses the security boundary will be the same
   as it was during NS execution. This means a malicious Non-secure application,
   can set up Non-secure interrupt priorities in a way that it can enter one or
   more of the NSPM APIs simultaneously.

   This might leave the NSPM database in an inconsistent state, however if the
   attacker has influence over the interrupt priorities, they can gain no
   additional privilege by this.

   .. Note::
       The NS software is able to consume the main stack of the Secure software.
       The Main Secure stack have to be protected by MSPLIM, to prevent stack
       overflow. However a denial of service attack is still possible.

2. PSA Client API, Library model service veneers

   When a veneer is called from Non-secure, the Secure code have to check
   whether the veneer is only entered by a single NS thread. This can be done by
   checking the veneer stack usage. It can only contain the locals of the veneer
   implementation. If the veneer has been entered from multiple NS threads,
   there is at least one extra context stack frame that was created by the
   hardware when the veneer execution had been interrupted by the NS systick.

********************
Implementation notes
********************

IPC model
=========

Save NS client ID on Secure service veneer entry
------------------------------------------------

As long as the Secure lock is in place, a single client ID have to be stored, so
it can be done in a global variable.

The caller client ID can be saved in the function
``void tfm_psa_ipc_request_handler(uint32_t svc_ctx[])`` depending on the return
value of the PSA API function. (Doesn't execute any Secure service code, only
sets signals, and triggrs scheduling. If the return value is success, that means
a scheduling is to happen, and a secure service is about to be entered.)

Check client ID on Secure service return
----------------------------------------

The saved client ID can be compared with the current client ID in the function
``tfm_core_ns_ipc_request``, after the SVC return. Before doing the comparison,
``BASEPRI_NS`` must be set to 1.

The original ``BASEPRI_NS`` value can be stored in a global variable (because of
the single context).

If the client ID's don't match, ``BASEPRI_NS`` must be reset, WFI to be issued,
and start the checking sequence from the beginning.

Library model
=============

Save NS client ID on Secure veneer entry
----------------------------------------

As long as the Secure lock is in place, only a single client ID have to be
stored, so it can be done in a global variable.

The caller client ID can be saved in the function
``uint32_t tfm_core_partition_request_svc_handler(uint32_t *svc_args, uint32_t excReturn)``.

Check client ID on SP return
----------------------------

The saved client ID can be compared with the current client ID in the function
``tfm_core_partition_request``, after the ``tfm_core_sfn_request return``.

If the client ID's don't match, WFI to be issued, and the checking sequence have
to be started from the beginning.

Common
======

Enforce single NS entry to Secure
---------------------------------

On Secure service entry (from the SVC implementation) check that (pseudocode)

.. code-block:: c

    svc_handler()
    {
        /* If there are multiple context stacked in veneer stack, hang NSPE */
        expected_sp_top = veneer_stack_addr -
                            sizeof(svc_state_context) + sizeof(locals);
        if (__get_PSP() != expected_sp_top) {
            /* Multiple frames are existing, panic */
            panic();
        }
    }

*******
Testing
*******

Basic scenario
==============

Basic testing of the feature is possible, by adding a new scenario to the
existing IRQ test. The flow of the test would be something like this:

============  =====================  ===========================================
IRQ_TEST_1    prepare test scenario  Do nothing
CORE_TEST_2   prepare test scenario  Do nothing
NS            prepare test scenario  Initialise and start timer
IRQ_TEST_1    execute test scenario  Do nothing
CORE_TEST_2   execute test scenario  Busy wait until NS interrupt is
                                     acknowledged (a flag in non Secure data is
                                     set) set flag CORE_TEST_2 waits on
NS            execute test scenario  Do nothing
============  =====================  ===========================================

The test is successful if NS execute test scenario returns

Advanced scenarios
==================

Testing advanced scenarios (that involves NS scheduling during secure execution,
NS interrupting Secure interrupt, Secure interrupting NS interrupt) would
require more advanced test framework and are not covered in this proposal.

*Copyright (c) 2019-2020, Arm Limited. All rights reserved.*
