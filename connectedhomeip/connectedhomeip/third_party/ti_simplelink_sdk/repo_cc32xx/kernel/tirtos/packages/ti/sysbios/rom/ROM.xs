/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 *  ======== ROM.xs ========
 */

var ROM = null;

var BIOS = null;
var Build = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    ROM = this;

    Build = xdc.module('ti.sysbios.Build');

    var GetSet = xdc.module("xdc.services.getset.GetSet");
    GetSet.onSet(this, "romName", _setRomName);
}

/*
 *  ======== module$use ========
 */
function module$use()
{
}

/*
 *  ======== writeFile ========
 */
function writeFile(filename, content)
{
    var idx = 0;

    /* Open/Create file */
    var file = new java.io.FileWriter(filename);

    for(idx = 0; idx < content.length; idx++) {
        file.write(content[idx] + "\n");
    }
    file.flush();
}

/*
 *  ======== makeExternsFile ========
 */
function makeExternsFile(fileName)
{
    if (ROM.groupFuncPtrs == true) {
        var funcArray = new Array();
	for (var i = 0; i < ROM.excludeFuncs.length; i++) {
	    funcArray[i] = ROM.excludeFuncs[i].name;
        }
        writeFile(fileName, funcArray);
    }
    else {
        writeFile(fileName, ROM.excludedFuncs);
    }
}

/*
 *  ======== getOtherLibs ========
 */
function getOtherLibs()
{
    var otherLibs = "";

    for (var i = 0; i < ROM.otherLibs.length; i++) {
        otherLibs += "-l \"" + ROM.otherLibs[i] + "\" ";
    }
    return (otherLibs);
}

/*
 *  ======== getLibDefs ========
 */
function getOtherLibDefs()
{
    var otherLibDefs = "";

    for (var i = 0; i < ROM.otherLibFuncs.length; i++) {
        otherLibDefs += " -D" + ROM.otherLibFuncs[i] + "=\\(*ti_sysbios_rom_ROM_" + ROM.otherLibFuncs[i] +"\\)";
    }

    return (otherLibDefs);
}

/*
 *  ======== getExternDefs ========
 */
function getExternDefs()
{
    var externDefs = "";

    for (var i = 0; i < ROM.excludedFuncs.length; i++) {
        externDefs += " -D" + ROM.excludedFuncs[i] + "=\\(*ti_sysbios_rom_ROM_" + ROM.excludedFuncs[i] +"\\)";
    }

    return (externDefs);
}

/*
 *  ======== getGeneratedDefs ========
 */
function getGeneratedDefs()
{
    var generatedDefs = "";

    for (var i = 0; i < ROM.generatedFuncs.length; i++) {
        generatedDefs += " -D" + ROM.generatedFuncs[i] + "=\\(*ti_sysbios_rom_ROM_" + ROM.generatedFuncs[i] +"\\)";
    }

    return (generatedDefs);
}

/*
 *  ======== getConfigDefs ========
 */
function getConfigDefs()
{
    var configDefs = "";

    for (var i = 0; i < ROM.configDefs.length; i++) {
        configDefs += " " + ROM.configDefs[i];
    }

    return (configDefs);
}

/*
 *  ======== getExterns ========
 */
function getExterns()
{
    var externs = "";

    for (var i = 0; i < ROM.excludedFuncs.length; i++) {
        externs += ROM.excludedFuncs[i] + " ";
    }

    return (externs.trim());
}

/*
 *  ======== getEnumString ========
 *  Get the enum value string name, not 0, 1, 2 or 3, etc.  For an enumeration
 *  type property.
 *
 *  Example usage:
 *  if obj contains an enumeration type property "Enum enumProp"
 *
 *  view.enumString = getEnumString(obj.enumProp);
 *
 */
function getEnumString(enumProperty)
{
    /*
     *  Split the string into tokens in order to get rid of the huge package
     *  path that precedes the enum string name. Return the last 2 tokens
     *  concatenated with "_"
     */
    var enumStrArray = String(enumProperty).split(".");
    var len = enumStrArray.length;
    return (enumStrArray[len - 1]);
}

/*
 *  ======== _setRomName ========
 */
function _setRomName(field, val)
{
    var RomModule = null;

    switch (ROM.romName) {
        case ROM.NO_ROM:
	    RomModule = null; 
            Build.buildROM = false;
            Build.buildROMApp = false;
            break;

        case ROM.CC2650:
            RomModule = xdc.useModule('ti.sysbios.rom.cortexm.cc26xx.CC26xx');
            break;

        case ROM.CC2640R2F:
            RomModule = xdc.useModule('ti.sysbios.rom.cortexm.cc26xx.r2.CC26xx');
            break;

        case ROM.CC2652:
            ROM.$logWarning("Unrecognized ROM name: " +
                getEnumString(ROM.romName) +
                ". Please use either 'CC26X2' or 'CC13X2'",
                ROM, "romName");
        case ROM.CC13X2:
        case ROM.CC26X2:
        case ROM.CC13X2V2:
        case ROM.CC26X2V2:
            RomModule = xdc.useModule('ti.sysbios.rom.cortexm.cc26xx.cc26x2v2.CC26xx');
            break;

        case ROM.CC1350:
            RomModule = xdc.useModule('ti.sysbios.rom.cortexm.cc13xx.CC13xx');
            break;

	default:
	    ROM.$logError("Unrecognized ROM name.", ROM, "romName");
	    break;
    }

    if (RomModule != null) {
        if (Build.buildROMApp == true) {
            RomModule.loadAppConfig();
        }
    }
}
