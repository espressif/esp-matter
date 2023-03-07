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
  * [Setting up the Thread Network](#usage-setup-nwk)
  * [Interfacing with the Example Application](#usage-control)

# <a name="intro"></a> Introduction

This document discusses how to use the Command Line Example Application.

The CLI example is the basic introduction to the Thread Stack. This example
allows the user to explore the inner workings of the OpenThread stack. This
example will allow you to form and join a network. The walk-through below will
setup a basic ad-hoc Thread network between two CLI examples.


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

- `cli.c`: Instantiation of command line object and heartbeat LED loop.

- `otstack.c`: OpenThread stack instantiation and processing.

- `task_config.h`: Defines RTOS task priorities and call stack sizes.

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

- `BTN-2` at boot: A factory reset of the non-volatile storage is
  performed.  This must be pressed at the start of the `OtStack_task` function.

## <a name="usage-display"></a> Display

The command line interface will present on the device's UART, and can be
displayed through a serial terminal emulator. To enable the serial terminal in
CCS press `Ctrl + Shift + Alt + T`, select `Serial Terminal` under `Choose
terminal`, select `115200` for Baud Rate and click OK.


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
