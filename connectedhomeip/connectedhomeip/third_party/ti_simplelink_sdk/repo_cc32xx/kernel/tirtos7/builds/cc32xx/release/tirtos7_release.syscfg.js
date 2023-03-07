

/* ******* Add SYS/BIOS 7.x to the configuration ********* */


/* ================ Kernel (SYS/BIOS) configuration ================ */
const BIOS  = scripting.addModule("/ti/sysbios/BIOS");
/*
 * Enable asserts in the BIOS library.
 *
 * Pick one:
 *  - true (default)
 *      Enables asserts for debugging purposes.
 *  - false
 *      Disables asserts for a reduced code footprint and better performance.
 */
//BIOS.assertsEnabled = true;
BIOS.assertsEnabled = false;

/*
 * Runtime instance creation enable flag.
 *
 * Pick one:
 *   - true (default)
 *      Allows Mod_create() and Mod_delete() to be called at runtime which
 *      requires a default heap for dynamic memory allocation.
 *   - false
 *      Reduces code footprint by disallowing Mod_create() and Mod_delete() to
 *      be called at runtime. Object instances are constructed via
 *      Mod_construct() and destructed via Mod_destruct().
 */
BIOS.runtimeCreatesEnabled = true;
//BIOS.runtimeCreatesEnabled = false;

/* ================ Memory configuration ================ */
/*
 * Use HeapMem primary heap instance to use linker-defined memory region
 * Add HeapTrack on top to find over-writes, invalid frees, and
 * aid in finding the correct sizing of the heap and memory leaks.
 */
BIOS.heapBaseAddr = "__primary_heap_start__";
BIOS.heapEndAddr = "__primary_heap_end__";
// BIOS.heapTrackEnabled = true;
BIOS.heapTrackEnabled = false;

/* ================ POSIX configuration ================ */
const POSIX = scripting.addModule("/ti/posix/tirtos/Settings");

/* ================ Clock configuration ================ */
const Clock = scripting.addModule("/ti/sysbios/knl/Clock", {}, false);
/*
 * Default value is family dependent. For example, Linux systems often only
 * support a minimum period of 10000 us and multiples of 10000 us.
 * TI platforms have a default of 1000 us.
 */
Clock.tickPeriod = 1000;


/* ================ Hwi configuration ================ */
const Hwi = scripting.addModule("/ti/sysbios/family/arm/m3/Hwi", {}, false);
/*
 * Checks for Hwi (system) stack overruns while in the Idle loop.
 *
 * Pick one:
 *  - true (default)
 *      Checks the top word for system stack overflows during the idle loop and
 *      raises an Error if one is detected.
 *  - false
 *      Disabling the runtime check improves runtime performance and yields a
 *      reduced flash footprint.
 */
//Hwi.checkStackFlag = true;
Hwi.checkStackFlag = false;

/*
 * The following options alter the system's behavior when a hardware exception
 * is detected.
 *
 * Pick one:
 *  - Hwi.enableException = true
 *      This option causes the default Hwi.excHandlerFunc function to fully
 *      decode an exception and dump the registers to the system console.
 *      This option raises errors in the Error module and displays the
 *      exception in ROV.
 *  - Hwi.enableException = false
 *      This option reduces code footprint by not decoding or printing the
 *      exception to the system console.
 *      It however still raises errors in the Error module and displays the
 *      exception in ROV.
 *  - Hwi.excHandlerFunc = null
 *      This is the most aggressive option for code footprint savings; but it
 *      can difficult to debug exceptions. It reduces flash footprint by
 *      plugging in a default while(1) trap when exception occur. This option
 *      does not raise an error with the Error module.
 */
//Hwi.enableException = true;
Hwi.enableException = false;
//Hwi.excHandlerFunc = null;

/*
 * Enable hardware exception generation when dividing by zero.
 *
 * Pick one:
 *  - 0 (default)
 *      Disables hardware exceptions when dividing by zero
 *  - 1
 *      Enables hardware exceptions when dividing by zero
 */
//Hwi.nvicCCR.DIV_0_TRP = 0;
//Hwi.nvicCCR.DIV_0_TRP = 1;


/* ================ Idle configuration ================ */
const Idle = scripting.addModule("/ti/sysbios/knl/Idle", {}, false);
/*
 * The Idle module is used to specify a list of functions to be called when no
 * other tasks are running in the system.
 *
 * Functions added here will be run continuously within the idle task.
 *
 * Function signature:
 *     Void func(Void);
 */
const powerIdle = Idle.addInstance();
powerIdle.$name = "powerIdle";
powerIdle.idleFxn = "Power_idleFunc";


/* ================ Semaphore configuration ================ */
var Semaphore = scripting.addModule("/ti/sysbios/knl/Semaphore", {}, false);
/*
 * Enables global support for Task priority pend queuing.
 *
 * Pick one:
 *  - true (default)
 *      This allows pending tasks to be serviced based on their task priority.
 *  - false
 *      Pending tasks are services based on first in, first out basis.
 */
//Semaphore.supportsPriority = true;
Semaphore.supportsPriority = false;

/*
 * Allows for the implicit posting of events through the semaphore,
 * disable for additional code saving.
 *
 * Pick one:
 *  - true
 *      This allows the Semaphore module to post semaphores and events
 *      simultaneously.
 *  - false (default)
 *      Events must be explicitly posted to unblock tasks.
 */
//Semaphore.supportsEvents = true;
Semaphore.supportsEvents = false;


/* ================ Swi configuration ================ */
const Swi = scripting.addModule("/ti/sysbios/knl/Swi", {}, false);
/*
 * A software interrupt is an object that encapsulates a function to be
 * executed and a priority. Software interrupts are prioritized, preempt tasks
 * and are preempted by hardware interrupt service routines.
 *
 * This module is included to allow Swi's in a users' application.
 */
/*
 * Change the number of task priorities.
 * The default is 16.
 * Decreasing the number of task priorities yield memory savings.
 */
Swi.numPriorities = 16;


/* ================ System configuration ================ */
const System = scripting.addModule("/ti/sysbios/runtime/System", {}, false);
/*
 * The Abort handler is called when the system exits abnormally.
 *
 * Pick one:
 *  - System.abortStd (default)
 *      Call the ANSI C Standard 'abort()' to terminate the application.
 *  - System.abortSpin
 *      A lightweight abort function that loops indefinitely in a while(1) trap
 *      function.
 *  - A custom abort handler
 *      A user-defined function. See the System module documentation for
 *      details.
 */
//System.abortFxn = "System_abortStd";
System.abortFxn = "System_abortSpin";
//System.abortFxn = "myAbortSystem";

/*
 * The Exit handler is called when the system exits normally.
 *
 * Pick one:
 *  - System.exitStd (default)
 *      Call the ANSI C Standard 'exit()' to terminate the application.
 *  - System.exitSpin
 *      A lightweight exit function that loops indefinitely in a while(1) trap
 *      function.
 *  - A custom exit function
 *      A user-defined function. See the System module documentation for
 *      details.
 */
//System.exitFxn = "System_exitStd";
System.exitFxn = "System_exitSpin";
//System.exitFxn = "myExitSystem";

/*
 * Minimize exit handler array in the System module. The System module includes
 * an array of functions that are registered with System_atexit() which is
 * called by System_exit(). The default value is 8.
 */
//System.maxAtexitHandlers = 2;

/*
 * Enable System_printf() to display floats.
 */
System.extendedFormats = "%f";

/*
 * The System.SupportModule defines a low-level implementation of System
 * functions such as System_printf(), System_flush(), etc.
 *
 * Pick one pair:
 *  - SysMin
 *      This module maintains an internal configurable circular buffer that
 *      stores the output until System_flush() is called.
 *      The size of the circular buffer is set via SysMin.bufSize.
 *  - SysCallback
 *      SysCallback allows for user-defined implementations for System APIs.
 *      The SysCallback support proxy has a smaller code footprint and can be
 *      used to supply custom System_printf services.
 *      The default SysCallback functions point to stub functions. See the
 *      SysCallback module's documentation.
 */
//const SysMin = scripting.addModule("/ti/sysbios/runtime/SysMin");
//SysMin.bufSize = 1024;
//System.supportModule = "SysMin";
const SysCallback = scripting.addModule("/ti/sysbios/runtime/SysCallback");
System.supportModule = "SysCallback";
//SysCallback.abortFxn = "myUserAbort";
//SysCallback.exitFxn  = "myUserExit";
//SysCallback.flushFxn = "myUserFlush";
//SysCallback.putchFxn = "myUserPutch";
//SysCallback.readyFxn = "myUserReady";


/* ================ Error configuration ================ */
const Error = scripting.addModule("/ti/sysbios/runtime/Error");

/*
 * The Error policy that is called when there is an error.
 *
 * Pick one:
 *  - Error_SPIN
 *      This is the most light-weight policy that loops indefinitely in a
 *      while(1) trap function.
 *  - Error_UNWIND
 *      This error policy will unwind the call and return to the caller.
 *  - Error_TERMINATE
 *      This error policy will call System_abort() when the error is raised.
 */
Error.policy = "Error_SPIN";

/*
 * Pick one:
 *  - true
 *      The details of the error will be printed using System_printf(). This
 *      results in a printf() implementation always being pulled into the
 *      application and adds several kB of flash overhead.
 *  - false
 *      The details of the error will not be printed out.
 */
Error.printDetails = false;


/* ================ Task configuration ================ */
const Task = scripting.addModule("/ti/sysbios/knl/Task", {}, false);
/*
 * Check task stacks for overflow conditions.
 *
 * Pick one:
 *  - true (default)
 *      Enables runtime checks for task stack overflow conditions during
 *      context switching ("from" and "to")
 *  - false
 *      Disables runtime checks for task stack overflow conditions.
 */
//Task.checkStackFlag = true;
Task.checkStackFlag = false;

/*
 * Set the default task stack size when creating tasks.
 *
 * The default is dependent on the device being used. Reducing the default stack
 * size yields greater memory savings.
 */
Task.defaultStackSize = 512;

/*
 * Enables the idle task.
 *
 * Pick one:
 *  - true (default)
 *      Creates a task with priority of 0 which calls idle hook functions. This
 *      option must be set to true to gain power savings provided by the Power
 *      module.
 *  - false
 *      No idle task is created. This option consumes less memory as no
 *      additional default task stack is needed.
 *      To gain power savings by the Power module without having the idle task,
 *      add Idle.run as the Task.allBlockedFunc.
 */
Task.enableIdleTask = true;
//Task.enableIdleTask = false;
//Task.allBlockedFunc = Idle.run;

/*
 * If Task.enableIdleTask is set to true, this option sets the idle task's
 * stack size.
 *
 * Reducing the idle stack size yields greater memory savings.
 */
Task.idleTaskStackSize = 512;

/*
 * Change the number of task priorities.
 * The default is 16.
 * Decreasing the number of task priorities yield memory savings.
 */
Task.numPriorities = 16;


/* ================ Additional configuration ================ */
/*
 * Add a few other commonly used modules.
 */
const Event = scripting.addModule("/ti/sysbios/knl/Event");
const Mailbox = scripting.addModule("/ti/sysbios/knl/Mailbox");
const Timestamp = scripting.addModule("/ti/sysbios/runtime/Timestamp");
