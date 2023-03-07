# Sleeptimer Wall Clock Bare Metal

This example application demonstrates the use of the sleeptimer's wall clock interface in a bare metal environment. The application uses low frequency RTC (Real Time Clock) peripheral to set and get the date and time. And this operations can be controlled over a virtual COM serial port.

Three types of time can be set and get in this application:

* Unix time (Shown as integer)
* Network Protocol Time (Shown as integer)
* Date and time (Shown in YYYY:MM:DD HH:MM:SS format)

## CLI Commands

Help		- Prints list of supported commands   
Get_unix_time 	- Prints unix time    
Set_unix_time 	- Can be used to set unix time (Ex: set_unix_time 17)    
get_ntp_time	- Prints ntp time   
set_ntp_time	- Can be used to set ntp time (Ex: set_ntp_time 2208988825)  
get_datetime	- Prints date and time   
set_datetime	- Can be used to set date and time (Ex: set_datetime YYYY-MM-DD HH:MM:SS)

## Requirements

Silicon Labs board with RTC peripheral.

## Resources

* [AN0005: Real Time Counters](https://www.silabs.com/documents/public/application-notes/AN0005-EFM32-RTC.pdf)
* [AN0014: EFM32 Timers](https://www.silabs.com/documents/public/application-notes/AN0014.pdf)