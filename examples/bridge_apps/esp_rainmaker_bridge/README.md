# Rainmaker Bridge

This example demonstrates a Matter-Rainmaker Bridge that bridges Rainmaker devices to Matter fabric.

The Matter Bridge device is running on ESP32-S3.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

💡 Important:  `create_bridge_devices` callback can be used to add data model elements (e.g., attributes, commands, etc.) to the bridge endpoint.

## 1. Additional Environment Setup

### 1.1 Hardware connection

This example run on ESP32S3 devkit by default.

### 1.2 Build and flash the Bridge (ESP32-S3)

For Standalone DevKit boards:

```
cd ${ESP_MATTER_PATH}/examples/bridge_apps/esp_rainmaker_bridge
idf.py set-target esp32s3
idf.py -p <port> build flash
```

## 2. Commissioning Setup

### 2.1 Use Rainmaker App pairing Bridge

Use Rainmaker App scan the qrcode print in device log to pairing the bridge

```
I (16104) NimBLE: GAP procedure initiated: advertise;
I (16104) NimBLE: disc_mode=2
I (16114) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=256 adv_itvl_max=256
I (16124) NimBLE:

I (16104) network_prov_mgr: Provisioning started with service name : PROV_54a900
I (16134) app_wifi: Provisioning started
I (16134) app_network: Scan this QR code from the ESP RainMaker phone app for Provisioning.
I (16144) QRCODE: Encoding below text with ECC LVL 0 & QR Code Version 5
I (16144) QRCODE: {"ver":"v1","name":"PROV_54a900","pop":"47d4fb71","transport":"ble"}

  █▀▀▀▀▀█ █  ▄▄█ ▀▀  ▄█  ▀▄ █▀▀▀▀▀█
  █ ███ █ ██▄ █▄ ▄▀▀▄ ▀▀▄██ █ ███ █
  █ ▀▀▀ █ ▄██ ▀▀█▀█▄▀▄ ▀▀▀▀ █ ▀▀▀ █
  ▀▀▀▀▀▀▀ ▀ █▄▀ ▀ ▀▄▀▄█▄▀▄█ ▀▀▀▀▀▀▀
  ██ ▄█▀▀▄▄ ▀ ▀ ▀█▄▀▄▀▄▀▄▄▄▄ █ ▀█▀▀
   ▄▀█▀█▀▀ █▀ ▄▀ ██▄  ▄██▀██▀▄██▀█
  ▄█▀█▄▀▀▀▄ ▄▀▄▄█▄█▀▀▀▄▀▄▀ ▄▀▄▀ ▄▄▀
   ▀█ ▀▄▀▄█▀██▀▀ ▀▄▄█▄ ██▀▄█ ▀ █▀▀▄
  ▀█▀▄█▀▀██▀  ▀▄▄▄█▀▀▀█▀   █▀▄▀▀  ▀
  ▄█▄▀▀▀▀ ▄██ ▄▀ ▀▀█▄▄ ▄█▀█ ▄█▄█▀▀▄
  ███ █▀▀█▀  ▀▄ ▄█▄▀█ ▄█ ▀▀▀▀ ▀ ▄█▀
      █▀▀ ▀▄▄█▀ ▀█▄▄ ▄ █▄▀  █▀▀▀▀▄▄
  ▀▀▀▀ ▀▀▀█ █ ▀▄▄▄▄██▀█▀ ▄█▀▀▀██▄▀
  █▀▀▀▀▀█ ▄▀▀ ▄ █▀██▀  ▄█▀█ ▀ ██▀▀
  █ ███ █ ▀▀▄▀█▄█▄█▀█▀ ▀█ ███▀█▀▄▄█
  █ ▀▀▀ █ ▄▄███▀ █▀▄ ▄███▀  ▀█▀▄ ▀
  ▀▀▀▀▀▀▀ ▀ ▀▀    ▀▀  ▀▀▀▀▀▀      ▀


I (16344) app_network: If QR code is not visible, copy paste the below URL in a browser.
https://rainmaker.espressif.com/qrcode.html?data={"ver":"v1","name":"PROV_54a900","pop":"47d4fb71","transport":"ble"}
I (16364) app_network: Provisioning will auto stop after 30 minute(s).

```

### 2.2 Use Rainmaker App pairing rainmaker end device

Follow this [guide](https://github.com/espressif/esp-rainmaker/blob/master/README.md) to setup Rainmaker device

### 2.3 Use chip-tool pairing bridge through onnetwork method

Use below command to pairing bridge

```
./chip-tool pairing onnetwork 0x1234 20202021
```

### 2.4 Control the bulb with chip-tool

Now you can control the Rainmaker device using the chip tool.

```
./chip-tool onoff toggle 0x1234 0x2
```

## 3. Device Performance

### 3.1 Memory usage

The following is the Memory and Flash Usage.

-   `Bootup` == Device just finished booting up. Device is not
    commissioned or connected to wifi yet.
-   `After Commissioning` == Device is connected to wifi and is also
    commissioned and is rebooted.
-   `After Adding a Bridged device` == A Rainmaker Color Light is added
    on the Bridge.
-   device used: ESP32-S3-DevKitC-1
-   tested on:
    [d0faa92c](https://github.com/espressif/esp-matter/commit/d0faa92c9336205de21a4b325c956893736c4d64)
    (2025-09-29)
-   IDF: v5.4.1 [4c2820d3](https://github.com/espressif/esp-idf/tree/v5.4.1)

|                         | Bootup | After Rainmaker Commissioning | After Matter Commissioning | After Adding a Bridged device |
|:-                       |:-:     |:-:                            |:-:                         |:-:                            |
|**Free Internal Memory** |162KB   |118KB                          |118KB                       |106KB                          |

**Flash Usage**: Firmware binary size: 1.65MB

This should give you a good idea about the amount of free memory that is
available for you to run your application's code.
