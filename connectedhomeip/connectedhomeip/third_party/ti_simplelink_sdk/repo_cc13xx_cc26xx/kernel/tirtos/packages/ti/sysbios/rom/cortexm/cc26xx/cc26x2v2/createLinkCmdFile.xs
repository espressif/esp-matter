/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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

var isIAR = false;
var isGNU = false;
var romMapArr;

/*
 *  ======== readFile ========
 */
function readFile(filename)
{
    var line, idx = 0;
    var lineArr = new Array();

    /* Open file */
    var file = new java.io.BufferedReader(new java.io.FileReader(filename));

    /* Read file */
    while ((line = file.readLine()) != null) {
        lineArr[idx++] = line;
    }

    return lineArr;
}

/*
 *  ======== writeXdtFile ========
 */
function writeXdtFile(filename, content)
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
 *  ======== buildUndefSegment ========
 */
function buildUndefSegment(objDumpArray)
{
    var idx = 0;
    var symbolName;
    var tokens = new Array();
    var lineArray = new Array();

    lineArray[lineArray.length] = "";

    if (isGNU) {
        lineArray[lineArray.length] = "}";
    }

    for (idx = 0; idx < objDumpArray.length; idx++) {
        tokens = String(objDumpArray[idx]).split(/\s+/);
        symbolName = String(tokens[6]);
        if (isIAR) {
            lineArray[lineArray.length] = "--keep " + symbolName;
        }
        else if (isGNU) {
            lineArray[lineArray.length] = "EXTERN(" + symbolName + ")";
        }
        else {
            lineArray[lineArray.length] = "-u " + symbolName;
        }
    }

    return (lineArray);
}

/*
 *  ======== buildRomSegment ========
 */
function buildRomSegment(romTextArray)
{
    var idx = 0;
    var virtualAddr, symbolName;
    var tokens = new Array();
    var lineArray = new Array();
    var isFunc = false;

    lineArray[lineArray.length] = "";

    for (idx = 0; idx < romTextArray.length; idx++) {
        tokens = String(romTextArray[idx]).split(/\s+/);
        virtualAddr = "0x" + String(tokens[0]);

        if(tokens[4] == 0) {
            isFunc = true;
        }
        else { /* consts placed in text region have non-zero size */
            isFunc = false;
        }

        /* encode function addresses with LSB = 1 */
        if (isFunc == true) {
            virtualAddr = (parseInt(virtualAddr) | 1).toString(16);
        }
        else {
            virtualAddr = (parseInt(virtualAddr)).toString(16);
        }

        symbolName = String(tokens[6]);

        if (isIAR) {
           /* only place function definitions in IAR linker commenad file */
            if (isFunc == true) {
                lineArray[lineArray.length] = "--define_symbol " + symbolName + "=0x" + virtualAddr;
            }
        }
        else {
/* !!! hack to workaround TI linker warning regarding redefinition of memcpy and memset !!! */
            if (symbolName != "memset" && symbolName != "memcpy") {
                lineArray[lineArray.length] = symbolName + " = 0x" + virtualAddr + ";";
            }
        }
    }

    return (lineArray);
}

/*
 *  ======== buildPlacementSegment ========
 */
function buildPlacementSegment(dataArray)
{
    var idx = 0;
    var virtualAddr, symbolName;
    var tokens = new Array();
    var lineArray = new Array();

    lineArray[lineArray.length] = "";

    if (!isIAR) {
        lineArray[lineArray.length] = "SECTIONS";
        lineArray[lineArray.length] = "{";
    }

    for (idx = 0; idx < dataArray.length; idx++) {
        tokens = String(dataArray[idx]).split(/\s+/);
        virtualAddr = "0x" + String(tokens[0]);
        symbolName = String(tokens[6]);
        if (isIAR) {
            lineArray[lineArray.length] = "place at address mem:" +
                virtualAddr + " {readwrite section .data_" + symbolName + "};";
        }
        else if (!isGNU) {
            lineArray[lineArray.length] =
                "    .data:" + symbolName + ": LOAD > " + virtualAddr;
        }
    }

    if (isGNU) {
        lineArray[lineArray.length] =
            "    .rodata_other : { *(.rodata_*) } > REGION_TEXT";

        var found_rom_data = 0, rom_data_header = 0;

        for (idx = 0; idx < romMapArr.length; idx++) {
            if (romMapArr[idx].match(/^\.sysbios_rom_data/)) {
                found_rom_data = 1;
                continue;
            }
            if (found_rom_data == 1) {
                if (romMapArr[idx].match(/\.data/)) {
                    symbolName =
                        romMapArr[idx].substring(romMapArr[idx].indexOf(":")+1,
                        romMapArr[idx].lastIndexOf(")"));
                    virtualAddr = romMapArr[idx].match(/[a-fA-F0-9].{8}/);

                    if (!rom_data_header) {
                        lineArray[lineArray.length] =
                        "    .sysbios_rom_data 0x" + virtualAddr + " : {";
                        lineArray[lineArray.length] =
                        "        __sysbios_rom_data_load__ = LOADADDR (.sysbios_rom_data);"
                        lineArray[lineArray.length] =
                        "        __sysbios_rom_data_start__ = .;";
                        rom_data_header = 1;
                    }

                    lineArray[lineArray.length] =
                        "        *(.data_" + symbolName + ")";
                }
                else if (romMapArr[idx].match(/^$/)) {
                    found_rom_data = 0;
                }
            }
        }

        lineArray[lineArray.length] =
        "        __sysbios_rom_data_end__ = .;";
        lineArray[lineArray.length] =
        "    } AT> REGION_TEXT";

        lineArray[lineArray.length] =
        "    .sysbios_data_other : {";
        lineArray[lineArray.length] =
        "        __sysbios_data_other_load__ = LOADADDR (.sysbios_data_other);"
        lineArray[lineArray.length] =
        "        __sysbios_data_other_start__ = .;";
        lineArray[lineArray.length] =
        "        *(.data*)";
        lineArray[lineArray.length] =
        "        __sysbios_data_other_end__ = .;";
        lineArray[lineArray.length] =
        "    } > REGION_DATA AT> REGION_TEXT";
    }

    if (!isIAR && !isGNU) {
        lineArray[lineArray.length] = "}";
    }

    if (isIAR) {
        /*
         * Add section placements for the 3 referenced structures
         * The values for the 3 address labels are provided
         * by the linker command file generated for the application
         */
        lineArray[lineArray.length] = "";
        lineArray[lineArray.length] =
           "place at address mem:ROM_constStructAddr {section .const_ROM_constStruct};"
        lineArray[lineArray.length] =
           "place at address mem:ROM_dataStructAddr {section .data_ROM_stateStruct};"
        lineArray[lineArray.length] =
           "place at address mem:ROM_externFuncStructAddr {section .const_ROM_externFuncPtrs};"
    }

    return (lineArray);
}

/*
 *  ======== fixIarOffsets ========
 *  The IAR linker can't handle defining a symbol to be the sum of a symbol and
 *  a constant:
 *
 *   --define_symbol xdc_runtime_Error_policy__C=ROM_constStruct + 0x00000000
 *
 *  Consequently, the IAR linker template can't be generated statically at ROM build
 *  time. It must be generated dynamically at application build time so that the
 *  literal value of ROM_constStruct can be added to the offset of the corresponding
 *  field within the ROM_constStruct array:
 *
 *   --define_symbol xdc_runtime_Error_policy__C=1024
 *
 *  This function translates lines (coming from the generated CC26xx_RomStructs.cmd file)
 *  of this format:
 *
 *    xdc_runtime_Error_policy__C = ROM_constStruct + 0x00000000
 *
 *  into lines of this format:
 *
 *   --define_symbol xdc_runtime_Error_policy__C=`ROM.constStructAddr + 0x00000000`
 *
 *  The iar linker template will then translate that line into this:
 *
 *   --define_symbol xdc_runtime_Error_policy__C=1024
 *
 *  Sadly, unlike TI and GNU tools, the user must specify the addresses of where to place
 *  ROM_constStruct, ROM_stateStruct, and ROM_externFuncPtrSruct in their config file
 */
function fixIarOffsets(offsetsArray)
{
    var idx = 0;
    var virtualAddr, symbolName;
    var tokens = new Array();
    var lineArray = new Array();

    lineArray[lineArray.length] = "";

    lineArray[lineArray.length] = "%var ROM = xdc.module('ti.sysbios.rom.ROM');"

    for (idx = 0; idx < offsetsArray.length; idx++) {
        var offsetsLine =  offsetsArray[idx];
        if (offsetsLine.match('ROM_constStruct')) {
            tokens = String(offsetsArray[idx]).split(/\s+/);
            offsetsLine = "--define_symbol " + tokens[0] + '=' + '`ROM.constStructAddr + ' + tokens[4] + '`';
            offsetsLine = offsetsLine.replace(';','');
        }
        else if (offsetsLine.match('ROM_stateStruct')) {
            tokens = String(offsetsArray[idx]).split(/\s+/);
            offsetsLine = "--define_symbol " + tokens[0] + '=' + '`ROM.dataStructAddr + ' + tokens[4] + '`';
            offsetsLine = offsetsLine.replace(';','');
        }
        lineArray[lineArray.length] = offsetsLine;
    }

    lineArray[lineArray.length] = "";

    lineArray[lineArray.length] = "--config_def ROM_constStructAddr=" + '`ROM.constStructAddr`';
    lineArray[lineArray.length] = "--config_def ROM_dataStructAddr=" + '`ROM.dataStructAddr`';
    lineArray[lineArray.length] = "--config_def ROM_externFuncStructAddr=" + '`ROM.externFuncStructAddr`';

    return (lineArray);
}

function main(args)
{
    var romTextFile = args[0];
    var romConstFile = args[1];
    var romDataFile = args[2];
    var linkCmdFile = args[3];
    var offsetCmdFile = args[4];

    var linkConfigIARi, romMapFile;
    var romTextArr, romConstArr, romDataArr;
    var romTextIdx, romConstIdx, romDataIdx;
    var romOffsetsArr;

    var undefSegment, undefSegmentIAR;
    var romSymbolSegment, romSymbolSegmentIAR;
    var placementSegment, placementSegmentIAR;

    if (linkCmdFile.match(/_iar/)) {
        isIAR = true;
    }

    if (linkCmdFile.match(/_gnu/)) {
        isGNU = true;
        romMapFile = args[5];
        romMapArr = readFile(romMapFile);
    }

    romTextArr = readFile(romTextFile);
//    romConstArr = readFile(romConstFile);
    romDataArr = readFile(romDataFile);
    romOffsetsArr = readFile(offsetCmdFile);

    if (isIAR) {
        iarRomOffsetsArr = fixIarOffsets(romOffsetsArr);
    }

    /*
     * the linkcmd.xdt template has 3 main segments:
     *
     *  the -u segment which forces the inclusion of all the ROM referenced
     *  symbols (consts and data) by -u undefining them.
     *
     *  the API function address assignment segment which defines all
     *  the symbols in the ROM and assigns their addresses to them.
     *
     *  the placement segment where all the ROM referenced symbols'
     *  (consts and data) sections are placed at specific addresses.
     */

    undefSegment = buildUndefSegment(romDataArr);
    romSymbolSegment = buildRomSegment(romTextArr);
    placementSegment = buildPlacementSegment(romDataArr);

print("Writing linkcmd.xdt");

    if (isIAR) {
        writeXdtFile(linkCmdFile, undefSegment.concat(romSymbolSegment, iarRomOffsetsArr));
        writeXdtFile("TIRTOS_ROM.icf", placementSegment);
    }
    else if (isGNU) {
        writeXdtFile(linkCmdFile, undefSegment.concat(romSymbolSegment, romOffsetsArr, placementSegment));
    }
    else {
        writeXdtFile(linkCmdFile, undefSegment.concat(romSymbolSegment, romOffsetsArr, placementSegment));
    }
}
