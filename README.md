# Espressif's SDK for Matter

[![Docker Image](https://github.com/espressif/esp-matter/actions/workflows/docker-image.yml/badge.svg)](https://github.com/espressif/esp-matter/actions/workflows/docker-image.yml)
[![Launchpad Deployment](https://github.com/espressif/esp-matter/actions/workflows/pages.yml/badge.svg)](https://github.com/espressif/esp-matter/actions/workflows/pages.yml)

## Introduction

Espressif's SDK for Matter is the official Matter development framework for ESP32 series SoCs. It is built on top of the [open source Matter SDK](https://github.com/project-chip/connectedhomeip/), and provides simplified APIs, commonly used peripherals, tools and utilities for security, manufacturing and production accompanied by exhaustive documentation. It includes rich production references, aimed to simplify the development process of Matter products and enable the users to go to production in the shortest possible time.


## Get the repository

Please clone this repository using the below command:

```
git clone --recursive https://github.com/espressif/esp-matter.git
```

> Note the --recursive option. This is required to pull in the various dependencies into esp-matter. In case you have already cloned the repository without this option, execute this to pull in the submodules: `git submodule update --init --recursive`


## Supported ESP-IDF and connectedhomeip versions

- This SDK currently works with [tag v1.0.0.2](https://github.com/espressif/connectedhomeip/tree/v1.0.0.2) of connectedhomeip.
- For Wi-Fi devices (ESP32, ESP32-C3, ESP32-S3), ESP-IDF [v4.4.3 release](https://github.com/espressif/esp-idf/releases/tag/v4.4.3) is required.
- For Thread devices (ESP32-H2) and Zigbee Bridge example, ESP-IDF release/v5.0 branch at [commit 20949d44](https://github.com/espressif/esp-idf/tree/20949d44) should be used.


## Documentation

Refer the [Programming Guide](https://docs.espressif.com/projects/esp-matter/en/main/) for the latest version of the documentation.


## Matter Specifications
Download the Matter 1.0 specifications from [CSA's official site](https://csa-iot.org/developer-resource/specifications-download-request/)

---

<a href="https://espressif.github.io/esp-launchpad/?flashConfigURL=https://espressif.github.io/esp-matter/launchpad.toml">
    <img alt="Try it with ESP Launchpad" src="https://espressif.github.io/esp-launchpad/assets/try_with_launchpad.png" width="250" height="70">
</a>
                                
