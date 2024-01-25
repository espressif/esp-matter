# Light Switch

This example creates an On/Off Light Switch device using the data model.

It creates the On/Off client and other devices can be bound to the
switch and then controlled from the switch.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

No additional setup is required.

## 2. Post Commissioning Setup

### 2.1 Bind light to switch

Using the chip-tool, commission 2 devices, the switch and a light.
If you are having trouble, try commissioning them one at a time (by powering off the other device) as
the default discriminator and passcode are same for both of them.
Then use the below commands to bind the light to the switch.

For the commands below:

-   Node Id of switch used during commissioning is 0x7283 (29315 in decimal)
-   Node Id of light used during commissioning is 0x5164 (20836 in decimal)
-   Cluster Id for OnOff cluster is 6
-   Binding cluster is currently present on endpoint 1 on the switch

Update the light's acl attribute to add the entry of remote device
(switch) in the access control list:
```
accesscontrol write acl '[{"privilege": 5, "authMode": 2, "subjects": [ 112233, 29315 ], "targets": null}]' 0x5164 0x0
```

Update the switch's binding attribute to add the entry of remote device
(light) in the binding table:
```
    binding write binding '[{"node":20836, "endpoint":1, "cluster":6}]' 0x7283 0x1
```

### 2.2 Bind a group to switch

Using the chip-tool, commission 3 (or more) devices, 1 switch and 2 (or more) lights.
If you are having trouble, try commissioning them one at a time (by powering off the other device) as
the default discriminator and passcode are same for both of them.
Then use the below commands to add the devices to the group and bind the group to the switch.

For the commands below:
-   Node Id of switch used during commissioning is 0x7283 (29315 in decimal)
-   Node Id of light1 used during commissioning is 0x5164 (20836 in decimal)
-   Node Id of light2 used during commissioning is 0x5163 (20835 in decimal)
-   Group Id for the devices is 257 which is assigned by chip-tool when using the testing-group command
-   Binding cluster is currently present on endpoint 1 on the switch

Send the testing-group command to the switch and lights.
This command will write the acl attributes of the nodes and add the endpoint 1 of the nodes to the group 257.
```
    tests TestGroupDemoConfig --nodeId 29315
    tests TestGroupDemoConfig --nodeId 20836
    tests TestGroupDemoConfig --nodeId 20835
```

Update the switch's binding attribute to add the entry of group in the binding table:
```
    binding write binding '[{"group": 257}]' 0x7283 0x1
```

### 2.3 Device console

Switch specific console commands:

-   Send command to the specified device on the specified cluster:
    (The IDs are in hex):
    ```
    matter esp client invoke <fabric_index> <remote_node_id> <remote_endpoint_id> <cluster_id> <command_id> <parameters>
    ```

    -   Example: Off:
        ```
        matter esp client invoke 0x1 0x5164 0x1 0x6 0x0
        ```

    -   Example: On:
        ```
        matter esp client invoke 0x1 0x5164 0x1 0x6 0x1
        ```

    -   Example: Toggle:
        ```
        matter esp client invoke 0x1 0x5164 0x1 0x6 0x2
        ```

    -   Example: Identify 0x78:
        ```
        matter esp client invoke 0x1 0x5164 0x1 0x3 0x78
        ```

-   Send command to the specified group on the specified cluster:
    (The IDs are in hex):
    ```
    matter esp client invoke-group <fabric_index> <group_id> <cluster_id> <command_id> <parameters>
    ```

    -   Example: Off:
        ```
        matter esp client invoke-group 0x1 0x101 0x6 0x0
        ```

    -   Example: On:
        ```
        matter esp client invoke-group 0x1 0x101 0x6 0x1
        ```

    -   Example: Toggle:
        ```
        matter esp client invoke-group 0x1 0x101 0x6 0x2

        ```
    -   Example: Identify 0x78:
        ```
        matter esp client invoke-group 0x1 0x101 0x3 0x78

        ```

-   Send command to all the bound devices on the specified cluster:
    (The IDs are in hex):
    ```
    matter esp bound invoke <local_endpoint_id> <cluster_id> <command_id> <parameters>
    ```

    -   Example: Off:
        ```
        matter esp bound invoke 0x1 0x6 0x0
        ```

    -   Example: On:
        ```
        matter esp bound invoke 0x1 0x6 0x1
        ```

    -   Example: Toggle:
        ```
        matter esp bound invoke 0x1 0x6 0x2
        ```

    -   Example: Identify 0x78:
        ```
        matter esp bound invoke 0x1 0x3 0x78
        ```

-   Send command to all the bound groups on the specified cluster:
    (The IDs are in hex):
    ```
    matter esp bound invoke-group <local_endpoint_id> <cluster_id> <command_id> <parameters>
    ```

    -   Example: Off:
        ```
        matter esp bound invoke-group 0x1 0x6 0x0
        ```

    -   Example: On:
        ```
        matter esp bound invoke-group 0x1 0x6 0x1
        ```

    -   Example: Toggle:
        ```
        matter esp bound invoke-group 0x1 0x6 0x2
        ```

    -   Example: Identify 0x78:
        ```
        matter esp bound invoke-group 0x1 0x3 0x78
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
    [6a244a7](https://github.com/espressif/esp-matter/commit/6a244a7b1e5c70b0aa1bf57254f19718b0755d95)
    (2022-06-16)

|                         | Bootup | After Commissioning |
|:-                       |:-:     |:-:                  |
|**Free Internal Memory** |114KB   |111KB                |

**Flash Usage**: Firmware binary size: 1.25MB

This should give you a good idea about the amount of free memory that is
available for you to run your application's code.

Applications that do not require BLE post commissioning, can disable it using app_ble_disable() once commissioning is complete. It is not done explicitly because of a known issue with esp32c3 and will be fixed with the next IDF release (v4.4.2).

## 4. Dynamic Passcode

If the device features a screen capable of displaying the pairing QR Code, it is advisable to utilize a dynamic passcode for this purpose as the static passcode shall conform to more stringent rules. To enable the use of a dynamic passcode in the example, please ensure that the following configuration options are activated.

```
CONFIG_CUSTOM_COMMISSIONABLE_DATA_PROVIDER=y
CONFIG_DYNAMIC_PASSCODE_COMMISSIONABLE_DATA_PROVIDER=y
```
After implementing these configurations, the device will generate a new, random passcode every time it reboots, if it is not yet commissioned. To obtain the commissioning QR Code, enter `matter onboardingcodes ble qrcode` in the device console, and then initiate the pairing process.
```
./chip-tool pairing code-wifi 1 <ssid> <password> <qrcode>
```

## A2 Appendix FAQs

### A2.1 Binding Failed

My light is not getting bound to my switch:

-   Make sure the light's acl is updated. You can read it again to make
    sure it is correct: `accesscontrol read acl 0x5164 0x0`.
-   If you are still facing issues, reproduce the issue on the default
    example for the device and then raise an [issue](https://github.com/espressif/esp-matter/issues).
    Make sure to share these:
    -   The complete device logs for both the devices taken over UART.
    -   The complete chip-tool logs.
    -   The esp-matter and esp-idf branch you are using.

### A2.2 Command Send Failed

I cannot send commands to the light from my switch:

-   Make sure the binding command was a success.
-   Make sure you are passing the local endpoint_id, and not the remote
    endpoint_id, to the cluster_update() API.
-   If using device console, make sure you are running the
    `bound invoke` command and not the `client invoke` command. The
    client commands are for devices which have not been bound.
-   If you are still facing issues, reproduce the issue on the default
    example for the device and then raise an [issue](https://github.com/espressif/esp-matter/issues).
    Make sure to share these:
    -   The complete device logs for both the devices taken over UART.
    -   The complete chip-tool logs.
    -   The esp-matter and esp-idf branch you are using.
