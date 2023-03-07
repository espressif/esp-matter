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
 *  ======== ble_bondmgr.syscfg.js ========
 */

"use strict";

// Get Bond Manager long descriptions
const Docs = system.getScript("/ti/ble5stack/bondManager/ble_bondmgr_docs.js");

const config = {
    name: "bondMgrConfig",
    displayName:"Bond Manager",
    description: "Configure Bonding Settings",
    config: [
        {
            name: "hideBondMgrGroup",
            default: false,
            hidden: true
        },
        {
            name: "maxBonds",
            displayName: "Maximum Number of Supported Bonds",
            description: "Maximum number of bonds that can be saved in NV",
            default: 10,
            longDescription: Docs.maxBondsLongDescription
        },
        {
            name: "maxCharCfg",
            displayName: "Maximum Number of Characteristic",
            description: "Maximum number of characteristic configuration that can be saved in NV",
            default: 4,
            longDescription: Docs.maxCharCfgLongDescription
        },
        {
            name: "bondPairing",
            displayName: "Pairing Mode",
            default: "GAPBOND_PAIRING_MODE_WAIT_FOR_REQ",
            readOnly: false,
            longDescription: Docs.bondPairingLongDescription,
            options: [
                { displayName: "Pairing is not allowed",     name: "GAPBOND_PAIRING_MODE_NO_PAIRING"   },
                { displayName: "Wait for a pairing request", name: "GAPBOND_PAIRING_MODE_WAIT_FOR_REQ" },
                { displayName: "Initiate a pairing request", name: "GAPBOND_PAIRING_MODE_INITIATE"     }
            ]
        },
        {
            name: "bondMITMProtection",
            displayName: "MITM Protection",
            longDescription: Docs.bondMITMProtectionLongDescription,
            default: true,
            onChange: onbondMITMProtectionChange
        },
        {
            name: "bondIOCaps",
            displayName: "IO Capabilities",
            default: "GAPBOND_IO_CAP_DISPLAY_ONLY",
            longDescription: Docs.bondIOCapsLongDescription,
            options: [
                { displayName: "Display Only Device",               name: "GAPBOND_IO_CAP_DISPLAY_ONLY"       },
                { displayName: "Display and Yes and No Capable",    name: "GAPBOND_IO_CAP_DISPLAY_YES_NO"     },
                { displayName: "Keyboard Only",                     name: "GAPBOND_IO_CAP_KEYBOARD_ONLY"      },
                { displayName: "No Display or Input Device",        name: "GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT" },
                { displayName: "Both Keyboard and Display Capable", name: "GAPBOND_IO_CAP_KEYBOARD_DISPLAY"   },
            ]
        },
        {
            name: "bonding",
            displayName: "Bonding",
            longDescription: Docs.bondingLongDescription,
            default: true
        },
        {
            name: "secureConn",
            displayName: "Secure Connection",
            default: "GAPBOND_SECURE_CONNECTION_ALLOW",
            onChange: onSecureConnChange,
            longDescription: Docs.secureConnLongDescription,
            options: [
                { displayName: "Supported",     name: "GAPBOND_SECURE_CONNECTION_ALLOW" },
                { displayName: "Only",          name: "GAPBOND_SECURE_CONNECTION_ONLY"  }
            ]
        },
        {
            name: "authenPairingOnly",
            displayName: "Authenticated Pairing Only",
            longDescription: Docs.authenPairingOnlyLongDescription,
            default: false
        },
        {
            name: "syncWLWithBondDev",
            displayName: "Sync Whitelist With Bonded Devices",
            longDescription: Docs.syncWLWithBondDevLongDescription,
            default: false
        },
        {
            name: "ECCKeyRegenPolicy",
            displayName: "ECC Key Regeneration Policy",
            hidden: false,
            longDescription: Docs.ECCKeyRegenPolicyLongDescription,
            default: 0
        },
        {
            name: "LTKSize",
            displayName: "Long Term Key (LTK) Size",
            readOnly: false,
            hidden: false,
            longDescription: Docs.LTKSizeLongDescription,
            default: 16
        },
        {
            name: "lruBondRep",
            displayName: "LRU Bond Replacement",
            longDescription: Docs.lruBondRepLongDescription,
            default: false
        },
        {
            name: "bondFailAction",
            displayName: "Bond Fail Action",
            default: "GAPBOND_FAIL_TERMINATE_LINK",
            longDescription: Docs.bondFailActionLongDescription,
            hidden: true,
            options: [
                {
                    displayName: "Take no action",
                    name: "GAPBOND_FAIL_NO_ACTION"
                },
                {
                    displayName: "Initiate pairing",
                    name: "GAPBOND_FAIL_INITIATE_PAIRING"
                },
                {
                    displayName: "Terminate link",
                    name: "GAPBOND_FAIL_TERMINATE_LINK"
                },
                {
                    displayName: "Terminate link and erase all existing bonds on device",
                    name: "GAPBOND_FAIL_TERMINATE_ERASE_BONDS"
                }
            ]
        },
        {
            name: "keyDistList",
            displayName: "Key Distribution List for Pairing",
            longDescription: Docs.keyDistListLongDescription,
            default: ["GAPBOND_KEYDIST_SENCKEY", "GAPBOND_KEYDIST_SIDKEY", "GAPBOND_KEYDIST_SSIGN",
                      "GAPBOND_KEYDIST_MENCKEY", "GAPBOND_KEYDIST_MIDKEY", "GAPBOND_KEYDIST_MSIGN"],
            options: [
                {
                    displayName: "Slave Encryption Key",
                    name: "GAPBOND_KEYDIST_SENCKEY"
                },
                {
                    displayName: "Slave IRK and ID information",
                    name: "GAPBOND_KEYDIST_SIDKEY",
                    description: "Resolving Identity Key"
                },
                {
                    displayName: "Slave CSRK",
                    name: "GAPBOND_KEYDIST_SSIGN",
                    description: "Connection Signature Resolving Key"
                },
                {
                    displayName: "Slave Link Key",
                    name: "GAPBOND_KEYDIST_SLINK"
                },
                {
                    displayName: "Master Encryption Key",
                    name: "GAPBOND_KEYDIST_MENCKEY"
                },
                {
                    displayName: "Master IRK and ID information",
                    name: "GAPBOND_KEYDIST_MIDKEY",
                    description: "Resolving Identity Key"
                },
                {
                    displayName: "Master CSRK",
                    name: "GAPBOND_KEYDIST_MSIGN",
                    description: "Connection Signature Resolving Key"
                },
                {
                    displayName: "Master Link Key",
                    name: "GAPBOND_KEYDIST_MLINK"
                }
            ]
        },
        {
            name: "eccDebugKeys",
            displayName: "ECC Debug Keys",
            longDescription: Docs.eccDebugKeysLongDescription,
            onChange: onEccDebugKeysChange,
            hidden: false,
            default: false
        },
		{
            name: "allowDebugKeys",
            displayName: "Allow Receiving Debug Keys",
            longDescription: Docs.allowDebugKeysLongDescription,
            onChange: onAllowDebugKeysChange,
            hidden: false,
            default: true
        },
        {
            name: "eraseBondWhileInConn",
            displayName: "Erase Bond In Active Connections",
            longDescription: Docs.eraseBondWhileInConnLongDescription,
            hidden: false,
            default: false
        }
    ]
}

/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - Bond Manager instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    //check what is the max value
    if(inst.maxBonds < 0 || inst.maxBonds > 32)
    {
        validation.logError("The Max number of bonds that can be saved in NV is 32"
                            , inst, "maxBonds");
    }

    if(inst.maxBonds > 21)
    {
        validation.logWarning("If scanning or connection initiation on the 2M PHY is used, "
        + "the max number of bonds that can be saved in NV is 21.", inst, "maxBonds");
    }

    if(inst.maxCharCfg < 0 || inst.maxCharCfg > 4)
    {
        validation.logError("The Max number of Characteristics that can be saved in "
                            + "NV for each bond is 4", inst, "maxCharCfg");
    }

    if(inst.LTKSize < 7 || inst.LTKSize > 16)
    {
        validation.logError("Key Size Range is 7 to 16 bytes", inst, "LTKSize");
    }

    if(inst.ECCKeyRegenPolicy < 0 || inst.ECCKeyRegenPolicy > 255)
    {
        validation.logError("ECC Key Regeneration Policy Range is 0 to 255", inst, "ECCKeyRegenPolicy");
    }

    if(inst.ECCKeyRegenPolicy > 10 && inst.ECCKeyRegenPolicy < 256)
    {
        validation.logWarning("The specification recommends that this value be set to no higher "
                              + "than 10 to avoid an attacker from learning too much about a "
                              + "private key before it is regenerated", inst, "ECCKeyRegenPolicy");
    }
}

/*
 * ======== onbondMITMProtectionChange ========
 * When MITM is set to flase
 * the authenPairingOnly value is changed to false and to readOnly mode
 *
 * When other option is selected the readOnly mode is disabled
 *
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onbondMITMProtectionChange(inst, ui)
{
    if(inst.bondMITMProtection == false)
    {
        inst.authenPairingOnly = false;
        ui.authenPairingOnly.readOnly = true;
    }
    else
    {
        ui.authenPairingOnly.readOnly = false;
    }
}

/*
 * ======== onSecureConnChange ========
 * When the GAPBOND_SECURE_CONNECTION_NONE option is selected 
 * the Key Size value is changed to 16 and to readOnly mode
 * 
 * When other option is selected the readOnly mode is disabled
 *
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onSecureConnChange(inst, ui)
{
    if(inst.secureConn == "GAPBOND_SECURE_CONNECTION_ONLY")
    {
        inst.LTKSize = 16;
        ui.LTKSize.readOnly = true;
    }
    else
    {
        ui.LTKSize.readOnly = false;
    }

    if(inst.secureConn == "GAPBOND_SECURE_CONNECTION_NONE")
    {
        ui.ECCKeyRegenPolicy.hidden = true;
        ui.LTKSize.hidden = true;
        ui.eccDebugKeys.hidden = true;
		ui.allowDebugKeys.hidden = true;
    }
    else
    {
        ui.ECCKeyRegenPolicy.hidden = false;
        ui.LTKSize.hidden = false;
        ui.eccDebugKeys.hidden = false;
		ui.allowDebugKeys.hidden = false;
    }

}

/*
* ======== onEccDebugKeysChange ========
* When using eccDebugKeys, GAPBOND_ECCKEY_REGEN_POLICY is not
* applied. Therefore, hide ECCKeyRegenPolicy
*
*
* @param inst - Module instance containing the config that changed
* @param ui - The User Interface object
*/
function onEccDebugKeysChange(inst, ui)
{
    if(inst.eccDebugKeys)
    {
        ui.ECCKeyRegenPolicy.hidden = true;
    }
    else
    {
        ui.ECCKeyRegenPolicy.hidden = false;
    }
}

/*
* ======== onAllowDebugKeysChange ========
* When not using allowDebugKeys, debug keys will not be used. Therefore, hide eccDebugKeys
*
*
* @param inst - Module instance containing the config that changed
* @param ui - The User Interface object
*/
function onAllowDebugKeysChange(inst, ui)
{
    if(inst.allowDebugKeys)
    {
        ui.eccDebugKeys.hidden = false;
    }
    else
    {
        ui.eccDebugKeys.hidden = true;
    }
}

/*
 *  ======== exports ========
 *  Export the BLE bond manager Configuration
 */
exports = {
    config: config,
    validate: validate
};
