## Example Summary

This application runs through a set of common SYS/BIOS APIs in order to
measure, in CPU cycles, each of their execution times.

## Example Usage

* Run the application, the corresponding minimum, maximum and average execution
time API execution times will be printed to the console.

## Application Design Details

* The SYS/BIOS objects used within the application are constructed within
`main()`, prior to start of the BIOS scheduler. The set of APIs to be tested are
run within the `benchmarkTask` where each individual function is called
multiple times in order to measure the aforementioned min, max and average
execution times.

> These benchmarks are target specific, please see the corresponding
configuration file *benchloop.cfg* for the specific settings used.

> For more debugging information such as the CPU and timestamp frequencies used
during the tests set the `USERDEBUG` flag to 1.

> In order to record accurate Kernel timing data, the TIRTOS driver and
middleware components have been excluded from this example.

> For IAR users using any SensorTag(STK) Board, the XDS110 debugger must be
selected with the 4-wire JTAG connection within your projects' debugger
configuration.

## References
* For GNU and IAR users, please read the following website for details about
enabling [semi-hosting](http://processors.wiki.ti.com/index.php/TI-RTOS_Examples_SemiHosting)
in order to view console output.

* For more information on how these and other SYS/BIOS benchmarks are measured,
visit the __Timing Benchmarks__ section within the SYS/BIOS User Guide
for more details.
