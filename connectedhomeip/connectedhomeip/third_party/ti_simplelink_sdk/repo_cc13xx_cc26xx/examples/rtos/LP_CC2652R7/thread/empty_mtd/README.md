# Empty Example Application

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

This document discusses how to use the Empty Example Application.

The Empty example is the basic introduction to the Thread Stack. This example
allows the user to build any form of project on top of a project that connects
to and sends and receives data across a Thread network.


# <a name="hardware-prereqs"></a> Hardware Prerequisites

- 2 x CC13X2 / CC26X2 Wireless MCU LaunchPads


# <a name="software-prereqs"></a> Software Prerequisites

- Code Composer Studio (CCS) v10.0 or higher


# <a name="functional-desc"></a> Functional Description


## <a name="software-overview"></a> Software Overview

This section describes software components and the corresponding source file.


### <a name="application"></a> Application Files

- `main.c`: The main entry point for the example. Creates and starts the tasks
  for operating the OpenThread stack and CLI example.

- `empty.[ch]`: Instantiation of empty object and heartbeat LED loop.

- `otstack.[ch]`: OpenThread stack instantiation and processing.

- `task_config.h`: Defines RTOS task priorities and call stack sizes.

- `tiop_config.[ch]`: Contains OpenThread stack configurations. If using a
  SysConfig-enabled project (see the Configuration with SysConfig section
  below), these files are generated and configured through the SysConfig GUI.
  If using a non-SysConfig project, the files are a part of the project and
  parameters can be directly modified.

- `tiop_ui.[ch]`: Contains functions and defines to enable Thread-specific
  functionality for the Common User Interface (CUI).

- `tiop_app_defs.h`: Contains application-specific configurations for CUI.


### <a name="example-application"></a> Example Application

This application provides an example of a thread application with no use cases
that can be built on to form any application the user requires. The empty
example can be configured as a Full Thread Device (FTD) or a Minimal Thread
Device (MTD). The empty example can connect to an established Thread network,
but has no CoAP functionality once connected.

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

This section describes how to set up and run the Empty Example Application.

## <a name="usage-buttons"></a> Buttons

- `BTN-2` at boot: A factory reset of the non-volatile storage is
  performed.  This must be pressed at the start of the `OtStack_task` function.

- `BTN-2`: Start the joining process. This may be pressed after the hold image
  appears on-screen.

## <a name="usage-display"></a> Display

The empty example will display the EUI64 ID and the default passkey through a
UART to serial terminal emulator. To enable the serial terminal in CCS press
`Ctrl + Shift + Alt T`, select `Serial Terminal` under `Choose terminal`,
select `115200` for Baud Rate and click `OK`. PuTTY may also be used as the
serial terminal emulator. The serial interface implements a Common User
Interface (CUI). More details are provided in the "Example Applications"
section in the Thread docs of the SDK. Application-specific portions of CUI are
described [below](#application-specific-cui).


## <a name="usage-setup-nwk"></a> Setting up the Thread Network

This section describes how to set up a Thread network. The application supports
the ability to be commissioned into a Thread network.  Commissioning may be
bypassed by setting the `TIOP_CONFIG_SET_NW_ID` parameter to 1 in
`tiop_config.h` and setting the network ID parameters there. This can also be
done through SysConfig in the TI-OpenThread stack module, under the Network ID
submodule. By setting the PAN ID to a valid, non-broadcast ID (not 0xFFFF), the
device can be pre-commissioned to an existing network.

1. Set up a LaunchPad as a CLI FTD device by following the README files in the
   respective application folder.

2. Load and run the Empty example on a second LaunchPad.

3. The Empty Example will print out the device's EUI64 and the application's
   PSKd (pre-shared key device identifier) over the UART terminal. If the
   device was already commissioned or bypassed as described above, skip to step
   8.

   ```
   Device Info: [EUI64] 0x00124b000f6e6113  [PSKD] PPSSKK
   ```

4. Start a commissioner on the CLI FTD by issuing the following command:
   `commissioner start`. It will display `Done` if it succeeds in becoming the
   active commissioner.

5. Add the Empty Example LaunchPad device as a joiner device by providing the
   EUI64 and pskd (from step 3) as credentials to the commissioner:
   `commissioner joiner add 00124b000f6e6113 PPSSKK`. It will display `Done` if
   it is successful in adding the joiner entry.

6. Now on the Empty Example LaunchPad, press `BTN-2` to start the joining
   process. The UART will print `Joining Nwk ...`.

7. Once the joining process has successfully completed, the UART will print
   `Joined Nwk`. The green LED should turn on on the LaunchPad once it has
   joined the network.

8. Next we need to get the IPv6 address of the empty example LaunchPad.
   On the CLI, use the command `ping ff03::1` to send an ICMP echo request to
   the realm-local all nodes multicast address.
   All devices on the Thread network will respond with an ICMP echo response.
   You will see in the terminal a response like the one below.

   ```
   8 bytes from fdde:ad00:beef:0:0:ff:fe00:b401: icmp_seq=1 hlim=64 time=11ms
   ```
