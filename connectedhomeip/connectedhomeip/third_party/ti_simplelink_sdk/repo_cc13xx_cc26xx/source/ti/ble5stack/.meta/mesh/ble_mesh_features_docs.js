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
 *  ======== ble_mesh_features_docs.js ========
 */

"use strict";

// Long description for the relay configuration parameter
const relayLongDescription = `A Node that has the Relay feature enabled is \
known as a Relay Node.\n
A Relay Node has the ability to receive and retransmit mesh messages over the \
advertising bearer to enable larger networks.\n
__Note__: If the Relay feature is enabled, the Relay mode state in the \
configuration server will be enabled as well.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_\
guide/html/ble-mesh/overview.html).\n`

// Long description for the relay configuration parameter
const proxyLongDescription = `A Node that has the Proxy feature enabled is \
known as a Proxy Node.\n
A Proxy Node has the ability to receive and retransmit mesh messages between \
GATT and advertising bearers.\n
__Note__: If the Proxy feature is enabled, the GATT Proxy state in the \
configuration server will be enabled as well.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_\
guide/html/ble-mesh/overview.html).\n`

// Long description for the friend configuration parameter
const friendLongDescription = `A Node that has the Friend feature enabled is \
known as a Friend Node.\n
A Friend Node has the ability to help a Node supporting the Low Power feature \
to operate efficiently, by storing messages destined for that Node and only \
delivering them when the Low Power Node polls the Friend Node.\n
The relationship between the Friend and the Low Power Node is known as \
Friendship.\n
__Note__: If the Friend feature is enabled, the Friend state in the \
configuration server will be enabled as well.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_\
guide/html/ble-mesh/overview.html).\n`

// Long description for the recieveWindow configuration parameter
const recieveWindowLongDescription = `Receive Window in milliseconds \
supported by the Friend node.`

// Long description for the queueSize configuration parameter
const queueSizeLongDescription = `Minimum number of buffers available to be \
stored for each local Friend Queue.`

// Long description for the subscriptionListSize configuration parameter
const subscriptionListSizeLongDescription = `Size of the Subscription List \
that can be supported by a Friend node for a Low Power node.`

// Long description for the lpnCount configuration parameter
const lpnCountLongDescription = `Number of Low Power Nodes the Friend can \
have a Friendship with simultaneously.`

// Long description for the segmentLists configuration parameter
const segmentListsLongDescription = `Number of incomplete segment lists that \
we track for each LPN that we are friends with. In other words, this \
determines how many elements we can simultaneously be receiving segmented \
messages from when the messages are going into the Friend queue.`

// Long description for the lowPower configuration parameter
const lowPowerLongDescription = `A Node that has the Low Power feature \
enabled is known as a Low Power Node (LPN).\n
LPN has the ability to operate within a mesh network at significantly reduced \
receiver duty cycles when working in conjunction with a Node supporting the \
Friend feature.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_\
guide/html/ble-mesh/overview.html).\n`

// Long description for the friendshipEstablishment configuration parameter
const friendshipEstablishmentLongDescription = `Perform the Friendship \
establishment using low power, with the help of a reduced scan duty cycle.\n
The downside of this is that the node may miss out on messages intended for \
it until it has successfully set up Friendship with a Friend node.`

// Long description for the autoEnableLpn configuration parameter
const autoEnableLpnLongDescription = `Automatically enable LPN functionality \
once provisioned and start looking for Friend nodes. If this option is \
disabled, LPN mode needs to be manually enabled by calling \
bt_mesh_lpn_set(true).`

// Long description for the autoEnableLpnTimeout configuration parameter
const autoEnableLpnTimeoutLongDescription = `Time in seconds from the last received \
message, that the node will wait before starting to look for Friend nodes.`

// Long description for the retryTimeout configuration parameter
const retryTimeoutLongDescription = `Time in seconds between Friend Requests, \
if a previous Friend Request did not receive any acceptable Friend Offers.`

// Long description for the rssiFactor configuration parameter
const rssiFactorLongDescription = `The contribution of the RSSI measured by \
the Friend node used in Friend Offer Delay calculations. 0 = 1, 1 = 1.5, \
2 = 2, 3 = 2.5.`

// Long description for the receiveWindowFactor configuration parameter
const receiveWindowFactorLongDescription = `The contribution of the supported \
Receive Window used in Friend Offer Delay calculations. 0 = 1, 1 = 1.5, \
2 = 2, 3 = 2.5.`

// Long description for the minQueueSize configuration parameter
const minQueueSizeLongDescription = `The MinQueueSizeLog field is defined as \
log_2(N), where N is the minimum number of maximum size Lower Transport PDUs \
that the Friend node can store in its Friend Queue. \n
As an example, MinQueueSizeLog value 1 gives N = 2, and value 7 gives N = 128.`

// Long description for the receiveDelay configuration parameter
const receiveDelayLongDescription = `The ReceiveDelay is the time between the \
Low Power node sending a request and listening for a response. This delay \
allows the Friend node time to prepare the response.\n
The value is in units of milliseconds.`

// Long description for the pollTimeout configuration parameter
const pollTimeoutLongDescription = `PollTimeout timer is used to measure time \
between two consecutive requests sent by the Low Power node. If no requests \
are received by the Friend node before the PollTimeout timer expires, then \
the friendship is considered terminated.\n
The value is in units of 100 milliseconds, so e.g. a value of 300 means 30 \
seconds.`

// Long description for the initialValuePollTimeout configuration parameter
const initialValuePollTimeoutLongDescription = `The initial value of the \
PollTimeout timer when Friendship gets established for the first time. \
After this the timeout will gradually grow toward the actual PollTimeout, \
doubling in value for each iteration.\n
The value is in units of 100 milliseconds, so e.g. a value of 300 means 30 \
seconds.`

// Long description for the scanLatency configuration parameter
const scanLatencyLongDescription = `Latency in milliseconds that it takes to \
enable scanning. This is in practice how much time in advance before the \
Receive Window that scanning is requested to be enabled.`

// Long description for the groups configuration parameter
const groupsLongDescription = `Maximum number of groups that the LPN can \
subscribe to.`

// Long description for the beaconState configuration parameter
const beaconStateLongDescription = `The Secure Network Beacon state \
determines if a node is periodically broadcasting Secure Network beacon \
messages.\n
__Note__: Relay and Friend nodes should send beacons and other nodes may send \
beacons.\n
If enabled, the Secure Network Beacon state in the configuration server will be enabled.`

// Exports the long descriptions for each configurable
exports = {
    relayLongDescription: relayLongDescription,
    proxyLongDescription: proxyLongDescription,
    friendLongDescription: friendLongDescription,
    recieveWindowLongDescription: recieveWindowLongDescription,
    queueSizeLongDescription: queueSizeLongDescription,
    subscriptionListSizeLongDescription: subscriptionListSizeLongDescription,
    lpnCountLongDescription: lpnCountLongDescription,
    segmentListsLongDescription:segmentListsLongDescription,
    lowPowerLongDescription: lowPowerLongDescription,
    friendshipEstablishmentLongDescription: friendshipEstablishmentLongDescription,
    autoEnableLpnLongDescription: autoEnableLpnLongDescription,
    autoEnableLpnTimeoutLongDescription: autoEnableLpnTimeoutLongDescription,
    retryTimeoutLongDescription: retryTimeoutLongDescription,
    rssiFactorLongDescription: rssiFactorLongDescription,
    receiveWindowFactorLongDescription: receiveWindowFactorLongDescription,
    minQueueSizeLongDescription: minQueueSizeLongDescription,
    receiveDelayLongDescription: receiveDelayLongDescription,
    pollTimeoutLongDescription: pollTimeoutLongDescription,
    initialValuePollTimeoutLongDescription: initialValuePollTimeoutLongDescription,
    scanLatencyLongDescription: scanLatencyLongDescription,
    groupsLongDescription: groupsLongDescription,
    beaconStateLongDescription:beaconStateLongDescription
};
