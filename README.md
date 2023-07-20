# Espressif's SDK for Matter

[![Docker Image](https://github.com/espressif/esp-matter/actions/workflows/docker-image.yml/badge.svg)](https://github.com/espressif/esp-matter/actions/workflows/docker-image.yml)
[![Launchpad Deployment](https://github.com/espressif/esp-matter/actions/workflows/pages.yml/badge.svg)](https://github.com/espressif/esp-matter/actions/workflows/pages.yml)

## Introduction

Espressif's SDK for Matter is the official Matter development framework for ESP32 series SoCs. It is built on top of the [open source Matter SDK](https://github.com/project-chip/connectedhomeip/), and provides simplified APIs, commonly used peripherals, tools and utilities for security, manufacturing and production accompanied by exhaustive documentation. It includes rich production references, aimed to simplify the development process of Matter products and enable the users to go to production in the shortest possible time.


[Supported Device Types](SUPPORTED_DEVICE_TYPES.md)


## Supported ESP-IDF and connectedhomeip versions

- This SDK currently works with [08b1366](https://github.com/project-chip/connectedhomeip/commit/08b13661b65f130d3bc7f7234e74e7c64ae2b3f7) of connectedhomeip.
- For ESP32, ESP32-C3, and ESP32-S3, ESP-IDF [v5.0.1 release](https://github.com/espressif/esp-idf/releases/tag/v5.0.1) is required.
- For ESP32-C2, ESP32-H2(preview) and ESP32-C6(preview) and Zigbee Bridge example, ESP-IDF branch at [commit ea5e0ff](https://github.com/espressif/esp-idf/tree/ea5e0ff) in branch release/v5.1 should be used.


## Documentation

Refer the [Programming Guide](https://docs.espressif.com/projects/esp-matter/en/latest/) for the latest version of the documentation.


## Matter Specifications
Download the Matter 1.0 specifications from [CSA's official site](https://csa-iot.org/developer-resource/specifications-download-request/)

---

<a href="https://espressif.github.io/esp-launchpad/?flashConfigURL=https://espressif.github.io/esp-matter/launchpad.toml">
    <img alt="Try it with ESP Launchpad" src="https://espressif.github.io/esp-launchpad/assets/try_with_launchpad.png" width="250" height="70">
</a>
