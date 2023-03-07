# RAIL - SoC Empty

## Introduction

The Empty Example application is a boilerplate example which doesn't do anything
except initializes RAIL to support a Single PHY radio configuration. Also, this
example defines an empty radio event handler. These are the necessary parts what
every RAIL-based application requires.

## Prerequisites

The Empty Example application is supported on every EFR32 generations and
families that support proprietary software.

## Getting Started

Unlike the most of the available example applications in Flex SDK, the Empty
Example does not define Command Line Interface nor does it support standard VCOM
printing or any other peripheral (LEDs/Button instances) by default.

Compile the project and download the application to a radio board. An unmodified
Empty Example shouldn't do any special:

- the radio is in idle state, while
- the MCU is running in an empty loop.

You might measure the current consumption (see the device's datasheet for the
expected active mode consumption) or debug the target device to make sure that
the example has been downloaded correctly to your device.

----

You can make the example energy friendly as simple as installing the `Power
Manager` component. Doing that the device will go to EM2 state after
initialization.

----

Adding basic packet TX/RX capabilities you might install the `Simple RAIL
Tx`/`Simple RAIL Rx` components.

## Peripherals

You can add LED and Button instances in the `Software Components` window.

To enable CLI interface you should install the `Command Line Interface (CLI)`
component. Make sure you satisfied the requirements by adding one of the `IO
Stream` dependency component, otherwise project generation will fail. If you are
working with a Silicon Labs development kit you should also ensure that the
Virtual COM port is enabled in the `Board Control` component in order to turn on
the serial bridge on the mainboard.

## Notes

> The application is designed in a way, that it can be run on an OS. You can add
> any RTOS support for this application in the `Software Components` window.
> Currently `MicriumOS` and `FreeRTOS` is supported.

## Conclusion

The `RAIL - SoC Empty` application sets up the absolute minimum to
start developing an application based on RAIL. If you want to test the basic
radio operation (packet transmission and reception) it is advised to create
`RAIL - SoC Simple TRX`.

## Resources

- [RAIL Tutorial
  Series](https://community.silabs.com/s/article/rail-tutorial-series?language=en_US):
  it is advised to read through the `Studio v5 series` first to familiarize the
  basics of packet transmission and reception.

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
