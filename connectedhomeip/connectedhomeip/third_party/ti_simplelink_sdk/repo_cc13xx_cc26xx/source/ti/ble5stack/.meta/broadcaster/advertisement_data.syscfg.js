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
 *  ======== advertisement_data.syscfg.js ========
 */

"use strict";
// Get advertisement_data long descriptions
const Docs = system.getScript("/ti/ble5stack/broadcaster/advertisement_data_docs.js");

// Get common utility functions
const Common = system.getScript("/ti/ble5stack/ble_common.js");

const config = [
    {
        name: "name",
        displayName: "Name",
        default: ""
    },
    {
        displayName: "Shortened Local Name",
        name: "GAP_ADTYPE_LOCAL_NAME_SHORT",
        longDescription: Docs.GAP_ADTYPE_LOCAL_NAME_SHORTLongDescription,
        default: false,
        onChange: onShortenedLocalNameChange
    },
    {
        name: "shortenedLocalName",
        displayName: "Shortened Local Name",
        default: "",
        hidden: true
    },
    {
        displayName: "Complete Local Name",
        name: "GAP_ADTYPE_LOCAL_NAME_COMPLETE",
        longDescription: Docs.GAP_ADTYPE_LOCAL_NAME_COMPLETELongDescription,
        default: false,
        onChange: onCompleteLocalNameChange
    },
    {
        name: "completeLocalName",
        displayName: "Complete Local Name",
        default: "",
        getValue: getDeviceNameValue,
        readOnly: true,
        hidden: true
    },
    {
        name: "hideAdvFlags",
        displayName: "Hide Advertising Flags",
        default: false,
        hidden: true,
        onChange: onHideAdvFlagsChange
    },
    {
        displayName: "Gap Advertising Flags",
        name: "GAP_ADTYPE_FLAGS",
        longDescription: Docs.GAP_ADTYPE_FLAGSLongDescription,
        default: false,
        readOnly: false,
        onChange: onGapAdveFlagsChange
    },
    {
        name: "advertisingFlags",
        displayName: "Advertising Flags",
        default: ["GAP_ADTYPE_FLAGS_GENERAL"],
        hidden: true,
        options: [
            {
                displayName: "LE Limited Discoverable Mode",
                name: "GAP_ADTYPE_FLAGS_LIMITED"
            },
            {
                displayName: "LE General Discoverable Mode",
                name: "GAP_ADTYPE_FLAGS_GENERAL"
            },
            {
                displayName: "BR/EDR Not Supported",
                name: "GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED"
            }
        ]
    },
    {
        displayName: "More 16-bit UUIDs Available",
        name: "GAP_ADTYPE_16BIT_MORE",
        longDescription: Docs.GAP_ADTYPE_16BIT_MORELongDescription,
        default: false,
        onChange: onMore16bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (16-bit More)",
        name: "numOfUUIDs16More",
        default: 0,
        onChange: onNumOfUUIDs16MoreChange,
        hidden: true
    },
    {
        name: "UUID016More", displayName: "UUID 0 (16-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID116More", displayName: "UUID 1 (16-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID216More", displayName: "UUID 2 (16-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID316More", displayName: "UUID 3 (16-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID416More", displayName: "UUID 4 (16-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID516More", displayName: "UUID 5 (16-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID616More", displayName: "UUID 6 (16-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID716More", displayName: "UUID 7 (16-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID816More", displayName: "UUID 8 (16-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID916More", displayName: "UUID 9 (16-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        displayName: "Complete List of 16-bit UUIDs",
        name: "GAP_ADTYPE_16BIT_COMPLETE",
        longDescription: Docs.GAP_ADTYPE_16BIT_COMPLETELongDescription,
        default: false,
        onChange: onComplete16bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (16-bit Complete)",
        name: "numOfUUIDs16Complete",
        longDescription: Docs.advTypeLongDescription,
        default: 0,
        onChange: onNumOfUUIDs16CompleteChange,
        hidden: true
    },
    {
        name: "UUID016Complete", displayName: "UUID 0 (16-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID116Complete", displayName: "UUID 1 (16-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID216Complete", displayName: "UUID 2 (16-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID316Complete", displayName: "UUID 3 (16-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID416Complete", displayName: "UUID 4 (16-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID516Complete", displayName: "UUID 5 (16-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID616Complete", displayName: "UUID 6 (16-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID716Complete", displayName: "UUID 7 (16-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID816Complete", displayName: "UUID 8 (16-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID916Complete", displayName: "UUID 9 (16-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        displayName: "More 32-bit UUIDs Available",
        name: "GAP_ADTYPE_32BIT_MORE",
        longDescription: Docs.GAP_ADTYPE_32BIT_MORELongDescription,
        default: false,
        onChange: onMore32bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (32-bit More)",
        name: "numOfUUIDs32More",
        default: 0,
        onChange: onNumOfUUIDs32MoreChange,
        hidden: true
    },
    {
        name: "UUID032More", displayName: "UUID 0 (32-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID132More", displayName: "UUID 1 (32-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID232More", displayName: "UUID 2 (32-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID332More", displayName: "UUID 3 (32-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID432More", displayName: "UUID 4 (32-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID532More", displayName: "UUID 5 (32-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID632More", displayName: "UUID 6 (32-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID732More", displayName: "UUID 7 (32-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID832More", displayName: "UUID 8 (32-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID932More", displayName: "UUID 9 (32-bit More)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        displayName: "Complete List of 32-bit UUIDs",
        name: "GAP_ADTYPE_32BIT_COMPLETE",
        longDescription: Docs.GAP_ADTYPE_32BIT_COMPLETELongDescription,
        default: false,
        onChange: onComplete32bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (32-bit Complete)",
        name: "numOfUUIDs32Complete",
        default: 0,
        onChange: onNumOfUUIDs32CompleteChange,
        hidden: true
    },
    {
        name: "UUID032Complete", displayName: "UUID 0 (32-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID132Complete", displayName: "UUID 1 (32-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID232Complete", displayName: "UUID 2 (32-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID332Complete", displayName: "UUID 3 (32-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID432Complete", displayName: "UUID 4 (32-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID532Complete", displayName: "UUID 5 (32-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID632Complete", displayName: "UUID 6 (32-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID732Complete", displayName: "UUID 7 (32-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID832Complete", displayName: "UUID 8 (32-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID932Complete", displayName: "UUID 9 (32-bit Complete)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        displayName: "More 128-bit UUIDs Available",
        name: "GAP_ADTYPE_128BIT_MORE",
        longDescription: Docs.GAP_ADTYPE_128BIT_MORELongDescription,
        default: false,
        onChange: onMore128bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (128-bit More)",
        name: "numOfUUIDs128More",
        longDescription: Docs.advTypeLongDescription,
        default: 0,
        onChange: onNumOfUUIDs128MoreChange,
        hidden: true
    },
    {
        name: "UUID0128More", displayName: "UUID 0 (128-bit More)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID1128More", displayName: "UUID 1 (128-bit More)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID2128More", displayName: "UUID 2 (128-bit More)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID3128More", displayName: "UUID 3 (128-bit More)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID4128More", displayName: "UUID 4 (128-bit More)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID5128More", displayName: "UUID 5 (128-bit More)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID6128More", displayName: "UUID 6 (128-bit More)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID7128More", displayName: "UUID 7 (128-bit More)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID8128More", displayName: "UUID 8 (128-bit More)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID9128More", displayName: "UUID 9 (128-bit More)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        displayName: "Complete List of 128-bit UUIDs",
        name: "GAP_ADTYPE_128BIT_COMPLETE",
        longDescription: Docs.GAP_ADTYPE_128BIT_COMPLETELongDescription,
        default: false,
        onChange: onComplete128bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (128-bit Complete)",
        name: "numOfUUIDs128Complete",
        default: 0,
        onChange: onNumOfUUIDs128CompleteChange,
        hidden: true
    },
    {
        name: "UUID0128Complete", displayName: "UUID 0 (128-bit Complete)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID1128Complete", displayName: "UUID 1 (128-bit Complete)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID2128Complete", displayName: "UUID 2 (128-bit Complete)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID3128Complete", displayName: "UUID 3 (128-bit Complete)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID4128Complete", displayName: "UUID 4 (128-bit Complete)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID5128Complete", displayName: "UUID 5 (128-bit Complete)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID6128Complete", displayName: "UUID 6 (128-bit Complete)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID7128Complete", displayName: "UUID 7 (128-bit Complete)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID8128Complete", displayName: "UUID 8 (128-bit Complete)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID9128Complete", displayName: "UUID 9 (128-bit Complete)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        displayName: "TX Power Level",
        name: "GAP_ADTYPE_POWER_LEVEL",
        longDescription: Docs.GAP_ADTYPE_POWER_LEVELLongDescription,
        default: false,
        onChange: onTXPowerLevelChange
    },
    {
        name: "TXPower",
        displayName: "TX Power (dBm)",
        longDescription: Docs.TXPowerLongDescription,
        default: 0,
        hidden: true
    },
    {
        displayName: "Slave Connection Interval Range",
        name: "GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE",
        description: "2 octets Min, 2 octets Max (0xFFFF indicates no conn interval min or max)",
        longDescription: Docs.GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGELongDescription,
        default: false,
        onChange: onSlaveConnIntRangeChange
    },
    {
        name: "minConnInterval",
        displayName: "Min Connection Interval (ms)",
        longDescription: Docs.minConnIntervalLongDescription,
        default: 100,
        hidden: true
    },
    {
        name: "maxConnInterval",
        displayName: "Max Connection Interval (ms)",
        longDescription: Docs.maxConnIntervalLongDescription,
        default: 200,
        hidden: true
    },
    {
        displayName: "Service Solicitation: 16-bit UUIDs",
        name: "GAP_ADTYPE_SERVICES_LIST_16BIT",
        longDescription: Docs.GAP_ADTYPE_SERVICES_LIST_16BITLongDescription,
        default: false,
        onChange: onSS16bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (16-bit)",
        name: "numOfUUIDs16SS",
        default: 0,
        onChange: onNumOfUUIDs16SSChange,
        hidden: true
    },
    {
        name: "UUID016SS", displayName: "UUID 0 (SS 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID116SS", displayName: "UUID 1 (SS 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID216SS", displayName: "UUID 2 (SS 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID316SS", displayName: "UUID 3 (SS 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID416SS", displayName: "UUID 4 (SS 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID516SS", displayName: "UUID 5 (SS 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID616SS", displayName: "UUID 6 (SS 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID716SS", displayName: "UUID 7 (SS 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID816SS", displayName: "UUID 8 (SS 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID916SS", displayName: "UUID 9 (SS 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        displayName: "Service Solicitation: 128-bit UUIDs",
        name: "GAP_ADTYPE_SERVICES_LIST_128BIT",
        longDescription: Docs.GAP_ADTYPE_SERVICES_LIST_128BITLongDescription,
        default: false,
        onChange: onSS128bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (128-bit)",
        name: "numOfUUIDs128SS",
        default: 0,
        onChange: onNumOfUUIDs128SSChange,
        hidden: true
    },
    {
        name: "UUID0128SS", displayName: "UUID 0 (SS 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID1128SS", displayName: "UUID 1 (SS 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID2128SS", displayName: "UUID 2 (SS 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID3128SS", displayName: "UUID 3 (SS 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID4128SS", displayName: "UUID 4 (SS 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID5128SS", displayName: "UUID 5 (SS 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID6128SS", displayName: "UUID 6 (SS 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID7128SS", displayName: "UUID 7 (SS 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID8128SS", displayName: "UUID 8 (SS 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID9128SS", displayName: "UUID 9 (SS 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        displayName: "Service Data - 16-bit UUID",
        name: "GAP_ADTYPE_SERVICE_DATA",
        longDescription: Docs.GAP_ADTYPE_SERVICE_DATALongDescription,
        default: false,
        onChange: onSD16bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (Service Data 16-bit)",
        name: "numOfUUIDs16SD",
        default: 0,
        onChange: onNumOfUUIDs16SDChange,
        hidden: true
    },
    {
        name: "UUID016SD", displayName: "UUID 0 (Service Data - 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID016SDData", displayName: "Data 0 (Service Data - 16-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID116SD", displayName: "UUID 1 (Service Data - 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID116SDData", displayName: "Data 1 (Service Data - 16-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID216SD", displayName: "UUID 2 (Service Data - 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID216SDData", displayName: "Data 2 (Service Data - 16-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID316SD", displayName: "UUID 3 (Service Data - 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID316SDData", displayName: "Data 3 (Service Data - 16-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID416SD", displayName: "UUID 4 (Service Data - 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID416SDData", displayName: "Data 4 (Service Data - 16-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID516SD", displayName: "UUID 5 (Service Data - 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID516SDData", displayName: "Data 5 (Service Data - 16-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID616SD", displayName: "UUID 6 (Service Data - 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID616SDData", displayName: "Data 6 (Service Data - 16-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID716SD", displayName: "UUID 7 (Service Data - 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID716SDData", displayName: "Data 7 (Service Data - 16-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID816SD", displayName: "UUID 8 (Service Data - 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID816SDData", displayName: "Data 8 (Service Data - 16-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID916SD", displayName: "UUID 9 (Service Data - 16-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID916SDData", displayName: "Data 9 (Service Data - 16-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        displayName: "Service Data - 32-bit UUID",
        name: "GAP_ADTYPE_SERVICE_DATA_32BIT",
        longDescription: Docs.GAP_ADTYPE_SERVICE_DATA_32BITLongDescription,
        default: false,
        onChange: onSD32bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (Service Data 32-bit)",
        name: "numOfUUIDs32SD",
        default: 0,
        onChange: onNumOfUUIDs32SDChange,
        hidden: true
    },
    {
        name: "UUID032SD", displayName: "UUID 0 (Service Data - 32-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID032SDData", displayName: "Data 0 (Service Data - 32-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID132SD", displayName: "UUID 1 (Service Data - 32-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID132SDData", displayName: "Data 1 (Service Data - 32-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID232SD", displayName: "UUID 2 (Service Data - 32-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID232SDData", displayName: "Data 2 (Service Data - 32-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID332SD", displayName: "UUID 3 (Service Data - 32-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID332SDData", displayName: "Data 3 (Service Data - 32-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID432SD", displayName: "UUID 4 (Service Data - 32-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID432SDData", displayName: "Data 4 (Service Data - 32-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID532SD", displayName: "UUID 5 (Service Data - 32-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID532SDData", displayName: "Data 5 (Service Data - 32-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID632SD", displayName: "UUID 6 (Service Data - 32-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID632SDData", displayName: "Data 6 (Service Data - 32-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID732SD", displayName: "UUID 7 (Service Data - 32-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID732SDData", displayName: "Data 7 (Service Data - 32-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID832SD", displayName: "UUID 8 (Service Data - 32-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID832SDData", displayName: "Data 8 (Service Data - 32-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID932SD", displayName: "UUID 9 (Service Data - 32-bit)",
        displayFormat: "hex", default: 0x0, hidden: true
    },
    {
        name: "UUID932SDData", displayName: "Data 9 (Service Data - 32-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        displayName: "Service Data - 128-bit UUID",
        name: "GAP_ADTYPE_SERVICE_DATA_128BIT",
        longDescription: Docs.GAP_ADTYPE_SERVICE_DATA_128BITLongDescription,
        default: false,
        onChange: onSD128bitUUIDsChange
    },
    {
        displayName: "Num of UUIDs (Service Data 128-bit)",
        name: "numOfUUIDs128SD",
        default: 0,
        onChange: onNumOfUUIDs128SDChange,
        hidden: true
    },
    {
        name: "UUID0128SD", displayName: "UUID 0 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID0128SDData", displayName: "Data 0 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID1128SD", displayName: "UUID 1 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID1128SDData", displayName: "Data 1 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID2128SD", displayName: "UUID 2 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID2128SDData", displayName: "Data 2 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID3128SD", displayName: "UUID 3 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID3128SDData", displayName: "Data 3 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID4128SD", displayName: "UUID 4 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID4128SDData", displayName: "Data 4 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID5128SD", displayName: "UUID 5 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID5128SDData", displayName: "Data 5 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID6128SD", displayName: "UUID 6 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID6128SDData", displayName: "Data 6 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID7128SD", displayName: "UUID 7 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID7128SDData", displayName: "Data 7 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID8128SD", displayName: "UUID 8 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID8128SDData", displayName: "Data 8 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID9128SD", displayName: "UUID 9 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        name: "UUID9128SDData", displayName: "Data 9 (Service Data - 128-bit)",
        displayFormat: "hex", default: system.utils.bigInt("00",16), hidden: true
    },
    {
        displayName: "Public Target Address",
        name: "GAP_ADTYPE_PUBLIC_TARGET_ADDR",
        longDescription: Docs.GAP_ADTYPE_PUBLIC_TARGET_ADDRLongDescription,
        default: false,
        onChange: onPublicTargetAddrChange
    },
    {
        displayName: "Num Of Public Addresses",
        name: "numOfPublicAddresses",
        default: 0,
        onChange: onNumOfPublicAddressesChange,
        hidden: true
    },
    {
        name: "publicAddress0", displayName: "Public Address 0",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "publicAddress1", displayName: "Public Address 1",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "publicAddress2", displayName: "Public Address 2",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "publicAddress3", displayName: "Public Address 3",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "publicAddress4", displayName: "Public Address 4",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "publicAddress5", displayName: "Public Address 5",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "publicAddress6", displayName: "Public Address 6",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "publicAddress7", displayName: "Public Address 7",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "publicAddress8", displayName: "Public Address 8",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "publicAddress9", displayName: "Public Address 9",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        displayName: "Random Target Address",
        name: "GAP_ADTYPE_RANDOM_TARGET_ADDR",
        longDescription: Docs.GAP_ADTYPE_RANDOM_TARGET_ADDRLongDescription,
        default: false,
        onChange: onRandomTargetAddrChange
    },
    {
        displayName: "Num Of Random Addresses",
        name: "numOfRandomAddresses",
        longDescription: Docs.advTypeLongDescription,
        default: 0,
        onChange: onNumOfRandomAddressesChange,
        hidden: true
    },
    {
        name: "randomAddress0", displayName: "Random Address 0",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "randomAddress1", displayName: "Random Address 1",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "randomAddress2", displayName: "Random Address 2",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "randomAddress3", displayName: "Random Address 3",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "randomAddress4", displayName: "Random Address 4",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "randomAddress5", displayName: "Random Address 5",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "randomAddress6", displayName: "Random Address 6",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "randomAddress7", displayName: "Random Address 7",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "randomAddress8", displayName: "Random Address 8",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        name: "randomAddress9", displayName: "Random Address 9",
        hidden: true, default: "00:00:00:00:00:00",
		placeholder: "00:00:00:00:00:00", textType: "mac_address_48"
    },
    {
        displayName: "Appearance",
        name: "GAP_ADTYPE_APPEARANCE",
        longDescription: Docs.GAP_ADTYPE_APPEARANCELongDescription,
        default: false,
        onChange: onAppearanceChange
    },
    {
        name: "appearanceValue",
        displayName: "Appearance Value",
        displayFormat: "hex",
        longDescription: Docs.appearanceValueLongDescription,
        default: 0x0,
        hidden: true
    },
    {
        displayName: "Advertising Interval",
        name: "GAP_ADTYPE_ADV_INTERVAL",
        description:"The advInterval shall be an integer multiple of 0.625 ms in the range of 20 ms to 10,485.759375 s",
        longDescription: Docs.GAP_ADTYPE_ADV_INTERVALLongDescription,
        default: false,
        onChange: onAdvIntChange
    },
    {
        name: "advIntValue",
        displayName: "Advertising Interval Value (ms)",
        longDescription: Docs.advIntValueLongDescription,
        default: 20,
        hidden: true
    },
    {
        displayName: "Manufacturer Specific Data",
        name:"GAP_ADTYPE_MANUFACTURER_SPECIFIC",
        description: "First 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data",
        longDescription: Docs.GAP_ADTYPE_MANUFACTURER_SPECIFICLongDescription,
        default: false,
        onChange: onManuSpecDataChange
    },
    {
        name: "companyIdentifier",
        displayName: "Company Identifier",
        displayFormat: "hex",
        longDescription: Docs.companyIdentifierLongDescription,
        displayFormat: {
            bitSize: 16,
            radix: "hex",
        },
        default: 0x0D00,
        hidden: true
    },
    {
        name: "additionalData",
        displayName: "Additional Data",
        displayFormat: "hex",
        default: system.utils.bigInt("00",16),
        hidden: true
    }
];

/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - Advertisement Data instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    //Validate the name
    if(inst.name.includes(" "))
    {
        validation.logError("The name should not contain spaces", inst, "name");
    }

    // Validate Tx Power value
    if(inst.TXPower < Common.advParamsRanges.advDataTXPowerMinValue ||
         inst.TXPower > Common.advParamsRanges.advDataTXPowerMaxValue)
    {
        validation.logError("TX Power Range is " + Common.advParamsRanges.advDataTXPowerMinValue
         + " to " + Common.advParamsRanges.advDataTXPowerMaxValue + " dBm", inst, "TXPower");
    }

    // Validate advertising interval value
    if(inst.advIntValue < Common.advParamsRanges.priAdvIntMinValue ||
         inst.advIntValue > Common.advParamsRanges.priAdvIntMaxValueAdvData)
    {
        validation.logError("Advertising Interval is " + Common.advParamsRanges.priAdvIntMinValue + " to " +
                             Common.advParamsRanges.priAdvIntMaxValueAdvData  + " ms", inst, "advIntValue");
    }

    // Validate connection interval values
    Common.validateConnInterval(inst,validation,inst.minConnInterval,"minConnInterval",inst.maxConnInterval,"maxConnInterval");

    // Make sure the only 16BIT_MORE or 16BIT_COMPLETE is used
    if(inst.GAP_ADTYPE_16BIT_MORE == true && inst.GAP_ADTYPE_16BIT_COMPLETE == true)
    {
        validation.logError("Data packet can contain only one instance of 16-bit UUIDs ",
                             inst, ["GAP_ADTYPE_16BIT_MORE", "GAP_ADTYPE_16BIT_COMPLETE"])
    }
    // Make sure the only 32BIT_MORE or 32BIT_COMPLETE is used
    if(inst.GAP_ADTYPE_32BIT_MORE == true && inst.GAP_ADTYPE_32BIT_COMPLETE == true)
    {
        validation.logError("Data packet can contain only one instance of 32-bit UUIDs ",
                             inst, ["GAP_ADTYPE_32BIT_MORE", "GAP_ADTYPE_32BIT_COMPLETE"])
    }
    // Make sure the only 128BIT_MORE or 128BIT_COMPLETE is used
    if(inst.GAP_ADTYPE_128BIT_MORE == true && inst.GAP_ADTYPE_128BIT_COMPLETE == true)
    {
        validation.logError("Data packet can contain only one instance of 128-bit UUIDs ",
                             inst, ["GAP_ADTYPE_128BIT_MORE", "GAP_ADTYPE_128BIT_COMPLETE"])
    }

    // Validate number of UUIDs and UUIDs length
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs16More","16More",4);
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs16Complete","16Complete",4);
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs32More","32More",8);
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs32Complete","32Complete",8);
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs128More","128More",32);
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs128Complete","128Complete",32);
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs16SS","16SS",4);
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs128SS","128SS",32);
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs16SD","16SD",4);
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs32SD","32SD",8);
    Common.validateNumOfUUIDs(inst,validation,"numOfUUIDs128SD","128SD",32);

    // Validate Number Of Public Addresses
    if(inst.numOfPublicAddresses < 0 || inst.numOfPublicAddresses > 10)
    {
        validation.logError("Please enter a number between 0 - 10", inst, "numOfPublicAddresses");
    }

    // Validate Number Of Random Addresses
    if(inst.numOfRandomAddresses < 0 || inst.numOfRandomAddresses > 10)
    {
        validation.logError("Please enter a number between 0 - 10", inst, "numOfRandomAddresses");
    }

    // Validate appearance Value length
    if(inst.appearanceValue.toString(16).length > 4)
    {
        validation.logError("The number of bytes is 2" ,inst, "appearanceValue");
    }

    // Validate Manufacturer Specific Data company identifier length
    if(inst.companyIdentifier.toString(16).length > 4)
    {
        validation.logError("The number of bytes is 2" ,inst, "companyIdentifier");
    }

    // Validate data length
    let maxLength;
    if (inst.$ownedBy["advParam"+inst.$ownedBy.numOfAdvSet].advType == "legacy")
    {
        maxLength = Common.maxLegacyDataLen;
    }
    else // !Legacy
    {
        let connectable = inst.$ownedBy["advParam"+inst.$ownedBy.numOfAdvSet].eventProps.includes("GAP_ADV_PROP_CONNECTABLE");
        let scannable = inst.$ownedBy["advParam"+inst.$ownedBy.numOfAdvSet].eventProps.includes("GAP_ADV_PROP_SCANNABLE");
        // connectable & AdvData or scannable & ScanRspData
        if ((connectable && !inst.hideAdvFlags) || (scannable  && inst.hideAdvFlags))
        {

            maxLength = Common.maxExtConnDataLen - Common.minExtHdrLen - Common.advAHdrLen;
            // Adds Target Address Header
            if (inst.$ownedBy["advParam"+inst.$ownedBy.numOfAdvSet].eventProps.includes("GAP_ADV_PROP_DIRECTED"))
            {
                maxLength -= Common.targetAHdrLen;
            }
            // Adds TX Power Header
            if (inst.$ownedBy["advParam"+inst.$ownedBy.numOfAdvSet].eventProps.includes("GAP_ADV_PROP_TX_POWER"))
            {
                maxLength -= Common.txPowerHdrLen;
            }
        }
        // Scan Rsp Data & Connectable or Adv Data & Scannable
        else if ((connectable && inst.hideAdvFlags) || (scannable  && !inst.hideAdvFlags))
        {
            maxLength = 0;
        }
        else
        {
            maxLength = Common.maxExtDataLen;
        }
    }
    if(Common.advDataTotalLength(inst, config) > maxLength)
    {
        // Adv Data or Scan RSP data is not allowed
        if (maxLength <= 0)
        {
            inst.hideAdvFlags? validation.logError("Scan Rsp Data is not allowed in this advertising mode." ,inst)
                               :validation.logError("Adv Data is not allowed in this advertising mode." ,inst);
        }
        // else, data length exceeded max length
        else
        {
            validation.logError("The current data length exceeds the allowed max length (" + maxLength + ")" ,inst);
        }
    }

    // Validate Complete Local Name
    if(inst.GAP_ADTYPE_LOCAL_NAME_COMPLETE)
    {
        if(inst.completeLocalName != inst.$ownedBy.$ownedBy.deviceName)
        {
            validation.logError("Complete Local Name should be equal to the Device Name "+
                                "configured in \"General Configuration\"",inst,"completeLocalName");
        }
    }
}

function onGapAdveFlagsChange(inst,ui){
    ui.advertisingFlags.hidden = !inst.GAP_ADTYPE_FLAGS;
}

function onHideAdvFlagsChange(inst,ui){
    let reason = "In Scan Response Data this data type should not be used";
    inst.hideAdvFlags ? ui.GAP_ADTYPE_FLAGS.readOnly = reason : ui.GAP_ADTYPE_FLAGS.hidden = false;
}

function onShortenedLocalNameChange(inst,ui){
    ui.shortenedLocalName.hidden = !inst.GAP_ADTYPE_LOCAL_NAME_SHORT;
}

function onCompleteLocalNameChange(inst,ui){
    ui.completeLocalName.hidden = !inst.GAP_ADTYPE_LOCAL_NAME_COMPLETE;
}

function onTXPowerLevelChange(inst,ui){
    ui.TXPower.hidden = !inst.GAP_ADTYPE_POWER_LEVEL;
}

function onSlaveConnIntRangeChange(inst,ui){
    ui.minConnInterval.hidden = !inst.GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE;
    ui.maxConnInterval.hidden = !inst.GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE;
}

function onPublicTargetAddrChange(inst, instUIState){
    instUIState["numOfPublicAddresses"].hidden = !inst.GAP_ADTYPE_PUBLIC_TARGET_ADDR;
    if(inst.GAP_ADTYPE_PUBLIC_TARGET_ADDR == false){
        hideAddress(instUIState, "public", 10);
    }
    else{
        changeAddressState(inst.numOfPublicAddresses,instUIState,"public", 10);
    }
}

function onNumOfPublicAddressesChange(inst,instUIState){
    changeAddressState(inst.numOfPublicAddresses,instUIState,"public", 10);
}

function onRandomTargetAddrChange(inst, instUIState){
    instUIState["numOfRandomAddresses"].hidden = !inst.GAP_ADTYPE_RANDOM_TARGET_ADDR;
    if(inst.GAP_ADTYPE_RANDOM_TARGET_ADDR == false){
        hideAddress(instUIState, "random", 10);
    }
    else{
        changeAddressState(inst.numOfRandomAddresses,instUIState,"random", 10);
    }
}

function onNumOfRandomAddressesChange(inst,instUIState){
    changeAddressState(inst.numOfRandomAddresses,instUIState,"random", 10);
}

function onAppearanceChange(inst, ui){
    ui.appearanceValue.hidden = !inst.GAP_ADTYPE_APPEARANCE;
}
function onAdvIntChange(inst,ui){
    ui.advIntValue.hidden = !inst.GAP_ADTYPE_ADV_INTERVAL;
}

function onManuSpecDataChange(inst,instUIState){
    instUIState["companyIdentifier"].hidden = !inst.GAP_ADTYPE_MANUFACTURER_SPECIFIC;
    instUIState["additionalData"].hidden = !inst.GAP_ADTYPE_MANUFACTURER_SPECIFIC;
}

/************************** 16-bit UUID **************************/

function onMore16bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs16More"].hidden = !inst.GAP_ADTYPE_16BIT_MORE;
    if(inst.GAP_ADTYPE_16BIT_MORE == false){
        hideUUIDs(instUIState, "16More", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs16More,instUIState,"16More", 10);
    }
}

function onComplete16bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs16Complete"].hidden = !inst.GAP_ADTYPE_16BIT_COMPLETE;
    if(inst.GAP_ADTYPE_16BIT_COMPLETE == false){
        hideUUIDs(instUIState, "16Complete", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs16Complete,instUIState,"16Complete", 10);
    }
}

function onNumOfUUIDs16MoreChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs16More,instUIState,"16More", 10);
}

function onNumOfUUIDs16CompleteChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs16Complete,instUIState,"16Complete", 10);
}
/*****************************************************************/

/************************** 32-bit UUID **************************/

function onMore32bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs32More"].hidden = !inst.GAP_ADTYPE_32BIT_MORE;
    if(inst.GAP_ADTYPE_32BIT_MORE == false){
        hideUUIDs(instUIState, "32More", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs32More,instUIState,"32More", 10);
    }
}

function onComplete32bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs32Complete"].hidden = !inst.GAP_ADTYPE_32BIT_COMPLETE;
    if(inst.GAP_ADTYPE_32BIT_COMPLETE == false){
        hideUUIDs(instUIState, "32Complete", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs32Complete,instUIState,"32Complete", 10);
    }
}

function onNumOfUUIDs32MoreChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs32More,instUIState,"32More", 10);
}

function onNumOfUUIDs32CompleteChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs32Complete,instUIState,"32Complete", 10);
}
/*****************************************************************/

/************************** 128-bit UUID **************************/

function onMore128bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs128More"].hidden = !inst.GAP_ADTYPE_128BIT_MORE;
    if(inst.GAP_ADTYPE_128BIT_MORE == false){
        hideUUIDs(instUIState, "128More", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs128More,instUIState,"128More", 10);
    }
}

function onComplete128bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs128Complete"].hidden = !inst.GAP_ADTYPE_128BIT_COMPLETE;
    if(inst.GAP_ADTYPE_128BIT_COMPLETE == false){
        hideUUIDs(instUIState, "128Complete", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs128Complete,instUIState,"128Complete", 10);
    }
}

function onNumOfUUIDs128MoreChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs128More,instUIState,"128More", 10);
}

function onNumOfUUIDs128CompleteChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs128Complete,instUIState,"128Complete", 10);
}
/******************************************************************/

/********************** Service Solicitation **********************/

function onSS16bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs16SS"].hidden = !inst.GAP_ADTYPE_SERVICES_LIST_16BIT;
    if(inst.GAP_ADTYPE_SERVICES_LIST_16BIT == false){
        hideUUIDs(instUIState, "16SS", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs16SS,instUIState,"16SS", 10);
    }
}

function onSS128bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs128SS"].hidden = !inst.GAP_ADTYPE_SERVICES_LIST_128BIT;
    if(inst.GAP_ADTYPE_SERVICES_LIST_128BIT == false){
        hideUUIDs(instUIState, "128SS", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs128SS,instUIState,"128SS", 10);
    }
}

function onNumOfUUIDs16SSChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs16SS,instUIState,"16SS", 10);
}

function onNumOfUUIDs128SSChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs128SS,instUIState,"128SS", 10);
}
/******************************************************************/

/************************** Service Data **************************/

function onSD16bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs16SD"].hidden = !inst.GAP_ADTYPE_SERVICE_DATA;
    if(inst.GAP_ADTYPE_SERVICE_DATA == false){
        hideUUIDs(instUIState, "16SD", 10);
        hideUUIDs(instUIState, "16SDData", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs16SD,instUIState,"16SD", 10);
        changeUUIDState(inst.numOfUUIDs16SD,instUIState,"16SDData", 10);
    }
}

function onSD32bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs32SD"].hidden = !inst.GAP_ADTYPE_SERVICE_DATA_32BIT;
    if(inst.GAP_ADTYPE_SERVICE_DATA_32BIT == false){
        hideUUIDs(instUIState, "32SD", 10);
        hideUUIDs(instUIState, "32SDData", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs32SD,instUIState,"32SD", 10);
        changeUUIDState(inst.numOfUUIDs32SD,instUIState,"32SDData", 10);
    }
}

function onSD128bitUUIDsChange(inst,instUIState){
    instUIState["numOfUUIDs128SD"].hidden = !inst.GAP_ADTYPE_SERVICE_DATA_128BIT;
    if(inst.GAP_ADTYPE_SERVICE_DATA_128BIT == false){
        hideUUIDs(instUIState, "128SD", 10);
        hideUUIDs(instUIState, "128SDData", 10);
    }
    else{
        changeUUIDState(inst.numOfUUIDs128SD,instUIState,"128SD", 10);
        changeUUIDState(inst.numOfUUIDs128SD,instUIState,"128SDData", 10);
    }
}

function onNumOfUUIDs16SDChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs16SD,instUIState,"16SD", 10);
    changeUUIDState(inst.numOfUUIDs16SD,instUIState,"16SDData", 10);
}

function onNumOfUUIDs32SDChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs32SD,instUIState,"32SD", 10);
    changeUUIDState(inst.numOfUUIDs32SD,instUIState,"32SDData", 10);
}

function onNumOfUUIDs128SDChange(inst,instUIState)
{
    changeUUIDState(inst.numOfUUIDs128SD,instUIState,"128SD", 10);
    changeUUIDState(inst.numOfUUIDs128SD,instUIState,"128SDData", 10);
}
/******************************************************************/

/*
 * ======== changeUUIDState ========
 * Changes the hidden state of a UUID instance
 *
 * @param selectedNum - Advertisement Data instance to be validated
 * @param instUIState - object to hold detected validation issues
 * @param type
 * @param maxNum
 */
function changeUUIDState(selectedNum,instUIState,type,maxNum)
{
    let i = 0;
    if(selectedNum <= maxNum)
    {
        for(i = 0; i < selectedNum; i++)
        {
            instUIState["UUID"+i+type].hidden = false;
        }
        for(; i < maxNum; i++)
        {
            instUIState["UUID"+i+type].hidden = true;
        }
    }
}

/*
 * ======== hideUUIDs ========
 * Changes the hidden state of a UUID instance to true
 *
 * @param instUIState - object to hold detected validation issues
 * @param type
 * @param maxNum
 */
function hideUUIDs(instUIState,type,maxNum)
{
    for(let i = 0; i < maxNum; i++)
    {
        instUIState["UUID"+i+type].hidden = true;
    }
}

/*
 * ======== changeAddressState ========
 * Changes the hidden state of an Address instance
 *
 * @param selectedNum - Advertisement Data instance to be validated
 * @param instUIState - object to hold detected validation issues
 * @param type
 * @param maxNum
 */
function changeAddressState(selectedNum,instUIState,type,maxNum)
{
    let i = 0;
    if(selectedNum <= maxNum)
    {
        for(i = 0; i < selectedNum; i++)
        {
            instUIState[type+"Address"+i].hidden = false;
        }
        for(; i < maxNum; i++)
        {
            instUIState[type+"Address"+i].hidden = true;
        }
    }
}

/*
 * ======== hideAddress ========
 * Changes the hidden state of an Address instance to true
 *
 * @param instUIState - object to hold detected validation issues
 * @param type
 * @param maxNum
 */
function hideAddress(instUIState,type,maxNum)
{
    for(let i = 0; i < maxNum; i++)
    {
        instUIState[type+"Address"+i].hidden = true;
    }
}

/*
 * ======== getDeviceNameValue ========
 * Returns the value of the ble module deviceName configurable
 *
 * @param inst  - Module instance containing the config that changed
 */
function getDeviceNameValue(inst)
{
    return system.modules["/ti/ble5stack/ble"].$static.deviceName;
}

/*
 *  ======== advDataModule ========
 *  Define the BLE Advertisement Data Configuration module properties and methods
 */
const advDataModule = {
    displayName: "Advertisement Data",
    config: config,
    validate: validate
};

/*
 *  ======== exports ========
 *  Export the Advertisement Data module
 */
exports = advDataModule;