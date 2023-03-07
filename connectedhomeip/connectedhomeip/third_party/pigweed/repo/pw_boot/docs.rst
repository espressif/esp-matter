.. _module-pw_boot:

-------
pw_boot
-------
Pigweed's boot module should provide a linker script and some early
initialization of static memory regions and C++ constructors. This is enough to
get many targets booted and ready to run C++ code.

This module is split into two components:

1. The facade (this module) which declares the symbols exported by the backend
2. The backend, provided elsewhere, that provides the implementation

.. warning::
  This module is currently NOT stable! Depending on this module may cause
  breakages as this module is updated.

Sequence
========
The high level pw_boot boot sequence looks like the following pseudo-code
invocation of the user-implemented functions:

.. code:: cpp

  void pw_boot_Entry() {  // Boot entry point provided by backend.
    pw_boot_PreStaticMemoryInit();  // User-implemented function.
    // Static memory initialization.
    pw_boot_PreStaticConstructorInit();  // User-implemented function.
    // C++ static constructors are invoked.
    pw_boot_PreMainInit();  // User-implemented function.
    main();  // User-implemented function.
    pw_boot_PostMain();  // User-implemented function.
    PW_UNREACHABLE;
  }

Setup
=====

User-Implemented Functions
--------------------------
This module expects all of these extern "C" functions to be defined outside this
module:

 - ``int main()``: This is where applications reside.
 - ``void pw_boot_PreStaticMemoryInit()``: This function executes just before
   static memory has been zeroed and static data is intialized. This function
   should set up any early initialization that should be done before static
   memory is initialized, such as:

   - Enabling the FPU or other coprocessors.
   - Opting into extra restrictions such as disabling unaligned access to ensure
     the restrictions are active during static RAM initialization.
   - Initial CPU clock, flash, and memory configurations including potentially
     enabling extra memory regions with .bss and .data sections, such as SDRAM
     or backup powered SRAM.
   - Fault handler initialization if required before static memory
     initialization.

   .. warning::
     Code running in this hook is violating the C spec as static values are not
     yet initialized, meaning they have not been loaded (.data) nor
     zero-initialized (.bss).

 - ``void pw_boot_PreStaticConstructorInit()``: This function executes just
   before C++ static constructors are called. At this point, other static memory
   has been zero or data initialized. This function should set up any early
   initialization that should be done before C++ static constructors are run,
   such as:

   - Run time dependencies such as Malloc, and ergo sometimes the RTOS.
   - Persistent memory that survives warm reboots.
   - Enabling the MPU to catch nullptr dereferences during construction.
   - Main stack watermarking.
   - Further fault handling configuration necessary for your platform which
     were not safe before pw_boot_PreStaticRamInit().
   - Boot count and/or boot session UUID management.

 - ``void pw_boot_PreMainInit()``: This function executes just before main, and
   can be used for any device initialization that isn't application specific.
   Depending on your platform, this might be turning on a UART, setting up
   default clocks, etc.

 - ``PW_NO_RETURN void pw_boot_PostMain()``: This function executes after main
   has returned. This could be used for device specific teardown such as an
   infinite loop, soft reset, or QEMU shutdown. In addition, if relevant for
   your application, this would be the place to invoke the global static
   destructors. This function must not return!


If any of these functions are unimplemented, executables will encounter a link
error.

Backend-Implemented Functions
-----------------------------
Backends for this module must implement the following extern "C" function:

 - ``void pw_boot_Entry()``: This function executes as the entry point for
   the application, and must perform call the user defined methods in the
   appropriate sequence for the target (see Sequence above).

Dependencies
============
  * ``pw_preprocessor`` module
