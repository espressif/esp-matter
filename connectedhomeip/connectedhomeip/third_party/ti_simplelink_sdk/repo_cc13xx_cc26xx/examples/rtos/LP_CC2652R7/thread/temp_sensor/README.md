# Temperature Sensor Example Application

## Table of Contents

* [SysConfig Notice](#sysconfig-notice)
* [Introduction](#intro)
* [Hardware Prerequisites](#hardware-prereqs)
* [Software Prerequisites](#software-prereqs)
* [Functional Description](#functional-desc)
  * [Software Overview](#software-overview)
    * [Application Files](#application)
    * [Example Application](#example-application)
* [Configuration With SysConfig](#sysconfig)
* [Usage](#usage)
  * [Buttons](#usage-buttons)
  * [Display](#usage-display)
  * [Setting up the Thread Network](#usage-setup-nwk)
  * [Interfacing with the Example Application](#usage-control)

# <a name="intro"></a> Introduction

This document discusses how to use the Temperature Sensor Example Application
and the different parts that compose it. Thermostat Example Application is a
standalone CoAP server example running on Thread.

Some of the areas explored are:

- Setting up a network.

- Bringing up the device as a Minimum Thread Device (MTD).

- Initialization and use of the Constrained Application Protocol (CoAP) APIs.

# <a name="hardware-prereqs"></a> Hardware Prerequisites


## Device Reporting example

- 3 x CC13X2 /CC26X2 Wireless MCU LaunchPads

- 1 x BeagleBone Black.

- (optional) 1 x Sharp128 LCD boosterpack.


## Basic CoAP usage

- 2 x CC13X2 /CC26X2 Wireless MCU LaunchPads

- (optional) 1 x Sharp128 LCD boosterpack.


# <a name="software-prereqs"></a> Software Prerequisites

- Code Composer Studio (CCS) v10.0 or higher


# <a name="functional-desc"></a> Functional Description


## <a name="software-overview"></a> Software Overview

This section describes software components and the corresponding source file.


### <a name="application"></a> Application Files

- `tempsensor.[ch]`: Contains the application's event loop, CoAP callback
  functions, device initialization function calls, and all temperature sensor
  specific logic.

- `otstack.[ch]`: OpenThread stack processing, instantiation and network
  parameters.

- `task_config.h`: This file contains the definitions of the RTOS task
  priorities and stack sizes.

- `tiop_config.[ch]`: Contains OpenThread stack configurations. If using a
  SysConfig-enabled project (see the Configuration with SysConfig section
  below), these files are generated and configured through the SysConfig GUI.
  If using a non-SysConfig project, the files are a part of the project and
  parameters can be directly modified.

- `tiop_ui.[ch]`: Contains functions and defines to enable Thread-specific
  functionality for the Common User Interface (CUI).

- `tiop_app_defs.h`: Contains application-specific configurations for CUI.


### <a name="example-application"></a> Example Application

This application provides an example implementation of a temperature sensor
using the Thread wireless protocol and CoAP as the application layer protocol.
The temperature sensor application is configured as a Minimal Thread Device
(MTD) which supports CoAP commands to read the temperature. The temperature
sensor example also includes basic reporting functionality to a known IPv6
address, in this case the thermostat example application.


# <a name="sysconfig"></a> Configuration With SysConfig

SysConfig is a GUI configuration tool that allows for TI driver and stack
configurations. In order to configure projects using SysConfig, use the
SysConfig-enabled version of the Thread examples located in
`<SDK_ROOT>/examples`

To configure using SysConfig, import the SysConfig-enabled project into CCS.
Double click the `*.syscfg` file from the CCS project explorer, where `*` is
the name of the example project. The SysConfig GUI window will appear, where
Thread stack and TI driver configurations can be adjusted. These settings will
be reflected in the generated files `tiop_config.[ch]`.

The example project comes with working default settings for SysConfig. It is
not recommended to change the default driver settings, as any changes may
impact the functionality of the example. The Thread stack settings may be
changed as required for your use case.

One important note about TI-OpenThread SysConfig is how SysConfig settings and
non-volatile storage settings are applied. If the LaunchPad non-volatile holds
a valid Thread dataset, SysConfig settings will not be applied on boot.
SysConfig settings are only applied when non-volatile storage does not hold a
valid Thread dataset. To guarantee SysConfig settings are applied, perform a
factory reset of the non-volatile storage, as described below.


# <a name="usage"></a> Usage

This section describes how to set up and run the Temperature Sensor Example
Application.


## <a name="usage-buttons"></a> Buttons

- `BTN-2` at boot: A factory reset of the non-volatile storage is
  performed.  This must be pressed at the start of the `OtStack_task` function.

- `BTN-2`: Start the joining process. This may be pressed after the hold image
  appears on-screen.

## <a name="usage-display"></a> Display

The temperature sensor events will be displayed through UART to a serial
terminal emulator. To enable the serial terminal in CCS press `Ctrl + Shift +
Alt + T`, select `Serial Terminal` under `Choose terminal`, select `115200` for
Baud Rate and click `OK`. PuTTY may also be used as the serial terminal
emulator. The serial interface implements a Common User Interface (CUI). More
details are provided in the "Example Applications" section in the Thread docs
of the SDK. Application-specific portions of CUI are described
[below](#application-specific-cui).


## <a name="usage-setup-nwk"></a> Setting up the Thread Network

This section describes how to set up a Thread network. The application supports
the ability to be commissioned into a Thread network. Commissioning may be
bypassed by setting the `TIOP_CONFIG_SET_NW_ID` parameter to 1 in
`tiop_config.h` and setting the network ID parameters there. This can also be
done through SysConfig in the TI-OpenThread stack module, under the Network ID
submodule. By setting the PAN ID to a valid, non-broadcast ID (not 0xFFFF), the
device can be pre-commissioned to an existing network.

1. Set up a LaunchPad as a CLI FTD device by following the README files in
   the respective application folder.

2. Load and run the Temperature Sensor example on a second LaunchPad.

3. The Temperature Sensor will print out the device's EUI64 and the
   application's PSKd (pre-shared key device identifier) in CUI. If the device
   was already commissioned or bypassed as described above, skip to step 8.

   ```
   Device Info: [EUI64] 0x00124b000f6e6113  [PSKD] TMPSENS1
   ```

4. Start a commissioner on the CLI FTD by issuing the following command:
   `commissioner start`. It will display `Done` if it succeeds in becoming the
   active commissioner.

5. Add the Temperature Sensor LaunchPad device as a joiner device by providing
   the EUI64 and pskd (from step 3) as credentials to the commissioner:
   `commissioner joiner add 00124b000f6e6113 TMPSENS1`. It will display `Done`
   if it is successful in adding the joiner entry.

6. Now on the Temperature Sensor LaunchPad, press `BTN-2` to start the joining
   process. The UART will print `Joining Nwk ...`.

7. Once the joining process has successfully completed, the UART will print
   `Joined Nwk`. The green LED should turn on on the LaunchPad once it has
   joined the network.

8. Next we need to get the IPv6 address of the temperature sensor LaunchPad.
   The temperature sensor is a sleepy end device; we cannot discover the
   address using the realm-local all nodes multicast. Transmission of
   realm-local multicasts by parent routers is detailed in section 5.2.3.2 of
   the Thread 1.1.1 specification. It is necessary to use the Realm-Local
   All-Thread-Nodes multicast address. This address will have the form
   `ff33:0040:<ML-PREFIX>::1`, where `<ML-PREFIX>` is the 64 bit mesh-local
   prefix. This address may be discovered with the command `ipmaddr`. On the
   CLI, use the command `ping ff33:0040:<ML-PREFIX>::1` to send an ICMP echo
   request to the realm-local all nodes multicast address. All devices on
   Thread network will respond with an ICMP echo response. You will see in the
   terminal a response like the one below.

   ```
   8 bytes from fdde:ad00:beef:0:0:ff:fe00:b401: icmp_seq=1 hlim=64 time=11ms
   ```


### Running the example with reporting

The Temperature Sensor example has a basic reporting feature. When connected to
a network with a Globally Unique Address, the temperature sensor will attempt
to post the temperature it reads to the thermostat. To enable this feature you
need to setup a Thread network with an NCP connected to a BeagleBone Black and
a LaunchPad with the Thermostat Example. Consult the NCP example's README for
information on setting up a BeagleBone Black based border router.

*NOTE*: This kind of static addressing is a hack of SLAAC. Proper discovery
mechanisms are being explored.


## <a name="usage-control"></a>
## Interfacing with the Temperature Sensor Example Application

The temperature sensor application hosts a simple CoAP server with one
registered resource for the current temperature. This resource supports CoAP
GET commands. Any device with scope of the temperature sensor's IPv6 address
can send commands to the temperature sensor application.

Temperature Sensor Attribute URI:

- Temperature value: `tempsensor/temperature`

Open up the serial terminal to the `cli_ftd` application and also to the
temperature sensor application.


### Starting the CoAP client

In the CLI FTD serial terminal, execute `coap start` at the prompt to start the
CoAP service. It will display the following message if it successful in
starting the CoAP service. ` Coap service started: Done`


### Getting status from the Temperature Sensor

To get the temperature sensor's current temperature, execute the
following command on the CLI FTD terminal.

```
coap get fd00:db7:0:0:0:ff:fe00:b401 tempsensor/temperature
```

*NOTE*: The IPv6 address will be different for your setup

The temperature sensor should respond, and the `cli_ftd` will print a message
like the following.

```
Received coap response with payload: 3730
```

Converting the payload from hex to ascii we get `70` which is the temperature
in degrees Fahrenheit.

This value is the same value displayed on sensor at the time of sending the
response.

```
   APP Info: [Temperature] 70 [Poll Period] 2000
```

*NOTE*: The `Poll Period` field indicates how often the device polls its
parent. This is used as both a keep-alive and a poll for any pending data.

#### <a name = "application-specific-cui"></a> Application-specific CUI

There are currently no application-specific actions for the temperature sensor.