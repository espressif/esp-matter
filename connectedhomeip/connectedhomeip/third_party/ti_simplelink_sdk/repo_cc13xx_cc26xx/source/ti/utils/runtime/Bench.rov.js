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
 *  ======== Bench.rov.js ========
 */

var Program = xdc.module('xdc.rov.Program');
var Monitor = xdc.module('xdc.rov.runtime.Monitor');

var moduleName = "Bench";
var Bench_Detail_STATE = 1;
var Bench_Detail_INSTANCE = 2;
var Bench_Detail_BUFFER = 3;
var Bench_Detail_ALL = 4;
var Bench_MAX_COUNT = 24; /* hack to manage constructor issue */
var Bench_TRIM = 8.0;

var viewMap = [
    {
        name: "Module",
        fxn: "viewInitModule",
        structName: "ModuleView"
    },
    {
        name: "Instance",
        fxn: "viewInitInstance",
        structName: "InstanceView"
    },
    {
        name: "Delta",
        fxn: "viewInitDelta",
        structName: "DeltaView",
        argsName: "DeltaArgs"
    },
    {
        name: "Report",
        fxn: "viewInitReport",
        structName: "ReportView"
    },
    {
        name: "Buffer",
        fxn: "viewInitBuffer",
        structName: "BufferView",
        argsName: "InstanceAddr"
    }
];

var argsMap = [
    {
        name: "InstanceAddr",
        description: "Instance Address",
        args: [
            { name: "Address", type: "number",  defaultValue: "0" }
        ]
    },
    {
        name: "DeltaArgs",
        description: "Delta View Settings",
        args: [
            { name: "msec", type: "boolean",  defaultValue: "false" },
            { name: "trim", type: "number",  defaultValue: "8.0" }
        ]
    }
];

function ReportView()
{
    this.Segment = "";
    this.Ticks = 0;
    this.Usecs = 0;
}

function viewInitReport()
{
//  Monitor.println("Bench: viewInitReport");
    var i;
    var data = Bench_fetchData();
    var table = new Array(data.count - 1);

    /* initialize table with segment names */
    var labels = segment_labels(data);

    for (i = 0; i < (data.count - 1); i++) {
        var view = new ReportView();
        view.Segment = labels[i];
        view.Ticks = 0;
        view.Usecs = 0;
        table[i] = view;
    }

    /* compute overall average of each segment */
    var mean = arithmetic_mean(data);

    /* compute the truncated mean */
    var avg = new Array(data.count - 1);
    var num = new Array(data.count - 1);

    for (i = 0; i < (data.count - 1); i++) {
        avg[i] = 0;
        num[i] = data.numEntries;
    }
    for (i = 1; i < data.count; i++) {
        for (var r = 0; r < data.numEntries; r++) {
            var x0 = data.inst[i-1].samples[r].tick;
            var x1 = data.inst[i].samples[r].tick;
            var dt = x1 - x0; /* delta tick */

            /* discard outlying samples */
            if (Math.abs(dt) > ((100 + Bench_TRIM)/100.0 * mean[i-1])) {
                num[i-1]--;
            }
            else {
                avg[i-1] += dt; /* compute sum of all samples */
            }
        }
    }
    /* divide the sum by number of remaining samples */
    for (i = 0; i < data.count - 1; i++) {
        avg[i] = Math.round(avg[i] / num[i]);
    }

    /* generate report */
    var totalTicks = 0;

    for (i = 0; i < (data.count - 1); i++) {
        table[i].Ticks = avg[i];
        table[i].Usecs = round(avg[i] * data.usec_tick, 3);
        totalTicks += table[i].Ticks;
    }

    /* add final row to present total execution count */
    var sepView = new ReportView();
    sepView.Segment = "--------";
    sepView.Ticks = "----";
    sepView.Usecs = "----";
    table.push(sepView);

    var totalView = new ReportView();
    totalView.Segment = "Total";
    totalView.Ticks = totalTicks;
    totalView.Usecs = round(totalTicks * data.usec_tick, 3);
    table.push(totalView);

    return (table);
}

function ModuleView()
{
    this.TimestampHZ = 0;
    this.NumInstances = 0;
    this.NumEntries = 0;
    this.Trim = 0;
}

function viewInitModule()
{
//  Monitor.println("Bench: viewInitModule");

    var data = Bench_fetchData(Bench_Detail_STATE);
    var table = new Array();
    var view = new ModuleView();

    view.TimestampHZ = data.tsHz;
    view.NumInstances = data.count;
    view.NumEntries = data.numEntries;
    view.Trim = Bench_TRIM;
    table.push(view);

    return (table);
}

function InstanceView()
{
    this.BufAddr = 0;
    this.Label = "";
    this.Proxy = "false";
}

function viewInitInstance()
{
    var data = Bench_fetchData(Bench_Detail_INSTANCE);
    var table = new Array();

    for (var i = 0; i < data.count; i++) {
        var inst = data.inst[i];
        var view = new InstanceView();
        view.BufAddr = inst.buffer;
        view.Label = inst.label;
        view.Proxy = (inst.proxy ? "true" : "false");
        table.push(view);
    }

    return (table);
}

function BufferView()
{
    this.Serial = 0;
    this.ClockTick = 0;
    this.Timestamp_msec = 0;
    this.Delta_msec = 0;
}

function viewInitBuffer(argstr)
{
//  Monitor.println("Bench: viewInitBuffer, argstr=" + argstr);

    var i;

    /* extract arguments from arg string */
    var args = argstr.split(",");
    var addr = args[0];

    /* find instance by its given address */
    var data = Bench_fetchData(Bench_Detail_INSTANCE);
    var index = -1;
    for (i = 0; i < data.count; i++) {
        if (addr == data.inst[i].buffer) {
            index = i;
            break;
        }
    }

    /* build view table with instance buffer data */
    var table = new Array();
    if (index < 0) {
        /* TODO - report error */
        return (table);
    }
    data = Bench_fetchData(Bench_Detail_BUFFER, index);

    var tick = 0;
    var last = 0;
    var dtick = 0;
    var inst = data.inst[index];

    for (i = 0; i < data.numEntries; i++) {
        var rec = new BufferView();
        rec.Serial = i + 1;
        tick = inst.samples[i].tick;
        rec.ClockTick = tick;
        rec.Timestamp_msec = round(inst.samples[i].ts_msec, 6);
        if (i == 0) {
            last = tick;
        }
        dtick = (last <= tick ? tick - last : (tick + 4294967296) - last);
        rec.Delta_msec = round((data.usec_tick * dtick) / 1000, 3);
        last = tick;
        table.push(rec);
    }

    return (table);
}

function DeltaView(count)
{
    /* function default value */
    count = (count === undefined ? Bench_MAX_COUNT : count);

    this.Serial = 0;

    for (var i = 1; i <= count; i++) {
        this["Mark" + i] = 0;
    }
}

function viewInitDelta(argstr)
{
//  Monitor.println("Bench: viewInitDelta, argstr=" + argstr);

    var i, j;

    /* extract arguments from arg string */
    var args = argstr.split(",");
    var msec = (args[0] == "false" ? false : true);
    var trim = Number(args[1]);
    Bench_TRIM = trim;

    var data = Bench_fetchData(Bench_Detail_ALL);
    var mean = arithmetic_mean(data);
    var table = new Array();

    /* add header row containing segment names */
    var labels = segment_labels(data);
    var header = new DeltaView(data.count);
    header.Serial = "";
    header.Mark1 = "";

    for (i = 2; i <= data.count; i++) {
        header["Mark" + i] = labels[i-2];
    }
    table.push(header);

    /* build the table one row at a time */
    for (i = 0; i < data.numEntries; i++) {

        /* collect the samples for current row */
        var tsAry = new Array(data.count);
        for (j = 0; j < data.count; j++) {
            if (data.inst[j].buffer == 0) {
                tsAry[j] = 0; /* null proxy instance */
            }
            else {
                tsAry[j] = data.inst[j].samples[i].tick;
            }
        }

        /* compute delta for each segment on the current row */
        var deltaCnt = data.count - 1;
        var deltaAry = new Array(deltaCnt);

        for (j = 1; j < data.count; j++) {
            var dx_tick = tsAry[j] - tsAry[j-1];
            /* apply trim indicator if delta exceeds trim value */
            var t = Math.abs(dx_tick) > ((100 + trim)/100.0 * mean[j-1]);
            deltaAry[j-1] = { tick: dx_tick, trim: t };
            if (msec) {
                var dx_usec = round(dx_tick * data.usec_tick, 3);
                deltaAry[j-1].usec = dx_usec;
            }
        }

        /* create one row of data for view */
        var rec = new DeltaView(data.count);
        rec.Serial = i + 1;
        rec.Mark1 = (msec ? "0 (0.00)" : "0");

        for (j = 0; j < deltaCnt; j++) {
            rec["Mark"+(j+2)] = "" + deltaAry[j].tick;
            if (msec) {
                rec["Mark"+(j+2)] += " ("+deltaAry[j].usec+")";
            }
            if (deltaAry[j].trim) {
                rec["Mark"+(j+2)] += " !";
            }
        }

        table.push(rec);
    }

    return (table);
}

function Bench_fetchData(detail, bufId)
{
//  Monitor.println("Bench: Bench_fetchData");

    var inst, addr;

    /* function default value */
    detail = (detail === undefined ? Bench_Detail_ALL : detail);

    var data = {};

    /* fetch state information */
    var state = Program.fetchVariable("Bench_state");
    data.numEntries = state.numEntries;
    data.tsHz = state.timestampHz;
    data.usec_tick = 1000000 / data.tsHz;
    data.count = Program.fetchVariable("Bench_count");

    if (detail == Bench_Detail_STATE) return (data);

    var typespecInst = Program.lookupType("Bench_Instance");
    data.inst = new Array(data.count);

    /* fetch instance information */
    for (var i = 0; i < data.count; i++) {
        addr = Program.lookupSymbolValue("Bench_config") +
            (i * typespecInst.size);
        var benchInst = Program.fetchFromAddr(addr, "Bench_Instance");

        inst = {};
        inst.addr = addr;
        inst.label = Program.fetchString(benchInst.label, false);
        inst.proxy = benchInst.proxy;
        inst.buffer = benchInst.buffer;
        data.inst[i] = inst;
    }

    /* sort instance array alphabetically by label */
    data.inst.sort(function(a, b) {
        if (a.label < b.label) {
            return -1;
        }
        else if (a.label > b.label) {
            return 1;
        }
        else {
            return 0;
        }
    });

    if (detail == Bench_Detail_INSTANCE) return (data);

    var instId = 0;

    if (detail == Bench_Detail_BUFFER) {
        if (bufId === undefined) {
            Monitor.println("Error: Bench_fetchData() bufId is undefined");
        }
        else {
            instId = bufId;
        }
    }

    /* fetch benchmark samples for all instances or just one */
    do {
        inst = data.inst[instId];
        inst.samples = new Array(data.numEntries);
        for (var r = 0; r < data.numEntries; r++) {
            addr = inst.buffer + (r * 4);
            var tick = Program.fetchFromAddr(addr, "uint32_t");
            var ts_msec = (data.usec_tick * tick) / 1000;
            inst.samples[r] = { tick: tick, ts_msec: ts_msec };
        }
        ++instId;
    } while ((instId < data.count) && (detail != Bench_Detail_BUFFER));

    return (data);
}

function round(num, prec)
{
    var m = Math.pow(10, prec);
    return (Math.round(num * m) / m);
}

/*
 *  ======== arithmetic_mean ========
 *  Compute the average of all samples for each segment
 */
function arithmetic_mean(data)
{
    var i;
    var mean = new Array(data.count - 1);

    for (i = 0; i < (data.count - 1); i++) {
        mean[i] = 0;
    }

    /* compute the sum of all samples for each segment */
    for (i = 1; i < data.count; i++) {
        for (var r = 0; r < data.numEntries; r++) {
            var x0 = data.inst[i-1].samples[r].tick;
            var x1 = data.inst[i].samples[r].tick;
            mean[i-1] += x1 - x0;
        }
    }

    /* compute the mean of each segment */
    for (i = 0; i < (data.count - 1); i++) {
        mean[i] /= data.numEntries;
    }

    return (mean);
}

/*
 *  ======== segment_labels ========
 *  Use mark-point labels to compute each segment name
 */
function segment_labels(data)
{
    var labels = new Array(data.count - 1);
    var label_prev = data.inst[0].label;

    for (var i = 1; i < data.count; i++) {
        var label = data.inst[i].label;
        labels[i-1] = label_prev + ":" + label;
        label_prev = label;
    }

    return (labels);
}
