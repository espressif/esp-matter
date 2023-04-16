# Generic Switch

This example creates a Generic Switch device using the ESP
Matter data model.
This example aims to demonstrate the use of Fixed Label Cluster and User Label Cluster which provide a feature for the device to tag an endpoint with zero or more read-only labels using nvs api and zero or more labels respectively.

Note:
    In order to retrieve the label-list from the fixed-label cluster the two options:
    ``CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER`` and ``CONFIG_ENABLE_ESP32_DEVICE_INFO_PROVIDER`` have been set through sdkcofig.defaults.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

The steps below should be followed in order to access the fixed-labels.
-   If monitoring the device using ``idf.py monitor``,press `` Ctrl + ]`` to stop the process.
-   The following command must be executed to flash the mfg partition:
```
esptool.py -p [port-name] write_flash 0x10000 mfg_binaries/20202020_3841.bin
```
-   Execute the command ``idf.py monitor``
-   Commission the device with ``discriminator: 20202020``and `` passcode: 3841``
-   Command:
```
chip-tool pairing ble-wifi 0x7283 [ssid] [password] 20202020 3841
```
-   To read the fixed-labels, use chip-tool.
-   Command :
```
chip-tool fixedlabel read label-list 0x7283 1
```

The example command given below should be executed to write to the label-list of User Label Cluster.
```
chip-tool userlabel write label-list '[{"label":"room", "value":"bedroom 1"}, {"label":"orientation", "value":"east"}]' 0x7283 1
```
To read label-list of User Label Cluster execute the command given below.
```
chip-tool userlabel read label-list 0x7283 1
```
## 2. Post Commissioning Setup

This should be followed by: Commission the generic switch device
-   Turn on chip-tool interactive mode.	``./chip-tool interactive start``
-   By default latching switch is enabled so subscribe to switch-latched event via chip-tool.
    ``switch subscribe-event switch-latched <min-interval> <max-interval> <destination-id> <endpoint-id>``
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
