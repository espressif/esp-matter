/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== code_export_param_docs.js ========
 *  Documentation for Code Export Parameters.
 */

"use strict";

const customOverrideDescription = `
The custom override feature allows adding overrides that are not part of the standard overrides for  
Radio Core software. Custom overrides may be applied on two levels. *Stack overrides* are used by for instance
the BLE stack to apply overrides that this particular stack needs but are not available as a core feature.
In addition *Application overrides* may be applied when there is a need for fine tuning of a particular application,
for instance Angle of Arrival. Stack and application overrides may be combined.

Inclusion of custom overrides is facilitated by the *Code Export Configuration* panel where a pair of configurables are 
provided for each of the two stack override types.

The "Stack Override File" expects a path name to the file where the stack override macro resides. The path must be 
relative to the SDK source directory. Example: "ti/devices/radioconfig/stack_override_example.h". The Stack override 
macro must be a macro-name that is defined in the Stack Override File, e.g. BLE_STACK_OVERRIDES.

"Application Override File" and "Application Override Macro" exhibits exactly the same behavior as the stack override.

It is possible to select files outside the SDK installation by "moving up" from the root, 
e.g ../../mysource/app_override.h

The SDK contains two example files, one for each custom override. The files can be found in 
"source/ti/devices/radioconfig" and contain further details.
`;

const codeExportDocs = [
    {
        configurable: "symGenMethod",
        longDescription: "Selects the desired Symbol Name Generation Method:\n\n"
            + " * Legacy - Compatible with SmartRF Studio. "
            + " Intended for use in a single setting application, multiple setting using Legacy mode is not allowed.\n"
            + " * Automatic - The short notation for the PHY setting is appended to the symbol.\n"
            + " * Custom - The symbol names are individually configured."
    },
    {
        configurable: "useConst",
        longDescription: "Add the *const* prefix to the generated RF commands.\n\n"
            + "__Default__: false"
    },
    {
        configurable: "useMulti",
        longDescription: "Use multi-protocol rather than individual protocol patches.\n\n"
            + "__Default__: false"
    },
    {
        configurable: "cmdPrefix",
        longDescription: "Sets the prefix to use before command names in the generated code.\n\n"
            + "__Default__: RF_"
    },
    {
        configurable: "cmdFormat",
        longDescription: "Select how the command names shall be formatted in the generated code.\n\n"
            + "The available options are:\n"
            + " * CamelCase\n"
            + " * underscore_case\n\n"
            + "__Default__: CamelCase"
    },
    {
        configurable: ["cmdList_prop", "cmdList_ble", "cmdList_ieee_15_4"],
        longDescription: "The list of available commands to choose from."
    },
    {
        configurable: "rfMode",
        longDescription: "Sets the name for the RF mode object."
    },
    {
        configurable: "txPower",
        longDescription: "Sets the name for the TX power table array."
    },
    {
        configurable: "txPowerSize",
        longDescription: "Sets the name for the TX power Table size variable."
    },
    {
        configurable: "overrides",
        longDescription: "Sets the name for the Override array."
    },
    {
        configurable: "stackOverride",
        longDescription: customOverrideDescription
    },
    {
        configurable: "stackOverrideMacro",
        longDescription: customOverrideDescription
    },
    {
        configurable: "appOverride",
        longDescription: customOverrideDescription
    },
    {
        configurable: "appOverrideMacro",
        longDescription: customOverrideDescription
    }
];

exports = {
    codeExportDocs: codeExportDocs
};
