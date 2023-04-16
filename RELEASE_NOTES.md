# 7-April-2023

- `tools/mfg_tool/mfg_tool.py` now uses `connectedhomeip/connectedhomeip/src/setup_payload/python/generate_setup_payload.py` instead of previously used compiled `chip-tool` binary executable which additionally depends on `python-stdnum` module.
- Reinstall dependencies to use `mfg_tool` by running the following commands -
```
source $IDF_PATH/export.sh
python3 -m pip install -r tools/mfg_tool/requirements.txt
```

# 15-March-2023

API Change

```
esp_err_t esp_matter::identification::init(uint16_t endpoint_id, uint8_t identify_type,
                                            uint8_t effect_identifier = EMBER_ZCL_IDENTIFY_EFFECT_IDENTIFIER_BLINK,
                                            uint8_t effect_variant = EMBER_ZCL_IDENTIFY_EFFECT_VARIANT_DEFAULT);
```

- Above API now accepts the parameters for initial identification `effect_identifier` and `effect_variant`.
- If not used then default value will be used for initialization

```
typedef esp_err_t (*callback_t)(callback_type_t type, uint16_t endpoint_id, uint8_t effect_id, uint8_t effect_variant,
                                    void *priv_data);
```

- Added additional parameter `effect_variant` in identification callback.

```
esp_matter::start(event_callback_t callback, intptr_t callback_arg = static_cast<intptr_t>(NULL))
```

- This API now accepts a parameter to pass additional data to the event callback. By default, the data is set to NULL in the API.

# 14-June-2022

First Github release of ESP Matter

Features
--------
    - Aligns to Matter Test Event 9 and ESP-IDF v4.4.1.
    - ESP Matter Data Model - Intuitive, simple and dynamic.
    - Utility device types supported:
        Root Node
        OTA Requestor
        OTA Provider
        Bridged Node
    - Application device types supported:
        Lighting
          On/Off Light
          Dimmable Light
          Color Temperature Light
          Extended Color Light
        Smart Plugs/Outlets
          On/Off Plug-in Unit
          Dimmable Plug-in Unit
        Switches
          On/Off Light Switch
          Dimmer Switch
          Color Dimmer Switch
        Sensors
          Contact Sensor
          Occupancy Sensor
          Temperature Sensor
        Closure
          Door Lock
        HVAC
          Thermostat
          Fan
    - Mass manufacturing tool that generates manufacturing NVS partition image and onboarding codes.
    - RainMaker integration.
    - Zigbee to Matter and BLE Mesh to Matter Bridge.

Known Issues
------------
    - BLE memory is not freed if the device has already been commissioned on bootup.
    - Host-based chip-tool shows the following error during commissioning/cluster control, but the functionality works fine:
        CHIP Error 0x00000070: Unsolicited msg with originator bit clear.
