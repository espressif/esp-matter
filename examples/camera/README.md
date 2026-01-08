# Matter Camera

This example demonstrates Matter camera using a two-chip split architecture,
where signaling and media streaming are separated across two processors
for optimal power efficiency.

## Architecture Overview

The split mode consists of two separate firmware images:

### 1. **matter_camera** (ESP32-C6)

-   **Role**: Matter camera with WebRTC signaling integration
-   **Responsibilities**:
    -   Matter stack execution
    -   WebRTC signaling
    -   Bridge communication with media_adapter
    -   Always-on connectivity for instant responsiveness

### 2. **media_adapter** (ESP32-P4)

-   **Role**: Media streaming device
-   **Implementation**: Uses the `streaming_only` example from
    `${KVS_SDK_PATH}/esp_port/examples/streaming_only`
-   **Responsibilities**:
    -   Video/audio capture and encoding
    -   WebRTC media streaming
    -   Power-optimized operation (sleeps when not streaming)
    -   Receives signaling commands via bridge from matter_camera


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

-   [ESP32-P4 Function EV Board](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html)
-   [ESP-IDF v5.5.1](https://github.com/espressif/esp-idf/releases/tag/v5.5.1)
-   [Amazon Kinesis Video Streams WebRTC SDK repository](https://github.com/awslabs/amazon-kinesis-video-streams-webrtc-sdk-c/tree/beta-reference-esp-port)


**Important**: This requires flashing two separate firmwares on
ESP32-C6 and ESP32-P4 of `ESP32-P4 Function EV Board`

### Setup Camera example (ESP32-C6)

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) to setup esp-idf and esp-matter

Build and flash

```bash
cd esp-matter/examples/camera
idf.py set-target esp32c6
idf.py build
idf.py -p [PORT] flash monitor
```

**NOTE**:
- ESP32-C6 does not have an onboard UART port. You will need to use [ESP-Prog](https://docs.espressif.com/projects/esp-iot-solution/en/latest/hw-reference/ESP-Prog_guide.html) board or any other JTAG.
- Use following Pin Connections:

| ESP32-C6 (J2/Prog-C6) | ESP-Prog |
|----------|----------|
| IO0      | IO9      |
| TX0      | TXD0     |
| RX0      | RXD0     |
| EN       | EN       |
| GND      | GND      |

### Setup Media adapter (ESP32-P4)

Clone and setup the WebRTC SDK

```bash
git clone https://github.com/awslabs/amazon-kinesis-video-streams-webrtc-sdk-c.git
git checkout beta-reference-esp-port
git submodule update --init --depth 1
export KVS_SDK_PATH=/path/to/amazon-kinesis-video-streams-webrtc-sdk-c
```

Build and flash

```bash
cd ${KVS_SDK_PATH}/esp_port/examples/streaming_only
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

### Testing
You can use any Matter based camera controller app to view the video feed. Alternatively, you can also use the [camera controller example](https://github.com/project-chip/connectedhomeip/tree/master/examples/camera-controller) from the connnectedhomeip repository.