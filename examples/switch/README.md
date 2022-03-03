# Switch Example

## Building and Flashing the Firmware

### Commissioning using  chip-tool

#### Pair a device over BLE

Commission switch and light using chip-tool

##### Command:

```
./out/debug/chip-tool pairing ble-wifi node-id SSID PSK SETUP_PINCODE DISCRIMINATOR

```
for switch SETUP_PINCODE = 20212020 DISCRIMINATOR = 240 (0xF0)

See the [README.md](../../README.md) file for more information about building and flashing the firmware.


## What to expect in this example?

Supported features:
 - Matter Commissioning
 - Switch will connect to light when Bind command is sent 
 - CLIs to control remote light from switch


## Bind light to switch

Send Bind command from chip tool to switch which adds entry of remote device (light) in binding table. 

```
 ./out/debug/chip-tool binding bind RemoteNodeId RemoteGroupId RemoteEndpointId RemoteClusterId node-id endpoint-id
```
e.g.
```
./out/debug/chip-tool binding bind 12344322 0 1 6 12344321 1
```
note: 
- 12344321 : node Id of switch used during commissioning
- 12344322 : node Id of light used during commissioning 
- Cluster Id for OnOff cluster is 6
-  binding cluster is currently present on endpoint 1

When bind command is sent switch will establish a CASE Session with light. After bind command on chip tool returns success, wait for 5-10 seconds until you see `CASE Session established` message in logs on switch side.

### Useful shell commands

After the binding is successful you can send the following command to control light from switch

```
> matter esp driver send_bind <endpoint_id> <cluster_id> <command_id>
```
here, endpoint_id, cluster_id, command_id of switch must be given in hex format 

e.g.

- Power Off command

```
> matter esp driver send_bind 0x0001 0x0006 0x0000 
```

- Power On command

```
> matter esp driver send_bind 0x0001 0x0006 0x0001
```

- Toggle command

```
>  matter esp driver send_bind 0x0001 0x0006 0x0002
```

