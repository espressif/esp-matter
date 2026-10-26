# Controller

Controller + commissioner example. Required configuration:
`ESP_MATTER_CONTROLLER_ENABLE=y`, `ESP_MATTER_COMMISSIONER_ENABLE=y`, and
`ESP_MATTER_ENABLE_MATTER_SERVER=n`.


See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

No additional setup is required.

## 2. Controller Example

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/controller.html) for more information
about pairing and controlling an end-device using this example.

### Saved devices

- Registry implementation and device commands: `main/devices.cpp`.
- Capacity: 32 node IDs, stored in Matter's persistent KVS and retained across reboot.
- Successful pairing saves the node ID. Storage failures report the node as
  commissioned but not saved.
- A full registry rejects new nodes before commissioning.
- Registry load errors do not block boot. Invalid or unsupported data remains
  unchanged; device commands other than `clear` require an available registry.
- `matter esp device clear` persists an empty registry and marks it available
  after a successful write. Active pairing blocks clearing.
- Clearing the registry does not reset credentials, unpair peers, or stop subscriptions.
  Matter factory reset clears the registry and credentials.
- Registry formats are not migrated. Saved node IDs alone cannot restore fabric
  access without the matching credentials.

### Pairing

- Setup codes: QR payload text or manual setup codes.
- `--transport auto|ble|on-network`: default `auto`.
- Network credentials: `--wifi <ssid> <password>` or `--thread <dataset-hex>`.
- The pairing API copies credentials before command return.
- `device pair status`: pairing observation state.
- `device pair cancel`: cancellation request with completion callbacks retained.
- `device pair stop`: callback detachment and cancellation request.

```text
matter esp wifi connect <ssid> <password>
matter esp device pair 1234 <setup-code> --transport on-network
matter esp device list
```

### Controller console

- Shell initialization: `esp_matter::console::init()`.
- Registered controller modules: read, write, invoke, subscription, window and ICD.
- Independent group module: `matter esp group`.
- Generic controller pairing CLI: not registered.
- Node IDs: decimal or `0x`-prefixed hexadecimal.
- Write and invoke payloads: `--` followed by one JSON argument.
- JSON fields: shell-quoted payloads with escaped double quotes and backslashes.
  Empty object: `-- {}`.

```text
matter esp controller read attr 1234 1 6 0
matter esp controller invoke 1234 1 6 2 -- {}
matter esp controller write attr 1234 0 40 5 -- "{\"0:STR\":\"desk lamp\"}"
matter esp controller sub add attr 1234 0xffff 0xffffffff 0xffffffff 1 60
matter esp controller sub list
matter esp controller sub remove 1234 all
matter esp device remove 1234
matter esp device list
```

- `device remove` persists local deletion before best-effort peer fabric removal.
- Local storage errors fail removal. Peer errors do not restore the record;
  unreachable peers may remain commissioned.
- Removal does not stop subscriptions.

### Commissioning windows

```text
matter esp controller window open-basic 1234 180
matter esp controller window open-passcode 1234 180 --iterations 10000 --discriminator 3840
matter esp controller window status
matter esp controller window stop
```

- Basic windows require target support.
- Status and stop apply to the local request.
- Stop does not revoke an already-open window.

### Group commands

- Group destinations use the active fabric.
- Destinations: `group <id>` or `0xFFFFFFFFFFFFXXXX` (`XXXX`: 16-bit group ID).
- Keyword form: no endpoint; encoded form: endpoint ignored.
- Group invocation: no timed option; success indicates dispatch, not member
  acknowledgement.
- Read and Subscribe: unicast only.

```text
matter esp controller invoke group 1 6 1 -- {}
matter esp controller invoke 0xFFFFFFFFFFFF0001 0 6 1 -- {}
matter esp controller write attr group 1 6 0x4003 -- "{\"0:U8\":2}"
matter esp controller write attr 0xFFFFFFFFFFFF0001 0 6 0x4003 -- "{\"0:U8\":2}"
```

- `matter esp group list|add|remove|bind|unbind` and `group keyset` manage
  controller-local configuration, not remote device membership.

## 3. Controller in Rainmaker Fabric

Matter Controller in Rainmaker Fabric is available in [RainMaker Matter Examples](https://github.com/espressif/esp-rainmaker/tree/master/examples/matter).

Two different controller types are provided in RainMaker repository:

- [Matter Server-based Controller](https://github.com/espressif/esp-rainmaker/tree/master/examples/matter/matter_controller): The controller is set up with a Matter server instance. So it can be commissioned to Matter fabric with standard Matter commissioning flow. Then the NOC will be updated to controller's NOC during controller setting up.
- [Matter client-only Controller](https://github.com/espressif/esp-rainmaker/tree/master/examples/matter/client_only_matter_controller): The controller is a client-only Controller. It doesn't have standard Matter data model. And it uses RainMaker provisioning to join a Wi-Fi network.

## 4. OpenThread Border Router (OTBR) feature

### 4.1 Hardware Platform

See the [docs](https://github.com/espressif/esp-thread-br#hardware-platforms) for more information about the hardware platform.

### 4.2 Build

`sdkconfig.defaults.otbr` holds the OTBR-specific options and is layered on top of the base
`sdkconfig.defaults`. Build and flash the controller example with both:

```
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.otbr" set-target esp32s3 build
idf.py -p <PORT> erase-flash flash monitor
```

*Notes*: The Thread Border Router DevKit Board uses USB port.

### 4.3 Pair and Control

- Connect the controller to Wi-Fi network with the device console

```
matter esp wifi connect {ssid} {password}
```

- Initializing a new Thread network dataset and commit it as the active one

```
matter esp ot_cli dataset init new
matter esp ot_cli dataset commit active
```

- Getting the operational dataset TLV-encoded string. The `<dataset_tlvs>` will be printed.

```
matter esp ot_cli dataset active -x
```

- Starting the Thread network

```
matter esp ot_cli ifconfig up
matter esp ot_cli thread start
```

- Pairing the Thread end-device

```
matter esp device pair 1234 <setup-code> --thread <dataset_tlvs> --transport ble
matter esp device list
```

- Control the Thread end-device on the device console (On/Off cluster Toggle command)

```
matter esp controller invoke 1234 1 6 2 -- {}
```

## A1 Appendix FAQs

### A1.1 Pairing Command Failed

I cannot finish the commissioning on the controller example:

-   For onnetwork pairing, please make sure that the end-device and the controller are on the same IP-network.
-   The controller uses the hard-code test PAA certificate by default so the PAI and DAC on the end-device should be generated by the [test cert](https://github.com/espressif/connectedhomeip/blob/4f7669b052b16bd054227376e1bbadac85419793/credentials/test/attestation/Chip-Test-PAA-NoVID-Cert.pem) and the [test key](https://github.com/espressif/connectedhomeip/blob/4f7669b052b16bd054227376e1bbadac85419793/credentials/test/attestation/Chip-Test-PAA-NoVID-Key.pem)
-   If you are still facing issues, reproduce the issue on the default example for the device and then raise an [issue](https://github.com/espressif/esp-matter/issues). Make sure to share these:
    -   The complete device logs for both the devices taken over UART.
    -   The esp-matter and esp-idf branch you are using.

### A1.2 Command Send Failed

I cannot send commands to the light from the controller:

-   Make sure the pairing command was a success.
-   The command data of cluster invoking command and attribute value data of attribute writing command should be a JSON string that matches the cluster description in Matter SPEC.
-   The CASESession will be lost on end-device after reboot. But the controller will still send commands on the previous CASESession. The controller will release previous CASESession after the last command is timeout. So the second command will re-establish a new CASESession in this situation.
-   If you are still facing issues, reproduce the issue on the default example for the device and then raise an [issue](https://github.com/espressif/esp-matter/issues). Make sure to share these:
    -   The complete device logs for both the devices taken over UART.
    -   The esp-matter and esp-idf branch you are using.

### A1.3 RAM optimization

The controller is RAM heavy, so `sdkconfig.defaults` puts the Wi-Fi/LWIP, NimBLE, Matter and mbedTLS allocations plus the BSS segments in PSRAM (`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY=y`). The `esp_matter` and `esp_matter_controller` components move the `libCHIP.a` / `libesp_matter.a` / `libesp_matter_controller.a` BSS automatically; [`main/linker.lf`](./main/linker.lf) covers `libmain.a` and `libopenthread.a`.

-   `sdkconfig.defaults` uses octal PSRAM (`CONFIG_SPIRAM_MODE_OCT=y`). If the device crashes at startup with `PSRAM chip not found or not supported, or wrong PSRAM line mode`, set the mode that matches your module, e.g. `CONFIG_SPIRAM_MODE_QUAD=y` for a 2 MB quad-PSRAM part.
-   See [Configuration options to optimize RAM and Flash](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/optimizations.html) for further options.
