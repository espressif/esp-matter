# ESP Matter

[Matter](https://buildwithmatter.com/) is the unified IP-based connectivity protocol built on proven technologies, helping connect and build reliable, secure IoT ecosystems. This new technology and royalty-free connectivity standard enables communications among a wide range of smart devices.

ESP Matter is the official Matter development framework for the Espressif's ESP32 series SoCs.

## Development Setup

This sections talks about setting up your development host, fetching the git repositories, and instructions to build and flash.

### Host Setup

You should install drivers and support packages for your development host. Windows, Linux and Mac OS-X, are supported development hosts. Please see [Get Started](https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/index.html) for the host setup instructions.

### Getting the Repositories

This only needs to be done once:
```
$ git clone --recursive https://github.com/espressif/esp-idf.git
$ cd esp-idf
$ git checkout release/v4.4
$ git submodule update --init --recursive
$ ./install.sh
$ cd ..

$ git clone --recursive https://glab.espressif.cn/esp-matter-preview/esp-matter.git
$ cd esp-matter/connectedhomeip/connectedhomeip
$ source scripts/bootstrap.sh
```

### Configuring the environment

This needs to be done everytime a new terminal is opened:
```
$ cd /path/to/esp-idf
$ . export.sh
$ cd /path/to/esp-matter/
$ . export.sh
$ cd examples/light/

$ export ESPPORT=/dev/cu.SLAB_USBtoUART (or /dev/ttyUSB0 or /dev/ttyUSB1 on Linux or COMxx on MinGW)
```

### Building and Flashing the Firmware

Choose IDF target.
```
idf.py set-target esp32c3 (or esp32 or other supported targets)
```

*   If IDF target has not been set explicitly, then `esp32` is considered as default.
*   The default device for `esp32`/`esp32c3` is `esp32-devkit-c`/`esp32c3-devkit-m`. If you want to use another device, you can export `ESP_MATTER_DEVICE_PATH` after choosing correct target, e.g for `m5stack` device:
    ```
    $ export ESP_MATTER_DEVICE_PATH=/path/to/esp_matter/device_hal/device/m5stack
    ```
    *   If the device that you have is of a different revision, and is not working as expected, you can create a new device and export your device path.
    *   The other peripheral components like led_driver, button_driver, etc are selected based on the device selected.
    *   The configuration of the peripheral components can be found in `$ESP_MATTER_DEVICE_PATH/esp_matter_device.cmake`.

Build and flash:
```
$ idf.py build
$ idf.py flash monitor
```

*   Note: If you are getting build errors like:
    ```
    ERROR: This script was called from a virtual environment, can not create a virtual environment again
    ```
    Run:
    ```
    pip install -r $IDF_PATH/requirements.txt
    ```

## Test Setup (Python Controller Setup)

### Environment setup

```
$ cd esp-matter/connectedhomeip/connectedhomeip
$ source ./scripts/activate.sh
$ gn gen out/debug
$ ninja -C out/debug

```

### Commissioning

Use `chip-tool` to pair the device:

```
$ ./out/debug/chip-tool pairing ble-wifi 12344321 TESTSSID TESTPASSWD 0 20202021 3840
```

### Cluster Control

Use the cluster commands to control the attributes.
```
$ ./out/debug/chip-tool onoff on 12344321 1
$ ./out/debug/chip-tool levelcontrol move-to-level 10 0 0 0 12344321 1
$ ./out/debug/chip-tool levelcontrol move-to-level 100 0 0 0 12344321 1
$ ./out/debug/chip-tool colorcontrol move-to-saturation 200 0 0 0 12344321 1
$ ./out/debug/chip-tool colorcontrol move-to-hue 150 0 0 0 0 12344321 1
$ ./out/debug/chip-tool onoff toggle 12344321 1
```
