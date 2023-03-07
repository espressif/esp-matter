/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
@file   RFCC26X2.h
@brief  Radio Frequency (RF) Core Driver for the CC13X2 and CC26X2 device
        family.

To use the RF driver, ensure that the correct driver library for your device
is linked in and include the top-level header file as follows:

@code
#include <ti/drivers/rf/RF.h>
@endcode

<hr>
@anchor rf_overview
Overview
========

The RF driver provides access to the radio core on the CC13x2/CC26x2 device
family. It offers a high-level interface for command execution and to the
radio timer (RAT). The RF driver ensures the lowest possible power consumption
by providing automatic power management that is fully transparent for the
application.

@note This document describes the features and usage of the RF driver API. For a
detailed explanation of the RF core, please refer to the
<a href='../../proprietary-rf/technical-reference-manual.html'><b>Technical
Reference Manual</b></a> or the
<a href='../../proprietary-rf/proprietary-rf-users-guide.html'><b>Proprietary
RF User Guide</b></a>.

<b>Key features are:</b>

@li @ref rf_command_execution "Synchronous execution of direct and immediate radio commands"
@li @ref rf_command_execution "Synchronous and asynchronous execution of radio operation commands"
@li Various @ref rf_event_callbacks "event hooks" to interact with RF commands and the RF driver
@li Automatic @ref rf_power_management "power management"
@li @ref rf_scheduling "Preemptive scheduler for RF operations" of different RF driver instances
@li Convenient @ref rf_rat "Access to the radio timer" (RAT)
@li @ref rf_tx_power "Programming the TX power level"
@li @ref rf_temperature_compensation "Temperature Compensation"

@anchor rf_setup_and_configuration
Setup and configuration
=======================

The RF driver can be configured at 4 different places:

1. In the build configuration by choosing either the single-client or
   multi-client driver version.

2. At compile-time by setting hardware and software interrupt priorities
   in the board support file.

3. During run-time initialization by setting #RF_Params when calling
   #RF_open().

4. At run-time via #RF_control().


Build configuration
-------------------

The RF driver comes in two versions: single-client and multi-client. The
single-client version allows only one driver instance to access the RF core at
a time. The multi-client driver version allows concurrent access to the RF
core with different RF settings. The multi-client driver has a slightly larger
footprint and is not needed for many proprietary applications. The driver
version can be selected in the build configuration by linking against a
RFCC26X2_multiMode pre-built library. The multi-client driver is the default
configuration in the SimpleLink SDKs.


Board configuration
-------------------

The RF driver handles RF core hardware interrupts and uses software interrupts
for its internal state machine. For managing the interrupt priorities, it
expects the existence of a global #RFCC26XX_HWAttrsV2 object. This object is configured
in SysConfig and defined in the generated file `ti_drivers_config.c`.
By default, the priorities are set to the lowest possible value:

@code
const RFCC26XX_HWAttrsV2 RFCC26XX_hwAttrs = {
    .hwiPriority        = INT_PRI_LEVEL7,  // Lowest HWI priority:  INT_PRI_LEVEL7
                                           // Highest HWI priority: INT_PRI_LEVEL1

    .swiPriority        = 0,               // Lowest SWI priority:  0
                                           // Highest SWI priority: Swi.numPriorities - 1

    .xoscHfAlwaysNeeded = true             // Power driver always starts XOSC-HF:       true
                                           // RF driver will request XOSC-HF if needed: false
};
@endcode


Initialization
--------------

When initiating an RF driver instance, the function #RF_open() accepts a
pointer to a #RF_Params object which might set several driver parameters. In
addition, it expects an #RF_Mode object and a setup command which is usually
generated by SmartRF Studio:

@code
RF_Params rfParams;
RF_Params_init(&rfParams);
rfParams.nInactivityTimeout = 2000;

RF_Handle rfHandle = RF_open(&rfObject, &RF_prop,
        (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
@endcode

The function #RF_open() returns a driver handle that is used for accessing the
correct driver instance. Please note that the first RF operation command
before an RX or TX operation command must be a `CMD_FS` to set the synthesizer
frequency. The RF driver caches both, the pointer to the setup command and the
physical `CMD_FS` for automatic power management.


Run-time configuration
----------------------

While a driver instance is opened, it can be re-configured with the function
#RF_control(). Various configuration parameters @ref RF_CTRL are available.
Example:

@code
uint32_t timeoutUs = 2000;
RF_control(rfHandle, RF_CTRL_SET_INACTIVITY_TIMEOUT, &timeoutUs);
@endcode

<hr>
@anchor rf_command_execution
Command execution
=================

The RF core supports 3 different kinds of commands:

1. Direct commands
2. Immediate commands
3. Radio operation commands

Direct and immediate commands are dispatched via #RF_runDirectCmd() and
#RF_runImmediateCmd() respectively. These functions block until the command
has completed and return a status code of the type #RF_Stat when done.

@code
#include <ti/devices/${DEVICE_FAMILY}/driverlib/rf_common_cmd.h>

RF_Stat status = RF_runDirectCmd(rfHandle, CMD_ABORT);
assert(status == RF_StatCmdDoneSuccess);
@endcode

Radio operation commands are potentially long-running commands and support
different triggers as well as conditional execution. Only one command can be
executed at a time, but the RF driver provides an internal queue that stores
commands until the RF core is free. Two interfaces are provided for radio
operation commands:

1. Asynchronous: #RF_postCmd() and #RF_pendCmd()
2. Synchronous: #RF_runCmd()

The asynchronous function #RF_postCmd() posts a radio operation into the
driver's internal command queue and returns a command handle of the type
#RF_CmdHandle which is an index in the command queue. The command is
dispatched as soon as the RF core has completed any previous radio operation
command.

@code
#include <ti/devices/${DEVICE_FAMILY}/driverlib/rf_common_cmd.h>

RF_Callback callback = NULL;
RF_EventMask subscribedEvents = 0;
RF_CmdHandle rxCommandHandle = RF_postCmd(rfHandle, (RF_Op*)&RF_cmdRx,
        RF_PriorityNormal, callback, subscribedEvents);

assert(rxCommandHandle != RF_ALLOC_ERROR); // The command queue is full.
@endcode

Command execution happens in background. The calling task may proceed with
other work or execute direct and immediate commands to interact with the
posted radio operation. But beware that the posted command might not have
started, yet. By calling the function #RF_pendCmd() and subscribing events of
the type #RF_EventMask, it is possible to re-synchronize to a posted command:

@code
// RF_EventRxEntryDone must have been subscribed in RF_postCmd().
RF_EventMask events = RF_pendCmd(rfHandle, rxCommandHandle,
        RF_EventRxEntryDone);

// Program proceeds after RF_EventRxEntryDone or after a termination event.
@endcode

The function #RF_runCmd() is a combination of both, #RF_postCmd() and
#RF_pendCmd() and allows synchronous execution.

A pending or already running command might be aborted at any time by calling
the function #RF_cancelCmd() or #RF_flushCmd(). These functions take command
handles as parameters, but can also just abort anything in the RF driver's
queue:

@code
uint8_t abortGraceful = 1;

// Abort a single command
RF_cancelCmd(rfHandle, rxCommandHandle, abortGraceful);

// Abort anything
RF_flushCmd(rfHandle, RF_CMDHANDLE_FLUSH_ALL, abortGraceful);
@endcode

When aborting a command, the return value of #RF_runCmd() or #RF_pendCmd()
will contain the termination reason in form of event flags. If the command is
in the RF driver queue, but has not yet start, the #RF_EventCmdCancelled event is
raised.

<hr>
@anchor rf_event_callbacks
Event callbacks
===============

The RF core generates multiple interrupts during command execution. The RF
driver maps these interrupts 1:1 to callback events of the type #RF_EventMask.
Hence, it is unnecessary to implement own interrupt handlers. Callback events
are divided into 3 groups:

- Command-specific events, documented for each radio operation command. An example
  is the #RF_EventRxEntryDone for the `CMD_PROP_RX`.

- Generic events, defined for all radio operations and originating on the RF core.
  These are for instance #RF_EventCmdDone and #RF_EventLastCmdDone. Both events
  indicate the termination of one or more RF operations.

- Generic events, defined for all radio operations and originating in the RF driver,
  for instance #RF_EventCmdCancelled.

@sa @ref RF_Core_Events, @ref RF_Driver_Events.

How callback events are subscribed was shown in the previous section. The
following snippet shows a typical event handler callback for a proprietary RX
operation:

@code
void rxCallback(RF_Handle handle, RF_CmdHandle command, RF_EventMask events)
{
    if (events & RF_EventRxEntryDone)
    {
        Semaphore_post(rxPacketSemaphore);
    }
    if (events & RF_EventLastCmdDone)
    {
        // ...
    }
}
@endcode

In addition, the RF driver can generate error and power-up events that do not
relate directly to the execution of a radio command. Such events can be
subscribed by specifying the callback function pointers #RF_Params::pErrCb and
#RF_Params::pPowerCb.

All callback functions run in software interrupt (SWI) context. Therefore,
only a minimum amount of code should be executed. When using absolute timed
commands with tight timing constraints, then it is recommended to set the RF
driver SWIs to a high priority.
See @ref rf_setup_and_configuration "Setup and configuration" for more details.

<hr>
@anchor rf_power_management
Power management
================

The RF core is a hardware peripheral and can be switched on and off. The RF
driver handles that automatically and provides the following power
optimization features:

- Lazy power-up and radio setup caching
- Power-down on inactivity
- Deferred dispatching of commands with absolute timing


Lazy power-up and radio setup caching
-------------------------------------

The RF core optimizes the power consumption by enabling the RF core as late as
possible. For instance does #RF_open() not power up the RF core immediately.
Instead, it waits until the first radio operation command is dispatched by
#RF_postCmd() or #RF_runCmd().

The function #RF_open() takes a radio setup command as parameter and expects a
`CMD_FS` command to follow. The pointer to the radio setup command and the
whole `CMD_FS` command are cached internally in the RF driver. They will be
used for every proceeding power-up procedure. Whenever the client re-runs a
setup command, the driver updates its internal cache with the new settings.
RF driver also caches the first CMD_FS from the list of done commands. Please
refer #RF_postCmd() for limitations of command chains.

By default, the RF driver measures the time that it needs for the power-up
procedure and uses that as an estimate for the next power cycle. On the
CC13x0/CC26x0 devices, power-up takes usually 1.6 ms. Automatic measurement
can be suppressed by specifying a custom power-up time with
#RF_Params::nPowerUpDuration. In addition, the client might set
#RF_Params::nPowerUpDurationMargin to cover any uncertainty when doing
automatic measurements. This is necessary in applications with a high hardware
interrupt load which can delay the RF driver's internal state machine
execution.


Power-down on inactivity
------------------------

Whenever a radio operation completes and there is no other radio operation in
the queue, the RF core might be powered down. There are two options in the RF
driver:

- **Automatic power-down** by setting the parameter
  #RF_Params::nInactivityTimeout. The RF core will then start a timer after
  the last command in the queue has completed. The default timeout is "forever"
  and this feature is disabled.

- **Manual power-down** by calling #RF_yield(). The client should do this
  whenever it knows that no further radio operation will be executed for a
  couple of milliseconds.

During the power-down procedure the RF driver stops the radio timer and saves
a synchronization timestamp for the next power-up. This keeps the radio timer
virtually in sync with the RTC even though it is not running all the time. The
synchronization is done in hardware.


Deferred dispatching of commands with absolute timing
-----------------------------------------------------

When dispatching a radio operation command with an absolute start trigger that
is ahead in the future, the RF driver defers the execution and powers the RF
core down until the command is due. It does that only, when:

1. `cmd.startTrigger.triggerType` is set to `TRIG_ABSTIME`

2. The difference between #RF_getCurrentTime() and `cmd.startTime`
   is at not more than 3/4 of a full RAT cycle. Otherwise the driver assumes
   that `cmd.startTime` is in the past.

3. There is enough time to run a full power cycle before `cmd.startTime` is
   due. That includes:

   - the power-down time (fixed value, 1 ms) if the RF core is already
     powered up,

   - the measured power-up duration or the value specified by
     #RF_Params::nPowerUpDuration,

   - the power-up safety margin #RF_Params::nPowerUpDurationMargin
     (the default is 282 microseconds).

If one of the conditions are not fulfilled, the RF core is kept up and
running and the command is dispatched immediately. This ensures, that the
command will execute on-time and not miss the configured start trigger.

<hr>
@anchor rf_scheduling
Preemptive scheduling of RF commands in multi-client applications
=================================================================

Schedule BLE and proprietary radio commands.

@code
RF_Object rfObject_ble;
RF_Object rfObject_prop;

RF_Handle rfHandle_ble, rfHandle_prop;
RF_Params rfParams_ble, rfParams_prop;
RF_ScheduleCmdParams schParams_ble, schParams_prop;

RF_Mode rfMode_ble =
{
  .rfMode      = RF_MODE_MULTIPLE,  // rfMode for dual mode
  .cpePatchFxn = &rf_patch_cpe_ble,
  .mcePatchFxn = 0,
  .rfePatchFxn = &rf_patch_rfe_ble,
};

RF_Mode rfMode_prop =
{
  .rfMode      = RF_MODE_MULTIPLE,  // rfMode for dual mode
  .cpePatchFxn = &rf_patch_cpe_genfsk,
  .mcePatchFxn = 0,
  .rfePatchFxn = 0,
};

// Init RF and specify non-default parameters
RF_Params_init(&rfParams_ble);
rfParams_ble.nInactivityTimeout = 200;     // 200us

RF_Params_init(&rfParams_prop);
rfParams_prop.nInactivityTimeout = 200;    // 200us

// Configure RF schedule command parameters directly.
schParams_ble.priority    = RF_PriorityNormal;
schParams_ble.endTime     = 0;
schParams_ble.allowDelay  = RF_AllowDelayAny;

// Alternatively, use the helper function to configure the default behavior
RF_ScheduleCmdParams_init(&schParams_prop);

// Open BLE and proprietary RF handles
rfHandle_ble  = RF_open(rfObj_ble,  &rfMode_ble,  (RF_RadioSetup*)&RF_cmdRadioSetup,        &rfParams_ble);
rfHandle_prop = RF_open(rfObj_prop, &rfMode_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams_prop);

// Run a proprietary Fs command
RF_runCmd(rfHandle_pro, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, NULL);

// Schedule a proprietary RX command
RF_scheduleCmd(rfHandle_pro, (RF_Op*)&RF_cmdPropRx, &schParams_prop, &prop_callback, RF_EventRxOk);

// Schedule a BLE advertiser command
RF_scheduleCmd(rfHandle_ble, (RF_Op*)&RF_cmdBleAdv, &schParams_ble, &ble_callback,
             (RF_EventLastCmdDone | RF_EventRxEntryDone | RF_EventTxEntryDone));

@endcode

<hr>
@anchor rf_rat
Accessing the Radio Timer (RAT)
==============================

The Radio Timer on the RF core is an independent 32 bit timer running at a
tick rate of 4 ticks per microsecond. It is only physically active while the
RF core is on. But because the RF driver resynchronizes the RAT to the RTC on
every power-up, it appears to the application as the timer is always running.
The RAT accuracy depends on the system HF clock while the RF core is active
and on the LF clock while the RF core is powered down.

The current RAT time stamp can be obtained by #RF_getCurrentTime():

@code
uint32_t now = RF_getCurrentTime();
@endcode

The RAT has 8 independent channels that can be set up in capture and compare
mode by #RF_ratCapture() and #RF_ratCompare() respectively. Three of these
channels are accessible by the RF driver. Each channel may be connected to
physical hardware signals for input and output or may trigger a callback
function.

In order to allocate a RAT channel and trigger a callback function at a
certain time stamp, use #RF_ratCompare():

@code
RF_Handle rfDriver;
RF_RatConfigCompare config;
RF_RatConfigCompare_init(&config);
config.callback = &onRatTriggered;
config.channel = RF_RatChannelAny;
config.timeout = RF_getCurrentTime() + RF_convertMsToRatTicks(1701);

RF_RatHandle ratHandle = RF_ratCompare(rfDriver, &config, nullptr);
assert(ratHandle != RF_ALLOC_ERROR);

void onRatTriggered(RF_Handle h, RF_RatHandle rh, RF_EventMask e, uint32_t compareCaptureTime)
{
    if (e & RF_EventError)
    {
        // RF driver failed to trigger the callback on time.
    }
    printf("RAT has triggered at %u.", compareCaptureTime);

    // Trigger precisely with the same period again
    config.timeout = compareCaptureTime + RF_convertMsToRatTicks(1701);
    ratHandle = RF_ratCompare(rfDriver, &config, nullptr);
    assert(ratHandle != RF_ALLOC_ERROR);
}
@endcode

The RAT may be used to capture a time stamp on an edge of a physical pin. This
can be achieved with #RF_ratCapture().

@code
#include <ti/drivers/pin/PINCC26XX.h>
// Map IO 26 to RFC_GPI0
PINCC26XX_setMux(pinHandle, IOID_26, PINCC26XX_MUX_RFC_GPI0);

RF_Handle rfDriver;
RF_RatConfigCapture config;
RF_RatConfigCapture_init(&config);
config.callback = &onSignalTriggered;
config.channel = RF_RatChannelAny;
config.source = RF_RatCaptureSourceRfcGpi0;
config.captureMode = RF_RatCaptureModeRising;
config.repeat = RF_RatCaptureRepeat;

RF_RatHandle ratHandle = RF_ratCapture(rfDriver, &config, nullptr);
assert(ratHandle != RF_ALLOC_ERROR);

void onSignalTriggered(RF_Handle h, RF_RatHandle rh, RF_EventMask e, uint32_t compareCaptureTime)
{
    if (e & RF_EventError)
    {
        // An internal error has occurred
    }
    printf("Rising edge detected on IO 26 at %u.", compareCaptureTime);
}
@endcode

In both cases, the RAT may generate an output signal when being triggered. The
signal can be routed to a physical IO pin:

@code
// Generate a pulse on an internal RAT output signal
RF_RatConfigOutput output;
RF_RatConfigOutput_init(&output);
output.mode = RF_RatOutputModePulse;
output.select = RF_RatOutputSelectRatGpo3;
RF_ratCompare(...);

// Map RatGpo3 to one of four intermediate doorbell signals.
// This has to be done in the override list in order to take permanent effect.
// The override list can be found in the RF settings .c file exported from
// SmartRF Studio.
// Attention: This will change the default mapping of the PA and LNA signal as well.
#include <ti/devices/[DEVICE_FAMILY]/inc/hw_rfc_dbell.h>
static uint32_t pOverrides[] =
{
    HW_REG_OVERRIDE(0x1110, RFC_DBELL_SYSGPOCTL_GPOCTL2_RATGPO3),
    // ...
}

// Finally, route the intermediate doorbell signal to a physical pin.
#include <ti/drivers/pin/PINCC26XX.h>
PINCC26XX_setMux(pinHandle, IOID_17, PINCC26XX_MUX_RFC_GPO2);
@endcode

<hr>
@anchor rf_tx_power
Programming the TX power level
==============================

The application can program a TX power level for each RF client with the function
#RF_setTxPower(). The new value takes immediate effect if the RF core is up and
running. Otherwise, it is stored in the RF driver client configuration.

TX power may be stored in a lookup table in ascending order. This table is usually
generated and exported from SmartRF Studio together with the rest of the PHY configuration.
A typical power table my look as follows:
@code
RF_TxPowerTable_Entry txPowerTable[] = {
    { .power = 11,  .value = { 0x1233, RF_TxPowerTable_DefaultPA }},
    { .power = 13,  .value = { 0x1234, RF_TxPowerTable_DefaultPA }},
    // ...
    RF_TxPowerTable_TERMINATION_ENTRY
};
@endcode

@note Some devices offer a high-power PA in addition to the default PA.
A client must not mix configuration values in the same power table and must
not hop from a default PA configuration to a high-power PA configuration unless it
can guarantee that the RF setup command is re-executed in between.

Given this power table format, the application may program a new power level in multiple
ways. It can use convenience functions to search a certain power level
in the power table or may access the table index-based:
@code
// Set a certain power level. Search a matching level.
RF_setTxPower(h, RF_TxPowerTable_findValue(txPowerTable, 17));

// Set a certain power level with a known level.
RF_setTxPower(h, txPowerTable[3].value);

// Set a certain power without using a human readable level.
RF_setTxPower(h, value);

// Set maximum power. Search the value.
RF_setTxPower(h, RF_TxPowerTable_findValue(txPowerTable, RF_TxPowerTable_MAX_DBM));

// Set minimum power without searching.
RF_setTxPower(h, txPowerTable[0].value);

// Set minimum power. Search the value.
RF_setTxPower(h, RF_TxPowerTable_findValue(txPowerTable, RF_TxPowerTable_MIN_DBM));

// Set maximum power without searching.
int32_t lastIndex = sizeof(txPowerTable) / sizeof(RF_TxPowerTable_Entry) - 2;
RF_setTxPower(h, txPowerTable[lastIndex].value);
@endcode

The current configured power level for a client can be retrieved by #RF_getTxPower().
@code
// Get the current configured power level.
int8_t power = RF_TxPowerTable_findPowerLevel(txPowerTable, RF_getTxPower(h));
@endcode

<hr>
@anchor rf_temperature_compensation
Temperature Compensation
==============================

The RF driver improves the accuracy of XOSC_HF by performing temperature
dependent compensation. This is commonly done in the BAW/SIP devices where the
compensation parameters are already available inside the package.

When temperature compensation is enabled, RF_enableHPOSCTemperatureCompensation()
is called during the board initialization(in Board_init()). This function enables
the RF driver to update HPOSC_OVERRIDE with the correct frequency offset according
to the ambient temperature at radio setup.

@code
// Enable RF Temperature Compensation
status = RF_enableHPOSCTemperatureCompensation(void)
@endcode

The RF driver also subscribes to a temperature notification event that triggers
for 3 degree Celsius change in temperature. At every 3 degree Celsius change in
temperature, it updates the RF core with the new frequency offset and re-subscribes
to the temperature notification with updated thresholds.

@warning At the moment, temperature compensation is only supported on BAW or SIP
device variants.

Error Handling
--------------
When temperature compensation is enabled, but HPOSC_OVERRIDE is not found, then
RF_open() returns a NULL handle.

RF_enableHPOSCTemperatureCompensation() returns #RF_StatInvalidParamsError if
the temperature notification fails to register.

When the temperature notification fails to register, a global callback can be
executed by subscribing to the event #RF_GlobalEventTempNotifyFail defined in
#RF_GlobalEvent.

@note The #RF_Handle in the global callback function will belong to the current
active client and this client is not causing the failure, since neither the
temperature event nor the failure is client specific.

<hr>
@anchor rf_convenience_features
Convenience features
====================

The RF driver simplifies often needed tasks and provides additional functions.
For instance, it can read the RSSI while the RF core is in RX mode using the
function :tidrivers_api:`RF_getRssi`:

@code
int8_t rssi = RF_getRssi(rfHandle);
assert (rssi != RF_GET_RSSI_ERROR_VAL); // Could not read the RSSI
@endcode

<hr>
 ******************************************************************************
 */

//*****************************************************************************
//
//! \addtogroup rf_driver
//! @{
//! \addtogroup rf_driver_cc13x2_cc26x2
//! @{
//
//*****************************************************************************

#ifndef ti_drivers_rfcc26x2__include
#define ti_drivers_rfcc26x2__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/utils/List.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_prop_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_ble_cmd.h)

/**
 *  @name RF Core Events
 *  @anchor RF_Core_Events
 *
 *  Events originating on the RF core and caused during command execution.
 *  They are aliases for the corresponding interrupt flags.
 *  RF Core Events are command-specific and are explained in the Technical Reference Manual.
 *
 *  @sa RF_postCmd(), RF_pendCmd(), RF_runCmd()
 *  @{
 */
#define   RF_EventCmdDone             (1 << 0)   ///< A radio operation command in a chain finished.
#define   RF_EventLastCmdDone         (1 << 1)   ///< A stand-alone radio operation command or the last radio operation command in a chain finished.
#define   RF_EventFGCmdDone           (1 << 2)   ///< A IEEE-mode radio operation command in a chain finished.
#define   RF_EventLastFGCmdDone       (1 << 3)   ///< A stand-alone IEEE-mode radio operation command or the last command in a chain finished.
#define   RF_EventTxDone              (1 << 4)   ///< Packet transmitted
#define   RF_EventTXAck               (1 << 5)   ///< ACK packet transmitted
#define   RF_EventTxCtrl              (1 << 6)   ///< Control packet transmitted
#define   RF_EventTxCtrlAck           (1 << 7)   ///< Acknowledgement received on a transmitted control packet
#define   RF_EventTxCtrlAckAck        (1 << 8)   ///< Acknowledgement received on a transmitted control packet, and acknowledgement transmitted for that packet
#define   RF_EventTxRetrans           (1 << 9)   ///< Packet retransmitted
#define   RF_EventTxEntryDone         (1 << 10)  ///< Tx queue data entry state changed to Finished
#define   RF_EventTxBufferChange      (1 << 11)  ///< A buffer change is complete
#define   RF_EventPaChanged           (1 << 14)  ///< The PA was reconfigured on the fly.
#define   RF_EventSamplesEntryDone    (1 << 15)  ///< CTE data has been copied, only valid if autocopy feature is enabled
#define   RF_EventRxOk                (1 << 16)  ///< Packet received with CRC OK, payload, and not to be ignored
#define   RF_EventRxNOk               (1 << 17)  ///< Packet received with CRC error
#define   RF_EventRxIgnored           (1 << 18)  ///< Packet received with CRC OK, but to be ignored
#define   RF_EventRxEmpty             (1 << 19)  ///< Packet received with CRC OK, not to be ignored, no payload
#define   RF_EventRxCtrl              (1 << 20)  ///< Control packet received with CRC OK, not to be ignored
#define   RF_EventRxCtrlAck           (1 << 21)  ///< Control packet received with CRC OK, not to be ignored, then ACK sent
#define   RF_EventRxBufFull           (1 << 22)  ///< Packet received that did not fit in the Rx queue
#define   RF_EventRxEntryDone         (1 << 23)  ///< Rx queue data entry changing state to Finished
#define   RF_EventDataWritten         (1 << 24)  ///< Data written to partial read Rx buffer
#define   RF_EventNDataWritten        (1 << 25)  ///< Specified number of bytes written to partial read Rx buffer
#define   RF_EventRxAborted           (1 << 26)  ///< Packet reception stopped before packet was done
#define   RF_EventRxCollisionDetected (1 << 27)  ///< A collision was indicated during packet reception
#define   RF_EventModulesUnlocked     (1 << 29)  ///< As part of the boot process, the CM0 has opened access to RF core modules and memories
#define   RF_EventInternalError       (uint32_t)(1 << 31) ///< Internal error observed
#define   RF_EventMdmSoft             0x0000002000000000  ///< Synchronization word detected (MDMSOFT interrupt flag)
/** @}*/

/**
 *  @name RF Driver Events
 *  @anchor RF_Driver_Events
 *
 *  Event flags generated by the RF Driver.
 *  @{
 */
#define   RF_EventCmdCancelled        0x1000000000000000  ///< Command canceled before it was started.
#define   RF_EventCmdAborted          0x2000000000000000  ///< Abrupt command termination caused by RF_cancelCmd() or RF_flushCmd().
#define   RF_EventCmdStopped          0x4000000000000000  ///< Graceful command termination caused by RF_cancelCmd() or RF_flushCmd().
#define   RF_EventRatCh               0x0800000000000000  ///< A user-programmable RAT channel triggered an event.
#define   RF_EventPowerUp             0x0400000000000000  ///< RF power up event. \deprecated This event is deprecated. Use #RF_ClientEventPowerUpFinished instead.
#define   RF_EventError               0x0200000000000000  ///< Event flag used for error callback functions to indicate an error. See RF_Params::pErrCb.
#define   RF_EventCmdPreempted        0x0100000000000000  ///< Command preempted by another command with higher priority. Applies only to multi-client applications.
/** @}*/

/**
 *  @name Control codes for driver configuration
 *  @anchor RF_CTRL
 *
 *  Control codes are used in RF_control().
 *
 *  @{
 */

/*!
 * @brief Control code used by RF_control to set inactivity timeout
 *
 * Setting this control allows RF to power down the radio upon completion of a radio
 * command after a specified timeout period (in us)
 * With this control code @b arg is a pointer to the timeout variable and returns RF_StatSuccess.
 */
#define RF_CTRL_SET_INACTIVITY_TIMEOUT            0
/*!
 * @brief Control code used by RF_control to update setup command
 *
 * Setting this control notifies RF that the setup command is to be updated, so that RF will take
 * proper actions when executing the next setup command.
 * Note the updated setup command will take effect in the next power up cycle when RF executes the
 * setup command. Prior to updating the setup command, user should make sure all pending commands
 * have completed.
 */
#define RF_CTRL_UPDATE_SETUP_CMD                  1
/*!
 * @brief Control code used by RF_control to set powerup duration margin
 *
 * Setting this control updates the powerup duration margin. Default is RF_DEFAULT_POWER_UP_MARGIN.
 */
#define RF_CTRL_SET_POWERUP_DURATION_MARGIN       2
/*!
 * @brief Control code used by RF_control to set the phy switching margin
 *
 * Setting this control updates the phy switching duration margin, which is used to calculate when
 * run-time conflicts shall be evaluated in case of colliding radio operations issued from two
 * different clients. Default is RF_DEFAULT_PHY_SWITCHING_MARGIN.
 */
#define RF_CTRL_SET_PHYSWITCHING_DURATION_MARGIN  3
/*!
 * @brief Control code used by RF_control to set max error tolerance for RAT/RTC
 *
 * Setting this control updates the error tol for how frequently the CMD_RAT_SYNC_STOP is sent.
 * Default is RF_DEFAULT_RAT_RTC_ERR_TOL_IN_US (5 us)
 * Client is recommeneded to change this setting before sending any commands.
 */
#define RF_CTRL_SET_RAT_RTC_ERR_TOL_VAL           4
/*!
 * @brief Control code used by RF_control to set power management
 *
 * Setting this control configures RF driver to enable or disable power management.
 * By default power management is enabled.
 * If disabled, once RF core wakes up, RF driver will not go to standby and will not power down RF core.
 * To configure power management, use this control to pass a parameter value of 0 to disable power management,
 * and pass a parameter value of 1 to re-enable power management.
 * This control is valid for dual-mode code only. Setting this control when using single-mode code has no effect
 * (power management always enabled).
 */
#define RF_CTRL_SET_POWER_MGMT                    5
/*!
 * @brief Control code used by RF_control to set the hardware interrupt priority level of the RF driver.
 *
 * This control code sets the hardware interrupt priority level that is used by the RF driver. Valid
 * values are INT_PRI_LEVEL1 (highest) until INT_PRI_LEVEL7 (lowest). The default interrupt priority is
 * set in the board support file. The default value is -1 which means "lowest possible priority".
 *
 * When using the TI-RTOS kernel, INT_PRI_LEVEL0 is reserved for zero-latency interrupts and must not be used.
 *
 * Execute this control code only while the RF core is powered down and the RF driver command queue is empty.
 * This is usually the case after calling RF_open(). Changing the interrupt priority level while the RF driver
 * is active will result in RF_StatBusyError being returned.
 *
 * Example:
 * @code
 * #include DeviceFamily_constructPath(driverlib/interrupt.h)
 *
 * int32_t hwiPriority = INT_PRI_LEVEL5;
 * RF_control(rfHandle, RF_CTRL_SET_HWI_PRIORITY, &hwiPriority);
 * @endcode
 */
#define RF_CTRL_SET_HWI_PRIORITY                  6
/*!
 * @brief Control code used by RF_control to set the software interrupt priority level of the RF driver.
 *
 * This control code sets the software interrupt priority level that is used by the RF driver. Valid
 * values are integers starting at 0 (lowest) until <tt>Swi_numPriorities - 1</tt> (highest). The default
 * interrupt priority is set in the board support file. The default value is 0 which means means
 * "lowest possible priority".
 *
 * Execute this control code only while the RF core is powered down and the RF driver command queue is empty.
 * This is usually the case after calling RF_open(). Changing the interrupt priority level while the RF driver
 * is active will result in RF_StatBusyError being returned.
 *
 * Example:
 * @code
 * #include <ti/sysbios/knl/Swi.h>
 *
 * // Set highest possible priority
 * uint32_t swiPriority = ~0;
 * RF_control(rfHandle, RF_CTRL_SET_SWI_PRIORITY, &swiPriority);
 * @endcode
 */
#define RF_CTRL_SET_SWI_PRIORITY                  7
/*!
 * @brief Control code used by RF_control to mask the available RAT channels manually.
 *
 * This control code can be used to manually disallow/allow access to certain RAT channels from the RAT APIs.
 * A typical use case is when a RAT channel is programmed through chained radio operations, and hence is
 * used outside the scope of the RF driver. By disallowing access to this channel one can prevent collision
 * between the automatic channel allocation through #RF_ratCompare()/#RF_ratCapture() and the direct
 * configuration through #RF_postCmd().
 */
#define RF_CTRL_SET_AVAILABLE_RAT_CHANNELS_MASK   8
/*!
 * @brief Control code used by RF_control to enable or disable the coexistence feature at runtime
 *
 * This control code can be used to manually override the statically configured setting for global enable/disable
 * of the coexistence feature. It will have no effect if coexistence is not originally enabled and included
 * in the compiled project.
 *
 * Example:
 * @code
 * // Disable the CoEx feature
 * uint32_t coexEnabled = 0;
 * RF_control(rfHandle, RF_CTRL_COEX_CONTROL, &coexEnabled);
 * @endcode
 */
#define RF_CTRL_COEX_CONTROL  9
/** @}*/

/**
 * @name TX Power Table defines
 * @{
 */

/**
 * Refers to the the minimum available power in dBm when accessing a power
 * table.
 *
 * \sa #RF_TxPowerTable_findValue()
 */
#define RF_TxPowerTable_MIN_DBM   -128

/**
 * Refers to the the maximum available power in dBm when accessing a power
 * table.
 *
 * \sa #RF_TxPowerTable_findValue()
 */
#define RF_TxPowerTable_MAX_DBM   126

/**
 * Refers to an invalid power level in a TX power table.
 *
 * \sa #RF_TxPowerTable_findPowerLevel()
 */
#define RF_TxPowerTable_INVALID_DBM   127

/**
 * Refers to an invalid power value in a TX power table.
 *
 * This is the raw value part of a TX power configuration. In order to check
 * whether a given power configuration is valid, do:
 *
 * @code
 * RF_TxPowerTable_Value value = ...;
 * if (value.rawValue == RF_TxPowerTable_INVALID_VALUE) {
 *     // error, value not valid
 * }
 * @endcode
 *
 * A TX power table is always terminated by an invalid power configuration.
 *
 * \sa #RF_getTxPower(), RF_TxPowerTable_findValue
 */
#define RF_TxPowerTable_INVALID_VALUE 0x3fffff

/**
 * Marks the last entry in a TX power table.
 *
 * In order to use #RF_TxPowerTable_findValue() and #RF_TxPowerTable_findPowerLevel(),
 * every power table must be terminated by a %RF_TxPowerTable_TERMINATION_ENTRY:
 *
 * @code
 * RF_TxPowerTable_Entry txPowerTable[] =
 * {
 *     { 20,  RF_TxPowerTable_HIGH_PA_ENTRY(1, 2, 3) },
 *     // ... ,
 *     RF_TxPowerTable_TERMINATION_ENTRY
 * };
 * @endcode
 */
#define RF_TxPowerTable_TERMINATION_ENTRY \
        { .power = RF_TxPowerTable_INVALID_DBM, .value = { .rawValue = RF_TxPowerTable_INVALID_VALUE, .paType = RF_TxPowerTable_DefaultPA } }

/**
 * Creates a TX power table entry for the default PA.
 *
 * The values for \a bias, \a gain, \a boost and \a coefficient are usually measured by Texas Instruments
 * for a specific front-end configuration. They can then be obtained from SmartRFStudio.
 */
#define RF_TxPowerTable_DEFAULT_PA_ENTRY(bias, gain, boost, coefficient) \
        { .rawValue = ((bias) << 0) | ((gain) << 6) | ((boost) << 8) | ((coefficient) << 9), .paType = RF_TxPowerTable_DefaultPA }

/**
 * Creates a TX power table entry for the High-power PA.
 *
 * The values for \a bias, \a ibboost, \a boost, \a coefficient and \a ldoTrim are usually measured by Texas Instruments
 * for a specific front-end configuration. They can then be obtained from SmartRFStudio.
 */
#define RF_TxPowerTable_HIGH_PA_ENTRY(bias, ibboost, boost, coefficient, ldotrim) \
        { .rawValue = ((bias) << 0) | ((ibboost) << 6) | ((boost) << 8) | ((coefficient) << 9) | ((ldotrim) << 16), .paType = RF_TxPowerTable_HighPA }


/** @} */

/**
 * @name Other defines
 * @{
 */
#define RF_GET_RSSI_ERROR_VAL                   (-128)   ///< Error return value for RF_getRssi()
#define RF_CMDHANDLE_FLUSH_ALL                  (-1)     ///< RF command handle to flush all RF commands
#define RF_ALLOC_ERROR                          (-2)     ///< RF command or RAT channel allocation error
#define RF_SCHEDULE_CMD_ERROR                   (-3)     ///< RF command schedule error
#define RF_ERROR_RAT_PROG                       (-255)   ///< A rat channel could not be programmed.
#define RF_ERROR_INVALID_RFMODE                 (-256)   ///< Invalid RF_Mode. Used in error callback.
#define RF_ERROR_CMDFS_SYNTH_PROG               (-257)   ///< Synthesizer error with CMD_FS. Used in error callback. If this error occurred in error callback, user needs to resend CMD_FS to recover. See the device's errata for more details.

#define RF_NUM_SCHEDULE_ACCESS_ENTRIES          2        ///< Number of access request entries
#define RF_NUM_SCHEDULE_COMMAND_ENTRIES         8        ///< Number of scheduled command entries
#define RF_NUM_SCHEDULE_MAP_ENTRIES             (RF_NUM_SCHEDULE_ACCESS_ENTRIES + RF_NUM_SCHEDULE_COMMAND_ENTRIES) ///< Number of schedule map entries. This is the sum of access request and scheduled command entries
#define RF_SCH_MAP_CURRENT_CMD_OFFSET           RF_NUM_SCHEDULE_ACCESS_ENTRIES      ///< Offset of the current command entry in the schedule map
#define RF_SCH_MAP_PENDING_CMD_OFFSET           (RF_SCH_MAP_CURRENT_CMD_OFFSET + 2) ///< Offset of the first pending command entry in the schedule map

#define RF_ABORT_PREEMPTION                     (1<<2)   ///< Used with RF_cancelCmd() to provoke subscription to RadioFreeCallback
#define RF_ABORT_GRACEFULLY                     (1<<0)   ///< Used with RF_cancelCmd() for graceful command termination

#define RF_SCH_CMD_EXECUTION_TIME_UNKNOWN       0        ///< For unknown execution time for RF scheduler

#define RF_RAT_ANY_CHANNEL                      (-1)     ///< To be used within the channel configuration structure. Allocate any of the available channels.
#define RF_RAT_TICKS_PER_US                     4        ///< Radio timer (RAT) ticks per microsecond.

#define RF_LODIVIDER_MASK                       0x7F     ///< Mask to be used to determine the effective value of the setup command's loDivider field.

/**
 *  @name Stack ID defines
 *  @anchor RF_Stack_ID
 *
 *  Reserved values to identify which stack owns an RF_Handle h (stored as h->clientConfig.nID)
 *  @{
 */
#define RF_STACK_ID_DEFAULT                     0x00000000  ///< No value is set.
#define RF_STACK_ID_154                         0x8000F154  ///< ID for TI 15.4 Stack
#define RF_STACK_ID_BLE                         0x8000FB1E  ///< ID for TI BLE Stack
#define RF_STACK_ID_EASYLINK                    0x8000FEA2  ///< ID for TI EasyLink Stack
#define RF_STACK_ID_THREAD                      0x8000FEAD  ///< ID for TI Thread Stack
#define RF_STACK_ID_TOF                         0x8000F00F  ///< ID for TI TOF Stack
#define RF_STACK_ID_CUSTOM                      0x0000FC00  ///< ID for Custom Stack
/** @} */

/*!
\brief Converts a duration given in \a microseconds into radio timer (RAT) ticks.
*/
#define RF_convertUsToRatTicks(microseconds) \
    ((microseconds) * (RF_RAT_TICKS_PER_US))

/*!
\brief Converts a duration given in \a milliseconds into radio timer (RAT) ticks.
*/
#define RF_convertMsToRatTicks(milliseconds) \
    ((milliseconds) * 1000 * (RF_RAT_TICKS_PER_US))

/*!
\brief Converts a duration given in radio timer (RAT) \a ticks into microseconds.
*/
#define RF_convertRatTicksToUs(ticks) \
    ((ticks) / (RF_RAT_TICKS_PER_US))

/*!
\brief Converts a duration given in radio timer (RAT) \a ticks into milliseconds.
*/
#define RF_convertRatTicksToMs(ticks) \
    ((ticks) / (1000 * (RF_RAT_TICKS_PER_US)))


/** @}*/


/**
 * \brief PA configuration value for a certain power level.
 *
 * A %RF_TxPowerTable_Value contains the power amplifier (PA) configuration for a certain power level.
 * It encodes the PA type as well as a raw configuration value for the RF core hardware.
 *
 * \sa #RF_getTxPower(), #RF_setTxPower(), #RF_TxPowerTable_Entry, #RF_TxPowerTable_PAType.
 */
typedef struct {
    uint32_t rawValue:22;      ///< Hardware configuration value.
                               ///<
                               ///< - \c [15:0] used for default PA,
                               ///< - \c [21:0] used for High-power PA
    uint32_t __dummy:9;
    uint32_t paType:1;         ///< Selects the PA type to be used.
                               ///<
                               ///< - 0: #RF_TxPowerTable_DefaultPA
                               ///< - 1: #RF_TxPowerTable_HighPA
} RF_TxPowerTable_Value;

/**
 * \brief TX power configuration entry in a TX power table.
 *
 * A %RF_TxPowerTable_Entry defines an entry in a lookup table. Each entry contains a
 * human-readable power level \a power as key and a hardware configuration \a value.
 *
 * Example of a typical power table:
 * \code
 * RF_TxPowerTable_Entry txPowerTable[] = {
 *    { .power = 20,  .value = { .rawValue = 0x1234, .paType = RF_TxPowerTable_HighPA }},
 *    { .power = 19,  .value = { .rawValue = 0x1233, .paType = RF_TxPowerTable_HighPA }},
 *    // ...
 *    RF_TxPowerTable_TERMINATION_ENTRY
 * };
 * \endcode
 *
 * \sa #RF_TxPowerTable_findPowerLevel(), #RF_TxPowerTable_findPowerLevel()
 */
typedef struct
{
    int8_t power;                 ///< Human readable power value representing
                                  ///< the output in dBm.

    RF_TxPowerTable_Value value;  ///< PA hardware configuration for that power level.
} __attribute__((packed)) RF_TxPowerTable_Entry;


/**
 * \brief Selects a power amplifier path in a TX power value.
 *
 * %RF_TxPowerTable_PAType selects one of the available power amplifiers
 * on the RF core. It is usually included in a #RF_TxPowerTable_Value.
 */
typedef enum {
    RF_TxPowerTable_DefaultPA = 0, ///< Default PA
    RF_TxPowerTable_HighPA = 1,    ///< High-power PA
} RF_TxPowerTable_PAType;


/** @brief Base type for all radio operation commands.
 *
 *  All radio operation commands share a common part.
 *  That includes the command id, a status field, chaining properties
 *  and a start trigger.
 *  Whenever an RF operation command is used with the RF driver, it needs
 *  to be casted to an RF_Op.
 *
 *  More information about RF operation commands can be found in the Proprietary RF
 *  User's Guide.
 *
 *  @sa RF_runCmd(), RF_postCmd(), RF_pendCmd()
 */
typedef rfc_radioOp_t RF_Op;


/** @brief Specifies a RF core firmware configuration.
 *
 *  %RF_Mode selects a mode of operation and points to firmware patches for the RF core.
 *  There exists one instance per radio PHY configuration, usually generated by
 *  SmartRF Studio.
 *  After assigning %RF_Mode configuration to the RF driver via RF_open(), the
 *  driver caches the containing information and re-uses it on every power-up.
 */
typedef struct {
    uint8_t rfMode;             ///< Specifies which PHY modes should be activated. Must be set to RF_MODE_MULTIPLE for dual-mode operation.
    void (*cpePatchFxn)(void);  ///< Pointer to CPE patch function
    void (*mcePatchFxn)(void);  ///< Pointer to MCE patch function
    void (*rfePatchFxn)(void);  ///< Pointer to RFE patch function
} RF_Mode;

/** @brief Scheduling priority of RF operation commands.
 *
 *  When multiple RF driver instances are used at the same time,
 *  commands from different clients may overlap.
 *  If an RF operation with a higher priority than the currently
 *  running operation is scheduled by RF_scheduleCmd(), then the
 *  running operation is interrupted.
 *
 *  In single-client applications, %RF_PriorityNormal should be used.
 */
typedef enum {
    RF_PriorityHighest = 2, ///< Highest priority. Only use this for urgent commands.
    RF_PriorityHigh    = 1, ///< High priority. Use this for time-critical commands in synchronous protocols.
    RF_PriorityNormal  = 0, ///< Default priority. Use this in single-client applications.
} RF_Priority;

/**
 *  @brief Priority level for coexistence priority signal.
 *
 *  When the RF driver is configured for three-wire coexistence mode, one of the
 *  output wires will signal the priority level of the coexistence request. When
 *  RF operations are scheduled with RF_scheduleCmd(), the scheduler can be configured
 *  to override the default coexistence priority level for the RF operation.
 *
 *  The coexistence priority level is binary because it translates to a high/low output signal.
 */
typedef enum {
    RF_PriorityCoexDefault  = 0,  ///< Default priority. Use value configured by setup command.
    RF_PriorityCoexLow      = 1,  ///< Low priority. Override default value configured by setup command.
    RF_PriorityCoexHigh     = 2,  ///< High priority. Override default value configured by setup command.
} RF_PriorityCoex;

/**
 *  @brief Behavior for coexistence request signal.
 *
 *  When the RF driver is configured for three-wire coexistence mode, one of the
 *  output wires will signal the request level of the coexistence request. When
 *  RF operations are scheduled with RF_scheduleCmd(), the scheduler can be configured
 *  to override the default coexistence request line behavior for the RF operation in RX.
 *
 *  This override will be ignored if the option to set request for an entire chain is active.
 */
typedef enum {
    RF_RequestCoexDefault     = 0, ///< Default request line behavior. Use value configured by setup command.
    RF_RequestCoexAssertRx    = 1, ///< Assert REQUEST in RX. Override default value configured by setup command.
    RF_RequestCoexNoAssertRx  = 2, ///< Do not assert REQUEST in RX. Override default value configured by setup command.
} RF_RequestCoex;

/**
 *  @brief Runtime coexistence override parameters
 *
 *  When  RF operations are scheduled with RF_scheduleCmd(), the scheduler can be configured
 *  to override the default coexistence behavior. This structure encapsulates the available parameters.
 */
typedef struct {
    RF_PriorityCoex   priority;   ///< Priority level for coexistence priority signal.
    RF_RequestCoex    request;    ///< Behavior for coexistence request signal.
} RF_CoexOverride;

/**
 *  @brief Coexistence override settings for BLE5 application scenarios
 *
 *  This configuration is provided to the BLE Stack to override the default coexistence configuration
 *  depending on the current application and stack states.
 */
typedef struct {
    RF_CoexOverride    bleInitiator;
    RF_CoexOverride    bleConnected;
    RF_CoexOverride    bleBroadcaster;
    RF_CoexOverride    bleObserver;
} RF_CoexOverride_BLEUseCases;

/** @brief Status codes for various RF driver functions.
 *
 *  RF_Stat is reported as return value for RF driver functions which
 *  execute direct and immediate commands.
 *  Such commands are executed by RF_runDirectCmd() and RF_runImmediateCmd() in the
 *  first place, but also by some convenience functions like RF_cancelCmd(),
 *  RF_flushCmd(), RF_getInfo() and others.
 */
typedef enum {
    RF_StatBusyError,          ///< Command not executed because RF driver is busy.
    RF_StatRadioInactiveError, ///< Command not executed because RF core is powered down.
    RF_StatCmdDoneError,       ///< Command finished with an error.
    RF_StatInvalidParamsError, ///< Function was called with an invalid parameter.
    RF_StatCmdEnded,           ///< Cmd is found in the pool but was already ended.
    RF_StatError   = 0x80,     ///< General error specifier.
    RF_StatCmdDoneSuccess,     ///< Command finished with success.
    RF_StatCmdSch,             ///< Command successfully scheduled for execution.
    RF_StatSuccess             ///< Function finished with success.
} RF_Stat;

/** @brief Data type for events during command execution.
 *
 *  Possible event flags are listed in @ref RF_Core_Events and @ref RF_Driver_Events.
 */
typedef uint64_t RF_EventMask;

/** @brief A unified type for radio setup commands of different PHYs.
 *
 *  Radio setup commands are used to initialize a PHY on the RF core.
 *  Various partially similar commands exist, each one represented
 *  by a different data type.
 *  RF_RadioSetup is a generic container for all types.
 *  A specific setup command is usually exported from SmartRF Studio
 *  and then passed to the RF driver in RF_open().
 */
typedef union {
    rfc_command_t                      commandId;   ///< Generic command identifier. This is the first field
                                                    ///< in every radio operation command.
    rfc_CMD_RADIO_SETUP_t              common;      ///< Radio setup command for BLE and IEEE modes
    rfc_CMD_BLE5_RADIO_SETUP_t         ble5;        ///< Radio setup command for BLE5 mode
    rfc_CMD_PROP_RADIO_SETUP_t         prop;        ///< Radio setup command for PROPRIETARY mode on 2.4 GHz
    rfc_CMD_PROP_RADIO_DIV_SETUP_t     prop_div;    ///< Radio setup command for PROPRIETARY mode on Sub-1 Ghz
    rfc_CMD_RADIO_SETUP_PA_t           common_pa;   ///< Radio setup command for BLE and IEEE modes with High Gain PA
    rfc_CMD_BLE5_RADIO_SETUP_PA_t      ble5_pa;     ///< Radio setup command for BLE5 mode with High Gain PA
    rfc_CMD_PROP_RADIO_SETUP_PA_t      prop_pa;     ///< Radio setup command for PROPRIETARY mode on 2.4 GHz with High Gain PA
    rfc_CMD_PROP_RADIO_DIV_SETUP_PA_t  prop_div_pa; ///< Radio setup command for PROPRIETARY mode on Sub-1 Ghz with High Gain PA
} RF_RadioSetup;

/** @brief Client-related RF driver events.
 *
 *  Events originating in the RF driver but not directly related to a specific radio command,
 *  are called client events.
 *  Clients may subscribe to these events by specifying a callback function RF_Params::pClientEventCb.
 *  Events are activated by specifying a bitmask RF_Params::nClientEventMask.
 *  The callback is called separately for every event providing an optional argument.
 *
 *  @code
 *  void onClientEvent(RF_Handle h, RF_ClientEvent event, void* arg)
 *  {
 *      switch (event)
 *      {
 *      case RF_ClientEventPowerUpFinished:
 *          // Set output port
 *          break;
 *      default:
 *          // Unsubscribed events must not be issued.
 *          assert(false);
 *      }
 *  }
 *
 *  RF_Params params;
 *  params.pClientEventCb = &onClientEvent;
 *  params.nClientEventMask = RF_ClientEventPowerUpFinished;
 *  RF_open(...);
 *  @endcode
 */
typedef enum {
    RF_ClientEventPowerUpFinished     = (1 << 0),   ///< The RF core has been powered up the radio setup has been finished.
    RF_ClientEventRadioFree           = (1 << 1),   ///< Radio becomes free after a command has been preempted by a high-priority command of another client.
                                                    ///< This event is only triggered on a client that has been preempted.
                                                    ///< Clients may use this event to retry running their low-priority RF operation.

    RF_ClientEventSwitchClientEntered = (1 << 2)    ///< Signals the client that the RF driver is about to switch over from another client.
} RF_ClientEvent;

/** @brief Global RF driver events.
 *
 *  The RF driver provides an interface through the global \c RFCC26XX_hwAttrs
 *  struct to register a global, client independent callback. This callback is
 *  typically used to control board related configurations such as antenna
 *  switches.
 *
 *  @code
 *  void globalCallback(RF_Handle h, RF_GlobalEvent event, void* arg)
 *  {
 *      switch (event)
 *      {
 *      case RF_GlobalEventRadioSetup:
 *      {
 *          RF_RadioSetup* setupCommand = (RF_RadioSetup*)arg;
 *          // Select antenna path
 *          if (setupCommand->common.commandNo == CMD_PROP_RADIO_DIV_SETUP) {
 *              // Sub-1 GHz ...
 *          } else {
 *              // 2.4 GHz ...
 *          }
 *      }
 *          break;
 *
 *      case RF_GlobalEventRadioPowerDown:
 *          // Disable antenna switch
 *          break;
 *
 *      default:
 *          // Unsubscribed events must not be issued.
 *          assert(false);
 *      }
 *  }
 *  @endcode
 *
 *  For the coexistence (coex) feature, some of the events are used to handle
 *  the I/O muxing of the GPIO signals for REQUEST, PRIORITY and GRANT.
 *
 *  @code
 *  void globalCallback(RF_Handle h, RF_GlobalEvent event, void* arg)
 *  {
 *      RF_Cmd* pCurrentCmd = (RF_Cmd*)arg;
 *
 *      if (event & RF_GlobalEventInit) {
 *          // Initialize and mux coex I/O pins to RF Core I/O signals
 *      }
 *      else if (event & RF_GlobalEventCmdStart) {
 *          if (pCurrentCmd->coexPriority != RF_PriorityCoexDefault){
 *              // Release PRIORITY pin from RF Core and set it to value of coexPriority
 *          }
 *      }
 *      else if (event & RF_GlobalEventCmdStop) {
 *          if (pCurrentCmd->coexPriority != RF_PriorityCoexDefault) {
 *              // Mux PRIORITY pin to RF Core signal to return to default priority level
 *          }
 *      }
 *  }
 *  @endcode
 *
 * \sa #RF_GlobalCallback
 */
typedef enum {
    RF_GlobalEventRadioSetup     = (1 << 0),             ///< The RF core is being reconfigured through a setup command.
                                                         ///< The \a arg argument is a pointer to the setup command.
                                                         ///< HWI context.

    RF_GlobalEventRadioPowerDown = (1 << 1),             ///< The RF core is being powered down.
                                                         ///< The \a arg argument is empty.
                                                         ///< SWI context.

    RF_GlobalEventInit           = (1 << 2),             ///< RF_open() is called for the first time (number of registered clients changes from 0 to 1).
                                                         ///< The \a arg argument is empty.
                                                         ///< Task context.

    RF_GlobalEventCmdStart       = (1 << 3),             ///< A command chain is being dispatched to the radio.
                                                         ///< The \a arg argument is a pointer to the current command.
                                                         ///< HWI context.

    RF_GlobalEventCmdStop        = (1 << 4),             ///< Command termination event is handled.
                                                         ///< The \a arg argument is a pointer to the current command.
                                                         ///< HWI context.

    RF_GlobalEventCoexControl    = (1 << 5),             ///< Change to coex configuration is requested
                                                         ///< The \a arg argument is pointer to at least 8-bit wide int with value 1=enable, or 0=disable
                                                         ///< Task/HWI context.

    RF_GlobalEventTempNotifyFail  = (1 << 6),            ///< Registration of temperature notification was unsuccessful
                                                         ///< (failure returned from temperature driver)
                                                         ///< The \a arg argument is empty.
                                                         ///< HWI context

} RF_GlobalEvent;


/** @brief Event mask for combining #RF_ClientEvent event flags in #RF_Params::nClientEventMask.
 *
 */
typedef uint32_t RF_ClientEventMask;

/** @brief Event mask for combining #RF_GlobalEvent event flags in #RFCC26XX_HWAttrsV2::globalEventMask.
 *
 */
typedef uint32_t RF_GlobalEventMask;

/** @brief Command handle that is returned by RF_postCmd().
 *
 *  A command handle is an integer number greater equals zero and identifies
 *  a command container in the RF driver's internal command queue. A client
 *  can dispatch a command with RF_postCmd() and use the command handle
 *  later on to make the RF driver interact with the command.
 *
 *  A negative value has either a special meaning or indicates an error.
 *
 *  @sa RF_pendCmd(), RF_flushCmd(), RF_cancelCmd(), ::RF_ALLOC_ERROR,
 *      ::RF_CMDHANDLE_FLUSH_ALL
 */
typedef int16_t RF_CmdHandle;

/** @struct RF_Object
 *  @brief Stores the client's internal configuration and states.
 *
 *  Before RF_open() can be called, an instance of RF_Object must be created where
 *  the RF driver can store its internal configuration and states.
 *  This object must remain persistent throughout application run-time and must not be
 *  modified by the application.
 *
 *  The size of #RF_Object can be optimized for single-mode applications by providing a
 *  `RF_SINGLEMODE` symbol at compilation time. The pre-built single-mode archive was generated
 *  with this symbol defined, hence any project using this archive must also define `RF_SINGLEMODE`
 *  on project level.
 *
 *  @note Except configuration fields before call to RF_open(), modification of
 *        any field in %RF_Object is forbidden.
 */


/** @cond */

typedef struct RF_ObjectMultiMode RF_Object;

/** Definition of the RF_Object structure for multi mode applications.
 *  It is applicable with the multi mode RF driver through the #RF_Object common type.
 */
struct RF_ObjectMultiMode{
    /// Configuration
    struct {
        uint32_t            nInactivityTimeout;          ///< Inactivity timeout in us.
        RF_Mode*            pRfMode;                     ///< Mode of operation.
        RF_RadioSetup*      pRadioSetup;                 ///< Pointer to the setup command to be executed at power up.
        uint32_t            nPhySwitchingDuration;       ///< Radio reconfiguration time to this client's phy and protocol.
        uint32_t            nPowerUpDuration;            ///< Radio power up time to be used to calculate future wake-up events.
        uint32_t            nPowerUpDurationFs;          ///< Same as nPowerUpDuration, specifically when radio wakes up to execute an FS cmd.
        bool                bMeasurePowerUpDuration;     ///< Indicates if nPowerUpDuration holds a fix value or being measured and updated at every power up.
        bool                bUpdateSetup;                ///< Indicates if an analog configuration update should be performed at the next setup command execution.
        uint16_t            nPowerUpDurationMargin;      ///< Power up duration margin in us.
        void*               pPowerCb;                    ///< \deprecated Power up callback, will go away in future versions, see clientConfig::pClienteventCb instead
        void*               pErrCb;                      ///< Error callback.
        void*               pClientEventCb;              ///< Client event callback.
        RF_ClientEventMask  nClientEventMask;            ///< Client event mask to activate client event callback.
        uint16_t            nPhySwitchingDurationMargin; ///< Phy switching duration margin in us. It is used to calculate when run-time conflicts shall be resolved.
        uint32_t            nID;                         ///< RF handle identifier.
    } clientConfig;
    /// State & variables
    struct {
        struct {
            rfc_CMD_FS_t        cmdFs;              ///< FS command to be executed when the radio is powered up.
        } mode_state;                               ///< (Mode-specific) state structure
        SemaphoreP_Struct       semSync;            ///< Semaphore used by RF_runCmd(), RF_pendCmd() and power down sequence.
        RF_EventMask volatile   eventSync;          ///< Event mask/value used by RF_runCmd() and RF_pendCmd().
        void*                   pCbSync;            ///< Internal storage of user callbacks when RF_runCmd() is used.
        RF_EventMask            unpendCause;        ///< Internal storage of the return value of RF_pendCmd().
        ClockP_Struct           clkReqAccess;       ///< Clock used for request access timeout.
        bool                    bYielded;           ///< Flag indicates that the radio can be powered down at the earliest convenience.
    } state;
};

/** @endcond */

/** @brief A handle that is returned by to RF_open().
 *
 *  %RF_Handle is used for further RF client interaction with the RF driver.
 *  An invalid handle has the value NULL.
 */
typedef RF_Object* RF_Handle;


/** @brief RAT handle that is returned by RF_ratCompare() or RF_ratCapture().
 *
 *  An %RF_RatHandle is an integer number with value greater than or equal to zero and identifies
 *  a Radio Timer Channel in the RF driver's internal RAT module. A client can interact with the
 *  RAT module through the RF_ratCompare(), RF_ratCapture() or RF_ratDisableChannel() APIs.
 *
 *  A negative value indicates an error. A typical example when RF_ratCompare() returns with RF_ALLOC_ERROR.
 */
typedef int8_t RF_RatHandle;

/** @brief Selects the entry of interest in RF_getInfo().
 *
 */
typedef enum {
    RF_GET_CURR_CMD,                              ///< Retrieve a command handle of the current command.
    RF_GET_AVAIL_RAT_CH,                          ///< Create a bitmask showing available RAT channels.
    RF_GET_RADIO_STATE,                           ///< Show the current RF core power state. 0: Radio OFF, 1: Radio ON.
    RF_GET_SCHEDULE_MAP,                          ///< Deprecated. Not supported.
    RF_GET_CLIENT_LIST,                           ///< Provide the client list.
    RF_GET_CLIENT_SWITCHING_TIME,                 ///< Provide the client to client switching times
} RF_InfoType;

/** @brief Stores output parameters for RF_getInfo().
 *
 *  This union structure holds one out of multiple data types.
 *  The contained value is selected by #RF_InfoType.
 */
typedef union {
    RF_CmdHandle ch;                              ///< Command handle (#RF_GET_CURR_CMD).
    uint16_t     availRatCh;                      ///< Available RAT channels (RF_GET_AVAIL_RAT_CH).
    bool         bRadioState;                     ///< Current RF core power state (#RF_GET_RADIO_STATE).
    RF_Handle    pClientList[2];                  ///< Client pointer list, [0]: client 1, [1]: client 2.
    uint32_t     phySwitchingTimeInUs[2];         ///< Phy switching time 0: client 1 -> 2, 1 : client 2 -> 1.
    void         *pScheduleMap;                   ///< Deprecated. Not supported.
} RF_InfoVal;

/** @brief RF schedule map entry structure.
 *
 */
typedef struct {
    RF_CmdHandle ch;                               ///< Command handle
    RF_Handle    pClient;                          ///< Pointer to client object
    uint32_t     startTime;                        ///< Start time (in RAT tick) of the command or access request
    uint32_t     endTime;                          ///< End time (in RAT tick) of the command or access request
    RF_Priority  priority;                         ///< Priority of the command or access request
} RF_ScheduleMapElement;

/** @brief RF schedule map structure.
 *
 */
typedef struct {
    RF_ScheduleMapElement  accessMap[RF_NUM_SCHEDULE_ACCESS_ENTRIES];    ///< Access request schedule map
    RF_ScheduleMapElement  commandMap[RF_NUM_SCHEDULE_COMMAND_ENTRIES];  ///< Command schedule map
} RF_ScheduleMap;

/** @brief Handles events related to RF command execution.
 *
 *  RF command callbacks notify the application of any events happening during RF command execution.
 *  Events may either refer to RF core interrupts (@ref RF_Core_Events) or may be generated by the RF driver
 *  (@ref RF_Driver_Events).
 *
 *  RF command callbacks are set up as parameter to RF_postCmd() or RF_runCmd() and provide:
 *
 *  - the relevant driver client handle \a h which was returned by RF_open(),
 *  - the relevant radio operation command handle \a ch,
 *  - an event mask \a e containing the occurred events.
 *
 *  RF command callbacks are executed in Software Interrupt (SWI) context and must not perform any
 *  blocking operation.
 *  The priority is configurable via #RFCC26XX_HWAttrsV2 in the board file or #RF_CTRL_SET_SWI_PRIORITY in RF_control().
 *
 *  The %RF_Callback function type is also used for signaling power events and
 *  errors.
 *  These are set in #RF_Params::pPowerCb and #RF_Params::pErrCb respectively.
 *  In case of a power event, \a ch can be ignored and \a e has #RF_EventPowerUp set.
 *  In case of an error callback, \a ch contains an error code instead of a command handle and
 *  \a e has the #RF_EventError flag set.
 *
 *  @note Error and power callbacks will be replaced by #RF_ClientCallback in future releases.
 */
typedef void (*RF_Callback)(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);

/** @brief Handles events related to the Radio Timer (RAT).
 *
 *  The RF driver provides an interface to the Radio Timer through RF_ratCompare(), RF_ratCapture() and
 *  RF_ratDisableChannel() APIs. Each API call receives an optional input argument of the type
 *  RF_RatCallback. When a timer event occurs (compare, capture or error events), the registered
 *  callback is invoked.
 *
 *  The RF_RatCallback provides the following argument:
 *  - the relevant driver client handle \a h which was returned by RF_open(),
 *  - the relevant rat timer handle \a rh which the event is caused by,
 *  - an event mask \a e containing the occurred event (RF_EventRatCh or RF_EventError)
 *  - the captured value or the compare time \a compareCaptureTime read from the Radio Timer channel.
 */
typedef void (*RF_RatCallback)(RF_Handle h, RF_RatHandle rh, RF_EventMask e, uint32_t compareCaptureTime);

/**
 *  @brief Handles events related to a driver instance.
 *
 *  The RF driver produces additional events that are not directly related to the execution of a certain command, but
 *  happen during general RF driver operations.
 *  This includes power-up events, client switching events and others.
 *
 *  A client callback provides the following arguments:
 *  - the relevant driver client handle \a h which was returned by RF_open(),
 *  - an event identifier \a event,
 *  - an optional argument \a arg depending on the event.
 *
 *  RF client callbacks are executed in Software Interrupt (SWI) context and must not perform any blocking operation.
 *  The priority is configurable via #RFCC26XX_HWAttrsV2 in the board file or #RF_CTRL_SET_SWI_PRIORITY in RF_control().
 */
typedef void (*RF_ClientCallback)(RF_Handle h, RF_ClientEvent event, void* arg);

/**
 *  @brief Handles global events as part of PHY configuration.
 *
 *  The RF driver serves additional global, client independent events by invoking the #RF_GlobalCallback function
 *  registered through #RFCC26XX_HWAttrsV2::globalCallback in the board file. The function can subscribe to
 *  particular events through the #RFCC26XX_HWAttrsV2::globalEventMask, and receives the following arguments:
 *  - the relevant driver client handle \a h which was returned by RF_open(),
 *  - an event identifier \a event,
 *  - an optional argument \a arg depending on the event.
 *
 *  If multiple events happen at the same time, the callback is always invoked separately for each event.
 *  Depending on the event, the callback might be invoked in SWI or HWI context.
 */
typedef void (*RF_GlobalCallback)(RF_Handle h, RF_GlobalEvent event, void* arg);

/** @brief RF driver configuration parameters.
 *
 *  %RF_Params is used for initial RF driver configuration.
 *  It is initialized by RF_Params_init() and used by RF_open().
 *  Each client has its own set of parameters.
 *  They are reconfigured on a client switch.
 *  Some of the parameters can be changed during run-time using RF_control().
 */
typedef struct {
    uint32_t            nInactivityTimeout;      ///< Inactivity timeout in microseconds.
                                                 ///< The default value is 0xFFFFFFFF (infinite).

    uint32_t            nPowerUpDuration;        ///< A custom power-up duration in microseconds.
                                                 ///< If 0, the RF driver will start with a conservative value and measure the actual time during the first power-up.
                                                 ///< The default value is 0.

    RF_Callback         pPowerCb;                ///< \deprecated Power up callback, will be removed future versions, see RF_Params::pClienteventCb instead.
                                                 ///< The default value is NULL.

    RF_Callback         pErrCb;                  ///< \deprecated Callback function for driver error events.

    uint16_t            nPowerUpDurationMargin;  ///< An additional safety margin to be added to #RF_Params::nPowerUpDuration.
                                                 ///< This is necessary because of other hardware and software interrupts
                                                 ///< preempting the RF driver interrupt handlers and state machine.
                                                 ///< The default value is platform-dependent.

    uint16_t            nPhySwitchingDurationMargin; ///< An additional safety margin to be used to calculate when conflicts shall be evaluated run-time.

    RF_ClientCallback   pClientEventCb;          ///< Callback function for client-related events.
                                                 ///< The default value is NULL.

    RF_ClientEventMask  nClientEventMask;        ///< Event mask used to subscribe certain client events.
                                                 ///< The purpose is to keep the number of callback executions small.

    uint32_t            nID;                     ///< RF handle identifier.
} RF_Params;

/** @brief Controls the behavior of the RF_scheduleCmd() API.
 *
 */
typedef enum {
    RF_StartNotSpecified = 0,
    RF_StartAbs          = 1,
} RF_StartType;

/** @brief Controls the behavior of the RF_scheduleCmd() API.
 *
 */
typedef enum {
    RF_EndNotSpecified = 0,
    RF_EndAbs          = 1,
    RF_EndRel          = 2,
    RF_EndInfinit      = 3,
 } RF_EndType;

/* RF command. */
typedef struct RF_Cmd_s RF_Cmd;

/* RF command . */
struct RF_Cmd_s {
    List_Elem            _elem;       /* Pointer to next and previous elements. */
    RF_Callback volatile pCb;         /* Pointer to callback function */
    RF_Op*               pOp;         /* Pointer to (chain of) RF operations(s) */
    RF_Object*           pClient;     /* Pointer to client */
    RF_EventMask         bmEvent;     /* Enable mask for interrupts from the command */
    RF_EventMask         pastifg;     /* Accumulated value of events happened within a command chain */
    RF_EventMask         rfifg;       /* Return value for callback 0:31 - RF_CPE0_INT, 32:63 - RF_HW_INT */
    RF_CmdHandle         ch;          /* Command handle */
    RF_Priority          ePri;        /* Priority of RF command */
    uint8_t volatile     flags;       /* [0: Aborted, 1: Stopped, 2: canceled] */
    uint32_t             startTime;   /* Command start time (in RAT ticks) */
    RF_StartType         startType;   /* Command start time type */
    uint32_t             allowDelay;  /* Delay allowed if the start time cannot be met. */
    uint32_t             endTime;     /* Command end time (in RAT ticks) */
    RF_EndType           endType;     /* Command end type */
    uint32_t             duration;    /* Command duration (in RAT ticks) */
    uint32_t             activityInfo; /* General value supported by user */
    RF_PriorityCoex      coexPriority; /* Command priority to use for coexistence request. */
    RF_RequestCoex       coexRequest; /* Command REQUEST line behavior to use for coexistence request. */
};

/** @brief RF Hardware attributes.
 *
 *  This data structure contains platform-specific driver configuration.
 *  It is usually defined globally in a board support file.
 */
typedef struct {
    uint8_t             hwiPriority;        ///< Priority for HWIs belong to the RF driver.
    uint8_t             swiPriority;        ///< Priority for SWIs belong to the RF driver.
    bool                xoscHfAlwaysNeeded; ///< Indicate that the XOSC HF should be turned on by the power driver
    RF_GlobalCallback   globalCallback;     ///< Pointer to a callback function serving client independent events listed in #RF_GlobalEvent.
    RF_GlobalEventMask  globalEventMask;    ///< Event mask which the globalCallback is invoked upon.
} RFCC26XX_HWAttrsV2;

/** @brief Controls the behavior of the state machine of the RF driver when a conflict is identified
 *         run-time between the commands waiting on the pend queue and the commands being actively executed
 *         by the radio.
 */
typedef enum
{
    RF_ExecuteActionNone             = 0,  ///< Execute if no conflict, let current command finish if conflict.
    RF_ExecuteActionRejectIncoming   = 1,  ///< Abort the incoming command, letting the ongoing command finish.
    RF_ExecuteActionAbortOngoing     = 2,  ///< Abort the ongoing command and run dispatcher again.
} RF_ExecuteAction;

/** @brief Describes the location within the pend queue where the new command was inserted by the scheduler.
 */
typedef enum
{
    RF_ScheduleStatusError   = -3,
    RF_ScheduleStatusNone    = 0,
    RF_ScheduleStatusTop     = 1,
    RF_ScheduleStatusMiddle  = 2,
    RF_ScheduleStatusTail    = 4,
    RF_ScheduleStatusPreempt = 8
} RF_ScheduleStatus;

/**
 *  @brief Handles the queue sorting algorithm when a new command is submitted to the driver from any of
 *         the active clients.
 *
 *  The function is invoked within the RF_scheduleCmd API.
 *
 *  The default algorithm is subscribed through the #RFCC26XX_SchedulerPolicy::submitHook and implemented
 *  in the RF driver. The arguments are:
 *      - \a pCmdNew points to the command to be submitted.
 *      - \a pCmdBg is the running background command.
 *      - \a pCmdFg is the running foreground command.
 *      - \a pPendQueue points to the head structure of pend queue.
 *      - \a pDoneQueue points to the head structure of done queue.
 *
 *  In case the radio APIs do not distinguish between background and foreground contexts, the active operation
 *  will be returned within the pCmdBg pointer. If there are no commands being executed, both the
 *  pCmdBg and pCmdFg pointers are returned as NULL.
 */
typedef RF_ScheduleStatus (*RF_SubmitHook)(RF_Cmd* pCmdNew, RF_Cmd* pCmdBg, RF_Cmd* pCmdFg, List_List* pPendQueue, List_List* pDoneQueue);

/**
 *  @brief Defines the execution and conflict resolution hook  at runtime.
 *
 *  The function is invoked before a scheduled command is about to be executed.
 *  If a conflict is identified before the start-time of the next radio command
 *  in the pending queue, this information is passed to the hook. The return
 *  value of type #RF_ExecuteAction determines the policy to be followed by the RF
 *  driver.
 *
 *  The arguments are:
 *      - \a pCmdBg is the running background command.
 *      - \a pCmdFg is the running foreground command.
 *      - \a pPendQueue points to the head structure of pend queue.
 *      - \a pDoneQueue points to the head structure of done queue.
 *      - \a bConflict whether the incoming command conflicts with ongoing.
 *      - \a conflictCmd command that conflicts with ongoing.
 */
typedef RF_ExecuteAction (*RF_ExecuteHook)(RF_Cmd* pCmdBg, RF_Cmd* pCmdFg, List_List* pPendQueue, List_List* pDoneQueue, bool bConflict, RF_Cmd* conflictCmd);

/** @brief RF scheduler policy.
 *
 *  This data structure contains function hooks which implements the scheduling
 *  algorithm used to inter-align one or more independent protocol stacks.
 */
typedef struct {
  RF_SubmitHook   submitHook;   ///< Function hook implements the scheduling policy to be executed at the time of RF_scheduleCmd API call.
  RF_ExecuteHook  executeHook; ///< Function hook implements the runtime last second go-no-go execute decision
} RFCC26XX_SchedulerPolicy;

/** @brief Controls the behavior of the RF_scheduleCmd() API.
 *
 */
typedef enum {
    RF_AllowDelayNone = 0,
    RF_AllowDelayAny  = UINT32_MAX
} RF_AllowDelay;

/* @brief RF schedule command parameter struct
 *
 * RF schedule command parameters are used with the RF_scheduleCmd() call.
 */
typedef struct {
    uint32_t            startTime;          ///< Start time in RAT Ticks for the radio command
    RF_StartType        startType;          ///< Start type for the start time
    uint32_t            allowDelay;         ///< Control word to define the policy of the scheduler if the timing of a command cannot be met.
                                            ///< Only applicable on CC13x2 and CC26x2 devices.
                                            ///<  RF_AllowDelayNone: Reject the command.
                                            ///<  RF_AllowDelayAny:  Append the command to the end of the queue.
   uint32_t            endTime;             ///< End time in RAT Ticks for the radio command
   RF_EndType          endType;             ///< End type for the end time
   uint32_t            duration;            ///< Duration in RAT Ticks for the radio command
   uint32_t            activityInfo;        ///< Activity info provided by user
   RF_PriorityCoex     coexPriority;        ///< Priority to use for coexistence request.
   RF_RequestCoex      coexRequest;         ///< REQUEST line behavior to use for coexistence request.
} RF_ScheduleCmdParams;

/** @brief RF request access parameter struct
 *
 *  RF request access command parameters are used with the RF_requestAccess() call.
 */
typedef struct {
    uint32_t            duration;           ///< Radio access duration in RAT Ticks requested by the client
    uint32_t            startTime;          ///< Start time window in RAT Time for radio access
    RF_Priority         priority;           ///< Access priority
} RF_AccessParams;

/** @brief Select the preferred RAT channel through the configuration of #RF_ratCompare() or #RF_ratCapture().
 *
 *  If RF_RatChannelAny is provided within the channel configuration (default), the API will
 *  allocate the first available channel. Otherwise, it tries to allocate the requested channel,
 *  and if it is not available, returns with #RF_ALLOC_ERROR.
 */
typedef enum {
    RF_RatChannelAny = -1,                  ///< Chose the first available channel.
    RF_RatChannel0   =  0,                  ///< Use RAT user channel 0.
    RF_RatChannel1   =  1,                  ///< Use RAT user channel 1.
    RF_RatChannel2   =  2,                  ///< Use RAT user channel 2.
} RF_RatSelectChannel;

/** @brief Selects the source signal for #RF_ratCapture().
 *
 *  The source of a capture event can be selected through the source field of the
 *  #RF_RatConfigCapture configuration structure.
 */
typedef enum {
      RF_RatCaptureSourceRtcUpdate    = 20, ///< Selects the RTC update signal source.
      RF_RatCaptureSourceEventGeneric = 21, ///< Selects the Generic event of Event Fabric as source.
      RF_RatCaptureSourceRfcGpi0      = 22, ///< Selects the RFC_GPI[0] as source. This can be used i.e.
                                            ///< to capture events on a GPIO. This requires that the GPIO
                                            ///< is connected to RFC_GPO[0] from the GPIO driver.
      RF_RatCaptureSourceRfcGpi1      = 23  ///< Selects the RFC_GPO[1] as source. This can be used i.e.
                                            ///< to capture events on a GPIO. This requires that the GPIO
                                            ///< is connected to RFC_GPO[1] from the GPIO driver.
} RF_RatCaptureSource;

/** @brief Selects the mode of #RF_ratCapture().
 *
 *  The trigger mode of a capture event can be selected through the mode field of
 *  #RF_RatConfigCapture configuration structure.
 */
typedef enum {
    RF_RatCaptureModeRising       = 0,      ///< Rising edge of the selected source will trigger a capture event.
    RF_RatCaptureModeFalling      = 1,      ///< Falling edge of the selected source will trigger a capture event.
    RF_RatCaptureModeBoth         = 2       ///< Both rising and falling edges of the selected source will generate
                                            ///< capture events.
} RF_RatCaptureMode;

/** @brief Selects the repetition of #RF_ratCapture().
 *
 *  The configuration of a capture channel also defines whether the channel should be
 *  freed or automatically rearmed after a capture event occurred. In the latter case, the
 *  user needs to free the channel manually through the #RF_ratDisableChannel() API.
 */
typedef enum {
    RF_RatCaptureSingle           = 0,      ///< Free the channel after the first capture event.
    RF_RatCaptureRepeat           = 1       ///< Rearm the channel after each capture events.
} RF_RatCaptureRepetition;

/** @brief Selects the mode of the RAT_GPO[x] for #RF_ratCompare() or #RF_ratCapture().
 *
 *  In case of compare mode, the channel can generate an output signal of the selected
 *  mode on the configured RAT_GPO[x] interface, and can be interconnected with
 *  other subsystems through the RFC_GPO[x] or Event Fabric. An example use case is
 *  to generate a pulse on a GPIO.
 *
 *  In case of capture mode, the channel can also generate an output signal of the
 *  selected mode on the configured RAT_GPO[x] interface. Note that the configuration
 *  of this output event is independent of the source signal of the capture event.
 *  An example use case is to generate a pulse on a GPIO on each raising edge of another
 *  GPIO source.
 *
 */
typedef enum {
    RF_RatOutputModePulse         = 0,      ///< Generates a one-clock period width pulse.
    RF_RatOutputModeSet           = 1,      ///< Sets the output high on a RAT event.
    RF_RatOutputModeClear         = 2,      ///< Sets the output low on a RAT event.
    RF_RatOutputModeToggle        = 3,      ///< Inverts the polarity of the output.
    RF_RatOutputModeAlwaysZero    = 4,      ///< Sets the output low independently of any RAT events.
    RF_RatOutputModeAlwaysOne     = 5,      ///< Sets the output high independently of any RAT events.
} RF_RatOutputMode;

/** @brief Selects GPO to be used with #RF_ratCompare() or #RF_ratCapture().
 *
 *  RAT_GPO[0]   - Reserved by the RF core. User shall not modify the configuration,
 *                 but can observe the signal through any of RFC_GPO[0:3].
 *  RAT_GPO[1]   - Reserved by the RF core only if sync word detection is enabled.
 *                 Otherwise can be used through RFC_GPO[0:3].
 *  RAT_GPO[2:3] - Available and can be used through any of the RFC_GPO[0:3].
 *  RAT_GPO[4:7] - Available and can be used through the Event fabric.
 */
typedef enum {
    RF_RatOutputSelectRatGpo1     = 1,      ///< Configure RAT_CHANNEL[x] to interface with RAT_GPO[1]
    RF_RatOutputSelectRatGpo2     = 2,      ///< Configure RAT_CHANNEL[x] to interface with RAT_GPO[2]
    RF_RatOutputSelectRatGpo3     = 3,      ///< Configure RAT_CHANNEL[x] to interface with RAT_GPO[3]
    RF_RatOutputSelectRatGpo4     = 4,      ///< Configure RAT_CHANNEL[x] to interface with RAT_GPO[4]
    RF_RatOutputSelectRatGpo5     = 5,      ///< Configure RAT_CHANNEL[x] to interface with RAT_GPO[5]
    RF_RatOutputSelectRatGpo6     = 6,      ///< Configure RAT_CHANNEL[x] to interface with RAT_GPO[6]
    RF_RatOutputSelectRatGpo7     = 7,      ///< Configure RAT_CHANNEL[x] to interface with RAT_GPO[7]
} RF_RatOutputSelect;

/** @brief RF_ratCapture parameter structure.
 *
 *  %RF_RatCapture parameters are used with the #RF_ratCapture() call.
 */
typedef struct {
    RF_RatCallback            callback;         ///< Callback function to be invoked upon a capture event (optional).
    RF_RatHandle              channel;          ///< RF_RatHandle identifies the channel to be allocated.
    RF_RatCaptureSource       source;           ///< Configuration of the event source to cause a capture event.
    RF_RatCaptureMode         captureMode;      ///< Configuration of the mode of event to cause a capture event.
    RF_RatCaptureRepetition   repeat;           ///< Configuration of the channel to be used in single or repeated mode.
} RF_RatConfigCapture;

/** @brief RF_ratCompare parameter structure.
 *
 *  %RF_RatCompare parameters are used with the #RF_ratCompare() call.
 */
typedef struct {
    RF_RatCallback            callback;         ///< Callback function to be invoked upon a capture event (optional).
    RF_RatHandle              channel;          ///< RF_RatHandle identifies the channel to be allocated.
    uint32_t                  timeout;          ///< Timeout value in RAT ticks to be programmed in the timer as the
                                                ///< trigger of compare event.
} RF_RatConfigCompare;

/** @brief RAT related IO parameter structure.
 *
 *  These parameters are used with the #RF_ratCompare() or #RF_ratCapture() calls.
 */
typedef struct {
    RF_RatOutputMode          mode;            ///< The mode the GPO should operate in.
    RF_RatOutputSelect        select;          ///< The signal which shall be connected to the GPO.
} RF_RatConfigOutput;

/** @brief Creates a a new client instance of the RF driver.
 *
 *  This function initializes an RF driver client instance using \a pObj as storage.
 *  It does not power up the RF core.
 *  Once the client starts the first RF operation command later in the application,
 *  the RF core is powered up and set into a PHY mode specified by \a pRfMode.
 *  The chosen PHY is then configured by a radio setup command \a pRadioSetup.
 *  Whenever the RF core is powered up, the RF driver re-executes the radio setup command \a pRadioSetup.
 *  Additional driver behavior may be set by an optional \a params.
 *
 *  @code
 *  // Define parameters
 *  RF_Params rfParams;
 *  rfParams.nInactivityTimeout = 4;
 *  RF_Params_init(&rfParams);
 *  rfParams.nInactivityTimeout = 1701; // microseconds
 *
 *  RF_Handle rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
 *  @endcode
 *
 *  @note Calling context : Task
 *
 *  @param pObj         Pointer to a #RF_Object that will hold the state for this
 *                      RF client. The object must be in persistent and writable
 *                      memory.
 *  @param pRfMode      Pointer to a #RF_Mode struct holding PHY information
 *  @param pRadioSetup  Pointer to the radio setup command used for this client.
 *                      This is re-executed by the RF Driver on each power-up.
 *  @param params       Pointer to an RF_Params object with the desired driver configuration.
 *                      A NULL pointer results in the default configuration being loaded.
 *  @return             A handle for further RF driver calls on success. Otherwise NULL.
 */
extern RF_Handle RF_open(RF_Object *pObj, RF_Mode *pRfMode, RF_RadioSetup *pRadioSetup, RF_Params *params);

/**
 *  @brief  Close client connection to RF driver
 *
 *  Allows a RF client (high-level driver or application) to close its connection
 *  to the RF driver.
 *  RF_close pends on all commands in the command queue before closing the connection.
 *  If a client has access to the radio by using RF_RequestAccess API, and the same client calls RF_close,
 *  then the connection to the RF driver is closed immediately without waiting for the access duration to be over.
 *
 *  @note Calling context : Task
 *
 *  @param h  Handle previously returned by RF_open()
 */
extern void RF_close(RF_Handle h);

/**
 *  @brief  Return current radio timer value
 *
 *  If the radio is powered returns the current radio timer value, if not returns
 *  a conservative estimate of the current radio timer value
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @return     Current radio timer value
 */
extern uint32_t RF_getCurrentTime(void);

/**
 *  @brief  Appends RF operation commands to the driver's command queue and returns a
 *          command handle.
 *
 *  The RF operation \a pOp may either represent a single operation or may be the first
 *  operation in a chain.
 *  If the command queue is empty, the \a pCmd is dispatched immediately. If there are
 *  other operations pending, then \a pCmd is processed after all other commands have been
 *  finished.
 *  The RF operation command must be compatible to the RF_Mode selected by RF_open(), e.g.
 *  proprietary commands can only be used when the RF core is configured for proprietary mode.
 *
 *  The returned command handle is an identifier that can be used to control command execution
 *  later on, for instance with RF_pendCmd() or RF_cancelCmd().
 *  It is a 16 Bit signed integer value, incremented on every new command.
 *  If the RF driver runs out of command containers, RF_ALLOC_ERROR is returned.
 *
 *  The priority \a ePri is only relevant in multi-client applications where commands of distinct
 *  clients may interrupt each other.
 *  Only commands started by RF_scheduleCmd() can preempt
 *  running commands. #RF_postCmd() or RF_runCmd() do never interrupt a running command.
 *  In single-client applications, \a ePri is ignored and should be set to ::RF_PriorityNormal.
 *
 *  A callback function \a pCb might be specified to get notified about events during command
 *  execution. Events are subscribed by the bit mask \a bmEvent.
 *  Valid event flags are specified in @ref RF_Core_Events and @ref RF_Driver_Events.
 *  If no callback is set, RF_pendCmd() can be used to synchronize the current task to command
 *  execution. For this it is necessary to subscribe all relevant events.
 *  The termination events ::RF_EventLastCmdDone, ::RF_EventCmdCancelled, ::RF_EventCmdAborted and
 *  ::RF_EventCmdStopped are always implicitly subscribed.
 *
 *  The following limitations apply to the execution of command chains:
 *
 *  - If TRIG_ABSTIME is used as a start trigger for the first command, TRIG_REL_FIRST_START
 *    can not be used for any other command. This is because the RF driver may insert a
 *    frequency-select command (CMD_FS) at the front of the chain when it performs an
 *    automatic power-up.
 *  - If a command chain has more than one CMD_FS, the first CMD_FS in the chain is cached.
 *    This CMD_FS is used on the next automatic power-up.
 *  - To avoid execution of cached CMD_FS and directly use a new CMD_FS on power up, place CMD_FS
 *    at the head of the command chain.
 *
 *  @note Calling context : Task/SWI
 *
 *  @sa RF_pendCmd(), RF_runCmd(), RF_scheduleCmd(), RF_RF_cancelCmd(), RF_flushCmd(), RF_getCmdOp()
 *
 *  @param h         Driver handle previously returned by RF_open()
 *  @param pOp       Pointer to the RF operation command.
 *  @param ePri      Priority of this RF command (used for arbitration in multi-client systems)
 *  @param pCb       Callback function called during command execution and upon completion.
 *                   If RF_postCmd() fails, no callback is made.
 *  @param bmEvent   Bitmask of events that will trigger the callback or that can be pended on.
 *  @return          A handle to the RF command. Return value of RF_ALLOC_ERROR indicates error.
 */
extern RF_CmdHandle RF_postCmd(RF_Handle h, RF_Op *pOp, RF_Priority ePri, RF_Callback pCb, RF_EventMask bmEvent);

/**
 *  @brief  Sorts and adds commands to the RF driver internal command queue.
 *
 *  @param pCmdNew    Pointer to the command to be submitted.
 *  @param pCmdBg     Running background command.
 *  @param pCmdFg     Running foreground command.
 *  @param pPendQueue Pointer to the head structure of pend queue.
 *  @param pDoneQueue Pointer to the head structure of done queue..
 *  @return           RF_defaultSubmitPolicy identifies the success or failure of queuing.
 */
extern RF_ScheduleStatus RF_defaultSubmitPolicy(RF_Cmd* pCmdNew, RF_Cmd* pCmdBg, RF_Cmd* pCmdFg, List_List* pPendQueue, List_List* pDoneQueue);

/**
 *  @brief  Makes a final decision before dispatching a scheduled command.
 *
 *  @param pCmdBg       Running background command.
 *  @param pCmdFg       Running foreground command.
 *  @param pPendQueue   Pointer to the head structure of pend queue.
 *  @param pDoneQueue   Pointer to the head structure of done queue.
 *  @param bConflict    Whether the incoming command conflicts with the ongoing.
 *  @param conflictCmd  Command that conflicts with ongoing.
 *  @return             RF_defaultSubmitPolicy identifies the success or failure of queuing.
 */
extern RF_ExecuteAction RF_defaultExecutionPolicy(RF_Cmd* pCmdBg, RF_Cmd* pCmdFg, List_List* pPendQueue, List_List* pDoneQueue, bool bConflict, RF_Cmd* conflictCmd);


/**
 *  @brief  Initialize the configuration structure to default values to be used with the RF_scheduleCmd() API.
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param  pSchParams Pointer to the configuration structure.
 *  @return none
 */
extern void RF_ScheduleCmdParams_init(RF_ScheduleCmdParams *pSchParams);

/**
 *  @brief  Schedule an RF operation (chain) to the command queue.
 *
 *  Schedule an #RF_Op to the RF command queue of the client with handle h. <br>
 *  The command can be the first in a chain of RF operations or a standalone RF operation.
 *  If a chain of operations are posted they are treated atomically, i.e. either all
 *  or none of the chained operations are run. <br>
 *  All operations must be posted in strictly increasing chronological order. Function returns
 *  immediately. <br>
 *
 *  Limitations apply to the operations posted:
 *  - The operation must be in the set supported in the chosen radio mode when
 *    RF_open() was called
 *  - Only a subset of radio operations are supported
 *  - Only some of the trigger modes are supported with potential power saving (TRIG_NOW, TRIG_ABSTIME)
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h         Handle previously returned by RF_open()
 *  @param pOp       Pointer to the #RF_Op. Must normally be in persistent and writable memory
 *  @param pSchParams Pointer to the schedule command parameter structure
 *  @param pCb       Callback function called upon command completion (and some other events).
 *                   If RF_scheduleCmd() fails no callback is made
 *  @param bmEvent   Bitmask of events that will trigger the callback.
 *  @return          A handle to the RF command. Return value of RF_ALLOC_ERROR indicates error.
 */
extern RF_CmdHandle RF_scheduleCmd(RF_Handle h, RF_Op *pOp, RF_ScheduleCmdParams *pSchParams, RF_Callback pCb, RF_EventMask bmEvent);

/**
 *  @brief  Synchronizes the calling task to an RF operation command \a ch and
 *          returns accumulated event flags.
 *
 *  After having dispatched an RF operation represented by \a ch with RF_postCmd(), the
 *  command is running in parallel on the RF core. Thus, it might be desirable to synchronize
 *  the calling task to the execution of the command.
 *  With #RF_pendCmd(), the application can block until one of the events specified in
 *  \a bmEvent occurs or until the command finishes.
 *  The function consumes and returns all accumulated event flags that occurred during
 *  execution if they have been previously subscribed by RF_postCmd().
 *  Possible events are specified in @ref RF_Core_Events and @ref RF_Driver_Events.
 *  The termination events ::RF_EventLastCmdDone, ::RF_EventCmdCancelled,
 *  ::RF_EventCmdAborted and ::RF_EventCmdStopped are always implicitly subscribed and
 *  can not be masked.
 *
 *  #RF_pendCmd() may be called multiple times for the same command.
 *
 *  If #RF_pendCmd() is called for a command handle representing a finished command,
 *  then only the ::RF_EventLastCmdDone flag is returned, regardless of how the command
 *  finished.
 *
 *  If the command has also a callback set, the callback is executed before #RF_pendCmd()
 *  returns.
 *
 *  Example:
 *  @code
 *  // Dispatch a command to the RF driver's command queue
 *  RF_CmdHandle ch = RF_postCmd(driver, (RF_Op*)&CMD_PROP_RX, RF_PriorityNormal, NULL, RF_EventRxEntryDone);
 *  assert(ch != RF_ALLOC_ERROR);
 *
 *  bool finished = false;
 *  while (finished == false)
 *  {
 *      // Synchronize to events during command execution.
 *      uint32_t events = RF_pendCmd(driver, ch, RF_EventRxEntryDone);
 *      // Check events that happen during execution
 *      if (events & RF_EventRxEntryDone)
 *      {
 *          // Process packet
 *      }
 *      if (events & (RF_EventLastCmdDone | RF_EventCmdStopped | RF_EventCmdAborted | RF_EventCmdCancelled))
 *      {
 *          finished = true;
 *      }
 *      // ...
 *  }
 *  @endcode
 *
 *  @note Calling context : Task
 *
 *  @param h        Driver handle previously returned by RF_open()
 *  @param ch       Command handle previously returned by RF_postCmd().
 *  @param bmEvent  Bitmask of events that make RF_pendCmd() return. Termination events
 *                  are always implicitly subscribed.
 *  @return         Event flags accumulated during command execution.
 *
 *  @sa RF_postCmd()
 */
extern RF_EventMask RF_pendCmd(RF_Handle h, RF_CmdHandle ch, RF_EventMask bmEvent);

/**
 *  @brief  Runs synchronously an RF operation command or a chain of commands and returns
 *          the termination reason.
 *
 *  This function appends an RF operation command or a chain of commands to the RF driver's
 *  command queue and then waits for it to complete.
 *  A command is completed if one of the termination events ::RF_EventLastCmdDone,
 *  ::RF_EventCmdCancelled, ::RF_EventCmdAborted, ::RF_EventCmdStopped occurred.
 *
 *  This function is a combination of RF_postCmd() and RF_pendCmd().
 *  All options and limitations for RF_postCmd() apply here as well.
 *
 *  An application should always ensure that the command completed in the expected way and
 *  with an expected status code.
 *
 *  @note Calling context : Task
 *
 *  @param h         Driver handle previously returned by RF_open()
 *  @param pOp       Pointer to the RF operation command.
 *  @param ePri      Priority of this RF command (used for arbitration in multi-client systems)
 *  @param pCb       Callback function called during command execution and upon completion.
 *                   If RF_runCmd() fails, no callback is made.
 *  @param bmEvent   Bitmask of events that will trigger the callback or that can be pended on.
 *  @return          The relevant termination event.
 *
 *  @sa RF_postCmd(), RF_pendCmd(), RF_cancelCmd(), RF_flushCmd()
 */
extern RF_EventMask RF_runCmd(RF_Handle h, RF_Op *pOp, RF_Priority ePri, RF_Callback pCb, RF_EventMask bmEvent);

/**
 *  @brief  Runs synchronously a (chain of) RF operation(s) for dual or single-mode.
 *
 *  Allows a (chain of) operation(s) to be scheduled to the command queue and then waits
 *  for it to complete. <br> A command is completed if one of the RF_EventLastCmdDone,
 *  RF_EventCmdCancelled, RF_EventCmdAborted, RF_EventCmdStopped occurred.
 *
 *  @note Calling context : Task
 *  @note Only one call to RF_pendCmd() or RF_runScheduleCmd() can be made at a time for
 *        each client
 *
 *  @param h         Handle previously returned by RF_open()
 *  @param pOp       Pointer to the #RF_Op. Must normally be in persistent and writable memory
 *  @param pSchParams Pointer to the schedule command parameter structure
 *  @param pCb       Callback function called upon command completion (and some other events).
 *                   If RF_runScheduleCmd() fails, no callback is made.
 *  @param bmEvent   Bitmask of events that will trigger the callback.
 *  @return          The relevant command completed event.
 */
extern RF_EventMask RF_runScheduleCmd(RF_Handle h, RF_Op *pOp, RF_ScheduleCmdParams *pSchParams, RF_Callback pCb, RF_EventMask bmEvent);

/**
 *  @brief  Abort/stop/cancel single command in command queue.
 *
 *  If command is running, aborts/stops it and posts callback for the
 *  aborted/stopped command. <br>
 *  If command has not yet run, cancels it it and posts callback for the
 *  canceled command. <br>
 *  If command has already run or been aborted/stopped/canceled, has no effect.<br>
 *  If RF_cancelCmd is called from a Swi context with same or higher priority
 *  than RF Driver Swi, when the RF core is powered OFF -> the cancel callback will be delayed
 *  until the next power-up cycle.<br>
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param ch           Command handle previously returned by RF_postCmd().
 *  @param mode         1: Stop gracefully, 0: abort abruptly
 *  @return             RF_Stat indicates if command was successfully completed
 */
extern RF_Stat RF_cancelCmd(RF_Handle h, RF_CmdHandle ch, uint8_t mode);

/**
 *  @brief  Abort/stop/cancel command and any subsequent commands in command queue.
 *
 *  If command is running, aborts/stops it and then cancels all later commands in queue.<br>
 *  If command has not yet run, cancels it and all later commands in queue.<br>
 *  If command has already run or been aborted/stopped/canceled, has no effect.<br>
 *  The callbacks for all canceled commands are issued in chronological order.<br>
 *  If RF_flushCmd is called from a Swi context with same or higher priority
 *  than RF Driver Swi, when the RF core is powered OFF -> the cancel callback will be delayed
 *  until the next power-up cycle.<br>
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param ch           Command handle previously returned by RF_postCmd().
 *  @param mode         1: Stop gracefully, 0: abort abruptly
 *  @return             RF_Stat indicates if command was successfully completed
 */
extern RF_Stat RF_flushCmd(RF_Handle h, RF_CmdHandle ch, uint8_t mode);

/**
 *  @brief Send any Immediate command. <br>
 *
 *  Immediate Command is send to RDBELL, if radio is active and the RF_Handle points
 *  to the current client. <br>
 *  In other appropriate RF_Stat values are returned. <br>
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param pCmdStruct   Pointer to the immediate command structure
 *  @return             RF_Stat indicates if command was successfully completed
 */
extern RF_Stat RF_runImmediateCmd(RF_Handle h, uint32_t *pCmdStruct);

/**
 *  @brief Send any Direct command. <br>
 *
 *  Direct Command value is send to RDBELL immediately, if radio is active and
 *  the RF_Handle point to the current client. <br>
 *  In other appropriate RF_Stat values are returned. <br>
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param cmd          Direct command value.
 *  @return             RF_Stat indicates if command was successfully completed.
 */
extern RF_Stat RF_runDirectCmd(RF_Handle h, uint32_t cmd);

/**
 *  @brief  Signal that radio client is not going to issue more commands in a while. <br>
 *
 *  Hint to RF driver that, irrespective of inactivity timeout, no new further
 *  commands will be issued for a while and thus the radio can be powered down at
 *  the earliest convenience. In case the RF_yield() is called within a callback,
 *  the callback will need to finish and return before the power down sequence is
 *  initiated. Posting new commands to the queue will cancel any pending RF_yield()
 *  request. <br>
 *
 *  @note Calling context : Task
 *
 *  @param h       Handle previously returned by RF_open()
 */
extern void RF_yield(RF_Handle h);

/**
 *  @brief  Function to initialize the RF_Params struct to its defaults.
 *
 *  @param  params      An pointer to RF_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      nInactivityTimeout = BIOS_WAIT_FOREVER
 *      nPowerUpDuration   = RF_DEFAULT_POWER_UP_TIME
 */
extern void RF_Params_init(RF_Params *params);

/**
 *  @brief Get value for some RF driver parameters. <br>
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param type         Request value parameter defined by RF_InfoType
 *  @param pValue       Pointer to return parameter values specified by RF_InfoVal
 *  @return             RF_Stat indicates if command was successfully completed
 */
extern RF_Stat RF_getInfo(RF_Handle h, RF_InfoType type, RF_InfoVal *pValue);

/**
 *  @brief Get RSSI value.
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param  h            Handle previously returned by RF_open()
 *  @return              RSSI value. Return value of RF_GET_RSSI_ERROR_VAL indicates error case.
 */
extern int8_t RF_getRssi(RF_Handle h);

/**
 *  @brief  Get command structure pointer.
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param cmdHnd       Command handle returned by RF_postCmd()
 *  @return             Pointer to the command structure.
 */
extern RF_Op* RF_getCmdOp(RF_Handle h, RF_CmdHandle cmdHnd);

/**
 *  @brief  Initialize the configuration structure to be used to set up a RAT compare event.
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param  channelConfig Pointer to the compare configuration structure.
 *  @return none
 */
extern void RF_RatConfigCompare_init(RF_RatConfigCompare* channelConfig);

/**
 *  @brief  Initialize the configuration structure to be used to set up a RAT capture event.
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param  channelConfig Pointer to the capture configuration structure.
 *  @return none
 */
extern void RF_RatConfigCapture_init(RF_RatConfigCapture* channelConfig);

/**
 *  @brief  Initialize the configuration structure to be used to set up a RAT IO.
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param  ioConfig Pointer to the IO configuration structure.
 *  @return none
 */
extern void RF_RatConfigOutput_init(RF_RatConfigOutput* ioConfig);

/**
 *  @brief  Setup a Radio Timer (RAT) channel in compare mode.
 *
 *  The %RF_ratCompare() API sets up one of the three available RAT channels in compare mode.
 *  When the compare event happens at the given compare time, the registered callback
 *  is invoked.
 *
 *  The RF driver handles power management. If the provided compare time is far into the future
 *  (and there is no other constraint set i.e. due to radio command execution), the RF core will be
 *  powered OFF and the device will enter the lowest possible power state. The RF core will be
 *  automatically powered ON just before the registered compare event. The callback function is
 *  served upon expiration of the allocated channel. The function is invoked with event type
 *  #RF_EventRatCh and runs in SWI context.
 *
 *  The API generates a "one-shot" compare event. Since the channel is automatically freed before
 *  the callback is served, the same channel can be reallocated from the callback itself through a
 *  new API call.
 *
 *  In case there were no available channels at the time of API call, the function returns with
 *  #RF_ALLOC_ERROR and no callback is invoked.
 *
 *  In case a runtime error occurs after the API successfully allocated a channel, the registered
 *  callback is invoked with event type #RF_EventError. A typical example is when the provided compare
 *  time is in the past and rejected by the RF core itself.
 *
 *  The events issued by the RAT timer can be output from the timer module through the RAT_GPO
 *  interface, and can be interconnected with other parts of the system through the RFC_GPO or
 *  the Event Fabric. The mapping between the allocated RAT channel and the selected RAT_GPO
 *  can be controlled through the optional ioConfig argument of %RF_ratCompare(). The possible
 *  RAT_GPO[x] are defined in #RF_RatOutputSelect.
 *
 *  @note Calling context : Task/SWI
 *
 *  @param rfHandle      Handle previously returned by RF_open().
 *  @param channelConfig Pointer to configuration structure needed to set up a channel in compare mode.
 *  @param ioConfig      Pointer to a configuration structure to set up the RAT_GPOs for the allocated
 *                       channel (optional).
 *  @return              Allocated RAT channel. If allocation fails, #RF_ALLOC_ERROR is returned.
 *
 *  \sa #RF_RatConfigCompare_init(), #RF_RatConfigOutput_init(), #RF_ratDisableChannel(), #RF_ratCapture()
 */
extern RF_RatHandle RF_ratCompare(RF_Handle rfHandle, RF_RatConfigCompare* channelConfig, RF_RatConfigOutput* ioConfig);

/**
 *  @brief  Setup a Radio Timer (RAT) channel in capture mode.
 *
 *  The %RF_ratCapture() API sets up one of the three available RAT channels in capture mode.
 *  The registered callback is invoked on the capture event.
 *
 *  The RF driver handles power management. If the RF core is OFF when the %RF_ratCapture() is called,
 *  it will be powered ON immediately and the RAT channel will be configured to capture mode. As long as
 *  at least one of the three RAT channels are in capture mode, the RF core will be kept ON. The callback
 *  function is served upon a capture event occurs. The function is invoked with event type RF_EventRatCh
 *  and runs in SWI context.
 *
 *  In case the channel is configured into single capture mode, the channel is automatically freed before
 *  the callback is called. In repeated capture mode, the channel remains allocated and automatically rearmed.
 *
 *  In case there were no available channels at the time of API call, the function returns with
 *  #RF_ALLOC_ERROR and no callback is invoked.
 *
 *  In case a runtime error occurs after the API successfully allocated a channel, the registered
 *  callback is invoked with event type #RF_EventError. A typical example is when the provided compare
 *  time is in the past and rejected by the RF core itself.
 *
 *  The events issued by the RAT timer can be output from the timer module through the RAT_GPO
 *  interface, and can be interconnected with other parts of the system through the RFC_GPO or
 *  the Event Fabric. The mapping between the allocated RAT channel and the selected RAT_GPO
 *  can be controlled through the optional ioConfig argument of %RF_ratCapture(). The possible
 *  RAT_GPO[x] are defined in #RF_RatOutputSelect. Note that this configuration is independent of
 *  the source signal of the capture event.
 *
 *  @note Calling context : Task/SWI
 *
 *  @param rfHandle      Handle previously returned by RF_open().
 *  @param channelConfig Pointer to configuration structure needed to set up a channel in compare mode.
 *  @param ioConfig      Pointer to a configuration structure to set up the RAT_GPO for the allocated
 *                       channel (optional).
 *  @return              Allocated RAT channel. If allocation fails, #RF_ALLOC_ERROR is returned.
 *
 *  \sa #RF_RatConfigCapture_init(), #RF_RatConfigOutput_init() , #RF_ratDisableChannel(), #RF_ratCompare()
 */
extern RF_RatHandle RF_ratCapture(RF_Handle rfHandle, RF_RatConfigCapture* channelConfig, RF_RatConfigOutput* ioConfig);

/**
 *  @brief  Disable a RAT channel.
 *
 *  The #RF_RatHandle returned by the #RF_ratCompare() or #RF_ratCapture() APIs can be used for further interaction with the
 *  Radio Timer. Passing the handle to %RF_ratDisableChannel() will abort a compare/capture event, and the provided channel
 *  is deallocated. No callback is invoked. This API can be called both if the RF core is ON or OFF. After the channel is
 *  freed, the next radio event will be rescheduled. A typical use case if a channel is configured in repeated capture mode,
 *  and the application decides to abort this operation.
 *
 *  @note Calling context : Task/SWI
 *
 *  @param rfHandle      Handle previously returned by RF_open().
 *  @param ratHandle     #RF_RatHandle returned by #RF_ratCompare() or #RF_ratCapture().
 *  @return              #RF_Stat indicates if command was successfully completed.
 *
 *  \sa #RF_ratCompare(), #RF_ratCapture()
 */
extern RF_Stat RF_ratDisableChannel(RF_Handle rfHandle, RF_RatHandle ratHandle);

/**
 *  @brief  Set RF control parameters.
 *
 *  @note Calling context : Task
 *
 *  @param h             Handle previously returned by RF_open()
 *  @param ctrl          Control codes
 *  @param args          Pointer to control arguments
 *  @return              RF_Stat indicates if API call was successfully completed.
 */
extern RF_Stat RF_control(RF_Handle h, int8_t ctrl, void *args);

/**
 *  @brief  Request radio access. <br>
 *
 *  Scope:
 *  1. Only supports request access which start immediately.<br>
 *  2. The #RF_AccessParams duration should be less than a pre-defined value
 *     RF_REQ_ACCESS_MAX_DUR_US in RFCC26X2_multiMode.c.<br>
 *  3. The #RF_AccessParams priority should be set RF_PriorityHighest.<br>
 *  4. Single request for a client at anytime.<br>
 *  5. Command from different client are blocked until the radio access
 *     period is completed.<br>
 *
 *  @note Calling context : Task
 *
 *  @param h             Handle previously returned by RF_open()
 *  @param pParams       Pointer to RF_AccessRequest parameters
 *  @return              RF_Stat indicates if API call was successfully completed.
 */
extern RF_Stat RF_requestAccess(RF_Handle h, RF_AccessParams *pParams);

/**
 * @brief Returns the currently configured transmit power configuration.
 *
 * This function returns the currently configured transmit power configuration under the assumption
 * that it has been previously set by #RF_setTxPower(). The value might be used for reverse
 * lookup in a TX power table. If no power has been programmed, it returns an invalid value.
 *
 * @code
 * RF_TxPowerTable_Value value = RF_getTxPower(handle);
 * if (value.rawValue == RF_TxPowerTable_INVALID_VALUE) {
 *     // error, value not valid
 * }
 * @endcode
 *
 * @param h         Handle previously returned by #RF_open()
 * @return          PA configuration struct
 *
 * @sa #RF_setTxPower(), #RF_TxPowerTable_findPowerLevel()
 */
extern RF_TxPowerTable_Value RF_getTxPower(RF_Handle h);

/**
 * @brief Updates the transmit power configuration of the RF core.
 *
 * This function programs a new TX power \a value and returns a status code. The API will return
 * with RF_StatBusyError if there are still pending commands in the internal queue. In case of
 * success, RF_StatSuccess is returned and the new configuration becomes effective from the next
 * radio operation.
 *
 * Some devices provide an integrated high-power PA in addition to the Default PA. On these devices
 * the API accepts configurations for both, and if \a value selects a different PA, the
 * \a globalCallback is invoked. The implementation of \a globalCallback is board specific and can
 * be used to reconfigure the external RF switches (if any).
 *
 * @param h         Handle previously returned by #RF_open()
 * @param value     TX power configuration value.
 * @return          #RF_StatSuccess on success, otherwise an error code.
 *
 * @sa #RF_getTxPower(), #RF_TxPowerTable_Value, #RF_TxPowerTable_findValue()
 */
extern RF_Stat RF_setTxPower(RF_Handle h, RF_TxPowerTable_Value value);

/**
 * @brief Retrieves a power level in dBm for a given power configuration value.
 *
 * \c %RF_TxPowerTable_findPowerLevel() searches in a lookup \a table for a given transmit power
 * configuration \a value and returns the power level in dBm if a matching configuration is found.
 * If \a value can not be found, #RF_TxPowerTable_INVALID_DBM is returned.
 *
 * This function does a reverse lookup compared to #RF_TxPowerTable_findValue() and has
 * O(n). It is assumed that \a table is terminated by a #RF_TxPowerTable_TERMINATION_ENTRY.
 *
 * @param table         List of #RF_TxPowerTable_Entry entries,
 *                      terminated by #RF_TxPowerTable_TERMINATION_ENTRY.
 *
 * @param value         Power configuration value.
 *
 * @return              Human readable power level in dBm on success,
 *                      otherwise #RF_TxPowerTable_INVALID_DBM.
 */
extern int8_t RF_TxPowerTable_findPowerLevel(RF_TxPowerTable_Entry table[], RF_TxPowerTable_Value value);

/**
 * @brief Retrieves a power configuration value for a given power level in dBm.
 *
 * \c %RF_TxPowerTable_findValue() searches in a lookup \a table for a given transmit power level
 * \a powerLevel in dBm and returns a matching power configuration. If \a powerLevel can not be
 * found, #RF_TxPowerTable_INVALID_VALUE is returned.
 *
 * This function performs a linear search in \a table and has O(n).
 * It is assumed that \a table is defined in ascending order and is terminated by a
 * #RF_TxPowerTable_TERMINATION_ENTRY.
 *
 * The following special values for \a powerLevel are also accepted:
 *
 * - #RF_TxPowerTable_MIN_DBM which returns always the lowest power value in the table
 * - #RF_TxPowerTable_MAX_DBM which returns always the highest power value in the table
 *
 * @param table         List of #RF_TxPowerTable_Entry entries,
 *                      terminated by #RF_TxPowerTable_TERMINATION_ENTRY.
 *
 * @param powerLevel    Human-readable power level in dBm.
 *
 * @return              PA configuration value on success.
 *                      otherwise #RF_TxPowerTable_INVALID_VALUE.
 */
extern RF_TxPowerTable_Value RF_TxPowerTable_findValue(RF_TxPowerTable_Entry table[], int8_t powerLevel);


/**
 * @brief Enables temperature monitoring and temperature based drift compensation
 *
 * @return #RF_StatSuccess if succesful or
 *         #RF_StatInvalidParamsError if temperature notification fails
 *          to register.
 *
 */
extern RF_Stat RF_enableHPOSCTemperatureCompensation(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_rfcc26x2__include */

//*****************************************************************************
//
//! Close the Doxygen group.
//! @}
//! @}
//
//*****************************************************************************
