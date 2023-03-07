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
 *  ======== ble_observer_docs.js ========
 */

"use strict";

// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

// Long description for the scanPHY configuration parameter
const scanPHYLongDescription = `The primary PHY that is used for scanning.\n
__Default__: 1M\n
Options |
--- |
1 M |
Coded |
`

// Long description for the scanType configuration parameter
const scanTypeLongDescription = `The Scan Type parameter specifies the type of\
scan to perform.\n
__Default__: Active\n
Options | Description
--- | ---
Active | Active Scanning. Scan request PDUs may be sent
Passive | Passive Scanning. No scan request PDUs shall be sent
`

// Long description for the scanInt configuration parameter
const scanIntLongDescription = `Time interval from when the Controller started\
its last scan until it begins the subsequent scan on the primary advertising\
channel.\n
__Default__: 500 ms\n
__Range__:  ${Common.connParamsRanges.scanIntMinValue} ms to \
${Common.connParamsRanges.scanIntMaxValue} ms\n
__Note__: Scan Interval shall be greater than or equal to Scan Window.\n
Scan Duration shall be greater than Scan Interval`

// Long description for the scanWin configuration parameter
const scanWinLongDescription = `Duration of the scan on the primary\
advertising channel.\n
__Default__: 500 ms\n
__Range__: ${Common.connParamsRanges.scanWinMinValue} ms to \
${Common.connParamsRanges.scanWinMaxValue} ms\n
__Note__: Scan Window shall be less than or equal to Scan Interval.`

// Long description for the scanDuration configuration parameter
const scanDurationLongDescription = `Scan Duration\n
__Default__: 1000 ms\n
__Range__: ${Common.connParamsRanges.scanDurationMinValue} ms to \
${Common.connParamsRanges.scanDurationMaxValue} ms\n
__Note__: Scan Duration shall be greater than Scan Interval.`

// Long description for the advRptFields configuration parameter
const advRptFieldsLongDescription = `Select which fields of an advertising\
report will be stored in the AdvRptList.\n
__Default__: Address Type, Address\n
Options |
--- |
Event Type |
Address Type |
Address |
Primary PHY |
Secondery PHY |
Advertise Set Id |
TX Power |
RSSI |
dirAddrType |
dirAddress |
prdAdvInterval |
Data Length |`

// Long description for the disDevBySerUuid configuration parameter
const disDevBySerUuidLongDescription = `When set to TRUE (checked), the \
application will filter and connect to peripheral devices with a desired \
service UUID. \n
__Note__: By defult, the service UUID is the TI Simple Profile UUID. \n
__Default__: False (unchecked)`

 // Exports the long descriptions for each configurable
exports = {
    scanPHYLongDescription: scanPHYLongDescription,
    scanTypeLongDescription: scanTypeLongDescription,
    scanIntLongDescription: scanIntLongDescription,
    scanWinLongDescription: scanWinLongDescription,
    scanDurationLongDescription: scanDurationLongDescription,
    advRptFieldsLongDescription: advRptFieldsLongDescription,
    disDevBySerUuidLongDescription: disDevBySerUuidLongDescription
};