# RAIL - SoC Long Preamble Duty Cycle

## Introduction

The Long Preamble Duty Cycle example is a complete duty cycle solution
implemented on top of RAIL. This example supports both receive and transmit
modes.

**In receive mode the device spends most of the time in a sleep state (EM1 or
EM2) and wakes up periodically for a short periods. In transmit mode the device
transmits one packet using long preamble. The default configuration ensures that
even in the worst case scenario at least one listening period falls into a
preamble-transmitting period, so the receiver will detect and receive the
transmitter's packet with high probability within detection limit.**

This application can be used to design LDC (Low Duty-cycle) periods on the
receiver in order to save as much energy as possible without PER (Packet Error
Rate) reduction.

## Prerequisites

The Long Preamble Duty Cycle example is supported on every EFR32 generations and
families that support proprietary software and the Duty Cycle/RX Channel Hopping
RAIL APIs.

Use the compile time symbol
[`RAIL_SUPPORTS_CHANNEL_HOPPING`](https://docs.silabs.com/rail/latest/group-features#gadcf2cddcac5cd29251a41acb7a2cb0ad)
or the runtime call
[`RAIL_SupportsChannelHopping()`](https://docs.silabs.com/rail/latest/group-features#ga4406c5dad879b047d6170e233ac23ee7)
to check whether the platform supports this feature.

## Getting Started

Compile the project and download the application to two radio boards.

On startup the transmitted preamble length, the bitrate and the off time will be
displayed:

```
Long Preamble Duty Cycle
Preamble length 50000 for bitrate 500000 b/s with 98000 us off time
```

----

You may encounter two kind of misconfiguration on startup:

1. Detection time is lower than the configured on time;
2. Transmitted preamble length cannot be configured on radio configuration
   level.

In case of 1. the application will throw an assert error, and it is needed to
increase the on time manually in the application configuration.

You'll get assert message like below:

```
C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/app/flex/component/rail/sl_duty_cycle_core/sl_duty_cycle_utility.c:82 :calculate_preamble_bit_length_from_time: Assertion 'preamble_time < on_time' failed: Please modify the on time according to the bitrate!
```

In case of 2. the application can manually adjust the preamble length to fit
into the configurable preamble range. This range can be vary by platforms and
the preamble configuration. Note that it is advised to decrease the off time
manually.

You'll get a notification when this happens, but the program does not stop
running:

```
Duty Cycle Off time was changed to ensure stable working
```

----

Transmitting a packet can be done by pressing a button or issuing `send` CLI
command:

```
send
> Send packet request
```

On the receiver side the following response will be printed out on packet
reception:

```
> Packet has been received: 0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 
```

If the reception is enabled the device will automatically switch back to
reception either after a successful or faulty reception or a transmission.

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

You can monitor the current consumption on the Energy Profiler extension though
it is not reliable for doing accurate measurements (mainboard leakage).

## Application Configuration

Though the duty cycle component cannot be added in the component editor, this
component is automatically installed in LDC example applications and generates a
configuration file (`sl_duty_cycle_config.h`) which is editable in a source code
editor.

The listening and sleeping periods are configured by the `DUTY_CYCLE_ON_TIME`
and `DUTY_CYCLE_OFF_TIME` macros respectively. The user must ensure that the
configured listening period is sufficiently long for the detection. Please,
consult with the referred article below or contact technical support if you are
not confident in this regards.

The transmitted preamble period is calculated by the following equation:

```
PREAMBLE_TIME = DUTY_CYCLE_OFF_TIME + (2 * DUTY_CYCLE_ON_TIME)

PREAMBLE_BIT_LENGTH = PREAMBLE_TIME * BITRATE
```

If the calculated `PREAMBLE_BIT_LENGTH` overflows the maximum preamble length
configurable on radio register-level, it will be decreased automatically as it
was discussed above.

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

> On most EFR32 radio platforms the application is initialized to work in
> preamble detection mode. However, different detection method may be preferred
> for particular EFR32 generations resulting in shorter listening period.
    
> On EFR32xG23 devices the `RAIL_GetDefaultRxDutyCycleConfig()` API might
> override the detection method to `RAIL_RX_CHANNEL_HOPPING_MODE_SQ` for certain
> radio configurations.

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
