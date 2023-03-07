# UARTDRV Bare Metal Application


This example project demonstrates use of the UARTDRV driver with LEUART
in a bare metal application.


The application will echo back any characters it receives over the
serial connection. The use of LEUART with DMA allows the device to
enter deep sleep mode EM2.


Note that the baud rate of the serial connection is limited to 9600.
For instructions on using the virtual COM port, refer to
the device's User's Guide.

