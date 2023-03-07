# Door Lock Example Application

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

This document discusses how to use the Door Lock Example App and the different
parts that compose it. Door Lock Example Application is a standalone CoAP
server example running on Thread.

Some of the areas explored are:

- Setting up a network.

- Bringing up the device as a Minimum Thread Device (MTD).

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

- `doorlock.[ch]`: Contains the application's event loop, CoAP callback
  functions, and device initialization function calls, and all door lock
  specific logic.

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

This application provides an example implementation of a door lock using the
Thread wireless protocol and CoAP as the application layer protocol. The door
lock application is configured as a Minimal Thread Device (MTD) which supports
CoAP commands to lock and unlock the door. The door lock can be in two states:
lock, and unlock.


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

This section describes how to set up and run the Door Lock Example Application.


## <a name="usage-buttons"></a> Buttons

- `BTN-2` at boot: A factory reset of the non-volatile storage is
  performed.  This must be pressed at the start of the `OtStack_task` function.

- `BTN-1` at boot: A reset to factory image is performed for OAD capable
  devices. This must be pressed at the start of the `OtStack_task` function.

- `BTN-2`: Start the joining process. This may be pressed after the hold image
  appears on-screen.


## <a name="usage-display"></a> Display

There are 2 ways that this application will display information to the user
which can be used simultaneously and are described below:

1. Serial terminal: The door lock events will be displayed through UART to
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
device can be pre-commissioned to an existing network.

1. Set up a LaunchPad as a CLI FTD device by following the README files in
   the respective application folder.

2. Load and run the DoorLock example on a second LaunchPad.

3. The Door Lock Launchpad will print out the device's EUI64 and the
   application's PSKd (pre-shared key device identifier) in CUI, on the Device
   Info line. If the device has already been commissioned, skip to step 8.

   ```
   Device Info: [EUI64] 00124b000f6e6113  [PSKD] DRRLCK1
   ```

4. Start a commissioner on the CLI FTD by issuing the following command:
   `commissioner start`. It will display `Done` if it succeeds in becoming the
   active commissioner.

5. Add the Door Lock LaunchPad device as a joiner device by providing its EUI64
   and pskd (from step 3) as credentials to the commissioner: `commissioner
   joiner add 00124b000f6e6113 DRRLCK1`. It will display `Done` if it is
   successful in adding the joiner entry.

6. Now on the Door Lock LaunchPad, press `BTN-2` to start the joining process.
   The display will show `Joining Nwk ...`.

   ```
   Conn Info: [Status] Joining Nwk ...
   ```

7. Once the joining process has successfully completed, the display will show
   `Nwk Joined`. If the LCD is used, it will then switch to the door lock
   image. The green LED should turn on on the LaunchPad once it has joined the
   network.

   ```
   Conn Info: [Status] Nwk Joined
   ```

8. Next we need to get the IPv6 address of the Door Lock LaunchPad. On the CLI,
   use the command `ping ff03::1` to send an ICMP echo request to the
   realm-local all nodes multicast address. All devices on the Thread network
   will respond with an ICMP echo response. You will see in the terminal a
   response like the one below.

   ```
   8 bytes from fd00:db7:0:0:0:ff:fe00:b401: icmp_seq=1 hlim=64 time=11ms
   ```


## <a name="usage-control"></a> 
## Interfacing with the Door Lock Application

The door lock application hosts a simple CoAP server with one registered
resource for the lock state. This resource supports CoAP GET and POST commands.
Any device with scope of the door lock's IPv6 address can send commands to the
door lock application.

Door lock Attribute URI:

- Door lock state: `doorlock/lockstate`

Open up the serial terminal to the `cli_ftd` application and also to the Door
lock application.

### Starting the CoAP client

In the CLI FTD serial terminal, execute `coap start` at the prompt to start the
CoAP service. It will display the following message if it successful in
starting the CoAP service: ` Coap service started: Done` .


### Getting status from the Door lock

To get the door lock's lock state, execute the following command on the
CLI FTD terminal.
```
coap get fd00:db7:0:0:0:ff:fe00:b401 /doorlock/lockstate
```

*NOTE*: The IPv6 address will be different for your setup

The door lock should respond, and the `cli_ftd` will print a message like the
following.

```
Received coap response with payload: 756e6c6f636b
```

Converting the payload from hex to ascii we get `unlock`.


### Controlling the Door Lock

The door lock state can be changed by sending the appropriate payload in a CoAP
confirmable (con) POST command message, to the IPv6 address of the door lock
and the resource URI attribute.

Use the following command in the CLI FTD terminal to set the door lock's lock
state to lock.

```
coap post fd00:db7:0:0:0:ff:fe00:b401 /doorlock/lockstate con lock
```

*NOTE*: The IPv6 address will be different for your setup

The initial command will result in the message `Sending coap request: Done`.
The door lock will respond, and the CLI FTD will print the following message.

```
Received coap response with payload: 6c6f636b
```

If the POST was successful, then the door lock will indicate this on the LCD
or UART terminal. If a LCD boosterpack is used, the door lock will change the
image displayed. If a UART terminal is used, the door lock will print its
updated state.

```
   APP Info: [Doorlock State] lock
```

The above process can be repeated with the string `unlock` in place of `lock`
to set the door lock example to the unlocked state.


#### <a name = "application-specific-cui"></a> Application-specific CUI

The application-specific action for the Door Lock example is `TOGGLE LOCK`.

```
TI Thread Doorlock

<  TOGGLE LOCK   >


```
This action will toggle the state of the door lock. For example, if the state
of the door lock is "lock", this action will change it to "unlock", and vice
versa. To use, hit the "Enter" key.