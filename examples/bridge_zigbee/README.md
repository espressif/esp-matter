# Bridge Example

## Building and Flashing the Firmware

This example should be built on IDF commit [b05b70c7f3](https://github.com/espressif/esp-idf/tree/b05b70c7f39a45b9bb8d09498b45edbe3b7bfc22)

See the [README.md](../../README.md) file for more information about building and flashing the firmware.

The Matter Bridge device is composed of two parts: The RCP running on ESP32-H2 and the bridge app
running on ESP32.

### Hardware connection

Connect the two SoCs via UART, below is an example setup with ESP32 DevKitC and ESP32-H2 DevKitC:

  ESP32 Pin  | ESP32-H2 Pin
-------------|--------------
   GND       |    GND
   GPIO4     |    GPIO7
   GPIO5     |    GPIO8

### Build and flash the RCP (ESP32-H2)

```
$ cd ${IDF_PATH}/examples/zigbee/esp_zigbee_rcp/
$ idf.py --preview set-target esp32h2
$ idf.py -p <port> build flash
```

The Matter Bridge app will run on the ESP32 and ZigBee network will be formed.

## Build chip-tool and provision the Matter Bridge device

Open a new terminal window and active matter environment

```
$ cd esp-matter/connectedhomeip/connectedhomeip/examples/chip-tool 
$ gn gen out
$ ninja -C out
```

Now you can provision the Matter Bridge device with `./out/chip-tool` (Please ensure that your PC
and the bridge device are on the same local network).

```
$ ./out/chip-tool pairing ble-wifi 12344321 {wifi-ssid} {wifi-password} 20202021 3840
```

After Provisioning success, you can read the parts list in Bridge app to get the number of the bridged devices.

```
$ ./out/chip-tool descriptor read parts-list 12344321 0
```

If there is no other ZigBee device on the ZigBee Network, you will get an empty result.

```
[1639378931.513638][1808055:1808060] CHIP:DMG:                                  Data = [
[1639378931.513641][1808055:1808060] CHIP:DMG: 
[1639378931.513645][1808055:1808060] CHIP:DMG:                                  ],
```

## Setup ZigBee Bulb on ESP32-H2

Build and run ZigBee Bulb app on another ESP32-H2 board. Open another terminal window and repeat Step 2 again.

```
$ cd ${IDF_PATH}/examples/zigbee/light_sample/light_bulb
$ idf.py --preview set-target esp32h2
$ idf.py -p <port> build flash monitor
```

The Zigbee Bulb will be added to the ZigBee Network and a dynamic endpoint will be added on the Bridge device. You can read the parts list again to get the dynamic endpoint ID.

```
$ ./out/chip-tool descriptor read parts-list 12344321 0
...
[1639379769.737877][1809119:1809124] CHIP:DMG:                                  Data = [
[1639379769.737881][1809119:1809124] CHIP:DMG:                                          1, 
[1639379769.737885][1809119:1809124] CHIP:DMG:                                  ],
```

It means that the ZigBee Bulb is added as Endpoint 1 on the Bridge device. You can read the cluster servers list on the dynamic endpoint.

```
$ ./out/chip-tool descriptor read server-list 12344321 1
...
[1639380020.748687][1809427:1809432] CHIP:TOO: OnDescriptorServerListListAttributeResponse: 4 entries
[1639380020.748695][1809427:1809432] CHIP:TOO:   [1]: 6
[1639380020.748699][1809427:1809432] CHIP:TOO:   [2]: 29
[1639380020.748703][1809427:1809432] CHIP:TOO:   [3]: 57
[1639380020.748706][1809427:1809432] CHIP:TOO:   [4]: 64
```

## Control the bulb with chip-tool

Now you can control the ZigBee bulb on chip tool.
```
$ ./out/chip-tool onoff toggle 12344321 1
```

