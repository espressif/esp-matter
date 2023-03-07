##########################
Change Log & Release Notes
##########################

This document contains a summary of the new features, changes, fixes and known
issues in each release of Trusted Firmware-M.

***********
Version 1.0
***********

New Features
============
-  First major release.

-  A Secure FW with support for PSA Level 1 and 2 isolation on Armv8-M
   using TrustZone extension and Dual-core Cortex-M config.

-  The PSA Firmware Framework (PSA FF)/Dev API interfaces exposed by the
   Secure FW to NS side.

-  A secure FW model with NS application example.

-  Secure services running within this SPE

    -  Secure Storage Service (PSA Protected Storage API - 1.0.0)
    -  Attestation (PSA Attestation API 1.0.0)
    -  Crypto Service (PSA API 1.0-beta-3)
    -  TF-M Audit Log
    -  Platform Service
    -  Internal Trusted Storage (PSA API 1.0.0)

-  PSA IPC support

-  Support for Armv8-M mainline and baseline and Dual-core Cortex-M systems.

-  Testcases running baremetal and with RTX to test the functionality.

-  BL2 bootloader for image authentication based on SHA256 and RSA-3072
   digital signature.

-  Build system based on CMake, supporting ARMCLANG and GNU Arm.

-  Support for integrated CryptoCell-312 cryptographic hardware accelerator
   on Musca-B1 platform.

-  Meets requirements for Updatable RoT for PSA Functional API, Level 1 and
   Level 2 Certifications in the feature list.

Platforms supported
===================
Current release has been tested on:

    - Cortex M33 based SSE-200 system:

        - `FPGA image loaded on MPS2 board.
          <https://developer.arm.com/products/system-design/development-boards/cortex-m-prototyping-systems/mps2>`__
        - `Fast model FVP_MPS2_AEMv8M.
          <https://developer.arm.com/products/system-design/fixed-virtual-platforms>`__
        - `Musca-A test chip board.
          <https://developer.arm.com/products/system-design/development-boards/iot-test-chips-and-boards/musca-a-test-chip-board>`__
        - `Musca-B1 test chip board.
          <https://developer.arm.com/products/system-design/development-boards/iot-test-chips-and-boards/musca-b-test-chip-board>`__
        - `Musca-S1 test chip board.`
        - `FPGA image loaded on MPS3 board.
          <https://developer.arm.com/tools-and-software/development-boards/fpga-prototyping-boards/mps3>`__
        - `Arm DesignStart FPGA on AWS Cloud.
          <https://developer.arm.com/docs/101965/0102/arm-designstart-fpga-on-cloud-arm-ds-getting-started>`__

    - Cortex M23 based IoT Kit system:

       - `FPGA image loaded on MPS2 board.
         <https://developer.arm.com/products/system-design/development-boards/cortex-m-prototyping-systems/mps2>`__

Other supported platforms:

    - Dual Core Cortex-M system:

        - `Cypress PSoc64.
          <https://www.cypress.com/documentation/product-brochures/cypress-psoc-64-secure-microcontrollers>`__

Platform Limitations
====================
- The PSA Arch Tests need to be split into several binaries to load onto
  Musca-A board because of low memory available to the NS world to use.

- The Regression tests on MPS3 AN524 FPGA takes about 40 minutes to complete.
  This is because AN524 uses QSPI Flash for runtime memory as the RAM is small.
  The slow speed of QSPI device causes the tests to run slowly.

- Warm reset of eFlash is not permitted on Musca-B1 due to HW bug
  https://community.arm.com/developer/tools-software/oss-platforms/w/docs/426/musca-b1-warm-reset-of-eflash
  As TF-M is executed in place from eFlash on Musca-B1, there is good chance
  that a warm reset of the board will have unexpected (even non-deterministic)
  effects on code execution. Hence the PSA Arch FF tests, which rely of warm
  reset of Musca-B1 were executed on RAM FS using "-DSST_RAM_FS=ON" config.

Known issues
============
Some open issues exist and will not be fixed in this release.

.. list-table::

  *  - AN521 FVP soft reset via AIRCR does not reset MPC / PPC / MPU and will
       cause boot failure. This is known issue for AN521 FVP. This will cause
       the system to not boot after a warm reset during PSA Arch FF testing.
     - Issue : https://developer.trustedfirmware.org/T692

  *  - PSA Arch Crypto tests have several known failures.
     - See this link for detailed analysis of the failures : https://github.com/ARM-software/psa-arch-tests/blob/master/api-tests/docs/test_failure_analysis.md

  *  - There are 2 additional failures for PSA-Arch Crypto tests with CC-312
       other than the known failures. This is due to limitation of CC-312
       implementation as it does not support MD_NONE hashing mode causing the
       additional failures.
     - The issue details are captured here : https://developer.trustedfirmware.org/T691

  *  - PS test case 2002 and 1002 does not fail on Musca-B1 flash when
       run for second time without erasing flash. The WRITE_ONCE assets created
       by SST module should not be updatable but after reboot, the update seems
       to happen and is not expected. This issue will happen on any platform
       using persistent storage for SST.
     - Issue created : https://developer.trustedfirmware.org/T693

  *  - Boot up fails if there is unexpected data in flash on Musca-A. The boot
       is successful and the tests pass if all the associated (SST/ITS/NV
       Counter) flash areas are erased.
     - Issue created : https://developer.trustedfirmware.org/T694

  *  - If the flash is not erased, boot fails on Musca-B1 when SST
       is using flash for Minsizerel config.
     - Issue created : https://developer.trustedfirmware.org/T695

  *  - When SST/ITS are using Flash on Musca-B1, PSA Arch FF test fails due
       to known warm reset limitation in the platform. But after the failure,
       Musca-B1 boot fails to boot. This could be related to general issues of
       the SST module when Flash data is inconsistent.
     - Issue created : https://developer.trustedfirmware.org/T696

  *  - The eflash driver on Musca-B1 can return random failures hence
       triggering random failures during PSA Arch ITS and PSA Arch PS tests.
       This happens when ITS/SST is configured to use flash.
     - Issue created : https://developer.trustedfirmware.org/T697

  *  - Release build of PSA Arch Crypto tests have a different number of tests
       when built for AN521 FVP. This is an issue in the PSA Arch Crypto tests.
     - Issue created for PSA Arch Tests project : https://github.com/ARM-software/psa-arch-tests/issues/169

--------------

*Copyright (c) 2020, Arm Limited. All rights reserved.*
