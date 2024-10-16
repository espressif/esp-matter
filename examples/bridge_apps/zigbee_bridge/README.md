# Zigbee Bridge

This example demonstrates a Matter-Zigbee Bridge that bridges Zigbee devices to Matter fabric.

The Matter Bridge device is composed of two parts: The RCP running on ESP32-H2 and the bridge app running on ESP32-S3.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

💡 Important:  `create_bridge_devices` callback can be used to add data model elements (e.g., attributes, commands, etc.) to the bridge endpoint.

## 1. Additional Environment Setup

### 1.1 Hardware connection

There are two hardware type options for this example. You can choose one of the two options in menuconfig `ESP Matter Zigbee Bridge Example`->`Zigbee Bridge board type`.

#### 1.1.1 Standalone DevKit boards
Connect the two SoCs via UART, below is an example setup with ESP32-S3 DevKitC and ESP32-H2 DevKitC:

![Zigbee Bridge Hardware Connection](../../docs/_static/zigbee_bridge_hardware_connection.jpg)

|  ESP32-S3 Pin  | ESP32-H2 Pin |
|----------------|--------------|
|   GND          |    GND       |
|   GPIO4        |    TX        |
|   GPIO5        |    RX        |

#### 1.1.2 Zigbee Gateway DevKit board

![Zigbee Gateway DevKit Board](../../docs/_static/esp-thread-border-router-zigbee-gateway-board.png)

### 1.2 Build and flash the RCP (ESP32-H2)

Please use the [ot_rcp](https://github.com/espressif/esp-idf/tree/master/examples/openthread/ot_rcp) example to build the RCP for the ZigBee Bridge.

```
cd $IDF_PATH/examples/openthread/ot_rcp/
idf.py menuconfig -> OPENTHREAD_NCP_VENDOR_HOOK=y
idf.py set-target esp32h2
idf.py -p <port> build flash
```

**Note**: The two SoCs on the Zigbee Gateway DevKit board use USB ports while the standalone DevKit boards use UART ports.

### 1.3 Build and flash the Bridge (ESP32-S3)

For Standalone DevKit boards:

```
cd ${ESP_MATTER_PATH}/examples/zigbee_bridge
idf.py set-target esp32s3
idf.py -p <port> build flash
```

For Zigbee Gateway board:

```
cd ${ESP_MATTER_PATH}/examples/zigbee_bridge
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.zb_gw_board" set-target esp32s3 build
idf.py -p <port> flash
```

The Matter Zigbee Bridge will run on the ESP32-S3 and Zigbee network will be formed.

## 2. Post Commissioning Setup

### 2.1 Discovering Zigbee Devices

Commissioning the Matter Zigbee Bridge first, 0x7283 in description below is its Node Id.

You can read the PartsList from the Bridge to get the number of the bridged devices.

```
descriptor read parts-list 0x7283 0x0
```

If there is no other Zigbee device on the Zigbee Network, you will get a result with only an aggregator endpoint. Example:

```
Data = [
    1,  <---------------------------- Aggregator Endpoint
],
```

Then read the PartsList from the Aggregator Endpoint, you will get an empty result.

```
descriptor read parts-list 0x7283 0x1
...
Data = [

],
...
```

### 2.2 Setup Zigbee Bulb on ESP32-H2

Build and run Zigbee Bulb app on another ESP32-H2 board.

```
cd /path/to/esp-zigbee-sdk/examples/esp_zigbee_HA_sample/HA_on_off_light
idf.py set-target esp32h2
idf.py -p <port> build flash monitor
```

The Zigbee Bulb will be added to the Zigbee Network and a dynamic endpoint will be added on the Bridge device. You can read the PartsList on Aggregator Endpoint again to get the dynamic endpoint ID.

```
descriptor read parts-list 0x7283 0x1
```

The data will now contain the information of the connected Zigbee devices. Example:

```
Data = [
    2,  <--------------------------Bridged Endpoint
],
```

It means that the Zigbee Bulb is added as Endpoint 1 on the Zigbee Bridge. You can read the DeviceTypeList on the dynamic endpoint.

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

You can also read the cluster ServerList on the dynamic endpoint.

```
descriptor read server-list 0x7283 0x1
```

This will give the list of supported server clusters. Example:

```
OnDescriptorServerListListAttributeResponse: 4 entries
    [1]: 3
    [2]: 4
    [3]: 5
    [4]: 6    <------------------------ OnOff Cluster
    [5]: 57   <------------------------ Bridged Device Basic Information Cluster
```

### 2.3 Control the bulb with chip-tool

Now you can control the Zigbee bulb using the chip tool.

```
onoff toggle 0x7283 0x2
```

## 3. Device Performance

### 3.1 Memory usage

The following is the Memory and Flash Usage.

-   `Bootup` == Device just finished booting up. Device is not
    commissioned or connected to wifi yet.
-   `After Commissioning` == Device is connected to wifi and is also
    commissioned and is rebooted.
-   `After Adding a Bridged device` == A Zigbee OnOff Light is added
    on the Bridge.
-   device used: esp32_devkit_c
-   tested on:
    [b40bf8e3](https://github.com/espressif/esp-matter/commit/b40bf8e398161bcac515fd57eb13d14e031e3a91)
    (2023-04-17)
-   IDF: release/v5.1 [420ebd20](https://github.com/espressif/esp-idf/commit/420ebd208ae9eb71cb71ebd22742d1175f11addc)

|                         | Bootup | After Commissioning | After Adding a Bridged device |
|:-                       |:-:     |:-:                  |:-:                            |
|**Free Internal Memory** |40KB    |113KB                |109KB                          |

**Flash Usage**: Firmware binary size: 1.6MB

This should give you a good idea about the amount of free memory that is
available for you to run your application's code.

Applications that do not require BLE post commissioning, So BLE is disable
once commissioning is complete in the test.
