# Openthread NCP

This is a NCP (Network Co-Processor) application that can be used in tandem with an OpenThread POSIX host application, to demonstrate the Co-Processor design of the OpenThread stack.

The standard NCP design has Thread features on the SoC and runs the application layer on a host processor, which is typically more capable (but has greater power demands) than the OpenThread device.

Communication between the NCP and the host processor is managed by wpantund through a serial interface, typically using SPI or UART, over the Spinel protocol.

The benefit of this design is that the higher-power host can sleep while the lower-power OpenThread device remains active to maintain its place in the Thread network. Since the SoC is not tied to the application layer, development and testing of applications is independent of the OpenThread build.

This design is useful for gateway devices or devices that have other processing demands, like IP cameras and speakers.
