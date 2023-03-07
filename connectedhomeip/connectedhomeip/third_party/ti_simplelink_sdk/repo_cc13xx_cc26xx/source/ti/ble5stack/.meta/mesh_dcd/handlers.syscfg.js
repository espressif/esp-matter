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
 *  ======== handlers.syscfg.js ========
 */

"use strict";

// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

const config = [
    {
        name: "numOfHandler",
        default: 0,
        description: "",
        hidden: true
    },
    {
        name: "handlerOpCode",
        displayName: "Opcode Parameter",
        displayFormat: "hex",
        displayFormat: {
            bitSize: 8,
            radix: "hex",
        },
        default: 0x00,
        description: "OpCode Parameter - range is (0x01 to 0xFF)",
    },
    {
        name: "handlerLenMsg",
        displayName: "Minimum Required Message Length",
        description: "Minimum Required Message Length - range is 1 to 255",
        default:0
    },
    {
        name: "handlerFunction",
        displayName: "Handler Name",
        description: "Handler function for this opcode",
        longDescription: "This area must be implemented in code B by the user\n\n\
        /** @brief Handler function for this opcode.\n\
        *\n\
        *  @param model Model instance receiving the message.\n\
        *  @param ctx   Message context for the message.\n\
        *  @param buf   Message buffer containing the message payload, not\n\
        *               including the opcode.\n\
        */",
        default:"handler",

    }
];


/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - BLE instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if(inst.handlerOpCode>255){
        validation.logError("Valid range is 1 to 255 (0x01 to 0xFF)",inst,"handlerOpCode");
    }
    if(inst.handlerLenMsg>255){
        validation.logError("Valid range is 1 to 255",inst,"handlerLenMsg");
    }
    if(!Common.alphanumeric(inst.handlerFunction)){
        validation.logError("Valid string contains only letters, numbers and underscore",inst,"handlerFunction");
    }
}


/*
 *  ======== exports ========
 *  Export the Advertisement Set module
 */
exports ={
    displayName: "Handler",
    config: config,
    validate: validate,
    //moduleInstances: moduleInstances
}