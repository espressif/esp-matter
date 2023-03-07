
## Log Example

This example shows how to instrument your source code using `Log`.
Additionally it shows how to configure route log sites to a logger transport
instance.

## Resources & Jumper Settings

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find Board.html in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

If using CC32xx, then the device must be debugged in SWD mode and the SOP
jumper must be placed in position 0 in order to send ITM messages
on the SWO pin.

If using CC13XX/26XX the SWO jumper must be connected on the LaunchPad.

## Definitions

The following terms are used throughout the Readme.

| Term                            | Definition                                                                                                                                                                                                                                                                                                                   |
|---------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `LogModule`                     | A parameter passed to Log APIs to indicate which software module the log statement  originated from.                                                                                                                                                                                                                         |
| `LogLevel`                      | The severity or importance of a given log statement.                                                                                                                                                                                                                                                                         |
| `BackendDelegate`               | Also simply called a logger. This is a transport specific logger implementation.   Transports include serial interfaces such as Instrumentation Trace Macrocell (ITM)  and memory interfaces such as a buffer. The Logging framework is flexible such that  multiple backend delegates may exist in a single firmware image. |
| `CallSite`                      | A specific invocation of a Log API in a given file or program.                                                                                                                                                                                                                                                               |
| `LogSite`                       | The syscfg module that handles routing of `LogModule`s to specific `BackendDelegate`s.                                                                                                                                                                                                                                       |
| Link Time Optimization `LTO`    | Conditional inclusion/exclusion of logs using link time optimization/dead code elimination. This approach will push the decision of log inclusion to the linker. In order to achieve good performance all code and libraries participating in logging must be compiled and linked at the highest optimization level.         |
| Preprocessor Optimization `PPO` | Conditional inclusion/exclusion of logs using the C preprocessor. The decision to include/exclude logs is made during preprocessing.                                                                                                                                                                                         |


###  Highlights

  * Exercises all 3 Logging APIs showing common use cases.
  * Uses two separate LogModules.
     - `ti_utils_runtime_LogMain` is the default log module that always exists
     - `ti_utils_examples_app` is a second module that is created via `LogSite`
       in syscfg.
  * Exercises the ITM and buffer based loggers (`LoggerBuf`, `LoggerITM`).
  * Application runs in a task which wakes up every 5 seconds.

###  Beta Disclaimer

The logging ecosystem and this example are to be considered beta quality.
They are not recommended for use in production code by TI.
APIs and behaviour will change in future releases.
Please report issues or feedback to [__E2E__][e2e].

TI is seeking feedback in the following areas:

 * Presentation/configuration of logging in syscfg.
 * LTO vs PPO
 * Log API

### Overview

This example uses PPO to control inclusion of log statements. There are two log
modules used.

The routing of the log data is controlled by a LogSite instance, in
SysConfig. This routing is controlled at the module level. That is,
all log data from a given module goes to the same logger back end delegate.

The big picture:

    <pre>
    file.c -->|[x]--> LoggerBuf  --> memory, ROV
              |[ ]--> LoggerITM  --> ITM, Wireshark
                ^
                |
    LogSite ----+
    </pre>

Visualization of logs depends on their transport. This is discussed in the
following section.

## Log Visualization

Each log module is routed to a different logger back end delegate.
Due to the differing nature of the transport mechanisms used by the back end
delegates, they are visualized using different methods. The table below shows
the tooling used for visualizing the different types of logs.

| Logger      | Visualization tool            |
|-------------|-------------------------------|
| `LoggerBuf` | Runtime Object View (ROV) 2   |
| `LoggerITM` | Wireshark + TI Python Backend |

The log statements can be viewed independently. It is not required to have
both host side tools running in order for the example to run.

## Example Usage

This demo illustrates a working example of logging.

### Import and Build the Example

 1. Import the project into your workspace.

 1. Build the project.

### Load and Run the Program

This program uses ROV to display program output. You may interact with
the program using memory flags controlled through the Expressions view.

 1. Follow the wireshark setup instructions in the readme in `tools/log`
 1. Load the program.

 1. To view LoggerBuf statements: Launch ROV.

    (Optional) If using FreeRTOS it may be necessary to configure the SDK
    product that you're using. In ROV select the settings cog -> ROV User
    Guide -> Using an Image Without a CCS Project for more information

    Open the ROV `LoggerBuf` view

    <pre>
    Tools > Runtime Object View
    Connect
    LoggerBuf
    LoggerBuf > Records
    </pre>

    Enable continuous refresh to see the output as it is generated.
    The default refresh rate is 1 second.

 1. To view LoggerBuf statements: Launch Wireshark.

    Follow the steps in the "Extracting some Logs" section of the readme in
    `tools/log`

 1. Run the program.

    In ROV, you will see the initial start up message, followed by a count
    update every 5 seconds.

    <pre>
    count=1
    count=2
    count=3
    </pre>

    In Wireshark, you will see the initial start up message (Hello world), and
    messages whenever the application's semaphore is posted

    <pre>
    log_clkFxn: post semaphore
    ...
    </pre>


Note: LoggerBuf requires ROV2 which is not available in IAR. Thus, there is
no proper visualizer for these messages besides inspecting their records in
memory. The ITM transport is supported across all IDEs/toolchains.


## Conditional Log Inclusion

There are two ways to conditionally include logs in your program. They are
enabled at build time using one of the following defines:

    <pre>
    ti_utils_runtime_Log_ENABLE_LTO
    ti_utils_runtime_Log_ENABLE_PPO
    </pre>

If enabled using the `ENABLE_LTO` build define, then individual log
levels are controlled using SysConfig. If enabled using the `ENABLE_PP0`
build define, then the log levels are controlled using additional build
defines:

    <pre>
    ti_utils_runtime_Log_Level_ENABLE_INFO1
    ti_utils_runtime_Log_Level_ENABLE_INFO2
    ti_utils_runtime_Log_Level_ENABLE_INFO3
    </pre>

## Source Code Instrumentation

To add instrumentation to your source code, requires three steps:

  1. Include Log.h
  1. Define a log event object
  1. Add an instrumentation call site

In Appl.c, you will see the following code which logs the count value:

    <pre>
    #include <ti/utils/runtime/Log.h>

    Log_EVENT_CONSTRUCT(ti_utils_runtime_LogMain, LogEvent_count, "count=%d");

    Log_event(ti_utils_runtime_LogMain, Log_INFO1, LogEvent_count, Appl_count);
    </pre>

The above code will implicitly bind the Appl.c module to the LogMain
module. This means that the log data will be routed to the logger
instance specified by LogMain. In addition, when building with `ENABLE_LTO`
build define, the active log levels are controlled by the settings in
LogMain.

## Adding Modules

All log statements in the above example use the default log module
`ti_utils_runtime_LogMain`. Log modules are useful for grouping and sorting log
statements. In general it is recommended that each module within the target
software use a different log module. In order to create a new module follow
the steps below:

  1. Define a module name
  1. Create a LogSite instance

It is a good idea to use a namespace qualified module name.

    <pre>
    #include <ti/utils/runtime/Log.h>
    Log_DECLARE_MODULE(ti_utils_runtime_App)
    </pre>

Using SysConfig, create a new LogSite instance. Make sure to specify
the exact same module name for the Module property of your LogSite
instance. Now you are able to use the newly created module in Log statements in
the code.

Each log module can be routed to a different Logger instance, these can be
controlled via LogSite::Logger Modules in SysConfig.

## SysConfig

Open the INSTRUMENTATION category. You will find the following modules:

  * LogMain - a proxy module for all classic source modules
  * LogSite - used to route log data and control log levels
  * LoggerBuf - a logger which stored data in a memory buffer

[e2e]: https://e2e.ti.com/
