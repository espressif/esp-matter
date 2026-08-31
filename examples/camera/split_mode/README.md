# Matter Camera

This example creates a Camera device using the ESP Matter data model.

# Split Mode Camera Example

This example demonstrates a **two-chip split architecture** for ESP32
Camera, where signaling and media streaming are separated across two processors
for optimal power efficiency.

## Architecture Overview

The split mode consists of two separate firmware images:

### 1. **matter_camera** (ESP32-C6)

-   **Role**: Matter camera with WebRTC signaling integration
-   **Responsibilities**:
    -   WebRTC signaling
    -   Bridge communication with media adapter
    -   Always-on connectivity for instant responsiveness

### 2. **media_adapter** (ESP32-P4)

-   **Role**: Media streaming device
-   **Implementation**: Uses the `streaming_only` example from
    `${KVS_SDK_PATH}/examples/streaming_only`
-   **Responsibilities**:
    -   Video/audio capture and encoding
    -   WebRTC media streaming
    -   Power-optimized operation (sleeps when not streaming)
    -   Receives signaling commands via bridge from esp32_camera

## Supported Boards

The following boards are supported out of the box:

### [ESP32-P4 Function EV Board](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html)

-   Contains both ESP32-P4 and ESP32-C6 processors
-   Built-in camera support
-   SDIO communication between processors
-   ESP32-C6 is flashed via an external [ESP-Prog](https://docs.espressif.com/projects/esp-iot-solution/en/latest/hw-reference/ESP-Prog_guide.html)/JTAG adapter on the J2 (Prog-C6) header

### [M5Stack Tab5](https://shop.m5stack.com/products/m5stack-tab5-iot-development-kit-esp32-p4)

-   Contains both ESP32-P4 and ESP32-C6 processors
-   Built-in camera and display
-   SDIO communication between processors
-   ESP32-C6 download interface is on the rear PCB (behind the back cover), **not** the external USB-C connector — see the [M5Stack Tab5 C6 flashing guide](https://docs.m5stack.com/en/guide/restore_factory/m5tab5_c6_wifi#2-flashing-tool)

## System Architecture

```
┌─────────────────┐      SDIO Bridge     ┌─────────────────┐
│    ESP32-C6     │◄────────────────────►│    ESP32-P4     │
│ (matter_camera) │      Communication   │ (media_adapter) │
│                 │                      │                 │
│ ┌─────────────┐ │                      │ ┌─────────────┐ │
│ │             │ │                      │ │ H.264       │ │
│ │   Matter    │ │                      │ │ Encoder     │ │
│ │             │ │                      │ │             │ │
│ │  Signaling  │ │                      │ │ Camera      │ │
│ │             │ │                      │ │ Interface   │ │
│ └─────────────┘ │                      │ └─────────────┘ │
└─────────────────┘                      └─────────────────┘
        ▲                                        ▲
        │                                        │
        ▼                                        ▼
   (Signaling)                              Video/Audio
                                             Hardware
```

## Quick Start

### Prerequisites

-   IDF version: v5.5.5
-   One of the boards listed under [Supported Boards](#supported-boards)
-   [ESP-IDF Port of Amazon Kinesis Video Streams WebRTC SDK repository](https://github.com/espressif/esp-port-for-amazon-kvs-sdk)

```
git clone --recursive https://github.com/espressif/esp-port-for-amazon-kvs-sdk
export KVS_SDK_PATH=/path/to/esp-port-for-amazon-kvs-sdk
```
### Build and Flash Instructions
**Note**: This requires **TWO separate firmware flashes** on the same
board (one for the ESP32-C6, one for the ESP32-P4).
#### Step 1: Flash camera example (ESP32-C6)
This handles WebRTC signaling and Matter integration.

#### For ESP32-P4 Function EV Board:
```bash
idf.py set-target esp32c6
idf.py build
idf.py -p [PORT] flash monitor
```
#### For M5Stack Tab5:

```bash
idf.py -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.defaults.esp32c6;sdkconfig.defaults.m5stack_tab5_pair.esp32c6' set-target esp32c6
idf.py build
idf.py -p [PORT] flash monitor
```

*__NOTE__* (M5Stack Tab5):
- The ESP32-C6 download interface is **not** exposed through the Tab5's external USB-C connector — it is located on the rear PCB, accessible after removing the back cover. See the [M5Stack Tab5 C6 flashing guide](https://docs.m5stack.com/en/guide/restore_factory/m5tab5_c6_wifi#2-flashing-tool) for the port location and flashing procedure.

*__NOTE__* (ESP32-P4 Function EV Board):
- ESP32-C6 does not have an onboard UART port. You will need to use [ESP-Prog](https://docs.espressif.com/projects/esp-iot-solution/en/latest/hw-reference/ESP-Prog_guide.html) board or any other JTAG.
- Use following Pin Connections:

| ESP32-C6 (J2/Prog-C6) | ESP-Prog |
|----------|----------|
| IO0      | IO9      |
| TX0      | TXD0     |
| RX0      | RXD0     |
| EN       | EN       |
| GND      | GND      |

#### Step 2: Flash media_adapter (ESP32-P4)

This handles video/audio streaming. The firmware is the `streaming_only` example
from the KVS SDK.

```bash
cd ${KVS_SDK_PATH}/examples/streaming_only
idf.py set-target esp32p4
idf.py menuconfig
# Go to Component config -> ESP System Settings -> Channel for console output
# (X) USB Serial/JTAG Controller # For ESP32-P4 Function_EV_Board V1.2 OR V1.5
# (X) Default: UART0 # For ESP32-P4 Function_EV_Board V1.4
idf.py build
idf.py -p [PORT] flash monitor
```

**Note**: If the console selection is wrong, you will only see the initial
bootloader logs. Please change the console as instructed above and reflash the
app to see the complete logs.

**Note**: Currently, due to flash size limitations of ESP32-C6 onboard the
ESP32-P4 Function EV Board, the `ota_1` partition (see
[`partitions.csv`](partitions.csv)) is disabled and the size of the `ota_0`
partition is increased. This prevents the firmware from performing OTA updates.
Hence, this configuration is not recommended for production use.
