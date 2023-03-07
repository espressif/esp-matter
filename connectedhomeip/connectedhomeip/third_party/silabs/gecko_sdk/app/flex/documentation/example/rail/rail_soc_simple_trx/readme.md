# RAIL - SoC Simple TRX

## Introduction

The Simple TRX example demonstrates the simplest exchange of transmit and
receive operation between two nodes implemented in RAIL.

This application can be used for setting up a simple link test, where the nodes
are listening except for the short periods where packet transmission takes
place. Also, this application can be used as a learning material.

## Prerequisites

The Simple TRX example is supported on every EFR32 generations and families that
support proprietary software except EFR32xG21 devices.

## Getting Started

This example implements both the receiver (RX) and the transmitter (TX) nodes.
The implementation is therefore symmetric, as both nodes are capable of sending
and receiving messages. In this document the use of `RX node` and `TX node` are
implicit and these terms refer the devices temporary, while the denoted
operations are performed.

----

Compile the project and download the application to two radio boards.

----

On startup the application's name will be displayed:

```
> Simple TRX
```

The devices start in receive state.

Issue the `send` command on the TX node to request packet transmission:

```
send
> Send packet request
> Packet has been sent
```

On the receiver side the following response will be printed out on packet
reception:

```
> Packet has been received: 0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 
```

If the reception is enabled the device will automatically switch back to RX mode
either after a successful or faulty reception or a transmission.

----

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

The `RAIL - SoC Simple TRX` example application serves as reference to get
the basics how proprietary packet exchange works in real-life implemented in
RAIL. Apart from using it on Silicon Labs example kits for a quick link test,
there is little use of starting your application from this example, rather you
might reuse some part of the implementation of this code by copying into your
project. We recommend to use `RAIL - SoC Empty` as starting project
instead.

## Resources

- [RAIL Tutorial
  Series](https://community.silabs.com/s/article/rail-tutorial-series?language=en_US):
  it is advised to read through the `Studio v5 series` first to familiarize the
  basics of packet transmission and reception

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
