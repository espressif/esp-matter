## Overview

The Execution Graph is an ROV Addon. It is used to render instrumentation
data in a graphical view in such a way as to express the CPU execution
path between tasks, interrupts, and software components.

## Setup

To use the Execution Graph, you must perform the following steps:

 1. Import the Execution Graph ROV Addon
 1. Enable instrumentation in the kernel
 1. Configure the Instrumentation module

### Import the Execution Graph ROV Addon

To import an Addon, you must open the ROV View. This requires an active
CCS Debug session. Select the active core and open the ROV View:

    Tools > Runtime Object View

Connect the ROV view to your debug session:

    Executable path: <Use the program currently being debugged>
    Target communication link: Debugger
    Connect

Open the Add-on View in ROV:

    Settings (gear) > Add-on views

Click on the Import icon and enter the full path to your addon folder
(use forward slashes in your path name):

    <SDK>/tools/rov/addons/execution_graph
    Import

The Execution Graph Addon will be listed under Imported add-ons. Close
the import dialog. There will be a new icon on the ROV toolbar called
"Open other view". Click it to open the Execution Graph Addon.

> **Note**: When you update to a new SDK release, remember to remove the
> Addon and import it again from the new SDK.

#### [Optional] Set the XDC_ROV_HTTPROOTS environment variable

As an alternative to explicitly importing the Addon (as described above),
you may use an environment variable to specify multiple root directories
which are searched to find available Addons.

The environment variable XDC_ROV_HTTPROOTS defines a `';'` separated list
of directories that will be searched by ROV to locate your Addon. Within
each directory specified in this list, ROV will look for a sub-directory
named `addons`. The Execution Graph Addon is shipped in the `tools/rov`
folder of the SDK. Therefore, add the following directory to
XDC_ROV_HTTPROOTS, specifying the full path to your SDK (use forward slashes
in your path name):

    <SDK>/tools/rov

When you update to a new SDK release, remember to update the environment
variable to use the new SDK and restart CCS for the change to take effect.

Once you have set the environment variable, and restarted CCS, import the
Execution Graph Addon as described above. However, the Addon will be listed
under Available add-ons without having to specify the import path. Select
the Execution Graph Addon and click Import.

### Enable instrumentation in your program

The TI-RTOS kernel configuration is specified in a cfg script. Add the
following code to your kernel configuration script to enable the Execution
Graph to track tasks and interrupts.

    /* ================ logging ================ */
    var Defaults = xdc.useModule('xdc.runtime.Defaults');
    var Diags = xdc.useModule('xdc.runtime.Diags');
    var LoggerBuf = xdc.useModule('xdc.runtime.LoggerBuf');
    var LoggerBufP = new LoggerBuf.Params();

    /* low-frequency events */
    LoggerBufP.numEntries = 100;
    Defaults.common$.logger = LoggerBuf.create(LoggerBufP);

    var Main = xdc.useModule('xdc.runtime.Main');
    Main.common$.diags_INFO = Diags.ALWAYS_ON;

    var Task = xdc.useModule('ti.sysbios.knl.Task');
    Task.common$.diags_USER1 = Diags.ALWAYS_ON;
    Task.common$.diags_USER2 = Diags.ALWAYS_ON;

    /* interrupt events (high frequency events) */
    LoggerBufP.numEntries = 40;
    var logger_intr = LoggerBuf.create(LoggerBufP);

    var Hwi = xdc.useModule('ti.sysbios.hal.Hwi');
    Hwi.common$.diags_USER2 = Diags.ALWAYS_ON;
    Hwi.common$.logger = logger_intr;

    var Swi = xdc.useModule('ti.sysbios.knl.Swi');
    Swi.common$.diags_USER2 = Diags.ALWAYS_ON;
    Swi.common$.logger = logger_intr;

    var Clock = xdc.useModule('ti.sysbios.knl.Clock');
    Clock.common$.diags_USER2 = Diags.ALWAYS_ON;
    Clock.common$.logger = logger_intr;

    /* SYS/BIOS configuration */
    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    BIOS.logsEnabled = true;

This configuration will create two LoggerBuf instances: one for low-frequency
events, and one for high-frequency events. This approach will maximize your
history view while minimizing your memory footprint. By using two separate
LoggerBuf instances, the low-frequency events, such as task switches, will
reach back further in time compared to the high-frequency events, such as
timer interrupts. Keep this in mind when analysing the Execution Graph.
Even though you will not see the timer interrupts when looking back in time,
it does not mean they did not occur; it only means they were overwritten by
more recent interrupt events.

If your configuration is using SYS/BIOS in ROM, you will need to disable
this in order to link with an instrumented version of SYS/BIOS. Comment
out the ROM configuration in your cfg script:

    // var ROM = xdc.useModule('ti.sysbios.rom.ROM');
    // ROM.romName = ...

If the default timestamp provider is running slow (e.g. 64KHz), you may
see multiple log events with the same timestamp. Switching to a high-frequency
timestamp provider will eliminate duplicate timestamps. However, this may
also require to disable deep-sleep mode. Add the following run-time code
to disable deep-sleep mode:

    #include <ti/drivers/Power.h>
    Power_disablePolicy();

On ARM processor, add the following to the SYS/BIOS configuration script
to use a high-frequency timestamp provider:

    var Timestamp = xdc.useModule('xdc.runtime.Timestamp');
    Timestamp.SupportProxy = xdc.useModule('ti.sysbios.family.arm.m3.TimestampProvider');

Be sure to update the timestamp frequency setting in the Execution Graph
Settings module to reflect the new timestamp provider tick rate (usually
ticks at the CPU clock frequency):

    SysConfig > TI UTILS RUN-TIME
    ExecGraphSetting
    Timestamp Frequency: 48000000

### Configure the Instrumentation module

The Execution Graph ROV Addon relies on the Instrumentation module for
information on how to interpret the data. For example, the Instrumentation
module specifies the clock frequency used for timestamping the log events.

In order to use the Execution Graph ROV Addon, use SysConfig to add the
Instrumentation module to your program's configuration. You will find it
in the ti.utils.runtime component.

## Using the Execution Graph ROV Addon

Use the Execution Graph ROV Addon in the Code Composer Studio IDE.
Load and run your program. Halt your program. Open an ROV view (Run-time
Object Viewer). Connect the ROV view to your target if auto-connect is
unselected.

The first time you use a new ROV Addon, you must import it into your
workspace. Click on the Settings menu and select Import. In the next
dialog, select the Execution Graph Addon and click the Import button.
Now you will see the Execution Graph ROV Addon listed on the left.
Click Close.

All ROV Addons which have been imported into your workspace will be
listed in the Addons menu. Select the Execution Graph from this menu
to open it.
