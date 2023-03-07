# Command Line Example Application

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
  * [OAD](#usage-oad)
  * [Setting up the Thread Network](#usage-setup-nwk)
  * [Interfacing with the Example Application](#usage-control)

# <a name="intro"></a> Introduction

This document discusses how to use the Over-the-Air Download enabled Command
Line Example App. The OAD CLI example application functions identically to the
Thread CLI FTD example application, with the added functionality of enabling
download with BLE OAD. This example will allow you to download a Thread image
from a BLE distributor to a BLE target, then form and join a network. The
walk-through below will setup a basic ad-hoc Thread network between two CLI
examples. Consult the Thread User's guide for information on the OAD process.


# <a name="hardware-prereqs"></a> Hardware Prerequisites

- 2 x CC13X2 / CC26X2 Wireless MCU LaunchPads


# <a name="software-prereqs"></a> Software Prerequisites

- Code Composer Studio (CCS) v10.0 or higher


# <a name="functional-desc"></a> Functional Description


## <a name="software-overview"></a> Software Overview

This section describes software components and the corresponding source file.


### <a name="application"></a> Application Files

- `main_oad.c`: The main entry point for the example. Creates and starts the
  tasks for operating the OpenThread stack and CLI example.

- `cli.c`: Instantiation of command line object and heartbeat LED loop.

- `otstack.c`: OpenThread stack instantiation and processing.

- `task_config.h`: Defines RTOS task priorities and call stack sizes.

- `oad_image_header.[ch]`: Defines structure for OAD image header.

- `bim_oad_debug_image.[ch]`: Array of a pre-compiled BIM for debugging.

- `tiop_config.[ch]`: Contains OpenThread stack configurations. If using a
  SysConfig-enabled project (see the Configuration with SysConfig section
  below), these files are generated and configured through the SysConfig GUI.
  If using a non-SysConfig project, the files are a part of the project and
  parameters can be directly modified.


### <a name="example-application"></a> Example Application

This application provides a simple debugging interface to the OpenThread stack.
This application is used as the certification interface for the OpenThread
stack to Thread test harness, but is also useful as an exploration tool.

Following is information on how to setup a simple network between two CLI
examples. For a more in-depth explanation of the commands available, refer the
README.md in the OpenThread source directory at
`<SDK_ROOT>/source/third_party/openthread/src/cli/README.md`.


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

This section describes how to set up and run the CLI Example Application.

## <a name="usage-buttons"></a> Buttons

- `BTN-2` at boot: The OAD image header of the currently running image is
  invalidated and a reset is executed. Upon running, the BIM will load the
  factory image from external flash.

- `BTN-1` at boot: A reset to factory image is performed for OAD capable devices.
   This must be pressed at the start of the `OtStack_task` function.

- `BTN-2`: Unused.


## <a name="usage-display"></a> Display

The command line interface will present on the device's UART, and can be
displayed through a serial terminal emulator. To enable the serial terminal in
CCS press `Ctrl + Shift + Alt + T`, select `Serial Terminal` under `Choose
terminal`, select `115200` for Baud Rate and click OK.


## <a name="usage-oad"></a> Performing an OAD

Out of the box, the LaunchPad is programmed with a OAD enabled BLE example
project. You should familiarize yourself with the "Over-the-Air Download (OAD)"
section of the Thread User's Guide. Setting up the OAD environment is detailed
in the "Setting up the BLE OAD Environment" section of the Thread User's Guide,
consult this section for setting up the distributor. Performing an OAD is
detailed in the "Performing a BLE OAD" section of the Thread User's guide.

1. If you have programmed your LaunchPad since opening the box, program the BIM
   application and an OAD enabled BLE example application, like
   `simple_peripheral` or `project_zero` onto the target LaunchPad.

2. Setup BTool and Host Test as a BLE OAD distributor as described in the
   Thread User's guide.

3. Build the OAD CLI Thread example application.

4. Use the BLE OAD distributor to download the Thread `*.bin` downloadable
   image to the target LaunchPad.

5. Connect to the target LaunchPad's UART and press `Enter` to see the Thread
   CLI prompt `>`.

6. You may now use the Thread example application like normal.

7. (Optional) to download another image, follow the instructions in the
   [Buttons][#usage-buttons] section to reload the OAD enabled BLE factory
   image.


## <a name="usage-setup-nwk"></a> Setting up the Thread Network

This section describes how to set up a Thread network between two CLI devices.
Program two LaunchPads with the CLI image and follow the steps below. One of
the CLI examples must be a Full Thread Device (FTD).

On the first CLI example, press `Enter` to see a prompt `>`. Then use the
`channel` command to set the desired channel. Use the `panid` command to set
the current running PAN. Finally use the commands `ifconfig up` and `thread
start` to start the Thread stack. Below is an example of bringing up a device
on channel `14` and PANID `0xface`.

```

> channel 14
Done
> panid 0xface
Done
> ifconfig up
Done
> thread start
Done
> state
detached
> state
leader
>
```

On the second CLI example, press `Enter` to see a prompt `>`. Then use the
`scan` command to make sure that this LaunchPad can see the first one. The
following prompt may be swallowed up by the output of the scan, press `Enter`
to get the prompt again. Then use the `channel` and `panid` commands as in the
first example to set the same channel and PANID. Finally, use the `ifconfig up`
and `thread start` commands to start the Thread stack. The second example will
connect to the first example as a child.

```

> scan
| J | Network Name     | Extended PAN     | PAN  | MAC Address      | Ch | dBm | LQI |
+---+------------------+------------------+------+------------------+----+-----+-----+
> | 0 | OpenThread       | dead00beef00cafe | face | 9e782c4394ca339f | 14 | -63 |  61 |
Done

> channel 14
Done
> panid 0xface
Done
> ifconfig up
Done
> thread start
Done
> state
child
>
```


## <a name="usage-control"></a> Pinging between the CLI examples

Once you have setup the network as above, you can ping between the two nodes.
Here we ping the IPv6 relm-local all nodes multicast address, and get a
response from our leader's link local address.

```
> ping ff03::1
> 8 bytes from fe80:0:0:0:f423:af0:300:b057: icmp_seq=1 hlim=64 time=37ms

>
```
