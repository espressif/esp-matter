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
 *  ======== ble_common.js ========
 */
"use strict";


const sigModelMapping = {
  //"sig_model": {
    "health_srv": {
      "Name":"Health Server",
      "isSupported" : true,
      "sig_id": "0x0002",
      "Group": "Foundation",
      "healthSrvCbk": "health_cb",
      "maxFaults": 16,
      "Element": "Main",
      "messages": {
        "num_of_msg": 0
      }
    },
    "health_cli": {
      "Name":"Health Client",
      "isSupported" : true,
      "sig_id": "0x0003",
      "Group": "Foundation",
      "publication_size": 0,
      "Element": "Main",
      "messages": {
        "num_of_msg": 0
      }
    },
    "gen_onoff_srv": {
      "Name":"Generic OnOff Server",
      "isSupported" : true,
      "sig_id": "0x1000",
      "Group": "Generic",
      "modelCbStructName": "generic_OnOff_srv_cb",
      "userDataStructType": "ctl_state_t",
      "defaultUserData": "generic_state",
      "publication_size": 2 + 3,
      "Element": "Main",
      "messages": {
        "num_of_msg": 3,
        "msgName1": "gen_OnOff_get",
        "min_len_msg1":0,
        "opcode1": "0x82, 0x01",
        "msgName2": "gen_OnOff_set",
        "min_len_msg2":0,
        "opcode2": "0x82, 0x02",
        "msgName3": "gen_OnOff_set_unack",
        "min_len_msg3":0,
        "opcode3": "0x82, 0x03",
      }
    },
    "gen_onoff_cli": {
      "Name":"Generic OnOff Client",
      "isSupported" : true,
      "sig_id": "0x1001",
      "Group": "Generic",
      "modelCbStructName": "",
      "userDataStructType": "",
      "defaultUserData": "",
      "publication_size": 2 + 4,
      "Element": "Main",
      "messages": {
        "num_of_msg": 1,
        "msgName1": "gen_OnOff_Status",
       "min_len_msg1":0,
        "opcode1": "0x82, 0x04"
      }
    },
    "gen_level_srv": {
      "Name":"Generic Level Server",
      "isSupported" : true,
      "sig_id": "0x1002",
      "Group": "Generic",
      "modelCbStructName": "",
      "userDataStructType": "",
      "defaultUserData": "",
      "publication_size": 2 + 5,
      "Element": "Main",
      "messages": {
        "num_of_msg": 7,
        "msgName1": "gen_Level_get",
       "min_len_msg1":0,
        "opcode1": "0x82, 0x05",
        "msgName2": "gen_Level_set",
       "min_len_msg2":0,
        "opcode2": "0x82, 0x06",
        "msgName3": "gen_Level_set_unack",
       "min_len_msg3":0,
        "opcode3": "0x82, 0x07",
        "msgName4": "gen_Delta_set",
       "min_len_msg4":0,
        "opcode4": "0x82, 0x09",
        "msgName5": "gen_Delta_set_unack",
       "min_len_msg5":0,
        "opcode5": "0x82, 0x0A",
        "msgName6": "gen_Move_set",
       "min_len_msg6":0,
        "opcode6": "0x82, 0x0B",
        "msgName7": "gen_Move_set_unack",
       "min_len_msg7":0,
        "opcode7": "0x82, 0x0C"
      }
    },
    "gen_level_cli": {
      "Name":"Generic Level Client",
      "isSupported" : true,
      "sig_id": "0x1003",
      "Group": "Generic",
      "modelCbStructName": "",
      "userDataStructType": "",
      "defaultUserData": "",
      "publication_size": 2 + 7,
      "Element": "Main",
      "messages": {
        "num_of_msg": 1,
        "msgName1": "gen_Level_Status",
       "min_len_msg1":0,
        "opcode1": "0x82, 0x08",
      }
    },
    "gen_def_trans_time_srv": {
      "Name":"Generic Default Transition Time Server",
      "isSupported" : true,
      "sig_id": "0x1004",
      "Group": "Generic",
      "modelCbStructName": "generic_defualt_transition_time_srv_cb",
      "userDataStructType": "ctl_state_t",
      "defaultUserData": "generic_state",
      "publication_size": 2 + 1,
      "Element": "Main",
      "messages": {
        "num_of_msg": 3,
        "msgName1": "gen_Default_Transition_Time_get",
       "min_len_msg1":0,
        "opcode1": "0x82, 0x0D",
        "msgName2": "gen_Default_Transition_Time_set",
       "min_len_msg2":0,
        "opcode2": "0x82, 0x0E",
        "msgName3": "gen_Default_Transition_Time_set_unack",
       "min_len_msg3":0,
        "opcode3": "0x82, 0x0F",
      }
    },
    "gen_def_trans_time_cli": {
      "Name":"Generic Default Transition Time Client",
      "isSupported" : true,
      "sig_id": "0x1005",
      "Group": "Generic",
      "modelCbStructName": "",
      "userDataStructType": "",
      "defaultUserData": "",
      "publication_size": 2 + 1,
      "Element": "Main",
      "messages": {
        "num_of_msg": 1,
        "msgName1": "gen_Default_Transition_Time_Status",
       "min_len_msg1":0,
        "opcode1": "0x82, 0x10"
      }
    },
    "gen_power_onoff_srv": {
      "Name":"Generic Power OnOff Server",
      "isSupported" : true,
      "sig_id": "0x1006",
      "Group": "Generic",
      "modelCbStructName": "",
      "userDataStructType": "genOnPowerUpState_t",
      "defaultUserData": "ponoff_state",
      "publication_size": 2 + 1,
      "Element": "Main",
      "messages": {
        "num_of_msg": 1,
        "msgName1": "gen_OnPowerUp_get",
       "min_len_msg1":0,
        "opcode1": "0x82, 0x11"
      }
    },
    "gen_power_onoff_cli": {
      "Name":"Generic Power OnOff Client",
      "isSupported" : true,
      "sig_id": "0x1007",
      "Group": "Generic",
      "modelCbStructName": "",
      "userDataStructType": "",
      "defaultUserData": "",
      "publication_size": 2 + 1,
      "Element": "Main",
      "messages": {
        "num_of_msg": 1,
        "msgName1": "gen_OnPowerUp_Status",
       "min_len_msg1":0,
        "opcode1": "0x82, 0x12"
      }
    },
    "gen_power_onoff_setup_srv": {
      "Name":"Generic Power OnOff Setup Server",
      "isSupported" : true,
      "sig_id": "0x1008",
      "Group": "Generic",
      "modelCbStructName": "generic_power_onoff_srv_cb",
      "userDataStructType": "genOnPowerUpState_t",
      "defaultUserData": "ponoff_state",
      "publication_size": 2 + 1,
      "Element": "Main",
      "messages": {
        "num_of_msg": 2,
        "msgName1": "gen_OnPowerUp_set",
       "min_len_msg1":0,
        "opcode1": "0x82, 0x13",
        "msgName2": "gen_OnPowerUp_set_unack",
       "min_len_msg2":0,
        "opcode2": "0x82, 0x14"
      }
    },
    "gen_battery_srv": {
      "Name":"Generic Battery Server",
      "isSupported" : true,
      "sig_id": "0x100C",
      "Group": "Generic",
      "modelCbStructName": "",
      "userDataStructType": "batteryStatus_t",
      "defaultUserData": "battery_server",
      "publication_size": 2 + 1,
      "Element": "Main",
      "messages": {
        "num_of_msg": 1,
        "msgName1": "gen_Battery_get",
        "min_len_msg1":0,
        "opcode1": "0x82, 0x23"
      }
    },
    "gen_battery_cli": {
      "Name":"Generic Battery Client",
      "isSupported" : true,
      "sig_id": "0x100D",
      "Group": "Generic",
      "modelCbStructName": "",
      "userDataStructType": "",
      "defaultUserData": "",
      "publication_size": 2 + 1,
      "Element": "Main",
      "messages": {
        "num_of_msg": 1,
        "msgName1": "gen_Battery_status",
        "min_len_msg1":0,
        "opcode1": "0x82, 0x24"
      }
    },
    "sensor_srv": {
      "Name":"Sensor Server",
      "isSupported" : true,
      "sig_id": "0x1100",
      "Group": "Sensor",
      "modelCbStructName": "",
      "userDataStructType": "",
      "defaultUserData": "",
      "publication_size": 2 + 5,
      "Element": "Main",
      "messages": {
        "num_of_msg": 4,
        "msgName1": "sensor_desc_get",
        "min_len_msg1":0,
        "opcode1": "0x82, 0x30",
		"msgName2": "sensor_get",
        "min_len_msg2":0,
        "opcode2": "0x82, 0x31",
		"msgName3": "sensor_Column_get",
        "min_len_msg3":0,
        "opcode3": "0x82, 0x32",
		"msgName4": "sensor_Series_get",
        "min_len_msg4":0,
        "opcode4": "0x82, 0x33",
      }
    },
    "sensor_cli": {
      "Name":"Sensor Client",
      "isSupported" : true,
      "sig_id": "0x1102",
      "Group": "Sensor",
      "modelCbStructName": "",
      "userDataStructType": "",
      "defaultUserData": "",
      "publication_size": 2 + 5,
      "Element": "Main",
      "messages": {
        "num_of_msg": 4,
        "msgName1": "sensor_desc_Status",
        "min_len_msg1":0,
        "opcode1": "0x51",
        "msgName2": "sensor_status",
        "min_len_msg2":0,
        "opcode2": "0x52",
        "msgName3": "sensor_Column_Status",
        "min_len_msg3":0,
        "opcode3": "0x53",
        "msgName4": "sensor_Series_Status",
        "min_len_msg4":0,
        "opcode4": "0x54"
      }
    },
    "sensor_setup_srv": {
      "Name":"Sensor Setup Server",
      "isSupported" : true,
      "sig_id": "0x1101",
      "Group": "Sensor",
      "modelCbStructName": "",
      "userDataStructType": "",
      "defaultUserData": "",
      "publication_size": 2 + 5,
      "Element": "Main",
      "messages": {
        "num_of_msg": 10,
        "msgName1": "sensor_Cadence_get",
       "min_len_msg1":0,
        "opcode1": "0x82, 0x34",
        "msgName2": "sensor_Cadence_set",
       "min_len_msg2":0,
        "opcode2": "0x55",
		    "msgName3": "sensor_Cadence_set_unack",
       "min_len_msg3":0,
        "opcode3": "0x56",
		"msgName4": "sensor_settings_get",
       "min_len_msg4":0,
        "opcode4": "0x82, 0x35",
		    "msgName5": "sensor_setting_get",
       "min_len_msg5":0,
        "opcode5": "0x82, 0x36",
        "msgName6": "sensor_setting_set",
       "min_len_msg6":0,
        "opcode6": "0x59",
		    "msgName7": "sensor_setting_set_unack",
       "min_len_msg7":0,
        "opcode7": "0x5A",
        "msgName8": "sensor_Cadence_status",
       "min_len_msg8":0,
        "opcode8": "0x57",
        "msgName9": "sensor_Settings_status",
       "min_len_msg9":0,
        "opcode9": "0x58",
        "msgName10": "sensor_Setting_status",
       "min_len_msg10":0,
        "opcode10": "0x5B",
      }
    }
  }
//}


const advParamsRanges = {
  priAdvIntMinValue:            "20",                 // Min value of Primary Advertising Interval (ms)
  priAdvIntMaxValue:            "480000",             // Max value of Primary Advertising Interval (ms)
  priAdvIntMaxValueAdvData:     "40959.375",          // Max value of Advertising Interval (ms)
  advDataTXPowerMinValue:       "-127",               // Min value of TX Power (dBm)
  advDataTXPowerMaxValue:       "127",                // Max value of TX Power (dBm)
  advParamTXPowerMinValue:      "-127",               // Min value of Advertising TX Power (dBm)
  advParamTXPowerMaxValue:      "20",                 // Max value of Advertising TX Power (dBm)
  advSIDMinValue:               "0",                  // Min value of Advertising SID
  advSIDMaxValue:               "15"                  // Max value of Advertising SID
}

const connParamsRanges = {
  connectionIntMinValue:        "7.5",                // Min value of Connection Interval (ms)
  connectionIntMaxValue:        "4000",               // Max value of Connection Interval (ms)
  scanIntMinValue:              "2.5",                // Min value of Scan Interval (ms)
  scanIntMaxValue:              "40959.375",          // Max value of Scan Interval (ms)
  scanWinMinValue:              "2.5",                // Min value of Scan Window (ms)
  scanWinMaxValue:              "40959.375",          // Max value of Scan Window (ms)
  scanDurationMinValue:         "10",                 // Min value of Scan Duration (ms)
  scanDurationMaxValue:         "655350",             // Max value of Scan Duration (ms)
  connLatencyMinValue:          "0",                  // Min value of Connection Latency
  connLatencyMaxValue:          "499",                // Max value of Connection Latency
  connTimeoutMinValue:          "100",                // Min value of Connection Timeout (ms)
  connTimeoutMaxValue:          "32000",              // Min value of Connection Timeout (ms)
  maxPDUSizeMinValue:           "27",                 // Min value of MAX_PDU_SIZE
  maxPDUSizeMaxValue:           "255"                 // Max value of MAX_PDU_SIZE
}

// Maximum number of advertisement sets
const maxNumAdvSets = 20;

// Maximum length of legacy advertise data
const maxLegacyDataLen = 31;

// Maximum length of extended advertise data
const maxExtDataLen = 1650;
const maxExtConnDataLen = 254;

// Minimum length of extended headers data
const minExtHdrLen = 3; // Exntended Header Flags + AdvDataInfo
const advAHdrLen = 6;
const targetAHdrLen = 6;
const txPowerHdrLen = 1;

// Dictionary mapping a device name to default LaunchPad; used to discover the
// appropriate RF settings when a device is being used without a LaunchPad
const deviceToBoard = {
  CC1352R: "CC1352R1_LAUNCHXL",
  CC1352P1: "CC1352P1_LAUNCHXL",
  CC2642R1: "CC26X2R1_LAUNCHXL",
  CC2652R1: "CC26X2R1_LAUNCHXL",
  CC2652RB: "LP_CC2652RB",
  CC2652P1FSIP: "LP_CC2652PSIP",
  CC2652R1FSIP: "LP_CC2652RSIP",
  CC2652R7: "LP_CC2652R7",
  CC1352P7: "LP_CC1352P7",
  CC2651P3: "LP_CC2651P3",
  CC2651R3: "LP_CC2651R3"
};

// Settings for ti/devices/CCFG module
const bleCentralCCFGSettings = {
  CC1312R1_LAUNCHXL_CCFG_SETTINGS: {},
  CC1352R1_LAUNCHXL_CCFG_SETTINGS: {},
  CC1352P1_LAUNCHXL_CCFG_SETTINGS: {},
  CC1352P_2_LAUNCHXL_CCFG_SETTINGS: {},
  CC1352P_4_LAUNCHXL_CCFG_SETTINGS: {},
  CC26X2R1_LAUNCHXL_CCFG_SETTINGS: {},
  LP_CC2652RSIP_CCFG_SETTINGS: {},
  LP_CC2652PSIP_CCFG_SETTINGS: {},
  LP_CC2652R7_CCFG_SETTINGS: {},
  LP_CC1352P7_1_CCFG_SETTINGS: {},
  LP_CC1352P7_4_CCFG_SETTINGS: {},
  LP_CC2651P3_CCFG_SETTINGS: {},
  LP_CC2651R3_CCFG_SETTINGS: {},
  LP_CC2652RB_CCFG_SETTINGS: {
    srcClkLF: "Derived from HF XOSC"
  }
};

const supportedMigrations = {
  // Boards
  CC1352R1_LAUNCHXL:  [
    {target: "CC1352R1_LAUNCHXL"},
    {target: "CC1352R1F3RGZ"},
  ],
  CC1352P1_LAUNCHXL:[
    {target: "CC1352P1_LAUNCHXL"},
    {target: "CC1352P1F3RGZ"},
  ],
  CC1352P_2_LAUNCHXL: [
    {target: "CC1352P_2_LAUNCHXL"},
    {target: "CC1352P1F3RGZ"},
  ],
  CC1352P_4_LAUNCHXL: [
    {target: "CC1352P_4_LAUNCHXL"},
    {target: "CC1352P1F3RGZ"},
  ],
  CC26X2R1_LAUNCHXL: [
    {target: "CC26X2R1_LAUNCHXL"},
    {target: "LP_CC2652RSIP"},
    {target: "LP_CC2652RB"},
    {target: "CC2652RB1FRGZ"},
    {target: "CC2652R1FRGZ"},
    {target: "CC2642R1FRGZ"},
    {target: "CC2652R1FSIP"},
  ],
  LP_CC2652RB: [
    {target: "CC26X2R1_LAUNCHXL"},
    {target: "LP_CC2652RSIP"},
    {target: "LP_CC2652RB"},
    {target: "CC2652RB1FRGZ"},
    {target: "CC2652R1FRGZ"},
    {target: "CC2642R1FRGZ"},
    {target: "CC2652R1FSIP"},
  ],
  LP_CC2652PSIP: [
    {target: "LP_CC2652PSIP"},
    {target: "CC2652P1FSIP"},
  ],
  LP_CC2652RSIP: [
    {target: "CC26X2R1_LAUNCHXL"},
    {target: "LP_CC2652RSIP"},
    {target: "LP_CC2652RB"},
    {target: "CC2652RB1FRGZ"},
    {target: "CC2652R1FRGZ"},
    {target: "CC2642R1FRGZ"},
    {target: "CC2652R1FSIP"},
  ],

  //Devices
  CC1352R1F3RGZ: [
    {target: "CC1352R1F3RGZ"},
    {target: "CC1352R1_LAUNCHXL"},
  ],
  CC1352P1F3RGZ: [
    {target: "CC1352P1F3RGZ"},
    {target: "CC1352P1_LAUNCHXL"},
    {target: "CC1352P_2_LAUNCHXL"},
    {target: "CC1352P_4_LAUNCHXL"},
  ],
  CC2642R1FRGZ: [
    {target: "CC26X2R1_LAUNCHXL"},
    {target: "LP_CC2652RSIP"},
    {target: "LP_CC2652RB"},
    {target: "CC2652RB1FRGZ"},
    {target: "CC2652R1FRGZ"},
    {target: "CC2642R1FRGZ"},
    {target: "CC2652R1FSIP"},
  ],
  CC2652R1FRGZ: [
    {target: "CC26X2R1_LAUNCHXL"},
    {target: "LP_CC2652RSIP"},
    {target: "LP_CC2652RB"},
    {target: "CC2652RB1FRGZ"},
    {target: "CC2652R1FRGZ"},
    {target: "CC2642R1FRGZ"},
    {target: "CC2652R1FSIP"},
  ],
  CC2652RB1FRGZ: [
    {target: "CC26X2R1_LAUNCHXL"},
    {target: "LP_CC2652RSIP"},
    {target: "LP_CC2652RB"},
    {target: "CC2652RB1FRGZ"},
    {target: "CC2652R1FRGZ"},
    {target: "CC2642R1FRGZ"},
    {target: "CC2652R1FSIP"},
  ],
  CC2652R1FSIP: [
    {target: "CC26X2R1_LAUNCHXL"},
    {target: "LP_CC2652RSIP"},
    {target: "LP_CC2652RB"},
    {target: "CC2652RB1FRGZ"},
    {target: "CC2652R1FRGZ"},
    {target: "CC2642R1FRGZ"},
    {target: "CC2652R1FSIP"},
  ],
  CC2652P1FSIP: [
    {target: "CC2652P1FSIP"},
    {target: "LP_CC2652PSIP"},
  ]
};

const boardName = getBoardOrLaunchPadName(true);
const centralRoleCcfgSettings = bleCentralCCFGSettings[boardName + "_CCFG_SETTINGS"];

/*
 * ======== alphanumeric ========
 * Check if the string contains only letters, numbers and underscore
 *
 * @param inputtxt       - string number to be checked
 * @returns True if the string contains only letters, numbers and underscore otherwise return False.
 */
function alphanumeric(inputtxt)
{
  var letterNumber = /^[a-zA-Z0-9_]+$/;
  return (inputtxt.match(letterNumber))? true : false;
}

/*
 * ======== validateConnInterval ========
 * Validate this inst's configuration
 *
 * @param inst       - instance to be validated
 * @param validation - object to hold detected validation issues
 * @param minValue
 */
function validateConnInterval(inst,validation,minValue,minName,maxValue,maxName)
{
  if(minValue > maxValue)
  {
      validation.logError("Shall be greater than or equal to connectin interval min"
                          , inst, maxName);
      validation.logError("Shall be less than or equal to connection interval max"
                          , inst, minName);
  }
  if(minValue < connParamsRanges.connectionIntMinValue || minValue > connParamsRanges.connectionIntMaxValue)
  {
      validation.logError("The Range of connection interval is " + connParamsRanges.connectionIntMinValue +
                          " ms to "+ connParamsRanges.connectionIntMaxValue +" ms", inst, minName);
  }
  if(maxValue < connParamsRanges.connectionIntMinValue || maxValue > connParamsRanges.connectionIntMaxValue)
  {
      validation.logError("The Range of connection interval is " + connParamsRanges.connectionIntMinValue +
                          " ms to " + connParamsRanges.connectionIntMaxValue + " ms" ,inst, maxName);
  }
}

/*
 * ======== validateAdvInterval ========
 * Validate the Adv Interval configuration
 *
 * @param inst       - instance to be validated
 * @param validation - object to hold detected validation issues
 * @param minValue
 */
function validateAdvInterval(inst,validation,minValue,minName,maxValue,maxName)
{
  if(minValue > maxValue)
  {
      validation.logError("Shall be greater than or equal to connectin interval min"
                          , inst, maxName);
      validation.logError("Shall be less than or equal to connection interval max"
                          , inst, minName);
  }
  if(minValue < advParamsRanges.priAdvIntMinValue || minValue > advParamsRanges.priAdvIntMaxValue)
  {
      validation.logError("The Range of connection interval is " + advParamsRanges.priAdvIntMinValue +
                          " ms to "+ advParamsRanges.priAdvIntMaxValue +" ms", inst, minName);
  }
  if(maxValue < advParamsRanges.priAdvIntMinValue || maxValue > advParamsRanges.priAdvIntMaxValue)
  {
      validation.logError("The Range of connection interval is " + advParamsRanges.priAdvIntMinValue +
                          " ms to " + advParamsRanges.priAdvIntMaxValue + " ms" ,inst, maxName);
  }
}

/*
 * ======== validateUUIDLength ========
 * Validate the UUIDs configurable length
 *
 * @param inst       - instance to be validated
 * @param validation - object to hold detected validation issues
 * @param numOfUUIDs - the number of UUIDs that where added by the user
 * @params uuid      - the name of the uuid parameter to validate
 * @length           - the length of each uuid
 */
function validateUUIDLength(inst,validation,numOfUUIDs,uuid,length)
{
  for(let i = 0; i < numOfUUIDs; i++)
  {
    if(inst["UUID" + i + uuid].toString(16).length > length)
    {
        validation.logError("The number of bytes is " + _.divide(length,2) ,inst, "UUID" + i + uuid);
    }
  }
}

/*
 * ======== validateNumOfUUIDs ========
 * Validate the number of UUIDs configurable
 *
 * @param inst       - instance to be validated
 * @param validation - object to hold detected validation issues
 * @param numOfUUIDs - the number of UUIDs that where added by the user
 * @params uuid      - the name of the uuid parameter to validate
 * @length           - the length of each uuid
 */
function validateNumOfUUIDs(inst,validation,numOfUUIDs,uuid,length)
{
  //Check if the number of UUIDs is valid
  if(inst[numOfUUIDs] < 0 || inst[numOfUUIDs] > 10)
  {
      validation.logError("Please enter a number between 0 - 10", inst, numOfUUIDs);
  }
  // Validate each UUID length
  else
  {
      validateUUIDLength(inst,validation,inst[numOfUUIDs],uuid,length);
  }
}

/*
 *  ======== addPeerAddress ========
 *  Gets an address in hex format and return it
 *  in the following format { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa }
 *
 *  @param param  - An address in hex format
 *
 *  @returns the address in the following format: { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa }
 */
function addPeerAddress(param)
{
  let address = "{ ";
  let prefix = "0x";

  address += prefix;
  for (let i = 0; i < param.length; i++)
  {
    address += param[i];
    if (i%2 != 0 && i < (param.length -1))
    {
      address += ", " + prefix;
    }
  }
  address += " }";

  return(address);
}

/*
 *  ======== reverseBytes ========
 *  Reverse the bytes of an hex string
 *
 *  @stringToReverse hex string to reverse its bytes
 */
function reverseBytes(stringToReverse)
{
  var str = stringToReverse.match(/.{1,2}/g);
  return str.reverse().toString(16).replace(/,/g,'');
}

/*
 *  ======== checkTXPower ========
 *  Help function for the advertisement params
 *  inst, TX Power
 *
 *  @param param
 *  @param value
 *  @returns the total length of an advertisement data
 */
function checkTXPower(param,value)
{
  return param == "GAP_ADV_TX_POWER_NO_PREFERENCE" ? param : value;
}

/*
 *  ======== getChanMap ========
 *  Returns the channel map according to the selected channels
 *
 *  @param array of selected channels
 *  @returns channel map
 */
function getChanMap(value)
{
  const allChan = ["GAP_ADV_CHAN_37", "GAP_ADV_CHAN_38", "GAP_ADV_CHAN_39"];
  if(_.isEqual(value, allChan))
  {
    return "GAP_ADV_CHAN_ALL";
  }
  else if(_.isEqual(value, ["GAP_ADV_CHAN_37", "GAP_ADV_CHAN_38"]))
  {
    return "GAP_ADV_CHAN_37_38";
  }
  else if(_.isEqual(value, ["GAP_ADV_CHAN_37", "GAP_ADV_CHAN_39"]))
  {
    return "GAP_ADV_CHAN_37_39";
  }
  else if(_.isEqual(value, ["GAP_ADV_CHAN_38", "GAP_ADV_CHAN_39"]))
  {
    return "GAP_ADV_CHAN_38_39";
  }
  else if(_.isEqual(value, ["GAP_ADV_CHAN_37"]))
  {
    return "GAP_ADV_CHAN_37";
  }
  else if(_.isEqual(value, ["GAP_ADV_CHAN_38"]))
  {
    return "GAP_ADV_CHAN_38";
  }
  else if(_.isEqual(value, ["GAP_ADV_CHAN_39"]))
  {
    return "GAP_ADV_CHAN_39";
  }
}

/*
 *  ======== addZeroFromLeft ========
 *  Adds zero to the left of an Hex format number 
 *  that is uneven
 *
 *  @param hexNum
 *  @returns an Hex number
 */
function addZeroFromLeft(hexNum)
{
    if(hexNum.length % 2 != 0)
    {
        hexNum = '0' + hexNum;
    }
    return hexNum;
}

/*
 *  ======== getSelectedDataList ========
 *  Gets the full advertisement data list
 *
 *  @param inst
 *  @param config
 *  @returns the selected parameters list
 */
function getSelectedDataList(inst, config)
{
  let values = [];
  for(let i = 0; i < config.length; i++)
  {
    if(config[i].name.includes("GAP_ADTYPE") && inst[config[i].name])
    {
      values.push(config[i]);
      i++;
      while(i < config.length && !(config[i].name.includes("GAP_ADTYPE")))
      {
        values.push(config[i]);
        i++;
      }
      i--;
    }
  }
  return values;
}

/*
 *  ======== advDataTotalLength ========
 *  Calculates the total length of advertisement data
 *
 *  @param inst
 *  @param config
 *  @returns the total length of an advertisement data
 */
function advDataTotalLength(inst, config)
{
    let values = getSelectedDataList(inst, config);

    let totalLength = 0;

    for(let i =0; i < values.length; i++)
    { 
      if(values[i].name.includes("GAP_ADTYPE"))
      {
        totalLength += 2;
      }
      if(values[i].name == "advertisingFlags" || values[i].name == "TXPower")
      {
        totalLength += 1;
      }
      else if(values[i].name.includes("numOfUUIDs") && values[i].name.includes("16"))
      {
        totalLength += 2*inst[values[i].name];
      }
      else if(values[i].name.includes("numOfUUIDs") && values[i].name.includes("32"))
      {
        totalLength += 4*inst[values[i].name];
      }
      else if(values[i].name.includes("numOfUUIDs") && values[i].name.includes("128"))
      {
        totalLength += 16*inst[values[i].name];
      }
      else if(values[i].name.includes("numOf") && values[i].name.includes("Addresses"))
      {
        totalLength += 6*inst[values[i].name];
      }
      else if(values[i].name.includes("LocalName"))
      {
        totalLength += inst[values[i].name].toString().length;
      }
      else if(values[i].name.includes("ConnInterval") || values[i].name.includes("advIntValue")
              || values[i].name.includes("appearanceValue"))
      {
        totalLength += 2;
      }
      else if(values[i].name.includes("companyIdentifier") || values[i].name.includes("additionalData"))
      {
        totalLength += _.divide(addZeroFromLeft(inst[values[i].name].toString(16)).length, 2);
      }
      else if(values[i].name.includes("16SDData") && inst.GAP_ADTYPE_SERVICE_DATA)
      {
        if(values[i].name[4] < inst.numOfUUIDs16SD)
        {
          totalLength += _.divide(addZeroFromLeft(inst[values[i].name].toString(16)).length,2);
        }
      }
      else if(values[i].name.includes("32SDData") && inst.GAP_ADTYPE_SERVICE_DATA_32BIT)
      {
        if(values[i].name[4] < inst.numOfUUIDs32SD)
        {
          totalLength += _.divide(addZeroFromLeft(inst[values[i].name].toString(16)).length,2);
        }
      }
      else if(values[i].name.includes("128SDData") && inst.GAP_ADTYPE_SERVICE_DATA_128BIT)
      {
        if(values[i].name[4] < inst.numOfUUIDs128SD)
        {
          totalLength += _.divide(addZeroFromLeft(inst[values[i].name].toString(16)).length,2);
        }
      }
    }
    return totalLength;
}

/*
 *  ======== advDataHexValues ========
 *  Gets Hex format number and return it
 *  in the following format 0xaa,\n 0xaa,\n ....
 *
 *  @param param  - Hex format number
 *
 *  @returns the Hex format number in the following format: 0xaa,\n 0xaa,\n ....
 */
function advDataHexValues(param)
{
  let address = "";
  let prefix = "0x";

  address += prefix;
  for (let i = 0; i < param.length; i++)
  {
    address += param[i];
    if (i%2 != 0)
    {
      address += ",\n"
      if(i < (param.length -1))
        {
          address += "  " + prefix;
        }
    }
  }

  return(address);
}
/*
 *  ======== listOfHexValues ========
 *  Gets Hex format number and return it
 *  in the following format 0xaa,0xaa,....
 *
 *  @param param  - Hex format number
 *
 *  @returns the Hex format number in the following format: 0xaa,\n 0xaa,\n ....
 */
function listOfHexValues(param)
{
  let hexList = "";
  let prefix = "0x";

  hexList += prefix;
  for (let i = 0; i < param.length; i++)
  {
    hexList += param[i];
    if (i%2 != 0)
    {
      if(i < (param.length -1))
        {
          hexList += ","
          hexList += prefix;
        }
    }
  }

  return(hexList);
}

/*!
 *  ======== device2DeviceFamily ========
 *  Map a pimux deviceID to a TI-driver device family string
 *
 *  @param deviceId  - a pinmux deviceId (system.deviceData)
 *
 *  @returns String - the corresponding "DeviceFamily_xxxx" string
 *                    used by driverlib header files.
 */
function device2DeviceFamily(deviceId)
{
    let driverString = null;

    /* Determine libraries required by device name. */
    if(deviceId.match(/CC1352P7/))
    {
        driverString = "DeviceFamily_CC13X2X7";
    }
    else if(deviceId.match(/CC2652.7/))
    {
        driverString = "DeviceFamily_CC26X2X7";
    }
    else if(deviceId.match(/CC13.2/))
    {
        driverString = "DeviceFamily_CC13X2";
    }
    else if(deviceId.match(/CC13.0/))
    {
        driverString = "DeviceFamily_CC13X0";
    }
    else if(deviceId.match(/CC26.0R2/))
    {
        driverString = "DeviceFamily_CC26X0R2";
    }
    else if(deviceId.match(/CC26.2/))
    {
        driverString = "DeviceFamily_CC26X2";
    }
    else if(deviceId.match(/CC26.0/))
    {
        driverString = "DeviceFamily_CC26X0";
    }
    else if(deviceId.match(/CC3220/))
    {
        driverString = "DeviceFamily_CC3220";
    }
    else if(deviceId.match(/MSP432E.*/))
    {
        driverString = "DeviceFamily_MSP432E401Y";
    }
    else if(deviceId.match(/MSP432P4.1.I/)
            || deviceId.match(/MSP432P4111/))
    {
        driverString = "DeviceFamily_MSP432P4x1xI";
    }
    else if(deviceId.match(/MSP432P4.1.T/))
    {
        driverString = "DeviceFamily_MSP432P4x1xT";
    }
    else if(deviceId.match(/MSP432P401/))
    {
        driverString = "DeviceFamily_MSP432P401x";
    }
    else if(deviceId.match(/CC26.1/))
    {
        driverString = "DeviceFamily_CC26X1";
    }
    else
    {
        driverString = "";
    }

    return(driverString);
}

/*!
 *  ======== getBoardOrLaunchPadName ========
 *  Get the name of the board (or device)
 *
 *  @param convertToBoard - Boolean. When true, return the associated LaunchPad
 *                          name if a device is being used without a LaunchPad
 *
 *  @returns String - Name of the board with prefix /ti/boards and
 *                    suffix .syscfg.json stripped off.  If no board
 *                    was specified, the device name is returned.
 */
function getBoardOrLaunchPadName(convertToBoard)
{
    let name = system.deviceData.deviceId;

    if(system.deviceData.board != null)
    {
        name = system.deviceData.board.source;

        /* Strip off everything up to and including the last '/' */
        name = name.replace(/.*\//, "");

        /* Strip off everything after and including the first '.' */
        name = name.replace(/\..*/, "");
    }

    // Check if this is a standalone device without a LaunchPad
    if(convertToBoard && !name.includes("LAUNCHXL") && !name.includes("LP_"))
    {
        // Find the LaunchPad name in deviceToBoard dictionary
        let key = null;
        for(key in deviceToBoard)
        {
            if(name.includes(key))
            {
                name = deviceToBoard[key];
                break;
            }
        }
    }

    return(name);
}

/*
 * ======== getRadioScript ========
 * Determines which rf_defaults script to use based on device or rfDesign
 *
 * @param rfDesign - the value of rfDesign parameter
 * @param deviceId - device being used
 *
 * @returns radioSettings - the rf_defaults script according to the selected
 *                          device/reDesign.
 *                          If device is not supported, returns null
 */
function getRadioScript(rfDesign, deviceId)
{
    let radioSettings = null;

    if(rfDesign !== null)
    {
        if(rfDesign === "LAUNCHXL-CC1352P-4")
        {
            radioSettings = system.getScript("/ti/ble5stack/rf_config/"
                + "CC1352P_4_LAUNCHXL_rf_defaults.js");
        }
        else if(rfDesign === "LAUNCHXL-CC1352P1")
        {
            radioSettings = system.getScript("/ti/ble5stack/rf_config/"
                + "CC1352P1_LAUNCHXL_rf_defaults.js");
        }
        else if(rfDesign === "LAUNCHXL-CC1352P-2")
        {
            radioSettings = system.getScript("/ti/ble5stack/rf_config/"
                + "CC1352P_2_LAUNCHXL_rf_defaults.js");
        }
        else if(rfDesign === "LP_CC1352P7-1")
        {
            radioSettings = system.getScript("/ti/ble5stack/rf_config/"
                + "LP_CC1352P7_1_rf_defaults.js");
        }
        else if(rfDesign === "LP_CC1352P7-4")
        {
            radioSettings = system.getScript("/ti/ble5stack/rf_config/"
                + "LP_CC1352P7_4_rf_defaults.js");
        }
    }
    else if(deviceId === "CC1352P1F3RGZ")
    {
        radioSettings = system.getScript("/ti/ble5stack/rf_config/"
            + "CC1352P1_LAUNCHXL_rf_defaults.js");
    }

    if(deviceId === "CC1352R1F3RGZ")
    {
        radioSettings = system.getScript("/ti/ble5stack/rf_config/"
            + "CC1352R1_LAUNCHXL_rf_defaults.js");
    }
    else if(deviceId === "CC2642R1FRGZ")
    {
        radioSettings = system.getScript("/ti/ble5stack/rf_config/"
            + "CC26X2R1_LAUNCHXL_rf_defaults.js");
    }
    else if(deviceId === "CC2652R1FRGZ")
    {
        radioSettings = system.getScript("/ti/ble5stack/rf_config/"
            + "CC26X2R1_LAUNCHXL_rf_defaults.js");
    }
    else if(deviceId === "CC2652RB1FRGZ")
    {
        radioSettings = system.getScript("/ti/ble5stack/rf_config/"
            + "LP_CC2652RB_rf_defaults.js");
    }
    else if(deviceId === "CC2652R1FSIP")
    {
        radioSettings = system.getScript("/ti/ble5stack/rf_config/"
            + "LP_CC2652RSIP_rf_defaults.js");
    }
    else if(deviceId === "CC2652P1FSIP")
    {
        radioSettings = system.getScript("/ti/ble5stack/rf_config/"
            + "LP_CC2652PSIP_rf_defaults.js");
    }
    else if(deviceId === "CC2652R7RGZ")
    {
        radioSettings = system.getScript("/ti/ble5stack/rf_config/"
            + "LP_CC2652R7_rf_defaults.js");
    }
    else if(deviceId === "CC2651P3RGZ")
    {
        radioSettings = system.getScript("/ti/ble5stack/rf_config/"
            + "LP_CC2651P3_rf_defaults.js");
    }
    else if(deviceId === "CC2651R3RGZ")
    {
        radioSettings = system.getScript("/ti/ble5stack/rf_config/"
            + "LP_CC2651R3_rf_defaults.js");
    }
    return(radioSettings);
}

/*
 *  ======== hideGroup ========
 *  Hide or UnHide an entire group
 *
 *  @param group   - The group config
 *  @param toHide  - True(Hide)/false(UnHide)
 *  @param ui      - The User Interface object
 */
function hideGroup(group, toHide, ui)
{
  let namesArray = _.map(group,function(n) {return n.name});
  _.each(namesArray, (cfg) => {cfg.includes("hide") ||
         cfg.includes("numOfDefAdvSets") ? true : ui[cfg].hidden = toHide;});
}

/*
 *  ======== getGroupByName ========
 *  Get a list of groups and a group name.
 *  Returns the group config array.
 *
 *
 *  @param groupList   - List of groups
 *  @param groupName   - The name of the group to return
 */
function getGroupByName(groupList, groupName)
{
  for(let i = 0; i < groupList.length; i++)
  {
    if(groupList[i].name == groupName)
    {
      return groupList[i].config;
    }
  }
}

function decimalToHexString(number)
{
  if (number < 0)
  {
    number = 0xFFFFFFFF + number + 1;
  }

  return '0x' + number.toString(16).toUpperCase();
}

/*
* ======== isMigrationValid ========
* Determines whether a migration from one board/device to another board/device
* is supported by the EasyLink module.
*
* @returns One of the following Objects:
*    - {} <--- Empty object if migration is valid
*    - {warn: "Warning markdown text"} <--- Object with warn property
*                                           if migration is valid but
*                                           might require user action
*    - {disable: "Disable markdown text"} <--- Object with disable property
*                                              if migration is not valid
*/
function isMigrationValid(currentTarget, migrationTarget)
{
  let migrationSupported = {disable: "Migration to this target is not supported via SysConfig. Consider starting from a more similar example to your desired migration target in <SDK_INSTALL_DIR>/examples/"};
  let currTarget = null;

  for( currTarget in supportedMigrations )
  {
    if( currTarget == currentTarget )
    {
      supportedMigrations[currTarget].forEach(target =>
      {
        if( target.target == migrationTarget )
        {
          migrationSupported = {};
        }
      });
    }
  }

  return migrationSupported;
}

/*
 * ======== migrate ========
 * Perform stack specific changes to the SysConfig env POST migration
 *
 * @param currTarget - Board/device being migrated FROM
 * @param migrationTarget - Board/device being migrated TO
 * @param env - SysConfig environment providing access to all configurables
 * @param projectName - Optional name of the project being migrated
 *
 * @returns boolean - true when migration is supported and succesful, false when
 *                    migration is not supported and/or unsuccesful
 */
function migrate(currTarget, migrationTarget, env, projectName = null)
{
  const migrationInfo = isMigrationValid(currTarget, migrationTarget);
  let migrationValid = true;

  // This is not a valid migration
  if(migrationInfo.disable)
  {
    migrationValid = false;
  }

  return migrationValid;
}

/*
* ======== getMigrationMarkdown ========
* Returns text in markdown format that customers can use to aid in migrating a
* project between device/boards. It is recommended to provide no more
* than 3 bullet points with up to 120 characters per line.
*
* @param currTarget - Board/device being migrated FROM
*
* @returns string - Markdown formatted string
*/
function getMigrationMarkdown(currTarget)
{
  // At this moment no message needed
  const migrationText = ``

  return(migrationText);
}

exports = {
    sigModelMapping:sigModelMapping,
    alphanumeric: alphanumeric,
    advParamsRanges: advParamsRanges,
    connParamsRanges: connParamsRanges,
    maxNumAdvSets: maxNumAdvSets,
    maxLegacyDataLen: maxLegacyDataLen,
    maxExtDataLen: maxExtDataLen,
    maxExtConnDataLen: maxExtConnDataLen,
    minExtHdrLen: minExtHdrLen,
    advAHdrLen: advAHdrLen,
    targetAHdrLen: targetAHdrLen,
    txPowerHdrLen: txPowerHdrLen,
    addPeerAddress: addPeerAddress,
    reverseBytes: reverseBytes,
    checkTXPower: checkTXPower,
    centralRoleCcfgSettings: centralRoleCcfgSettings,
    getChanMap: getChanMap,
    getSelectedDataList: getSelectedDataList,
    validateConnInterval: validateConnInterval,
    validateAdvInterval: validateAdvInterval,
    validateUUIDLength: validateUUIDLength,
    validateNumOfUUIDs: validateNumOfUUIDs,
    advDataTotalLength: advDataTotalLength,
    addZeroFromLeft: addZeroFromLeft,
    advDataHexValues: advDataHexValues,
    listOfHexValues: listOfHexValues,
    getBoardOrLaunchPadName: getBoardOrLaunchPadName,
    device2DeviceFamily: device2DeviceFamily,
    getRadioScript: getRadioScript,
    hideGroup: hideGroup,
    getGroupByName: getGroupByName,
    decimalToHexString: decimalToHexString,
    isMigrationValid: isMigrationValid,
    migrate: migrate,
    getMigrationMarkdown: getMigrationMarkdown
};
