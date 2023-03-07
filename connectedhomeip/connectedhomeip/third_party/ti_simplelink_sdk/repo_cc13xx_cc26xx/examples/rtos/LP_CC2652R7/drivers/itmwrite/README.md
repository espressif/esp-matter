---
# itmwrite

---

## Example Summary

Example that uses the Instrumentation Trace Macrocell (ITM) hardware to write
software instrumentation messages to the PC.

These messages can be read using a suite of tools available in CCS, IAR, or
standalone tools.

## Peripherals Exercised

* `Board_GPIO_LED0` - Indicates that the board was initialized within `main()`

## Resources & Jumper Settings

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find Board.html in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

If using CC32xx, then the device must be debugged in SWD mode and the SOP
jumper must be placed in position 0 in order to send ITM messages
on the SWO pin.

If using CC13XX/26XX the SWO jumper must be connected on the LaunchPad.

## Example Usage

* Use Device Manager to check the value of the XDS110 Class Auxiliary Data
Port. Do not use the "User UART" port. This should be a value like `COM22`.

* Follow the setup instructions under the "Install Python Modules" heading in
&lt;SDK_INSTALL_DIR&gt;tools/log/README to generate a virtual environment.
This will setup the ITM reader's python environment. You do not need Wireshark
for this example, and instructions for it should be ignored. Stop at the end of
"Install Python Modules" and move on to the next step in this document.

* Start a terminal in the &lt;SDK_INSTALL_DIR&gt;tools/log/ folder and
activate the virtual environment from the previous step by running
`> ./.venv/Scripts/activate`. Some terminals will now display an extra note
with the prompt (e.g. `(.venv)` in PowerShell) indicating that a venv is active.

* In the same terminal, start the tool: `tilogger_itm_viewer 115200 <PORT>`
where <PORT> is the COM port from the first step (for example
`tilogger_itm_viewer 115200 COM22`).

* Build and flash the example using CCS or another tool. Once the firmware is
flashed, disconnect the debugger. ITM output will not display if the debugger
is connected.

* Reset the board. `Board_GPIO_LED0` turns ON to indicate driver initialization
is complete. The ITM tool will not begin printing until it receives a reset
frame.

* The target will write a string message to the ITM software port:
"Hello World from ITM". Each write to the ITM stream will appear in the tool.
See below for expected output and how this string appears in the data stream.

* A clock function is setup that will update a global variable. This variable's
address is also setup as a DWT watch point. Every time the clock function runs
the variable will update and the LED will toggle.

## Expected Output
When running the tool and resetting the board, this is an example of expected
output:

```
> tilogger_itm_viewer 115200 COM62
Serial port opened
SW SWIT at +0, port STIM_DRIVER: 0xBB 0xBB 0xBB 0xBB
SW SWIT at +0, port STIM_RESV0: 0x48 0x65 0x6C 0x6C
TIMESTAMP in sync: + 169 cycles
SW SWIT at +169, port STIM_RESV0: 0x6F 0x20 0x57 0x6F
Unknown/Reserved timestamp header: 16
SW SWIT at +0, port STIM_RESV0: 0x72 0x6C 0x64 0x20
Unknown/Reserved timestamp header: 16
SW SWIT at +0, port STIM_RESV0: 0x66 0x72 0x6F 0x6D
SW SWIT at +0, port STIM_RESV0: 0x20 0x49 0x54 0x4D
SW SWIT at +0, port STIM_RESV0: 0x00
TIMESTAMP packet and timestamp delayed: + 34 cycles
TIMESTAMP in sync: + 1999999 cycles
HW Trace at 1999.999, Read Access, comparator: 0, value : 0x0
TIMESTAMP in sync: + 1030325 cycles
HW Trace at 1030.325, Write Access, comparator: 0, value : 0x1
TIMESTAMP in sync: + 1999999 cycles
HW Trace at 1999.999, Read Access, comparator: 0, value : 0x1
TIMESTAMP in sync: + 1020673 cycles
HW Trace at 1020.673, Write Access, comparator: 0, value : 0x2
TIMESTAMP in sync: + 1999999 cycles
HW Trace at 1999.999, Read Access, comparator: 0, value : 0x2
TIMESTAMP in sync: + 1020757 cycles
HW Trace at 1020.757, Write Access, comparator: 0, value : 0x3
```

First, we can see the reset frame `0xBB 0xBB 0xBB 0xBB`.

Then we see a series of hex values interleaved with timestamps. These values
match the expected hex output for "Hello World from ITM":

```
 H    e    l    l    o         W    o    r    l    d
0x48 0x65 0x6C 0x6C 0x6F 0x20 0x57 0x6F 0x72 0x6C 0x64 0x20

 f    r    o    m         I    T    M
0x66 0x72 0x6F 0x6D 0x20 0x49 0x54 0x4D 0x00
```

After this we see only the `HW Trace` packets. These are messages from the DWT
Watchpoint module, which is tracking accesses to `variableToWatch`, which is
accessed periodically by `clockTimeoutFunction`.

The lines indicating `Unknown/Reserved timestamp` are a limitation with the
current tooling.

## Host Side Tooling

Several IDEs such as IAR or CCS offer ITM focused tooling. These tools will
configure the ITM registers directly through the debugger. This example
will configure the ITM on device from firmware. For this reason,
this example will not be compatible with these tools as the configuration
settings may clash.

If it is desired to use these tools, it is recommended to comment out any APIs
related to ITM configuration (e.g. `ITM_open`, `ITM_close`, `ITM_enable*`,
`ITM_disable*`) and use `ITM_applyPinMux` directly to mux out the SWO pin.

From there, the settings and configuration of ITM is configured by the IDE.
However the SW message APIs can be used to write to the stimulus ports.

## Syscfg

The ITM driver parameters such as serial format, baudrate, and trace port enable
can be configured via syscfg. When enabled, the ITM driver will hook into the
Power policy using `ITM_flush` and `ITM_restore` to ensure that all pending
messages are flushed before the device powers down the CPU domain.

## Application Design Details

* This example shows how to initialize the ITM driver and use it to produce
software messages and DWT messages from the target.

* A single thread, `mainThread`, configures and accesses the ITM.

TI-RTOS:

* When building in Code Composer Studio, the kernel configuration project will
be imported along with the example. The kernel configuration project is
referenced by the example, so it will be built first. The "release" kernel
configuration is the default project used. It has many debug features disabled.
These feature include assert checking, logging and runtime stack checks. For a
detailed difference between the "release" and "debug" kernel configurations and
how to switch between them, please refer to the SimpleLink MCU SDK User's
Guide. The "release" and "debug" kernel configuration projects can be found
under &lt;SDK_INSTALL_DIR&gt;/kernel/tirtos/builds/&lt;BOARD&gt;/(release|debug)/(ccs|gcc).

FreeRTOS:

* Please view the `FreeRTOSConfig.h` header file for example configuration
information.

IAR:

* When using any SensorTag(STK) Board, the XDS110 debugger must be
selected with the 4-wire JTAG connection within your projects' debugger
configuration.
