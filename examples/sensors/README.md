# Matter Sensors

This example demonstrates the integration of temperature and humidity sensors (SHTC3)
and an occupancy sensor (PIR). 

This application creates the temperature sensor, humidity sensor, and occupancy sensor
on endpoint 1, 2, and 3 respectively.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html)
for more information about building and flashing the firmware.

## Connecting the sensors

- Connecting the SHTC3, temperature and humidity sensor

| ESP32-C3 Pin | SHTC3 Pin |
|--------------|-----------|
| GND          | GND       |
| 3V3          | VCC       |
| GPIO 4       | SDA       |
| GPIO 5       | SCL       |

- Connecting the PIR sensor

| ESP32-C3 Pin | PIR Pin |
|--------------|---------|
| GND          | GND     |
| 3V3          | VCC     |
| GPIO 7       | Output  |

**_NOTE:_**:
- Above mentioned wiring connection is configured by default in the example.
- Ensure that the GPIO pins used for the sensors are correctly configured through menuconfig.
- Modify the configuration parameters as needed for your specific hardware setup.

## Usage

- Commission the app using Matter controller and read the attributes.

Below, we are using chip-tool to commission and subscribe the sensor attributes.
- 
```
# Commission
chip-tool pairing ble-wifi 1 (SSID) (PASSPHRASE) 20202021 3840

# Start chip-tool in interactive mode
chip-tool interactive start

# Subscribe to attributes
> temperaturemeasurement subscribe measured-value 3 10 1 1
> relativehumiditymeasurement subscribe measured-value 3 10 1 2
> occupancysensing subscribe occupancy 3 10 1 3
```

## 🛠️ Troubleshooting

If you encounter the following runtime error:

`
i2c: CONFLICT! driver_ng is not allowed to be used with this old driver
`

This error occurs due to a conflict between the legacy I2C driver and the newer driver model (`driver_ng`).

### ✅ Solution

Enable the following option via `idf.py menuconfig`:

`CONFIG_I2C_SKIP_LEGACY_CONFLICT_CHECK=y`


**Important**: This option is only available in the **latest ESP-IDF release branches**:

Pull the latest code from release branches.

- `release/v5.2`
- `release/v5.3`
- `release/v5.4`
- `release/v5.5`
- `master`

- If you're using an older ESP-IDF version, you can apply this [commit as a patch](https://github.com/espressif/esp-idf/commit/466328cd7e4c90c749a406d2bcee73f782ac0016) to add support manually.
