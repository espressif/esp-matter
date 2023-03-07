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
 */

/*
 *  ======== LoggerText.rov.js ========
 */

var Program = xdc.module('xdc.rov.Program');
var Monitor = xdc.module('xdc.rov.runtime.Monitor');

var moduleName = "LoggerText";

var viewMap = [
    {
        name: "Instance",
        fxn: "viewInitInstance",
        structName: "InstanceView"
    },
    {
        name: "Records",
        fxn: "viewInitRecord",
        structName: "RecordView",
        argsName: "InstanceIndex"
    }
];

var argsMap = [
    {
        name: "InstanceIndex",
        description: "Instance Index",
        args: [
            { name: "Index", type: "number",  defaultValue: "1" }
        ]
    }
];

function InstanceView()
{
    this.Index = 1;
    this.Label = "";
    this.Serial = 0;
    this.NumEntries = 0;
    this.Type = "";
}

function viewInitInstance()
{
//  Monitor.println("LoggerText: viewInitInstance");

    var count = Program.fetchVariable("LoggerText_count");
    var config = Program.fetchVariable("LoggerText_config");
    var table = new Array();

    var typespecInst = Program.lookupType("LoggerText_Instance");

    for (var i = 0; i < count; i++) {
        var view = new InstanceView();
        var addr = Program.lookupSymbolValue("LoggerText_config") +
            (i * typespecInst.size);
        var inst = Program.fetchFromAddr(addr, "LoggerText_Instance");
        view.Index = i + 1;
        view.Label = Program.fetchString(inst.label, false);
        view.Serial = inst.serial;
        view.NumEntries = inst.numEntries;
        view.Type = (inst.bufType == 101 ? "Circular" : "Linear");
        table.push(view);
    }

    return (table);
}

function RecordView()
{
    this.Serial = 0;
    this.Timestamp = 0;
    this.Text = "";
}

function viewInitRecord(argstr)
{
//  Monitor.println("LoggerText: viewInitRecord, argstr=" + argstr);

    var addr, count, i, hdr, text, rec;

    /* extract arguments from arg string */
    var args = argstr.split(",");
    var index = Number(args[0]) - 1;

    count = Program.fetchVariable("LoggerText_count");
    var config = Program.fetchVariable("LoggerText_config");
    var table = new Array();

    if (index > count) return (table);

    /* read requested instance from target memory */
    var tsInst = Program.lookupType("LoggerText_Instance");
    addr = Program.lookupSymbolValue("LoggerText_config") +
        (index * tsInst.size);
    var inst = Program.fetchFromAddr(addr, "LoggerText_Instance");

    /* check for empty log store */
    if (inst.serial == 0) return (table);

    /* read instance log store from target memory */
    var storeAddr = inst.store;
    var tsHeader = Program.lookupType("LoggerText_Header");
    var recSize = tsHeader.size + inst.textLen;

    if (inst.bufType == 102) { /* fixed */
        for (i = 0; i < inst.serial; i++) {
            addr = inst.store + (i * recSize);
            hdr = Program.fetchFromAddr(addr, "LoggerText_Header");
            text = Program.fetchString(addr + tsHeader.size, false);

            rec = new RecordView();
            rec.Serial = hdr.serial;
            rec.Timestamp = ((4294967296 * hdr.ts_hi) + hdr.ts_lo);
            rec.Text = text;
            table.push(rec);
        }
    }
    else { /* circular */
        if (inst.serial < inst.numEntries) {
            addr = inst.store;
            count = inst.serial;
        }
        else {
            addr = (inst.curEntry == inst.endEntry ? inst.store :
                inst.curEntry + recSize);
            count = inst.numEntries - 1;
        }

        for (i = 0; i < count; i++) {
            hdr = Program.fetchFromAddr(addr, "LoggerText_Header");
            text = Program.fetchString(addr + tsHeader.size, false);

            rec = new RecordView();
            rec.Serial = hdr.serial;
            rec.Timestamp = ((4294967296 * hdr.ts_hi) + hdr.ts_lo);
            rec.Text = text;
            table.push(rec);

            addr = (addr == inst.endEntry ? inst.store : addr + recSize);
        }
    }

    return (table);
}
