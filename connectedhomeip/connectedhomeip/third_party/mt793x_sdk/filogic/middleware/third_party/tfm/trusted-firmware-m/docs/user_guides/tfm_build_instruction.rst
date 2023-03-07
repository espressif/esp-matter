##################
Build instructions
##################
Please make sure you have all required software installed as explained in the
:doc:`software requirements <tfm_sw_requirement>`.

The external dependecies are briefly listed in the
:ref:`docs/user_guides/tfm_sw_requirement:External dependencies` section.

The configuration-table
:ref:`docs/user_guides/tfm_build_instruction:Configuring the build` section
explains all the supported build parameters:

****************
TF-M build steps
****************
TF-M uses `cmake <https://cmake.org/overview/>`__ to provide an out-of-source
build environment. The instructions are below.

.. Note::

    It is recommended to build each different build configurations in separate
    directories.

Getting the source-code
=======================
.. code-block:: bash

    cd <TF-M base folder>
    git clone https://git.trustedfirmware.org/trusted-firmware-m.git
    git clone https://github.com/ARMmbed/mbed-crypto.git -b mbedcrypto-3.0.1
    git clone https://github.com/ARM-software/CMSIS_5.git -b 5.5.0

    # If CMSIS was cloned before the git-lfs dependency was installed,
    # use the following command to download the binaries.
    cd CMSIS_5 && git lfs pull

.. Note::
   `CMSIS_5` now uses `git-lfs` for storing large size binaries so the cloning
   process has changed from previous releases. Please refer to the `CMSIS_5`
   documentation for more details.

.. Note::
   For building with Armclang compiler version 6.10.0, please follow the note
   in :ref:`docs/user_guides/tfm_sw_requirement:External dependencies` section.

.. Note::
   For building with the IAR toolchain, please see the notes in
   :doc:`software requirements <tfm_build_instruction_iar>`

Build steps for the AN521 target platform:
==========================================
.. code-block:: bash

    cd <TF-M base folder>
    cd trusted-firmware-m
    mkdir cmake_build
    cd cmake_build
    cmake ../ -G"Unix Makefiles" -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG
    cmake --build ./ -- install

Regression Tests for the AN521 target platform
==============================================
*TF-M build regression tests on Linux*

.. code-block:: bash

    cd <TF-M base folder>
    cd trusted-firmware-m
    mkdir cmake_test
    cd cmake_test
    cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink -f ../configs/ConfigRegression.cmake` -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG ../
    cmake --build ./ -- install

*TF-M build regression tests on Windows*

.. code-block:: bash

    cd <TF-M base folder>
    cd trusted-firmware-m
    mkdir cmake_test
    cd cmake_test
    cmake -G"Unix Makefiles" -DPROJ_CONFIG=`cygpath -am ../configs/ConfigRegression.cmake` -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG ../
    cmake --build ./ -- install

Build for PSA Developer API compliance tests
============================================
The build system provides the support for linking with prebuilt PSA Developer
API compliance NS test libraries when using the ``ConfigPsaApiTest.cmake``,
``ConfigPsaApiTestIPC.cmake`` or ``ConfigPsaApiTestIPCTfmLevel2.cmake`` config
file. The build system assumes that the PSA API compliance test suite is checked
out at the same level of the TF-M root folder and the default name for the build
folder has been used when compiling the PSA API compliance tests. Each set of
tests for the Internal Trusted Storage, Secure Storage, Crypto and Attestation
services needs to be enabled at the build configuration step by defining::

    -DPSA_API_TEST_INTERNAL_TRUSTED_STORAGE=ON
    -DPSA_API_TEST_PROTECTED_STORAGE=ON
    -DPSA_API_TEST_STORAGE=ON
    -DPSA_API_TEST_CRYPTO=ON
    -DPSA_API_TEST_INITIAL_ATTESTATION=ON

respectively for the corresponding service. For example, to enable the PSA API
tests for the Crypto service only:

.. code-block:: bash

    cd <TF-M base folder>
    cd trusted-firmware-m
    mkdir cmake_psa_test
    cd cmake_psa_test
    cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink -f ../configs/ConfigPsaApiTest.cmake` -DPSA_API_TEST_CRYPTO=ON -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG ../
    cmake --build ./ -- install

Build for PSA FF (IPC) compliance tests
=======================================

The build system assumes that the PSA FF compliance test suite is checked out
at the same level of the TF-M root folder and the default name for the build
folder has been used when compiling the PSA FF compliance tests.

Parse the PSA FF compliance tests partition manifests using a tool script named
as ``tfm_parse_manifest_list.py``. This tool updates the TFM partitions data
structure with PSA test suite partitions detail and creates the manifest output
files that are required for the PSA test suite build. Using these manifest
output files, build the PSA FF compliance tests as per the instructions
given in the PSA FF compliance tests README.

.. code-block:: bash

    cd <TF-M base folder>
    cd trusted-firmware-m
    python tools/tfm_parse_manifest_list.py -m tools/tfm_psa_ff_test_manifest_list.yaml append

The build system provides the support for linking with prebuilt PSA FF
compliance NS and S test libraries when using the ``ConfigPsaApiTestIPC.cmake``
or ``ConfigPsaApiTestIPCTfmLevel2.cmake`` config file.  The PSA FF compliance
tests need to be enabled at the build configuration step by defining::

    -DPSA_API_TEST_IPC=ON.

For example, to enable the PSA FF tests for ConfigPsaApiTestIPCTfmLevel2.cmake
config :

.. code-block:: bash

    cd <TF-M base folder>
    cd trusted-firmware-m
    mkdir cmake_psa_test
    cd cmake_psa_test
    cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink -f ../configs/ConfigPsaApiTestIPCTfmLevel2.cmake` -DPSA_API_TEST_IPC=ON -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG ../
    cmake --build ./ -- install

Location of build artifacts
===========================
The build system defines an API which allow easy usage of build
artifacts. The ``install`` build target copies all files which might be needed
as a dependency by external tools or build systems to the
``<build_dir>/install/outputs``
directory, with the following directory hierarchy:

::

    <build_dir>/install/outputs/fvp/
    <build_dir>/install/outputs/<target_platform>/

There is one folder for FVP testing, with more elaborate naming and
there is an other for testing on target hardware platform (AN521, etc.), where
naming convention is aligned with 8.3 format. The dependency tree of
``install`` build target ensures a proper update (i.e. build) of all output
files before the actual installation step takes place. As such it is suggested
to use this build target to build TF-M.

Export dependency files for NS applications
-------------------------------------------
An NS application requires a number of files to interface with TF-M.
The build system exports these files as part of the ``install`` target and
places them in to a single directory::

    <build_dir>/install/export/tfm

Further details on how to integrate a new NS app with TF-M are available in the
:doc:`integration guide <tfm_integration_guide>`.

Building the documentation
==========================
Please ensure the dependencies for building the firmware and the
documentation are installed as explained in the
:doc:`software requirements <tfm_sw_requirement>`.

Building PDF output is optional and can be disabled by removing LaTex from the
PATH.

.. Note::
   For building the documentation all tools needed to build the firmware must
   be available.

Building the Reference Manual
-----------------------------
.. code-block:: bash

    cd <TF-M base folder>
    mkdir cmake_doc
    cd cmake_doc
    cmake ../ -G"Unix Makefiles" -DTARGET_PLATFORM=AN521 -DCOMPILER=GNUARM
    cmake --build ./ -- install_doc

The documentation files will be available under the directory::

    cmake_doc/install/doc/reference_manual

Building the User Guide
-----------------------
.. code-block:: bash

    cd <TF-M base folder>
    mkdir cmake_doc
    cd cmake_doc
    cmake ../ -G"Unix Makefiles" -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG
    cmake --build ./ -- install_userguide

The documentation files will be available under the directory::

    cmake_doc/install/doc/user_guide

*********************
Configuring the build
*********************
The build configuration for TF-M is provided to the build system using command
line arguments:

.. list-table::
   :widths: 20 80

   * - -DPROJ_CONFIG=<file>
     - Specifies the way the application is built.

       | <file> is the absolute path to configurations file
         named as ``Config<APP_NAME>.cmake``.
       | i.e. On Linux:
         ``-DPROJ_CONFIG=`readlink -f ../configs/ConfigRegression.cmake```

   * - -DTARGET_PLATFORM=<target platform name>
     - Specifies the target platform.
       Supported platforms:

          - Cortex-M33 SSE-200 subsystem for MPS2+ (AN521)
            ``-DTARGET_PLATFORM=AN521``
          - Cortex-M23 IoT Kit subsystem for MPS2+ (AN519)
            ``-DTARGET_PLATFORM=AN519``
          - Arm SSE-123 Example Subsystem for MPS2+ (AN539)
            ``-DTARGET_PLATFORM=AN539``
          - Musca-A test chip board (Cortex-M33 SSE-200 subsystem)
            ``-DTARGET_PLATFORM=MUSCA_A``
          - Musca-B1 test chip board (Cortex-M33 SSE-200 subsystem)
            ``-DTARGET_PLATFORM=MUSCA_B1``
          - Musca-S1 test chip board (Cortex-M33 SSE-200 subsystem)
            ``-DTARGET_PLATFORM=MUSCA_S1``
          - Cortex-M33 SSE-200 subsystem for MPS3 (AN524)
            ``-DTARGET_PLATFORM=AN524``
          - Cypress CYS0644ABZI-S2D44 board (PSoC64 platform)
            ``-DTARGET_PLATFORM=psoc64``
            See :doc:`Cypress PSoC 64 platform specifics </platform/ext/target/cypress/psoc64/cypress_psoc64_spec>`
          - DesignStart FPGA on Cloud: Cortex-M33 based platform (SSE-200_AWS platform)
            ``-DTARGET_PLATFORM=SSE-200_AWS``

   * - -DCOMPILER=<compiler name>
     - Specifies the compiler toolchain
       The possible values are:

         - ``ARMCLANG``
         - ``GNUARM``
   * - -DCMAKE_BUILD_TYPE=<build type>
     - Configures debugging support.
       The possible values are:

         - ``Debug``
         - ``Release``
         - ``Relwithdebinfo``
         - ``Minsizerel``
   * - -DMBEDCRYPTO_DEBUG=<ON|OFF>
     - Enables debug symbols for Mbed Crypto library. If a cryptographic
       accelerator is enabled then this will also enable debug symbols and
       logging for any accelerator libraries.
   * - -DBUILD_DWARF_VERSION=<dwarf version>
     - Configures DWARF version.
       The possible values are:

         - 2
         - 3
         - 4

.. Note::
    Follow :doc:`secure boot <./tfm_secure_boot>` to build the binaries with or
    without BL2 bootloader.

**************
Configurations
**************
Configurations files under `configs` are TF-M provided configurations for building.
They are used by the `-DPROJ_CONFIG` argument for the build command line.
The following table describes the differences between the configurations:

+------------------------+------------+---------------+-----------------+----------------+---------------+-------------------+-------------------------+
|     Configuration      | Model [1]_ | TF-M LVL [2]_ | Regression [3]_ | Core Test [4]_ | IPC Test [5]_ | PSA API Test [6]_ |        Comment          |
+========================+============+===============+=================+================+===============+===================+=========================+
| Default                | Library    | 1             | No              | No             | No            | No                | TF-M, no tests          |
+------------------------+------------+---------------+-----------------+----------------+---------------+-------------------+-------------------------+
| Regression             | Library    | 1             | Yes             | Yes            | No            | No                | TF-M & Regression tests |
+------------------------+------------+---------------+-----------------+----------------+---------------+-------------------+-------------------------+
| CoreIPC                | IPC        | 1             | No              | No             | No            | No                | TF-M, no tests          |
+------------------------+------------+---------------+-----------------+----------------+---------------+-------------------+-------------------------+
| CoreIPCTfmLevel2       | IPC        | 2             | No              | No             | No            | No                | TF-M, no tests          |
+------------------------+------------+---------------+-----------------+----------------+---------------+-------------------+-------------------------+
| RegressionIPC          | IPC        | 1             | Yes             | Yes            | Yes           | No                | TF-M & Regression tests |
+------------------------+------------+---------------+-----------------+----------------+---------------+-------------------+-------------------------+
| RegressionIPCTfmLevel2 | IPC        | 2             | Yes             | Yes            | Yes           | No                | TF-M & Regression tests |
+------------------------+------------+---------------+-----------------+----------------+---------------+-------------------+-------------------------+
| PsaApiTest             | Library    | 1             | No              | No             | No            | Yes               | TF-M & PSA API tests    |
+------------------------+------------+---------------+-----------------+----------------+---------------+-------------------+-------------------------+
| PsaApiTestIPC          | IPC        | 1             | No              | No             | No            | Yes               | TF-M & PSA API tests    |
+------------------------+------------+---------------+-----------------+----------------+---------------+-------------------+-------------------------+
| PsaApiTestIPCTfmLevel2 | IPC        | 2             | No              | No             | No            | Yes               | TF-M & PSA API tests    |
+------------------------+------------+---------------+-----------------+----------------+---------------+-------------------+-------------------------+

.. [1] Which TF-M Model is set: `CORE_IPC`. If `CORE_IPC` is set to true then the model is IPC model, otherwise it's library model.

.. [2] The TF-M isolation level `TFM_LVL`. Currently Library model supports level 1. IPC model supports level 1 and 2.

.. [3] Build regression tests or not: `REGRESSION`.

.. [4] Build core tests or not: `CORE_TEST`.

.. [5] Build IPC tests or not: `IPC_TEST`. It can be only enabled in IPC model

.. [6] Build for PSA API compliance tests or not: `PSA_API_TEST`.

--------------

*Copyright (c) 2017-2020, Arm Limited. All rights reserved.*
