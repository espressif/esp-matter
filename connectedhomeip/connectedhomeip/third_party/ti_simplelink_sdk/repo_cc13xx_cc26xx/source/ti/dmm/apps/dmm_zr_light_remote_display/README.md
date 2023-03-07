# DMM Zigbee Router + BLE Remote Display

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#HardwarePrerequisites)
* [Software Prerequisites](#SoftwarePrerequisites)
* [Dynamic Multi-protocol Manager](#DynamicMultiprotocolManager)
* [Usage](#Usage)
	* [BLE Services Overview](#Services)
	* [Using the Common User Interface](#CUI)
	* [Provisioning The Zigbee Router To A Network](#ProvisioningZigbeeSwToNetwork)
	* [Provisioned Zigbee Router](#ProvisionedZrLight)
  * [Disabling Common User Interface](#DisableCUI)

## <a name="Introduction"></a>Introduction

The dmm_zr_light_remote_display project showcases a dynamic multi-protocol example which
enables concurrent zigbee router device and BLE5. This example implements
a Zigbee Router with a BLE Remote Display, using TI's DMM (Dynamic
Multi-protocol Manager) to enable the multi-protocol features of
the CC13x2 and CC26x2 devices.

The BLE remote display allows a SmartPhone App to commission the Zigbee Router to a Zigbee network and
act as a Light device that acts as a *Target* of Finding and Binding

This project has the following app configurations available:

|App Build Configuration         | Description                                                       |
|--------------------------------|-------------------------------------------------------------------|
|Release (default)      | All TI-RTOS debug features disabled but application-logging to UART remains enabled. <br> The application uses the config file `ble_release.cfg` and the defines are in `<app name>_FlashROM_Release.opt`.|

All application configurations use the stack library configuration,
FlashROM_Library. This build configuration will yield additional flash
footprint optimizations by the linker since the application and stack can share
contiguous flash pages. This configuration is further discussed in the Stack
Library Configuration section of the [BLE5-Stack User's
Guide](../../../../../docs/ble5stack/ble_user_guide/ble5stack-users-guide.html) provided in
the SDK.

This document discusses the procedure for using the DMM Zigbee Light Remote Display
application.

## <a name="HardwarePrerequisites"></a>Hardware Prerequisites

The default Simple Peripheral configuration uses the
[LAUNCHXL-CC1352R](http://www.ti.com/tool/launchxl-cc1352r1) or the [LAUNCHXL-CC26X2R](http://www.ti.com/tool/LAUNCHXL-CC26X2R1).

 >Note: The SDK requires CC13x2/CC26x2 PG2.1 Silicon

The hardware configuration is shown in the below image:

<img src="resource/hardware_setup.jpg" width="300"/>

For custom hardware, see the Running the SDK on Custom Boards section of the
[BLE5-Stack User's
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
868/916MHz band as well as the 2.4GHz band. Using the DMM on CC1352 devices
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
refer to `<SDK_DIR>\source\ti\common\dmm\dmm_priority_ble_zigbee_zr.c/h`.

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

See the DMM chapter in your protocol stack user's guide for more information
about DMM.

## <a name="Usage"></a>Usage

This example should be used in conjunction with the Zigbee Light Coordinator.

This application uses the UART peripheral to provide a user
interface for the application.

This document will use Putty to serve as the display for the output of the
CC1352 LaunchPad. The LightBlue smartphone application will act
as the BLE Central device that will connect to the Remote Display service in
this example. The LightBLue smartphone application is available on App Store
and Google Play. Note that any other serial terminal and smart phone BLE
scanner application can be used. The following default parameters are used for the UART
peripheral for display:

  |UART Param     |Default Values |
  |-------------- |---------------|
  |Baud Rate      |115200         |
  |Data length    |8 bits         |
  |Parity         |None           |
  |Stop bits      |1 bit          |
  |Flow Control   |None           |

> Note: If you are using a smartphone as the BLE central device and the same
CC1352 LaunchPad was used previously with a different DMM or BLE example then
you may need to forget the device in the smartphone BLE settings, then disable
and enable BLE. Not doing this could result in the smartphone application
displaying incorrect services and characteristics

Once the Zigbee Light Remote Display application and the Zigbee Light Coordinator starts, the UI will wait for user input. The output to the terminal will also report its BLE address and the fact that it is advertising which is enabled by default upon startup, as shown below:

<img src="resource/zrlight_startupscreen.png"/>

## <a name="Services"></a>BLE Services Overview

This project contains 2 service:
-  Provisioning Service used to configure the device for connecting to a Zigbee network.
-  Light Service used to control a secondary Light on the Zigbee Network

The characteristic UUID is a 128b, with 16b that are used to specify the characteristic. The below shows the format of the 128b UUID:

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LOW BYTE OF UUID, HIGH  BYTE OF UUID, 0x00, 0xF0

###  <a name="Provisioning Service/Profile Table"></a>Provisioning Service/Profile Table

The provisioning profile is defined in `software_stacks/ble_stack/profiles/provisioning_gatt_profile.c` The Characteristics and properties are shown below.

|Characteristic             | UUID | Format          | Properties       |
|---------------------------|------|-----------------|------------------|
|Network PAN ID             | 1191 | 2 Byte Integer  | Auth Read/Write  |
|Network Channel Mask       | 1194 | 17 Byte Integer | Auth Read/Write  |
|Start Network Provisioning | 1197 | 1 Byte Integer  | Auth Read/Write  |
|Network Provisioning State | 1198 | 1 Byte Integer  | Auth Read        |

###  <a name="Light Switch Service/Profile Table"></a>Light Switch Service/Profile Table

The light profile is defined in `software_stacks/ble_stack/profiles/light_gatt_profile.c` The Characteristics and properties are shown below.

|Characteristic             | UUID | Format          | Properties       |
|---------------------------|------|-----------------|------------------|
|Light On/Off State         | 11A1 | 1 Byte Integer  | Read/Notify      |
|Target Address Type        | 11A2 | 1 Byte Integer  | Read/Write       |
|Target Address             | 11A3 | 2 Byte Integer  | Read/Write       |
|Battery Level              | 11A4 | 1 Byte Integer  | Read             |

## <a name="CUI"></a>Using the Common User Interface

>Note: The screenshots in this section were taken from the DMM ZED switch + BLE project. There will be slight differences in the UART terminal depending on which project you are working with.

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


## <a name="ProvisioningZigbeeSwToNetwork"></a>Provisioning the Zigbee Light to the Network

The user has 3 choices regarding the provisioning the device to a Zigbee Network:

- Use the LEFT button to make the Zigbee Router Light search for and join a network with default settings, as configured with the  SysConfig tool.
- Use a BLE central device (such as the LightBLue Smartphone Application) to configure the provisioning settings and make the Zigbee Light search for and join a network with those settings.
- Use the Common User Interface to configure and commission the device

> Note: Before the Zigbee Light can join the Zigbee Network the the network must be opened on the coordinator.

### <a name="ProvisioningZigbeeDeviceviaBLE"></a>Provisioning the Zigbee Device via BLE

To provision the device using BLE use LightBlue to scan for the
device, you should see a device with the name of "DMM Zigbee Light RD" (note that
the navigation might be slightly different depending on your mobile
application):

<img src="resource/zrlight_advertisement.png" width="300"/>

If there is more than 1 device called DMM Zigbee Light RD, the RSSI value under
the strength bars on the left can be used to find which device you are trying
to connect to. Press the "connect" button to connect to the device. When
prompted enter the pairing code "123456".

In LightBlue, you should be able to see various information about the Zigbee Light device:

<img src="resource/zrlight_connected.png" width="300"/>

You should see the provisioning service, which is labeled as "UUID: F0001190-0451-B000-000000000000" (this may be
slightly different for other smartphone applications). Under this service there should be the following characteristics shown:

<img src="resource/zrlight_prov_service.png" width="300"/>

The following sections detail the functions and operations of each of the characteristics.


#### <a name="NetworkPANID"></a>Network PAN ID Characteristic (UUID: 1191)

This characteristic sets PAN ID that the Zigbee Light will join. This is the same as setting PAN_ID in the UI configuration menu.

Setting PAN_ID in the UI or Network PAN ID Characteristic to 0xFFFF (the default) on the Zigbee Light allows it to join any network, setting it to a specific valeue forces it to only join a network with that PAN ID.  Setting PAN_ID in UI to 0xFFFF on the Coordinator allows it to form a network with a random PAN ID, setting it to a specific value forces the Zigbee Light or Coordinator to join or form a network with that specific PAN ID.

#### <a name="ZrlightOperatingChannels"></a>Zigbee Light Operating Channels Characteristic (UUID: 1194)

This characteristic sets the Channel Mask that the Zigbee Light will use to find a network. This is the same as setting Primary Channel in the UI.

To successfully discover and join a network there must be an open network on one of the channels in the mask.

>Note: The operating channels attribute when read, will return the previously provisioned channel mask and will update once provisioning is attempted.

#### <a name="ProvisionZrlight"></a>Provision Zigbee Light Characteristic (UUID: 1197)

This characteristic is used to make the Zigbee Light search for and join a network using the provisioning characteristics. The value of 0xAA must be written to this characteristic to start the Joining processes.

>Note: All the above provisioning characteristics default to those set with the SysConfig tool. The characteristics only need to e written to if the user requires to change them from the defaults.

>Note: While the Zigbee Light is scanning for a network the BLE connection may dropped. The BLE connection can be resumed once the Zigbee Light is associated to a Zigbee network, and fully concurrent 802.15.4 and BLE advertisements or connection can be demonstrated.

#### <a name="ZrLightProvisioingState"></a>Zigbee Light Provisioning State (UUID: 1198)

This characteristic is used to read the Zigbee Lights network state. The following states will be observed:

- Zigbee Light Provisioning State 0: Waiting to be Initialized
- Zigbee Light Provisioning State 1: Device Initialized
- Zigbee Light Provisioning State 2: Discovering PAN's to Join
- Zigbee Light Provisioning State 3: Joining Network
- Zigbee Light Provisioning State 4: Re-Joining Network
- Zigbee Light Provisioning State 5: Joined but not Authenticated
- Zigbee Light Provisioning State 6: Joined Network as End Device
- Zigbee Light Provisioning State 7: Joined Network as Router
- Zigbee Light Provisioning State 8: Starting as Zigbee Coordinator
- Zigbee Light Provisioning State 9: Started as Zigbee Coordinator
- Zigbee Light Provisioning State 10: Orphaned from Network
- Zigbee Light Provisioning State 11: Sending KeepAlive Message
- Zigbee Light Provisioning State 12: Device waiting before trying to rejoin network
- Zigbee Light Provisioning State 13: Re-Joining Network in Secure Mode Scanning All Channels
- Zigbee Light Provisioning State 14: Re-Joining Network in Trust Center Mode Scanning Current Channel
- Zigbee Light Provisioning State 15: Re-Joining Network in Trust Center Mode Scanning All Channels

### <a name="ProvisioningZigbeeDeviceCUI"></a>Provisioning the Zigbee Device via CUI

To provision the device using the CUI, navigate to the `TI Sample Light` menu and
press the *ENTER* key to enter this menu.

Then, navigate to the `COMMISSION` tab and press the *ENTER* key.

The ZED switch will now search for and join a network with the default settings as configured with the SysConfig
tool. In the **ZDO Info** line, the state should change to **Initialized** as shown below.

<img src="resource/zrlight_commission_CUI.png"/>

Once connected, the following UART output should be displayed:

<img src="resource/zrlight_joined_CUI.png"/>

For more details regarding the use of CUI for Zigbee End Device Switch,
refer to the project's README.html file, located here: `<SDK
DIR>\examples\rtos\CC1352R1_LAUNCHXL\zstack\zr_light\README.html>`

## <a name="ProvisionedZrLight"></a>Provisioned Zigbee Light

Once provisioned (commissioned in Zigbee terminology) the ZR Light can route traffic on the Zigbee network. If the binding was successful during commissioning then the light's Light Service will control the Light on/off state of all the Lights in the binding.

You should see the light's Light service, which is labeled as "UUID: F00011A0-0451-B000-000000000000" (this may be
slightly different for other smartphone applications). Under this service there should be the following characteristics shown:

<img src="resource/zrlight_lightonoff_service.png" width="300"/>

#### <a name="Light On/Off"></a>Light Light On/Off Characteristic (UUID: 11A1)

This characteristic supports  read and write properties, and it contains
a 1-byte-long value indicating the on/off state. Writing a 1 to this will send an Zigbee ZCL On message. Writing 0 will send a Zigbee ZCL Off message

<img src="resource/zrlight_lightonoff_char.png" width="300" height="533" />

#### <a name="TargetAddressTypeCharacteristic"></a>Target Address Type Characteristic (UUID: 11A2)

This characteristic supports read and write properties, it contains
a 1-byte-long value indicating the target address type. Currently only 2 values are supported:
- 0x0: Send to the Binding Table
- 0x2: Send to the 16b address specified in the Target Address Characteristic

#### <a name="TargetAddressCharacteristic"></a>Target Address Characteristic (UUID: 11A3)

This characteristic supports read and write properties, it contains
a 2-byte-long value that is used for as the Destination Address of the Zigbee ZCL On/Off message. This is only used if the Target Address Type Characteristic is set to 0x02.

>Note: The endpoint is hard coded to 8, which is the endpoint of the TI Zigbee Light.

#### <a name="BetteryLevelCharacteristic"></a>Battery Level Characteristic (UUID: 11A4)

This characteristic supports read only properties, it contains
a 1-byte-long value that is used to diaplay the battery level.

>Note: In this implementation the Launchpad is USB powerd and this value always reads 100.

## <a name="DisableCUI"></a>Disabling Common User Interface

The common user interface (CUI) is a UART based interface that allows users to control and receive updates regarding the application. For various reasons, including reducing the memory footprint, the user is able to disable the common user interface (CUI). To disable the CUI, the following variable must be defined in `dmm_zr_light_remote_display_app.opts`:

```
-DCUI_DISABLE
```

> Please Note: particular features that are dependednt on the CUI wil be unavailable when this feature is enabled.
