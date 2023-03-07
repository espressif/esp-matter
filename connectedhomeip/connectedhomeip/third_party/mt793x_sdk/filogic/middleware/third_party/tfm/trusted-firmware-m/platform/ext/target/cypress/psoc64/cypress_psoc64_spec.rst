########################
Cypress PSoC64 Specifics
########################

*************
Prerequisites
*************

PSoC64 must first be provisioned with SecureBoot firmware and a provisioning packet
containing policy and secure keys. Please refer to the guide at
https://www.cypress.com/documentation/software-and-drivers/psoc-64-secure-mcu-secure-boot-sdk-user-guide
Use the following policy file for provisioning and signing:
policy_multi_img_CM0p_CM4_debug_2M.json

Please make sure you have all required software installed as explained in the
:doc:`software requirements </docs/user_guides/tfm_sw_requirement>`.

Please also make sure that all the source code are fetched by following
:doc:`general building instruction </docs/user_guides/tfm_build_instruction>`.

Install CySecureTools.

.. code-block:: bash

    pip3 install cysecuretools==1.4.0


Note: the upcoming CY SecureBoot release requires CySecuretools v2.0.0 or newer.

.. code-block:: bash

    pip3 install cysecuretools==2.0.0

For more details please refer to
`CySecureTools <https://pypi.org/project/cysecuretools>`_ page.

Install OpenOCD with PSoC6 support. Download the latest revision from:
https://github.com/cypresssemiconductorco/openocd/releases

Note: the latest CY SecureBoot release requires OpenOCD 3.1.0 or newer.

******************************************
Building Multi-Core TF-M on Cypress PSoC64
******************************************

Configuring the build
=====================

The build configuration for TF-M is provided to the build system using command
line arguments:

.. list-table::
   :widths: 20 80

   * - -DPROJ_CONFIG=<file>
     - Specifies the way the application is built.

       | <file> is the absolute path to configurations file
         named as ``Config<APP_NAME>.cmake``.
       | e.g. On Linux:
         ``-DPROJ_CONFIG=`readlink -f ../configs/ConfigRegressionIPC.cmake```
       | Supported configurations files

           - IPC model without regression test suites in Isolation Level 1
             ``ConfigCoreIPC.cmake``
           - IPC model with regression test suites in Isolation Level 1
             ``ConfigRegressionIPC.cmake``
           - IPC model with PSA API test suite in Isolation Level 1
             ``ConfigPsaApiTestIPC.cmake``
           - IPC model without regression test suites in Isolation Level 2
             ``ConfigCoreIPCTfmLevel2.cmake``
           - IPC model with regression test suites in Isolation Level 2
             ``ConfigRegressionIPCTfmLevel2.cmake``
           - IPC model with PSA API test suite in Isolation Level 2
             ``ConfigPsaApiTestIPCTfmLevel2.cmake``

   * - -DTARGET_PLATFORM=psoc64
     - Specifies target platform name ``psoc64``

   * - -DCOMPILER=<compiler name>
     - Specifies the compiler toolchain
       The possible values are:

         - ``ARMCLANG``
         - ``GNUARM``
         - ``IARARM``

   * - -DCMAKE_BUILD_TYPE=<build type>
     - Configures debugging support.
       The possible values are:

         - ``Debug``
         - ``Release``


Build Instructions
==================

The following instructions build multi-core TF-M without regression test suites
in Isolation Level 1 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink \
          -f ../configs/ConfigCoreIPC.cmake` \
          -DTARGET_PLATFORM=psoc64 \
          -DCOMPILER=ARMCLANG \
          -DCMAKE_BUILD_TYPE=Release \
          ../
    popd
    cmake --build <build folder> -- -j VERBOSE=1

The following instructions build multi-core TF-M with regression test suites
in Isolation Level 1 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink \
          -f ../configs/ConfigRegressionIPC.cmake` \
          -DTARGET_PLATFORM=psoc64 \
          -DCOMPILER=ARMCLANG \
          -DCMAKE_BUILD_TYPE=Release ../
    popd
    cmake --build <build folder> -- -j VERBOSE=1

The following instructions build multi-core TF-M with PSA API test suite for
the attestation service in Isolation Level 1 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. list-table::
   :widths: 20 80

   * - -DPSA_API_TEST_BUILD_PATH=<path> (optional)
     - Specifies the path to the PSA API build directory

         - ``${TFM_ROOT_DIR}/../psa-arch-tests/api-tests/BUILD`` (default)

   * - -D<PSA_API_TEST_xxx>=1 (choose exactly one)
     - Specifies the service to support
       The possible values are:

         - ``PSA_API_TEST_INITIAL_ATTESTATION``
         - ``PSA_API_TEST_CRYPTO``
         - ``PSA_API_TEST_PROTECTED_STORAGE``
         - ``PSA_API_TEST_INTERNAL_TRUSTED_STORAGE``
         - ``PSA_API_TEST_STORAGE``

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake ../ \
        -G"Unix Makefiles" \
        -DPROJ_CONFIG=`readlink -f ../configs/ConfigPsaApiTestIPC.cmake` \
        -DPSA_API_TEST_BUILD_PATH=../psa-arch-tests/api-tests/BUILD_ATT.GNUARM
        -DPSA_API_TEST_INITIAL_ATTESTATION=1 \
        -DTARGET_PLATFORM=psoc64 \
        -DCOMPILER=ARMCLANG \
        -DCMAKE_BUILD_TYPE=Release
    popd
    cmake --build <build folder> -- -j VERBOSE=1

The following instructions build multi-core TF-M without regression test suites
in Isolation Level 2 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake -G"Unix Makefiles" \
          -DPROJ_CONFIG=`readlink -f ../configs/ConfigCoreIPCTfmLevel2.cmake` \
          -DTARGET_PLATFORM=psoc64 \
          -DCOMPILER=ARMCLANG \
          -DCMAKE_BUILD_TYPE=Release \
          ../
    popd
    cmake --build <build folder> -- -j VERBOSE=1

The following instructions build multi-core TF-M with regression test suites
in Isolation Level 2 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake -G"Unix Makefiles" \
          -DPROJ_CONFIG=`readlink \
          -f ../configs/ConfigRegressionIPCTfmLevel2.cmake` \
          -DTARGET_PLATFORM=psoc64 \
          -DCOMPILER=ARMCLANG \
          -DCMAKE_BUILD_TYPE=Release \
          ../
    popd
    cmake --build <build folder> -- -j VERBOSE=1

The following instructions build multi-core TF-M with PSA API test suite for
the protected storage service in Isolation Level 2 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. list-table::
   :widths: 20 80

   * - -DPSA_API_TEST_BUILD_PATH=<path> (optional)
     - Specifies the path to the PSA API build directory

         - ``${TFM_ROOT_DIR}/../psa-arch-tests/api-tests/BUILD`` (default)

   * - -D<PSA_API_TEST_xxx>=1 (choose exactly one)
     - Specifies the service to support
       The possible values are:

         - ``PSA_API_TEST_INITIAL_ATTESTATION``
         - ``PSA_API_TEST_CRYPTO``
         - ``PSA_API_TEST_PROTECTED_STORAGE``
         - ``PSA_API_TEST_INTERNAL_TRUSTED_STORAGE``
         - ``PSA_API_TEST_STORAGE``

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake ../ \
        -G"Unix Makefiles" \
        -DPROJ_CONFIG=`readlink -f ../configs/ConfigPsaApiTestIPCTfmLevel2.cmake` \
        -DPSA_API_TEST_BUILD_PATH=../psa-arch-tests/api-tests/BUILD_PS.GNUARM
        -DPSA_API_TEST_PROTECTED_STORAGE=1 \
        -DTARGET_PLATFORM=psoc64 \
        -DCOMPILER=ARMCLANG \
        -DCMAKE_BUILD_TYPE=Release
    popd
    cmake --build <build folder> -- -j VERBOSE=1

**********************
Signing the images
**********************

First, convert tfm_s.axf and tfm_ns.axf images to hex format. This also places
resulting files one folder level up.

GNUARM build:

.. code-block:: bash

    arm-none-eabi-objcopy -O ihex <build folder>/secure_fw/tfm_s.axf <build folder>/tfm_s.hex
    arm-none-eabi-objcopy -O ihex <build folder>/app/tfm_ns.axf <build folder>/tfm_ns.hex

ARMCLANG build:

.. code-block:: bash

    fromelf --i32 --output=<build folder>/tfm_s.hex <build folder>/secure_fw/tfm_s.axf
    fromelf --i32 --output=<build folder>/tfm_ns.hex <build folder>/app/tfm_ns.axf

IARARM build:

.. code-block:: bash

    ielftool --silent --ihex <build folder>/secure_fw/tfm_s.axf <build folder>/tfm_s.hex
    ielftool --silent --ihex <build folder>/app/tfm_ns.axf <build folder>/tfm_ns.hex

Copy secure keys used in the board provisioning process to
platform/ext/target/cypress/psoc64/security/keys:

-MCUBOOT_CM0P_KEY_PRIV.pem - private OEM key for signing CM0P image
-USERAPP_CM4_KEY_PRIV.pem  - private OEM key for signing CM4 image

Note: provisioned board in SECURE claimed state is required, otherwise refer to
Cypress documentation for details on the provisioning process.


Depending on the used CySecureTools, signing process is different.

CySecureTools 1.x.x
===================

Sign the images with a helper script (sign.py overwrites unsigned files with
signed ones). For CySecureTools 1.x.x use policy
policy_multi_img_CM0p_CM4_debug_2M_legacy.json:

.. code-block:: bash

    ./platform/ext/target/cypress/psoc64/security/sign.py \
      -p platform/ext/target/cypress/psoc64/security/policy_multi_img_CM0p_CM4_debug_2M_legacy.json \
      -d cy8ckit-064b0s2-4343w \
      -s <build folder>/tfm_s.hex \
      -n <build folder>/tfm_ns.hex

Note: each image can be signed individually, for example:

.. code-block:: bash

    ./platform/ext/target/cypress/psoc64/security/sign.py \
      -p platform/ext/target/cypress/psoc64/security/policy_multi_img_CM0p_CM4_debug_2M_legacy.json \
      -d cy8ckit-064b0s2-4343w \
      -n <build folder>/tfm_ns.hex

.. code-block:: bash

    ./platform/ext/target/cypress/psoc64/security/sign.py \
      -p platform/ext/target/cypress/psoc64/security/policy_multi_img_CM0p_CM4_debug_2M_legacy.json \
      -d cy8ckit-064b0s2-4343w \
      -s <build folder>/tfm_s.hex

Running the sign.py script will result in creation of the following files:

* tfm_<s/ns>_signed.hex    - signed image for programming
* tfm_<s/ns>_unsigned.hex  - a copy of original unsigned hex file for reference
* tfm_<s/ns>_upgrade.hex   - signed image for upgrade (if device policy
  specifies upgrade slot). Flashing this image into device will
  trigger the image update. Upgrade image from the
  secondary slot will be moved to the primary slot.

CySecureTools 2.x.x
===================

Sign the images using CySecureTools CLI tool.

SPE image:

.. code-block:: bash

    cysecuretools \
    --policy platform/ext/target/cypress/psoc64/security/policy_multi_img_CM0p_CM4_debug_2M.json \
    --target cy8ckit-064b0s2-4343w \
    sign-image \
    --hex <build folder>/tfm_s.hex \
    --image-type BOOT \
    --image-id 1

NSPE image:

.. code-block:: bash

    cysecuretools \
    --policy platform/ext/target/cypress/psoc64/security/policy_multi_img_CM0p_CM4_debug_2M.json \
    --target cy8ckit-064b0s2-4343w \
    sign-image \
    --hex <build folder>/tfm_ns.hex \
    --image-type BOOT \
    --image-id 16

* CySecureTools sign-image overwrites unsigned file with a signed one,
  also it creates an unsigned copy _unsigned.hex.

* image-type option: "--image-type BOOT" creates a signed hex file with offsets
  for the primary image slot. Use "--image-type UPGRADE" if you want to create
  an image for the secondary "upgrade" slot.
  When booting, CyBootloader will validate image in the secondary slot and copy
  it to the primary boot slot.

* image-id option: Each image has its own ID. By default, SPE image running on
  CM0P core has ID=1, NSPE image running on CM4 core has ID=16. Refer to the
  policy file for the actual ID's.


**********************
Programming the Device
**********************

After building and signing, the TFM images must be programmed into flash
memory on the PSoC64 device. There are three methods to program it.

DAPLink mode
============

Using KitProg3 mode button, switch it to DAPLink mode.
Mode LED should start blinking rapidly and depending on the host computer
settings DAPLINK will be mounted as a media storage device.
Otherwise, mount it manually.

Copy tfm hex files one by one to the DAPLINK device:

.. code-block:: bash

    cp <build folder>/tfm_ns.hex <mount point>/DAPLINK/; sync
    cp <build folder>/tfm_s.hex <mount point>/DAPLINK/; sync

OpenOCD v.2.x
=============

Using KitProg3 mode button, switch to KitProg3 CMSIS-DAP BULK or
CMSIS-DAP HID mode. Status LED should be ON and not blinking for
CMSIS-DAP BULK or slowly blinking for CMSIS-DAP HID mode. Device programming
in CMSIS-DAP BULK mode is faster and thus is recommended.

To program the signed tfm_s and tfm_ns images to the device with openocd
(assuming OPENOCD_PATH is pointing at the openocd installation directory)
run the following commands:

.. code-block:: bash

    OPENOCD_PATH=<cyprogrammer dir>/openocd
    BUILD_DIR=<build folder>

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -c "set ENABLE_ACQUIRE 0" \
            -f target/psoc6_2m_secure.cfg \
            -c "init; reset init; flash write_image erase ${BUILD_DIR}/tfm_s_signed.hex" \
            -c "resume; reset; exit"

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -c "set ENABLE_ACQUIRE 0" \
            -f target/psoc6_2m_secure.cfg \
            -c "init; reset init; flash write_image erase ${BUILD_DIR}/tfm_ns_signed.hex" \
            -c "resume; reset; exit"

Optionally, erase SST partition:

.. code-block:: bash

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -f target/psoc6_2m_secure.cfg \
            -c "init; reset init" \
            -c "flash erase_address 0x101c0000 0x10000" \
            -c "shutdown"

Note that the ``0x101C0000`` in the command above must match the SST start
address of the secure primary image specified in the file:

    platform/ext/target/cypress/psoc64/partition/flash_layout.h

so be sure to change it if you change that file.

OpenOCD v.3.x
=============

Using KitProg3 mode button, switch to KitProg3 CMSIS-DAP BULK or
CMSIS-DAP HID mode. Status LED should be ON and not blinking for
CMSIS-DAP BULK or slowly blinking for CMSIS-DAP HID mode. Device programming
in CMSIS-DAP BULK mode is faster and thus is recommended.

To program the signed tfm_s and tfm_ns images to the device with openocd
(assuming OPENOCD_PATH is pointing at the openocd installation directory)
run the following commands:

.. code-block:: bash

    OPENOCD_PATH=<cyprogrammer dir>/openocd
    BUILD_DIR=<build folder>

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -f target/psoc6_2m_secure.cfg \
            -c "init; reset" \
            -c "flash write_image erase ${BUILD_DIR}/tfm_s.hex" \
            -c "shutdown"

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -f target/psoc6_2m_secure.cfg \
            -c "init; reset" \
            -c "flash write_image erase ${BUILD_DIR}/tfm_ns.hex" \
            -c "reset run"

Optionally, erase SST partition:

.. code-block:: bash

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -f target/psoc6_2m_secure.cfg \
            -c "init; reset" \
            -c "flash erase_address 0x101c0000 0x10000" \
            -c "shutdown"

Note that the ``0x101C0000`` in the command above must match the SST start
address of the secure primary image specified in the file:

    platform/ext/target/cypress/psoc64/partition/flash_layout.h

so be sure to change it if you change that file.


PyOCD
=====

PyOCD is installed by CySecureTools automatically. It can be used
to program TFM images into the board.

Using KitProg3 mode button, switch to KitProg3 CMSIS-DAP BULK or
CMSIS-DAP HID mode. Status LED should be ON and not blinking for
CMSIS-DAP BULK or slowly blinking for CMSIS-DAP HID mode. Device programming
in CMSIS-DAP BULK mode is faster and thus is recommended.

To program the signed tfm_s and tfm_ns images to the device with pyocd
run the following commands:

.. code-block:: bash

    pyocd flash  -t cy8c64xa_cm4_full_flash ${BUILD_DIR}/tfm_s.hex

    pyocd flash  -t cy8c64xa_cm4_full_flash ${BUILD_DIR}/tfm_ns.hex

Optionally, erase SST partition:

.. code-block:: bash

    pyocd erase -t cy8c64xa_cm4_full_flash 0x101c0000+0x10000

*Copyright (c) 2017-2020, Arm Limited. All rights reserved.*

*Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.*
