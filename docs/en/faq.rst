A1 Appendix FAQs
================

A1.1 Compilation errors
-----------------------

I cannot build the application:

-  Make sure you are on the correct esp-idf branch/release. Run ``git submodule
   update —init —recursive`` to make sure the submodules are at the
   correct heads
-  Make sure you have the correct ESP_MATTER_PATH (and any other required paths).
-  Delete the build/ directory and also sdkconfig and sdkconfig.old and
   then build again.
-  If you are still facing issues, reproduce it on the default
   example and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Please make sure to
   share these:

   -  The complete device logs taken over UART.
   -  The esp-matter and esp-idf branch you are using.

A1.2 Device commissioning using chip-tool
-----------------------------------------

I cannot commission a new device through the chip-tool:

-  If the ``chip-tool pairing ble-wifi`` command is failing,
   make sure the arguments are correct.
-  Make sure Bluetooth is turned on, on your client (host).
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The complete device logs.
   -  The complete chip-tool logs.
   -  The esp-matter and esp-idf branch you are using.

Bluetooth/BLE does not work on by device:

-  There are some `known issues <https://github.com/project-chip/connectedhomeip/issues/13303>`__ where BLE does not work on MacOS.
-  In this case, the following can be done:

   -  Run the device console command:
      ``matter esp wifi connect <ssid> <password>``.
   -  Run the chip-tool command for commissioning over ip:
      ``chip-tool pairing onnetwork 0x7283 20202021``.

-  If you are still facing issues, reproduce it on the default
   example for the device and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The complete device logs taken over UART.
   -  The complete chip-tool logs.
   -  The esp-matter and esp-idf branch you are using.

A1.3 Device crashing
--------------------

My device is crashing:

-  Given the tight footprint requirements of the device, please make
   sure any issues in your code have been ruled out. If you believe the
   issue is with the Espressif SDK itself, please recreate the issue on
   the default example application (without any changes) and go through
   the following steps:
-  Make sure you are on the correct esp-idf branch. Run ``git submodule
   update —init —recursive`` to make sure the submodules are at the
   correct heads.
-  Make sure you have the correct ESP_MATTER_PATH (and any other paths)
   is (are) exported.
-  Delete the build/ directory and also sdkconfig and sdkconfig.old and
   then build and flash again.
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The steps you followed to reproduce the issue.

   -  The complete device logs taken over UART.

   -

      .. raw:: html

         <example>

      .elf file from the build/ directory.

   -  If you have gdb enabled, run the command ``backtrace`` and share the
      output of gdb too.

   -  The esp-matter and esp-idf branch you are using.

A1.4 Device not crashed but not responding
------------------------------------------

My device is not responding to commands:

-  Make sure your device is commissioned successfully and is connected
   to the Wi-Fi.
-  Make sure the node_id and the endpoint_id are correct in the command
   from chip-tool.
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The steps you followed to reproduce the issue.
   -  The complete device logs taken over UART.
   -  The esp-matter and esp-idf branch you are using.

A1.5 QR code not rendering
--------------------------

The QR code on my device console is not rendering properly:

-  Check the below lines on your terminal and copy paste the given URL
   in a browser:

   ::

      If QR code is not visible, copy paste the URL in a browser:
      https://project-chip.github.io/connectedhomeip/qrcode.html?data=....

A1.6 Onboard LED not working
----------------------------

The LED on my devkit is not working:

-  Make sure you have selected the proper ``device``. You can explicitly
   do that by exporting the ``ESP_MATTER_DEVICE_PATH`` to the correct
   path.
-  Check the version of your board, and if it has the LED connected to a
   different pin. If it is different, you can change the
   led_driver_config_t accordingly in the device.c file.
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The complete device logs taken over UART.
   -  The esp-matter and esp-idf branch you are using.
   -  The devkit and its version that you are using.

A1.7 Using Rotating Device Identifier
-------------------------------------

What is Rotating Device Identifier:

-  The Rotating Device Identifier provides a non-trackable identifier which is unique per-device and
   that can be used in one or more of the following ways:

   -  Provided to the vendor's customer support for help in pairing or establishing Node provenance;
   -  Used programmatically to obtain a Node's Passcode or other information in order to provide a
      simplified setup flow. Note that the mechanism by which the Passcode may be obtained is
      outside of this specification. If the Rotating Device Identifier is to be used for this purpose, the
      system implementing this feature SHALL require proof of possession by the user at least once
      before providing the Passcode. The mechanism for this proof of possession, and validation of it,
      is outside of this specification.

How to use Rotating Device Identifier

-  Enable the Rotating Device Identifier support in menuconfig.
-  Add the ``--enable-rotating-device-id`` and add the ``--rd-id-uid`` to specify the ``Rotating ID Unique ID``
   when use the mfg_tool.py to generate partition.bin file.

Difference between Rotating ID Unique ID and Unique ID

-  The ``Rotating ID Unique ID`` is a parameter used to generate ``Rotating Device Identifier``, it is
   a unique per-device identifier and shall consist of a randomly-generated 128-bit or longer octet string which
   shall be programmed during factory provisioning or delivered to the device by the vendor using secure means
   after a software update, it shall stay fixed during the lifetime of the device.

-  The ``Unique ID`` is an attribute in ``Basic Information Cluster``, it shall indicate a unique
   identifier for the device, which is constructed in a manufacturer specific manner. It may be
   constructed using a permanent device identifier (such as device MAC address) as basis.
   In order to prevent tracking:

   -  it SHOULD NOT be identical to (or easily derived from) such permanent device identifier
   -  it SHOULD be updated when the device is factory reset
   -  it SHALL not be identical to the SerialNumber attribute
   -  it SHALL not be printed on the product or delivered with the product

   The value does not need to be human readable.

**Also check the "Appendix FAQs" sections of the respective examples.**

-  :project_file:`Light <examples/light/README.md>`
-  :project_file:`Light Switch <examples/light_switch/README.md>`
-  :project_file:`Zap Light <examples/zap_light/README.md>`
-  :project_file:`Zigbee Bridge <examples/zigbee_bridge/README.md>`
-  :project_file:`BLE Mesh Bridge <examples/blemesh_bridge/README.md>`

A1.8 ModuleNotFoundError: No module named 'lark'
------------------------------------------------

Encountering the above error while building the esp-matter example could indicate that the steps outlined in the
`getting the repository <https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#getting-the-repositories>`__
section of the documentation were not properly followed.

The esp-matter example relies on several python dependencies that can be found in the
`requirements.txt <https://github.com/espressif/esp-matter/blob/main/requirements.txt>`__.
These dependencies must be installed into the python environment of the esp-idf to ensure that the example builds successfully.

One recommended approach to installing these requirements is by running the command
``source $IDF_PATH/export.sh`` before running ``esp-matter/install.sh``, as suggested in the programming guide.
However, if the error persists, you can try the following steps to resolve it:

    ::

        cd esp-idf
        source ./export.sh

        cd esp-matter
        python3 -m pip install -r requirements.txt

        # Now examples will build without any error
        cd examples/...
        idf.py build


A1.9 Why does free RAM increase after first commissioning
---------------------------------------------------------

After the first commissioning, you may notice that the free RAM increases. This is because, by default,
BLE is only used for the commissioning process. Once the commissioning is complete, BLE is deinitialized,
and all the memory allocated to it is recovered. Here's the link to the
`implementation <https://github.com/espressif/esp-matter/blob/c52fa686d1a3be275b0a5c872ee5f1a3c8f2420d/components/esp_matter/esp_matter_core.cpp#L859-L891>`__ which frees the BLE memory.

However, if you want to continue using the BLE even after the commissioning process, you can disable the
``CONFIG_USE_BLE_ONLY_FOR_COMMISSIONING``. This will ensure that the memory allocated to the BLE functionality
is not released after the commissioning process, and the free RAM won't go up.

A1.10 How to generate Matter Onboarding Codes (QR Code and Manual Pairing Code)
-------------------------------------------------------------------------------

When creating a factory partition using ``mfg_tool.py``, both the QR code and manual pairing codes are generated.

Along with that, there are two more methods for generating Matter onboarding codes:

-  Python script: `generate_setup_payload.py <https://github.com/project-chip/connectedhomeip/tree/master/src/setup_payload/python>`__.

    ::

        ./generate_setup_payload.py --discriminator 3131 --passcode 20201111 \
                                    --vendor-id 65521 --product-id 32768 \
                                    --commissioning-flow 0 --discovery-cap-bitmask 2

- chip-tool

    ::

        // Generate the QR Code
        chip-tool payload generate-qrcode --discriminator 3131 --setup-pin-code 20201111 \
                                          --vendor-id 0xFFF1 --product-id 0x8004 \
                                          --version 0 --commissioning-mode 0 --rendezvous 2

        // Generates the short manual pairing code (11-digit).
        chip-tool payload generate-manualcode --discriminator 3131 --setup-pin-code 20201111 \
                                              --version 0 --commissioning-mode 0

        // To generate a long manual pairing code (21-digit) that includes both the vendor ID and product ID,
        // --commissioning-mode parameter must be set to either 1 or 2, indicating a non-standard commissioning flow.
        chip-tool payload generate-manualcode --discriminator 3131 --setup-pin-code 20201111 \
                                              --vendor-id 0xFFF1 --product-id 0x8004 \
                                              --version 0 --commissioning-mode 1

To create a QR code image, copy the QR code text and paste it into
`CHIP: QR Code <https://project-chip.github.io/connectedhomeip/qrcode.html>`__.

A1.11 Chip stack locking error ... Code is unsafe/racy
------------------------------------------------------

    ::

        E (84728) chip[DL]: Chip stack locking error at 'src/system/SystemLayerImplFreeRTOS.cpp:55'. Code is unsafe/racy
        E (84728) chip[-]: chipDie chipDie chipDie
        abort() was called at PC 0x40139b7f on core 0
        0x40139b7f: chip::Platform::Internal::AssertChipStackLockedByCurrentThread(char const*, int) at /home/jonathan/Desktop/Workspace/firmware/build/esp-idf/chip/../../../../esp-matter/connectedhomeip/connectedhomeip/config/esp32/third_party/connectedhomeip/src/lib/support/CodeUtils.h:508
         (inlined by) chipDie at /home/jonathan/Desktop/Workspace/firmware/build/esp-idf/chip/../../../../esp-matter/connectedhomeip/connectedhomeip/config/esp32/third_party/connectedhomeip/src/lib/support/CodeUtils.h:518
         (inlined by) chip::Platform::Internal::AssertChipStackLockedByCurrentThread(char const*, int) at /home/jonathan/Desktop/Workspace/firmware/build/esp-idf/chip/../../../../esp-matter/connectedhomeip/connectedhomeip/config/esp32/third_party/connectedhomeip/src/platform/LockTracker.cpp:36


When interacting with Matter resources, it is necessary to perform the operations from within the Matter thread to avoid
assertion errors. This applies to tasks such as getting and setting attributes, invoking commands, and performing
operations using the server's object, such as opening or closing the commissioning window.

To address this, there are two possible approaches:

- Locking the Matter thread

    ::

        lock::chip_stack_lock(portMAX_DELAY);
        ... // eg: access Matter attribute, open/close commissioning window.
        lock::chip_stack_unlock();

- Scheduling the work on Matter thread

    ::

        static void WorkHandler(intptr_t context);
        {
            ... // Do the stuff
        }
        chip::DeviceLayer::PlatformMgr().ScheduleWork(WorkHandler, <intptr_t>(nullptr));


A1.12 Firmware Version Number
-----------------------------

Similar to the ESP-IDF's application versioning scheme, the ESP-Matter SDK provides two options for setting the firmware
version. It depends on `CONFIG_APP_PROJECT_VER_FROM_CONFIG <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-app-project-ver-from-config>`__
option and by default option is disabled.

If the ``CONFIG_APP_PROJECT_VER_FROM_CONFIG`` option is disabled, you need to set the version and version string by
defining the CMake variables in the project's ``CMakeLists.txt`` file. All the examples use this scheme and have these
variables set. Here's an example:

    ::

        set(PROJECT_VER "1.0")
        set(PROJECT_VER_NUMBER 1)

On the other hand, if the ``CONFIG_APP_PROJECT_VER_FROM_CONFIG`` option is enabled, you need to set the version using
the following configuration options:

- Software Version
    Set the ``CONFIG_DEVICE_SOFTWARE_VERSION_NUMBER`` option.
    (Component config -> CHIP Device Layer -> Device Identification Options -> Device Software Version Number)

- Software Version String
    Set the ``CONFIG_APP_PROJECT_VER`` option. (Application manager -> Get the project version from Kconfig)

**NOTES:**

- Ensure you use the correct versioning scheme when building the OTA image.
- Verify that the software version number in the firmware matches the one specified in the Matter OTA header.
- The software version number of the OTA image must be numerically higher.
- If you need to perform a functional rollback, the version number in the OTA image must be higher than the current
  version, even though the binary content may match the previous OTA image.


A1.13 Stuck at "Solving dependencies requirements ....."
--------------------------------------------------------

When building an example, if it is stuck at "Solving dependencies requirements..."
you can resolve this issue by clearing the component manager cache.

    ::

        # On Linux
        rm -rf ~/.cache/Espressif/ComponentManager

        # On macOS
        rm -rf ~/Library/Caches/Espressif/ComponentManager
