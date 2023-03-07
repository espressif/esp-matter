# Sleeptimer Bare Metal

This exmaple application demonstrates use of timers in a bare metal environment. The application uses low frequency RTC (Real Time Clock) peripheral to create periodic and one-shot timers. The on-board button used to control the timers stop and start operation. The status will be shown on the virtual COM serial port console.

This application has three timers:

* Periodic timer - Toogles LED0 when timeout
* One-shot timer - Toggels LED1 when timeout
* Perdioic timer - Prints remaining time of periodc & one-shot timers

Once you flash the application both timers will be started. Button0 is used to start/stop the periodic timer and button1 is used to start/stop the one-shot timer. The status timer will never be stopped and prints status of other two timers periodiocally when timeout occurs.

## Requirements

Silicon Labs board with 2 buttons and 2 LEDs.

## Resources

* [AN0005: Real Time Counters](https://www.silabs.com/documents/public/application-notes/AN0005-EFM32-RTC.pdf)
* [AN0014: EFM32 Timers](https://www.silabs.com/documents/public/application-notes/AN0014.pdf)