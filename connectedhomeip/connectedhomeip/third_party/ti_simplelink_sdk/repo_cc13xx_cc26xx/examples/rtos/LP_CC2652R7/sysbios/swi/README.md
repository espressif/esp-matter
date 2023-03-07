## Example Summary

This application shows how to use the SYS/BIOS `ti.sysbios.knl.Swi` module by
exercising some of the various Swi APIs.

## Example Usage

* Run the application, two Swi's, each configured differently, will run their
prospective functions. At that time, data such as when the application entered
and exitied the respective Swis will be output to the console.

## Application Design Details

* The SYS/BIOS objects, such as the two Swis with different priorities and
   trigger counts, used within the application are constructed within `main()`.

* The applications' task, `task0Fxn` will run and call a series of Swi APIs
   designed to alter the Swis' trigger counts and eventually run the Swi
   functions.
* `BIOS_exit()` is called from the second Swi to terminate the application.

> See the projects' `main` function for more information on how the SYS/BIOS
objects are constructed.

## References
* For GNU and IAR users, please read the following website for details about
enabling [semi-hosting](http://processors.wiki.ti.com/index.php/TI-RTOS_Examples_SemiHosting)
in order to view console output.

* For more help, search the SYS/BIOS User Guide.
