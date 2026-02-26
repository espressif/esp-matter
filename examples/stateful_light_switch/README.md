# Stateful Light Switch

A composite Matter device that combines:
- **Dimmable Light (0x0101)**: Server clusters for On/Off and Level Control, controls local WS2812 LED
- **Dimmer Switch (0x0104)**: Client clusters for sending commands to bound devices

Both device types are on a single endpoint, allowing the device to act as both a controllable light and a switch controller.

## Hardware Requirements

- ESP32-C6 development board
- WS2812 RGB LED on **GPIO 8** (used as white light only)
- Button on **GPIO 9**

## Device Functionality

### Matter Clusters

| Cluster | Role | Purpose |
|---------|------|---------|
| On/Off | Server | Receives on/off commands, controls local LED |
| On/Off | Client | Sends on/off commands to bound devices |
| LevelControl | Server | Receives brightness commands, controls LED brightness |
| LevelControl | Client | Sends brightness commands to bound devices |
| Binding | Server | Stores bindings to remote devices |

### Button Operations

| Press Pattern | Action | Description |
|--------------|--------|-------------|
| **Single Short Press** | Toggle | Toggles local LED on/off and sends Toggle command to bound devices |
| **Hold** | Dim Up | Increases brightness gradually. Sends Move command to bound devices at start, Stop on release |
| **Short Press + Hold** | Dim Down | Decreases brightness gradually. Press briefly, then hold within 1.5s to dim down |

### LED Behavior

- LED brightness follows the LevelControl CurrentLevel attribute (0-254)
- LED turns off completely when On/Off attribute is OFF
- LED can be controlled locally or remotely from a Matter controller (e.g., Home Assistant)

## Building and Flashing

```bash
cd examples/stateful_light_switch
idf.py set-target esp32c6
idf.py build flash monitor
```

## Post Commissioning Setup

### Bind a Light to the Switch

After commissioning both devices, update the light's ACL and the switch's binding:

1. Update light's ACL to allow switch access:
```
chip-tool accesscontrol write acl '[{"privilege": 5, "authMode": 2, "subjects": [112233, <switch_node_id>], "targets": null}]' <light_node_id> 0
```

2. Add binding on switch to control the light:
```
chip-tool binding write binding '[{"node": <light_node_id>, "endpoint": 1, "cluster": 6}, {"node": <light_node_id>, "endpoint": 1, "cluster": 8}]' <switch_node_id> 1
```

Note: Cluster 6 = On/Off, Cluster 8 = Level Control

## Console Commands

If console is enabled (`CONFIG_ENABLE_CHIP_SHELL=y`):

### Send commands to bound devices
```
matter esp bound invoke <local_endpoint_id> <cluster_id> <command_id> [parameters]
```

Examples:
- Toggle: `matter esp bound invoke 0x1 0x6 0x2`
- On: `matter esp bound invoke 0x1 0x6 0x1`
- Off: `matter esp bound invoke 0x1 0x6 0x0`

## Troubleshooting

### "failed to notify the bound cluster changed"
This error appears when no devices are bound to the switch. It is expected behavior and will disappear once you configure bindings.

### Brightness changes from controller don't work
Ensure the device is commissioned and the controller has proper ACL access. The device processes attribute updates in the PRE_UPDATE callback.
