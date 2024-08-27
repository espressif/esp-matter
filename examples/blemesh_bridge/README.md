# BLE Mesh Bridge

This example demonstrates a Matter-BLE Mesh Bridge that bridges BLE Mesh devices to Matter fabric.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

💡 Important:  `create_bridge_devices` callback can be used to add data model elements (e.g., attributes, commands, etc.) to the bridge endpoint.

## 1. Additional Environment Setup

No additional setup is required.

## 2. Post Commissioning Setup

### 2.1 Discovering BLE Mesh Devices

You can read the parts list from the Bridge to get the number of the
bridged devices.

```
descriptor read parts-list 0x7283 0x0
```

If there is no other BLE Mesh device on the BLE Mesh Network, you will get
a result with an aggregator endpoint. Example:

```
Data = [
    1,  <---------------------------- Aggregator Endpoint
],
```

There is no child endpoint for the aggregator endpoint. Read the parts list
on the aggregator endpoint, and you will get an empty result.

```
descriptor read parts-list 0x7283 1
...
Data = [

],
...
```

### 2.2 Setup BLE Mesh Node on ESP32-C3

Build and run BLE Mesh onoff_server app on another ESP32-C3 board.

```
cd ${IDF_PATH}/examples/bluetooth/esp_ble_mesh/onoff_models/onoff_server
idf.py set-target esp32c3
idf.py -p <port> build flash monitor
```

The BLE Mesh device will be provisioned by provisioner and a dynamic
endpoint will be added on the Bridge device. You can read the parts list
on Endpoint 1 again to get the dynamic endpoint ID.

```
descriptor read parts-list 0x7283 1
```

The data will now contain the information of the connected BLE Mesh devices.
Example:

```
Data = [
    2,  <---------------------------- Bridged Endpoint
],
```

It means that the BLE Mesh Node is added as Endpoint 2 on the Bridge
device. You can read the device type list on the Bridged Endpoint.

```
descriptor read device-type-list 0x7283 2
```

You will get the device types of the endpoint:

```
DeviceTypeList: 2 entries
  [1]: {
    Type: 19 <-------------------- Bridged Node device type
    Revision: 1
   }
  [2]: {
    Type: 256 <-------------------- OnOff Light device type
    Revision: 2
   }
```

You can also read the cluster servers list on the dynamic endpoint.

```
descriptor read server-list 0x7283 2
```

This will give the list of supported server clusters. Example:

```
OnDescriptorServerListListAttributeResponse: 5 entries
  [1]: 3
  [2]: 4
  [3]: 5
  [4]: 6    <------------------------ OnOff Cluster
  [5]: 57   <------------------------ Bridged Device Basic Information Cluster
```

### 2.3 Control the BLE Mesh Node with chip-tool

Now you can control the BLE Mesh Node on chip tool.

```
onoff toggle 0x7283 2
```

## 3. Device Performance

### 3.1 Memory usage

The following is the Memory and Flash Usage.

-   `Bootup` == Device just finished booting up. Device is not
    commissioned or connected to wifi yet.
-   `After Commissioning` == Device is connected to wifi and is also
    commissioned and is rebooted.
-   `After Adding a Bridged Device` == A BLE-Mesh OnOff Light is added
    on the Bridge.
-   device used: esp32c3_devkit_m
-   tested on:
    [b40bf8e3](https://github.com/espressif/esp-matter/commit/b40bf8e398161bcac515fd57eb13d14e031e3a91)
    (2022-04-17)
-   IDF: release/v5.1 [420ebd20](https://github.com/espressif/esp-idf/commit/420ebd208ae9eb71cb71ebd22742d1175f11addc)

|                         | Bootup | After Commissioning | After Adding a Bridged Device |
|:-                       |:-:     |:-:                  |:=:                            |
|**Free Internal Memory** |61KB    |58KB                 |54KB                           |

**Flash Usage**: Firmware binary size: 1.6MB

This should give you a good idea about the amount of free memory that is
available for you to run your application's code.
