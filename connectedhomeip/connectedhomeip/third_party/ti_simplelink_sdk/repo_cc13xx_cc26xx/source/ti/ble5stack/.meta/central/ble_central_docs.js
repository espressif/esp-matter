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
 *  ======== ble_central_docs.js ========
 */

"use strict";

// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

// Long description for the initiatingPHY configuration parameter
const initiatingPHYLongDescription = `PHY used to try initiating a connection on\n
__Default__: 1 M PHY\n
Options | Description
--- | ---
1M | 1 M PHY
2M | 2 M PHY
Coded | Coded PHY
`

// Long description for the rssiPollingPeriod configuration parameter
const rssiPollingPeriodLongDescription = `How often to read the RSSI value\n
__Default__: 3000 ms\n
__Range__: 0 to sizeof(uint32)`

// Long description for the connIntMin configuration parameter
const connIntMinLongDescription = `In BLE connections, a frequency-hopping \
scheme is used. The two devices each send and receive data from one another \
only on a specific channel at a specific time. These devices meet a specific \
amount of time later at a new channel (the link layer of the BLE protocol \
stack handles the channel switching). This meeting is where the two devices \
send and receive data is known as a connection event. If there is no \
application data to be sent or received, the two devices exchange link layer \
data to maintain the connection. The connection interval is the amount of \
time between two connection events in units of 1.25 ms.\n
For more information, refer to the [BLE Stack User's Guide]\
(ble5stack/ble_user_guide/html/ble-stack-5.x/gap.html#id4).\n\

__Range__: ${Common.connParamsRanges.connectionIntMinValue} ms to \
${Common.connParamsRanges.connectionIntMaxValue} ms\n
__Note__:\n 
Min connection interval will be displayed in the range of 6 to 3200 (in units \
of 1.25 ms)\n
Min connection interval <= Max connection interval`

// Long description for the connIntMax configuration parameter
const connIntMaxLongDescription = `In BLE connections, a frequency-hopping \
scheme is used. The two devices each send and receive data from one another \
only on a specific channel at a specific time. These devices meet a specific \
amount of time later at a new channel (the link layer of the BLE protocol \
stack handles the channel switching). This meeting is where the two devices \
send and receive data is known as a connection event. If there is no \
application data to be sent or received, the two devices exchange link layer \
data to maintain the connection. The connection interval is the amount of \
time between two connection events.\n
For more information, refer to the [BLE Stack User's Guide]\
(ble5stack/ble_user_guide/html/ble-stack-5.x/gap.html#id4).\n\

__Range__: ${Common.connParamsRanges.connectionIntMinValue} ms to \
${Common.connParamsRanges.connectionIntMaxValue} ms\n
__Note__:\n
Max connection interval will be displayed in the range of 6 to 3200 (in units \
of 1.25 ms)\n
Max connection interval >= Min connection interval`

 // Exports the long descriptions for each configurable
 exports = {
    initiatingPHYLongDescription: initiatingPHYLongDescription,
    rssiPollingPeriodLongDescription: rssiPollingPeriodLongDescription,
    connIntMinLongDescription: connIntMinLongDescription,
    connIntMaxLongDescription: connIntMaxLongDescription
};