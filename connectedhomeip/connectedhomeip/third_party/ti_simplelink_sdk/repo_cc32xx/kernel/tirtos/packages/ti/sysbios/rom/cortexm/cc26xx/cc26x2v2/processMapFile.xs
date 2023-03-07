/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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

function spacePad(num) {
    var s = "        " + num;
    return (s.substr(s.length-5));
}

/*
 *  ======== addFuncAliases ========
 * 
 * The linker doesn't show symbol aliases in the .sysbios_rom_text
 * section lists. Must use symbols sorted by address segment of the map
 * file to extract ALL the symbols in the .sysbios_rom_text section
 */
function addFuncAliases(mapFile, textFileArr)
{
    var tokens = new Array();
    var textTokens = new Array();
    var fullTextFileArr = new Array();
    var mapIdx, textIdx, fullTextIdx, virtualAddr, symbolName;
    var addr, start, end;
    var rootFuncName, rootFuncAddr, rootFuncSize, rootFound;

    mapFileArr = readFile(mapFile);

    textIdx = 0;

    /* extract start and end address of RTOS_ROM region */
    for (mapIdx = 0; mapIdx < mapFileArr.length; mapIdx++) {
        if (String(mapFileArr[mapIdx]).match("RTOS_ROM")) {
            tokens = String(mapFileArr[mapIdx++]).split(/\s+/);
	    start = Number("0x" + tokens[2]);
	    end = start + Number("0x" + tokens[4]);
            break;
	}
    }

    /* skip to symbols sorted by address section of .map file */
    for (; mapIdx < mapFileArr.length; mapIdx++) {
        if (String(mapFileArr[mapIdx]).match("SORTED BY Symbol Address")) {
            mapIdx += 4; /* skip past the section summary line */
            break;
        }
    }

    /* skip ahead to symbols within RTOS_ROM address range */
    for (; mapIdx < mapFileArr.length; mapIdx++) {
        tokens = String(mapFileArr[mapIdx]).split(/\s+/);
	addr = Number("0x" + tokens[0]) & 0xfffffffe; /* remove Thumb func encoded LSB */
	/* include only those symbols within the RTOS_ROM region (ie .sysbios_rom_text) */
	if (addr >= start) {
            break;
        }
    }

    /* get initial .sysbios_rom_text symbol address */
    textTokens = String(textFileArr[textIdx++]).split(/\s+/);
    rootFuncName = textTokens[2];
    rootFuncAddr = Number(textTokens[0]);
    rootFuncSize = spacePad(Number(textTokens[1]));
    rootFound = false;
    for (; mapIdx < mapFileArr.length; mapIdx++) {
        if (String(mapFileArr[mapIdx]) == "") {
            break;
        }
        tokens = String(mapFileArr[mapIdx]).split(/\s+/);
        addr = Number("0x" + tokens[0]) & 0xfffffffe; /* remove Thumb func encoded LSB */

        if (addr >= end) {
            break;
        }

        symbolName = String(tokens[1]);
        if (addr == rootFuncAddr) {
            if (rootFound == false) {
                fullTextFileArr[fullTextFileArr.length] = textTokens[0] + " " + rootFuncSize + "  " + symbolName;
                rootFound = true;
            }
            else {
                fullTextFileArr[fullTextFileArr.length] = textTokens[0] + " " + spacePad(0) + "  " + symbolName;
            }
        }
        else {
            if (textIdx >= textFileArr.length) {
                break;
            }

            /* finished with symbol from .sysbios_rom_text section, advance to next one */
            textTokens = String(textFileArr[textIdx++]).split(/\s+/);
            rootFuncName = textTokens[2];
            rootFuncAddr = Number(textTokens[0]);
            rootFuncSize = spacePad(Number(textTokens[1]));
            rootFound = false;
            if (addr == rootFuncAddr) {
                fullTextFileArr[fullTextFileArr.length] = textTokens[0] + " " + rootFuncSize + "  " + symbolName;
                rootFound = true;
            }
            else {
                if (addr < rootFuncAddr) {
                    /* symbol is in sorted list but NOT in section */
                    fullTextFileArr[fullTextFileArr.length] =  "0x" + addr.toString(16) + " " + spacePad(0) + "  " + symbolName;
                }
                else {
                    /* symbol is in section but not in sorted list */
                    fullTextFileArr[fullTextFileArr.length] = textTokens[0] + " " + rootFuncSize + "  " + textTokens[2];
                    /* move to next symbol in section list */
                    textTokens = String(textFileArr[textIdx++]).split(/\s+/);
                    rootFuncName = textTokens[2];
                    rootFuncAddr = Number(textTokens[0]);
                    rootFuncSize = spacePad(Number(textTokens[1]));
                    rootFound = false;
                }
            }
        }
    }

    return (fullTextFileArr);
}

/*
 *  ======== myCompare ========
 *  sorting function to compare symbol names
 */
function myCompare(a, b)
{
    var atokens = new Array();
    var btokens = new Array();
    atokens = a.split(/\s+/);
    btokens = b.split(/\s+/);
    if (atokens[2] > btokens[2]) return 1;
    if (atokens[2] == btokens[2]) return 0;
    if (atokens[2] < btokens[2]) return -1;
}

/*
 *  ======== myCompareSize ========
 *  sorting function to compare function sizes
 */
function myCompareSize(a, b)
{
    var atokens = new Array();
    var btokens = new Array();
    atokens = a.split(/\s+/);
    btokens = b.split(/\s+/);
    if (Number(atokens[1]) < Number(btokens[1])) return 1;
    if (Number(atokens[1]) == Number(btokens[1])) return 0;
    if (Number(atokens[1]) > Number(btokens[1])) return -1;
}

/*
 *  ======== buildRomTextFile ========
 */
function buildRomTextFile(mapFile, textFile)
{
    var textFileArr = new Array();
    var tokens = new Array();
    var mapIdx, virtualAddr, symbolName;
    var fullTextFileArr;

    var mapFileArr = readFile(mapFile);
    
    for (mapIdx = 0; mapIdx < mapFileArr.length; mapIdx++) {
        if (String(mapFileArr[mapIdx]).indexOf(".sysbios_rom_text") == 0) {
            mapIdx += 2; /* skip past the section summary line */
            while (String(mapFileArr[mapIdx]) != "") {
                if (String(mapFileArr[mapIdx]).match("text") ||
                    String(mapFileArr[mapIdx]).match("const")) {
                    tokens = String(mapFileArr[mapIdx++]).split(/\s+/);
                    symbolName = String(tokens[4]);
                    symbolName = symbolName.substring(symbolName.indexOf(":")+1,
                             symbolName.lastIndexOf(")"));
                    textFileArr[textFileArr.length] = "0x" + tokens[1] + " " + Number("0x" + tokens[2]) + " " + symbolName;
                }
                else {
                    mapIdx++;
                }
            }
        }
    }

    /* add in all the function aliases not shown in the .sysbios_rom_text section */
    /* naturally sorted by function address */
    fullTextFileArr = addFuncAliases(mapFile, textFileArr);

    /* copy and sort by function name */
    var sortedBySymbol = new Array;
    sortedBySymbol = fullTextFileArr.slice(0, fullTextFileArr.length);
    sortedBySymbol.sort(myCompare);
    /* Add "sorted by" banner */
    sortedBySymbol.unshift("", "Sorted by Symbol", "");

    /* copy and sort by function size */
    var sortedBySize = new Array;
    sortedBySize = fullTextFileArr.slice(0, fullTextFileArr.length);
    sortedBySize.sort(myCompareSize);
    /* Add "sorted by" banner */
    sortedBySize.unshift("", "Sorted by Size", "");

    /* Add "sorted by" banner */
    fullTextFileArr.unshift("", "Sorted by Address", "");

    /* write 3 sorted arrays out to file */
    writeFile(textFile, sortedBySymbol.concat(sortedBySize.concat(fullTextFileArr)));
}

/*
 *  ======== buildRomConstFile ========
 */
function buildRomConstFile(mapFile, constFile)
{
    var constFileArr = new Array();
    var tokens = new Array();
    var mapIdx, virtualAddr, symbolName;

    var mapFileArr = readFile(mapFile);
    
    for (mapIdx = 0; mapIdx < mapFileArr.length; mapIdx++) {
        if (String(mapFileArr[mapIdx]).indexOf(".sysbios_rom_const") == 0) {
            mapIdx += 2; /* skip past the section summary line */
            while (String(mapFileArr[mapIdx]) != "") {
                tokens = String(mapFileArr[mapIdx++]).split(/\s+/);
                symbolName = String(tokens[4]);
                symbolName = symbolName.substring(symbolName.indexOf(":")+1,
                             symbolName.lastIndexOf(")"));
                constFileArr[constFileArr.length] = tokens[1] + " " + tokens[2] + " " + symbolName;
            }
        }
    }
    writeFile(constFile, constFileArr);
}

/*
 *  ======== buildRomDataFile ========
 */
function buildRomDataFile(mapFile, dataFile)
{
    var dataFileArr = new Array();
    var tokens = new Array();
    var mapIdx, virtualAddr, symbolName;

    var mapFileArr = readFile(mapFile);
    
    for (mapIdx = 0; mapIdx < mapFileArr.length; mapIdx++) {
        if (String(mapFileArr[mapIdx]).indexOf(".sysbios_rom_data") == 0) {
            mapIdx += 2; /* skip past the section summary line */
            while (String(mapFileArr[mapIdx]) != "") {
                tokens = String(mapFileArr[mapIdx++]).split(/\s+/);
                symbolName = String(tokens[4]);
                symbolName = symbolName.substring(symbolName.indexOf(":")+1,
                             symbolName.lastIndexOf(")"));
                dataFileArr[dataFileArr.length] = tokens[1] + " " + tokens[2] + " " + symbolName;
            }
        }
    }
    writeFile(dataFile, dataFileArr);
}

function main(args)
{
    var romMapFile = args[0];
    var romTextFile = args[1];
    var romConstFile = args[2];
    var romDataFile = args[3];

    buildRomTextFile(romMapFile, romTextFile);
    buildRomConstFile(romMapFile, romConstFile);
    buildRomDataFile(romMapFile, romDataFile);
}
