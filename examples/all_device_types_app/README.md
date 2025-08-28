# All Device Types App

This is a All Device Types App using the ESP Matter data model. It covers all the device types that ESP
MATTER data model implements. It has been tested with multiple ecosystem Apple, Google, Alexa.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

**This example only implements mandatory clusters, attributes, commands, events, etc from the spec.**
If you want to add any optional data model bits please add them in [create api](https://github.com/espressif/esp-matter/blob/main/examples/all_device_types_app/main/esp_matter_console_helpers.cpp#L146) under your desired device type case using [data model creation development guide](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#developing-your-product).

## 1. Additional Environment Setup

No additional setup is required.

## 2. Usage

#### To use diagnostic tracing

Enable Diagnostics trace and BDX protocol for diagnostic logs transfer from menuconfig

```
idf.py menuconfig
CONFIG_ENABLE_ESP_DIAGNOSTICS_TRACE
CONFIG_CHIP_ENABLE_BDX_LOG_TRANSFER
```

Set diagnostic storage buffer size from `Platform Diagnostics` menu

-   End user buffer default size 4096
-   Retrieval buffer default size 4096

#### To retrieve the diagnostic via diagnostic logs cluster

```
# Commission the app
chip-tool pairing ble-wifi 1 SSID PASSPHRASE 20202021 3840

# Read end user support logs using response payload protocol
chip-tool diagnosticlogs retrieve-logs-request 0 0 1 0

# Read end user diagnostic using BDX protocol
chip-tool interactive start
> diagnosticlogs retrieve-logs-request 0 1 1 0 --TransferFileDesignator enduser-diag.log

# Retrieve crash summary over BDX
> diagnosticlogs retrieve-logs-request 2 1 1 0 --TransferFileDesignator crash-summary.bin
```

esp-idf supports storing and retrieving
[core dump in flash](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/core_dump.html#core-dump-to-flash).

To support that, application needs to add core dump partition's entry in
[partitions.csv](partitions.csv#7) and we need to enable few extra menuconfig options along with above diagnostic tracing options.

```
CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH=y
CONFIG_ESP32_COREDUMP_DATA_FORMAT_ELF=y
```

This example's partition table and sdkconfig.default are already modified

-   Retrieve the core dump using diagnostic logs cluster

    ```
    # Read crash summary over BDX
    chip-tool interactive start
    > diagnosticlogs retrieve-logs-request 2 1 1 0 --TransferFileDesignator crash-summary.bin
    ```

#### To build the app with a specific PROJECT_VER and PROJECT_VER_NUMBER for OTA firmware, use the following command from the command line:

   For e.g.:

   `idf.py -DCLI_PROJECT_VER="10.0" -DCLI_PROJECT_VER_NUMBER=10 build`

On boot-up esp-idf console starts. In order to create a device user have to use console command.

-   Use `create --device_type` to list all supported device types.
-   Use `create --device_type=on_off_light` to create light device.
-   To delete existing device perform `matter esp factoryreset`.
-   To add new device, say fan or any other device type use `create --device_type=fan`.

Setup OTBR for a device

-   Please use the [OTBR board](https://github.com/espressif/esp-thread-br#hardware-platforms) as the hardware platform running this example.
-   The sdkconfig file `sdkconfig.defaults.otbr` is provided to enable the OTBR feature on the device. Build and flash the example with the sdkconfig file 'sdkconfig.defaults.otbr'
```
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.otbr" set-target esp32s3 build
idf.py -p <PORT> erase-flash flash monitor

```
-   After pairing the device via ble-wifi method with chip-tool command, you can initialize a new Thread network dataset and commit it as the active one, user should use console command to do the below operation.
```
matter esp ot_cli dataset init new
matter esp ot_cli dataset commit active
matter esp ot_cli ifconfig up
matter esp ot_cli thread start
```
-   Get the operational dataset TLV-encoded string command
```
matter esp ot_cli dataset active -x

Done
0e080000000000010000000300000d35060004001fffe00208c62a4c5f80346de00708fd9ae261c39015460510874e399792fcb276c571dee6f71c260a030f4f70656e5468726561642d3861363301028a630410a6676a0f60ca6dd3af116bdefd2a21270c0402a0f7f8
Done

```

You can simulate changing an attribute value locally using the command

-   Command to set attribute value: `matter esp attribute set <endpoint_id> <cluster_id> <attribute_id> <value>`
-   Command to get attribute value: `matter esp attribute get <endpoint_id> <cluster_id> <attribute_id>`


Refer examples/all_device_types_app/main/device_types.h for supported device types.

## 3. Post Commissioning Setup

No additional setup is required.
