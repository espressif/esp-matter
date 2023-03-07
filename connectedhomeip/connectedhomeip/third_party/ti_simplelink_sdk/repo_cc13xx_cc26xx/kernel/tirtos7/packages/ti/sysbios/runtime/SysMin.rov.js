/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 *  ======== SysMin.rov.js ========
 */

var moduleName = "ti.sysbios.runtime::SysMin";

var viewMap = [
    {name: "Module", fxn: "viewSysMinModule", structName: "ModuleInfo"},
    {name: "Output Buffer", fxn: "viewInitOutputBuffer", structName: "Line"}
];

/*
 * BIOS Module Information
 */
function ModuleInfo()
{
    this.address      = null;
    this.OutputBuffer = null;
    this.outBufSize   = null;
    this.outBufIndex  = 0;
    this.wrapped      = 0;
}


/* Line constructor */
function Line() {
    this.entry = null;
}

/*
 *  ======== viewSysMinModule ========
 */
function viewSysMinModule()
{
    var modView = new ModuleInfo();

    try {
        var modState = Program.fetchVariable("ti_sysbios_runtime_SysMin_Module_state");
    }
    catch (e) {
        return (modView);
    }

    modView.address = toHexString(Program.lookupSymbolValue("ti_sysbios_runtime_SysMin_Module_state"));
    modView.OutputBuffer = modState.outbuf;
    modView.outBufSize = modState.bufsize;
    modView.outBufIndex = modState.outidx;
    modView.wrapped = modState.wrapped ? "true" : "false";

    return (modView);
}

/*
 * ======== toHexString ========
 * converts a number to a hex string with "0x" prefix.
 */
function toHexString(num) {
    return ("0x" + Number(num).toString(16));
}

/*
 *  ======== viewInitOutputBuffer ========
 *  Displays the contents of SysMin's output buffer in ROV.
 */
function viewInitOutputBuffer()
{
    var view = new Array();

    var modState = Program.fetchVariable("ti_sysbios_runtime_SysMin_Module_state");

    /*
     * If the buffer has not wrapped and the index of the next character to
     * write is 0, then the buffer is empty, and we can just return.
     */
    if (!modState.wrapped && (modState.outidx == 0)) {
        var line = new Line();
        view.push(line);
        return (view);
    }

    /* Get the buffer size from the configuration. */
    var bufSize = modState.bufsize;
    var bufAddr = Program.lookupSymbolValue("ti_sysbios_runtime_SysMin_outbuf");

    /* Read in the outbuf */
    var outbuf;

    try {
        outbuf = Program.fetchFromAddr(bufAddr, "uint8_t", bufSize);
    }
    /* If there's a problem, just re-throw the exception. */
    catch (e) {
        var line = new Line();
        line.entry = e;
        view.push(line);
        return (view);
    }

    /* Leftover characters from each read which did not end in a newline. */
    var leftover = "";

    /* If the output buffer has wrapped... */
    if (modState.wrapped) {
        /* Read from outidx to the end of the buffer. */
        var leftover = readChars("", outbuf, modState.outidx,
                                 outbuf.length - 1, view);
    }

    /* Read from the beginning of the buffer to outidx */
    leftover = readChars(leftover, outbuf, 0, modState.outidx - 1,
                         view);

    /*
     * If there are any leftover characters not terminated in a newline,
     * create an element for these and display them.
     */
    if (leftover != "") {
        var elem = new Line();
        elem.entry = leftover;
        view[view.length] = elem;
    }

   return (view);
}

/*
 *  ======== readChars ========
 *  Reads characters from 'buffer' from index 'begin' to 'end' and adds
 *  any newline-terminated strings as elements to the 'elements' array.
 *  If the last character is not a newline, this function returns what it's
 *  read from the "incomplete" string.
 *  The string 'leftover', the leftover incomplete string from the previous
 *  call, is prepended to the first string read.
 */
function readChars(leftover, buffer, begin, end, view)
{
    /* Start with the previous incomplete string. */
    var str = leftover;

    /* For each of the specified characters... */
    for (var i = begin; i <= end; i++) {

        /* Convert the target values to characters. */
        var ch = String.fromCharCode(buffer[i]);

        /* Add the character to the current string. */
        str += ch;

        /*
         * If a string ends in a newline, create a separate table entry for it.
         */
        if (ch == '\n') {
            /*
             * Create a view structure to display the string, and add
             * it to the table.
             */
            var elem = new Line();

            elem.entry = str;
            view[view.length] = elem;

            /* Reset the string */
            str = "";
        }
    }

    /* Return any unfinished string characters. */
    return (str);
}
