#USB Composite FreeRTOS application
This example project demonstrates use of the USB stack with FreeRTOS.

The application will create a composite USB device consisting of
a CDC ACM interface and an HID mouse interface. By connecting to the
ACM interface using a serial client, the user can send text commands
to change the position of the mouse.

 Please note that the command will be parsed by the USB Device only when an End of Line is received.
For the End of Line to be handled correctly by the application, it must only be one character long. 
Please make sure that your EOL is set to CR or LF but not CRLF.  
