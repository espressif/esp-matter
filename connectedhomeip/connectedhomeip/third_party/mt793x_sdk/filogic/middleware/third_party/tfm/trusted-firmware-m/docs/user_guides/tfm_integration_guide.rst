#################
Integration guide
#################
The purpose of this document is to provide a guide on how to integrate TF-M
with other hardware platforms and operating systems.

*****************
How to build TF-M
*****************
Follow the :doc:`Build instructions <tfm_build_instruction>`.

********************************************************
How to export files for building non-secure applications
********************************************************
Explained in the :doc:`Build instructions <tfm_build_instruction>`.

*************************
How to add a new platform
*************************
The hardware platforms currently supported are:

- Soft Macro Model (SMM) Cortex-M33 SSE-200 subsystem for MPS2+ (AN521)
- Cortex-M23 IoT Kit subsystem for MPS2+ (AN519)
- Arm SSE-123 Example Subsystem for MPS2+ (AN539)
- Musca-A test chip board (Cortex-M33 SSE-200 subsystem)
- Musca-B1 test chip board (Cortex-M33 SSE-200 subsystem)
- Musca-S1 test chip board (Cortex-M33 SSE-200 subsystem)
- CoreLink SSE-200 Subsystem for MPS3 (AN524)
- DesignStart FPGA on Cloud: Cortex-M33 based platform (SSE-200_AWS)

The files related to the supported platforms are contained under the
``platform`` subfolder. The platform specific files are under
``platform/ext/target``, which is organised by boards
(e.g. ``platform/ext/target/mps2``), while the folder ``platform/ext/common``
is used to store source and header files which are platform generic.

More information about subsystems supported by the MPS2+ board can be found in:
`MPS2+ homepage <https://developer.arm.com/products/system-design/development-boards/fpga-prototyping-boards/mps2>`__

More information about the Musca-A test chip board can be found in:
`Musca-A homepage <https://developer.arm.com/products/system-design/development-boards/iot-test-chips-and-boards/musca-a-test-chip-board>`__

More information about the Musca-B1 test chip board can be found in:
`Musca-B1 homepage <https://www.arm.com/products/development-tools/development-boards/musca-b1-iot>`__

More information about the Musca-S1 test chip board can be found in:
`Musca-S1 homepage <https://www.arm.com/company/news/2019/05/arm-demonstrates-new-iot-test-chip-and-board>`__

More information about subsystems supported by the MPS3 board can be found in:
`MPS3 homepage <https://www.arm.com/products/development-tools/development-boards/mps3>`__

More information about the SSE-200_AWS platform can be found in:
`SSE-200_AWS product page <https://aws.amazon.com/marketplace/pp/ARM-DesignStart-FPGA-on-Cloud-Cortex-M33-based-pla/B082DMMTLW>`__

Generic drivers and startup/scatter files
=========================================
The addition of a new platform means the creation of a new subfolder inside
``target/<board_name>`` to provide an implementation of the drivers currently
used by TF-M, in particular MPC, PPC, and USART drivers. In addition to the
drivers, startup and scatter files need to be provided for the supported
toolchains.

There are also board specific drivers which are used by the board
platform to interact with the external world, for example during tests, that
have to be provided, e.g. to blink LEDs or count time in the MPS2 board.

.. Note::

    Currently SST and BL2 bootloader use different flash interface

Target configuration files
==========================
Inside the base root folder of the selected target, each implementation has to
provide its own copy of ``target_cfg.c/.h``. This file has target specific
configuration functions and settings that are called by the TF-M during the
platform configuration step during TF-M boot. Examples of the configurations
performed during this phase are the MPC configuration, the SAU configuration,
or eventually PPC configuration if supported by the hardware platform.
Similarly, the ``uart_stdout.c`` is used to provide functions needed to redirect
the stdout on UART (this is currently used by TF-M to log messages).

Platform retarget files
=======================
An important part that each new platform has to provide is the set of retarget
files which are contained inside the ``retarget`` folder. These files define the
peripheral base addresses for the platform, both for the secure and non-secure
aliases (when available), and bind those addresses to the base addresses used by
the devices available in the hardware platform.

***************************
How to integrate another OS
***************************
To work with TF-M, the OS needs to support the Armv8-M architecture and, in
particular, it needs to be able to run in the non-secure world. More
information about OS migration to the Armv8-M architecture can be found in the
:doc:`OS requirements <os_migration_guide_armv8m>`. Depending upon the system
configuration this may require configuring drivers to use appropriate address
ranges.

Interface with TF-M
===================
The files needed for the interface with TF-M are exported at the
``<build_dir>/install/export/tfm`` path. The NS side is only allowed to call
TF-M secure functions (veneers) from the NS Thread mode. For this reason, the
API is a collection of functions in the ``<build_dir>/install/export/tfm/inc``
directory. For example, the interface for the Secure STorage (SST) service
is described in the file ``psa_sst_api.h`` as a collection of functions that
call service veneer functions. This API is a wrapper for the secure veneers,
and returns the return value from the service to the caller.

The secure storage service uses a numerical ID, to identify the clients that use
the service. For details see
:doc:`ns client identification documentation <tfm_ns_client_identification>`.

Interface with non-secure world regression tests
================================================
A non-secure application that wants to run the non-secure regression tests
needs to call the ``tfm_non_secure_client_run_tests()``. This function is
exported into the header file ``test_framework_integ_test.h`` inside the
``<build_dir>/install`` folder structure in the test specific files,
i.e. ``<build_dir>/install/export/tfm/test/inc``. The non-secure regression
tests are precompiled and delivered as a static library which is available in
``<build_dir>/install/export/tfm/test/lib``, so that the non-secure application
needs to link against the library to be able to invoke the
``tfm_non_secure_client_run_tests()`` function. The SST non-secure side
regression tests rely on some OS functionality e.g. threads, mutexes etc. These
functions comply with CMSIS RTOS2 standard and have been exported as thin
wrappers defined in ``os_wrapper.h`` contained in
``<build_dir>/install/export/tfm/test/inc``. OS needs to provide the
implementation of these wrappers to be able to run the tests.

NS client Identification
========================
See
:doc:`ns client identification documentation <tfm_ns_client_identification>`.

*********************
Non-secure interrupts
*********************
Non-secure interrupts are allowed to preempt Secure thread mode.
With the current implementation, a NSPE task can spoof the identity of another
NSPE task. This is an issue only when NSPE has provisions for task isolation.
Note, that ``AIRCR.PRIS`` is still set to restrict the priority range available
to NS interrupts to the lower half of available priorities so that it wouldn't
be possible for any non-secure interrupt to preempt a higher-priority secure
interrupt.

--------------

*Copyright (c) 2017-2019, Arm Limited. All rights reserved.*
