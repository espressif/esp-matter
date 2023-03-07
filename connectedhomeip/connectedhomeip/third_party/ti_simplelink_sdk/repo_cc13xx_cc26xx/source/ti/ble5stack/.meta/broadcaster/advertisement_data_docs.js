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
 *  ======== advertisement_data_docs.js ========
 */

"use strict";

// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

// Long description for the Shortened local name configuration parameter
const GAP_ADTYPE_LOCAL_NAME_SHORTLongDescription = `Choose if to enable the shortened \
version of the local Name. \n
The Local Name data type value indicates if the name is complete or shortened. If the\
name is shortened, the complete name can be read using the remote name request\
procedure over BR/EDR or by reading the device name characteristic after the\
connection has been established using GATT.\n
When using privacy feature, the device should not (not recommended) send the device\
name or unique data in the advertising data which can be used to recognize the device,\
i.e., "Address Mode" in "General Configuration" is set to "RPA with Public ID" or\
"RPA with Randon ID".\n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the Complete local name configuration parameter
const GAP_ADTYPE_LOCAL_NAME_COMPLETELongDescription = `Choose if to enable the complete\
version of the local Name. \n
The Local Name data type value indicates if the name is complete or shortened. If the\
name is shortened, the complete name can be read using the remote name request\
procedure over BR/EDR or by reading the device name characteristic after the\
connection has been established using GATT.\n
When using privacy feature, the device should not (not recommended) send the device\
name or unique data in the advertising data which can be used to recognize the device,\
i.e., "Address Mode" in "General Configuration" is set to "RPA with Public ID" or\
"RPA with Randon ID".\n
For more information, refer to the [Bluetooth Core Specification Supplement \
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the Gap Advertising Flags configuration parameter
const GAP_ADTYPE_FLAGSLongDescription = `Choose if Advertising Flags\
data type shall be included in the advertising packet\n\n
Options |
--- |
LE Limited Discoverable Mode |
LE General Discoverable Mode |
BR/EDR Not Supported |\n\n
__Note__: If the Flags AD type is not present in a non-connectable advertisement,\
the Flags should be considered as unknown and no assumptions should be made by the scanner.\n
__Default__: False (unchecked)\n
For more information, refer to the [Bluetooth Core Specification Supplement Document]\
(https://www.bluetooth\.com/specifications/bluetooth-core-specification/)\n`

// Long description for the More 16-bit UUIDs configuration parameter
const GAP_ADTYPE_16BIT_MORELongDescription = `Indicates that the 16-bit Service UUID\
list is incomplete. \n
__Note__: A packet or data block shall not contain more than one instance for each\
Service UUID data size. If a device has no Service UUIDs of a certain size,\
16-, 32-, or 128-bit, the corresponding field in the extended inquiry response or\
advertising data packet shall be marked as complete with no Service UUIDs.\
An omitted Service UUID data type shall be interpreted as an empty incomplete-list. \n
16-bit and 32-bit UUIDs shall only be used if they are assigned by the
Bluetooth SIG. The Bluetooth SIG may assign 16-bit and 32-bit UUIDs to
member companies or organizations. \n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the complete list 16-bit UUIDs configuration parameter
const GAP_ADTYPE_16BIT_COMPLETELongDescription = `Indicates that the 16-bit Service UUID\
list is complete. \n
__Note__: A packet or data block shall not contain more than one instance for each\
Service UUID data size. If a device has no Service UUIDs of a certain size,\
16-, 32-, or 128-bit, the corresponding field in the extended inquiry response or\
advertising data packet shall be marked as complete with no Service UUIDs.\
An omitted Service UUID data type shall be interpreted as an empty incomplete-list. \n
16-bit and 32-bit UUIDs shall only be used if they are assigned by the
Bluetooth SIG. The Bluetooth SIG may assign 16-bit and 32-bit UUIDs to
member companies or organizations. \n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the more 32-bit UUIDs configuration parameter
const GAP_ADTYPE_32BIT_MORELongDescription = `Indicates that the 32-bit Service UUID\
list is incomplete. \n
__Note__: A packet or data block shall not contain more than one instance for each\
Service UUID data size. If a device has no Service UUIDs of a certain size,\
16-, 32-, or 128-bit, the corresponding field in the extended inquiry response or\
advertising data packet shall be marked as complete with no Service UUIDs.\
An omitted Service UUID data type shall be interpreted as an empty incomplete-list. \n
16-bit and 32-bit UUIDs shall only be used if they are assigned by the
Bluetooth SIG. The Bluetooth SIG may assign 16-bit and 32-bit UUIDs to
member companies or organizations. \n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

 // Long description for the complete 32-bit UUIDs configuration parameter
const GAP_ADTYPE_32BIT_COMPLETELongDescription = `Indicates that the 32-bit Service UUID\
list is complete. \n
__Note__: A packet or data block shall not contain more than one instance for each\
Service UUID data size. If a device has no Service UUIDs of a certain size,\
16-, 32-, or 128-bit, the corresponding field in the extended inquiry response or\
advertising data packet shall be marked as complete with no Service UUIDs.\
An omitted Service UUID data type shall be interpreted as an empty incomplete-list. \n
16-bit and 32-bit UUIDs shall only be used if they are assigned by the
Bluetooth SIG. The Bluetooth SIG may assign 16-bit and 32-bit UUIDs to
member companies or organizations. \n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the more 128-bit UUIDs configuration parameter
const GAP_ADTYPE_128BIT_MORELongDescription = `Indicates that the 128-bit Service UUID\
list is incomplete. \n
__Note__: A packet or data block shall not contain more than one instance for each\
Service UUID data size. If a device has no Service UUIDs of a certain size,\
16-, 32-, or 128-bit, the corresponding field in the extended inquiry response or\
advertising data packet shall be marked as complete with no Service UUIDs.\
An omitted Service UUID data type shall be interpreted as an empty incomplete-list. \n
16-bit and 32-bit UUIDs shall only be used if they are assigned by the
Bluetooth SIG. The Bluetooth SIG may assign 16-bit and 32-bit UUIDs to
member companies or organizations. \n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the complete 128-bit UUIDs configuration parameter
const GAP_ADTYPE_128BIT_COMPLETELongDescription = `Indicates that the 128-bit Service UUID\
list is complete. \n
__Note__: A packet or data block shall not contain more than one instance for each\
Service UUID data size. If a device has no Service UUIDs of a certain size,\
16-, 32-, or 128-bit, the corresponding field in the extended inquiry response or\
advertising data packet shall be marked as complete with no Service UUIDs.\
An omitted Service UUID data type shall be interpreted as an empty incomplete-list. \n
16-bit and 32-bit UUIDs shall only be used if they are assigned by the
Bluetooth SIG. The Bluetooth SIG may assign 16-bit and 32-bit UUIDs to
member companies or organizations. \n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the TX Power Level configuration parameter
const GAP_ADTYPE_POWER_LEVELLongDescription = `The TX Power Level data type indicates\
the transmitted power level of the packet containing the data type. \n
__Note__: When the TX Power Level data type is not present, the TX power level of
the packet is unknown. \n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the TX Power configuration parameter
const TXPowerLongDescription = `The TX Power in DBm \n
__Default__: 0 \n
__Range__: To see the valid values refer to the "Default Tx Power" parameter \
in the Radio section \n`

// Long description for the GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE configuration parameter
const GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGELongDescription = `Contains the Peripheral’s
preferred connection interval range. \n
Configure Min and Max values of the connection interval. \n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the minConnInterval configuration parameter
const minConnIntervalLongDescription = `In BLE connections, a frequency-hopping \
scheme is used. The two devices each send and receive data from one another \
only on a specific channel at a specific time. These devices meet a specific \
amount of time later at a new channel (the link layer of the BLE protocol \
stack handles the channel switching). This meeting is where the two devices \
send and receive data, and is known as a connection event. If there is no \
application data to be sent or received, the two devices exchange link layer \
data to maintain the connection. The connection interval is the amount of \
time between two connection events in units of 1.25 ms.\n

__Default__: 100 ms\n
__Range__: ${Common.connParamsRanges.connectionIntMinValue} ms to \
${Common.connParamsRanges.connectionIntMaxValue} ms`

// Long description for the MaxConnInterval configuration parameter
const maxConnIntervalLongDescription = `In BLE connections, a frequency-hopping \
scheme is used. The two devices each send and receive data from one another \
only on a specific channel at a specific time. These devices meet a specific \
amount of time later at a new channel (the link layer of the BLE protocol \
stack handles the channel switching). This meeting is where the two devices \
send and receive data, and is known as a connection event. If there is no \
application data to be sent or received, the two devices exchange link layer \
data to maintain the connection. The connection interval is the amount of \
time between two connection events.\n

__Default__: 200 ms\n
__Range__: ${Common.connParamsRanges.connectionIntMinValue} ms to \
${Common.connParamsRanges.connectionIntMaxValue} ms`

// Long description for the Service Solicitation: list of 16-bit Service UUIDs configuration parameter
const GAP_ADTYPE_SERVICES_LIST_16BITLongDescription = `A Peripheral device may \
send the Service Solicitation data type to invite Central devices that expose one \
or more of the services specified in the Service Solicitation data to connect. \n
The Peripheral device should be in the undirected connectable mode and in one of \
the discoverable modes. This enables a Central device providing one or more of \
these services to connect to the Peripheral device, so that the Peripheral device \
can use the services on the Central device.\n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the Service Solicitation: list of 128-bit Service UUIDs configuration parameter
const GAP_ADTYPE_SERVICES_LIST_128BITLongDescription = `A Peripheral device may \
send the Service Solicitation data type to invite Central devices that expose one \
or more of the services specified in the Service Solicitation data to connect. \n
The Peripheral device should be in the undirected connectable mode and in one of \
the discoverable modes. This enables a Central device providing one or more of \
these services to connect to the Peripheral device, so that the Peripheral device \
can use the services on the Central device.\n
For more information, refer to the [Bluetooth Core Specification Supplement\
Document](https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the Service Data - 16-bit UUID configuration parameter
const GAP_ADTYPE_SERVICE_DATALongDescription = `The Service Data data type consists \
of a service UUID with the data associated with that service. \n
__Default__: False (unchecked)\n
__Note__: The data size will be at least 2 or more octets .The first 2 octets contain \
the 16 bit Service UUID followed by additional service data.\n`

// Long description for the Service Data - 32-bit UUID configuration parameter
const GAP_ADTYPE_SERVICE_DATA_32BITLongDescription = `The Service Data data type consists \
of a service UUID with the data associated with that service. \n
__Default__: False (unchecked)\n
__Note__: The data size will be at least 4 or more octets .The first 4 octets contain \
the 32 bit Service UUID followed by additional service data.\n`

// Long description for the Service Data - 128-bit UUID configuration parameter
const GAP_ADTYPE_SERVICE_DATA_128BITLongDescription = `The Service Data data type consists \
of a service UUID with the data associated with that service. \n
__Default__: False (unchecked)\n
__Note__: The data size will be at least 16 or more octets .The first 16 octets contain \
the 128 bit Service UUID followed by additional service data.\n`

// Long description for the Public Target Address configuration parameter
const GAP_ADTYPE_PUBLIC_TARGET_ADDRLongDescription = `The Public Target Address data \
type defines the address of one or more intended recipients of an advertisement when \
one or more devices were bonded using a public address. This data type is intended to \
be used to avoid a situation where a bonded device unnecessarily responds to an \
advertisement intended for another bonded device. \n
__Default__: False (unchecked)\n`

// Long description for the Random Target Address configuration parameter
const GAP_ADTYPE_RANDOM_TARGET_ADDRLongDescription = `The Random Target Address data \
type defines the address of one or more intended recipients of an advertisement when \
one or more devices were bonded using a random address. This data type is intended to \
be used to avoid a situation where a bonded device unnecessarily responds to an \
advertisement intended for another bonded device. \n
__Default__: False (unchecked)\n`

// Long description for the Apperance configuration parameter
const GAP_ADTYPE_APPEARANCELongDescription = `The Appearance data type defines \
the external appearance of the device. \n
__Default__: False (unchecked)\n`

// Long description for the Apperance Value configuration parameter
const appearanceValueLongDescription = `The Appearance data type defines \
the external appearance of the device. \n
__Note__: The Appearance value shall be the enumerated value as defined by \
Assigned Numbers.  For more information, refer to the [Bluetooth Core\
Specification Supplement Document](https://www.bluetooth.com/specifications/\
bluetooth-core-specification/)\n
__Default__: 0x0 \n`

// Long description for the Advertising Interval configuration parameter
const GAP_ADTYPE_ADV_INTERVALLongDescription = `The Advertising Interval data \
type contains the advInterval value as defined in the Core specification. \n
__Note__: The advInterval shall be an integer multiple of 0.625 ms in the \
range of 20 ms to 10,485.759375 s (according to the spec), but is limited to \
2 bytes and therefore the range is 20 ms to 40.959375 s. \n
For more information, refer to the \
[Bluetooth Core Specification Document](https://www.bluetooth.com/specifications/\
bluetooth-core-specification/)\n
__Default__: False (unchecked)\n`

// Long description for the Advertising Interval Value configuration parameter
const advIntValueLongDescription = ` \n
__Range__: ${Common.advParamsRanges.priAdvIntMinValue} ms to \
${Common.advParamsRanges.priAdvIntMaxValueAdvData} ms\n
__Note__: The advInterval shall be an integer multiple of 0.625 ms For more \
information, refer to the [Bluetooth Core Specification Document]\
(https://www.bluetooth.com/specifications/bluetooth-core-specification/)\n
__Default__: 20 \n`

// Long description for the Manufacturer Specific Data configuration parameter
const GAP_ADTYPE_MANUFACTURER_SPECIFICLongDescription = `Enables the use of the \
Manufacturer Specific Data type\n
__Default__: False (unchecked)\n`

// Long description for the Company Identifier configuration parameter
const companyIdentifierLongDescription = `First 2 octets contain \
the Company Identifier Code followed by the additional manufacturer specific data\n
__Default__: 0x0D00\n`

// Exports the long descriptions for each configurable
 exports = {
    GAP_ADTYPE_LOCAL_NAME_SHORTLongDescription: GAP_ADTYPE_LOCAL_NAME_SHORTLongDescription,
    GAP_ADTYPE_LOCAL_NAME_COMPLETELongDescription: GAP_ADTYPE_LOCAL_NAME_COMPLETELongDescription,
    GAP_ADTYPE_FLAGSLongDescription: GAP_ADTYPE_FLAGSLongDescription,
    GAP_ADTYPE_16BIT_MORELongDescription: GAP_ADTYPE_16BIT_MORELongDescription,
    GAP_ADTYPE_16BIT_COMPLETELongDescription: GAP_ADTYPE_16BIT_COMPLETELongDescription,
    GAP_ADTYPE_32BIT_MORELongDescription: GAP_ADTYPE_32BIT_MORELongDescription,
    GAP_ADTYPE_32BIT_COMPLETELongDescription: GAP_ADTYPE_32BIT_COMPLETELongDescription,
    GAP_ADTYPE_128BIT_MORELongDescription: GAP_ADTYPE_128BIT_MORELongDescription,
    GAP_ADTYPE_128BIT_COMPLETELongDescription: GAP_ADTYPE_128BIT_COMPLETELongDescription,
    GAP_ADTYPE_POWER_LEVELLongDescription: GAP_ADTYPE_POWER_LEVELLongDescription,
    TXPowerLongDescription: TXPowerLongDescription,
    GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGELongDescription: GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGELongDescription,
    minConnIntervalLongDescription: minConnIntervalLongDescription,
    maxConnIntervalLongDescription: maxConnIntervalLongDescription,
    GAP_ADTYPE_SERVICES_LIST_16BITLongDescription: GAP_ADTYPE_SERVICES_LIST_16BITLongDescription,
    GAP_ADTYPE_SERVICES_LIST_128BITLongDescription: GAP_ADTYPE_SERVICES_LIST_128BITLongDescription,
    GAP_ADTYPE_SERVICE_DATALongDescription: GAP_ADTYPE_SERVICE_DATALongDescription,
    GAP_ADTYPE_SERVICE_DATA_32BITLongDescription: GAP_ADTYPE_SERVICE_DATA_32BITLongDescription,
    GAP_ADTYPE_SERVICE_DATA_128BITLongDescription: GAP_ADTYPE_SERVICE_DATA_128BITLongDescription,
    GAP_ADTYPE_PUBLIC_TARGET_ADDRLongDescription: GAP_ADTYPE_PUBLIC_TARGET_ADDRLongDescription,
    GAP_ADTYPE_RANDOM_TARGET_ADDRLongDescription: GAP_ADTYPE_RANDOM_TARGET_ADDRLongDescription,
    GAP_ADTYPE_APPEARANCELongDescription: GAP_ADTYPE_APPEARANCELongDescription,
    appearanceValueLongDescription: appearanceValueLongDescription,
    GAP_ADTYPE_ADV_INTERVALLongDescription: GAP_ADTYPE_ADV_INTERVALLongDescription,
    advIntValueLongDescription: advIntValueLongDescription,
    GAP_ADTYPE_MANUFACTURER_SPECIFICLongDescription: GAP_ADTYPE_MANUFACTURER_SPECIFICLongDescription,
    companyIdentifierLongDescription: companyIdentifierLongDescription
};