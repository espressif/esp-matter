# Switch Example

## Building and Flashing the Firmware
First update the submodules:

```
git submodule sync --recursive
git submodule update --init --recursive
```
Commission the light and switch device using chip-tool. 

### Commissioning using  chip-tool

Build the chip-tool example application using these commands:

```
cd examples/chip-tool
git submodule update --init
source third_party/connectedhomeip/scripts/activate.sh
gn gen out/debug
ninja -C out/debug
```

#### Pair a device over BLE

Commission switch and light using chip-tool command:

```
./out/debug/chip-tool pairing ble-wifi node-id SSID PSK SETUP_PINCODE DISCRIMINATOR

```
for switch SETUP_PINCODE = 20212020 DISCRIMINATOR = 240 (0xF0)

See the [README.md](../../README.md) file for more information about building and flashing the firmware.

## Bind light to switch

Send Bind command to switch which adds entry of remote device in binding table

```
 ./out/debug/chip-tool binding bind RemoteNodeId RemoteGroupId RemoteEndpointId RemoteClusterId node-id endpoint-id
```
e.g.
```
./out/debug/chip-tool binding bind 12344322 0 1 6 12344321 1
```
note: i)12344321 : node Id of switch used during commissioning
ii)12344322 : node Id of light used during commissioning 
iii) Cluster Id for OnOff cluster is 6
iv) binding cluster is currently present on endpoint 1

## What to expect in this example?

Supported features:
 - Matter Commissioning
 - Connect to remote light device 
 - Switch will connect to light when Bind command is send 

### Useful shell commands

- Toggle power of light

```
> matter esp switch on
```

- Make Power On

```
> matter esp switch off
```

- Make Power Off
```
>  matter esp switch toggle
```
