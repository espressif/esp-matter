# Multiple On Off plugin unit

This example demonstrates the creation of a Multiple On-Off Plugin Unit, where multiple endpoints are mapped to GPIO pins.

## Plugin Manager Configuration
Three default on-off plugin units have been created. You can create similar plugin units.

To update the existing CONFIG_GPIO_PLUG values, follow these steps:

1. Open a terminal.
1. Run the following command to access the configuration menu: 
`idf.py menuconfig`
1. Navigate to the "Plugin manager" menu.
1. Update the GPIO pin number values (**Use only available GPIO pins as per the target chip**).


You can update maximum configurable plugin units from same menu.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

No additional setup is required.

## 2. Post Commissioning Setup

No additional setup is required.

## 3. Device Performance

### 3.1 Memory usage

The following is the Memory and Flash Usage.

-   `Bootup` == Device just finished booting up. Device is not
    commissionined or connected to wifi yet.
-   `After Commissioning` == Device is conneted to wifi and is also
    commissioned and is rebooted.
-   device used: esp32c3_devkit_m
-   tested on:
    [6a244a7](https://github.com/espressif/esp-matter/commit/6a244a7b1e5c70b0aa1bf57254f19718b0755d95)
    (2022-06-16)

|                         | Bootup | After Commissioning |
|:-                       |:-:     |:-:                  |
|**Free Internal Memory** | 212KB   |127KB                |

**Flash Usage**: Firmware binary size: 1.40MB

This should give you a good idea about the amount of free memory that is
available for you to run your application's code.

Applications that do not require BLE post commissioning, can disable it using app_ble_disable() once commissioning is complete. It is not done explicitly because of a known issue with esp32c3 and will be fixed with the next IDF release (v4.4.2).
