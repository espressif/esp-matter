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
 *  ======== ble_general_docs.js ========
 */

"use strict";

// Long description for the deviceName configuration parameter
const deviceNameLongDescription = `The device name is the user-friendly name\
that a BLE device exposes to remote devices. The name is a character string\
held in the Device Name characteristic.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_\
guide/html/ble-stack-5.x/gatt.html#gatt-services-and-profile).\n
__Range__: Max Length of Device Name Attribute is 21`

// Long description for the addressMode configuration parameter
const addressModeLongDescription = `Devices are identified using a device\
address. Device addresses may be either a public device address or a random\
device address. A public device address and a random device address are both\
48 bits in length.\n
A device shall use at least one type of device address and may contain both.\n
Options | Description
--- | ---
Public Address | The local device will only use a Public device address
Random Static Address | The local device will only use a Random Static \
device address
RPA with Public ID | The local device will always use an RPA that can be \
resolved to a Public Identity Address. The device will only accept requests \
from peers who use its RPA over the air. It will not accept requests if the \
request is being directed to its identity address.
RPA with Random ID | The local device will always use an RPA that can be \
resolved to a Random Static Identity Address. The device will only accept \
requests from peers who use its RPA over the air. It will not accept peer \
requests if the request is being directed to its identity address.\n
For more information, refer to the [BLE User's Guide](ble5stack/\
ble_user_guide/html/ble-stack-5.x/privacy.html#privacy-and-address-types).\n
__Default__: RPA with Public ID \n
__Note__: When using the address mode as Random Static Address or RPA with \
Random ID, \`GAP_DeviceInit()\` should be called with its last parameter \
set to a static random address.\n `

// Long description for the randomAddress configuration parameter
const randomAddressLongDescription = `The Random Address of the device.\n
Please enter a valid address according to the [Bluetooth Core Specification \
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)`

// Long description for the rpaRead configuration parameter
const rpaReadLongDescription = `How often to read current RPA (in ms).\n
__Default__: 3000 ms\n
__Range__: 0 to sizeof(uint32)`

// Long description for the maxConnNum configuration parameter
const maxConnNumLongDescription = `This is the maximum number of simultaneous \
BLE connections allowed. Adding more connections uses more RAM and may \
require increasing HEAPMGR_SIZE.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/creating-a-custom-bluetooth-low-energy-application-cc13x2_26x2.\
html#ram-optimization).\n
__Default__: 8\n
__Range__: Central Role: 0 to 32, Other Roles: 0 to 16`

// Long description for the maxPDUNum configuration parameter
const maxPDUNumLongDescription = `Maximum number of BLE HCI PDUs.\n
__Default__: 5\n
__Range__: 0 to sizeof(uint8)\n
__Note__: If the maximum number of connections is set to 0, then this number \
should also be set to 0.\n
Set \`MAX_NUM_PDU\` and \`MAX_PDU_SIZE\` to reduce the amount of packets that\
can be queued up by the stack at a time. This will reduce heap consumption.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/creating-a-custom-bluetooth-low-energy-application-cc13x2_26x2.\
html#ram-optimization).\n`

// Long description for the maxPDUSize configuration parameter
const maxPDUSizeLongDescription = `Maximum size in bytes of the BLE HCI PDU.\n
__Default__: 69\n
__Range__: 27 to 255\n
__Note__: If using LE Secure Connections, MAX_PDU_SIZE must be >= 69\n
The maximum ATT_MTU is MAX_PDU_SIZE - 4\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/creating-a-custom-bluetooth-low-energy-application-cc13x2_26x2.\
html#ram-optimization).\n`

 // Exports the long descriptions for each configurable
 exports = {
    addressModeLongDescription: addressModeLongDescription,
    randomAddressLongDescription: randomAddressLongDescription,
    rpaReadLongDescription: rpaReadLongDescription,
    maxConnNumLongDescription: maxConnNumLongDescription,
    maxPDUNumLongDescription: maxPDUNumLongDescription,
    maxPDUSizeLongDescription: maxPDUSizeLongDescription,
    deviceNameLongDescription: deviceNameLongDescription
};