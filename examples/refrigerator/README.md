# Refrigerator

This example creates a Refrigerator device using the ESP
Matter data model.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

No additional setup is required.

## 2. Post Commissioning Setup

No additional setup is required.

## 3. TemperatureLevel feature.

Set the config `ESP_MATTER_TEMPERATURE_CONTROL_CLUSTER_ENDPOINT_COUNT` (default 1) to the number of endpoints on which
you are using temperature control cluster with temperature level feature from menuconfig (Component config -> ESP Matter).

Note: By default `SupportedTemperatureLevels` attribute is added for endpoint 1 only. If you want to add for more endpoints,
please add it in `connectedhomeip/connectedhomeip/examples/all-clusters-app/all-clusters-common/src/static-supported-temperature-levels.cpp`
