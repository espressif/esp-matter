/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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

/*
 *  ========================== thread_pm.syscfg.js ==========================
 *  Submodule definition for SysConfig Power Management submodule
 */

"use strict";

const Common = system.getScript("/ti/thread/thread_common.js");

/* Description text for configurables */
const powerModeLongDescription = `Specifies whether the radio should always \
be on or be allowed to sleep. The period of polling in sleepy operation is \
determined by the *Poll Period* field, which appears when selecting \
the *Sleepy* option.

Note that for Full Thread Devices, the power mode of operation is fixed to \
be *Always On*. This is required for Full Thread Devices.

This configurable sets the radio power mode by toggling the *RX on while idle* \
bit of the Thread RSDN mode bits.

For more information, refer to the [Device Modes](/thread/html/thread/\
thread-protocol.html#device-modes) section of the Thread User's Guide.

**Default:** Always On`;

const pollPeriodDescription = `The period (in milliseconds) between poll \
messages`;

const pollPeriodLongDescription = `The period (in milliseconds) between poll \
messages. This period must be greater than or equal to \
OPENTHREAD_CONFIG_MINIMUM_POLL_PERIOD, specified in the \
libopenthread_<ftd/mtd> project, in /src/core/openthread-core-default-config.h.

**Default Period:** 2000

**Range:** 1 - ${Common.POLL_PERIOD_MAX} ms`;

/* Power management submodule for the Thread module */
const pmModule = {
    config: [
        {
            name: "deviceType",
            displayName: "Device Type",
            description: "Hidden configurable for passing in device type",
            default: "",
            hidden: true,
            onChange: onDeviceTypeChange
        },
        {
            name: "powerModeOperation",
            displayName: "Power Mode of Operation",
            description: "Specify whether the radio should always be on "
                         + "or be allowed to sleep",
            longDescription: powerModeLongDescription,
            default: "sleepy",
            options: [
                {
                    name: "alwaysOn",
                    displayName: "Always On"
                },
                {
                    name: "sleepy",
                    displayName: "Sleepy"
                }
            ],
            onChange: onPowerModeChange
        },
        {
            name: "pollPeriod",
            displayName: "Poll Period (ms)",
            description: pollPeriodDescription,
            longDescription: pollPeriodLongDescription,
            default: 2000,
            hidden: false
        }

    ],
    validate: validate
};

/* Function to handle changes in the deviceType configurable */
function onDeviceTypeChange(inst, ui)
{
    if(inst.deviceType === "ftd")
    {
        inst.powerModeOperation = "alwaysOn";
        ui.powerModeOperation.readOnly = true;
        ui.pollPeriod.hidden = true;
    }
    else /* mtd */
    {
        inst.powerModeOperation = "sleepy";
        ui.powerModeOperation.readOnly = false;
        ui.pollPeriod.hidden = false;
    }
}

/* Function to handle changes in powerModeOperation configurable */
function onPowerModeChange(inst, ui)
{
    if(inst.powerModeOperation === "alwaysOn")
    {
        ui.pollPeriod.hidden = true;
    }
    else /* sleepy */
    {
        ui.pollPeriod.hidden = false;
    }
}

/* Validation function for the PM submodule */
function validate(inst, validation)
{
    /* Warning for poll period above 0 but below default minimum specified
     * by OPENTHREAD_CONFIG_MINIMUM_POLL_PERIOD (10 ms) */
    if(inst.pollPeriod > 0 && inst.pollPeriod < Common.POLL_PERIOD_WARN_MIN)
    {
        validation.logWarning(
            "Poll period is less than the default minimum specified by "
            + "OpenThread (" + Common.POLL_PERIOD_WARN_MIN + " ms). Make sure "
            + "the poll period here is greater than or equal to "
            + "OPENTHREAD_CONFIG_MINIMUM_POLL_PERIOD, specified in the "
            + "libopenthread_<ftd/mtd> project, in /src/core/"
            + "openthread-core-default-config.h", inst, "pollPeriod"
        );
    }

    /* Verify poll period min bound */
    if(inst.pollPeriod <= 0)
    {
        validation.logError(
            "Poll period must be positive", inst, "pollPeriod"
        );
    }

    /* Verify poll period max bound */
    if(inst.pollPeriod > Common.POLL_PERIOD_MAX)
    {
        validation.logError(
            "Poll period must be less than or equal to "
            + Common.POLL_PERIOD_MAX + " milliseconds",
            inst, "pollPeriod"
        );
    }
}

exports = pmModule;
