# Matter Camera

This example creates a Camera device using the ESP Matter data model.

# Standalone Camera Example

In standalone mode, the entire camera application runs on the ESP32-P4: Matter commissioning and signaling, WebRTC media streaming, and audio/video capture are all handled on a single SoC. The ESP32-C6 is used only as a network adapter (Wi-Fi connectivity) over SDIO.

### System Architecture

```
┌────────────────────────────────┐      ┌─────────────────────────────┐
│         ESP32-P4               │ SDIO │         ESP32-C6            │
│                                │◄────►│     (Network Adapter)       │
│  ┌──────────────────────────┐  │      └─────────────────────────────┘
│  │   ESP Matter Camera      │  │
│  │   (Standalone Mode)      │  │
│  ├──────────────────────────┤  │
│  │  • Signaling             │  │
│  │  • Media Streaming       │  │
│  │  • Audio/Video Capture   │  │     
│  └──────────────────────────┘  │
└────────────────────────────────┘
```
## Quick Start

### Prerequisites

-   IDF version: v5.5.4
-   [ESP32-P4 Function EV Board](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html)
-   [ESP-IDF Port of Amazon Kinesis Video Streams WebRTC SDK repository](https://github.com/espressif/esp-port-for-amazon-kvs-sdk)

```bash
    git clone --recursive git@github.com:espressif/esp-port-for-amazon-kvs-sdk.git esp-port-for-amazon-kvs-sdk
    export KVS_SDK_PATH=/path/to/esp-port-for-amazon-kvs-sdk
```
### Build and Flash Instructions

Go to the example directory and follow the steps below:
```bash
    cd esp-matter/examples/camera/standalone
    idf.py set-target esp32p4
    idf.py menuconfig
    # Go to Component config -> ESP System Settings -> Channel for console output
    # (X) USB Serial/JTAG Controller # For ESP32-P4 Function_EV_Board V1.2 OR V1.5
    # (X) Default: UART0 # For ESP32-P4 Function_EV_Board V1.4
```

- If the console selection is wrong, you will only see the initial bootloader logs. Please change the console as instructed above and reflash the app to see the complete logs.

- Build and flash the example
```bash
    idf.py build
    idf.py -p [PORT] flash monitor
```

*__NOTE__*:
- While using P4+C6 setup, please build and flash the network_adapter example from `${KVS_SDK_PATH}/examples/network_adapter` on ESP32-C6.
- ESP32-C6 does not have an onboard UART port. You will need to use [ESP-Prog](https://docs.espressif.com/projects/esp-iot-solution/en/latest/hw-reference/ESP-Prog_guide.html) board or any other JTAG.
- Use following Pin Connections:

| ESP32-C6 (J2/Prog-C6) | ESP-Prog |
|----------|----------|
| IO0      | IO9      |
| TX0      | TXD0     |
| RX0      | RXD0     |
| EN       | EN       |
| GND      | GND      |

```bash
    cd ${KVS_SDK_PATH}/examples/network_adapter
    idf.py set-target esp32c6
    idf.py build
    idf.py -p [ESP32-C6-PORT] flash monitor
```
