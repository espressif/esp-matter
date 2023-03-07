# Thermostat Example Application

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

This document discusses how to use the Thermostat Example Application and the
different parts that compose it. Thermostat Example Application is a standalone
CoAP server example running on Thread.

Some of the areas explored are:

- Setting up a network.

- Bringing up the device as a Full Thread Device(FTD).

- Initialization and use of the Constrained Application Protocol (CoAP) APIs.

# <a name="hardware-prereqs"></a> Hardware Prerequisites

- 2 x CC13X2 / CC26X2 Wireless MCU LaunchPads

- (optional) 1 x Sharp128 LCD boosterpack.


# <a name="software-prereqs"></a> Software Prerequisites

- Code Composer Studio (CCS) v10.0 or higher


# <a name="functional-desc"></a> Functional Description


## <a name="software-overview"></a> Software Overview

This section describes software components and the corresponding source file.


### <a name="application"></a> Application Files

- `thermostat.[ch]`: Contains the application's event loop, CoAP callback
  functions, device initialization function calls, and all thermostat specific
  logic.

- `otstack.[ch]`: OpenThread stack processing, instantiation and network
  parameters.

- `task_config.h`: This file contains the definitions of the RTOS task
  priorities and stack sizes.

- `images.[ch]`: Contains the raw binary of the images being displayed on the
  LCD screen.

- `tiop_config.[ch]`: Contains OpenThread stack configurations. If using a
  SysConfig-enabled project (see the Configuration with SysConfig section
  below), these files are generated and configured through the SysConfig GUI.
  If using a non-SysConfig project, the files are a part of the project and
  parameters can be directly modified.

- `tiop_ui.[ch]`: Contains functions and defines to enable Thread-specific
  functionality for the Common User Interface (CUI).

- `tiop_app_defs.h`: Contains application-specific configurations for CUI.


### <a name="example-application"></a> Example Application

This application provides an example implementation of a thermostat using the
Thread wireless protocol and CoAP as the application layer protocol. The
thermostat application is configured as a Full Thread Device (FTD) which
supports CoAP commands to control the temperature and setpoint. These are
integers in the range of [10 .. 99].


# <a name="sysconfig"></a> Configuration With SysConfig

SysConfig is a GUI configuration tool that allows for TI driver and stack
configurations. In order to configure projects using SysConfig, use the
SysConfig-enabled version of the Thread examples located in
`<SDK_ROOT>/examples`.

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

This section describes how to set up and run the Thermostat Example
Application.


## <a name="usage-buttons"></a> Buttons

- `BTN-2` at boot: A factory reset of the non-volatile storage is performed.
  This must be pressed at the start of the `OtStack_task` function.

- `BTN-2`: Start the joining process. This may be pressed after the hold image
  appears on-screen.


## <a name="usage-display"></a> Display

There are 2 ways that this application will display information to the user
which can be used simultaneously and are described below:

1. Serial terminal: The thermostat events will be displayed through UART to a
   serial terminal emulator.  To enable the serial terminal in CCS press `Ctrl
   + Shift + Alt + T`, select `Serial Terminal` under `Choose terminal`, select
   `115200` for Baud Rate and click `OK`. PuTTY may also be used as the serial
   terminal emulator. The serial interface implements a Common User Interface
   (CUI). More details are provided in the "Example Applications" section in
   the Thread docs of the SDK. Application-specific portions of CUI are
   described [below](#application-specific-cui).

2. Sharp128 LCD boosterpack: There is no extra configuration needed to use the
   LCD boosterpack other than plugging it to the LaunchPad running the example
   application.


## <a name="usage-setup-nwk"></a> Setting up the Thread Network

This section describes how to set up a Thread network. The application supports
the ability to be commissioned into a Thread network. Commissioning may be
bypassed by setting the `TIOP_CONFIG_SET_NW_ID` parameter to 1 in
`tiop_config.h` and setting the network ID parameters there. This can also be
done through SysConfig in the TI-OpenThread stack module, under the Network ID
submodule. By setting the PAN ID to a valid, non-broadcast ID (not 0xFFFF), the
device can be pre-commissioned to an existing network. The Temperature Sensor
example has the ability to report to the Thermostat, consult its README for
details on this functionality.

1. Set up a LaunchPad as a CLI FTD device by following the README files in the
   respective application folder.

2. Load and run the Thermostat example on a second LaunchPad.

3. The Thermostat will print out the device's EUI64 and the application's PSKd
   (pre-shared key device identifier) in CUI. If the device has already been
   commissioned, skip to step 8.

   ```
   Device Info: [EUI64] 0x00124b000f6e6113  [PSKD] THERMSTAT1
   ```

4. Start a commissioner on the CLI FTD by issuing the following command:
   `commissioner start`. It will display `Done` if it succeeds in becoming the
   active commissioner.

5. Add Thermostat LaunchPad device as a joiner device by providing the EUI64
   and pskd (from step 3) as credentials to the commissioner: `commissioner
   joiner add 00124b000f6e6113 THERMSTAT1`. It will display `Done` if it is
   successful in adding the joiner entry.

6. Now on the Thermostat LaunchPad, press `BTN-2` to start the joining process.
   The display will show `Joining Nwk ...`.

7. Once the joining process has successfully completed, the display will show
   `Nwk Joined`. If the LCD is used, it will then switch to the thermostat
   image. The green LED should turn on on the LaunchPad once it has joined the
   network.

8. Next we need to get the IPv6 address of the thermostat LaunchPad. On the
   CLI, use the command `ping ff03::1` to send an ICMP echo request to the
   realm-local all nodes multicast address. All devices on the Thread network
   will respond with an ICMP echo response. You will see in the terminal a
   response like the one below.

   ```
   8 bytes from fd00:db7:0:0:0:ff:fe00:b401: icmp_seq=1 hlim=64 time=11ms
   ```


## <a name="usage-control"></a>
## Interfacing with the Thermostat Example Application

The thermostat application hosts a simple CoAP server with two registered
resources for the current temperature and setpoint. These resources support
CoAP GET and POST commands. Any device with scope of the thermostat's IPv6
address can send commands to the thermostat application.

Thermostat Attribute URI:

- Temperature value: `thermostat/temperature`

- Temperature setpoint: `thermostat/setpoint`

Open up the serial terminal to the `cli_ftd` application and also to the
thermostat application.

### Starting the CoAP client

In the CLI FTD serial terminal, execute `coap start` at the prompt to start the
CoAP service. It will display the following message if it successful in
starting the CoAP service. ` Coap service started: Done`


### Getting status from the Thermostat

To get the thermostat's current temperature, execute the following command into
the CLI FTD terminal.

```
coap get fd00:db7:0:0:0:ff:fe00:b401 thermostat/temperature
```

*NOTE*: The IPv6 address will be different for your setup

The thermostat should respond, and the `cli_ftd` will print a message like the
following.

```
Received coap response with payload: 3638
```

Converting the payload from hex to ascii we get `68` which is the default
temperature in degrees Fahrnenheit.

The above process can be repeated with the endpoint `thermostat/setpoint` in
place of `thermostat/temperature` to get the current setpoint.


### Controlling the Thermostat

The thermostat temperature can be changed by sending the appropriate payload in
a CoAP confirmable (con) POST command message, to the IPv6 address of the
thermostat and the resource URI attribute.

Use the following command in the CLI FTD terminal to set the thermostat's
temperature to 90 degrees Fahrenheit.

```
coap post fd00:db7:0:0:0:ff:fe00:b401 thermostat/temperature con 90
```

*NOTE*: The IPv6 address will be different for your setup.

The initial command will result in the message `Sending coap request: Done`.
The thermostat will respond, and the CLI FTD will print the following message.

```
Received coap response with payload: 3930
```

If the POST was successful, then the door lock will indicate this on the LCD
or UART terminal.
If a LCD boosterpack is used, the thermostat will change the image displayed.
If a UART terminal is used, the thermostat will print its updated state.

```
   APP Info: [Temperature] 90 [Set Point] 34
```

The above process can be repeated with the endpoint `thermostat/setpoint` to
change the current setpoint of the thermostat.


#### <a name = "application-specific-cui"></a> Application-specific CUI

The application-specific action for the Thermostat example is `SET POINT`:

```
    22

<    SET POINT   >

```
This action will adjust the set point to the specified value.
To use, hit the "Enter" key, then set the desired value using the number keys.
Hit the "Enter" key again to apply your changes.
