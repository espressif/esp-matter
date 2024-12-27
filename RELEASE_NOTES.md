# 27-December-2024

Added ``matter esp factoryreset`` command to factory reset a Matter device.
This command erases the esp-matter and connectedhomeip SDK's non-volatile storage.

# 15-Oct-2024

API Change
```
esp_err_t get_bounds(attribute_t *attribute, esp_matter_attr_bounds_t *bounds);
```
- Above API returns the `esp_err_t` and has parameter `bounds`.

# 28-Aug-2024

- Removed the configurability of the NameSupport attribute of the Groups cluster and enabled
  it by default. The NameSupport and feature-map attributes of the Groups cluster must be kept
  in sync, the connectedhomeip SDK enables the GroupName feature by default and set the
  NameSupport attribute to appropriate value.

# 08-July-2024
- Changed diagnostic cluster namespaces as per the matter specifications
  - diagnostics_network_wifi -> wifi_network_diagnotics  
  - diagnostics_network_thread -> thread_network_diagnostics  
  - diagnostics_network_ethernet -> ethernet_network_diagnostics

# 07-May-2024

- `tools/mfg_tool.py` is moved to https://github.com/espressif/esp-matter-tools/tree/main/mfg_tool
  and can be installed by running `python3 -m pip install esp-matter-mfg-tool`.

# 14-February-2024

- An optional argument, `max_val_size`, has been introduced to the `esp_matter::attribute::create()` API.
  This argument is utilized specifically when creating attributes of the char string and long char string data types
  to specify the maximum supported value size of an attribute.

# 29-January-2024

- Add a new parameter for esp_matter::client::connect() to set the CASESessionManager for finding or establishing the CASE sessions.

# 16-January-2024

- We have moved the creation of bridge devices to the application callback.
  Previously, bridge devices were created in the examples/common, but now creation
  is delegated to the application.

    Initialize the bridge with a callback.

    ```
    esp_err_t err = app_bridge_initialize(node, bridge_device_type_callback);
    ```

    Below is the example definition of callback creating on-off light.
    ```
	esp_err_t create_bridge_devices(esp_matter::endpoint_t *ep, uint32_t device_type_id, void *priv_data)
	{
		esp_err_t err = ESP_OK;

		switch (device_type_id) {
		case ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID: {
			on_off_light::config_t on_off_light_conf;
			err = on_off_light::add(ep, &on_off_light_conf);
			break;
        }
		.
		.
		.
		default: {
			ESP_LOGE(TAG, "Unsupported bridged matter device type");
			return ESP_ERR_INVALID_ARG;

		}
		return err;
	}

    ```

- Similar changes are made in bridge apps.

# 9-November-2023

- esp_matter_controller: Change the format of the command data field payload for cluster-invoke commands and the attribute value payload for attribute-write commands. Used unified JSON object format for these payloads. Please refer the document of ``Matter controller`` to learn how to construct them.

# 2-November-2023

All of the non-volatile attribute values now are stored in the namespace `esp_matter_kvs` with the attribute key base64-encoded of bytes (`endpoint-id`+`cluster-id`+`attribute-id`). For the devices that store the non-volatile attribute values in the previous namespace with previous attribute-key, the values will be moved and the previous keys will be erased.

# 29-August-2023

- `ot_storage` partition is no longer required for thread devices as the mechanism for storing data related to the Thread network has been changed in the `openthread` component in ESP-IDF.

# 14-April-2023

#### *Changed how attribute data is stored in NVS*
By default primitive data types are stored as blob and that consumes more space in NVS.
Added a configuration option (`CONFIG_ESP_MATTER_NVS_USE_COMPACT_ATTR_STORAGE`) which uses
the primitive data type specific NVS APIs to store the attribute. This saves space in NVS partition.

Newer developments shall enable option `ESP_MATTER_NVS_USE_COMPACT_ATTR_STORAGE` to save on NVS space.
All the examples have this option enabled.

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
- If not used, then default value will be used for initialization

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

# 25-Sep-2023

API Change

```
esp_err_t add(endpoint_t *endpoint, config_t *config)
```
- Above API returns the esp_err instead of endpoint.

Known Issues
------------
    - BLE memory is not freed if the device has already been commissioned on bootup.
    - Host-based chip-tool shows the following error during commissioning/cluster control, but the functionality works fine:
        CHIP Error 0x00000070: Unsolicited msg with originator bit clear.
