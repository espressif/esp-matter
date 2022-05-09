# Adding External Platforms for Matter

esp-matter provides support for overriding the device layer in Matter. Here are the required steps for adding an external platform.

## Creating the external platform directory

Create a directory `platform/${NEW_PLATFORM_NAME}` in your codebase. You can typically copy `${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/platform/ESP32` as a start. Note that the new platform name shall be different with name `ESP32`. In this article we'll use `ESP32_custom` as an example. The directory must be under `platform` folder to meet the Matter include path conventions.

## Modifying the BUILD.gn target

We've provided an example BUILD.gn file for the `ESP32_custom` example platform. It simply compiles the ESP32 platform in Matter without any modifications.

The new platform directory must be added to the Matter include path. See the `ESP32_custom_include` config in the [BUILD.gn](./BUILD.gn).

Multiple build configs must be exported to the build system. See the `buildconfig_header` section in the [BUILD.gn](./BUILD.gn) for the required definitions.

## Required Kconfigs

The config `CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM` shall be enabled.
The config `CONFIG_CHIP_EXTERNAL_PLATFORM_DIR` shall be the relative path from `${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/config/esp32` to the external platform directory.
For instance, if your source tree is:

```
my_project
├── esp-matter
└── platform
    └── ESP32_custom
```

Then `CONFIG_CHIP_EXTERNAL_PLATFORM_DIR` will be `../../../../../platform/ESP32_custom`.

The config `CONFIG_BUILD_CHIP_TESTS` shall be disabled.

If your external platform does not support the [shell interface](../../connectedhomeip/connectedhomeip/src/lib/shell) provided in the Matter shell library, then `CONFIG_ENABLE_CHIP_SHELL` shall also be disabled.
