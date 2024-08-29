# ESP-NOW Matter Bridge Light

This example demonstrates a Matter to ESP-NOW bridge that bridges an ESP-NOW switch device to Matter fabric. It integrates ESP-NOW into a Matter light device to create a ESP-NOW bridge light.

In the ESP-NOW part, it acts as a responder whereas a ESP-NOW switch is an initiator.

In the Matter part, it creates a Color Temperature Light device and an aggregator device using the ESP Matter data model. When binding with an ESP-NOW switch is complete, it also creates a dynamic OnOff Switch device that works like a normal Matter switch. When unbind from the ESP-NOW switch, the dynamic switch device is removed.

This diagram gives a simplistic view of the data models involved in the interaction.

![data model](./docs/esp-now-bridge-datamodel.drawio.svg)

See the [docs](https://docs.espressif.com/projects/esp-matter/en/main/esp32/developing.html) for more information about building and flashing the firmware.

💡 Important:  `create_bridge_devices` callback can be used to add data model elements (e.g., attributes, commands, etc.) to the bridge endpoint.

## 1. Additional Environment Setup

* Use a DevKit with LED, for example ESP32-C3-DevKitM to get more status indication.
* Use the [switch example](https://github.com/espressif/esp-now/tree/master/examples/coin_cell_demo/switch) in ESP-NOW repository as the ESP-NOW switch.
* Use the [light example](../light/) in this repository as the light device.

## 2. Post Commissioning Setup

### 2.1 Discovering bridge endpoint

You can read the parts list from the Bridge to get the endpoint ID of the bridged devices.

```
descriptor read parts-list 0x7283 0x0
```

If binding with ESP-NOW switch is not done, you will get 2 entries 1 and 2. Example:

```
PartsList: 2 entries
  [1]: 1
  [2]: 2
```

You can read the device type list for each of the endpoints.

```
descriptor read device-type-list 0x7283 1
descriptor read device-type-list 0x7283 2
```

Endpoint 1:

```
DeviceTypeList: 1 entries
  [1]: {
    Type: 268 <-------------------- Color temperature light device type
    Revision: 2
   }
```

Endpoint 2:

```
DeviceTypeList: 1 entries
  [1]: {
    Type: 14 <-------------------- Aggregator device type
    Revision: 1
   }
```

### 2.2 Bind bridge to an ESP-NOW switch

Trigger binding from the ESP-NOW switch. If you are using a C3 DevKit, double click the Boot button. If binding is successful, the LED on the DevKit running bridge example will toggle. A dynamic endpoint will be added on the Bridge device. You can read the parts list again to get the dynamic endpoint ID.

```
descriptor read parts-list 0x7283 0
```

There is a new endpoint added to the device. Example:

```
  PartsList: 3 entries
    [1]: 1
    [2]: 2
    [3]: 3
```

You can read the device type list for the new endpoint.

```
descriptor read device-type-list 0x7283 3
```

Endpoint 3:

```
DeviceTypeList: 2 entries
  [1]: {
    Type: 19 <-------------------- Bridged node device type
    Revision: 1
   }
  [2]: {
    Type: 259 <-------------------- ONOFF switch device type
    Revision: 2
   }
```

There are two device types in this endpoint: 19 (0x0013) is bridged node device type, and 259 (0x0103) is ONOFF switch device type. You can read the cluster servers and clients list on the dynamic endpoint.

```
descriptor read server-list 0x7283 3
descriptor read client-list 0x7283 3
```

Server list:

```
ServerList: 4 entries
  [1]: 29
  [2]: 57 <-------------------- Bridged device basic information cluster
  [3]: 3
  [4]: 30 <-------------------- Binding cluster
```

Client list:

```
ClientList: 2 entries
  [1]: 3
  [2]: 6 <---------------------- OnOff Cluster
```

### 2.3 Unbind bridge from an ESP-NOW switch

Trigger unbinding from the ESP-NOW switch. If you are using a C3 DevKit, long press the Boot button. If unbinding is successful, the LED on the DevKit running bridge example will toggle. The dynamic endpoint will be removed from the Bridge device. You can read the PartsList attribute again to verify the dynamic endpoint ID has been removed.

> __Note__
According to Matter specification, dynamic endpoint ID's will not be reused (apart from the exceptional wrap-around corner case). Hence if you bind the bridge to the switch again, a new endpoint number will be assigned.

### 2.4 Bind light to bridge

Follow the [instruction](../light_switch/README.md#21-bind-light-to-switch) in the light switch example to create a binding between the bridge and a Matter light. After that, pressing on the ESP-NOW switch will toggle both the LED on the bridge and the Matter light.

## 3. Device Performance

### 3.1 Memory Usage

The following is the Memory and Flash Usage.

-   `Bootup` == Device just finished booting up. Device is not commissioned or connected to Wi-Fi yet.
-   `After Commissioning` == Device is connected to Wi-Fi and is also commissioned and rebooted.
-   `After Creating Bridge` == Device is bound to an ESP-NOW switch.
-   device used: esp32c3_devkit_m
-   tested on: [2f0929e](https://github.com/espressif/esp-matter/commit/2f0929e5f8fd58b1a4cbaa2bd8b171ce50ad2a75)
    (2023-05-25)
-   IDF: release/v5.1

|                         | Bootup | After Commissioning | After Creating Bridge |
|:-                       |:-:     |:-:                  |:-:                    |
|**Free Internal Memory** |62KB    |127KB                |124KB                  |

**Flash Usage**: Firmware binary size: 1.3MB

Note that the steps of commissioning and creating bridge are interchangeable. This should give you a good idea about the amount of free memory that is
available for you to run your application's code.

Applications that do not require BLE post commissioning, can disable it using app_ble_disable() once commissioning is complete.
