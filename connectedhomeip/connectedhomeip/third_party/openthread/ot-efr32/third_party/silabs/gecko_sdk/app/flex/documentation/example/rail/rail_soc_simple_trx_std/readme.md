# RAIL - SoC Simple TRX Standards

## Introduction

The Simple TRX Standards example demonstrates the simplest packet exchange of
transmit and receive operation between two nodes implemented in RAIL compatible
with BLE, IEEE 802.15.4 (2.4 GHz only) and IEEE 802.15.4g (subGHz only)
standards.

This application can be used for setting up a simple link test, where the nodes
are listening except for the short periods where packet transmission takes
place. Also, this application can be used as a learning material.

## Prerequisites

The Simple TRX Standards example is supported on every EFR32 generations and
families that support proprietary software. However, the supported standards
might differ by device generations or by the particular radio's PA capabilities:
for the supported protocols consult with the device's datasheet or the radio
board's user guide.

## Getting started

This example implements both the receiver (RX) and the transmitter (TX) nodes.
The implementation is therefore symmetric, as both nodes are capable of sending
and receiving messages. In this document the use of `RX node` and `TX node` are
implicit and these terms refer the devices temporary, while the denoted
operations are performed.

----

The protocol can be selected after the project creation. If your target device
supports both IEEE 802.15.4(g) and BLE modes you can choose between the `Flex -
RAIL BLE support` or `Flex - RAIL 802.15.4 support` components in the `Software
Components` window.

Issue `info` command to get the `Radio type`. The following table helps to
decode which protocols are supported on the your device.

| Radio type | 802.15.4 | 802.15.4g | BLE |
|:----------:|:--------:|:---------:|:---:|
|   2P4GHz   |     X    |     -     |  X  |
|   SubGHz   |     -    |     X     |  -  |
|  Dual band |     X    |     X     |  X  |

The protocol selection happens compile-time, so switching between BLE and IEEE
802.15.4(g) modes is not supported once the application is downloaded onto your
device. However, on dual band boards runtime switch between 802.15.4 and
802.15.4g is possible as it is discussed later.

The startup protocol can be found in the in the `sl_flex_802154_init_config.h`
or the `sl_flex_util_ble_init_config.h` files.

----

Compile the project and download the application to two radio boards.

----

### IEEE 802.15.4(g)

#### Supported Features

With IEEE 802.15.4 the frame can be sent and received with CSMA/CA, auto-ACK,
address filtering and configurable Frame Control Frame (available settings
broadcast or unicast).

With IEEE 802.15.4g the frame can be sent and received with auto-ACK, address
filtering and configurable Frame Control Frame (available settings broadcast or
unicast).

----

On startup the name of the application and the selected protocol will be
displayed:

You can switch between IEEE 802.15.4 and IEEE 802.15.4g modes with the `std`
command. Issue `help` command for the available arguments. 

```
> Simple TRX IEEE 802.15.4
```

Issue the `send` command on the TX node to request packet transmission:

```
send
> Send packet request
> Packet has been sent
```

On the receiver side the following response will be printed out on packet
reception if IEEE 802.15.4 standard is selected:

```
> MHR field:
>  Frame control:     0x9841
>  Seq. number:       0x01
>  PAN ID:            0xFFFF
>  Dest. address:     0xFFFF
>  Src address:       0x0000
> Payload: 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 
```

If the IEEE 802.15.4g protocol has been selected the following fields will be
displayed:

```
IEEE 802.15.4g 915MHz standard set.
> PHR DW,CRC cfg (0x08):
>  Whitening:         OFF
>  CRC length:        2B
> MHR field:
>  Frame control:     0x9841
>  Seq. number:       0x01
>  PAN ID:            0xFFFF
>  Dest. address:     0xFFFF
>  Src address:       0x0000
> Payload: 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 
```

The addresses and the PAN ID can be customized via CLI commands. For the
complete list of the available commands issue the `help` command:

```
help
  info                          Unique ID of the MCU
  send                          Send a packet
  receive                       Turn ON/OFF received message forwarding on CLI
                                [uint8] ON/OFF
  std                           Sets the Standard (IEEE 802.15.4 or IEEE 802.15.4g)
                                [string] b is (basic) IEEE 802.15.4, g is IEEE 802.15.4g
                                [stringopt] IEEE 802.15.4g (863 is 863MHz, 915 is 915MHz)
  panid                         Sets the PAN ID
                                [uint16] PAN ID in hex form
  srcaddr                       Sets the short address of source
                                [uint16] Short address in hex form
  destaddr                      Sets the short address of destination
                                [uint16] Short address in hex form
  ackreq                        ACK is requested or not
                                [uint8] 0 is NOT requested, 1 is requested
  configdw                      Configures the IEEE Std. 802.15.4g whitening
                                [uint8] 0 -> Data whitening OFF, 1 -> Data whitening ON
  configcrc                     Configures the IEEE Std. 802.15.4g CRC
                                [uint8] 2 -> CRC 2 bytes, 4 -> CRC 4 bytes
```

----

If the TX node receives an ACK packet, the device will notify the user with the
following message:

```
> ACK received
> ACK: 0x08, 0xA0, 0x02, 0x00, 0x01, 
```

This can be enabled or disable by the `ackreq` CLI command:

```
ackreq 1
> ACK is requested
(...)
ackreq 0
> ACK is NOT requested
```

### BLE

On startup the name of the application and the selected protocol will be
displayed:

```
> Simple TRX BLE
```

Issue the `send` command on the TX node to request packet transmission:

```
send
> Send packet request
> BLE Packet: (size = 33) (payload_len = 17) {
> [0] -> 0x02
> [1] -> 0x1F
> [2] -> 0xC1
> [3] -> 0x29
> [4] -> 0xD8
> [5] -> 0x57
> [6] -> 0x0B
> [7] -> 0x00
> [8] -> 0x02
> [9] -> 0x01
> [10] -> 0x06
> [11] -> 0x15
> [12] -> 0xFF
> [13] -> 0xFF
> [14] -> 0x02
> [15] -> 0x01
> [16] -> 0x00
> [17] -> 0x01
> [18] -> 0x02
> [19] -> 0x03
> [20] -> 0x04
> [21] -> 0x05
> [22] -> 0x06
> [23] -> 0x07
> [24] -> 0x08
> [25] -> 0x09
> [26] -> 0x0A
> [27] -> 0x0B
> [28] -> 0x0C
> [29] -> 0x0D
> [30] -> 0x0E
> [31] -> 0x0F
> [32] -> 0x10
> }
> Packet has been sent
```

On the receiver side the following response will be printed out on packet
reception:

```
> BLE Packet: (size = 33) (payload_len = 17) {
> [0] -> 0x02
> [1] -> 0x1F
> [2] -> 0xC1
> [3] -> 0x29
> [4] -> 0xD8
> [5] -> 0x57
> [6] -> 0x0B
> [7] -> 0x00
> [8] -> 0x02
> [9] -> 0x01
> [10] -> 0x06
> [11] -> 0x15
> [12] -> 0xFF
> [13] -> 0xFF
> [14] -> 0x02
> [15] -> 0x01
> [16] -> 0x00
> [17] -> 0x01
> [18] -> 0x02
> [19] -> 0x03
> [20] -> 0x04
> [21] -> 0x05
> [22] -> 0x06
> [23] -> 0x07
> [24] -> 0x08
> [25] -> 0x09
> [26] -> 0x0A
> [27] -> 0x0B
> [28] -> 0x0C
> [29] -> 0x0D
> [30] -> 0x0E
> [31] -> 0x0F
> [32] -> 0x10
> }
> BLE Packet has been received. Payload (17): 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 
```

The transmitted packet is a typical BLE advertising frame.

----

If the reception is enabled the device will automatically switch back to RX mode
either after a successful or faulty reception or a transmission.

If the reception is disabled and auto-ACK is requested (available only for IEEE
802.15.4 and IEEE 802.15.4g modes), the TX node will go the RX state listening
for the ACK-packet, but once the ACK-packet is received or the defined timeout
period expires the receive mode turns down.

----

The devices start in receive state.

The reception can be disabled by the `receive` command.

```
receive 0
> Received packets: OFF
```

Receive mode can be enabled again by the same command:

```
receive 1
> Received packets: ON
```

## Peripherals

This example requires a `Simple Button` instance and two `Simple LED` instances
(`led0` and `led1`) installed in the Project's `Software Components` window,
which is the default creating this application for Silicon Labs radio boards.

Below we discuss these peripheral functions when the application is running on a
Silicon Labs development kit (E.g., mainboard + radio board).

- Pressing the PB0/PB1 push buttons on the development kit initiates packet
  transmission, just like the `send` CLI command.
- Toggles on LED0 indicate that a packet reception on the RX node was
  successful.
- Toggles on LED1 indicate that a packet transmission on the TX node was
  successful.

## What This Example Shouldn't Be Used For

In the following scenarios it is not advised to use this example:

- Radio config validation: since this application has strict limitations, if you
  have to evaluate a newly generated radio configuration you might want to use
  `RAIL - SoC RAILtest` example to validate your PHY(s) as an alternative.
- Radio range testing: we have a complete solution for testing the radio's
  performance if the two devices are placed far to each other. In such cases use
  the `RAIL - SoC Range Test` example application instead.

## Notes

> The application is designed in a way, that it can be run on an OS. You can add
> any RTOS support for this application in the `Software Components` window.
> Currently `MicriumOS` and `FreeRTOS` is supported.

> Due to the higher current consumption of the continuous radio usage
> (especially in RX Mode), it is not recommended to power the boards from a coin
> cell. Instead, an USB power bank can be used if portability is needed.

## Conclusion

The `RAIL - SoC Simple TRX Standards` example application serves as reference
to get the basics how IEEE 802.15.4(g) or BLE packet exchange works in real-life
implemented in RAIL. Apart from using it on Silicon Labs example kits for a
quick link test, there is little use of starting your application from this
example, due to it's complexity, rather you might reuse some part of the
implementation of this code by copying into your project. We recommend to use
`RAIL - SoC Empty` as starting project instead.

## Resources

- [RAIL Tutorial
  Series](https://community.silabs.com/s/article/rail-tutorial-series?language=en_US):
  it is advised to read through the `Studio v5 series` first to familiarize the
  basics of packet transmission and reception
- [RAIL Documentation: IEEE 802.15.4 Protocol Specific
  APIs](https://docs.silabs.com/rail/latest/group-i-e-e-e802-15-4)
- [RAIL Documentation: BLE Protocol Specific
  APIs](https://docs.silabs.com/rail/latest/group-b-l-e)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
