# Shade Example Application

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

This document discusses how to use the Shade Example App and the different
parts that compose it. Shade Example Application is a standalone CoAP server
example running on Thread.

Some of the areas explored are:

- Setting up a network.

- Bringing up the device as a Minimum Thread Device(MTD).

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

- `shade.[ch]`: Contains the application's event loop, CoAP callback functions,
  and device initialization function calls, and all shade specific logic.

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

This application provides an example implementation of a shade (blinds) using
the Thread wireless protocol and CoAP as the application layer protocol. The
shade application is configured as a Minimal Thread Device (MTD) which supports
CoAP commands to control the shade state. The shade can be in three states:
open, closed, and drawn.


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

This section describes how to set up and run the Shade Example Application.


## <a name="usage-buttons"></a> Buttons

- `BTN-2` at boot: A factory reset of the non-volatile storage is
  performed.  This must be pressed at the start of the `OtStack_task` function.

- `BTN-2`: Start the joining process. This may be pressed after the hold image
  appears on-screen.


## <a name="usage-display"></a> Display

There are 2 ways that this application will display information to the user
which can be used simultaneously and are described below:

1. Serial terminal: The shade events will be displayed through UART to a serial
   terminal emulator.  To enable the serial terminal in CCS press `Ctrl +
   Shift + Alt + T`, select `Serial Terminal` under `Choose terminal`, select
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
device can be pre-commissioned to an existing network.

1. Set up a LaunchPad as a CLI FTD device by following the README files in
   the respective application folder.

2. Load and run the Shade example on a second LaunchPad.

3. The Shade will print out the device's EUI64 and the application's PSKd
   (pre-shared key device identifier) over CUI. If the device has already been
   commissioned, skip to step 8.

   ```
   Device Info: [EUI64] 0x00124b000f6e6113  [PSKD] SHADEEX1
   ```

4. Start a commissioner on the CLI FTD by issuing the following command:
   `commissioner start`. It will display `Done` if it succeeds in becoming the
   active commissioner.

5. Add the Shade LaunchPad device as a joiner device by providing the EUI64 and
   pskd (from step 3) as credentials to the commissioner: `commissioner joiner
   add 00124b000f6e6113 SHADEEX1`. It will display `Done` if it is successful
   in adding the joiner entry.

6. Now on the Shade LaunchPad, press `BTN-2` to start the joining process. The
   display will show `Joining Nwk ...`.

7. Once the joining process has successfully completed, the display will show
   `Nwk Joined`. 
   If the LCD is used, it will then switch to the shade image. 
   The green LED should turn on on the LaunchPad once it has joined the
   network.

8. Next we need to get the IPv6 address of the shade LaunchPad.
   On the CLI, use the command `ping ff03::1` to send an ICMP echo request to
   the realm-local all nodes multicast address.
   All devices on the Thread network will respond with an ICMP echo response.
   You will see in the terminal a response like the one below.

   ```
   8 bytes from fd00:db7:0:0:0:ff:fe00:b401: icmp_seq=1 hlim=64 time=11ms
   ```


## <a name="usage-control"></a> Interfacing with the Shade Example Application

The shade application hosts a simple CoAP server with one registered resource
for the blind state. This resource supports CoAP GET and POST commands. Any
device with scope of the shade's IPv6 address can send commands to the shade
application.

Shade Attribute URI:

- Shade state: `blinds/state`

Open up the serial terminal to the `cli_ftd` application and also to the shade
application.

### Starting the CoAP client

In the CLI FTD serial terminal, execute `coap start` at the prompt to start the
CoAP service. It will display the following message if it successful in
starting the CoAP service. ` Coap service started: Done`


### Getting status from the Shade

To get the shade's blind state, execute the following command on the CLI FTD
terminal.
```
coap get fd00:db7:0:0:0:ff:fe00:b401 /blinds/state
```

*NOTE*: The IPv6 address will be different for your setup

The shade should respond, and the `cli_ftd` will print a message like the
following.

```
Received coap response with payload: 636c6f736564
```

Converting the payload from hex to ascii we get `closed`.


### Controlling the Shade

The shade state can be changed by sending the appropriate payload in a CoAP
confirmable (con) POST command message, to the IPv6 address of the shade and
the resource URI attribute.

Use the following command in the CLI FTD terminal to set the shade's blind
state to open.

```
coap post fd00:db7:0:0:0:ff:fe00:b401 blinds/state con open
```

*NOTE*: The IPv6 address will be different for your setup

The initial command will result in the message `Sending coap request: Done`.
The shade will respond, and the CLI FTD will print the following message.

```
Received coap response with payload: 636c6f736564
```

If the POST was successful, then the shade will indicate this on the LCD
or UART terminal. If a LCD boosterpack is used, the shade will change the image
displayed. If a UART terminal is used, the shade will print its updated state.

```
   APP Info: [Shade State] open
```

The above process can be repeated with the string `closed` or `drawn` in place
of `open` to set the shade example to those respective state.


#### <a name = "application-specific-cui"></a> Application-specific CUI

The application-specific actions for the Shade example are `OPEN SHADE`, `CLOSE
SHADE`, and `DRAW SHADE`:

```
TI Thread Shade

<   OPEN SHADE   >


```
This action will set the state of the shade to "open". To use, hit the "Enter"
key.

```
TI Thread Shade

<   CLOSE SHADE  >


```
This action will set the state of the shade to "closed". To use, hit the
"Enter" key.

```
TI Thread Shade

<   DRAW SHADE   >


```
This action will set the state of the shade to "drawn".To use, hit the "Enter"
key.