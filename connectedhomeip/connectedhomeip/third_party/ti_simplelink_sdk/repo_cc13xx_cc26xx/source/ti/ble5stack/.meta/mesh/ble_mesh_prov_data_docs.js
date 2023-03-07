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
 *  ======== ble_mesh_prov_data_docs.js ========
 */

"use strict";

// Long description for the uuid configuration parameter
const uuidLongDescription = `The UUID that's used when advertising \
unprovisioned beacon`

// Long description for the uri configuration parameter
const uriLongDescription = `A Uniform Resource Identifier (URI) may be \
specified (optional), it can point the provisioner to the location of some \
Out Of Band information, such as the device's public key or an authentication \
value database.\n
The URI is advertised in a separate beacon, however the unprovisioned beacon \
will contain a hash of it so the two can be associated by the provisioner.`

// Long description for the oobInfo configuration parameter
const oobInfoLongDescription = `Out of Band information field`

// Long description for the oobAuthenticationMethod configuration parameter
const oobAuthenticationMethodLongDescription = `Supported Authentication \
method for provisioning.\n
The provisioner uses the selected authentication method to authenticate the \
unprovisioned device.\n

Options | Description
--- | ---
None | No OOB is used
Static OOB | An authentication value is assigned to the device in production, \
which the provisioner can query in some application specific way
Input OOB | The provisioner generates a random number, and the user inputs it \
on the unprovisioned device
Output OOB | The device output a random number, and the user of the \
provisioner inputs the number observed to authenticate the unprovisioned \
device.
`

// Long description for the staticVal configuration parameter
const staticValLongDescription = `The Static OOB value`

// Long description for the outputSize configuration parameter
const outputSizeLongDescription = `Maximum size of Output OOB supported`

// Long description for the outputActions configuration parameter
const outputActionsLongDescription = `Supported Output OOB Actions`

// Long description for the inputSize configuration parameter
const inputSizeLongDescription = `Maximum size of Input OOB supported`

// Long description for the inputActions configuration parameter
const inputActionsLongDescription = `Supported Input OOB Actions`

// Long description for the outputNumberCbName configuration parameter
const outputNumberCbNameLongDescription = `
\n
/* Output of a number is requested.\n

This callback notifies the application that it should output the given \
number using the given action.\n

* param act - Action for outputting the number.\n
* param num - Number to be outputted.\n

return Zero on success or negative error code otherwise\n
*/`

// Long description for the outputStringCbName configuration parameter
const outputStringCbNameLongDescription = `
/* Output of a string is requested.\n

This callback notifies the application that it should display the given \
string to the user.\n

* param str - String to be displayed.\n

return Zero on success or negative error code otherwise\n
 */`

// Long description for the inputCbName configuration parameter
const inputCbNameLongDescription = `
/* Input is requested.

This callback notifies the application that it should request input from the \
user using the given action. The requested input will either be a string or a \
number, and the application needs to consequently call the \
bt_mesh_input_string() or bt_mesh_input_number() functions once the data has \
been acquired from the user.\n

* param act - Action for inputting data.\n
* param num - Maximum size of the inputted data.\n

return Zero on success or negative error code otherwise\n
*/`

// Long description for the inputCompleteCbName configuration parameter
const inputCompleteCbNameLongDescription = `
/* The other device finished their OOB input.\n

This callback notifies the application that it should stop displaying its \
output OOB value, as the other party finished their OOB input.\n
*/`

// Long description for the unprovBeaconCbName configuration parameter
const unprovBeaconCbNameLongDescription = `
/* Unprovisioned beacon has been received.\n

This callback notifies the application that an unprovisioned beacon has been \
received.\n

* param uuid     - UUID\n
* param oob_info - OOB Information\n
* param uri_hash - Pointer to URI Hash value. NULL if no hash was present in \
the beacon.\n
*/`

// Long description for the linkOpenCbName configuration parameter
const linkOpenCbNameLongDescription = `
/* Provisioning link has been opened.

This callback notifies the application that a provisioning link has been \
opened on the given provisioning bearer.\n

* param bearer - Provisioning bearer.\n
*/`

// Long description for the linkCloseCbName configuration parameter
const linkCloseCbNameLongDescription = `
/* Provisioning link has been closed.\n

This callback notifies the application that a provisioning link has been \
closed on the given provisioning bearer.\n

* param bearer - Provisioning bearer.\n
*/`

// Long description for the completeCbName configuration parameter
const completeCbNameLongDescription = `
/* Provisioning is complete.\n

This callback notifies the application that provisioning has been \
successfully completed, and that the local node has been assigned the \
specified NetKeyIndex and primary element address.\n

* param net_idx - NetKeyIndex given during provisioning.\n
* param addr    - Primary element address.\n
*/`

// Long description for the nodeAddedCbName configuration parameter
const nodeAddedCbNameLongDescription = `
/* A new node has been added to the provisioning database.\n

This callback notifies the application that provisioning has been \
successfully completed, and that a node has been assigned the specified \
NetKeyIndex and primary element address.\n

* param net_idx  - NetKeyIndex given during provisioning.\n
* param addr     - Primary element address.\n
* param num_elem - Number of elements that this node has.\n
*/`

// Long description for the resetCbName configuration parameter
const resetCbNameLongDescription = `
/* Node has been reset.\n

This callback notifies the application that the local node has been reset and \
needs to be reprovisioned. The node will not automatically advertise as \
unprovisioned, rather the bt_mesh_prov_enable() API needs to be called to \
enable unprovisioned advertising on one or more provisioning bearers.\n
*/`

// Exports the long descriptions for each configurable
 exports = {
    uuidLongDescription: uuidLongDescription,
    uriLongDescription: uriLongDescription,
    oobInfoLongDescription: oobInfoLongDescription,
    oobAuthenticationMethodLongDescription: oobAuthenticationMethodLongDescription,
    staticValLongDescription: staticValLongDescription,
    outputSizeLongDescription: outputSizeLongDescription,
    outputActionsLongDescription: outputActionsLongDescription,
    inputSizeLongDescription: inputSizeLongDescription,
    inputActionsLongDescription: inputActionsLongDescription,
    outputNumberCbNameLongDescription: outputNumberCbNameLongDescription,
    outputStringCbNameLongDescription: outputStringCbNameLongDescription,
    inputCbNameLongDescription: inputCbNameLongDescription,
    inputCompleteCbNameLongDescription, inputCompleteCbNameLongDescription,
    unprovBeaconCbNameLongDescription: unprovBeaconCbNameLongDescription,
    linkOpenCbNameLongDescription: linkOpenCbNameLongDescription,
    linkCloseCbNameLongDescription: linkCloseCbNameLongDescription,
    completeCbNameLongDescription: completeCbNameLongDescription,
    nodeAddedCbNameLongDescription: nodeAddedCbNameLongDescription,
    resetCbNameLongDescription: resetCbNameLongDescription
};