.. _module-pw_cpu_exception_cortex_m:

=========================
pw_cpu_exception_cortex_m
=========================
This backend provides an implementations for the CPU exception module frontend
for the following Cortex-M architectures:

* ARMv7-M - Cortex M3
* ARMv7-EM - Cortex M4, M7
* ARMv8-M Mainline - Cortex M33, M33P

Setup
=====
There are a few ways to set up the Cortex M exception handler so the
application's exception handler is properly called during an exception.

**1. Use existing CMSIS functions**
  Inside of CMSIS fault handler functions, branch to ``pw_cpu_exception_Entry``.

  .. code-block:: cpp

    __attribute__((naked)) void HardFault_Handler(void) {
    asm volatile(
        " ldr r0, =pw_cpu_exception_Entry  \n"
        " bx r0                            \n");
    }

**2. Modify a startup file**
  Assembly startup files for some microcontrollers initialize the interrupt
  vector table. The functions to call for fault handlers can be changed here.
  For ARMv7-M and ARMv8-M, the fault handlers are indexes 3 to 6 of the
  interrupt vector table. It's also may be helpful to redirect the NMI handler
  to the entry function (if it's otherwise unused in your project).

  Default:

  .. code-block:: cpp

    __isr_vector_table:
      .word  __stack_start
      .word  Reset_Handler
      .word  NMI_Handler
      .word  HardFault_Handler
      .word  MemManage_Handler
      .word  BusFault_Handler
      .word  UsageFault_Handler

  Using CPU exception module:

  .. code-block:: cpp

    __isr_vector_table:
      .word  __stack_start
      .word  Reset_Handler
      .word  pw_cpu_exception_Entry
      .word  pw_cpu_exception_Entry
      .word  pw_cpu_exception_Entry
      .word  pw_cpu_exception_Entry
      .word  pw_cpu_exception_Entry

  Note: ``__isr_vector_table`` and ``__stack_start`` are example names, and may
  vary by platform. See your platform's assembly startup script.

**3. Modify interrupt vector table at runtime**
  Some applications may choose to modify their interrupt vector tables at
  runtime. The exception handler works with this use case (see the
  exception_entry_test integration test), but keep in mind that your
  application's exception handler will not be entered if an exception occurs
  before the vector table entries are updated to point to
  ``pw_cpu_exception_Entry``.

Module Usage
============
For lightweight exception handlers that don't need to access
architecture-specific registers, using the generic exception handler functions
is preferred.

However, some projects may need to explicitly access architecture-specific
registers to attempt to recover from a CPU exception. ``pw_cpu_exception_State``
provides access to the captured CPU state at the time of the fault. When the
application-provided ``pw_cpu_exception_DefaultHandler()`` function returns, the
CPU state is restored. This allows the exception handler to modify the captured
state so that execution can safely continue.

Expected Behavior
-----------------
In most cases, the CPU state captured by the exception handler will contain the
basic register frame in addition to an extended set of registers
(see ``cpu_state.h``).

The exception to this is when the program stack pointer is in an MPU-protected
or otherwise invalid memory region when the CPU attempts to push the exception
register frame to it. In this situation, the PC, LR, and PSR registers will NOT
be captured and will be marked with ``0xFFFFFFFF`` to indicate they are invalid.
This backend will still be able to capture all the other registers though.

``0xFFFFFFFF`` is an illegal LR value, which is why it was selected for this
purpose. PC and PSR values of 0xFFFFFFFF are dubious too, so this constant is
clear enough at suggesting that the registers weren't properly captured.

In the situation where the main stack pointer is in a memory protected or
otherwise invalid region and fails to push CPU context, behavior is undefined.

Nested Exceptions
-----------------
To enable nested fault handling:

1. Enable separate detection of usage/bus/memory faults via the SHCSR.
2. Decrease the priority of the memory, bus, and usage fault handlers. This
   gives headroom for escalation.

While this allows some faults to nest, it doesn't guarantee all will properly
nest.

Configuration Options
=====================

- ``PW_CPU_EXCEPTION_CORTEX_M_EXTENDED_CFSR_DUMP``: Enable extended logging in
  ``pw::cpu_exception::LogCpuState()`` that dumps the active CFSR fields with
  help strings. This is disabled by default since it increases the binary size
  by >1.5KB when using plain-text logs, or ~460 Bytes when using tokenized
  logging. It's useful to enable this for device bringup until your application
  has an end-to-end crash reporting solution.
- ``PW_CPU_EXCEPTION_CORTEX_M_LOG_LEVEL``: The log level to use for this module.
  Logs below this level are omitted.

Exception Analysis
==================
This module provides Python tooling to analyze CPU state captured by a Cortex-M
core during an exception. This can be useful as part of a crash report analyzer.

CFSR decoder
------------
The ARMv7-M and ARMv8-M architectures have a Configurable Fault Status Register
(CFSR) that explains what illegal behavior caused a fault. This module provides
a simple command-line tool to decode CFSR contents (e.g. 0x00010000) as
human-readable information (e.g. "Encountered invalid instruction").

For example:

  .. code-block::

    $ python -m pw_cpu_exception_cortex_m.cfsr_decoder 0x00010100
    20210412 15:11:14 INF Exception caused by a usage fault, bus fault.

    Active Crash Fault Status Register (CFSR) fields:
    IBUSERR     Instruction bus error.
        The processor attempted to issue an invalid instruction. It
        detects the instruction bus error on prefecting, but this
        flag is only set to 1 if it attempts to issue the faulting
        instruction. When this bit is set, the processor has not
        written a fault address to the BFAR.
    UNDEFINSTR  Encountered invalid instruction.
        The processor has attempted to execute an undefined
        instruction. When this bit is set to 1, the PC value stacked
        for the exception return points to the undefined instruction.
        An undefined instruction is an instruction that the processor
        cannot decode.

    All registers:
    cfsr       0x00010100

.. note::
  The CFSR is not supported on ARMv6-M CPUs (Cortex M0, M0+, M1).

--------------------
Snapshot integration
--------------------
This ``pw_cpu_exception`` backend provides helper functions that capture CPU
exception state to snapshot protos.

SnapshotCpuState()
==================
``SnapshotCpuState()`` captures the ``pw_cpu_exception_State`` to a
``pw.cpu_exception.cortex_m.ArmV7mCpuState`` protobuf encoder.


SnapshotMainStackThread()
=========================
``SnapshotMainStackThread()`` captures the main stack's execution thread state
if active either from a given ``pw_cpu_exception_State`` or from the current
running context. It captures the thread name depending on the processor mode,
either ``Main Stack (Handler Mode)`` or ``Main Stack (Thread Mode)``. The stack
limits must be provided along with a stack processing callback. All of this
information is captured by a ``pw::thread::Thread`` protobuf encoder.

.. note::
  We recommend providing the ``pw_cpu_exception_State``, for example through
  ``pw_cpu_exception_DefaultHandler()`` instead of using the current running
  context to capture the main stack to minimize how much of the snapshot
  handling is captured in the stack.

Python processor
================
This module's included Python exception analyzer tooling provides snapshot
integration via a ``process_snapshot()`` function that produces a multi-line
dump from a serialized snapshot proto, for example:

.. code-block::

  Exception caused by a usage fault.

  Active Crash Fault Status Register (CFSR) fields:
  UNDEFINSTR  Undefined Instruction UsageFault.
      The processor has attempted to execute an undefined
      instruction. When this bit is set to 1, the PC value stacked
      for the exception return points to the undefined instruction.
      An undefined instruction is an instruction that the processor
      cannot decode.

  All registers:
  pc         0x0800e1c4 example::Service::Crash(_example_service_CrashRequest const&, _pw_protobuf_Empty&) (src/example_service/service.cc:131)
  lr         0x0800e141 example::Service::Crash(_example_service_CrashRequest const&, _pw_protobuf_Empty&) (src/example_service/service.cc:128)
  psr        0x81000000
  msp        0x20040fd8
  psp        0x20001488
  exc_return 0xffffffed
  cfsr       0x00010000
  mmfar      0xe000ed34
  bfar       0xe000ed38
  icsr       0x00000803
  hfsr       0x40000000
  shcsr      0x00000000
  control    0x00000000
  r0         0xe03f7847
  r1         0x714083dc
  r2         0x0b36dc49
  r3         0x7fbfbe1a
  r4         0xc36e8efb
  r5         0x69a14b13
  r6         0x0ec35eaa
  r7         0xa5df5543
  r8         0xc892b931
  r9         0xa2372c94
  r10        0xbd15c968
  r11        0x759b95ab
  r12        0x00000000

Module Configuration Options
============================
The following configurations can be adjusted via compile-time configuration of
this module, see the
:ref:`module documentation <module-structure-compile-time-configuration>` for
more details.

.. c:macro:: PW_CPU_EXCEPTION_CORTEX_M_LOG_LEVEL

  The log level to use for this module. Logs below this level are omitted.

  This defaults to ``PW_LOG_LEVEL_DEBUG``.

.. c:macro:: PW_CPU_EXCEPTION_CORTEX_M_EXTENDED_CFSR_DUMP

  Enables extended logging in pw::cpu_exception::LogCpuState() and
  pw::cpu_exception::cortex_m::LogExceptionAnalysis() that dumps the active
  CFSR fields with help strings. This is disabled by default since it
  increases the binary size by >1.5KB when using plain-text logs, or ~460
  Bytes when using tokenized logging. It's useful to enable this for device
  bringup until your application has an end-to-end crash reporting solution.

  This is disabled by default.
