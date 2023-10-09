# Espressif's SDK for Matter

[![Docker Image](https://github.com/espressif/esp-matter/actions/workflows/docker-image.yml/badge.svg)](https://github.com/espressif/esp-matter/actions/workflows/docker-image.yml)
[![Launchpad Deployment](https://github.com/espressif/esp-matter/actions/workflows/pages.yml/badge.svg)](https://github.com/espressif/esp-matter/actions/workflows/pages.yml)

## Introduction

Espressif's SDK for Matter is the official Matter development framework for ESP32 series SoCs. It is built on top of the [open source Matter SDK](https://github.com/project-chip/connectedhomeip/), and provides simplified APIs, commonly used peripherals, tools and utilities for security, manufacturing and production accompanied by exhaustive documentation. It includes rich production references, aimed to simplify the development process of Matter products and enable the users to go to production in the shortest possible time.

[Supported Device Types](SUPPORTED_DEVICE_TYPES.md)


## Supported Matter specification versions

| Matter Specification Version |                              Supported Branch                             |
|:----------------------------:|:-------------------------------------------------------------------------:|
|             v1.0             | [release/v1.0](https://github.com/espressif/esp-matter/tree/release/v1.0) |
|             v1.1             | [release/v1.1](https://github.com/espressif/esp-matter/tree/release/v1.1) |
|     v1.2 (Ongoing effort)    |         [main](https://github.com/espressif/esp-matter/tree/main)         |


## Getting the repositories

For efficient cloning of the ESP-Matter repository, please refer
[Getting the Repositories](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#getting-the-repositories)
section in the ESP-Matter Programming Guide.

## Supported ESP-IDF and connectedhomeip versions

- This SDK currently works with [5b4f800](https://github.com/project-chip/connectedhomeip/commit/5b4f8004662d00bdb111367fec7d3ea978c23372) of connectedhomeip.
- For Matter projects development with this SDK, it is recommended to utilize ESP-IDF [v5.1.1 release](https://github.com/espressif/esp-idf/releases/tag/v5.1.1).


## Documentation

Refer the [Programming Guide](https://docs.espressif.com/projects/esp-matter/en/latest/) for the latest version of the documentation.


## Matter Specifications
Download the Matter specification from [CSA's official site](https://csa-iot.org/developer-resource/specifications-download-request/)

---

<a href="https://espressif.github.io/esp-launchpad/?flashConfigURL=https://espressif.github.io/esp-matter/launchpad.toml">
    <img alt="Try it with ESP Launchpad" src="https://espressif.github.io/esp-launchpad/assets/try_with_launchpad.png" width="250" height="70">
</a>
