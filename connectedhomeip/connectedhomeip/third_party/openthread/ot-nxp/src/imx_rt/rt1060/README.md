# OpenThread on NXP RT1060 (host) + K32W061 (rcp) example (Experimental support)

This directory contains example platform drivers for the [NXP RT1060][rt1060]
platform.

The example platform drivers are intended to present the minimal code necessary
to support OpenThread. As a result, the example platform drivers do not
necessarily highlight the platform's full capabilities.

[rt1060]: https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/i-mx-rt-crossover-mcus/i-mx-rt1060-crossover-mcu-with-arm-cortex-m7-core:i.MX-RT1060

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

## Tools

- Download and install the [MCUXpresso IDE][mcuxpresso ide].

[mcuxpresso ide]: https://www.nxp.com/support/developer-resources/software-development-tools/mcuxpresso-software-and-tools/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE

- Download [IMXRT1060 SDK 2.10.1](https://mcuxpresso.nxp.com/). Creating an
  nxp.com account is required before being able to download the SDK. Once the
  account is created, login and follow the steps for downloading
  SDK_2.10.1_EVK-MIMXRT1060. In the SDK Builder UI selection you should select
  the **FreeRTOS component**, the **BT/BLE component** and the **ARM GCC
  Toolchain**.

## Building the examples

```bash
$ cd <path-to-ot-nxp>
$ export NXP_RT1060_SDK_ROOT=/path/to/previously/downloaded/SDK
$ ./script/build_rt1060
```

After a successful build, the ot-cli-rt1060 FreeRTOS version could be found in
`build_rt1060` and include FTD (Full Thread Device).

Note: FreeRTOS is required to be able to build the IMXRT1060 platform files.

## Hardware requirements

Host part:

- 1 EVK-MIMXRT1060

Transceiver part:

- 1 OM15076-3 Carrier Board (DK6 board)
- 1 K32W061 Module to be plugged on the Carrier Board

## Board settings

The below table explains pin settings (UART settings) to connect the
evkmimxrt1060 (host) to a k32w061 transceiver (rcp).

| PIN NAME | DK6 (K32W061) | I.MXRT1060 | PIN NAME OF RT1060 | GPIO NAME OF RT1060 |
| :------: | :-----------: | :--------: | :----------------: | :-----------------: |
| UART_TXD |  PIO, pin 8   | J22, pin 1 |    LPUART3_RXD     |    GPIO_AD_B1_07    |
| UART_RXD |  PIO, pin 9   | J22, pin 2 |    LPUART3_TXD     |    GPIO_AD_B1_06    |
| UART_RTS |  PIO, pin 6   | J23, pin 3 |    LPUART3_CTS     |    GPIO_AD_B1_04    |
| UART_CTS |  PIO, pin 7   | J23, pin 4 |    LPUART3_RTS     |    GPIO_AD_B1_05    |

The below picture shows pins connections.

![rt1060_k32w061_pin_settings](../../../doc/img/imxrt1060/rt1060_k32w061_pin_settings.jpg)

Note: it is recommended to first
[flash the K32W061 OT-RCP transceiver image](#Flashing-the-K32W061-OT-RCP-transceiver-image)
before connecting the DK6 to the IMXRT1060.

## Flash Binaries

### Flashing the K32W061 OT-RCP transceiver image

Connect to the DK6 board by plugging a mini-USB cable to the connector marked
with _FTDI USB_. Also, make sure that jumpers jp4/JP7 are situated in the middle
position (_JN UART0 - FTDI_).

DK6 Flash Programmer can be found inside the [SDK][sdk_mcux] SDK_EVK-MIMXRT1060
previously downloaded at path
`<sdk_path>/middleware/wireless/ethermind/port/pal/mcux/bluetooth/controller/k32w061/JN-SW-4407-DK6-Flash-Programmer`.
This is a Windows application that can be installed using the .exe file. Once
the application is installed, the COM port for K32W061 must be identified:

```
C:\nxp\DK6ProductionFlashProgrammer>DK6Programmer.exe  --list
Available connections:
COM29
```

The ot-rcp image has to be built. For that, follow the [K32W061
Readme][k32w061-readme]. The new K32W061 ot-rcp binary will be located in
`ot-nxp/build_k32w061/rcp_only_uart_flow_control/openthread/examples/apps/ncp/ot-rcp.bin`.

Once the COM port is identified, the required binary can be flashed:

[k32w061-readme]: ../../k32w0/k32w061/README.md

```
C:\nxp\DK6ProductionFlashProgrammer>DK6Programmer.exe -s COM29 -p "<ot_rcp_path>\ot-rcp.bin"
```

[sdk_mcux]: https://mcuxpresso.nxp.com/en/welcome

### Flashing the IMXRT ot-cli-rt1060 host image using MCUXpresso IDE

In order to flash the application for debugging we recommend using
[MCUXpresso IDE (version >= 11.3.1)](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE?tab=Design_Tools_Tab).

- Import the previously downloaded NXP SDK into MCUXpresso IDE. This can be
  done by drag-and-dropping the SDK archive into MCUXpresso IDE.

- Import ot-nxp repo in MCUXpresso IDE as Makefile Project. Use _none_ as
  _Toolchain for Indexer Settings_:

```
File -> Import -> C/C++ -> Existing Code as Makefile Project
```

- Configure MCU Settings:

```
Right click on the Project -> Properties -> C/C++ Build -> MCU Settings -> Select MIMXRT1060 -> Apply & Close
```

![MCU_Sett](../../../doc/img/imxrt1060/mcu_settings.JPG)

- Configure the toolchain editor:

```
Right click on the Project -> C/C++ Build-> Tool Chain Editor -> NXP MCU Tools -> Apply & Close
```

![MCU_Sett](../../../doc/img/k32w/toolchain.JPG)

- Create a debug configuration:

To create a new debug configuration for our application, we will duplicate an
existing debug configaturation.

- Create a debug configuration for the hello word projet

1. Click on "Import SDK example(s)..." on the bottom left window of MCUXpresso.
2. Select the "evkmimxrt1060" SDK, click on "Next"
3. Expand "demo_apps", select the "hello_word" example, click on next and then
   finish.
4. Build the imported "Hello word" application by right clicking on the project
   and select "Build Project".
5. Right click again on the project and select "Debug As" and click on
   "MCUXpresso IDE LinkServer" option. Doing this will flash the application on
   the board. Then click on the red "Terminate" button.

- Duplicate the hello word debug configaturation to create a new debug
  configuration for the ot_cli

1. Right click on the "Hello Word" project, select "Debug As" and then select
   "Debug Configurations".
2. Right click on the "Hello Word" debug configuration and click on "Duplicate".
3. Rename the Duplicated debug configuration "ot-cli".
4. In the "C/C++ Application", click on "Browse" and select the ot-cli-rt1060
   app (should be located in "ot-nxp/build_rt1060/ot-cli-rt1060"). Then click on
   Apply and Save.
5. Click on "Organize Favorites".
   ![MCU_Sett](../../../doc/img/imxrt1060/organize_favorites.png)
6. Add the ot-cli debug configuration
7. Run the ot-cli debug configuration

[cmsis-dap]: https://os.mbed.com/handbook/CMSIS-DAP

## Running the example

1. The CLI example uses UART connection. To view raw UART output, start a
   terminal emulator like PuTTY and connect to the used COM port with the
   following UART settings (on the IMXRT1060):

   - Baud rate: 115200
   - 8 data bits
   - 1 stop bit
   - No parity
   - No flow control

2. Follow the process describe in [Interact with the OT CLI][validate_port].

[validate_port]: https://openthread.io/guides/porting/validate-the-port#interact-with-the-cli

For a list of all available commands, visit [OpenThread CLI Reference
README.md][cli].

[cli]: https://github.com/openthread/openthread/blob/master/src/cli/README.md
