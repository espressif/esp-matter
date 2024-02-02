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
|             v1.2             | [release/v1.2](https://github.com/espressif/esp-matter/tree/release/v1.2)         |


## Getting the repositories

For efficient cloning of the ESP-Matter repository, please refer
[Getting the Repositories](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#getting-the-repositories)
section in the ESP-Matter Programming Guide.

## Supported ESP-IDF and connectedhomeip versions

- This SDK currently works with [v1.2.0.1 tag](https://github.com/project-chip/connectedhomeip/tree/v1.2.0.1) of connectedhomeip.With [9ec9ce2](https://github.com/espressif/connectedhomeip/tree/9ec9ce24a7fa33de711dfc7bd26a969000a9b672) and [aceeac7](https://github.com/espressif/connectedhomeip/tree/aceeac7c231ecb32cfd60e65d87d1b7a013d60b0) as fixes.
- For Matter projects development with this SDK, it is recommended to utilize ESP-IDF [v5.1.2](https://github.com/espressif/esp-idf/tree/v5.1.2).


## Documentation

Refer the [Programming Guide](https://docs.espressif.com/projects/esp-matter/en/latest/) for the latest version of the documentation.


## Matter Specifications
Download the Matter specification from [CSA's official site](https://csa-iot.org/developer-resource/specifications-download-request/)

---

<a href="https://espressif.github.io/esp-launchpad/?flashConfigURL=https://espressif.github.io/esp-matter/launchpad.toml">
    <img alt="Try it with ESP Launchpad" src="https://espressif.github.io/esp-launchpad/assets/try_with_launchpad.png" width="250" height="70">
</a>
