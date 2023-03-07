# RAIL - SoC Burst Duty Cycle

## Introduction

The Burst Duty Cycle example is a complete duty cycle solution implemented on
top of RAIL. This example implements both the Master (sender) and the Slave
(listener) nodes.

**In receive mode the device spends most of the time in sleep state (EM1 or EM2)
and wakes up periodically for short periods. In transmit mode the device
transmits multiple packets in quick succession. The default configuration
ensures that even in the worst-case scenario at least one listening period falls
into a burst packet transmission, so that the receiver will detect and receive
at least one of the transmitter's packet with high probability within detection
limit.**

This application can be used to optimize radio configuration and designing LDC
(Low Duty-cycle) periods on the receiver in order to save as much energy as
possible without PER (Packet Error Rate) reduction.

## Prerequisites

The Burst Duty Cycle example is supported on every EFR32 generations and
families that support proprietary software and the Duty Cycle/RX Channel Hopping
RAIL APIs.

Use the compile time symbol
[`RAIL_SUPPORTS_CHANNEL_HOPPING`](https://docs.silabs.com/rail/latest/group-features#gadcf2cddcac5cd29251a41acb7a2cb0ad)
or the runtime call
[`RAIL_SupportsChannelHopping()`](https://docs.silabs.com/rail/latest/group-features#ga4406c5dad879b047d6170e233ac23ee7)
to check whether the platform supports this feature.

## Getting started

Compile the project and download the application to two radio boards.

On startup the bitrate and the state timings will be displayed:

```
Burst Duty Cycle
Bitrate is 500000 b/s with 497000 us off time and 3000 us on time
```

The devices start in receive mode.

Issue `send` packet on the transmitter node to request a burst packet
transmission:

```
send
> Send packet request
> Burst of 834 packets sent.
```

The number of the transmitted packets may vary by burst transmissions.

On the receiver side the following response will be printed out on packet
reception:

```
> Packet has been received: 0x01, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 
```

The first byte in each packets is the burst ID increasing on each burst of
transmission.  

If the reception is enabled the device will automatically switch back to
reception either after a successful or faulty reception or a transmission.
Therefore, it could happen that multiple packets have been received on a
receiver node with the same burst ID. In that case only one packet will be
printed out from the received packets.

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

## Application Configuration

Though the duty cycle component cannot be added in the component editor, this
component is automatically installed in LDC example applications and generates a
configuration file (`sl_duty_cycle_config.h`) which is editable in a source code
editor.

The listening and sleeping periods are configured by the `DUTY_CYCLE_ON_TIME`
and `DUTY_CYCLE_OFF_TIME` macros respectively. The user must ensure that the
configured listening period is sufficiently long for the detection. Please,
contact technical support if you are not confident in this regards.

The transmitted burst period is calculated by the following equation:

```
BURST_TIME = DUTY_CYCLE_OFF_TIME + (2 * DUTY_CYCLE_ON_TIME)
```

The example optionally supports LCD on which the number of the received and
transmitted packets are shown. Also, packet transmission can be requested by
pressing the push button on the mainboard. These features could be turn off by
setting the `DUTY_CYCLE_USE_LCD_BUTTON`  to `0` in the configuration file.

If the `DUTY_CYCLE_ALLOW_EM2` is `1`, the device will spend the sleep period in
EM2 mode saving much more energy. Note that in EM2 state CLI might be
unresponsive.

The application is designed in a way, that it can be run on an OS. You can add
any RTOS support for this application in the `Software Component` window.

## Notes

> The application is designed in a way, that it can be run on an OS. You can add
> any RTOS support for this application in the `Software Components` window.
> Currently `MicriumOS` and `FreeRTOS` is supported.

> Due to the higher current consumption of the continuous radio usage
> (especially in TX Mode), it is not recommended to power the boards from a coin
> cell. Instead, an USB power bank can be used if portability is needed.

## Conclusion

The low duty cycle mode of the EFR32 radios is a simple and effective way to
reduce the current consumption of the system by autonomously waking up the
receiver to look for transmitted packets, while the MCU remains in its lowest
power mode.

## Resources

- [LDC in
  Theory](https://community.silabs.com/s/article/low-duty-cycle-mode?language=en_US)
- [RAIL
  documentation](https://docs.silabs.com/rail/latest/group-rx-channel-hopping)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
