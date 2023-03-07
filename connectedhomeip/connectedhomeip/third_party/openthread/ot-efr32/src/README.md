# OpenThread on EFR32

## Table of Contents

- [OpenThread on EFR32](#openthread-on-efr32)
  - [Table of Contents](#table-of-contents)
  - [Matching versions](#matching-versions)
  - [Prerequisites](#prerequisites)
    - [Toolchain](#toolchain)
    - [Flashing and debugging tools](#flashing-and-debugging-tools)
  - [Building examples](#building-examples)
  - [Flashing Binaries](#flashing-binaries)
    - [Simplicity Commander](#simplicity-commander)
      - [Download Links](#download-links)
    - [J-Link Commander](#j-link-commander)
  - [Run the example with EFR32 boards](#run-the-example-with-efr32-boards)
  - [Debugging with J-Link GDB Server](#debugging-with-j-link-gdb-server)
  - [Additional features](#additional-features)
  - [Verification](#verification)

---

This directory contains example platform drivers for the [Silicon Labs EFR32MG][efr32mg] based on [EFR32™ Mighty Gecko Wireless Starter Kit][slwstk6000b] or [Thunderboard™ Sense 2 Sensor-to-Cloud Advanced IoT Development Kit][sltb004a].

[efr32mg]: https://www.silabs.com/support/getting-started/mesh-networking/thread/mighty-gecko
[slwstk6000b]: http://www.silabs.com/products/development-tools/wireless/mesh-networking/mighty-gecko-starter-kit
[sltb004a]: https://www.silabs.com/products/development-tools/thunderboard/thunderboard-sense-two-kit

The example platform drivers are intended to present the minimal code necessary to support OpenThread. [EFR32MG][efr32mg] has rich memory and peripheral resources which can support all OpenThread capabilities.

See [EFR32 Sleepy Demo Example](../examples/sleepy-demo/README.md) for instructions for an example that uses the low-energy modes of the EFR32MG when running as a Sleepy End Device.

<a name="matching-versions"/>

## Matching versions

When using this repo to generate a solution that requires two different projects, such as an RCP & OTBR, make sure they are relying on the same OpenThread version to ensure maximum compatibility. You can check which commit this repo relies upon in the "openthread" submodule (in the root of this repo).

If your OTBR project was generated using the Silabs GSDK / Simplicity Studio, we recommend to also use it to generate the RCP project and not this repo. This will mitigate potential incompatibility issues due to mismatched OpenThread versions.

<a name="prerequisites"/>

## Prerequisites

<a name="toolchain"/>

### Toolchain

Download and install the [GNU toolchain for ARM Cortex-M][gnu-toolchain].

[gnu-toolchain]: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm

In a `bash` terminal, follow these instructions to install the GNU toolchain and other dependencies.

```bash
$ cd <path-to-ot-efr32>
$ ./script/bootstrap
```

<a name="flash-debug"/>

### Flashing and debugging tools

Install [Simplicity Studio][simplicity-studio] to flash, debug, and make use of logging features with SEGGER J-Link.

[simplicity-studio]: https://www.silabs.com/developers/simplicity-studio

Alternatively, the [J-Link][jlink-software-pack] software pack can be used to flash and debug.

[jlink-software-pack]: https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack

<a name="build"/>

## Building examples

Before building example apps, make sure to initialize all submodules. Afterward, the build can be launched using `./script/build`.

**Example**

The example below demonstrates how to build for `efr32mg12` on `brd4161a`, but the same command maybe used for other platforms and boards.

```bash
$ cd <path-to-ot-efr32>
$ git submodule update --init --recursive .
$ board="brd4161a"
$ ./script/build $board
...
-- Configuring done
-- Generating done
-- Build files have been written to: <path-to-ot-efr32>/build/<board>
+ [[ -n ot-rcp ot-cli-ftd ot-cli-mtd ot-ncp-ftd ot-ncp-mtd sleepy-demo-ftd sleepy-demo-mtd ]]
+ ninja ot-rcp ot-cli-ftd ot-cli-mtd ot-ncp-ftd ot-ncp-mtd sleepy-demo-ftd sleepy-demo-mtd
[572/572] Linking CXX executable bin/ot-ncp-ftd
+ cd <path-to-ot-efr32>
```

After a successful build, the `elf` files are found in `<path-to-ot-efr32>/build/<board>/bin`.

```bash
$ ls build/$board/bin
ot-cli-ftd      ot-cli-mtd      ot-ncp-ftd      ot-ncp-mtd      ot-rcp      sleepy-demo-ftd      sleepy-demo-mtd
ot-cli-ftd.s37  ot-cli-mtd.s37  ot-ncp-ftd.s37  ot-ncp-mtd.s37  ot-rcp.s37  sleepy-demo-ftd.s37  sleepy-demo-mtd.s37
```

<a name="flash"/>

## Flashing Binaries

Compiled binaries may be flashed onto the EFR32 using various tools from the [J-Link][j-link] software pack. EFR32 Starter kit mainboard integrates an on-board SEGGER J-Link debugger.

<a name="simplicity-commander"/>

### Simplicity Commander

Simplicity Commander provides a graphical interface for J-Link Commander. It's included as part of [Simplicity Studio][simplicity-studio] and is also available as a standalone application

#### Download Links

- [Linux](https://www.silabs.com/documents/public/software/SimplicityCommander-Linux.zip)
- [Mac](https://www.silabs.com/documents/public/software/SimplicityCommander-Mac.zip)
- [Windows](https://www.silabs.com/documents/public/software/SimplicityCommander-Windows.zip)

```bash
$ <path-to-simplicity-studio>/developer/adapter_packs/commander/commander
```

1. In the J-Link Device drop-down list select the serial number of the device to flash.
2. Click the Adapter Connect button.
3. Ensure the Debug Interface drop-down list is set to SWD and click the Target Connect button.
4. Click on the Flash icon on the left side of the window to switch to the flash page.
5. In the Flash MCU pane, enter the path of the `ot-cli-ftd.s37` file or choose the file with the Browse... button.
6. Click the Flash button located under the Browse... button.

For more information see [UG162: Simplicity Commander Reference][ug162]

[ug162]: https://www.silabs.com/documents/public/user-guides/ug162-simplicity-commander-reference-guide.pdf

<a name="jlink-commander"/>

### J-Link Commander

Compiled binaries also may be flashed onto the specified EFR32 dev board using [J-Link Commander][j-link-commander].

[j-link-commander]: https://www.segger.com/products/debug-probes/j-link/tools/j-link-commander/

**Example:** Flashing `ot-cli-ftd` to a `brd4161a` device

```bash
$ cd <path-to-ot-efr32>
$ source ./script/efr32-definitions
$ board="brd4161a"
$ cd <path-to-ot-efr32>/build/$board/bin
$ arm-none-eabi-objcopy -O ihex ot-cli-ftd ot-cli-ftd.hex
$ <path-to-JLinkGDBServer>/JLinkExe -device $(efr32_get_jlink_device $board) -speed 4000 -if SWD -autoconnect 1 -SelectEmuBySN <SerialNo>
$ J-Link>loadfile ot-cli-ftd.hex
$ J-Link>r
$ J-Link>q
```

**Note**: `SerialNo` is J-Link serial number. Use the following command to get the serial number of the connected J-Link.

```bash
$ JLinkExe
```

<a name="example"/>

## Run the example with EFR32 boards

1. Flash two EFR32 boards with the `CLI example` firmware (as shown above).
2. Open terminal to first device `/dev/ttyACM0` (serial port settings: 115200 8-N-1). Type `help` for a list of commands.

   ```bash
   > help
   help
   channel
   childtimeout
   contextreusedelay
   extaddr
   extpanid
   ipaddr
   keysequence
   leaderweight
   mode
   netdata register
   networkidtimeout
   networkkey
   networkname
   panid
   ping
   prefix
   releaserouterid
   rloc16
   route
   routerupgradethreshold
   scan
   start
   state
   stop
   ```

3. Start a Thread network as Leader.

   ```bash
   > dataset init new
   Done
   > dataset
   Active Timestamp: 1
   Channel: 13
   Channel Mask: 0x07fff800
   Ext PAN ID: d63e8e3e495ebbc3
   Mesh Local Prefix: fd3d:b50b:f96d:722d::/64
   Network Key: dfd34f0f05cad978ec4e32b0413038ff
   Network Name: OpenThread-8f28
   PAN ID: 0x8f28
   PSKc: c23a76e98f1a6483639b1ac1271e2e27
   Security Policy: 0, onrcb
   Done
   > dataset commit active
   Done
   > ifconfig up
   Done
   > thread start
   Done

   wait a couple of seconds...

   > state
   leader
   Done
   ```

4. Open terminal to second device `/dev/ttyACM1` (serial port settings: 115200 8-N-1) and attach it to the Thread network as a Router.

   ```bash
   > dataset networkkey dfd34f0f05cad978ec4e32b0413038ff
   Done
   > dataset commit active
   Done
   > routerselectionjitter 1
   Done
   > ifconfig up
   Done
   > thread start
   Done

   wait a couple of seconds...

   > state
   router
   Done
   ```

5. List all IPv6 addresses of Leader.

   ```bash
   > ipaddr
   fd3d:b50b:f96d:722d:0:ff:fe00:fc00
   fd3d:b50b:f96d:722d:0:ff:fe00:c00
   fd3d:b50b:f96d:722d:7a73:bff6:9093:9117
   fe80:0:0:0:6c41:9001:f3d6:4148
   Done
   ```

6. Send an ICMPv6 ping to Leader's Mesh-EID IPv6 address.

   ```bash
   > ping fd3d:b50b:f96d:722d:7a73:bff6:9093:9117
   16 bytes from fd3d:b50b:f96d:722d:558:f56b:d688:799: icmp_seq=1 hlim=64 time=24ms
   ```

For a list of all available commands, visit [OpenThread CLI Reference][cli].

[cli]: https://github.com/openthread/openthread/blob/main/src/cli/README.md

<a name="debugging-jlink-gdb-server"/>

## Debugging with J-Link GDB Server

A debug session may be started with [J-LinkGDBServer][jlinkgdbserver].

[jlinkgdbserver]: https://www.segger.com/jlink-gdb-server.html

**Example:** Debugging `ot-cli-ftd` on a `brd4161a` device

```bash
$ source <path-to-ot-efr32>/script/efr32-definitions
$ board="brd4161a"
$ cd <path-to-JLinkGDBServer>
$ sudo ./JLinkGDBServer -if swd -singlerun -device $(efr32_get_jlink_device $board)
$ cd <path-to-ot-efr32>/build/$board/bin
$ arm-none-eabi-gdb ot-cli-ftd
$ (gdb) target remote 127.0.0.1:2331
$ (gdb) load
$ (gdb) monitor reset
$ (gdb) c
```

<a name="additional-features"/>

## Additional features

The above example demonstrates basic OpenThread capabilities. Enable more features/roles (e.g. commissioner, joiner, DHCPv6 Server/Client, etc.) by assigning compile-options before compiling.

**Example** Building efr32mg12 for board `brd4161a` with some more features/roles enabled

```bash
$ cd <path-to-ot-efr32>
$ ./script/build brd4161a -DOT_COMMISSIONER=ON -DOT_JOINER=ON -DOT_DHCP6_CLIENT=ON -DOT_DHCP6_SERVER=ON
```

<a name="verification"/>

## Verification

The following toolchain has been used for testing and verification:

- gcc version 7.3.1

The EFR32 example has been verified with following Silicon Labs Gecko SDK Library version:

- Silicon Labs Gecko SDK v4.1.x
