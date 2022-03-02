# BLE Mesh Bridge

This example demonstrates a Matter-BLE Mesh Bridge that bridges BLE Mesh devices to Matter fabric.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Commissioning Setup

### 1.1 Discovering BLE Mesh Devices

You can read the parts list from the Bridge to get the number of the
bridged devices.

```
chip-tool descriptor read parts-list 0x7283 0x0
```

If there is no other BLE Mesh device on the BLE Mesh Network, you will get
an empty result. Example:

```
Data = [

],
```

### 1.2 Setup BLE Mesh Node on ESP32-C3

Build and run BLE Mesh onoff_server app on another ESP32-C3 board.

```
$ cd ${IDF_PATH}/examples/bluetooth/esp_ble_mesh/ble_mesh_node/onoff_server
$ idf.py set-target esp32c3
$ idf.py -p <port> build flash monitor
```

The BLE Mesh device will be provisioned by provisioner and a dynamic
endpoint will be added on the Bridge device. You can read the parts list
again to get the dynamic endpoint ID.

```
$ chip-tool descriptor read parts-list 12344321 0
```

The data will now contain the information of the connected BLE Mesh
devices. Example:

```
Data = [
        1, 
],
```

It means that the BLE Mesh Node is added as Endpoint 1 on the Bridge
device. You can read the cluster servers list on the dynamic endpoint.

```
$ chip-tool descriptor read server-list 12344321 1
```

This will give the list of supported server clusters. Example:

```
OnDescriptorServerListListAttributeResponse: 4 entries
  [1]: 6
  [2]: 29
  [3]: 57
  [4]: 64
```

### 1.3 Control the BLE Mesh Node with chip-tool

Now you can control the BLE Mesh Node on chip tool.

```
$ ./out/chip-tool onoff toggle 12344321 1
```

## 2. Device Performance

### 2.1 Memory usage

The following is the Memory and Flash Usage.

-   `Bootup` == Device just finished booting up. Device is not
    commissioned or connected to wifi yet.
-   `After Commissioning` == Device is connected to wifi and is also
    commissioned and is rebooted.
-   device used: esp32c3_devkit_m
-   tested on:
    [2d04492](https://github.com/espressif/esp-matter/commit/2d044929ab78e9b036e41ed54e155a242e1d2e73)
    (2022-05-20)

|                         | Bootup | After Commissioning |
|:-                       |:-:     |:-:                  |
|**Free Internal Memory** |100KB   |95KB                 |

**Flash Usage**: Firmware binary size: 1.4MB

This should give you a good idea about the amount of free memory that is
available for you to run your application's code.
