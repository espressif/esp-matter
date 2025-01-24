A1 Appendix FAQs
================

A1.1 Compilation errors
-----------------------

I cannot build the application:

-  Make sure you are on the correct esp-idf branch/release.
-  Run ``git submodule update —init —recursive`` to make sure the submodules are at the
   correct heads.
-  Make sure you have the correct ``ESP_MATTER_PATH`` (and any other required paths).
-  Delete the ``build/`` directory and also ``sdkconfig`` and ``sdkconfig.old`` and
   then build again.
-  If you are still facing issues, reproduce it on the default
   example and then raise a `Github issue`_.

A1.2 Device commissioning using chip-tool
-----------------------------------------

I cannot commission a new device through the chip-tool:

-  If the ``chip-tool pairing ble-wifi`` command is failing,
   make sure the arguments are correct.
- Please check ``chip-tool pairing ble-wifi --help`` for argument help.
-  Make sure Bluetooth is turned on, on your client (host).

Bluetooth/BLE does not work on by device:

-  There is a known issues `#13303`_ where BLE does not work on MacOS.
-  In this case, the following can be done:

   -  Run the device console command:
      ``matter esp wifi connect <ssid> <password>``.
   -  Run the chip-tool command for commissioning over ip:
      ``chip-tool pairing onnetwork 0x7283 20202021``.

-  If you are still facing issues, reproduce it on the default
   example for the device and then raise a `Github issue`_.

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
-  Make sure you have the correct ``ESP_MATTER_PATH`` (and any other paths)
   is (are) exported.
-  Delete the ``build/`` directory and also ``sdkconfig`` and ``sdkconfig.old`` and
   then build and flash again.
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise a `Github issue`_. Along with the
   details mentioned in the issue template, please share the following details:

   -  The steps you followed to reproduce the issue.
   -  The complete device logs taken over UART.
   -  The .elf file from the build/ directory.
   -  If you have gdb enabled, run the command ``backtrace`` and share the
      output of gdb too.

A1.4 Device not crashed but not responding
------------------------------------------

My device is not responding to commands:

-  Make sure your device is commissioned successfully and is connected
   to the Wi-Fi.
-  Make sure the node_id and the endpoint_id are correct in the command
   from chip-tool.
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise a `Github issue`_. Along with the
   details mentioned in the issue template, please share the following details:

   -  The steps you followed to reproduce the issue.
   -  The complete device logs taken over UART.


A1.5 Onboard LED not working
----------------------------

The LED on my devkit is not working:

-  Make sure you have selected the proper ``device``. You can explicitly
   do that by exporting the ``ESP_MATTER_DEVICE_PATH`` to the correct
   path.
-  Check the version of your board, and if it has the LED connected to a
   different pin. If it is different, you can change the
   ``led_driver_config_t`` accordingly in the ``device.c`` file.
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise a `Github issue`_.


A1.6 Using Rotating Device Identifier
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
   when use the ``esp-matter-mfg-tool`` to generate partition.bin file.

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


A1.7 ModuleNotFoundError: No module named 'lark'
------------------------------------------------

Encountering the above error while building the esp-matter example could indicate that the steps outlined in the
`getting the repository`_ section of the documentation were not properly followed.

The esp-matter example relies on several python dependencies that can be found in the
:project_file:`requirements.txt <requirements.txt>`.
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


A1.8 Why does free RAM increase after first commissioning
---------------------------------------------------------

After the first commissioning, you may notice that the free RAM increases. This is because, by default,
BLE is only used for the commissioning process. Once the commissioning is complete, BLE is deinitialized,
and all the memory allocated to it is recovered. Here's the link to the
:project_file:`implementation which frees the BLE memory <components/esp_matter/esp_matter_core.cpp#L859-L891>`.

However, if you want to continue using the BLE even after the commissioning process, you can disable the
``CONFIG_USE_BLE_ONLY_FOR_COMMISSIONING``. This will ensure that the memory allocated to the BLE functionality
is not released after the commissioning process, and the free RAM won't go up.

A1.9 How to generate Matter Onboarding Codes (QR Code and Manual Pairing Code)
------------------------------------------------------------------------------

When creating a factory partition using ``esp-matter-mfg-tool``, both the QR code and manual pairing codes are generated.

Along with that, there are two more methods for generating Matter onboarding codes:

-  Python script: `generate_setup_payload.py`_

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

To create a QR code image, copy the QR code text and paste it into `CHIP QR Code`_.


A1.10 Chip stack locking error ... Code is unsafe/racy
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


A1.11 Firmware Version Number
-----------------------------

Similar to the ESP-IDF's application versioning scheme, the ESP-Matter SDK provides two options for setting the firmware
version. It depends on `CONFIG_APP_PROJECT_VER_FROM_CONFIG`_ option and by default option is disabled.

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

.. note::

    - Ensure you use the correct versioning scheme when building the OTA image.
    - Verify that the software version number in the firmware matches the one specified in the Matter OTA header.
    - The software version number of the OTA image must be numerically higher.
    - If you need to perform a functional rollback, the version number in the OTA image must be higher than the current
      version, even though the binary content may match the previous OTA image.


A1.12 Stuck at "Solving dependencies requirements ....."
--------------------------------------------------------

When building an example, if it is stuck at "Solving dependencies requirements..."
you can resolve this issue by clearing the component manager cache.

    ::

        # On Linux
        rm -rf ~/.cache/Espressif/ComponentManager

        # On macOS
        rm -rf ~/Library/Caches/Espressif/ComponentManager


A1.13 ESP32-C2 log garbled, unable to perform Matter commissioning and other abnormal issues
--------------------------------------------------------------------------------------------

When encountering the above issues, the following possible causes may exist:
1. Incorrect baud rate settings. See `UART console baud rate`_
2. Incorrect XTAL crystal frequency settings. The default XTAL crystal frequency in the SDK examples is 26 Mhz, if the ESP32-C2 board used for testing is 40 MHz, please change the configuration as `CONFIG_XTAL_FREQ_40=y`. See `Main XTAL frequency`_ You can check the XTAL frequency with this command. 

   ::

      $ esptool.py flash_id
      esptool.py v4.7.0
      Serial port /dev/ttyUSB0
      Connecting....
      Detecting chip type... ESP32-C2
      Chip is ESP32-C2 (revision v1.0)
      Features: WiFi, BLE
      Crystal is 26MHz
      MAC: 08:3a:8d:49:b3:90

A1.14 Generating Matter Onboarding Codes on the device itself
-------------------------------------------------------------

The Passcode serves as both proof of possession for the device and the shared secret needed to establish
the initial secure channel for onboarding.

For best practices in Passcode generation and storage on the device, refer to
**Section 5.1.7: Generation of the Passcode** in the Core Matter Specification.

Ideally, devices should only store the Spake2p verifier, not the Passcode itself.
If the Passcode is stored on the device, it must be physically separated from the Spake2p verifier's location
and must be accessible only through local interface and must not be accessible to the unit handling the
Spake2p verifier.

For devices capable of displaying the onboarding payload, the use of a dynamic Passcode is recommended.

The :project_file:`Light Switch <examples/light_switch/README.md>`
example in the SDK demonstrates the use of a dynamic Passcode.
It implements a custom Commissionable Data Provider that generates the dynamic Passcode,
along with the corresponding Spake2p verifier and onboarding payload, directly on the device.

Please check `#1128`_ and `#1126`_ for relevant discussion on Github issue

A1.15 Using BLE after Matter commissioning
------------------------------------------

Most Matter applications do not require BLE after commissioning. By default, BLE is deinitialized after commissioning
to reclaim RAM and increase the available free heap. Refer to `A1.8 Why does free RAM increase after first commissioning`_
for more details.

However, if BLE functionality is needed even after commissioning, you can disable the ``CONFIG_USE_BLE_ONLY_FOR_COMMISSIONING``
option. This ensures that the memory allocated to BLE functionality is retained, allowing BLE to be used for other
purposes post-commissioning.

After commissioning is complete, Matter will stop advertising, but the application can utilize BLE for other roles or operations.
e.g. BLE Peripheral, BLE Central, etc.

To learn more, refer to the `bleprph`_ and `blecent`_ examples in ``esp-idf/examples/bluetooth/nimble``. These examples
demonstrate BLE Peripheral and BLE Central roles. It also provides the step-by-step tutorial for building such devices.

For implementation details on Peripheral and Central roles, refer to the `bleprph_advertise()`_ and `blecent_scan()`_ functions in
the respective examples.

BLE Central role is disabled by default in the esp-matter SDK's default example configurations.
Please enable ``CONFIG_BT_NIMBLE_ROLE_CENTRAL`` option if you plan to use BLE Central role.

.. note::

   Above mentioned details apply specifically to the NimBLE host.


For more advanced BLE usage, you can use the external platform feature.
It also serves as a way to integrate custom BLE usage with Matter.

Please refer to the `advance setup`_ section in the programming guide.
This has been demonstrated in the `blemesh_bridge`_ and `light_wifi_prov`_ examples.


A1.16 Moving BSS Segments to PSRAM to Reduce Memory Usage
---------------------------------------------------------

The BSS section of libesp_matter.a and libCHIP.a can consume significant internal memory.
For devices with PSRAM, you can move the BSS segments to external memory to significantly
reduce the internal memory footprint.

To move the BSS segments of libCHIP.a and libesp_matter.a into external RAM:

1. Enable the ``CONFIG_ESP_ALLOW_BSS_SEG_EXTERNAL_MEMORY`` option in menuconfig.

2. Create a ``linker.lf`` file in your project's main component, you can check the the example 
   :project_file:`linker.lf <examples/all_device_types_app/main/linker.lf>` file.

3. Modify your main component's ``CMakeLists.txt`` to include:

   ::

       set(ldfragments linker.lf)
       idf_component_register(
           ...
           LDFRAGMENTS "${ldfragments}")

This configuration will move the BSS segments to PSRAM when ``CONFIG_ESP_ALLOW_BSS_SEG_EXTERNAL_MEMORY`` is enabled,
significantly reducing the internal memory usage of your application.

Please check `#1123`_ for relevant discussion on Github issue.

.. _bleprph: https://github.com/espressif/esp-idf/tree/b5ac4fbdf9e9fb320bb0a98ee4fbaa18f8566f37/examples/bluetooth/nimble/bleprph
.. _blecent: https://github.com/espressif/esp-idf/tree/b5ac4fbdf9e9fb320bb0a98ee4fbaa18f8566f37/examples/bluetooth/nimble/blecent
.. _bleprph_advertise(): https://github.com/espressif/esp-idf/blob/b5ac4fbdf9e9fb320bb0a98ee4fbaa18f8566f37/examples/bluetooth/nimble/bleprph/main/main.c#L146
.. _blecent_scan(): https://github.com/espressif/esp-idf/blob/b5ac4fbdf9e9fb320bb0a98ee4fbaa18f8566f37/examples/bluetooth/nimble/blecent/main/main.c#L435
.. _advance setup: https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#advanced-setup
.. _blemesh_bridge: https://github.com/espressif/esp-matter/tree/main/examples/bridge_apps/blemesh_bridge
.. _light_wifi_prov: https://github.com/espressif/esp-matter/tree/main/examples/light_wifi_prov#4-external-platform
.. _getting the repository: https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#getting-the-repository
.. _requirements.txt: https://github.com/espressif/esp-matter/blob/main/requirements.txt
.. _implementation which frees the BLE memory: https://github.com/espressif/esp-matter/blob/c52fa686d1a3be275b0a5c872ee5f1a3c8f2420d/components/esp_matter/esp_matter_core.cpp#L859-L891
.. _generate_setup_payload.py: https://github.com/project-chip/connectedhomeip/tree/master/src/setup_payload/python
.. _CHIP QR Code: https://project-chip.github.io/connectedhomeip/qrcode.html
.. _CONFIG_APP_PROJECT_VER_FROM_CONFIG: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-app-project-ver-from-config
.. _UART console baud rate: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c2/api-reference/kconfig.html#config-esp-console-uart-baudrate
.. _Main XTAL frequency: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c2/api-reference/kconfig.html#main-xtal-config

.. _Github issue: https://github.com/espressif/esp-matter/issues/new?template=issue-template.md
.. _`#1123`: https://github.com/espressif/esp-matter/issues/1123
.. _`#1126`: https://github.com/espressif/esp-matter/issues/1126
.. _`#1128`: https://github.com/espressif/esp-matter/issues/1128
.. _`#13303`: https://github.com/project-chip/connectedhomeip/issues/13303