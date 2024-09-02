# Generic Switch

This example creates a Generic Switch device using the ESP
Matter data model.
This example demonstrates the use of few optional data model elements like :
- Fixed Label Cluster : provides a feature for the device to tag an endpoint with zero or more read only labels (demonstrated through nvs).
- User Label Cluster : This cluster provides a feature to tag an endpoint with zero or more labels.
- Taglist Feature of Descriptor Cluster : used to disambiguate sibling endpoints where two or more sibling
  endpoints have an overlap in the supported device types with each such endpoint having a unique TagList.


Note:
    In order to retrieve the label-list from the fixed-label cluster the two options:
    ``CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER`` and ``CONFIG_ENABLE_ESP32_DEVICE_INFO_PROVIDER`` have been set through sdkconfig.defaults.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

### 1.1 Building the firmware

There are two types of switches in the example which can be configured
through menuconfig:

- idf.py menuconfig -> Demo -> Generic Switch Type
- To use latching switch, enable `GENERIC_SWITCH_TYPE_LATCHING`.
- To use a momentary switch, enable `GENERIC_SWITCH_TYPE_MOMENTARY`.
- By default momentary switch i.e `GENERIC_SWITCH_TYPE_MOMENTARY` is enabled.

### 1.2 Enabling insights (Optional)

Follow the steps mentioned [here](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/insights.html)

### 1.3 Flash the factory partition

The steps below should be followed in order to access the fixed-labels.
-   If monitoring the device using ``idf.py monitor``,press `` Ctrl + ]`` to stop the process.
-   The following command must be executed to flash the mfg partition:

```
esptool.py -p [port-name] write_flash 0x10000 mfg_binaries/20202020_3841.bin
```

-   Execute the command ``idf.py monitor``

## 2.Commissioning and Control
-   Commission the device with ``discriminator: 3841``and `` passcode: 20202020``

    ```
    chip-tool pairing ble-wifi 0x7283 [ssid] [password] 20202020 3841
    ```

- Alternatively, below QR Code or Manual pairing code can be used for commissioning
    - Manualcode : 34970012334
    - QRCode     :
    - ![QRCode](mfg_binaries/matter_qrcode_20202020_3841.png)

### 2.1 Fixed-Labels
- To read the fixed-labels, use chip-tool.

    ```
    chip-tool fixedlabel read label-list 0x7283 1
    ```

### 2.2 User-Labels
- The example command given below should be executed to write to the label-list of User Label Cluster.

```
chip-tool userlabel write label-list '[{"label":"room", "value":"bedroom 1"}, {"label":"orientation", "value":"east"}]' 0x7283 1
```

- To read label-list of User Label Cluster execute the command given below.

```
chip-tool userlabel read label-list 0x7283 1
```

### 2.3 Using the TagList Feature

To read the taglist of the Descriptor cluster execute the command given below.

```
chip-tool descriptor read tag-list 0x7283 1
```

## 2. Post Commissioning Setup

This should be followed by: Commission the generic switch device
-   Turn on chip-tool interactive mode.	``./chip-tool interactive start``
-   By default momentary switch is enabled so subscribe to long-press event via chip-tool.
    ``switch subscribe-event long-press <min-interval> <max-interval> <destination-id> <endpoint-id>``
-   `Double press the boot button` on device so that client will receive event after max-interval.

### 2.1 Latching switch

Following are latching switch events mapped with boot button on device.

-   `Double Press` -----------> `switch-latched`

### 2.2 Momentary switch

Following are momentary switch events mapped with boot button on device.

-   `Button Press Down` 		    -----------> `initial-pressed`
-   `Button Press Up ( Release )`	    -----------> `short-release`
-   `Button Long Press ( 5 sec )` 	    -----------> `long-pressed`
-   `Button Press Up ( Long Release )`  -----------> `long-release`
-   `Button Press Repeat` 		    -----------> `multipress-ongoing`
-   `Button Press Repeat Done` 	    -----------> `multipress-completed`

## 3. Device Performance

### 3.1 Memory usage

The following is the Memory and Flash Usage.

-   `Bootup` == Device just finished booting up. Device is not
    commissionined or connected to wifi yet.
-   `After Commissioning` == Device is conneted to wifi and is also
    commissioned and is rebooted.
-   device used: esp32c3_devkit_m
-   tested on:
    [4881109](https://github.com/espressif/esp-matter/commit/4881109ce26c92e547ca11d6f022d2c9f908834e)
    (2022-12-15)

|                         | Bootup | After Commissioning |
|:-                       |:-:     |:-:                  |
|**Free Internal Memory** |94KB   |110KB                |

**Flash Usage**: Firmware binary size: 1.24MB

This should give you a good idea about the amount of free memory that is
available for you to run your application's code.
