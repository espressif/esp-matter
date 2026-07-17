# ESP Matter Camera Examples

This directory contains ESP Matter camera examples that demonstrate building Matter cameras using ESP chipsets with [AWS Kinesis Video Streams](https://docs.aws.amazon.com/kinesisvideostreams-webrtc-dg/latest/devguide/what-is-kvswebrtc.html) (KVS) [WebRTC SDK](https://github.com/espressif/esp-port-for-amazon-kvs-sdk) integration

**Key Features of the ESP Matter Camera:**
- Complete WebRTC stack with STUN and TURN capabilities
- KVS peer connection with media streaming

## Available Examples

### 1. Standalone Mode (`standalone/`)

Complete WebRTC implementation including both signaling and media streaming on a single device.

**Use Case:**
- Single device deployment
- Direct camera streaming with WebRTC
- Faster Peer connection setup

**Supported Devices:**
- ESP32-P4 (FHD live streaming)

[Learn more →](standalone/README.md)

### 2. Split Mode (`split_mode/`)

Split architecture where streaming is handled by a separate MCU and Signaling is handled by the ESP Matter Camera.

**Use Case:**
- Distributed architecture (signaling on one device, streaming on another)
- This enables manufactures to swap streaming implementation swiftly
- Power-optimized deployments

**Supported Devices:**
  - ESP32-P4 Function Ev Board
    - ESP32-C6 (ESP Matter signaling integration)
    - ESP32-P4 (streaming side - uses KVS streaming_only example)

[Learn more →](split_mode/README.md)

## Prerequisites

- **IDF version**: v5.5.4
- **Amazon Kinesis Video Streams WebRTC SDK**: Clone the default branch

  ```bash
  git clone --recursive git@github.com:espressif/esp-port-for-amazon-kvs-sdk.git
  export KVS_SDK_PATH=/path/to/esp-port-for-amazon-kvs-sdk
  ```

## Getting Started

1. Choose the appropriate example based on your use case:
   - Device for complete experience → **Standalone Mode**
   - Distributed setup with power saving feature → **Split Mode**

2. Follow the detailed instructions in the respective example's README:
   - [Standalone Mode README](standalone/README.md)
   - [Split Mode README](split_mode/README.md)

## Architecture

### Standalone Mode

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

### Split Mode

```
┌────────────────────────────────┐      ┌─────────────────────────────┐
│         ESP32-C6               │ SDIO │         ESP32-P4            │
│      (ESP Matter Camera)       │◄────►│     (Streaming Device)      │
│  ┌──────────────────────────┐  │      ├─────────────────────────────┤
│  │   Camera Split Mode      │  │      │                             │
│  ├──────────────────────────┤  │      │   • Media Streaming         │
│  │   • Signaling            │  │      │   • Audio/Video Capture     │
│  │                          │  |      |   • Audio Playback          │
│  └──────────────────────────┘  │      └─────────────────────────────┘
└────────────────────────────────┘
```
