# OpenThread on NXP K32W061 Example

This directory contains example platform drivers for the [NXP K32W0x1][k32w0x1]
based on [K32W0x1-DK006][k32w0x1-dk006] hardware platform, including all
variants of devices like K32W041 and K32W061.

The example platform drivers are intended to present the minimal code necessary
to support OpenThread. As a result, the example platform drivers do not
necessarily highlight the platform's full capabilities.

[k32w061]: https://www.nxp.com/products/wireless/thread/k32w061-41-high-performance-secure-and-ultra-low-power-mcu-for-zigbeethread-and-bluetooth-le-5-0-with-built-in-nfc-option:K32W061_41
[k32w061-dk006]: https://www.nxp.com/document/guide/getting-started-with-the-k32w061:GS-K32W061

## Prerequisites

Before you start building the examples, you must download and install the
toolchain and the tools required for flashing and debugging.

## Toolchain

OpenThread environment is suited to be run on a Linux-based OS.

In a Bash terminal (found, for example, in Ubuntu OS), follow these instructions
to install the GNU toolchain and other dependencies.

```bash
$ cd <path-to-ot-nxp>
$ ./script/bootstrap
```

For signing, several Python packages are required:

```bash
$ sudo apt-get install python3-pip
$ pip3 install pycrypto
$ pip3 install pycryptodome
```

## Tools

- Download and install the [MCUXpresso IDE][mcuxpresso ide].

[mcuxpresso ide]: https://www.nxp.com/support/developer-resources/software-development-tools/mcuxpresso-software-and-tools/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE

- Download [K32W061 SDK 2.6.6](https://mcuxpresso.nxp.com/). Creating an
  nxp.com account is required before being able to download the SDK. Once the
  account is created, login and follow the steps for downloading
  SDK_2.6.6_K32W061DK6. The SDK Builder UI selection should be similar with
  the one from the image below.
  ![MCUXpresso SDK Download](../../../doc/img/k32w/mcux-sdk-download.JPG)

## Building the examples

```bash
$ cd <path-to-ot-nxp>
$ export NXP_K32W0_SDK_ROOT=/path/to/previously/downloaded/SDK
$ ./script/build_k32w061
```

After a successful build, the `elf` files are found in
`build_k32w061/openthread/examples/apps/cli` and include FTD (Full Thread
Device), MTD (Minimal Thread Device) and variants of CLI appliations.

## Flash Binaries

If only flashing is needed then DK6 Flash Programmer can be used. Otherwise, if
also debugging capabilities are needed then MCUXpresso IDE should be used.

### Using DK6Programmer

Connect to the DK6 board by plugging a mini-USB cable to the connector marked
with _FTDI USB_. Also, make sure that jumpers jp4/JP7 are situated in the middle
position (_JN UART0 - FTDI_).

![DK6_FTDI](../../../doc/img/k32w/dk6_ftdi.jpg)

DK6 Flash Programmer can be found inside the [SDK][sdk_mcux] at path
`tools/JN-SW-4407-DK6-Flash-Programmer`. This is a Windows application that can
be installed using the .exe file. Once the application is installed, the COM
port for K32W061 must be identified:

```
C:\nxp\DK6ProductionFlashProgrammer>DK6Programmer.exe  --list
Available connections:
COM29
```

Once the COM port is identified, the required binary can be flashed:

```
C:\nxp\DK6ProductionFlashProgrammer>DK6Programmer.exe -s COM29 -p ot-rcp.bin
```

[sdk_mcux]: https://mcuxpresso.nxp.com/en/welcome

### Using MCUXpresso IDE

Connect to the DK6 board by plugging a mini-USB cable to the connector marked
with _TARGET_. Also, make sure that jumpers JP4/JP7 are situated in the leftmost
position (_LPC-JN UART0_).

![DK6_BOARD_FTDI](../../../doc/img/k32w/dk6_lpc.jpg)

In order to flash the application for debugging we recommend using
[MCUXpresso IDE (version >= 11.0.0)](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE?tab=Design_Tools_Tab).

- Import the previously downloaded NXP SDK into MCUXpresso IDE. This can be
  done by drag-and-dropping the SDK archive into MCUXpresso IDE's _Installed
  SDKs_ tab:

![Installed_SDKS](../../../doc/img/k32w/installed_sdks.JPG)

- Import OpenThread repo in MCUXpresso IDE as Makefile Project. Use _none_ as
  _Toolchain for Indexer Settings_:

```
File -> Import -> C/C++ -> Existing Code as Makefile Project
```

- Configure MCU Settings:

```
Right click on the Project -> Properties -> C/C++ Build -> MCU Settings -> Select K32W061 -> Apply & Close
```

![MCU_Sett](../../../doc/img/k32w/mcu_settings.JPG)

- Configure the toolchain editor:

```
Right click on the Project -> C/C++ Build-> Tool Chain Editor -> NXP MCU Tools -> Apply & Close
```

![MCU_Sett](../../../doc/img/k32w/toolchain.JPG)

- Create a debug configuration:

```
Right click on the Project -> Debug -> As->MCUXpresso IDE LinkServer (inc. CMSIS-DAP) probes -> OK -> Select elf file
```

![debug_1](../../../doc/img/k32w/debug_conf1.JPG)

- Set the _Connect script_ for the debug configuration to _QN9090connect.scp_
  from the dropdown list:

```
Right click on the Project -> Debug As -> Debug configurations... -> LinkServer Debugger
```

![connect](../../../doc/img/k32w/gdbdebugger.JPG)

- Set the _Initialization Commands_ to:

```
Right click on the Project -> Debug As -> Debug configurations... -> Startup

set non-stop on
set pagination off
set mi-async
set remotetimeout 60000
##target_extended_remote##
set mem inaccessible-by-default ${mem.access}
mon ondisconnect ${ondisconnect}
set arm force-mode thumb
${load}
```

![init](../../../doc/img/k32w/startup.JPG)

- Set the _vector.catch_ value to _false_ inside the .launch file:

```
Right click on the Project -> Utilities -> Open Directory Browser here -> edit *.launch file:

<booleanAttribute key="vector.catch" value="false"/>

```

- Debug using the newly created configuration file:

![debug](../../../doc/img/k32w/debug_start.JPG)

[cmsis-dap]: https://os.mbed.com/handbook/CMSIS-DAP

## Running the example

1. Prepare two boards with the flashed `CLI Example` (as shown above). Make sure
   that the JN4 jumper is set to RX and the JN7 jumper is set to TX, connecting
   the LPC and JN UART0 pins.
2. The CLI example uses UART connection. To view raw UART output, start a
   terminal emulator like PuTTY and connect to the used COM port with the
   following UART settings:

   - Baud rate: 115200
   - 8 data bits
   - 1 stop bit
   - No parity
   - No flow control

3. Open a terminal connection on the first board and start a new Thread network.

```bash
> panid 0xabcd
Done
> ifconfig up
Done
> thread start
Done
```

4. After a couple of seconds the node will become a Leader of the network.

```bash
> state
Leader
```

5. Open a terminal connection on the second board and attach a node to the
   network.

```bash
> panid 0xabcd
Done
> ifconfig up
Done
> thread start
Done
```

6. After a couple of seconds the second node will attach and become a Child.

```bash
> state
Child
```

7. List all IPv6 addresses of the first board.

```bash
> ipaddr
fdde:ad00:beef:0:0:ff:fe00:fc00
fdde:ad00:beef:0:0:ff:fe00:9c00
fdde:ad00:beef:0:4bcb:73a5:7c28:318e
fe80:0:0:0:5c91:c61:b67c:271c
```

8. Choose one of them and send an ICMPv6 ping from the second board.

```bash
> ping fdde:ad00:beef:0:0:ff:fe00:fc00
16 bytes from fdde:ad00:beef:0:0:ff:fe00:fc00: icmp_seq=1 hlim=64 time=8ms
```

For a list of all available commands, visit [OpenThread CLI Reference
README.md][cli].

[cli]: https://github.com/openthread/openthread/blob/main/src/cli/README.md
