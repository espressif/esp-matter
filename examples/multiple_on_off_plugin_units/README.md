# Multiple On Off plugin unit

This example demonstrates the creation of a Multiple On-Off Plugin Unit, where multiple endpoints are mapped to GPIO pins.

## Plugin Manager Configuration
Three default on-off plugin units have been created. You can create similar plugin units.

To update the existing CONFIG_GPIO_PLUG values, follow these steps:

1. Open a terminal.
1. Run the following command to access the configuration menu: 
`idf.py menuconfig`
1. Navigate to the "Plugin manager" menu.
1. Update the GPIO pin number used for the factory reset button and plug output (**Use only available GPIO pins as per the target chip**).

You can update the number of plugin units from same menu.

The following table defines the default GPIO pin numbers for each supported target device.

| IO Function          | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
|----------------------|-------|----------|----------|----------|----------|----------|
| Factory Reset Button | 0     | 9        | 9        | 9        | 9        | 0        |
| Plug 1               | 2     | 2        | 2        | 2        | 2        | 2        |
| Plug 2               | 4     | 4        | 4        | 4        | 4        | 4        |
| Plug 3               | 5     | 5        | 5        | 5        | 5        | 5        |
| Plug 4               | 12    | 6        | 6        | 12       | 12       | 12       |
| Plug 5               | 13    | 7        | 7        | 13       | 13       | 13       |
| Plug 6               | 14    | 8        | 8        | 15       | 14       | 14       |
| Plug 7               | 15    | 10       | 10       | 18       | 22       | 15       |
| Plug 8               | 16    | 18       | 18       | 19       | 25       | 16       |
| Plug 9               | 17    | 0        | 19       | 20       | 26       | 17       |
| Plug 10              | 18    | 1        | 0        | 21       | 27       | 18       |
| Plug 11              | 19    | 3        | 1        | 22       | 0        | 19       |
| Plug 12              | 21    |          | 3        | 23       | 1        | 20       |
| Plug 13              | 22    |          |          | 0        | 3        | 21       |
| Plug 14              | 23    |          |          | 1        | 8        | 35       |
| Plug 15              | 25    |          |          | 3        | 10       | 36       |
| Plug 16              | 26    |          |          | 6        | 11       | 37       |

**Note**: ESP32-C2 and ESP32-C3 do not have enough IO to use all 16 plugs

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
