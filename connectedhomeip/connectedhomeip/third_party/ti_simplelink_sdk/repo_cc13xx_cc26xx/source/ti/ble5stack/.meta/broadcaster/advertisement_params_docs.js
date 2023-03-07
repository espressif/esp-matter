/*
 * Copyright (c) 2018 Texas Instruments Incorporated - http://www.ti.com
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
 *
 */

/*
 *  ======== advertisement_params_docs.js ========
 */

"use strict";

// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

// Long description for the Advertisement Type configuration parameter
const advTypeLongDescription = `Bluetooth 5 and above includes Extended Advertising \
feature. The BLE Stack supports legacy advertising maintaining backwards \
compatibility with previous versions.\n\n

Options |
--- |
Legacy |
Extended |\n\n
__Default__: Legacy \n`


// Long description for the legacyEvnPropOptions configuration parameter
const legacyEvnPropOptionsLongDescription = `If legacy advertising type is \
being used, the following advertising Event Parameter properties combinations \
are allowed:\n
Options |
--- |
Connectable and scannable undirected |
Connectable directed |
High Duty Cycle Connectable directed |
Scannable undirected |
Non-connectable and nonscannable undirected |`

// Long description for the Properties configuration parameter
const eventPropsLongDescription = `Choose advertising Event Parameter properties \n\n
Options |
--- |
Connectable advertising |
Scannable advertising |
Directed advertising |
High Duty Cycle Directed Advertising |
Legacy advertising PDU's |
Omit advertiser's address from PDU's |
Include TxPower in the extended header of the advertising PDU |\n\n
__Default__: When using Legacy advertising: Connectable advertising & Scannable \
advertising & Legacy advertising PDU's\n
When using Extended: Connectable advertising\n`

// Long description for the Primary PHY Interval Minimum configuration parameter
const primIntMinLongDescription = `Minimum advertising interval of the primary PHY.\n

__Default__: 100 ms\n
__Range__: ${Common.advParamsRanges.priAdvIntMinValue} ms to ${Common.advParamsRanges.priAdvIntMaxValue} ms`

// Long description for the Primary PHY Interval Maximum configuration parameter
const primIntMaxLongDescription = `Maximum advertising interval of the primary PHY.\n

__Default__: 100 ms\n
__Range__: ${Common.advParamsRanges.priAdvIntMinValue} ms to ${Common.advParamsRanges.priAdvIntMaxValue} ms`

// Long description for the Primary Channel Map configuration parameter
const primChanMapLongDescription = `Choose advertising channel map for the primary PHY.\n\n

Options |
--- |
Channel 37 |
Channel 38 |
Channel 39 |\n\n
__Default__: Channel 37 & Channel 38 & Channel 39\n`

// Long description for the Peer Address Type configuration parameter
const peerAddrTypeLongDescription = `\n
Options |
--- |
Public Or Public ID |
Random or Random ID |\n\n
__Default__: Public Or Public ID\n`

// Long description for the Peer Address configuration parameter
const peerAddrLongDescription = `Public Device Address, Random Device Address, \
Public Identity Address, or Random (static) Identity Address of the device to be \
connected.\n

__Default__: 0xaaaaaaaaaaaa\n`

// Long description for the Peer Address configuration parameter
const filterPolicyLongDescription = `The GAP Scanner module provides 5 \
different types of filter to reduce the amount of advertising report \
notifications to the application and eventually save memory and power \
consumption. The result of the filtering affects both advertising reports \
and advertising report recording. The packets filtered out by the filter \
configurations are discarded and will neither be reported nor recorded. \n\n

Options | Description
--- | ---
Process requests from all devices | Process scan and connection requests \
from all devices (i.e., the WhiteList is not in use)
Process conn req from all devices and only scan req from devices in WL | \
Process connection requests from all devices and only scan requests \
from devices that are in the White List
Process scan req from all devices and only conn req from devices in WL | \
Process scan requests from all devices and only connection requests from \
devices that are in the White List
Process requests only from devices in WL | Process scan and connection \
requests only from devices in the White List
\n\

__Default__: Process requests from all devices\n`

// Long description for the TX Power configuration parameter
const txPowerLongDescription = `Configure if the TX power level will be \
set by the controller or set to a value by the user. \n
Options |
--- |
The Controller will choose the Tx power |
TX Power Value |\n\n
__Default__: The Controller will choose the Tx power\n`

// Long description for the TX Power configuration parameter
const txPowerValueLongDescription = `The TX Power in DBm \n
The TX Power indicates the maximum level at which the advertising \
packets are to be transmitted on the advertising channels. \n
__Default__: 0 \n
__Range__: To see the valid values refer to the "Default Tx Power" parameter \
in the Radio section \n`

// Long description for the Primary PHY configuration parameter
const primPhyLongDescription = `Configures the physical parameters \
of the radio transmission and reception. It determines how a bit \
(and its value) are represented over the air. By switching the PHY, \
the physical properties of the RF signal is changed.\n
Options |
--- |
1M |
Coded S8 |
Coded S2 |\n\n
__Default__: 1M \n`

// Long description for the Secondery PHY configuration parameter
const secPhyLongDescription = `The secondary PHY parameter will \
decide the PHY of any auxiliary advertisement packets.\n
__Note__: If advertising non-connectable and non-scannable, \
an \`ADV_EXT_IND\` PDU with no Adv Data can be sent without an \
auxiliary packet. In all other cases, the \`ADV_EXT_IND\` PDU must \
contain a pointer to an auxiliary advertisement packet, \`AUX_ADV_IND\`. \
The \`AUX_ADV_IND PDU\` is sent on the PHY given in secPhy, and on one of \
the secondary channels (also known as data channels).\n
Options |
--- |
1M |
Coded S8 |
Coded S2 |\n\n
__Default__: 1M \n`

// Long description for the Set ID configuration parameter
const sidLongDescription = `Value of the Advertising SID subfield in the \
ADI field of the PDU.\n
__Range__: 0 - 15 \n
__Default__: 0 \n`

 // Exports the long descriptions for each configurable
 exports = {
    advTypeLongDescription: advTypeLongDescription,
    legacyEvnPropOptionsLongDescription: legacyEvnPropOptionsLongDescription,
    eventPropsLongDescription: eventPropsLongDescription,
    primIntMinLongDescription: primIntMinLongDescription,
    primIntMaxLongDescription: primIntMaxLongDescription,
    primChanMapLongDescription: primChanMapLongDescription,
    peerAddrTypeLongDescription: peerAddrTypeLongDescription,
    peerAddrLongDescription: peerAddrLongDescription,
    filterPolicyLongDescription: filterPolicyLongDescription,
    txPowerLongDescription: txPowerLongDescription,
    txPowerValueLongDescription: txPowerValueLongDescription,
    primPhyLongDescription: primPhyLongDescription,
    secPhyLongDescription: secPhyLongDescription,
    sidLongDescription: sidLongDescription
};