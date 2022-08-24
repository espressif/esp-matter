# ESP Matter

## Introduction

ESP Matter is the official Matter development framework for Espressif’s ESP32 series SoCs. It is built on top of the [open source Matter SDK](https://github.com/project-chip/connectedhomeip/), and provides simplified APIs, commonly used peripherals, tools and utilities for security, manufacturing and production accompanied by exhaustive documentation. It includes rich production references, aimed to simplify the development process of Matter products and enable the users to go to production in the shortest possible time.


## Get ESP Matter

Please clone this repository using the below command:

```
git clone --recursive https://github.com/espressif/esp-matter.git
```

> Note the --recursive option. This is required to pull in the various dependencies into esp-matter. In case you have already cloned the repository without this option, execute this to pull in the submodules: `git submodule update --init --recursive`


## Supported ESP-IDF and connectedhomeip versions

- ESP Matter currently works with [commit 68dbb8f](https://github.com/project-chip/connectedhomeip/tree/68dbb8f) of connectedhomeip.
- For Wi-Fi devices (ESP32, ESP32-C3, ESP32-S3), ESP-IDF [v4.4.2 release](https://github.com/espressif/esp-idf/releases/tag/v4.4.2) is required.
- For Thread devices (ESP32-H2) and Zigbee Bridge example, ESP-IDF release/v5.0 branch at [commit ccdeb43](https://github.com/espressif/esp-idf/tree/ccdeb43) should be used.


## Documentation

Refer [ESP Matter Programming Guide](https://docs.espressif.com/projects/esp-matter/en/main/) for the latest version of the documentation.
