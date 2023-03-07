/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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
 */

/*
 *  =========================== thread_common.js ===========================
 *  Common constants and functions shared within the SysConfig Thread module
 */

"use strict";

/* Poll period bounds (ms) */
const POLL_PERIOD_MAX = 0x3FFFFFF;
const POLL_PERIOD_WARN_MIN = 10;

/* PSKd length bounds in number of characters (bytes) */
const PSKD_MIN_LEN = 6;
const PSKD_MAX_LEN = 32;

/* Extended address (device ID) length (bytes) */
const EXT_ADDR_LEN = 8;

/* PAN ID length (bytes) */
const PAN_ID_LEN = 2;

/* Extended PAN ID length (bytes) */
const EXT_PAN_ID_LEN = 8;

/* Master key length (bytes) */
const MASTER_KEY_LEN = 16;

/* Network name (UTF-8) max length (bytes) */
const NETWORK_NAME_MAX_LEN = 16;

// Dictionary mapping a device name to default LaunchPad; used to discover the
// appropriate RF settings when a device is being used without a LaunchPad
const deviceToBoard = {
    CC1352R: "CC1352R1_LAUNCHXL",
    CC1352P: "CC1352P1_LAUNCHXL",
    CC2652R1: "CC26X2R1_LAUNCHXL",
    CC2652RB: "LP_CC2652RB"
};

/*!
 *  ======== getDeviceOrLaunchPadName ========
 *  Get the name of the board (or device)
 *
 *  @param convertToBoard - Boolean. When true, return the associated LaunchPad
 *                          name if a device is being used without a LaunchPad
 *
 *  @returns String - Name of the board with prefix /ti/boards and
 *                    suffix .syscfg.json stripped off.  If no board
 *                    was specified, the device name is returned.
 */
function getDeviceOrLaunchPadName(convertToBoard)
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
    if(convertToBoard && !name.includes("_LAUNCHXL") && !name.includes("LP_"))
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
 * Text for each disable case.
 */
const disabled_PtoR = { disable: "Migration from P to R devices requires a different rf setup command and is not currently supported" };
const disabled_RtoP = { disable: "Migration from R to P devices requires a different rf setup command and is not currently supported" };
const disabled_noSupport = { disable: "This target is not supported" };

/*
 * Migration between R devices is supported. This does not require changes to
 * the code. It is a bit odd to mark that current device migrating to current
 * device is supported.
 */
const migrationsR = {
    CC1312R1_LAUNCHXL: disabled_noSupport,
    CC1352R1_LAUNCHXL: { },
    CC26X2R1_LAUNCHXL: { },
    LP_CC2652RSIP:     { },
    CC1312R1F3RGZ:     disabled_noSupport,
    CC1352R1F3RGZ:     { },
    CC2652R1FRGZ:      { },
    CC2652R1FSIP:      { },

    CC1352P1_LAUNCHXL:  disabled_RtoP,
    CC1352P_2_LAUNCHXL: disabled_RtoP,
    CC1352P_4_LAUNCHXL: disabled_RtoP,
    LP_CC2652PSIP:      disabled_RtoP,
    CC1352P1F3RGZ:      disabled_RtoP,
    CC2652P1FSIP:       disabled_RtoP,
    CC2652P1FRGZ:       disabled_RtoP,
};

/*
 * Migration between P devices is supported. This does not require changes to
 * the code. It is a bit odd to mark that current device migrating to current
 * device is supported.
 */
const migrationsP = {
    CC1352P1_LAUNCHXL:  { },
    CC1352P_2_LAUNCHXL: { },
    CC1352P_4_LAUNCHXL: { },
    LP_CC2652PSIP:      { },
    CC1352P1F3RGZ:      { },
    CC2652P1FSIP:       { },
    CC2652P1FRGZ:       { },

    CC1312R1_LAUNCHXL: disabled_noSupport,
    CC1352R1_LAUNCHXL: disabled_PtoR,
    CC26X2R1_LAUNCHXL: disabled_PtoR,
    LP_CC2652RSIP:     disabled_PtoR,
    CC1312R1F3RGZ:     disabled_noSupport,
    CC1352R1F3RGZ:     disabled_PtoR,
    CC2652R1FRGZ:      disabled_PtoR,
    CC2652R1FSIP:      disabled_PtoR,
};

/*
 * `migrations` is a map of maps, with the first key being the current
 * platform and the second key being the target platform. A non existent key
 * will default to that migration being disabled. The object within the second
 * map will be returned by `isMigrationValid` and can contain `warn` or
 * `disable` members to offer more intelligent prompts. An empty object will
 * mean the migration is supported.
 *
 */
const migrations = {
    CC1352R1_LAUNCHXL:  migrationsR,
    CC26X2R1_LAUNCHXL:  migrationsR,
    LP_CC2652RSIP:      migrationsR,
    CC1352P1_LAUNCHXL:  migrationsP,
    CC1352P_2_LAUNCHXL: migrationsP,
    CC1352P_4_LAUNCHXL: migrationsP,
    LP_CC2652PSIP:      migrationsP,

    // Devices
    CC1352R1F3RGZ: migrationsR,
    CC2652R1FRGZ:  migrationsR,
    CC2652R1FSIP:  migrationsR,
    CC1352P1F3RGZ: migrationsP,
    CC2652P1FSIP:  migrationsP,
    CC2652P1FRGZ:  migrationsP,
};


/*
 * ======== isMigrationValid ========
 * Determines whether a migration from one board/device to another board/device
 * is supported by the TI-OpenThread module.
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

    if (migrations[currentTarget] && migrations[currentTarget][migrationTarget]) {
        migrationSupported = migrations[currentTarget][migrationTarget];
    }
    return(migrationSupported);
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
function migrate(currTarget, migrationTarget, env, projectName=null)
{
    const migrationInfo= isMigrationValid(currTarget, migrationTarget);
    let migrationValid = true;

    if (migrationInfo.disable)
    {
        migrationValid = false;
    }

    if (migrationValid)
    {
        // disabled
    }

    return(migrationValid);
}

/*
 * ======== getMigrationMarkdown ========
 * Returns text in markdown format that customers can use to aid in migrating a
 * project between device/boards. It's recommended to keep the text within X
 * characters in the following format:
 *
 * TBD
 *
 * @param currTarget - Board/device being migrated FROM
 *
 * @returns string - Markdown formatted string
 */
function getMigrationMarkdown(currTarget)
{
    const migrationText = `\
* Migration of OAD examples between device types may require changes to device family predefines\n
  * [Consult the OAD documentation for more details](thread/html/thread-oad/native-oad.html#ti-openthread-oad-examples)\n
`

    return(migrationText);
}

// Settubgs fir ti/devices/CCFG module
const threadCCFGSettings = {
    CC1352R1_LAUNCHXL_CCFG_SETTINGS: {
        forceVddr: true
    },
    CC1352P1_LAUNCHXL_CCFG_SETTINGS: {
    },
    CC1352P_2_LAUNCHXL_CCFG_SETTINGS: {
        forceVddr: true
    },
    CC1352P_4_LAUNCHXL_CCFG_SETTINGS: {
    },
    CC26X2R1_LAUNCHXL_CCFG_SETTINGS: {
    },
    LP_CC2652RB_CCFG_SETTINGS: {
    },
    LP_CC2652PSIP_CCFG_SETTINGS: {
    },
    LP_CC2652RPSIP_CCFG_SETTINGS: {
    },
    LP_CC2674R10_CCFG_SETTINGS: {
    }
};

const boardName = getDeviceOrLaunchPadName(true);
const ccfgSettings = threadCCFGSettings[boardName + "_CCFG_SETTINGS"];

exports = {
    POLL_PERIOD_MAX: POLL_PERIOD_MAX,
    POLL_PERIOD_WARN_MIN: POLL_PERIOD_WARN_MIN,
    PSKD_MIN_LEN: PSKD_MIN_LEN,
    PSKD_MAX_LEN: PSKD_MAX_LEN,
    EXT_ADDR_LEN: EXT_ADDR_LEN,
    PAN_ID_LEN: PAN_ID_LEN,
    EXT_PAN_ID_LEN: EXT_PAN_ID_LEN,
    MASTER_KEY_LEN: MASTER_KEY_LEN,
    NETWORK_NAME_MAX_LEN: NETWORK_NAME_MAX_LEN,
    getDeviceOrLaunchPadName: getDeviceOrLaunchPadName,
    ccfgSettings: ccfgSettings,
    isMigrationValid: isMigrationValid,
    migrate: migrate,
    getMigrationMarkdown: getMigrationMarkdown
};
