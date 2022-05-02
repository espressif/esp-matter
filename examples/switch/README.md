# Switch Example

## Building and Flashing the Firmware

See the [README.md](../../README.md) file for more information about building and flashing the firmware.

## What to expect in this example?

Supported features:
 - Matter Commissioning
 - Switch will connect to light when Bind command is sent 
 - CLIs to control remote light from switch

## Bind light to switch

Update the light's acl attribute which adds entry of remote device (switch) in the access control list:
```
./out/debug/chip-tool accesscontrol write acl '[{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [ 112233, 12344321 ], "targets": null}]' 12344322 0
```

Update the switch's binding attribute which adds entry of remote device (light) in binding table:
```
./out/debug/chip-tool binding write binding '[{"fabricIndex": 1, "node":12344322, "endpoint":1, "cluster":6}]' 12344321 1
```

note: 
- 12344321 : node Id of switch used during commissioning
- 12344322 : node Id of light used during commissioning 
- Cluster Id for OnOff cluster is 6
- Binding cluster is currently present on endpoint 1

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
