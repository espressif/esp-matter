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
 *  ======== ble_mesh_docs.js ========
 */

"use strict";

// Long description for the useNV configuration parameter
const useNVLongDescription = `Enables the option to store (and restore) the \
mesh node state and configuration persistently in flash.\n
When this option has been enabled, it’s important that the application makes \
a call to \`settings_load()\` after having called \`bt_mesh_init()\`.\n`

// Long description for the staticProv configuration parameter
const staticProvLongDescription = `When using static provisioning the node \
provision itself by calling \`bt_mesh_provision()\`.\n
The provisioning parameters needed for static provisioning can be defined in \
static_prov.c or static_prov.h files.\n
__Note:__ When adding or removing models from the Device Composition Data \
section, update the \`MeshApp_provConfigure()\` function. This function is \
responsiable for models binding, models subscription, etc.`

// Long description for the filterSize configuration parameter
const filterSizeLongDescription = `This option specifies how many Proxy \
Filter entries the local node supports.`

// Long description for the nodeIdTimeout configuration parameter
const nodeIdTimeoutLongDescription = `This option determines for how long the \
local node advertises using Node Identity.\n
The specification limits this value to up to 60 seconds, and implies that 60 \
seconds is the appropriate value.\n
__Note__: The given value is in seconds.`

// Long description for the advbearer configuration parameter
const advbearerLongDescription = `Enable this option to allow the device to \
be provisioned over the advertising bearer.`

// Long description for the gattBearer configuration parameter
const gattBearerLongDescription = `Enable this option to allow the device to \
be provisioned over GATT bearer.`

// Long description for the appKeyCount configuration parameter
const appKeyCountLongDescription = `This option specifies how many \
application keys the device can store per network.`

// Long description for the subnetCount configuration parameter
const subnetCountLongDescription = `This option specifies how many subnets a \
Mesh network can participate in at the same time.`

// Long description for the msgCacheSize configuration parameter
const msgCacheSizeLongDescription = `Number of messages that are cached for \
the network. This helps prevent unnecessary decryption operations and \
unnecessary relays.`

// Long description for the numAdvBuf configuration parameter
const numAdvBufLongDescription = `Number of advertising buffers available.\n
This should be chosen based on what kind of features the local node should \
have. E.g. a relay will perform better the more buffers it has.\n
Another thing to consider is outgoing segmented messages. There must be at \
least three more advertising buffers than the maximum supported outgoing \
segment count (BT_MESH_TX_SEG_MAX).`

// Long description for the maxNumOfSegMsgTX configuration parameter
const maxNumOfSegMsgTXLongDescription = `Maximum number of simultaneous \
outgoing multi-segment and/or reliable messages.`

// Long description for the maxNumSegInMsgTX configuration parameter
const maxNumSegInMsgTXLongDescription = `Maximum number of segments supported \
for outgoing messages. This value should typically be fine-tuned based on \
what models the local node supports, i.e. what’s the largest message payload \
that the node needs to be able to send.\n

__Note__: This value affects memory and call stack consumption, which is why \
the default is lower than the maximum that the specification would allow \
(32 segments).\n

The maximum outgoing SDU size is 12 times this number (out of which 4 or 8 \
bytes is used for the Transport Layer MIC). \
For example, 5 segments means the maximum SDU size is 60 bytes, which leaves \
56 bytes for application layer data using a 4-byte MIC and 52 bytes using an \
8-byte MIC.

Be sure to specify a sufficient number of advertising buffers when setting \
this option to a higher value. There must be at least three more advertising \
buffers (BT_MESH_ADV_BUF_COUNT) as there are outgoing segments.`

// Long description for the maxNumSegInMsgRX configuration parameter
const maxNumSegInMsgRXLongDescription = `Maximum number of segments supported \
for incoming messages. This value should typically be fine-tuned based on \
what models the local node supports, i.e. what’s the largest message payload \
that the node needs to be able to receive. This value affects memory and call \
stack consumption, which is why the default is lower than the maximum that \
the specification would allow (32 segments).\n

__Note__: The maximum incoming SDU size is 12 times this number \
(out of which 4 or 8 bytes is used for the Transport Layer MIC). \
For example, 5 segments means the maximum SDU size is 60 bytes, which leaves \
56 bytes for application layer data using a 4-byte MIC and 52 bytes using an \
8-byte MIC.`

// Long description for the maxNumSegMsgRX configuration parameter
const maxNumSegMsgRXLongDescription = `Maximum number of simultaneous \
incoming multi-segment and/or reliable messages.`

// Long description for the maxSizeRXSdu configuration parameter
const maxSizeRXSduLongDescription = `Maximum incoming Upper Transport Access \
PDU length.\n
This determines also how many segments incoming segmented messages can have. \
Each segment can contain 12 bytes, so this value should be set to a multiple \
of 12 to avoid wasted memory. The minimum requirement is 2 segments \
(24 bytes) whereas the maximum supported by the Mesh specification is 32 \
segments (384 bytes).`

// Long description for the modelGroupAddrCount configuration parameter
const modelGroupAddrCountLongDescription = `This option specifies how many \
group addresses each model can at most be subscribed to.`

// Long description for the modelAppKeysCount configuration parameter
const modelAppKeysCountLongDescription = `This option specifies how many \
application keys each model can at most be bound to.`

// Long description for the modelExtensions configuration parameter
const modelExtensionsLongDescription = `Enable support for the model \
extension concept, allowing the Access layer to know about Mesh model \
relationships.`

// Long description for the provTimeout configuration parameter
const provTimeoutLongDescription = `Timeout value in ms, of retransmit provisioning PDUs. \
Valid value ranges from 100 to 800 ms.`

// Exports the long descriptions for each configurable
exports = {
    useNVLongDescription: useNVLongDescription,
    staticProvLongDescription: staticProvLongDescription,
    filterSizeLongDescription: filterSizeLongDescription,
    nodeIdTimeoutLongDescription: nodeIdTimeoutLongDescription,
    advbearerLongDescription: advbearerLongDescription,
    gattBearerLongDescription: gattBearerLongDescription,
    appKeyCountLongDescription: appKeyCountLongDescription,
    subnetCountLongDescription: subnetCountLongDescription,
    msgCacheSizeLongDescription: msgCacheSizeLongDescription,
    numAdvBufLongDescription: numAdvBufLongDescription,
    maxNumOfSegMsgTXLongDescription: maxNumOfSegMsgTXLongDescription,
    maxNumSegInMsgTXLongDescription: maxNumSegInMsgTXLongDescription,
    maxNumSegInMsgRXLongDescription: maxNumSegInMsgRXLongDescription,
    maxNumSegMsgRXLongDescription: maxNumSegMsgRXLongDescription,
    maxSizeRXSduLongDescription: maxSizeRXSduLongDescription,
    modelGroupAddrCountLongDescription: modelGroupAddrCountLongDescription,
    modelAppKeysCountLongDescription: modelAppKeysCountLongDescription,
    modelExtensionsLongDescription: modelExtensionsLongDescription,
    provTimeoutLongDescription: provTimeoutLongDescription
};
