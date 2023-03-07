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
 *  ======================== thread_security.syscfg.js ========================
 *  Submodule definition for the SysConfig Thread security submodule
 */

"use strict";

const Common = system.getScript("/ti/thread/thread_common.js");

/* Description text for configurables */
const pskdLongDescription = `The Pre-Shared Key (PSKd) for joiner devices to \
use for commissioning.

The purpose of this configurable is to set the Pre-Shared Key for joiner \
devices. In order to use this functionality, your application must use the \
function TIOP_configuredJoinerStart() in tiop_config.h to start itself as a \
joiner. This function will use the configured PSKD to start the joiner.

Note that TI-OpenThread joiner examples already use this function, allowing \
the PSKd to be configured through this field.

For more information on the commissioning process, refer to the \
[Commissioning](/thread/html/thread/thread-protocol.html#commissioning) \
section of the Thread User's Guide.

**Default Key:** PSK001`;

/* Security submodule for the Thread module */
const securityModule = {
    config: [
        {
            name: "pskd",
            displayName: "Pre-Shared Key (Joiner Device)",
            description: "The Pre-Shared Key (PSKd) for joiner devices "
                         + "to use for commissioning",
            longDescription: pskdLongDescription,
            default: "PSK001"
        }
    ],
    validate: validate
};

/* Validation function for the RF submodule */
function validate(inst, validation)
{
    /* Validate pskd length */
    if(inst.pskd.length < Common.PSKD_MIN_LEN)
    {
        validation.logError(
            "Pre-Shared Key must be at least " + Common.PSKD_MIN_LEN
            + " characters", inst, "pskd"
        );
    }
    else if(inst.pskd.length > Common.PSKD_MAX_LEN)
    {
        validation.logError(
            "Pre-Shared Key must be at most " + Common.PSKD_MAX_LEN
            + " characters", inst, "pskd"
        );
    }

    /* Validate pskd characters */
    const pskdReg = /^[0-9A-HJ-NPR-Y]*$/g;
    if(pskdReg.test(inst.pskd) === false)
    {
        validation.logError(
            "Pre-Shared Key must only contain uppercase alphanumeric "
            + "characters excluding I, O, Q, and Z", inst, "pskd"
        );
    }
}

exports = securityModule;
