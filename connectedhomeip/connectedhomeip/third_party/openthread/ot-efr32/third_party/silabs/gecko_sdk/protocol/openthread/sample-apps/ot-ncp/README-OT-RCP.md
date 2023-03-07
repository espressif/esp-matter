# Openthread RCP

This is a Radio Co-Processor (RCP) application that can be used in tandem with an OpenThread POSIX host application, to demonstrate the Co-Processor design of the OpenThread stack.

In an RCP design, the core of OpenThread lives on the host processor with only a minimal MAC layer "controller" on the device with the Thread radio. The host processor typically doesn’t sleep in this design, in part to ensure reliability of the Thread network.

Communication between the RCP and the host processor is managed by the OpenThread Daemon through a SPI or UART interface over the Spinel protocol.

The advantage of this design is that OpenThread can utilize the resources on the more powerful processor.

This design is useful for devices that are less sensitive to power constraints. For example, the host processor on a video camera is always on to process video.

OpenThread Border Router supports an RCP design. For more information, refer to *AN1256: Using the Silicon Labs RCP with the OpenThread Border Router*.

The corresponding host application is located at `util/third_party/ot-br-posix`