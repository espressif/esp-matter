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
 *  ======================== thread_network.syscfg.js ========================
 *  Submodule definition for the SysConfig Thread network submodule
 */

"use strict";

const Common = system.getScript("/ti/thread/thread_common.js");

/* Description text for configurables */
const setNetworkLongDescription = `Specify whether to set the network \
parameters directly via SysConfig. This includes the PAN ID, Extended PAN ID, \
Master Key, and Network Name.

SysConfig overrides the default network values from OpenThread. If this is \
not desired, leave this unchecked.

If this device is to be commissioned into an exisiting network, you may \
leave this unchecked. The network configurations will be overridden by \
the new network credentials when joining.

If you wish to set custom network parameters without commissioning or \
using device defaults, check this and set the network paramters that \
appear in this subcategory.

**Default:** Setup Network Enabled`;

const panIDLongDescription = `The ${Common.PAN_ID_LEN * 8} bit PAN ID.

0xFFFF (the broadcast PAN ID) is a special PAN ID in Thread. This PAN ID is \
invalid for network formation. However, it is used by joiner devices during \
the commissioning process. If the broadcast PAN ID is set, the joiner device \
will go through the MLE discovery process during commissioning, attempting to \
populate its Thread dataset (including a new PAN ID) via a valid commissioner.

For more information on the commissioning process, refer to the \
[Commissioning](/thread/html/thread/thread-protocol.html#commissioning) \
section of the Thread User's Guide.

**Default PAN ID:** 0xFFFF`;

const extPanIDDescription = `The ${Common.EXT_PAN_ID_LEN * 8} bit Extended \
PAN ID.`;

const extPanIDLongDescription = `The ${Common.EXT_PAN_ID_LEN * 8} bit \
Extended PAN ID.

**Default Ext PAN ID:** 0xdead00beef00cafe`;

const masterKeyDescription = `The ${Common.MASTER_KEY_LEN * 8} bit Thread \
Master Key.`;

const masterKeyLongDescription = `The ${Common.MASTER_KEY_LEN * 8} bit \
Thread Master Key.

**Default MasterKey:** 0x00112233445566778899AABBCCDDEEFF`;

const networkNameDescription = `The network name for the Thread network \
(max length of ${Common.NETWORK_NAME_MAX_LEN - 1} bytes).`;

const networkNameLongDescription = `The network name for the Thread network \
(max length of ${Common.NETWORK_NAME_MAX_LEN - 1} bytes).

**Default Name:** OpenThread`;

/* Network submodule for the Thread module */
const networkModule = {
    config: [
        {
            name: "setNetwork",
            displayName: "Setup Network",
            description: "Specify whether to set the network parameters "
                         + "directly. Network parameters should not be set "
                         + "as a joiner, as they will be overwritten during "
                         + "commissioning.",
            longDescription: setNetworkLongDescription,
            default: true,
            onChange: onSetNetworkChange
        },
        {
            name: "panID",
            displayName: "PAN ID",
            description: "The " + Common.PAN_ID_LEN * 8 + " bit PAN ID. Set to "
                         + "0xFFFF for the broadcast address, used for joiner "
                         + "devices to discover network parameters via "
                         + "commissioning.",
            longDescription: panIDLongDescription,
            default: "0xFFFF"
        },
        {
            name: "extPanID",
            displayName: "Extended PAN ID",
            description: extPanIDDescription,
            longDescription: extPanIDLongDescription,
            default: "0xdead00beef00cafe"
        },
        {
            name: "masterKey",
            displayName: "Thread Master Key",
            description: masterKeyDescription,
            longDescription: masterKeyLongDescription,
            default: "0x00112233445566778899AABBCCDDEEFF"
        },
        {
            name: "networkName",
            displayName: "Network Name",
            description: networkNameDescription,
            longDescription: networkNameLongDescription,
            default: "OpenThread"
        }
    ],
    validate: validate
};

/* Function to handle changes in the setNetwork configurable */
function onSetNetworkChange(inst, ui)
{
    ui.panID.hidden = !inst.setNetwork;
    ui.extPanID.hidden = !inst.setNetwork;
    ui.masterKey.hidden = !inst.setNetwork;
    ui.networkName.hidden = !inst.setNetwork;
}

/* Validation function for the RF submodule */
function validate(inst, validation)
{
    /* Validate PAN ID */
    const panIDReg = new RegExp(
        "^0x[0-9A-Fa-f]{" + Common.PAN_ID_LEN * 2 + "}$", "g"
    );
    if(panIDReg.test(inst.panID) === false)
    {
        validation.logError(
            "PAN ID must be a valid hexidecimal number (0x...) of length "
            + Common.PAN_ID_LEN * 8 + " bits", inst, "panID"
        );
    }

    /* Info for broadcast PAN ID (0xFFFF) */
    const panIDReservedReg = new RegExp(
        "^0x[Ff]{" + Common.PAN_ID_LEN * 2 + "}$", "g"
    );
    if(panIDReservedReg.test(inst.panID) === true)
    {
        validation.logInfo(
            "PAN ID 0xFFFF is reserved as the broadcast PAN ID. This is "
            + "invalid for network formation, but is used for joiner devices "
            + "when discovering network parameters via commissioning.",
            inst, "panID"
        );
    }

    /* Validate Extended PAN ID */
    const extPanIDReg = new RegExp(
        "^0x[0-9A-Fa-f]{" + Common.EXT_PAN_ID_LEN * 2 + "}$", "g"
    );
    if(extPanIDReg.test(inst.extPanID) === false)
    {
        validation.logError(
            "Extended PAN ID must be a valid hexidecimal number (0x...) "
            + "of length " + Common.EXT_PAN_ID_LEN * 8 + " bits",
            inst, "extPanID"
        );
    }

    /* Validate Master Key */
    const masterKeyReg = new RegExp(
        "^0x[0-9A-Fa-f]{" + Common.MASTER_KEY_LEN * 2 + "}$", "g"
    );
    if(masterKeyReg.test(inst.masterKey) === false)
    {
        validation.logError(
            "Master Key must be a valid hexidecimal number (0x...) of length "
            + Common.MASTER_KEY_LEN * 8 + " bits", inst, "masterKey"
        );
    }

    /* Validate Network Name max length */
    const networkNameUTFLen = encodeURI(inst.networkName)
        .split(/%..|./).length; // Includes null term
    if(networkNameUTFLen > Common.NETWORK_NAME_MAX_LEN)
    {
        validation.logError(
            "Network Name must have a UTF-8 encoded length of at most "
            + (Common.NETWORK_NAME_MAX_LEN - 1) + " bytes", inst, "networkName"
        );
    }
}

exports = networkModule;
