.. _module-pw_interrupt:

------------
pw_interrupt
------------
Pigweed's interrupt module provides a consistent interface for to determine
whether one is currently executing in an interrupt context (IRQ or NMI) or not.

.. c:function:: bool InInterruptContext()

  Returns true if currently executing within an interrupt service routine
  handling an IRQ or NMI.:w!

