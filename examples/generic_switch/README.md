# Generic Switch

This example creates a Generic Switch device using the ESP
Matter data model.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/main/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

No additional setup is required.

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
