## Example Summary

This application demonstrates how to use the SYS/BIOS Clock and Task modules in
a C++ application, to create a real-time clock/calendar.

## Example Usage

Run the application. The current times of two Clock objects (`id 1` and
`id 2`) will be output to the console.  By default the program will
terminate after 2 seconds.  The program can be modified to run longer, or
continuously, by changing when the call to clockTerminate() is made.

The default output:

```
bigTime started.
id 1 : 0:0:0.0
id 1 : August 19, 2010
id 2 : 0:0:0
id 2 : August 19, 2010
id 1 : 0:0:0.1
id 1 : August 19, 2010
id 1 : 0:0:0.2
id 1 : August 19, 2010
id 1 : 0:0:0.3
id 1 : August 19, 2010
id 1 : 0:0:0.4
id 1 : August 19, 2010
id 1 : 0:0:0.5
id 1 : August 19, 2010
id 1 : 0:0:0.6
id 1 : August 19, 2010
id 1 : 0:0:0.7
id 1 : August 19, 2010
id 1 : 0:0:0.8
id 1 : August 19, 2010
id 1 : 0:0:0.9
id 1 : August 19, 2010
id 1 : 0:0:1.0
id 1 : August 19, 2010
id 2 : 0:0:1
id 2 : August 19, 2010
bigTime ended.
```

## Application Design Details

* The C++ class object, Clock, is driven by a variety of SYS/BIOS objects: two
Clock objects, two Task objects, and an Idle object. Each SYS/BIOS object has
its own instantiation of the Clock.  When the Clock, Task, and Idle functions
execute, they call their clock's tick function, advancing the connected timer
by one second.

* Because the functions execute at different rates, the attached clocks also
advance at different rates.  For example, one of the Clock objects, `cl2`,
has a period of one second.  When `cl2` runs, it advances its timer by
one second. This results in an accurate clock.  On the other hand,
`cl0` runs with every pass through the idle loop.  Therefore, the number of
seconds passed for its attached timer indicates the time spent in the idle loop.

> See the projects' `main` function for more information on how the SYS/BIOS
objects are constructed.

> You can experiment with advancing Clock at different rates
by changing the `Clock::tick function`

> Some targets utilize the RTOS analyzer to output the Clock times as
`Log_info` events rather than using `System_printf`. To view the logs, open
`Tools->RTOS Analyzer->Printf and Error Logs` and switch to the `Live
Session` tab.

## References
* For GNU and IAR users, please read the following website for details about
enabling [semi-hosting](http://processors.wiki.ti.com/index.php/TI-RTOS_Examples_SemiHosting)
in order to view console output.

* For more help, search the SYS/BIOS User Guide.
