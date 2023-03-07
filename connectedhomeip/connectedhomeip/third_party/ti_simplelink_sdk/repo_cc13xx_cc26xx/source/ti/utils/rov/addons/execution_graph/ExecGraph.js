/*
 *  ======== ExecGraph.js ========
 *  Implement the execution graph path computation engine
 *
 *  Object Model
 *  ------------
 *  exec_data
 *  {
 *      ts_freq: Number                 Hz
 *      adapter: {
 *          count: Number
 *      }
 *      threads: {                      thread database (includes hwi, swi)
 *          stack: []
 *          lastTask_h:
 *          mode: String
 *          running:                    running task (index into tasks[])
 *          y_val:                      y-axis value of running task
 *          tasks: [{
 *              label: String           "Idl: 0x########"
 *              hndl: String            "0x########"
 *              type: Number            0 kernel, 1 task, 2 swi, 3 hwi
 *              pri: Number             -1
 *              stack: []               call stack
 *          }]
 *      }
 *
 *      filters: [
 *          {
 *              logger: String          "xdc.runtime.LoggerBuf"
 *              name: String            "ti.sysbios.knl.Task"
 *              fxn: Object             fxn(logEvent) {...}
 *          }
 *      ]
 *
 *      logs: [{                        collated array of all log sources
 *          seq: Number                 sequence number (local)
 *          ts: Number                  timestamp
 *          label: String
 *          type: String                event type
 *          y: Number                   -1
 *      }]
 *  }
 *
 *  event_type
 *  {
 *      Task_Block: {
 *          hndl: String                task handle
 *      }
 *      Task_Ready: {
 *          hndl: String                task handle
 *      }
 *      Task_Switch: {
 *          hndl_new: String            task handle (incoming)
 *          hndl_old: String            task handle (outgoing)
 *      }
 *      Swi_Begin: {
 *          hndl: String                hwi handle
 *      }
 *      Swi_End:
 *          hndl: String                hwi handle
 *      }
 *      Hwi_Begin: {
 *          hndl: String                hwi handle
 *      }
 *      Hwi_End: {
 *          hndl: String                hwi handle
 *      }
 *      Data: {
 *          label: String               format string
 *      }
 *  }
 */

/*
 *  ======== ExecGraph_collate ========
 *  Collate events from all log sources
 */
function ExecGraph_collate(exec_data, logs_in)
{
    var logs_out = new Array();
    var data_in = new Array();

    /* make sure new logs are not empty */
    if (logs_in.length == 0) {
        return;
    }

    /* setup data source trackers */
    data_in.push({
        di: 0,
        elems: logs_in
    });

    if (exec_data.logs.length > 0) {
        data_in.push({
            di: 0,
            elems: exec_data.logs
        });
    }

    var evt_next = {
        si: -1,                 /* source index */
        ts: Number.MAX_VALUE    /* timestamp */
    };

    var li = 0;         /* log_out[] index */
    var ts = 0;         /* event timestamp */

    var working = data_in.length;

    while (working) {
        /* find next event */
        for (var si = 0; si < data_in.length; si++) {
            if (data_in[si].di >= 0) {
                ts = data_in[si].elems[data_in[si].di].ts;
                if (ts < evt_next.ts) {
                    evt_next.si = si;
                    evt_next.ts = ts;
                }
            }
        }

        /* consume next event */
        var di = data_in[evt_next.si].di;
        var evt = data_in[evt_next.si].elems[di];
        ++(data_in[evt_next.si].di);
        if (data_in[evt_next.si].di == data_in[evt_next.si].elems.length) {
            data_in[evt_next.si].di = -1; /* data source empty */
            --working;
        }
        evt_next.ts = Number.MAX_VALUE;

        /* output event */
        logs_out[li++] = evt;
    }

    /* replace old logs with new collated logs */
    exec_data.logs = logs_out;
    return;
}

function ExecGraph_collate_1(logs_in, moduleViewName, viewData)
{
    var logs_out = new Array();
    var data_in = new Array();

    /* make sure view data is not empty */
    if ((moduleViewName != "xdc.runtime.LoggerBuf.Records") &&
        (viewData.elements.length == 0)) {
        return (logs_in);
    }

    /* setup data source trackers */
    if (logs_in.length > 0) {
        data_in.push({
            name: "logs",
            di: 0,
            elems: logs_in
        });
    }

    if (moduleViewName == "xdc.runtime.LoggerBuf.Records") {
        for (var i = 0; i < viewData.length; i++) {
            data_in.push({
                name: moduleViewName,
                di: 0,
                elems: viewData[i].elements
            });
        }
    }
    else if (moduleViewName.match(/^C.LoggerText.Records_\d+/)) {
        data_in.push({
            name: moduleViewName,
            di: 0,
            elems: viewData.elements
        });
    }
    else if (moduleViewName == "C.LoggerNDK.Records") {
        data_in.push({
            name: moduleViewName,
            di: 0,
            elems: viewData.elements
        });
    }

    var evt_next = {
        si: -1,                 /* source index */
        ts: Number.MAX_VALUE    /* timestamp */
    };

    var li = 0;         /* log_out[] index */
    var ts = 0;         /* event timestamp */

    var working = data_in.length;
    while (working) {
        /* find next event */
        for (var si = 0; si < data_in.length; si++) {
            if (data_in[si].di >= 0) {
                switch (data_in[si].name) {
                    case "logs":
                    ts = data_in[si].elems[data_in[si].di].ts;
                    break;
                    case "xdc.runtime.LoggerBuf.Records":
                    ts = Number(data_in[si].elems[data_in[si].di].timestampRaw);
                    break;
                    case "C.LoggerText.Records_1":
                    ts = Number(data_in[si].elems[data_in[si].di].Timestamp);
                    break;
                    case "C.LoggerText.Records_2":
                    ts = Number(data_in[si].elems[data_in[si].di].Timestamp);
                    break;
                    case "C.LoggerText.Records_3":
                    ts = Number(data_in[si].elems[data_in[si].di].Timestamp);
                    break;
                    case "C.LoggerText.Records_4":
                    ts = Number(data_in[si].elems[data_in[si].di].Timestamp);
                    break;
                    case "C.LoggerText.Records_5":
                    ts = Number(data_in[si].elems[data_in[si].di].Timestamp);
                    break;
                    case "C.LoggerNDK.Records":
                    ts = Number(data_in[si].elems[data_in[si].di].Timestamp);
                    break;
                }

                if (ts < evt_next.ts) {
                    evt_next.si = si;
                    evt_next.ts = ts;
                }
            }
        }

        /* consume next event */
        var di = data_in[evt_next.si].di;
        var evt = data_in[evt_next.si].elems[di];
        var name = data_in[evt_next.si].name;
        ++(data_in[evt_next.si].di);
        if (data_in[evt_next.si].di == data_in[evt_next.si].elems.length) {
            data_in[evt_next.si].di = -1; /* data source empty */
            --working;
        }
        evt_next.ts = Number.MAX_VALUE;

        /* output event */
        switch (name) {
            case "logs":
                logs_out[li] = evt;
                break;
            case "xdc.runtime.LoggerBuf.Records":
                logs_out[li] = {
                    serial: evt.serial,
                    ts: Number(evt.timestampRaw),
                    label: ExecGraph_eventLabel(evt),
                    eventName: evt.eventName,
                    arg0: evt.arg0,
                    arg2: evt.arg2,
                    y: -1
                };
                break;
            case "C.LoggerText.Records_1":
            case "C.LoggerText.Records_2":
            case "C.LoggerText.Records_3":
            case "C.LoggerText.Records_4":
            case "C.LoggerText.Records_5":
                logs_out[li] = {
                    serial: evt.Serial,
                    ts: Number(evt.Timestamp),
                    label: evt.Text,
                    eventName: name,
                    arg0: 0,
                    arg2: 0,
                    y: -1
                };
                break;
            case "C.LoggerNDK.Records":
                logs_out[li] = {
                    serial: evt.Serial,
                    ts: Number(evt.Timestamp),
                    label: evt.Text,
                    eventName: "LoggerNDK",
                    arg0: 0,
                    arg2: 0,
                    y: -1
                };
                break;
        }
        ++li;
    }

    return (logs_out);
}

/*
 *  ======== ExecGraph_computePath ========
 *  Replay log events and compute thread states
 */
function ExecGraph_computePath(exec_data)
{
    /* initialize */
    ExecGraph_createProxyThreads(exec_data);
    ExecGraph_sortThreads(exec_data);
    exec_data.threads.running = ExecGraph_findRunningTask(exec_data);

    /* compute y-axis value for each log event */
    for (var i = 0; i < exec_data.logs.length; i++) {
        y_val = ExecGraph_taskValY(i, exec_data.threads, exec_data.logs);
        exec_data.logs[i].y = y_val + 1;
    }
}

/*
 *  ======== ExecGraph_createProxyThreads ========
 *  Create proxy threads for orphan log events
 *
 *  Scan the logs looking for events from thread objects which are
 *  not in the database. For example, constructed Swi objects will
 *  not be reported by the Swi ROV view, but they may have raised
 *  log events. Create proxy instances for these orphan events.
 */
function ExecGraph_createProxyThreads(exec_data)
{
    for (var e = 0; e < exec_data.logs.length; e++) {
        var evt = exec_data.logs[e];

        if (evt.type == "Swi_Begin") {
            if (ExecGraph_getThread(exec_data, 2, evt.hndl) == null) {
                exec_data.threads.tasks.push({
                    label: "Swi: proxy " + evt.hndl,
                    hndl: evt.hndl,
                    type: 2,
                    pri: -1
                });
            }
        }
    }
}

/*
 *  ======== ExecGraph_eventLabel ========
 */
function ExecGraph_eventLabel(evt)
{
    var label = "";

    if (evt.eventName.match(/^ti\.sysbios\..+\.Hwi\.LM_begin/)) {
        label = "Hwi begin: 0x" + Number(evt.arg0).toString(16);
    }
    else if (evt.eventName.match(/^ti\.sysbios\..+\.Hwi\.LD_end/)) {
        label = "Hwi end: 0x" + Number(evt.arg0).toString(16);
    }
    else {
        switch (evt.eventName) {
            case "ti.sysbios.knl.Clock.LM_begin":
                label = "Clock run: 0x" + Number(evt.arg0).toString(16);
                break;
            case "ti.sysbios.knl.Clock.LM_tick":
                label = "Clock tick: " + evt.arg0;
                break;
            case "ti.sysbios.knl.Swi.LM_begin":
                label = "Swi begin: 0x" + Number(evt.arg0).toString(16);
                break;
            case "ti.sysbios.knl.Swi.LD_end":
                label = "Swi end: 0x" + Number(evt.arg0).toString(16);
                break;
            case "ti.sysbios.knl.Swi.LM_post":
                label = "Swi post: 0x" + Number(evt.arg0).toString(16);
                break;
            case "ti.sysbios.knl.Task.LD_block":
                label = "Task block: 0x" + Number(evt.arg0).toString(16);
                break;
            case "ti.sysbios.knl.Task.LD_ready":
                label = "Task ready: 0x" + Number(evt.arg0).toString(16);
                break;
            case "ti.sysbios.knl.Task.LM_setPri":
                label = "Task setPri: 0x" + Number(evt.arg0).toString(16) +
                    ", old=" + evt.arg2 + ", new=" + evt.arg3;
                break;
            case "ti.sysbios.knl.Task.LM_switch":
                label = "Task switch: 0x" + Number(evt.arg2).toString(16);
                break;
            case "xdc.runtime.Log_print":
                label = evt.text;
                break;
            default:
                label = evt.text;
                break;
        }
    }

    return (label);
}

/*
 *  ======== ExecGraph_hwiInfo ========
 *  Store swi info for render phase
 *
 *  viewData [in]: ti.sysbios.knl.Swi.BasicView
 *
 *  task: Object {
 *      label: String task.label
 *      hndl: String "0x" + task.address.toString(16)
 *      pri: Number 
 *      type: 0 kernel, 1 task, 2 swi, 3 hwi
 *  }
 */
function ExecGraph_hwiInfo(threadDB, viewData)
{
    var label = "";

    for (var i = 0; i < viewData.length; i++) {
        var hwi = viewData[i];

        if (hwi.label != "") {
            label = "Hwi: " + hwi.label;
        }
        else if (hwi.fxn.match(/Timer_isrStub/i)) {
            label = "Hwi: Timer isrStub";
        }
        else {
//          label = "Hwi: " + hwi.address;
            label = "Hwi: " + hwi.fxn;
        }

        /* create thread object */
        var thread = {
            label: label,
            hndl: hwi.address,
            pri: Number(hwi.priority),
            type: 3
        };

        /* find insertion point for hwi */
        var idx;
        for (idx = 1; idx < threadDB.tasks.length; idx++) {
            if ((threadDB.tasks[idx].type == thread.type) &&
                    (threadDB.tasks[idx].pri > thread.pri)) {
                /* insert lower priority before higher one */
                break;
            }
        }

        /* insert new thread at position array[idx] */
        threadDB.tasks.splice(idx, 0, thread);
    }
}

/*
 *  ======== ExecGraph_swiInfo ========
 *  Store swi info for render phase
 *
 *  viewData [in]: ti.sysbios.knl.Swi.BasicView
 *
 *  task: Object {
 *      label: String task.label
 *      hndl: String "0x" + task.address.toString(16)
 *      pri: Number 
 *      type: 0 kernel, 1 task, 2 swi, 3 hwi
 *  }
 */
function ExecGraph_swiInfo(threadDB, viewData)
{
    var label = "";

    for (var i = 0; i < viewData.length; i++) {
        var swi = viewData[i];

        if (swi.label != "") {
            label = "Swi: "+ swi.label;
        }
        /* TODO - why is fxn an array? */
        else if (swi.fxn[0].match(/Clock_workFunc/i)) {
            label = "Swi: Clock workFunc";
        }
        else {
//          label = "Swi: " + swi.address;
            label = "Swi: " + swi.fxn[0];
        }

        /* create thread object */
        var thread = {
            label: label,
            hndl: swi.address,
            pri: Number(swi.priority),
            type: 2
        };

        /* find insertion point for swi */
        var idx;
        for (idx = 1; idx < threadDB.tasks.length; idx++) {
            if (threadDB.tasks[idx].type > thread.type) {
                /* insert swi before hwi */
                break;
            }
            else if ((threadDB.tasks[idx].type == thread.type) &&
                    (threadDB.tasks[idx].pri > thread.pri)) {
                /* insert lower priority before higher one */
                break;
            }
        }

        /* insert new thread at position array[idx] */
        threadDB.tasks.splice(idx, 0, thread);
    }
}

/*
 *  ======== ExecGraph_taskInfo ========
 *  Store task info for render phase
 *
 *  viewData [in]: ti.sysbios.knl.Task.BasicView
 *
 *  task: Object {
 *      label: String task.label
 *      hndl: String "0x" + task.address.toString(16)
 *      pri: Number 
 *      type: 0 kernel, 1 task, 2 swi, 3 hwi
 *  }
 */
function ExecGraph_taskInfo(threadDB, viewData)
{
    var label = "";

    for (var i = 0; i < viewData.length; i++) {
        var task = viewData[i];

        if (task.label.match(/IdleTask/i)) {
            label = "Idl: " + task.address;
        }
        else if (task.label != "") {
            label = "Tsk: "+ task.label;
        }
        else {
            /* TODO - why is fxn an array? */
//          label = "Tsk: " + task.fxn[0] + " " + task.address;
            label = "Tsk: " + task.fxn[0];
        }

        /* create thread object */
        var thread = {
            label: label,
            hndl: task.address,
            pri: Number(task.priority),
            type: 1
        };

        /* find insertion point for task */
        var idx;
        for (idx = 1; idx < threadDB.tasks.length; idx++) {
            if (threadDB.tasks[idx].type > thread.type) {
                /* insert task before swi and hwi */
                break;
            }
            else if ((threadDB.tasks[idx].type == thread.type) &&
                    (threadDB.tasks[idx].pri > thread.pri)) {
                /* insert lower priority before higher one */
                break;
            }
        }

        /* insert new thread at position array[idx] */
        threadDB.tasks.splice(idx, 0, thread);
    }
}

/*
 *  ======== ExecGraph_LoggerTextInfo ========
 *  Store logger instances for render phase
 *
 *  threadDB [inout]
 *  viewData [in]: C.LoggerText.InstanceView[]
 *
 *  task: Object {
 *      label: String task.label
 *      hndl: String "0x" + task.address.toString(16)
 *      pri: Number 
 *      type: 0 kernel, 1 task, 2 swi, 3 hwi
 *  }
 */
function ExecGraph_LoggerTextInfo(threadDB, viewData)
{
    var label = "";

    for (var i = 0; i < viewData.elements.length; i++) {
        var logger = viewData.elements[i];

        label = "LoggetText: " + logger.Label;

        /* create thread object */
        var thread = {
            label: label,
            hndl: "C.LoggerText.Records_" + (i+1),
            pri: 33, /* TODO - this is a hack, need better solution */
            type: 1 /* TODO - need new type for LoggerText */
        };

        /* find insertion point for logger instance */
        var idx;
        for (idx = 1; idx < threadDB.tasks.length; idx++) {
            if (threadDB.tasks[idx].type > thread.type) {
                /* insert before swi and hwi */
                break;
            }
        }

        /* insert new thread at position array[idx] */
        threadDB.tasks.splice(idx, 0, thread);
    }
}

/*
 *  ======== ExecGraph_sortThreads ========
 *  Sort the given threads in order of execution precedence
 *
 *  Arrange the threads (task, swi, hwi) in the exec_data object
 *  as task, swi, hwi going from bottom to top (y-axis) and
 *  priority as low to high within each group.
 */
function ExecGraph_sortThreads(exec_data)
{
    exec_data.threads.tasks.sort(function(a,b) {
        if (a.type < b.type) {
            return (-1);
        }
        if (a.type > b.type) {
            return (1);
        }
        if (a.pri < b.pri) {
            return (-1);
        }
        if (a.pri > b.pri) {
            return (1);
        }
        return (0);
    });
}

/*
 *  ======== ExecGraph_taskValY ========
 *  Look up given task in database
 *
 *  logRec: xdc.runtime.LoggerBuf.RecordView
 *  return: index value in database of given task
 *
 *  The render engine has state which contains the current "thread" and
 *  an execution context stack.
 *
 *  threadDB: {
 *      state: Number   1 - task, 2 - swi, 3 - hwi
 *      thr_h: String   address
 *      tasks: task[]
 *      stack: { thr_h, state }[]
 *      lastTask_h: String
 *      mode: String    NS - non-secure, S - secure
 *  }
 *
 *  task: {
 *      label: String
 *      hndl: String    "0x00000000"
 *      pri: Number     -1, 1, 2, ...
 *      type: Number
 *  }
 */
function ExecGraph_taskValY(evtIdx, threadDB, logs)
{
    var evtn = logs[evtIdx];
    var evtn1 = (evtIdx + 1) < logs.length ? logs[evtIdx + 1] : {
        type: "" };
    var tsk_run = threadDB.tasks[threadDB.running];
    var hndl = "";

    /* return array index of given handle */
    function hndl_indx(hndl, threadDB)
    {
        for (var i = 0; i < threadDB.tasks.length; i++) {
            if (hndl == threadDB.tasks[i].hndl) {
                return (i);
            }
        }
        return (-1);
    }

    if (evtn.type == "Hwi_Begin") {
        tsk_run.stack.push(evtn.hndl);
        threadDB.y_val = hndl_indx(evtn.hndl, threadDB);
    }
    else if (evtn.type == "Hwi_End") {
        if (evtn.hndl == tsk_run.stack[tsk_run.stack.length-1]) {
            tsk_run.stack.pop();
        }

        if (((evtn1.ts - evtn.ts) < 1000) && ((evtn1.type == "Hwi_Begin") ||
            (evtn1.type == "Swi_Begin") || (evtn1.type == "Task_Switch"))) {
            threadDB.y_val = 0; /* kernel */
        }
        else {
            hndl = tsk_run.stack[tsk_run.stack.length-1];
            threadDB.y_val = hndl_indx(hndl, threadDB);
        }
    }
    else if (evtn.type == "Swi_Begin") {
        tsk_run.stack.push(evtn.hndl);
        threadDB.y_val = hndl_indx(evtn.hndl, threadDB);
    }
    else if (evtn.type == "Swi_End") {
        if (evtn.hndl == tsk_run.stack[tsk_run.stack.length-1]) {
            tsk_run.stack.pop();
        }

        if (((evtn1.ts - evtn.ts) < 1000) && ((evtn1.type == "Hwi_Begin") ||
            (evtn1.type == "Swi_Begin") || (evtn1.type == "Task_Switch"))) {
            threadDB.y_val = 0; /* kernel */
        }
        else {
            hndl = tsk_run.stack[tsk_run.stack.length-1];
            threadDB.y_val = hndl_indx(hndl, threadDB);
        }
    }
    else if (evtn.type == "Task_Block") {
        /* force the running context into the kernel */
        threadDB.running = 0;
        threadDB.y_val = 0;
    }
    else if (evtn.type == "Task_Ready") {
        /* no change to running context */
    }
    else if (evtn.type == "Task_Switch") {
        for (var i = 0; i < threadDB.tasks.length; i++) {
            if (evtn.hndl_new == threadDB.tasks[i].hndl) {
                threadDB.running = i;
                var task = threadDB.tasks[i];
                hndl = task.stack[task.stack.length-1];
                break;
            }
        }

        threadDB.y_val = hndl_indx(hndl, threadDB);
    }
    else if (evtn.type == "Data") {
        /* no change to running context */
    }

    return (threadDB.y_val);
}

/*
 *  ======== ExecGraph_taskValY_1 ========
 *  Look up given task in database
 *
 *  logRec: xdc.runtime.LoggerBuf.RecordView
 *  return: index value in database of given task
 *
 *  The render engine has state which contains the current "thread" and
 *  an execution context stack.
 *
 *  threadDB: {
 *      state: Number   1 - task, 2 - swi, 3 - hwi
 *      thr_h: String   address
 *      tasks: task[]
 *      stack: { thr_h, state }[]
 *      lastTask_h: String
 *      mode: String    NS - non-secure, S - secure
 *  }
 *
 *  task: {
 *      label: String
 *      hndl: String    "0x00000000"
 *      pri: Number     -1, 1, 2, ...
 *      type: Number
 *  }
 */
function ExecGraph_taskValY_1(evtIdx, threadDB, logs)
{
    var y_val = -1; /* unknown task */
    var evtn = logs[evtIdx];
    var evtn1 = (evtIdx + 1) < logs.length ? logs[evtIdx + 1] : {
        eventName: "" };

    if (evtn.eventName.match(/^ti\.sysbios\..+\.Hwi.LM_begin/)) {
        threadDB.stack.push({
            thr_h: threadDB.thr_h,
            state: threadDB.state
        });
        threadDB.thr_h = "0x" + Number(evtn.arg0).toString(16);
        threadDB.state = 3; /* hwi */
    }
    else if (evtn.eventName.match(/^ti\.sysbios\..+\.Hwi.LD_end/)) {
        if (evtn1.eventName == "ti.sysbios.knl.Swi.LM_begin") {
            threadDB.thr_h = "0x00000000"; /* TODO - make this stronger */
            threadDB.state = 3; /* hwi */
        }
        else if (evtn1.eventName == "ti.sysbios.knl.Task.LM_switch") {
            if (threadDB.stack.length > 0) {
//              threadDB.stack.pop(); /* discard thread context */
                threadDB.stack = new Array(); /* discard stack context */
            }
            threadDB.thr_h = "0x00000000"; /* TODO - make this stronger */
            threadDB.state = 3; /* hwi */
        }
        else {
            if (threadDB.stack.length > 0) {
                var s = threadDB.stack.pop();
                threadDB.thr_h = s.thr_h;
                threadDB.state = s.state;
            }
            else { /* missing data */
                /* missing data, use existing thread handle */
            }
        }
    }
    else if (evtn.eventName == "ti.sysbios.knl.Swi.LM_begin") {
        if (threadDB.thr_h != "0x00000000") {
            threadDB.stack.push({
                thr_h: threadDB.thr_h,
                state: threadDB.state
            });
        }
        threadDB.thr_h = "0x" + Number(evtn.arg0).toString(16);
        threadDB.state = 2; /* swi */
    }
    else if (evtn.eventName == "ti.sysbios.knl.Swi.LD_end") {
        if (evtn1.eventName == "ti.sysbios.knl.Task.LM_switch") {
//          threadDB.stack.pop(); /* discard thread context */
            threadDB.stack = new Array(); /* discard stack context */
            threadDB.thr_h = "0x00000000"; /* TODO - make this stronger */
            threadDB.state = 2; /* swi */
        }
        else {
            if (threadDB.stack.length > 0) {
                var s = threadDB.stack.pop();
                threadDB.thr_h = s.thr_h;
                threadDB.state = s.state;
            }
            else {
                /* missing data, use last task switch event */
                threadDB.thr_h = threadDB.lastTask_h;
                threadDB.state = 1; /* task */
            }
        }
    }
    else if (evtn.eventName == "ti.sysbios.knl.Task.LD_block") {
        threadDB.thr_h = "0x00000000";
    }
    else if (evtn.eventName == "ti.sysbios.knl.Task.LM_switch") {
        threadDB.thr_h = "0x" + Number(evtn.arg2).toString(16);
        threadDB.lastTask_h = threadDB.thr_h;
        if (threadDB.stack.length > 0) {
            threadDB.stack = new Array(); /* discard stack context */
        }
        threadDB.state = 1; /* task */
    }
    else if (evtn.eventName.match(/^C.LoggerText.Records_\d+/)) {
        if ((threadDB.thr_h != evtn.eventName) && (threadDB.state <= 1)) {
            threadDB.stack.push({
                thr_h: threadDB.thr_h,
                state: threadDB.state,
            });
            threadDB.thr_h = evtn.eventName;
        }
    }

    /* compute the graph y-val (y == array position + 1) */
    for (var i = 0; i < threadDB.tasks.length; i++) {
        if (threadDB.tasks[i].hndl == threadDB.thr_h) {
            y_val = i;
            break;
        }
    }

    if (evtn.eventName.match(/^C.LoggerText.Records_\d+/) &&
        (threadDB.state <= 1)) {

        var s = threadDB.stack.pop();
        threadDB.thr_h = s.thr_h;
        threadDB.state = s.state;
    }

    return (y_val);
}

/*
 *  ======== ExecGraph_findRunningTask ========
 */
function ExecGraph_findRunningTask(exec_data)
{
    var hndl = "";

    /* search log buffer for first task switch event */
    for (var i = 0; i < exec_data.logs.length; i++) {
        var evt = exec_data.logs[i];
        if (evt.type == "Task_Switch") {
            /* out-going task was the running task */
            hndl = evt.hndl_old;
            break;
        }
    }

    if (hndl != "") {
        for (var i = 0; i < exec_data.threads.length; i++) {
            if (hndl == exec_data.threads.tasks[i].hndl) {
                return (i);
            }
        }
    }

    /* TODO - consult tasks list */

    return (0);
}

/*
 *  ======== ExecGraph_getThread ========
 */
function ExecGraph_getThread(exec_data, type, hndl)
{
    for (var i = 1; i < exec_data.threads.tasks.length; i++) {
        var thr = exec_data.threads.tasks[i];
        if ((thr.type == type) && (thr.hndl == hndl)) {
            return (thr);
        }
    }

    /* not found */
    return (null);
}
