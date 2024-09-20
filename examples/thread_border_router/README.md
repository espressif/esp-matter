# Thread Border Router

This example creates a Matter Thread Border Router device using the ESP Matter data model.


See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

### 1.1 Hardware Platform

The [ESP Thread Border Router board](https://github.com/espressif/esp-thread-br?tab=readme-ov-file#esp-thread-border-router-board) which provides an integrated module of an ESP32-S3 and an ESP32-H2 is required for this example.

### 1.2 Firmware for RCP

The [OpenThread RCP](https://github.com/espressif/esp-idf/tree/master/examples/openthread/ot_rcp) should be run on ESP32-H2 of the Border Router board. You can flash it directly:


```
$ cd /path/to/esp-idf/examples/openthread/ot_rcp
$ idf.py set-target esp32h2 build
$ idf.py -p <port> erase-flash flash
```

Or you can flash the firmware of ESP32-H2 with [esp_rcp_update](https://github.com/espressif/esp-thread-br/tree/main/components/esp_rcp_update) after enabling `AUTO_UPDATE_RCP` in menuconfig:

```
$ cd /path/to/esp-idf/examples/openthread/ot_rcp
$ idf.py set-target esp32h2 build
```

After flashing the Thread Border Router firmware to ESP32-S3, it will flash the RCP firmware to ESP32-H2 automatically.

## 2. Post Commissioning Setup

After commissioning the Border Router with chip-tool, you can set up the Thread network with ThreadBorderRouterManagement cluster.

```
$ ./chip-tool generalcommissioning arm-fail-safe 180 1 0x7283 0
$ ./chip-tool threadborderroutermanagement set-active-dataset-request hex:<thread-dataset-tlvs> 0x7283 1
$ ./chip-tool generalcommissioning commissioning-complete 0x7283 0
```
Then the Thread Border Router will form/join a Thread network and you can commission a Thread End-device to the Thread network with chip-tool.

```
$ ./chip-tool pairing ble-thread 0x7384 hex:<thread-dataset-tlvs> 20202021 3840
```
