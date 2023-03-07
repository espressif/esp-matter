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
 *  ======== ble_docs.js ========
 */

"use strict";

// Long description for the deviceRole configuration parameter
const deviceRoleLongDescription = `The GAP Roles that the project \
operates as.\n
Role | Description
--- | ---
Broadcaster | The device is an advertiser that is non connectable
Observer | The device scans for advertisements but cannot initiate connections
Peripheral | The device is an advertiser that is connectable and operates as \
slave in a single or multiple link-layer connection
Central | The device scans for advertisements and initiates connections and \
operates as a master in a single or multiple link-layer connections
Central + Broadcaster | The device scans for advertisements and initiates \
connections and operates as a master in a single or multiple link-layer \
connections. In addition, the device is an advertiser that is non connectable
Peripheral + Observer | The device is an advertiser that is connectable and \
operates as slave in a single or multiple link-layer connection. In addition, \
the device scans for advertisements but cannot initiate connections
Peripheral + Central | The device is an advertiser that is connectable and \
operates as slave in a single or multiple link-layer connection. In addition, \
the device scans for advertisements and initiates connections and \
operates as a master in a single or multiple link-layer connections
\n\
For more information, refer to the [BLE Stack User's Guide]\
(ble5stack/ble_user_guide/html/ble-stack-5.x/gap.html#gap-roles).\n
__Default__: Peripheral\n`

// Long description for the bondManager configuration parameter
const bondManagerLongDescription = `The GAP Bond Manager (GAPBondMgr) is a\
configurable module that offloads most of the Pairing & Bonding security\
mechanisms associated with the Security Manager (SM) protocol from the application.\
The GAPBongMgr executes in the protocol stack task’s context. For more information,\
refer to the [BLE Stack User's Guide](/ble5stack/ble_user_guide/html/ble-stack-5.x/\
gapbondmngr.html#gap-bond-manager-and-le-secure-connections)\n
__Default__: True (checked)\n`

// Long description for the extAdv configuration parameter
const extAdvLongDescription = `BLE5 Extended advertising feature.\n
For more information, refer to the [BLE Stack User's Guide]\
(/ble5stack/ble_user_guide/html/ble-stack-5.x/gap-cc13x2_26x2.html)\n
__Default__: True (enabled)\n
__Note__: When the feature is disabled, remove all extended advertising sets`

// Long description for the periodicAdv configuration parameter
const periodicAdvLongDescription = `Periodic advertising feature.\n
For more information, refer to the [BLE Stack User's Guide]\
(/ble5stack/ble_user_guide/html/ble-stack-5.x/gap-cc13x2_26x2.html#periodic-\
advertising)\n
__Default__: False (disabled)\n
__Note__: When the feature is enabled, add non-connectable non-scannable \
extended advertising set`

// Long description for the periodicAdvSync configuration parameter
const periodicAdvSyncLongDescription = `Periodic adv sync feature.\n
Synchronization can only occur when scanning is enabled. While scanning is \
disabled, no attempt to synchronize will take place.\n
For more information, refer to the [BLE Stack User's Guide]\
(/ble5stack/ble_user_guide/html/ble-stack-5.x/gap-cc13x2_26x2.html#periodic-\
advertising)\n
__Default__: False (disabled)\n`

// Long description for the gattDB configuration parameter
const gattDBLongDescription = `Indicates that the GATT database is maintained off\
the chip on the Application Processor (AP)\n
__Default__: False (unchecked)\n`

// Long description for the gattNoClient configuration parameter
const gattNoClientLongDescription = `In order to use Privacy, the GAP Bond \
Manager requires the stack’s GATT client to read the peer device’s GAP \
characteristics. To accomplish this, GATT_NO_CLIENT should be \
unchecked so that the GATT client is included.\n
For more information, refer to the [BLE Stack User's Guide](/ble5stack/\
ble_user_guide/html/ble-stack-5.x/privacy.html#using-privacy-in-stack)\n
__Default__: False (unchecked)\n`

// Long description for the L2CAPCOC configuration parameter
const L2CAPCOCLongDescription = `Enable/Disable the use of L2CAP Connection Oriented Channels.\
The BLE5-Stack provides APIs to create L2CAP CoC channels to transfer bidirectional data between\
two Bluetooth Low Energy devices supporting this feature. For more information, refer to the \
[BLE Stack User's Guide](/ble5stack/ble_user_guide/html/ble-stack-common/l2cap.html#connection\
-oriented-channels-example) .\n
__Default__: False (unchecked)\n`

// Long description for the delayingAttReadReq configuration parameter
const delayingAttReadReqLongDescription = `Delaying an ATT_READ_REQ from a \
registered service is possible by enabling this parameter.\n
For more information, refer to the [BLE Stack User's Guide](/ble5stack/\
ble_user_guide/html/ble-stack-5.x/gatt.html#delaying-an-att-read-request)\n
__Default__: False (unchecked)\n`

// Long description for the trensLayer configuration parameter
const trensLayerLongDescription = `HCI Transport Layer Mode.\n
For more information, refer to the [BLE Stack User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/hci.html).\n
__Default__: None \n
__Note__: When using PTM configuration please choose \`None\` \n`


 // Exports the long descriptions for each configurable
 exports = {
    deviceRoleLongDescription: deviceRoleLongDescription,
    bondManagerLongDescription: bondManagerLongDescription,
    extAdvLongDescription: extAdvLongDescription,
    periodicAdvLongDescription: periodicAdvLongDescription,
    periodicAdvSyncLongDescription: periodicAdvSyncLongDescription,
    gattDBLongDescription: gattDBLongDescription,
    gattNoClientLongDescription: gattNoClientLongDescription,
    L2CAPCOCLongDescription: L2CAPCOCLongDescription,
    delayingAttReadReqLongDescription: delayingAttReadReqLongDescription,
    trensLayerLongDescription: trensLayerLongDescription
};