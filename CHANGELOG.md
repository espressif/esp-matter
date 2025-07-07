# Changelog for esp_matter component registry

## [1.4.0~1](https://components.espressif.com/components/espressif/esp_matter/versions/1.4.0~1)

#### Bug Fixes

- fix compiling error for `to_underlying` when using c++23
- `NDEBUG` flag will be added if `CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE` is set.

#### Features

- Use v1.4.0~N based on Matter v1.4.0, remove component v1.4.1
- esp-matter commit: bea837ee556c82571537791c774d2e2aa219fa33
- connectedhomeip commit: 87cf8e5030284e3357cfdff5e0b83eb9d5ff73c9

## [1.4.0](https://components.espressif.com/components/espressif/esp_matter/versions/1.4.0)

#### Features

- Component based on Matter 1.4
- esp-matter commit: 30af618a6e962623a0098ad6a33b468f33dc49c7
- connectedhomeip commit: cd07c4f5c3988258e6eba9d34d3dc0767d95e5a3

## [1.3.1](https://components.espressif.com/components/espressif/esp_matter/versions/1.3.1)

#### Features

- Work with IDF component management (version >= 2.0) without hash problem
- External platform path setting: support both absolute path or relative path to top-level CMakeLists
- More default compile options for the source files
- Could enable CONFIG_CHIP_OTA_IMAGE_BUILD to generate ota.bin file with IDF component management (version >= 2.0)
- esp-matter commit: 5a9bc09a640d5cfd4c6b137f7e3b13ce5fa9cca4
- connectedhomeip commit: 735b69f73e386cb675242371afd0b237a8a78982

## [1.3.0](https://components.espressif.com/components/espressif/esp_matter/versions/1.3.0)

#### Features

- Component `esp_matter` based on branch release/v1.3 [commit](https://github.com/espressif/esp-matter/commit/d76d4857d768b32eb215e825448d938018e5691a), commit date Jun 11, 2024.
