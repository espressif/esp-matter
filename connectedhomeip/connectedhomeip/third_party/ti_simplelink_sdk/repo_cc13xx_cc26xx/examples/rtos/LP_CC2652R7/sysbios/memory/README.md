## Example Summary

This application demonstrates shows the use of `xdc.runtime.Memory module`
and different `xdc.runtime.IHeap` implementations to manage memory.

## Example Usage

* Run the application, the two tasks that run will allocate memory from two
different `IHeap` implementations and print their memory statuses to the
console.

## Application Design Details

* The application dynamically constructs two tasks, both tasks use
`xdc.runtime.Memory` to allocate memory but use two different different
`IHeap` implementations or heaps.

* The first task runs as `task0Fxn` uses `ti.sysbios.heaps.HeapBuf` to
dynamically create its heap, `task0Heap`, because the allocations are of
a fixed size.

* The second task run as `task1Fxn` uses `ti.sysbios.heaps.HeapMem` to
dynamically construct its heap, `task1Heap`, as this task allocates variable
block sizes. Both tasks print their heap status before allocations and after freeing the allocated memory.

> The system heap is created statically using `ti.sysbios.heaps.HeapMem` and is
plugged into `xdc.runtime.memory` as the `defaultHeapInstance`. Please see
the corresponding configuration file *memory.cfg* for details.

## References
* For GNU and IAR users, please read the following website for details about
enabling [semi-hosting](http://processors.wiki.ti.com/index.php/TI-RTOS_Examples_SemiHosting)
in order to view console output.

* For more help, search the SYS/BIOS User Guide.
