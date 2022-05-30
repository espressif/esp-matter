# RainMaker Light

This example creates a Color Dimmable Light device using the ESP
Matter data model.

It also initializes [ESP RainMaker](https://rainmaker.espressif.com/) which enables Device Management and
OTA using the RainMaker cloud. If user node association is done, it also
enables Remote Control through RainMaker.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

### 1.1 Getting the Repositories

```
git clone --recursive https://github.com/espressif/esp-rainmaker.git
```

Setup the RainMaker CLI from here: https://rainmaker.espressif.com/docs/cli-setup.html

### 1.2 Configuring the Environment

```
export ESP_RMAKER_PATH=/path/to/esp-rainmaker
```

### 1.3 RainMaker Claiming

If self-claiming is not enabled/supported, this need to be done before
flashing the firmware.

RainMaker CLI:

```
cd $ESP_RMAKER_PATH/cli
rainmaker.py claim --addr 0x3E0000 $ESPPORT
```

## 2. Post Commissioning Setup

### 2.1 RainMaker User-Node Association

This needs to be done after commissioning.

Check if the device already has user node association done, using the
custom RainMaker cluster (cluster_id: 0x131b0000):

```
chip-tool any read-by-id 0x131b0000 0x0 0x7283 0x0
```

-   If the above custom status attribute (attribute_id: 0x0) returns
    true, the association has already been done.
-   If the attribute returns false, the association has not been done.
    And the below custom configuration command (command_id: 0x0) can be
    used to do the association.

Get the RainMaker node id (attribute_id: 0x1):

```
chip-tool any read-by-id 0x131b0000 0x1 0x7283 0x0
```

RainMaker CLI:

Trigger user-node association using the above rainmaker-node-id: This
will print the user-id and secret-key (do not close this):

```
rainmaker.py test --addnode <rainmaker-node-id>

>> add-user <user-id> <secret-key>
```

Prepare the command payload using the above details:

```
payload: <user-id>::<secret-key>
```

Now use the payload to run the RainMaker configuration command from
chip-tool:

```
chip-tool any command-by-id 0x131b0000 0x0 '"<user-id>::<secret-key>"' 0x7283 0x0
```

The device/node should now be associated with the user.

### 2.2 Device console

RainMaker specific console commands:

-   User Node Association:

    ```
    matter esp rainmaker add-user <user-id> <secret-key>
    ```

## 3. Device Performance

### 3.1 Memory usage

The following is the Memory and Flash Usage.

-   `Bootup` == Device just finished booting up. Device is not
    commissionined or connected to wifi yet.
-   `After Commissioning` == Device is conneted to wifi and is also
    commissioned and is rebooted.
-   device used: esp32c3_devkit_m
-   tested on:
    [bd951b8](https://github.com/espressif/esp-matter/commit/bd951b84993d9d0b5742872be4f51bb6c9ccf15e)
    (2022-05-05)

|                         | Bootup | After Commissioning |
|:-                       |:-:     |:-:                  |
|**Free Internal Memory** |87KB    |65KB                 |

**Flash Usage**: Firmware binary size: 1.52MB

This should give you a good idea about the amount of free memory that is
available for you to run your application's code.

## A2 Appendix FAQs

### A2.1 User Node association is failing

User Node association is failing on my device:

-   Make sure the device has been claimed.
-   If the device prints "This command has reached a limit", reboot the
    device to run the command again.
-   Read the status attribute:
    `chip-tool any read-by-id 0x131b0000 0x0 0x7283 0x0`. If this custom
    status attribute (attribute_id: 0x0) returns true, the association
    has already been done.
-   If you are still facing issues, reproduce the issue on the default
    example for the device and then raise an [issue](https://github.com/espressif/esp-matter/issues).
    Make sure to share these:
    -   The complete device logs taken over UART.
    -   The esp-matter, esp-idf and esp-rainmaker branch you are using.
