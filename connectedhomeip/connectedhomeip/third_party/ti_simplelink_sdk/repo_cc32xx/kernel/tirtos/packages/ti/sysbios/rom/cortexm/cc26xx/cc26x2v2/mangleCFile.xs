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
        line = String(line);
        /* suppress blank lines */
        if (line.length != 0) {
            lineArr[idx++] = line;
        }
    }

    return lineArr;
}

function main(args)
{
    var sourceFile = args[0];
    var destFile = args[1];
    var funcsFile = args[2];
    var funcs = String(args[3]);

    var funcsRegEx;
    var srcLine;
    var matchIndex, funcType;
    var srcIdx, funcsIdx;
    var result;

    var mangle;

    var srcFile = new java.io.BufferedReader(new java.io.FileReader(sourceFile));
    var dstFile = new java.io.FileWriter(destFile);
    var symbolMap = args[0] + "_SYMS=";

    if (args[3] === undefined) {
        var funcsArr = readFile(funcsFile);
        funcsRegEx = new RegExp(funcsArr.join("|"));
    }
    else {
        funcsRegEx = new RegExp(funcs);
    }

    var linesProcessed = 0;  
    var pastRomStructsDefinition = false;
	
    /* Read file */
    while ((srcLine = srcFile.readLine()) != null) {
        srcLine = String(srcLine);
        if (funcsRegEx.source != "") {
            var firstChar = srcLine[0];
            var firstChars = srcLine.substring(0, 2);
            /* lines must be at least "ti_sysbios_BIOS" in length */
            if ((srcLine.length > 14) &&
                (firstChars != "ex") &&
                (firstChars != "st") &&
                (firstChars != "if") &&
                (firstChars != "ty") &&
                (firstChars != "# ") &&
                (firstChars != "#l") &&
                (firstChars != "en") &&
                (firstChar != " ") &&
                (firstChar != "}")) {
                linesProcessed++;
                result = funcsRegEx.exec(srcLine);
                if (result != null) {
		            /* mangle the function name so it won't be found in the sysbios lib */
		            srcLine = srcLine.replace(result[0], result[0]+"__mangled__");
                    symbolMap += result[0] +"|";
	            }
            }
			else {
                if (pastRomStructsDefinition == true) {
				    if ((firstChar == " ")) {
                        result = funcsRegEx.exec(srcLine);
                        if (result != null) {
		                    /* mangle the function name so it won't be found in the sysbios lib */
//print(srcLine);						
		                    srcLine = srcLine.replace(result[0], "(xdcRomExternFuncPtr->" + result[0] + ")");
//print(srcLine);						
	                    }
                    }
				}
				else {
				    if (srcLine.match(" ROM_ExternFuncPtrs;")) pastRomStructsDefinition = true;
				}
			}
        }
        dstFile.write(srcLine + "\n");
    }

    /* write matching symbol maps to common output file */
    if (args[4] !== undefined) {
        if (symbolMap.lastIndexOf("|") != -1) {
            symbolMap = symbolMap.substring(0,symbolMap.lastIndexOf("|"));
        }
        else {
            symbolMap += "\"\"";
        }
        var symbolMapFile = new java.io.FileWriter(args[4], true);
        symbolMapFile.write(symbolMap + "\n");
        symbolMapFile.close(); 
    }

//print(sourceFile, linesProcessed);
    dstFile.flush();
    srcFile.close();
    dstFile.close();
}
