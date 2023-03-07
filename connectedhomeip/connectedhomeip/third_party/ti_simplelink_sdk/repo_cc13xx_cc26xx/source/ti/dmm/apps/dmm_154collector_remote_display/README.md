# DMM 15.4 Collector + BLE Remote Display

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#HardwarePrerequisites)
* [Software Prerequisites](#SoftwarePrerequisites)
* [Dynamic Multi-protocol Manager](#DynamicMultiprotocolManager)
* [Usage](#Usage)
  * [BLE Services Overview](#Services)
  * [Using the Common User Interface](#CUI)
  * [Provisioning the 15.4 Network](#Provisioning154Network)
  * [Provisioned 15.4 Network on the Collector](#Provisioned15.4Collector)
  * [15.4 Network Device Discovery via BLE](#NetworkDeviceDiscovery15.4Collector)
  * [DMM 15.4 Security Manager (SM) Commissioning](#DMM15.4SecurityManager)
  * [Disabling Common User Interface](#DisableCUI)
* [DMM Collector Reboot and Reset](#DMMCollectorRebootAndReset)
* [DMM Limitations](#DmmLimitations)

## <a name="Introduction"></a>Introduction

The dmm_154collector_remote_display project showcases a dynamic multi-protocol example which
enables concurrent proprietary 802.15.4g frequency hopping (WiSUN FAN) and BLE5. This example implements
an IEEE 802.15.4g Wireless Network with a BLE Remote Display, using TI's DMM (Dynamic
Multi-protocol Manager) to enable the multi-protocol and dual band features of
the CC13x2/CC26x2 devices.

The BLE remote display allows a smartphone application to commission the IEEE 802.15.4g
network with the collector and to see connected sensor data on the IEEE
802.15.4g Wireless Network.

This project has the following app configuration available:

|App Build Configuration         | Description                                                       |
|--------------------------------|-------------------------------------------------------------------|
|FlashROM_Release (default)      | All TI-RTOS debug features disabled but application-logging to UART remains enabled. <br> The application uses the config file `ble_release.cfg` and the defines are in `<app name>_FlashROM_Release.opt`.|

All application configurations use the stack library configuration,
FlashROM_Library. This build configuration will yield additional flash
footprint optimizations by the linker since the application and stack can share
contiguous flash pages. This configuration is further discussed in the Stack
Library Configuration section of the  [BLE5-Stack User's
Guide](../../../../../docs/ble5stack/ble_user_guide/ble5stack-users-guide.html) provided in
the SDK.

This document discusses the procedure for using the DMM 15.4 Collector Remote Display
application when the FlashROM_Release configuration is used.

## <a name="HardwarePrerequisites"></a>Hardware Prerequisites

The default Simple Peripheral configuration uses the
[LAUNCHXL-CC1352R1](http://www.ti.com/tool/launchxl-cc1352r1). This hardware
configuration is shown in the below image:

<img src="resource/hardware_setup.jpg" width="300"/>

For custom hardware, see the **Running the SDK on Custom Hardware** section of
the [BLE5-Stack User's
Guide](../../../../../docs/ble5stack/ble_user_guide/ble5stack-users-guide.html).

## <a name="SoftwarePrerequisites"></a>Software Prerequisites

For information on what versions of Code Composer Studio and IAR Embedded
Workbench to use, see the dependencies section of the Release Notes located
in the root folder of the SDK. 

For information on how to import this project into your IDE workspace and
build/run, please refer to the device's Platform section in the [BLE5-Stack
Quick Start
Guide](../../../../../docs/simplelink_mcu_sdk/html/quickstart-guide/ble5-quick-start.html).

>Note: If you are running an OAD example, be sure to first build and flash a BIM project located in `/examples/nortos/<BOARD>/bim`. For more instructions on how to do this, please refer to the [BLE5-Stack
Quick Start
Guide](../../../../../docs/simplelink_mcu_sdk/html/quickstart-guide/ble5-quick-start.html).

## <a name="DynamicMultiprotocolManager"></a>Dynamic Multi-protocol Manager

The DMM uses 2 main components to dynamically arbitrate the RF resource.

- A Policy Manager
- A Scheduler

TI CC1352 devices are capable of dual band operation, the CC1352 Launchpad (TI
development platform) has a dual band antenna which performs well in the
868/916MHz band as well as the 2.4GHz band. Using the DMM on a CC1352 device
enables not only multi protocol, but also dual band. The below diagram shows
how the DMM allows 2 stacks to run on the same device, which was previously
only possible on 2 devices. In the diagram the Stack 1 is shown functioning
along side Stack 2, this could be a BLE stack and a proprietary Sub1G stack
(called Wireless Sensor Network) or a 15.4 Stack. The Stacks used are defined
by the developer.

Single mode setup:

<img src="resource/singleMode.png" width="650"/>

DMM Setup:

<img src="resource/dmm.png" width="800"/>

The DMM makes scheduling decisions based on **Application Level information**,
**Stack Level information**, and the **Global Priority Table (GPT)**.  

The Stack Level information is embedded in each RF command and it includes:
Start Type, Start Time, AllowDelay, Priority, Activity, etc. 

The Global Priority Table (GPT) defines relative priorities of the two stacks
based on the activity and priority of RF commands.  For details of the GPT,
refer to `<SDK_DIR>\source\ti\common\dmm\dmm_priority_ble_154collector.c/h`.

The Application Level information is the user defined information via the policy
table and inludes: Application State Name, Weight, AppliedActivity, Pause, etc;
the policy table can have multiple policies. A policy defines a specific state
of the DMM application where the user wants to apply different priority based on
an action.  A simplified policy table is shown below, it defines the behavior
when Stack 1 is performing an OAD and does not take into account the specific
state of the stack 2 (i.e. any state). It specifies that when Stack 1 is in an
OAD state, the priority of the `BLE_CONNECTION`, in this case, is increased by
25. If any RF scheduling conflicts happen during the OAD, they will be resolved
by taking into account the modified priority defined in the policy.


|Stack 1 State Name   | Stack 2 State Name     |  Weight   | Applied Activity  |  Description
|-------------|----------------|-------------|-------------|---------------------------------
|OAD      | Any            |   25         |   CONNECTION        | Increase the priority of CONNECTION activity of Stack 1 by 25 during OAD operation


The policy used in this example is more complex than the illustration above. For
details of the actual policy used refer to
`<SDK_DIR>\source\ti\common\dmm\policy.h` and `ti_dmm_application_policy.c/h` as
generated by  the SysConfig tool. 

The DMM also supports pausing/resuming stacks based on a policy change. This is
achieved by an application callback, which is called from the DMM policy manager
when a policy change requiring a stack to pause operation is entire. An example
of pausing the Zigbee stack during a BLE_OAD is provided in the DMM Zigbee
Coordinator Switch + BLE OAD example.

>Note: The DMM Collector + BLE OAD example does not currently pause the 15.4
stack during an OAD.

See the DMM chapter in your protocol stack user's guide for more information
about DMM.
## <a name="Usage"></a>Usage

>Note: This example should be used in conjunction with one or more 15.4 Stack
Sensor(s). The 15.4 Sensor(s) must be built with the same 15.4 mode setting, as
configured with the SysConfig tool. 

This application uses the UART peripheral to provide an
interface for the application. 

This document will use PuTTY to serve as the display for the output of the
CC1352 LaunchPad. The LightBlue smartphone application will act
as the BLE Central device that will connect to the Remote Display service in
this example. The LightBlue smartphone application is available on the App Store
and Google Play. Note that any other serial terminal and smartphone BLE
scanner application may be used. The following default parameters are used for the UART
peripheral for display:

|UART Parameter |Default Values |
|---------------|---------------|
|Baud Rate      |115200         |
|Data length    |8 bits         |
|Parity         |None           |
|Stop bits      |1 bit          |
|Flow Control   |None           |

> Note: If you are using a smartphone as the BLE central device and the same
CC1352 LaunchPad was used previously with a different DMM or BLE example then
you may need to forget the device in the smartphone BLE settings, then disable
and enable BLE. Not doing this could result in the smartphone application
displaying incorrect services and characteristics.

Once the 15.4 Collector Remote Display application starts, the output to the
terminal will report its BLE address and the fact that it is advertising which
is enabled by default upon startup, as shown below:

<img src="resource/collector_startup.png"/>

The Sensor should be showing:

<img src="resource/sensor_startup.png"/>

## <a name="Services"></a>BLE Services Overview

This project contains 3 services:

-  Provisioning Service used to configure the device for setting up an IEEE
   802.15.4g network.
-  Remote Display Service used to configure a connected sensor's reporting
   interval, toggle a sensor's LED, or disassociate a sensor from the IEEE
   802.15.4g network.
-  Network Discovery Service used to display the list of devices connected to
   the IEEE 802.15.4g network.

The characteristic UUID is a 128b value, with 16b that are used to specify the
characteristic. The below shows the format of the 128b UUID:

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LOW BYTE OF UUID, HIGH  BYTE OF UUID, 0x00, 0xF0

###  <a name="Provisioning Service/Profile Table"></a>Provisioning Service/Profile Table

The provisioning profile is defined in
`software_stacks/ble_stack/profiles/provisioning_gatt_profile.c` 

The Characteristics and
properties are shown below.

|Characteristic               | UUID | Format          | Properties       |
|-----------------------------|------|-----------------|------------------|
|Network PAN ID               | 1191 | 2 Byte Integer  | Auth Read/Write  |
|Extended Network PAN ID      | 1192 | 8 Byte Integer  | Auth Read/Write  |
|Collector Operating Frequency| 1193 | 1 Byte Integer  | Auth Read/Write  |
|Collector Operating Channels | 1194 | 17 Byte Integer | Auth Read/Write  |
|IEEE FFD address             | 1195 | 8 Byte Integer  | Auth Read/Write  |
|Network Security Key         | 1196 | 16 Byte Integer | Auth Read/Write  |
|Provision Collector          | 1197 | 1 Byte Integer  | Auth Read/Write  |
|Collector Provisioning State | 1198 | 1 Byte Integer  | Auth Read        |

###  <a name="Collector Service/Profile Table"></a>Collector Service/Profile Table

The collector service profile is defined in
`software_stacks/ble_stack/profiles/remote_display_gatt_profile.c` 

The Characteristics and
properties are shown below.

|Characteristic             | UUID | Format          | Properties       |
|---------------------------|------|-----------------|------------------|
|Sensor Address             | 1181 | 2 Byte Integer  | Read/Notify      |
|Sensor Report Interval     | 1182 | 4 Byte Integer  | Write            |
|Toggle LED                 | 1183 | 1 Byte Integer  | Read/Write       |
|Disassociate Sensor        | 1184 | 2 Byte Integer  | Read/Write       |

###  <a name="Network Device Service/Profile Table"></a>Network Device Service/Profile Table

The network device service profile is defined in
`software_stacks/ble_stack/profiles/network_device_gatt_profile.c` 

The Characteristics and
properties are shown below.

|Characteristic             | UUID | Format          | Properties       |
|---------------------------|------|-----------------|------------------|
|Network Device             | 11B1 | 7 Byte Integer  | Write/Notify     |

## <a name="CUI"></a>Using the Common User Interface

The Common User Interface (CUI) controls the access to User Interface
resources. On a LaunchPad device, these resources are made up of Buttons, LEDs
and UART I/O.

Using a UART terminal with the configuration described above, use the arrow
keys to navigate the menu. Upon startup, pressing the *ENTER* key will display
the **HELP** menu, as shown below:

<img src="resource/CUI.png"/>

To demonstrate the use of the CUI, let us change the BLE PHY. First, use the
arrow keys to select the `TI Remote Display` menu. Pressing the *ENTER* key
will take us to the `TI Remote Display` menu. Next, we see the option to
`CONFIGURE` or go `BACK` to the previous menu. Make sure `CONFIGURE` is
selected and hit the *ENTER* key to enter the `CONFIGURE` menu. Here, we have
the option to `SET PHY`. After pressing *ENTER* once more, we can select a
PHY. Chose the 2M PHY. You should see confirmation of this on the UART
display, as shown below.

<img src="resource/BLE_PHY_update.png"/>

> Note: When using the CUI, the buttons on the LaunchPad should not be used 
to navigate the CUI menu. These buttons perform other actions as described 
in the following section.

## <a name="Provisioning154Network"></a>Provisioning the 15.4 Network

Before the user can connect any sensors to the 15.4 network, the network must
be provisioned, followed by opening the network to allow new sensors to join
the network.

The user has 3 choices regarding the provisioning of the 15.4 Network:

- Press the LEFT button to start the 15.4 network on the collector, followed
  by the RIGHT button to open the network
- Use a BLE central device (such as the LightBLue Smartphone Application) to
  configure the provisioning settings and start the collector network.
- Use the Common User Interface (CUI) to configure and start the 15.4 network 

> Note: The UART terminal output will be similar regardless of which method is
used to provision the network and control the connected sensors.

### <a name="Provisioning154NetworkBLE"></a>Provisioning the 15.4 Network via BLE

To provision the device using BLE use LightBlue to scan for the
device, you should see a device with the name of "DMM 15.4 COLLEC RD" (note that
the navigation might be slightly different depending on your mobile
application):

<img src="resource/rd_advertise.png" width="300"/>

If there is more than 1 device called Node Remote Display, the RSSI value under
the strength bars on the left can be used to find which device you are trying
to connect to. Select "DMM 15.4 Collector RD" to connect to the device. When
prompted enter the pairing code "123456".

Once connected, the terminal window will show the address of the connected
device (smartphone):

<img src="resource/collector_connected.png"/>

In LightBlue, you should be able to see various information about the Node
Remote Display device:

<img src="resource/rd_connected.png" width="300"/>

You should see the provisioning service, which is labeled as "UUID:
F0001190-0451-B000-000000000000" (this may be slightly different for other
smartphone applications). Under this service there should be the following
characteristics shown:

<img src="resource/rd_provisioning.png" width="300"/>

The following sections detail the functions and operations of each of the
characteristics.

#### <a name="NetworkPANID"></a>Network PAN ID Characteristic (UUID: 1191)

This characteristic sets PAN ID of the 15.4 network. This is the same as
setting CONFIG_PAN_ID as configured with the SysConfig Tool.

Setting CONFIG_PAN_ID or Node Data Characteristic to 0xFFFF (the default) on
the Collector allows it to join any network, setting it to a specific valeue
forces it to only join a network with that PAN ID.  Setting CONFIG_PAN_ID to
0xFFFF on the Collector allows it to form a network with a random PAN ID,
setting it to a specific value forces it to form a network with that specific
PAN ID.

#### <a name="CollectorOperatingChannels"></a>Collector Operating Channels Characteristic (UUID: 1194)

This characteristic sets the Channel mask that the 15.4 Collector will use to
find a network. This is the same as setting CONFIG_CHANNEL_MASK,
CONFIG_FH_CHANNEL_MASK and FH_ASYNC_CHANNEL_MASK as configured with the
SysConfig Tool.

To successfully discover and join a network the same setting must be used on
the Sensor and Collector.

#### <a name="NetworkSecurityKey"></a>Network Security Key Characteristic (UUID: 1196)

This characteristic sets security key that the 15.4 sensor will use encrypt
and decrypt 15.4 packet data payload. This is the same as setting
KEY_TABLE_DEFAULT_KEY in the Collector  as configured with the SysConfig Tool.

To successfully communicate with connected sensors, the Network Security Key
Characteristic must be set the same as the Collector KEY_TABLE_DEFAULT_KEY as
configured with the SysConfig Tool.

#### <a name="ProvisionCollector"></a>Provision Collector Characteristic (UUID: 1197)

This characteristic is used to start the IEEE 802.15.4g network and open/close
the network to allow sensors to join. These commands are defined in
provisioning_gatt_profile.h.

Writing the value 0xAA to this characteristic will start the IEEE 802.15.4g
network using the provisioning settings.

Writing the value 0x55 to this characteristic will open the IEEE 802.15.4g
network and allow sensors to join the network.

Writing the value 0xCC to this characteristic will close the IEEE 802.15.4g
network and block new sensors from joining the network.

> Note: All the above provisioning characteristics default to those configured
with the SysConfig Tool. The characteristics only need to be written to if the
user requires to change them from the defaults.

> Note: In FH mode, the BLE connection may drop while new sensors associate to
the IEEE 802.15.4g network. The BLE connection can be resumed once the 15.4
Sensor is associated to a 15.4 network, and full concurrent IEEE 802.15.4g
Frequency Hopping and BLE advertisements or connection can be demonstrated.

#### <a name="CollectorProvisioingState"></a>Collector Provisioning State (UUID: 1198)

This characteristic is used to read the Collector's IEEE 802.15.4g network
state. The following states will be observed:

- Collector Provisioning State 0: Waiting to be initialized
- Collector Provisioning State 1: Starting a network
- Collector Provisioning State 2: Restoring a network
- Collector Provisioning State 3: Started a network
- Collector Provisioning State 4: Restored a network
- Collector Provisioning State 5: Network Open
- Collector Provisioning State 6: Network Closed

### <a name="Provisioning154NetworkCUI"></a>Provisioning the 15.4 Network via CUI

To provision the device using the CUI, navigate to the `TI COLLECTOR` menu and
press the *ENTER* key to enter this menu. 

Then, navigate to the `NETWORK ACTIONS` tab and press the *ENTER* key to enter
this submenu. 

Next, navigate to `FORM NWK` and press the *ENTER* key.

Once the network it started, navigate to `OPEN NWK` and press the *ENTER* key
to allow new sensors to join the formed IEEE 802.15.4g network. This will turn
'PermitJoin=On'. After the above actions have been performed, the UART
terminal should look like the following:

<img src="resource/collector_nwk_start_CUI.png"/>

> Note 1: The above screenshot was taken after a BLE connection was
established between the LaunchPad and a smartphone.
 
> Note 2: The screenshots shown in this example do not reflect default network
settings.

Finally, use the CUI on the sensor project to start the association process
from the sensor.

Once connected, the following UART output should be displayed:

<img src="resource/sensor_provisioned.png"/>

> Note: "Joined - 0x0001" will be displayed followed by a Config Rsp **on the
same line**. After that, you should see sensor message updates printed as
displayed above.

For more details regarding the use of CUI for 15.4 network configuration,
refer to the collector project's README.html file, located here: `<SDK
DIR>\examples\rtos\CC1352R1_LAUNCHXL\ti154stack\collector\README.html`


## <a name="Provisioned15.4Collector"></a>Provisioned 15.4 Network on the Collector

The DMM 15.4 Collector example enables the user to control its connected
sensors in the following ways:

- Disassociate a connected sensor from the 15.4 network
- Update a connected sensor's reporting interval
- Toggle a connected sensor's LED

You should see the remote display service, which is labeled as "UUID:
F0001180-0451-4000-B000-000000000000" (this may be slightly different for
other smartphone applications). Under this service there should be the
following characteristics shown:

<img src="resource/rd_remotedisplay.png" width="300"/>

#### <a name="SensorAddress"></a>Sensor Address Characteristic (UUID: 1181)

This characteristic supports read and write properties, and it contains a
2-byte-long value indicating the address of the sensor to control. The user
must enter the address of the sensor it wishes to control before writing to
the other characteristics in this service.

#### <a name="SensorReportIntervalCharacteristic"></a>Sensor Report Interval Characteristic (UUID: 1182)

This characteristic supports write properties, it contains a 4-byte-long value
indicating the sensor report interval in ms. By default this is set to 3s. In
the LightBlue application select the Sensor Report Interval characteristic.
From here the reporting interval can only be written to. When a value is
written, the rate at which node data is updated changes to reflect the new
value.

> Note: In LightBlue you must write to the characteristic in hex, using the
correct number of digits, or the length will be incorrect and the Remote
Display profile will reject the write. To set the report interval to 1s write
a value of 000003E8.

#### <a name="CollectorLEDIdentify"></a>Toggle LED (UUID: 1183)

This characteristic supports read and write properties, it contains a
1-byte-long value used to make the Sub1GHz 802.15.4 Collector blink a
connected sensor's LED. The application is not dependent on this value. A
write to this characteristic of any value will toggle the selected sensor's
LED.

#### <a name="DisassociateSensor"></a>Disassociate Sensor (UUID: 1184)

This characteristic supports read and write properties, and it contains a
2-byte-long value indicating the address of the sensor to disassociate from
the 15.4 network. 

## <a name="NetworkDeviceDiscovery15.4Collector"></a>15.4 Network Device Discovery via BLE

The DMM 15.4 Collector example enables the user to discover the devices
connected to its IEEE 802.15.4g network with the Network Device Service which
is labeled as "UUID: F00011B0-0451-4000-B000-000000000000" (this may be
slightly different for other smartphone applications).

#### <a name="Network Device"></a>Network Device Characteristic (UUID: 11B1)

This characteristic supports write and notify properties, and it contains a
7-byte-long value indicating the address of the sensor to retrieve the latest
data from.

Writing the value 0xFFFF0000000000 to this characteristic will start the IEEE
802.15.4g network discovery process. The network device discovery process will
send a BLE notification for each sensor in the network. If a sensor was
previously in the network but is not responding, these sensors will also be
reported; their device status will be 0 in this case.

The Network Device information in the BLE notification will be sent as follows:

|2 Bytes               | 2 Bytes 		| 1 Byte       | 1 Byte |  1 Byte   	|
|----------------------|----------------|--------------|--------|---------------|
|Device Address        | Parent Address | Sensor Data  |  RSSI  | Device Status |

The user also has the option to check the status of a single device by writing
the address of the sensor it would like to receive a notification from. For
example, to receive a BLE notification from sensor with short address 0x0001,
simply write the value 0x00010000000000 to this characteristic.

<img src="resource/rd_nwkDiscover.png" width="300"/>

The figure above shows notifications being received for 3 sensors connected to
the 15.4 network. 

> In LightBlue, make sure to select the "Listen for notifications" button.

## <a name="DMM15.4SecurityManager"></a>DMM 15.4 Security Manager (SM) Commissioning

If using either a `dmm_154sensor_sm_remote_display_app` or `dmm_154collector_sm_remote_display_app` project, these will support secure commissioning of a SM Sensor to a SM Collector. This DMM project will enable remote control and monitoring of the secure commissioning service via a BLE phone application connected to a DMM device. Once the phone is connected to the DMM device, the user will be able to select the Authentication Mode via the BLE phone application. If the user selects the "Passkey" mode, then the user will be able to enter the passkey via the app. This passkey will be passed to the BLE Remote Display application and then passed to the SM Sensor application. This passkey will then be used to begin commissioning of the Sensor to the Collector network. Throughout the commissioning process status updates will be sent to the BLE phone application.

![DMM SM Commissioning](resource/dmm_sm_diagram.png)

### Using DMM SM Commissioning Example

The `SimpleLink Starter` mobile application is the easiest way to get started using the DMM SM feature. The steps below outline the how to use the app:

1. Open the `SimpleLink Starter` mobile application, scan and connect to the `DMM 15.4 SM Collec`. Once the services load, select the `DMM Provisioning Service`.

2. Click `Form Network` to form a new 15.4 network.

3. Once the network has formed, click `Open Network` to allow devices to connect to the network.

4. Once the DMM SM Collector network has been setup, disconnect the mobile app from the device.

5. Scan and connect the `DMM 15.4 SM Sensor` device. Wait for all the services to load and select the `DMM Provisioning Service`.

    ![SimpleLink Starter DMM Provisioning Service Selection](resource/simplelink_starter_dmm_provisioning_service.png)

6. Once connected, the app will read and display the current SM State in the `TI15.4 Security Manager` section. Tap the `Auth Mode Selection` drop down menu to select one of the three desired authentication modes (Passkey, Default Code, Just Allow). If nothing is selected, the "Default Code" mode will be used during the commissioning process.

    ![SimpleLink Starter SM Auth Mode Selection](resource/simplelink_starter_auth_mode_selection.png)

7. In the `TI DMM Provisioning Interface` section, click the `START PROVISIONING (CONNECT)` button.

    ![SimpleLink Starter Start Commissioning](resource/simplelink_starter_start_provisioning.png)

8. Once the SM Sensor associates to the open SM Collector, the SM Commissioning process will start. If "Default Mode" or "Just Allow" was selected the commissioning process will be automatic and the commissioning state will eventually update to `SM Success` if the commissioning process was successful. If the commissioning process fails, it will be re-attempted two more times. If all attempts fail, the Sensor will decommission from the Collector's network.

    ![SimpleLink Starter Passkey Entry Success](resource/simplelink_starter_sm_success.png)

9. If "Passkey" was selected as the authentication mode, the app will eventually pop-up a module for the user to enter a custom 6-digit passkey.

    ![SimpleLink Starter SM Success](resource/simplelink_starter_passkey_entry.png)

10. After the passkey has been entered on the DMM SM Sensor device, disconnect the mobile app from the DMM SM Sensor and connect to the `DMM 15.4 SM Collec` device.

11. Once the services load, select the `DMM Provisioning Service`.

    ![SimpleLink Starter DMM Provisioning Service Selection](resource/simplelink_starter_dmm_provisioning_service.png)

12. The app will eventually pop-up a module for the user to enter the same custom 6-digit passkey entered on the DMM SM Sensor.

    ![SimpleLink Starter SM Success](resource/simplelink_starter_passkey_entry.png)

13. After the passkey has been entered, the commissioning process will complete and the commissioning state will eventually update to `SM Success` if the commissioning process was successful. If the commissioning process fails, it will be re-attempted two more times. On each re-attempt, the user will have the ability to re-enter a custom passkey on each device. If all attempts fail, the Sensor will decommission from the Collector's network.

    ![SimpleLink Starter Passkey Entry Success](resource/simplelink_starter_sm_success.png)

14. If desired, after commissioning, the user may disconnect from the DMM SM Collector and connect to the DMM SM Sensor via the `SimpleLink Starter` app to view the current SM Commissioning State of the sensor.

By default, the DMM examples can support connecting up to 4 BLE centrals devices at a time. Due to the secure nature of the SM Commissioning Profile, it is important to only allow a single device being able to read/write to the SM Auth Mode and Passkey characteristics at a time. This will limit the ability for an attacker to interfere with the SM commissioning process. This will be accomplished by only allowing the first device that has an authenticated BLE bond and is trying to access an SM characteristic. Once this first device disconnects, the next device that aligns with the same criteria will be allowed to read/write the characteristics.

### SM Commissioning BLE Service

A separate BLE profile `sm_commisioning_gatt_profile` is enabled to control and track all SM related activities. The `sm_commisioning_gatt_profile` has the following characeristics and permissions.

Service UUID: 0xf000baaa-0451-4000-b000-000000000000

Below summarizes the characteristics that comprise the SM Service:

* [SMCOMMISSIONING_PROFILE_STATECHAR_UUID](#SMCOMMISSIONING_PROFILE_STATECHAR_UUID) 
* [SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_UUID](#SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_UUID)
* [SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_UUID](#SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_UUID)

#### <a name="SMCOMMISSIONING_PROFILE_STATECHAR_UUID"></a>SMCOMMISSIONING_PROFILE_STATECHAR_UUID

* **Description**: BLE characteristic used to define state of the sensor as it is commissioning to a SM Collector.
* **UUID**: 0xf000baab-0451-4000-b000-000000000000
* **Len**: 1
* **GATT Properties**: GATT_PROP_READ, GATT_PROP_NOTIFY
* **GATT Val Permissions**: GATT_PERMIT_AUTHEN_READ
* **Valid Values**:

```
0x0000: SM Idle
0x0001: SM Starting
0x0002: SM Request Passkey
0x0003: SM Passkey Timeout
0x0004: SM Commissioning
0x0005: SM Success
0x0006: SM Error
Otherwise: Error
```

The software flowchart below describes the different scenarios for the SMCOMMISSIONING_PROFILE_STATECHAR_UUID to be updated. Updates to the SMCOMMISSIONING_PROFILE_STATECHAR_UUID characteristic are illustrated by the green circle objects.

![DMM SM Commissioning](resource/dmm_sm_states.png)

#### <a name="SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_UUID"></a>SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_UUID

* **Description**: BLE characteristic used to define the Authentication Mode used by the TI154Stack SM commissioning process.
* **UUID**: 0xf000baac-0451-4000-b000-000000000000
* **Len**: 1
* **GATT Properties**: GATT_PROP_READ, GATT_PROP_WRITE
* **GATT Val Permissions**: GATT_PERMIT_AUTHEN_READ, GATT_PERMIT_AUTHEN_WRITE
* **Valid Values**:

```
0x0001: Passkey
0x0002: Default Code
0x0004: Just Allow
```

#### <a name="SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_UUID"></a>SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_UUID

* **Description**: BLE characteristic used to define the Passkey entered by a user during the TI154Stack SM Passkey based commissioning process.
* **UUID**: 0xf000baad-0451-4000-b000-000000000000
* **Len**: 4
* **GATT Properties**: GATT_PROP_WRITE
* **GATT Val Permissions**: GATT_PERMIT_AUTHEN_WRITE
* **Valid Values**: Any

## <a name="DisableCUI"></a>Disabling Common User Interface

The common user interface (CUI) is a UART based interface that allows users to control and receive updates regarding the application. For various reasons, including reducing the memory footprint, the user is able to disable the common user interface (CUI). To disable the CUI, the following variable must be defined in the `dmm_154collector_remote_display_app.opts`:

```
-DCUI_DISABLE
```

> Please Note: particular features that are dependednt on the CUI wil be unavailable when this feature is enabled.


## <a name="DMMCollectorRebootAndReset"></a>DMM Collector Reboot and Reset

By default the DMM 15.4 Collector RD is configured to store network parameters
in Non-Volatile memory and will be allow sensors to rejoin the network on
power cycle or reset. To disable this feature you need to change:

    -DNV_RESTORE

to

    -DxNV_RESTORE

in the
`Tools/dmm_154collector_remote_display_cc13x2r1lp_app_FlashROM_Release.opt`
file.

Once this feature is disabled the collector will not store its connected
sensor information in NV. Connected sensors will not rejoin the network when
it is reset or power cycled. 

By default, when the Collector is reset or power cycled the Provisioning State
will be 2 (Restoring a network), and the UART should print *"Restarted"* once
the collector network has been restarted. This will happen if there is old
network information in the NV.

> By default, the DMM Collector will not restart the 15.4 network on its own
on reset. It must be done manually using the steps above with either the GPIO
buttons, the CUI, or BLE.

To clear the NV, perform a device reset while holding the **RIGHT** button.

> Note: Once booted the **RIGHT** button is used to turn Permit Join on in the
network. Be careful not to hold the Right button down for to long.

## <a name="DmmLimitations"></a>DMM Limitations

The initial implementation of 15.4 + BLE DMM has only been tested with this policy.

The following limitations exist in the DMM implementation:

- Connection intervals lower than 60ms will result in high sub1G packet loss when
BLE is connected.
- When connecting to a central device Sub-1G packet loss may be experienced until
the update parameters are sent 1s after connection. This will be dependent on the
initial connection interval of the BLE central devices.
- Only 1 BLE connection is supported. The number of concurrent BLE connections
  is set in the application .opt file, located in the project workspace under
  the TOOLS folder.
