# Light Example

## Building and Flashing the Firmware

See the [README.md](../../README.md) file for more information about building and flashing the firmware.


## What to expect in this example?

The example provides a minimal implementation to build a Matter light device on ESP32 series SoCs.

Supported features:
 - Matter Commissioning
 - On/Off, Brightness and Color (on ESP32-C3 for now) control
 - (Optional) Interactive shell

### Useful shell commands

- BLE commands

```
> chip ble <start|stop|state>
```

Set and get the BLE advertisement state.

- Wi-Fi commands

```
> chip wifi mode [disable|ap|sta]
```

Set and get the Wi-Fi mode.

```
> chip wifi connect <ssid> <psk>
```

Connect to Wi-Fi network.

- Device configuration

```
> chip config
```

Dump the device static configuration


- Facotry reset

```
> chip device factoryreset
```

- On-boarding codes

```
> chip onboardingcodes
```

Dump the on-boarding pairing code payloads.
