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
 *  ======== ble_conn_update_params_docs.js ========
 */

"use strict";

// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");


// Long description for the paramUpdateDelay configuration parameter
const paramUpdateDelayLongDescription = `Delay after connection establishment,\
before sending a a GAP Update Link Parameter Request (in ms).\n

__Default__: 6000 ms\n
__Range__: 0 to sizeof(uint32)`

// Long description for the reqMinConnInt configuration parameter
const reqMinConnIntLongDescription = `In BLE connections, a frequency-hopping \
scheme is used. The two devices each send and receive data from one another \
only on a specific channel at a specific time. These devices meet a specific \
amount of time later at a new channel (the link layer of the BLE protocol \
stack handles the channel switching). This meeting is where the two devices \
send and receive data is known as a connection event. If there is no \
application data to be sent or received, the two devices exchange link layer \
data to maintain the connection. The connection interval is the amount of \
time between two connection events in units of 1.25 ms.\n

__Default__: 500 ms\n
__Range__: ${Common.connParamsRanges.connectionIntMinValue} ms to \
${Common.connParamsRanges.connectionIntMaxValue} ms\n
__Note__:\n
Min connection interval will be displayed in the range of 6 to 3200 (in units \
of 1.25 ms)\n
Min connection interval <= Max connection interval\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/gap.html#id4).\n`

// Long description for the reqMaxConnInt configuration parameter
const reqMaxConnIntLongDescription = `In BLE connections, a frequency-hopping \
scheme is used. The two devices each send and receive data from one another \
only on a specific channel at a specific time. These devices meet a specific \
amount of time later at a new channel (the link layer of the BLE protocol \
stack handles the channel switching). This meeting is where the two devices \
send and receive data is known as a connection event. If there is no \
application data to be sent or received, the two devices exchange link layer \
data to maintain the connection. The connection interval is the amount of \
time between two connection events.\n

__Default__: 1000 ms\n
__Range__: ${Common.connParamsRanges.connectionIntMinValue} ms to \
${Common.connParamsRanges.connectionIntMaxValue} ms\n
__Note__:\n
Max connection interval will be displayed in the range of 6 to 3200 (in units \
of 1.25 ms)\n
Max connection interval >= Min connection interval\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/gap.html#id4).\n`

// Long description for the reqSlaveLat configuration parameter
const reqSlaveLatLongDescription =`This parameter gives the slave (peripheral) \
device the option of skipping a number of connection events. This ability \
gives the peripheral device some flexibility. If the peripheral does not have \
any data to send, it can skip connection events, stay asleep, and save power. \
The peripheral device selects whether to wake or not on a per connection \
event basis. The peripheral can skip connection events but must not skip more \
than allowed by the slave latency parameter or the connection fails.\n

For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/gap.html#id4).\n

__Default__: 0 \n
__Range__: ${Common.connParamsRanges.connLatencyMinValue} to \
${Common.connParamsRanges.connLatencyMaxValue}`

// Long description for the reqConnTo configuration parameter
const reqConnToLongDescription =`This time-out is the maximum amount of time \
between two successful connection events. If this time passes without a \
successful connection event, the device terminates the connection and returns \
to an unconnected state.\n

__Default__: 6000 ms\n
__Range__: ${Common.connParamsRanges.connTimeoutMinValue} ms to \
${Common.connParamsRanges.connTimeoutMaxValue} ms\n

__Note__:\n
Connection time-out will be displayed in 10 to 3200 (in units of 10 ms)\n
LSTO (time-out) > (1 + Slave Latency) * (Connection Interval * 2) \n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/gap.html#id4).\n`

 // Exports the long descriptions for each configurable
 exports = {
    paramUpdateDelayLongDescription: paramUpdateDelayLongDescription,
    reqMinConnIntLongDescription: reqMinConnIntLongDescription,
    reqMaxConnIntLongDescription: reqMaxConnIntLongDescription,
    reqSlaveLatLongDescription: reqSlaveLatLongDescription,
    reqConnToLongDescription: reqConnToLongDescription
};