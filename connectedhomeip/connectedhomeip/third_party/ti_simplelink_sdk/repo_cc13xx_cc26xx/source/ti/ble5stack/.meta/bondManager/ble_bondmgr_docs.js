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
 *  ======== ble_bondmgr_docs.js ========
 */

"use strict";

// Long description for the maxBonds configuration parameter
const maxBondsLongDescription = `Maximum number of bonds that can be saved in NV.\n
__Default__: 10\n`

// Long description for the maxCharCfg configuration parameter
const maxCharCfgLongDescription = `Maximum number of characteristic configurations \
that can be saved in NV.\n
__Default__: 4\n`

// Long description for the bondPairing configuration parameter
const bondPairingLongDescription = ` The Pairing mode dictates whether to allow pairing,\
and if so, whether to initiate pairing.\n
__Default__: Wait for a pairing request\n
Pairing Mode | Description
--- | ---
Pairing is not allowed | With pairing mode set to \`GAPBOND_PAIRING_MODE_NO_PAIRING\`,\
the BLE stack automatically rejects any attempt at pairing.
Wait for a pairing request | The Peripheral can be configured to wait for a Pairing Request\
from the Central when the pairing mode is set to \`GAPBOND_PAIRING_MODE_WAIT_FOR_REQ\`.\
When this pairing mode is selected, the GAPBondMgr will automatically respond with a\
Pairing Response based on other GAPBondMgr configured parameters.
Initiate a pairing request | To initiate the pairing process on Peripheral role devices,\
\`GAPBOND_PAIRING_MODE_INITIATE\` will send a Slave Security Request shortly after the\
GAPBondMgr is informed that the connection is formed. For Central role devices,\
\`GAPBOND_PAIRING_MODE_INITIATE\` will send a Pairing Request or request the Link Layer\
to encrypt the link if the device has previously paired/bonded
\n\
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/ble-stack-5.x/\
gapbondmngr-cc13x2_26x2.html#gapbondmgr-examples-for-different-pairing-methods).\n`

// Long description for the bondMITMProtection configuration parameter
const bondMITMProtectionLongDescription = `Man in the Middle protection. MITM provides\
authentication during the pairing process which helps prevent a malicious attacker\
from impersonating the peer device during the key exchange.\n
__Default__: True (checked)\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/gapbondmngr-cc13x2_26x2.html#selection-of-pairing-method).\n`

// Long description for the bondIOCaps configuration parameter
const bondIOCapsLongDescription = `The I/O capabilities of the local device \n
__Default__: Display Only Device\n
Options |
--- |
Display Only Device |
Display and Yes and No Capable |
Keyboard Only |
No Display or Input Device |
Both Keyboard and Display Capable |\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/gapbondmngr-cc13x2_26x2.html#selection-of-pairing-method).\n`

// Long description for the bonding configuration parameter
const bondingLongDescription = `Storing the keys generated during the pairing\
process in nonvolatile memory to use for the next encryption sequence.\n
__Default__: True (checked)\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/gapbondmngr-cc13x2_26x2.html#gapbondmgr-example-with-bonding-enabled).\n`

// Long description for the bondMITMProtection configuration parameter
const secureConnLongDescription = `If enabled, LE Secure Connections will use an ECDH public\
-private key pair for the pairing process. as part of LESC pairing Phase 1 each device will \
generate its own ECDH public-private key pair. As part of LESC pairing Phase 2 each device \
will compute the Diffie-Hellman (DH) Key based on the public keys that are exchanged.\n
__Default__: Supported\n
Options |
--- |
Supported |
LE Secure Connection Only |\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/\
html/ble-stack-5.x/gapbondmngr-cc13x2_26x2.html#selection-of-pairing-method).\n`

// Long description for the authenPairingOnly configuration parameter
const authenPairingOnlyLongDescription = `If enabled and the key generation method does not result in a key that provides Authenticated MITM protection \
then the device will send the Pairing Failed command with the error code Authentication Requirements\n
__Default__: False (unchecked)\n`

// Long description for the syncWLWithBondDev configuration parameter
const syncWLWithBondDevLongDescription = `Synchronize the whitelist with bonded devices.\n
If \`TRUE\`, the whitelist will first be cleared. Then, each unique address stored\
by bonds in SNV will be synched with the whitelist indefinitely or until this is set\
to \`FALSE\` or the maximum number of supported bonds is reached\n
__Default__: False (unchecked)\n
For more information, refer to the [BLE Stack API Reference](/ble5stack/ble_user_guide/\
doxygen/ble/html/group___g_a_p_bond_mgr___params.html).\n`

// Long description for the ECCKeyRegenPolicy configuration parameter
const ECCKeyRegenPolicyLongDescription = `Define reuse of the private and public ECC keys\
for multiple pairings.The default is to always regenerate the keys upon each new pairing.\
This parameter has no effect when the application specifies the keys using the \`GAPBOND_ECC_KEYS\` \
parameter. The behavior is that upon each pairing the number of recycles remaining is decremented \
by 1, but if the pairing fails the count is decremented by 3. The specification recommends that \
this value be set to no higher than 10 to avoid an attacker from learning too much about a\
private key before it is regenerated.\n
__Note__: Only applicable for Secure Connections.\n
__Default__: 0\n
__Range__: 0 to 255\n
For more information, refer to the [BLE Stack API Reference](/ble5stack/ble_user_guide/\
doxygen/ble/html/group___g_a_p_bond_mgr___params.html).\n`

// Long description for the LTKSize configuration parameter
const LTKSizeLongDescription = `Size of the Long Term Key (LTK) used for pairing\n
__Default__: 16\n
For more information, refer to the [BLE Stack API Reference](/ble5stack/ble_user_guide/\
doxygen/ble/html/group___g_a_p_bond_mgr___params.html).\n`

// Long description for the lruBondRep configuration parameter
const lruBondRepLongDescription = ` If this parameter is set to false, it is not possible\
to add any more bonds without manually deleting a bond. If the parameter is set to true,\
the least recently used bond is deleted to make room for the new bond.\n
__Default__: False (unchecked)\n
__Note__: Only applies when the bonding information table is full.\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/gapbondmngr-cc13x2_26x2.html#gapbondmgr-and-snv).\n`

// Long description for the bondFailAction configuration parameter
const bondFailActionLongDescription = `The action that the device takes after an\
unsuccessful bonding attempt.\n
__Default__: Terminate link\n
Options |
--- |
Take no action |
Initiate pairing |
Terminate link |
Terminate link and erase all existing bonds on device |\n
For more information, refer to the [BLE Stack API Reference](/ble5stack/ble_user_guide/\
doxygen/ble/html/group___g_a_p_bond_mgr___params.html).\n`

// Long description for the keyDistList configuration parameter
const keyDistListLongDescription = `Key distribution list for pairing.\n
__Default__: Slave Encryption Key & Slave IRK and ID information & Slave CSRK\
& Master Encryption Key & Master IRK and ID information & Master CSRK\n
Options |
--- |
Slave Encryption Key |
Slave IRK and ID information |
Slave CSRK |
Slave Link Key |
Master Encryption Key |
Master IRK and ID information |
Master CSRK |
Master Link Key |\n
For more information, refer to the [BLE Stack API Reference](/ble5stack/ble_user_guide/\
doxygen/ble/html/group___g_a_p_bond_mgr___params.html).\n`

// Long description for the eccDebugKeys configuration parameter
const eccDebugKeysLongDescription = `The private and public keys to use for Secure\
Connections pairing\n
Only applicable for Secure Connections. Allows the application to specify the private\
and public keys to use pairing. When this is set, the keys are used indefinitely even\
if a regeneration policy was set with \`GAPBOND_ECCKEY_REGEN_POLICY\`. To make the Bond\
Manager stop using these keys, pass a 1 byte value of 0x00. These keys are stored in\
RAM and are not retained in non-volatile memory. These keys can be defined by the\
application, or the application can request them using the \`SM_GetEccKeys\` command.\n
__Size__: \`gapBondEccKeys_t\` \n
__Default__:  by default, the keys are generated using \`GAPBOND_ECCKEY_REGEN_POLICY\` \n
__Rage__: A valid \`gapBondEccKeys_t\` structure \n
For more information, refer to the [BLE Stack API Reference](/ble5stack/ble_user_guide/\
doxygen/ble/html/group___g_a_p_bond_mgr___params.html).\n`

// Long description for the allowDebugKeys configuration parameter
const allowDebugKeysLongDescription = `Allow or reject debug keys during public keys exchange.\n
Only applicable for Secure Connections. Allows the application to specify if the device should \
reject debug keys or accept them from the peer during public keys exchange.\n
If \`FALSE\`, reject debug keys .\n
If \`TRUE\`, accept debug keys .\n
__Default__: True (checked)\n
For more information, refer to the [BLE Stack API Reference](/ble5stack/ble_user_guide/\
doxygen/ble/html/group___g_a_p_bond_mgr___params.html).\n`

// Long description for the eraseBondWhileInConn configuration parameter
const eraseBondWhileInConnLongDescription = `Allows to erase a bond even if there \
are active connections.\n
If \`FALSE\`, a bond can be earsed only when there are no active connections.\n
If \`TRUE\`, a bond can be always earsed (when active connections are zero or not).\n
__Note__: Once performing \`GAPBOND_ERASE_SINGLEBOND\`, the erase would be done according to the \
status of the flag and the number of connections.\n
__Default__: False (unchecked)\n
For more information, refer to the [BLE User's Guide](ble5stack/ble_user_guide/html/\
ble-stack-5.x/gapbondmngr-cc13x2_26x2.html#gapbondmgr-and-snv)\n`

 // Exports the long descriptions for each configurable
 exports = {
    maxBondsLongDescription: maxBondsLongDescription,
    maxCharCfgLongDescription: maxCharCfgLongDescription,
    bondPairingLongDescription: bondPairingLongDescription,
    bondMITMProtectionLongDescription: bondMITMProtectionLongDescription,
    bondIOCapsLongDescription: bondIOCapsLongDescription,
    bondingLongDescription:bondingLongDescription,
    secureConnLongDescription: secureConnLongDescription,
    authenPairingOnlyLongDescription: authenPairingOnlyLongDescription,
    syncWLWithBondDevLongDescription: syncWLWithBondDevLongDescription,
    ECCKeyRegenPolicyLongDescription: ECCKeyRegenPolicyLongDescription,
    LTKSizeLongDescription: LTKSizeLongDescription,
    lruBondRepLongDescription: lruBondRepLongDescription,
    bondFailActionLongDescription: bondFailActionLongDescription,
    keyDistListLongDescription: keyDistListLongDescription,
    eccDebugKeysLongDescription: eccDebugKeysLongDescription,
	allowDebugKeysLongDescription: allowDebugKeysLongDescription,
    eraseBondWhileInConnLongDescription: eraseBondWhileInConnLongDescription
};