# RAIL - SoC Energy Mode

## Introduction

The Energy Mode example application demonstrates the most essential flavors on
how to create a low-energy application on top of RAIL. Also, this application
serves as a complete, ready-to-use program, with which you can accelerate the
evaluation of the of energy consumption on your custom radio design by iterating
over the available energy modes or using TX/RX radio operations.

## Prerequisites

The Energy Mode application is supported on every EFR32 generations and families
that support proprietary software except EFR32xG21 devices.

## Getting Started

Compile the project and download the application to one or more radio boards.

----

Note that in this example the `Power Manager` platform-level component is
installed by default. This is the key element for changing energy modes most
efficiently and conveniently: it manages the system's energy modes transparently
depending on the requirements set by different software modules.

----

Button and LED instances are not installed in this example, though you can add
them in the `Software Components` window. Therefore, the only way to interact
with the device is using the CLI commands via the serial interface.

Note that CLI does not work under EM2. You get a warning message whenever it's
getting inactive:

```
> CLI won't work under EM1 level, restart will be needed
```

----

For the available CLI command options and a short description on these command
issue `help`:

```
help
  cw                            Set radio to TX CW stream mode in the selected energy mode
                                [uint8] Available energy mode: 0 - empty loop; 1 - EM1
  rx                            Set radio to RX mode in the selected energy mode
                                [uint8] Available energy mode: 0 - empty loop; 1 - EM1
  periodic_tx                   Transmits packet periodically with delay, using selected energy mode
                                [uint32] Delay in us
                                [uint8] Available energy mode: 0 - empty loop; 1 - EM1; 2 - EM2
  periodic_rx                   Receives for rxTime periodically with delay, using selected energy mode
                                [uint32] Delay in us
                                [uint32] rxTime in us
                                [uint8] Available energy mode: 0 - empty loop; 1 - EM1; 2 - EM2
  idle                          Set radio to Idle mode in the selected energy mode
                                [uint8] Available energy mode: 0 - empty loop; 1 - EM1; 2 - EM2; 3 - EM3
  set_power                     Set the current transmit power in deci-dBm
                                [int16] Power in deci-dBm
  set_power_raw                 Set the current transmit power in raw units
                                [uint8] Power in raw units
  get_power                     Get the current transmit power in deci-dBm units
  get_power_raw                 Get the current transmit power in raw units
```

### Operational Modes

The following table depicts the supported operational modes:

| Energy Mode/Radio operation | Idle | TX stream | Continuous RX | Periodic TX* | Periodic RX* |
|:---------------------------:|:----:|:---------:|:-------------:|:------------:|:------------:|
|             EM0             |   X  |     X     |       X       |       X      |       X      |
|             EM1             |   X  |     X     |       X       |       X      |       X      |
|             EM2             |   X  |     -     |       -       |       X      |       X      |
|             EM3             |   X  |     -     |       -       |       -      |       -      |

> (\* in case of periodic operation the Energy Mode applies when the radio is
> inactive; during the active operation, the radio is in the highest available
> energy state, which is lower or equal than the maximum of the requested state
> and EM1(P); e.g.:
> - `periodic_rx <inactive_period> <active_period> 2` set EM2 as the inactive
>  state, but the device is in EM1(P) during the RX period,
> - `periodic_rx <inactive_period> <active_period> 0` keeps the device in active
> mode)

The device operates in the selected mode until a mode change is requested.

The `idle` command terminates the ongoing radio operation, disables the radio
and sets the device to the requested energy mode. This mode is useful when only
the MCU's energy consumption should be measured.

To measure the radio's average TX/RX current consumption it is recommended to
use the `cw` or the `rx` command, respectively. However, you can initiate
periodic TX and RX operations by the `periodic_tx` and `periodic_rx` commands,
respectively. The duration of the TX operation depends on the packet
configuration, while the RX time is a configurable parameter of the
`periodic_rx` command. Between the radio operations the device goes to the EM
state defined as the last argument of the command.

#### Changing TX Power

The `set_power` and `set_power_raw` CLI commands set the TX power in raw units
or in deci-dBm, respectively. Note that not all power levels are achievable, and
these functions sets the radio's TX power as close as possible to the desired
power without exceeding it.

## Notes on Energy Consumption Measurements

> The Energy Profiler does not make it possible to measure accurate sleep
> current (EM2 and above). This tool is good for monitoring how often the system
> wakes up, but it might provide false results in such low power range, as the
> BRD4001 mainboard's sample rate is insufficient to measure it.

> If you are working with a Silicon Labs radio board, it is advised to measure
> the sleep current of the radio board detached from the mainboard, due to
> unwanted leakage current might be present between the radio board and the
> mainboard.

> You should consider to terminate with 50 Ohm the active RF path during radio
> operation. Also, the antenna's characteristic should be considered as
> reflection might lead to false results, too. Do not place reflector in the
> path of RF signal propagation! E.g., doing the measurement on a table with a
> vertically aligned antenna should be avoided.

## What This Example Shouldn't Be Used For

In the following scenarios it is not advised to use this example:

- Using this example as a starting point for development: the Energy mode
  example is quite complex and has many dependencies, therefore we recommend to
  start with `RAIL - SoC Empty` instead.
- Testing radio operation: if you want to test the basic radio operation (packet
  transmission and reception) it is advised to create `RAIL - SoC Simple TRX`
  instead.
- Validating radio configuration: since this application has strict limitations,
  if you have to evaluate a newly generated radio configuration you might want
  to use `RAIL - SoC RAILtest` example to validate your PHY(s) as an
  alternative.
- Testing radio range: we have a complete solution for testing the radio's
  performance if the two devices are placed far to each other. In such cases use
  the `RAIL - SoC Range Test` example application instead.

## Notes

> The application is designed in a way, that it can be run on an OS. You can add
> any RTOS support for this application in the `Software Components` window.
> Currently `MicriumOS` and `FreeRTOS` is supported.

> Due to the higher current consumption of the continuous radio usage
> (especially in RX or continuous TX Mode), it is not recommended to power the
> boards from a coin cell. Instead, an USB power bank can be used if portability
> is needed.

## Conclusion

Primarily, the `RAIL - SoC Energy Mode` example improves user experience by
being an off-the-shelf solution for the problem of energy consumption evaluation
on kits or custom boards.

Apart from using it for performing energy consumption tests, there is little use
of starting your application from this example, due to it's complexity; you
might still reuse some part of the implementation of this example by copying
codes from it into your project if you are going to make your project more
energy friendly.

## Resources

- [RAIL Tutorial
  Series](https://community.silabs.com/s/article/rail-tutorial-series?language=en_US):
  it is advised to read through the `Studio v5 series` first to familiarize the
  basics of packet transmission and reception.
- [RAIL Tutorial: Timer Synchronization and Sleep](https://community.silabs.com/s/article/rail-tutorial-timer-synchronization-and-sleep)
- [Power Manager component documentation](https://docs.silabs.com/gecko-platform/latest/service/power_manager/overview)
- [Energy Profiler documentation](https://docs.silabs.com/simplicity-studio-5-users-guide/1.0/using-the-tools/energy-profiler/)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
