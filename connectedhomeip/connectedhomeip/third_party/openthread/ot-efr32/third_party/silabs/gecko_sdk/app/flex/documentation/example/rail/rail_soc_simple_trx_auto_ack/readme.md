# RAIL - SoC Simple TRX with Auto-ACK

## Introduction

The Simple TRX with Auto-ACK example demonstrates how RAIL's Auto-ACK feature
works.

This application can be used  for setting up a simple link test which requires
Auto-ACK functionality. Also, this application can be used as a learning
material.

## Prerequisites

The Simple TRX with Auto-ACK example is supported on every EFR32 generations and
families that support proprietary software except EFR32xG21 devices.

## Getting Started

This example implements both the receiver (RX) and the transmitter (TX) nodes.
The implementation is therefore symmetric, as both nodes are capable of sending
and receiving normal and ACK messages. However, in this document we will refer
the device, which initializes transmission and waiting for an ACK packet as `TX
node`, whereas `RX node` listens for packets and sends ACK packets in response.
These terms therefore mark the nodes temporary.

----

Compile the project and download the application to two radio boards.

On startup the application's name will be displayed:

```
> Simple TRX with Auto-ACK
```

The devices start in receive state.

Issue the `send` command on the TX node to request packet transmission:

```
send
> Send packet request
> Packet has been sent
```

### Possible Scenarios after Initiating a Transmission

From this point multiple scenarios might happen.

1. RX node successfully receives the packet

If the RX node had been set up, and receives the packet, it sends automatically
an ACK packet in response.

The RX node prints out the following message:

```
> Packet has been received: 0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 
```

If the TX device was able to receive the ACK packet, it will notify the user:

```
> ACK was received
```

After the ACK-packet exchange both device will go back to RX state.

2. The TX's packet will be missed

The TX device's packet can be missed if

- on the RX device the reception is disabled, or
- an RX error happens.

In those cases no further notification messages will be displayed.

> Since addressing is not used by this example, if there are multiple devices
> listening, each of the RX nodes might send ACK packets, which might collide
> and the TX node won't receive any of them.

----

If the reception is enabled the device will automatically switch back to RX mode

- either after an ACK packet transmission or faulty reception on the RX node, or
- after the ACK packet reception/timeout on the TX node.

If the reception is disabled, the TX node will go the RX state listening for the
ACK-packet, but once the ACK-packet is received or the defined timeout period
expires the receive mode turns down.

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
(`led0` and `led1`) installed in the Project's Component Editor, which is the
default creating this application for Silicon Labs radio boards.

Below we discuss these peripheral functions when the application is running on a
Silicon Labs development kit (E.g., mainboard + radio board).

- Pressing the PB0/PB1 push buttons on the development kit initiates packet
  transmission, just like the `send` CLI command.
- Toggles on LED0 indicate that a normal packet reception on the RX node was
  successful.
- Toggles on LED1 indicate that an ACK-packet reception on the TX node was
  successful.

## What This Example Shouldn't Be Used For

In the following scenarios it is not advised to use this example:

- Radio config validation: since this application has strict limitations, if you
  have to evaluate a newly generated radio configuration you might want to use
  `RAIL - SoC RAILtest` example to validate your PHY(s) as an alternative.
- Radio range testing: we have a complete solution for testing the radio's
  performance if the two devices are placed far to each other. In such cases use
  the `RAIL - SoC Range Test` example application instead.
- Automatic ACK with IEEE 802.15.4 framing: Since Auto ACK with 15.4 requires a
  radio config with 15.4 frame configuration, 15.4 ACK is implemented with
  slightly different APIs that are demonstrated in `RAIL - SoC Simple TRX
  Standards`.

## Notes

> Due to this application uses a strict workflow, it is not recommended to
> program more than 2 devices with the same example: it might lead unspecified
> results.

> The application is designed in a way, that it can be run on an OS. You can add
> any RTOS support for this application in the `Software Components` window.
> Currently `MicriumOS` and `FreeRTOS` is supported.

> Due to the higher current consumption of the continuous radio usage
> (especially in RX Mode), it is not recommended to power the boards from a coin
> cell. Instead, an USB power bank can be used if portability is needed.

## Conclusion

The `RAIL - SoC Simple TRX with Auto-ACK` example application serves as
reference to get the basics how the RAIL's Auto-ACK feature works in real-life.
Apart from using it on Silicon Labs example kits for a quick Auto-ACK link test,
there is little use of starting your application from this example, rather you
might reuse some part of the implementation of this code by copying into your
project. We recommend to use `RAIL - SoC Empty` as starting project
instead.  

## Resources

- [RAIL Tutorial
  Series](https://community.silabs.com/s/article/rail-tutorial-series?language=en_US):
  it is advised to read through the `Studio v5 series` first to familiarize the
  basics of packet transmission and reception
- [RAIL documentation on
  Auto-ACK](https://docs.silabs.com/rail/latest/group-auto-ack)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
