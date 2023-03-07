## Example Summary

This example shows the use of `xdc.runtime.Error `module to both catch and raise
errors.

## Example Usage

Run the application, it will attempt to incorrectly run certain SYS/BIOS APIs
in order to cause errors. These failures are printed on the console as
they occur.

## Application Design Details

An `Error_Block` passed to `Task_create()` checked to see if the create
was unsuccessful due to a memory allocation failure. In no `Error_Block`
is passed, then the application will terminate when an error occurs.
This is shown in the `Memory_alloc()` call when the `Error_Block`
passed in is `NULL`.

> The configuration file *error.cfg* shows how to plug error hook function
that will get called as soon as an error occurs rather than program
termination.

## References
* For GNU and IAR users, please read the following website for details about
enabling [semi-hosting](http://processors.wiki.ti.com/index.php/TI-RTOS_Examples_SemiHosting)
in order to view console output.

* For more help, search the SYS/BIOS User Guide.
