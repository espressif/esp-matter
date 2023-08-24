# All Device Types App

This is a All Device Types App using the ESP Matter data model. It covers all the device types that ESP
MATTER data model implements. It has been tested with multiple ecosystem Apple, Google, Alexa.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

No additional setup is required.

## 2. Usage

On boot-up esp-idf console starts. In order to create a device user have to use console command.

-   Use `create --device_type` to list all supported device types.
-   Use `create --device_type=on_off_light` to create light device.
-   To delete existing device perform `matter device factoryreset`.
-   To add new device, say fan or any other device type use `create --device_type=fan`.

You can simulate changing an attribute value locally using the command

-   Command to set attribute value: `matter esp attribute set <endpoint_id> <cluster_id> <attribute_id> <value>`
-   Command to get attribute value: `matter esp attribute get <endpoint_id> <cluster_id> <attribute_id>`


Refer examples/all_device_types_app/main/device_types.h for supported device types.

## 3. Post Commissioning Setup

No additional setup is required.
