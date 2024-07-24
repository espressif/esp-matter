# Light with RainMaker-WiFi-Provisioning

This example creates a Color Temperature Light device using the ESP Matter data model and is integrated with ESP-RainMaker and wifi_provisioning.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Prerequisites

- ESP32-S3-DevKitM
- chip-tool (For Matter commissioning)
- Android RainMaker APP of version 3.4.1 or iOS RainMaker APP of version 3.2.0 (For RainMaker Provisioning)
- [ESP-IDF](https://github.com/espressif/esp-idf) on commit 1022b2b447d364d2f2600045c75d81dd203dd15e
- [ESP-RainMaker](https://github.com/espressif/esp-rainmaker) on commit 90a8d9edf9802ca23b14ff87fe3bbf826a1df228

## 2. Additional Environment Setup

To build this example, you need to clone ESP-RainMaker repository and export the path as RMAKER_PATH:

```
git clone https://github.com/espressif/esp-rainmaker.git <rainmaker_path>
cd <rainmaker_path>
git checkout 90a8d9edf9802ca23b14ff87fe3bbf826a1df228
git submodule update --recursive --init
export RMAKER_PATH=<rainmaker_path>
```

## 3. Post Commissioning Setup

No additional setup is required for Matter Commissioning. See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#commissioning-and-control) for more information about Matter commissioning with chip-tool

## 4. External platform

This example uses [external platform](../common/secondary_ble_adv/).

In the external platform, `SetSecondaryXX()` APIs are added in the `BLEManagerImpl` class for setting up the secondary BLE advertisement and services. After you call these APIs of `BLEManagerImpl`, there will be an additional BLE advertisement and corresponding service for wifi_provisioning after you initialize the Matter stack.

There are also some WiFi stack initialization changes to avoid duplicated Wi-Fi stack initialization of wifi_provisioning and Matter stack.

## 5. RainMaker Provisioning

After you flash the example to the DevKit board, the console will print the QR code for RainMaker Provisioning. After you scan the QR code with the RainMaker Phone App, the RainMaker provisioning will be done and the device will be added to your RainMaker home.
