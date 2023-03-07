############################
Cooperative Scheduling Rules
############################

:Author: Ashutosh Singh
:Organization: Arm Limited
:Contact: Ashutosh Singh <ashutosh.singh@arm.com>
:Status: Accepted

TF-M Scheduler - Rules
======================

On ArmV8-M CPUs, NSPE and SPE share the same physical processing element(PE). A
TF-M enabled system need to be able to handle asynchronous events (interrupts)
regardless of current security state of the PE, and that may lead to scheduling
decisions. This introduces significant complexity into TF-M. To keep the
integrity of (NSPE and SPE) schedulers and call paths between NSPE and SPE,
following set of rules are imposed on the TF-M scheduler design.

Objectives and Requirements
===========================

1. Decoupling of scheduling decisions between NSPE and SPE
2. Efficient interrupt handling by SPE as well as NSPE
3. Reduce critical sections on the secure side to not block interrupts
   unnecessarily
4. Scalability to allow simplification/reduction of overheads to scale down the
   design for constrained devices
5. Reduce impact on NSPE software design

  a. NSPE interrupt handling implementation should be independent
  b. Impact on the NSPE scheduler should be minimal
  c. No assumptions should be made about NSPE's scheduling capabilities

Scheduler Rules for context switching between SPE and NSPE
==========================================================

To allow coherent cooperative scheduling, following set of rules are imposed on
security state changes.
The switching between SPE and NSPE can be triggered in multiple ways.

`Involuntary security state switch`; when the software has no control over the
switch:

- A NSPE interrupt take control into NSPE from SPE
- A SPE interrupt takes control into SPE from NSPE

`Voluntary security state switch`; when software programmatically makes the
switch:

- A NSPE exception handler returns from NSPE to pre-empted SPE context
- A SPE exception handler returns from SPE to pre-empted NSPE context
- NSPE makes a function call into SPE
- SPE returns a call from NSPE
- SPE makes a function call into NSPE (not covered in current design)
- NSPE returns a call from SPE (not covered in current design)

In order to maintain the call stack integrity across NSPE and SPE, following
rules are imposed on all security state switches.

Rules for NSPE Exception handling
---------------------------------

1. **The NSPE exception handler is allowed to trigger a NSPE context switch**
   **(regardless of security state of the preempted context.**

This is expected behaviour of any (RT)OS.

2. **The NSPE scheduler must eventually 'restore' the preempted (by**
   **exception) context.**

This is expected behaviour of any (RT)OS.

3. **If NSPE exception results in a NSPE context switch, SPM must be informed**
   **of the scheduling decision; this must be done BEFORE the execution of**
   **newly scheduled-in context.**

This is to ensures integrity of the call stack when SPE is ready to return a
previous function call from NSPE.

Rules for SPE Exception handling
--------------------------------

1. **All of the SPE interrupts must have higher priority than NSPE interrupts**

This is rule is primarily for simplifying the SPM design.

2. **The SPE interrupt handler is allowed to trigger a SPE context switch**
   **(regardless of security state of the pre-empted context)**

If the SPE context targeted by the interrupt is not same as current SPE context,
the SPM may choose to switch the current running SPE context based on priority.

3. **SPE scheduler must treat pre-empted context as one of the SPE contexts**

  a. If the pre-empted SPE context is SP1, the TCB for SP1 should be used for
     saving the context. i.e. the context of SP1 should be saved before
     scheduling anything other secure partition.
  b. If SP1 was pre-empted by a NSPE interrupt, and subsequent NSPE execution is
     pre-empted by SPE exception (before NSPE scheduling decision is communicated
     back to SPM) -- SP1 TCB must be used for saving the context
     In this case SPM is not yet aware of the NSPE context switch, from SPM's
     standpoint SP1 is still executing, so SPM assumes that the preempted context
     is SP1.
  c. If SP1 was pre-empted by a NSPE interrupt, and subsequent NSPE execution is
     pre-empted by SPE exception `after` NSPE scheduling decision is
     communicated back to SPM) - a TCB dedicated to NSPE should be used for
     saving the context.

  When NSPE scheduler communicates the scheduling decision to SPM, SPM must save
  the SP1 context, if a SPE interrupt preempts the currently running NSPE context,
  SPM should save the context to a dedicated NSPE TCB.

  d. The SPE scheduler must eventually 'restore' the pre-empted context.
  This is an expected behaviour of any scheduler.

4. **All of the interrupts belonging to a partition must have same priority.**

This serializes ISR execution targeted for same partition.

5. **In case of nested interrupts, all of the ISRs must run to finish before**
   **any service code is allowed to run**

This is an expected behaviour of any scheduler.

6. **If the previously preempted context was a NSPE ISR context, SPE ISR**
   **handler must return to preempted NSPE context.**

This is an expected behaviour of any scheduler to return to preempted context.

Rules for NSPE to SPM function call (and NSPE scheduler)
--------------------------------------------------------

1. Current NSPE context must have been communicated to SPM, otherwise SPM cannot
   guarantee NSPE function calling stack integrity.

Rules for Function Return from SPE to NSPE with result
------------------------------------------------------

1. **The result available on SPE side are for currently active NSPE context.**

To maintain call stack integrity, if SPE is ready to return to NSPE, it can do
function return only if the SPE return path corresponds to currently active NSPE
context.

2. **Last entry into secure world happened programmatically (Voluntary**
   **security state switch into SPE)**

i.e. control is voluntarily given back by NSPE, either through a function call,
or a context restore via 'return to SPE from NSPE'. As opposed to a SPE
interrupt bringing back the execution into SPE.

3. **The current NSPE call stack has not already been returned with SPM_IDLE.**

This rule applies if following optional feature is enabled.

Rules for Return from SPE to NSPE with SPM_IDLE
-----------------------------------------------

This is optional part of the design as it introduces significant complexity on
both sides of the security boundary.
It allows yielding of the CPU to NSPE when SPE has not CPU execution to do but
it has not yet finished the previous request(s) from NSPE; i.e. SPE is waiting
on arrival of a SPE interrupt.

1. **Last entry into secure world happens programmatically (Voluntary**
   **security context switch into SPE)**

i.e. control is voluntarily given back by NSPE, either through a function call,
or a context restore via 'return to SPE from NSPE'. As opposed to a SPE
interrupt bringing back the execution into SPE.

2. **The result for the currently active NSPE entity is not yet available,**
   **the called service is waiting (on interrupt/event).**

SPE request corresponding to currently active NSPE caller is not yet completed
and is waiting on an ISR.

3. **The current NSPE call stack has not already been returned with SPM_IDLE.**

Rules for NSPE pend irq based return from SPE to NSPE
-----------------------------------------------------

This is optional part of the design as it introduces significant complexity on
both sides. This works in conjunction with [Rules for Return from SPE to NSPE
with SPM_IDLE](#rules-for-return-from-spe-to-nspe-with-spm_idle).
In this scenario, when SPE is ready with result for a previous call from NSPE,
it raises a pended IRQ to NSPE instead of returning the function call path.

1. **The SPE has finished a NSPE request.**

2. **The corresponding NSPE context has already been returned with SPM_IDLE.**

Rules for ISR pre-emption
-------------------------

1. **A higher priority NSPE interrupt is allowed to preempt a lower priority**
   **NSPE ISR**

2. **A higher priority SPE interrupt is allowed to preempt a lower priority**
   **SPE ISR**

3. **A SPE interrupt is allowed to preempt NSPE ISR**

4. **A NSPE interrupt is not allowed to preempt SPE ISR**

5. **All interrupts belonging to a service must have same priority**

--------------

*Copyright (c) 2019, Arm Limited. All rights reserved.*
