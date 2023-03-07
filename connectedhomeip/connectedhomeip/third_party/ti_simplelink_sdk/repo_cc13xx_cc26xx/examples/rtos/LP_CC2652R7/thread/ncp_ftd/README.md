# Network Co-Processor Example Application

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

This document discusses how to use the Network Co-Processor example
application.

The NCP example is intended to be used with a host processor connected through
UART or SPI. This example is setup to connect with the UART interface over the
XDS110 USB-to-UART. The NCP uses the Spinel protocol to communicate to the
host. The NCP can be used with the wpantund or border router packages from
OpenThread.

# <a name="hardware-prereqs"></a> Hardware Prerequisites

- 2 x CC13X2 / CC26X2 Wireless MCU LaunchPads

- 1 x Linux computer


# <a name="software-prereqs"></a> Software Prerequisites

- Code Composer Studio (CCS) v10.0 or higher


# <a name="functional-desc"></a> Functional Description


## <a name="software-overview"></a> Software Overview

This section describes software components and the corresponding source file.


### <a name="application"></a> Application Files

- `main.c`: The main entry point for the example. Creates and starts the tasks
  for operating the OpenThread stack and NCP example.

- `ncp.c`: Instantiation of Network Co-Processor object and heartbeat LED loop.

- `otstack.c`: OpenThread stack instantiation and processing.

- `task_config.h`: Defines RTOS task priorities and call stack sizes.

- `tiop_config.[ch]`: Contains OpenThread stack configurations. If using a
  SysConfig-enabled project (see the Configuration with SysConfig section
  below), these files are generated and configured through the SysConfig GUI.
  If using a non-SysConfig project, the files are a part of the project and
  parameters can be directly modified.


### <a name="example-application"></a> Example Application

This application provides an interface for a host system to control the
OpenThread networking stack.

A full example of the NCP working with a host processor can be found in the
[Border Router Repository][br-gh]. This will create a border router on the host
processor that allows off-mesh commissioning. Information on building the
border router can be found on [openthread.io][br-install].

Another example application to control the NCP can be found in [OpenThread's
wpantund repository][wpantund-gh]. This must be built on your Linux host PC
before using this NCP example. Information on building wpantund can be found in
the [INSTALL.md][wpantund-install].

[br-gh]: https://github.com/openthread/borderrouter
[br-install]: https://openthread.io/guides/border_router/beaglebone_black
[wpantund-gh]: https://github.com/openthread/wpantund
[wpantund-install]: https://github.com/openthread/wpantund


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

This section describes how to set up and run the NCP Example Application. It is
assumed that you have installed wpantund on the host Linux machine.


## <a name="usage-buttons"></a> Buttons

- `BTN-2` at boot: A factory reset of the non-volatile storage is
  performed.  This must be pressed at the start of the `OtStack_task` function.

## <a name="usage-display"></a> Display

There is no display for the NCP example application.


## <a name="usage-setup-nwk"></a> Setting up the Thread Network

This section describes how to set up a Thread network.

*NOTE*: It is assumed that you have also built a CLI example.

On the CLI example; set the channel and panid, then start the OpenThread stack.

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

The first device has started a Thread network on channel 14 with panid 0xface.

*NOTE*: It is assumed that you have installed wpantund according to the
OpenThread instructions.

Your LaunchPad may present its data port on `/dev/ttyUSB1` instead of
`/dev/ttyUSB0`. This can be set in the configuration file for wpantund.

On your host Linux PC start the wpan tunnel daemon.

```
$ sudo /usr/local/sbin/wpantund -o NCPSocketName /dev/ttyUSB0
```

Open another terminal and start the wpan control application. Then scan for
networks and join the OpenThread network started by the CLI example above.

```
$ sudo /usr/local/bin/wpanctl
wpanctl:wpan0> scan
   | Joinable | NetworkName        | PAN ID | Ch | XPanID           | HWAddr           | RSSI
---+----------+--------------------+--------+----+------------------+------------------+------
 1 |       NO | "OpenThread"       | 0xFACE | 12 | DEAD00BEEF00CAFE | CA3E7FFCA19D3633 |  -45
wpanctl:wpan0> join -T r -p 0xface -x dead00beef00cafe -c 12 OpenThread
Joining "OpenThread" DEAD00BEEF00CAFE as node type "router"
Successfully Joined!
wpanctl:wpan0>
```

Open another terminal to ensure that the interface settings have been
registered.

*NOTE*: Interfaces other than loopback and wpan elided by the "SNIP".

```
$ ifconfig

----- 8< ----- SNIP ----- 8< -----

lo        Link encap:Local Loopback
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          UP LOOPBACK RUNNING  MTU:65536  Metric:1
          RX packets:753 errors:0 dropped:0 overruns:0 frame:0
          TX packets:753 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:0
          RX bytes:67805 (67.8 KB)  TX bytes:67805 (67.8 KB)
wpan0     Link encap:UNSPEC  HWaddr 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00
          inet6 addr: fe80::18d6:da3a:cc8f:1b46/64 Scope:Link
          UP POINTOPOINT RUNNING NOARP MULTICAST  MTU:1280  Metric:1
          RX packets:1 errors:0 dropped:0 overruns:0 frame:0
          TX packets:7 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:500
          RX bytes:73 (73.0 B)  TX bytes:448 (448.0 B)
```


## <a name="usage-control"></a> Pinging between the CLI examples

Once you have setup the network as above, you can ping between the NCP example
and the CLI example nodes. Here we ping the IPv6 all nodes multicast address,
and get a response from the CLI's link local address.

*NOTE*: Since we are using link local prefixes, it is necessary to specify the
link to send the packet on.

```
$ ping6 -c 4 -I wpan0 ff02::1
PING fe80:0:0:0:4c7:6d8b:f8cd:9ada(fe80::4c7:6d8b:f8cd:9ada) from fe80::18d6:da3a:cc8f:1b46 wpan0: 56 data bytes
64 bytes from fe80::4c7:6d8b:f8cd:9ada: icmp_seq=1 ttl=64 time=33.5 ms
64 bytes from fe80::4c7:6d8b:f8cd:9ada: icmp_seq=2 ttl=64 time=33.7 ms
64 bytes from fe80::4c7:6d8b:f8cd:9ada: icmp_seq=3 ttl=64 time=34.2 ms
64 bytes from fe80::4c7:6d8b:f8cd:9ada: icmp_seq=4 ttl=64 time=33.4 ms
--- fe80:0:0:0:4c7:6d8b:f8cd:9ada ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3004ms
rtt min/avg/max/mdev = 33.439/33.756/34.280/0.366 ms
$
```
