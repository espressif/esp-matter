# Water Heater Optional Attributes Support

## Problem

The standard Water Heater endpoint generator (`create()`) provisions only mandatory attributes:
- `heater_types` (mandatory)
- `heat_demand` (mandatory) 
- `boost_state` (mandatory)

Optional attributes defined in the Matter specification are **not** created:
- `tank_volume` (Energy Management feature)
- `estimated_heat_required` (Energy Management feature)
- `tank_percentage` (Tank Percent feature)

## Impact

Applications trying to read or write optional attributes receive errors:
```
Attribute is not managed by esp matter data model
GET_VAL FAILED! for cluster 148
```

This breaks compliance with the Matter specification for Water Heater device type.

## Solution

Use the extended API `create_extended()` to provision optional features:

### Basic Usage

```cpp
#include <water_heater_device.h>
#include <water_heater_device_extended.h>

using namespace esp_matter;

// Prepare optional feature configs
cluster::water_heater_management::feature::energy_management::config_t energy_config;
energy_config.tank_volume = 100;  // liters
energy_config.estimated_heat_required = 5000000;  // joules

cluster::water_heater_management::feature::tank_percent::config_t tank_config;
tank_config.tank_percentage = 50;  // 50%

// Prepare base water heater config
endpoint::water_heater::config_t base_config;
base_config.descriptor.endpoint_version = 0;
// ... configure other clusters (water_heater_management, water_heater_mode, thermostat)

// Create extended config with optional features
endpoint::water_heater::extended_config_t config;
config.base = base_config;
config.energy_management = &energy_config;
config.tank_percent = &tank_config;

// Create endpoint with optional attributes
endpoint_t *ep = endpoint::water_heater::create_extended(
    node::get(),
    &config,
    ENDPOINT_FLAG_NONE,
    nullptr
);
```

### Selective Optional Features

You can enable only the features you need by setting the others to `nullptr`:

```cpp
endpoint::water_heater::extended_config_t config;
config.base = base_config;
config.energy_management = &energy_config;  // Enable this feature
config.tank_percent = nullptr;                // Disable this feature
```

## Files

- `water_heater_device_extended.h` - Extended API declaration
- `water_heater_device_extended.cpp` - Extended API implementation
- `water_heater_device.h/cpp` - Standard generated code (unchanged)

## Backward Compatibility

The standard `create()` function remains unchanged. Existing code continues to work without modification.

## Related Issues

- arduino-esp32 PR #12945 - MatterWaterHeater endpoint implementation
- arduino-esp32 issue #12948 - Optional attributes not provisioned

## Future Improvements

A complete upstream solution would modify the ZAP generator to:
1. Extend the `config_t` structure to include optional feature configs
2. Conditionally call `feature::*::add()` in the `create()` function
3. Maintain backward compatibility for existing code

This extended API is a pragmatic interim solution.
