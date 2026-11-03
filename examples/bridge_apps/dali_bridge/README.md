# DALI Bridge (Matter)

This example scans a DALI (IEC 62386) bus through the ESP32 RMT peripheral and creates one **bridged Matter light endpoint** for every discovered DALI short address. DT6 control gear is exposed as a Dimmable Light and DT8 control gear as an Extended Color Light.

The application is a Matter bridge: a single commissioning identity with an Aggregator endpoint and one Bridged Node per DALI control gear. Bridged endpoints are persisted in NVS, so their Matter endpoint IDs stay stable across reboots. The device is compatible with **Home Assistant** (Matter integration) and other Matter controllers.

## Matter topology

```text
Endpoint 0  Root Node
Endpoint 1  Aggregator
Endpoint 2… Bridged Dimmable Light / Extended Color Light endpoints
```

## Adding a new device

The DALI bus is scanned only once, when the Module boots. While the bridge is running, the poll task queries only short addresses that already have a bridged endpoint, so a control gear attached later is not added.

Power-cycle the Module to add gear. On the next boot the bridge scans the bus again, creates a bridged endpoint for each new short address, and reports that endpoint to the already commissioned controller. The bridge itself is not commissioned again.

Endpoint IDs already stored in NVS stay the same. A new endpoint takes the next unused ID, which is one greater than the previous bridged endpoint, or higher if some IDs in between are already used. If the last light is Endpoint 4, the new light is Endpoint 5 or a later ID.

## Reachability

The bridge polls every known DALI short address with `QUERY STATUS` every 5 seconds:

- A device that stops responding (removed, powered-off, or failed lamp) has its
`BridgedDeviceBasicInformation::Reachable` attribute set to **false**.
- The bridged endpoint is kept; when the gear answers again the attribute
returns to **true** and the last persisted on/off + brightness state is
pushed to the hardware.

## Supported clusters

| Cluster                       | Attribute                                                   | DALI mapping                       |
| ----------------------------- | ----------------------------------------------------------- | ---------------------------------- |
| BridgedDeviceBasicInformation | Reachable                                                   | `QUERY STATUS` polling             |
| OnOff                         | OnOff                                                       | `OFF` command / DAPC to last level |
| LevelControl                  | CurrentLevel (1–254)                                        | Direct Arc Power Control (DAPC)    |
| ColorControl (DT8)            | ColorTemperatureMireds / CurrentX+Y / CurrentHue+Saturation | IEC 62386-209 `SET COLOR`          |

## Hardware

| Signal  | Default GPIO | Kconfig option        |
| ------- | ------------ | --------------------- |
| DALI RX | GPIO 5                       | `CONFIG_DALI_RX_GPIO` |
| DALI TX | GPIO 6 (GPIO 4 on ESP32/H2)  | `CONFIG_DALI_TX_GPIO` |

On ESP32, GPIO6 is connected to the on-module SPI flash. On ESP32-H2-DevKitM-1, GPIO6 is not broken out. For both targets `CONFIG_DALI_TX_GPIO` defaults to GPIO4; RX stays GPIO5.

A typical interface circuit connects the ESP32 GPIOs to an opto-isolator and a DALI power supply (e.g., a simple 16 V current-limited supply).
Set `CONFIG_DALI_TX_INVERT` / `CONFIG_DALI_RX_INVERT` in `sdkconfig.defaults` if your hardware performs an odd number of signal inversions. Both default to `y` for the inverting reference transceiver; change them to `n` for a non-inverting interface.

The hardware used in this sample project includes:

- ESP32C6 module/development board
- DALI2 Communication Expansion Board
- DALI Bus Power Supply
- HDR-60-24 Transformer
- 5-Channel DALI Dimmer Controller (DT6/8)
- 4-Channel DLI Dimming Controller
- RGBCW Color LED Strips
- COB LED Strips

Hardware Connection Diagram:

```text
                           [ Type-C 5V Power Supply ]
                                       |
                                       v
                             +-------------------+
                             |       ESP32       |
                             |  + DALI to TTL    |
                             +-------------------+
                                 | DA2       | DA1
                                 |           |
      +----------------+         |           |
      | DALI Bus Power |         |           |
      |                |---------+           |
      |   (Part 101)   |---------------------+
      +----------------+         |           |
               ^                 |           |   <-- DALI Local Bus
               |                 |           |  (Two-wire, Non-polarized Backbone Network)
         ~ AC 50/60Hz ~          |           |
       (Mains Power 220V)        |           |
               |                 +-----------+
               v                 |           |
      +----------------+         |           |
      | Constant Volt. |         |           |
      |                |         |           |
      |  (12-48V DC)   |         |           |
      +----------------+         |           |
         | V+      | V-          |           |
         |         |             |           |
         |         |             |           |
         |         |             |           |
         |         |             |           |
         |         |             |           |        +--------------------+
         |         |             |           |        |                    |
         |         |             |           +------->| DA1                |
         |         |             +------------------->| DA2                |
         |         |             |           |        | DALI Driver (DT6)  |
         |         +----------------------+--+------->| DC_IN- (Part 207)  |
         +--------------------------------+--+------->| DC_IN+    LED-  V+ |
         |         |             |           |         +------------+----+--+
         |         |             |           |                      |    |
         |         |             |           |                      v    v
         |         |             |           |         +--------------------------+
         |         |             |           |         | Single-Color LED Fixtures|
         |         |             |           |         +--------------------------+
         |         |             |           |
         |         |             v           v        +---------------------------+
         |         |             |           |        |                           |
         |         |             |           +------->| DA1                       |
         |         |             +------------------->| DA2                       |
         |         |                                  |      DALI Driver (DT8)    |
         |         +--------------------------------->| DC_IN- (Part 209)         |
         +------------------------------------------->| DC_IN+                    |
                                                      |    (R   G    B  C  W  V+) |
                                                      +-----+---+---+---+--+---+---
                                                            |   |   |   |  |   |
                                                            v   v   v   v  v   v
                                                         +------------------------+
                                                         |    RGBCW LED Fixtures  |
                                                         +------------------------+
```

## Build & Flash

```bash
# Set required environment variables
cd /path/to/esp-matter
. ./export.sh                  # Set the esp-matter environment variable
cd /path/to/esp-idf
. ./export.sh                  # Set the esp-idf environment variable

cd $ESP_MATTER_PATH/examples/bridge_apps/dali_bridge

# Configure (optional — defaults are in sdkconfig.defaults)
idf.py set-target esp32c6     # or esp32, esp32s3, esp32h2, …
idf.py menuconfig

# Build and flash
idf.py build flash monitor
```

ESP32-H2 has no Wi-Fi. `sdkconfig.defaults.esp32h2` turns Wi-Fi station off and enables OpenThread, so that target is commissioned over Thread. The Kconfig default for `CONFIG_DALI_TX_GPIO` is GPIO4 on both ESP32 (GPIO6 is wired to the SPI flash) and ESP32-H2 (GPIO6 is not on the ESP32-H2-DevKitM-1 header); all other targets default to GPIO6.

> The bridge supports up to 31 bridged devices (`CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT=33` minus root and aggregator). Raise it if your installation has more DALI control gear.

## Project structure

```
dali_bridge/
├── CMakeLists.txt          Top-level project CMake
├── partitions.csv          Custom partition table (OTA-capable, 4 MB flash)
├── sdkconfig.defaults      Default Kconfig values
└── main/
    ├── CMakeLists.txt      Component registration + dali component dependency
    ├── Kconfig.projbuild   Example-level Kconfig menu (DALI GPIO defaults/invert)
    ├── app_priv.h          Driver API declarations and compile-time constants
    ├── app_main.cpp        Matter bridge data model setup
    └── app_driver.cpp      Hardware driver layer (DALI bus commands)
```
