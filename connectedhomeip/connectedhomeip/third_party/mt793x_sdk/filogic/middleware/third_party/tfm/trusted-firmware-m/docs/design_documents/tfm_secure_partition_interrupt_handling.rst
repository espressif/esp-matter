###################################
Secure Partition Interrupt Handling
###################################

:Author: Miklos Balint
:Organization: Arm Limited
:Contact: Miklos Balint <miklos.balint@arm.com>
:Status: Accepted

Secure Partitions that implement peripheral drivers may need to use interrupts
to efficiently manage PE utilization.

************************
Partition implementation
************************

IRQ lines declared in manifest files for Secure Partitions are assigned IRQ
signals as described in PSA Firmware Framework.

Interrupt handling is implemented as interrupt service routines (Partition ISR)
that are executed in the partition context.

Partitions that are owners of IRQ must define interrupt service routines for
them.

manifest file IRQ declaration example
=====================================

.. code-block:: json

    "irqs": [
    {
        "line_num": 17,
        "signal": "RTC"
    },
    {
        "line_name": "UART1_IRQ",
        "signal": "UART1"
    }
    ]

Partition ISR function
======================

The symbol name of the ISR implemented for a given interrupt must be
``manifest[‘irqs’][idx].signal`` attribute post-fixed with "_isr".

Partition ISR has the signature of a regular interrupt service routine, e.g.:

.. code-block:: c

    void UART1_isr(void);

*************
SPM behaviour
*************

In the vector table, each interrupt that has an associated partition ISR is
assigned to an SPM interrupt handler (SPM ISR) that delegates handling to a
Partition ISR.

Each partition is allocated an asserted signal mask. Every IRQ associated with
the partition is assigned a position in the signal mask.

When a secure hardware interrupt is asserted, the SPM:

- Acknowledges the interrupt and masks the hardware interrupt line.

- Identifies the Secure Partition which has registered the interrupt (in the
  manifest). This identification can happen either

  - using a runtime lookup or

  - by registering different instances of the SPM ISR for each interrupt, so the
    runtime lookup is avoided both for the partition Id and the ISR function
    location.

- Sets up execution environment for the secure partition that has registered the
  interrupt.

- Asserts the IRQ signal for the interrupt in the partition's signal mask.

- Execute Partition ISR. If the partition’s stack is active at the time of
  pre-emption by the interrupt (i.e. the Secure Partition is not in idle state),
  the Partition ISR stack frame will be amended to that. If the Secure Partition
  had been idle, a stack frame will be reserved for the duration of the
  Partition ISR execution. The Secure Partition state is changed to running for
  the duration of the Partition ISR.

When the Secure Partition ISR returns, execution is returned to the context
pre-empted by the IRQ.

Implementation notes
====================

Interrupts can pre-empt NSPE or secure service execution. Pre-emption of an
interrupt is only possible by an interrupt of a higher priority, ensuring
deterministic nesting/un-nesting of stack frames.

*************
API functions
*************

psa_wait()
==========

A call to ``psa_wait()`` from a secure service yields execution to the framework
and becomes blocked waiting for the assertion of a signal. In the meantime, SPM
will schedule other contexts that are ready to run. The client remains blocked
until the service function returns.

If an IRQ signal matching one in ``signal_mask`` to ``psa_wait()`` is asserted,
the Secure Partition becomes ready to run. When the scheduler activates the
Secure Partition, the IRQ signal(s) that had been asserted are returned by
``psa_wait()``. When the service function completes its execution and returns,
control is taken back to client.

.. Note::

    The only signals implemented in the current TF-M implementation are
    interrupt signals.

Signature
---------

.. code-block:: c

    psa_signal_t psa_wait(psa_signal_t signal_mask, uint32_t timeout);

Parameters
----------

``psa_signal_t signal_mask`` defines the set of interrupt signals that can
resume execution of the secure service.

``uint32_t timeout`` defines timeout for the function, as defined in PSA
Firmware Framework 1.0-beta-0 (Chapter 4.3.3).

Return
------

The return value indicates the signal(s) that triggered the resumption of the
service; i.e. If multiple interrupt events have been handled, it will be
indicated by the mask value in the return code.

tfm_enable_irq()
================

A call to ``tfm_enable_irq()`` from a secure service enables an irq.

Signature
---------

.. code-block:: c

    void tfm_enable_irq(psa_signal_t irq_signal);

Parameters
----------

``psa_signal_t irq_signal`` defines the interrupt signal to be enabled.

Return
------

``void`` Success.

Does not return: The call is invalid, one or more of the following are true:

- irq_signal is not an interrupt signal.
- irq_signal indicates more than one signal.

tfm_disable_irq()
=================

A call to ``tfm_disable_irq()`` from a secure service disables an irq.

Signature
---------

.. code-block:: c

    void tfm_disable_irq(psa_signal_t irq_signal);

Parameters
----------
``psa_signal_t irq_signal`` defines the interrupt signal to be disabled.

Return
------

``void``: Success.

Does not return: The call is invalid, one or more of the following are true:

- irq_signal is not an interrupt signal.
- irq_signal indicates more than one signal.

psa_eoi()
=========

A call ``to psa_eoi()`` from a secure service function or a Partition ISR
informs SPM that an interrupt has been processed. This clears the IRQ signal in
the asserted signal mask associated with the partition.

Signature
---------

.. code-block:: c

    void psa_eoi(psa_signal_t irq_signal);

Parameters
----------

``psa_signal_t irq_signal`` defines the interrupt signal that has been
processed.

Return
------

``void``: Success.

Does not return: The call is invalid, one or more of the following are true:

- ``irq_signal`` is not an interrupt signal.
- ``irq_signal`` indicates more than one signal.
- ``irq_signal`` is not currently asserted.

*Copyright (c) 2019-2020, Arm Limited. All rights reserved.*