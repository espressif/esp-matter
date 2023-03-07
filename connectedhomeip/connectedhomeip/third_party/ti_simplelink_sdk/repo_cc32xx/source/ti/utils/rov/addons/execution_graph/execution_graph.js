/*
 *  ======== execution_graph.js ========
 *  Graph example template for an ROV2 addon element
 *
 *  See also:
 *      https://www.polymer-project.org/1.0/docs/devguide/feature-overview
 *  and for Plotly.js charts:
 *      https://plot.ly/javascript/
 */

function ExGraph_onZoom(evt, vo)
{
    if (evt['xaxis.range[0]'] == undefined) {
        return;
    }

    var ts_hz = vo.exec_data.ts_freq;
    var vp = vo.viewport;
    var min_usec = 0; // usec
    var max_usec = 0; // usec

    if (vo.vp_time_unit == 0) { // timestamp
        min_usec = evt['xaxis.range[0]'] * (1000000 / ts_hz);
        max_usec = evt['xaxis.range[1]'] * (1000000 / ts_hz);
        vp.min_ts = evt['xaxis.range[0]'];
        vp.max_ts = evt['xaxis.range[1]'];
    }
    else if (vo.vp_time_unit == 1000000) { // usec
        min_usec = evt['xaxis.range[0]'];
        max_usec = evt['xaxis.range[1]'];
        vp.min_ts = evt['xaxis.range[0]'] * (ts_hz / 1000000);
        vp.max_ts = evt['xaxis.range[1]'] * (ts_hz / 1000000);
    }
    else if (vo.vp_time_unit == 1000) { // msec
        min_usec = evt['xaxis.range[0]'] * 1000;
        max_usec = evt['xaxis.range[1]'] * 1000;
        vp.min_ts = evt['xaxis.range[0]'] * (ts_hz / 1000);
        vp.max_ts = evt['xaxis.range[1]'] * (ts_hz / 1000);
    }
    else { // sec
        min_usec = evt['xaxis.range[0]'] * 1000000;
        max_usec = evt['xaxis.range[1]'] * 1000000;
        vp.min_ts = evt['xaxis.range[0]'] * ts_hz;
        vp.max_ts = evt['xaxis.range[1]'] * ts_hz;
    }

    vo.data_time_width = max_usec - min_usec; // usec

    scale_data(vo);

    /* update graph's x-axis and redraw it */
    Plotly.update(vo.$.graph, [vo.trace], vo.layout);
}


/* Polymer element registration */
Polymer({
    /* identify this code's element: must match <dom-module> id */
    is: "xdc-rov-polymerUI-examples-execution-graph",

    /* common view behaviors. See ti-rov-behaviors.html for reference */
    behaviors: [rovBehaviors.viewBehaviors],

    /* properties for this element's public API */
    properties: {
        logs: {        /* the data being plotted */
          type:  Array,
          value: null
        },
        threadDB: {    /* task data base, used for rendering */
            type: Object,
            value: {}
        },
        viewName: {    /* required read-only view name: shown in addon menu */
          type:  String,
          value: "Execution Graph"
        },
        viewRefresh: { /* recommended view state indicator */
          type:  Boolean,
          value: false
        },
        trace : {       /*  plotly line trace */
            type: Object,
            value: null
        },
        layout : {      /*  plotly layout */
            type: Object,
            value: null
        },
        plotly_config: {
            type: Object,
            value: null
        },
        VISIBLE_ROWS: { /* Visible x axis */
            type:  Number,
            value: 101
        },
        graphWidth: {
            type:  Number,
            value: 390
        },
        graphHeight: {
            type:  Number,
            value: 264
        },
        viewport: {
            type: Object,
            value: null
        },
        vp_time_scale: {
            type: Number,
            value: 120          // ticks / usec
        },
        vp_time_width: {
            type: Number,
            value: 100000000    /* 100 rows * 1,000,000 ns per ms */
        },
        vp_time_unit: {
            type: Number,
            value: 1000000
        },
        data_time_width: {
            type: Number,
            value: 0
        },
        views: {
          type:  Array,
          value: null
        },
        exec_data: {            // execution data
            type: Object,
            value: null
        },
        on_zoom: {
          type:  Function,
          value: null
        },

        /*  Names of properties specific to this element that
         *  we want to persist when the element is saved and restored
         */
        persistProperties: {
            type: Array,
            value: ['graphWidth', 'graphHeight', 'VISIBLE_ROWS']
        }
    },

    /* methods on the element's prototype */

    onModeBar: function(e)
    {
        var graph = this.$.graph;

        /* delete existing graph */
        Plotly.purge(graph);

        /* toggle the mode bar */
        this.plotly_config.displayModeBar = !this.plotly_config.displayModeBar;

        /* re-create graph */
        Plotly.newPlot(graph, [this.trace], this.layout, this.plotly_config);
    },

    /*
     *  ======== onZoom ========
     *  Called on graph zoom event (fence zoom)
     */
    onZoom: function(evt, vo)
    {
        if (evt['xaxis.range[0]'] == undefined) {
            return;
        }

//      var vo = rovBehaviors.addOnBehaviors.viewObj;

        var ts_hz = vo.exec_data.ts_freq;
        var vp = vo.viewport;
        var min_usec = 0; // usec
        var max_usec = 0; // usec

        if (vo.vp_time_unit == 0) { // timestamp
            min_usec = evt['xaxis.range[0]'] * (1000000 / ts_hz);
            max_usec = evt['xaxis.range[1]'] * (1000000 / ts_hz);
            vp.min_ts = evt['xaxis.range[0]'];
            vp.max_ts = evt['xaxis.range[1]'];
        }
        else if (vo.vp_time_unit == 1000000) { // usec
            min_usec = evt['xaxis.range[0]'];
            max_usec = evt['xaxis.range[1]'];
            vp.min_ts = evt['xaxis.range[0]'] * (ts_hz / 1000000);
            vp.max_ts = evt['xaxis.range[1]'] * (ts_hz / 1000000);
        }
        else if (vo.vp_time_unit == 1000) { // msec
            min_usec = evt['xaxis.range[0]'] * 1000;
            max_usec = evt['xaxis.range[1]'] * 1000;
            vp.min_ts = evt['xaxis.range[0]'] * (ts_hz / 1000);
            vp.max_ts = evt['xaxis.range[1]'] * (ts_hz / 1000);
        }
        else { // sec
            min_usec = evt['xaxis.range[0]'] * 1000000;
            max_usec = evt['xaxis.range[1]'] * 1000000;
            vp.min_ts = evt['xaxis.range[0]'] * ts_hz;
            vp.max_ts = evt['xaxis.range[1]'] * ts_hz;
        }

        vo.data_time_width = max_usec - min_usec; // usec

        scale_data(vo);

        /* update graph's x-axis and redraw it */
        Plotly.update(vo.$.graph, [vo.trace], vo.layout);
    },

    /*
     *  ======== plotNewData ========
     *  Process acquired log data and render to graph
     */
    plotNewData: function()
    {
        var ts_hz = this.exec_data.ts_freq;
        var logs = this.exec_data.logs;

        /* save min and max timestamp of new data */
        this.viewport.min_ts = logs[0].ts;
        this.viewport.max_ts = logs[logs.length - 1].ts;

        /* compute viewport time scale (timestamp, usec, msec, sec) */
        var min_usec = logs[0].ts * (1000000 / ts_hz);
        var max_usec = logs[logs.length - 1].ts
            * (1000000 / ts_hz);

        this.data_time_width = max_usec - min_usec; // usec

        if (this.data_time_width < 500) { // < 500 usec, use timestamp
            this.vp_time_scale = 1;
            this.vp_time_unit = 0; // timestamp
            this.layout.xaxis.title = 'Time (timestamp)';
        }
        else if (this.data_time_width < 20 * 1000) { // < 20 msec, use usec
            this.vp_time_scale = 1000000 / ts_hz;
            this.vp_time_unit = 1000000; // usec
            this.layout.xaxis.title = 'Time (usec)';
        }
        else if (this.data_time_width < 100000) {  // < 1 sec, use msec
            this.vp_time_scale = 1000 * ts_hz;
            this.vp_time_unit = 1000; // msec
            this.layout.xaxis.title = 'Time (msec)';
        }
        else {  // use sec
            this.vp_time_scale = 1 / ts_hz;
            this.vp_time_unit = 1; // sec
            this.layout.xaxis.title = 'Time (sec)';
        }
        
        /* add data to trace object, scale x-axis to the current time unit */
        this.trace.x = new Array();
        this.trace.y = new Array();
        this.trace.hovertext = new Array();

        for (var i = 0; i < logs.length; i++) {
            if (this.vp_time_unit == 0) {
                this.trace.x[i] = logs[i].ts;
            }
            else {
                this.trace.x[i] = logs[i].ts * this.vp_time_scale;
            }
            this.trace.y[i] = logs[i].y;
            this.trace.hovertext[i] = logs[i].seq + " " + logs[i].label;
        }

        if (this.trace.x.length >= 2) {
            this.layout.xaxis.range[0] = this.trace.x[0];
            this.layout.xaxis.range[1] = this.trace.x[this.trace.x.length - 1];
        }
        else {
            this.layout.xaxis.range[0] = 0;
            this.layout.xaxis.range[1] = this.VISIBLE_ROWS - 1;
        }

        this.layout.yaxis.range[0] = 0;
        this.layout.yaxis.range[1] = this.exec_data.threads.tasks.length + 1;

        /* update y-axis */
        var y_axis = this.layout.yaxis;
        y_axis.tickvals = new Array();
        y_axis.ticktext = new Array();

        for (var i = 0; i < this.exec_data.threads.tasks.length; i++) {
            y_axis.tickvals.push(i+1); /* skip tick zero */
            y_axis.ticktext.push(this.exec_data.threads.tasks[i].label);
        }

        /* update graph's x-axis (if necessary) and redraw it */
        Plotly.update(this.$.graph, [this.trace], this.layout);
    },

    /*
     *  ======== getViewCallback ========
     *  Callback from rovData.getView()
     *
     *  Params
     *    error      - null when there is no error; otherwise an error
     *                 message string
     *    viewData   - the ROV view data requested as a JSON object
     *    moduleName - module name string for the returned view data
     *    viewName   - view name string for the returned view data
     */
    getViewCallback: function(error, viewData, moduleName, viewName)
    {
        /* show the refresh button and hide the refresh spinner */
        if (this.viewRefresh) {
            this.viewRefresh = false;
        }

        /* handle data acquisition error (if any) */
        if (error != null) {
            /* TODO */
        }

        /* identify which view callback is invoked */
        var mvName = moduleName + "." + viewName;

        switch (mvName) {
            case "Global.ExecGraphSettings.Module":
                var module = viewData.elements[0];
                this.exec_data.ts_freq = Number(module.TimestampHZ);

                /* acknowledge this request has been completed */
                for (var i = 0; i < this.views.length; i++) {
                    var v = this.views[i];
                    if (mvName == v.name + "." + v.tab) {
                        v.done = true;
                        break;
                    }
                }
                break;

            case "Global.ExecGraphAdapter.Module":
                var module = viewData.elements[0];
                this.exec_data.adapter.count = Number(module.NumInstances);

                /* acknowledge this request has been completed */
                for (var i = 0; i < this.views.length; i++) {
                    var v = this.views[i];
                    if (mvName == v.name + "." + v.tab) {
                        v.done = true;
                        break;
                    }
                }
                break;

            case "Global.ExecGraphAdapter.Instance":
                for (var i = 0; i < viewData.elements.length; i++) {
                    var elem = viewData.elements[i];
                    var moduleName = "";

                    /* if no wild characters, use given module name */
                    if (elem.Module.indexOf("*") == -1) {
                        moduleName = elem.Module;
                    }
                    else {
                        /* find module name using given pattern */
                        var re = RegExp("^" + elem.Module + "$");
                        var viewList = this.rovPanel.getViewList();
                        for (var p in viewList) {
                            if (re.test(p)) {
                                moduleName = p;
                                break;
                            }
                        }
                    }
                    if (moduleName != "") {
                        var req = {
                            name: moduleName,
                            tab: elem.View,
                            fxn: new Function("execData", "viewData", elem.Fxn),
                            done: false
                        };
                        this.views.push(req);
                        this.rovData.getView(req.name, req.tab, this);
                    }
                    else {
                        /* TODO - raise error, module not found */
                    }
                }

                /* acknowledge this request has been completed */
                for (var i = 0; i < this.views.length; i++) {
                    var v = this.views[i];
                    if (mvName == v.name + "." + v.tab) {
                        v.done = true;
                        break;
                    }
                }
                break;

            case "Global.ExecGraphFilter.Instance":
                /* construct a filter object for each instance */
                for (var i = 0; i < viewData.elements.length; i++) {
                    var elem = viewData.elements[i];
                    var filter = {
                        logger: elem.Logger,
                        name: elem.Module,
                        fxn: new Function("logEvent", elem.Fxn)
                    };
                    this.exec_data.filters.push(filter);
                }

                /* acknowledge this request has been completed */
                for (var i = 0; i < this.views.length; i++) {
                    var v = this.views[i];
                    if (mvName == v.name + "." + v.tab) {
                        v.done = true;
                        break;
                    }
                }

                /* submit request for logger adapters, must be after filters */
                var logger = "Global.ExecGraphLogger";
                var viewList = this.rovPanel.getViewList();
                if (logger in viewList) {
                    var req = {
                        name: logger,
                        tab: "Instance",
                        done: false
                    };
                    this.views.push(req);
                    this.rovData.getView(req.name, req.tab, this);
                }

                break;

            case "Global.ExecGraphLogger.Instance":
                var viewList = this.rovPanel.getViewList();

                /* for each instance, submit request for logger data */
                for (var i = 0; i < viewData.elements.length; i++) {
                    var elem = viewData.elements[i];
                    if (elem.Module in viewList) {
                        var req = {
                            name: elem.Module,
                            tab: elem.View,
                            fxn: new Function("execData", "viewData", elem.Fxn),
                            done: false
                        };
                        this.views.push(req);
                        this.rovData.getView(req.name, req.tab, this);
                    }
                }

                /* acknowledge this request has been completed */
                for (var i = 0; i < this.views.length; i++) {
                    var v = this.views[i];
                    if (mvName == v.name + "." + v.tab) {
                        v.done = true;
                        break;
                    }
                }
                break;

            default:
                /* search for this ROV module-view in array of requests */
                for (var i = 0; i < this.views.length; i++) {
                    var v = this.views[i];
                    if (mvName == v.name + "." + v.tab) {
                        /* found request, invoke adapter function */
                        v.fxn(this.exec_data, viewData);
                        v.done = true;
                    }
                }
                break;
        }

if (false) {
        if (mvName.match(/^ti\.sysbios\.family\..*\.Hwi/)) {
            ExecGraph_hwiInfo(this.threadDB, viewData);
        }
        else {
            switch (mvName) {
                case "C.ExecGraphAdapter.Module":
                    var module = viewData.elements[0];
                    this.exec_data.ts_freq = Number(module.TimestampHZ);
                    break;
                case "C.LoggerText.Instance":
                    if (error != null) {
                        /* TODO - don't request record views */
                        break;
                    }
                    for (var i = 0; i < viewData.elements.length; i++) {
                        var req = {
                            name: "C.LoggerText",
                            tab: "Records_" + (i+1),
                            done: false
                        };
                        this.views.push(req);
                        this.rovData.getView(req.name, req.tab, this);
                    }

                    /* add logger instances to thread database */
                    ExecGraph_LoggerTextInfo(this.threadDB, viewData);
                    break;

                case "C.LoggerText.Records_1":
                case "C.LoggerText.Records_2":
                case "C.LoggerText.Records_3":
                case "C.LoggerText.Records_4":
                case "C.LoggerText.Records_5":
                    this.logs = ExecGraph_collate(this.logs, mvName, viewData);
                    break;
                case "C.LoggerNDK.Records":
                    this.logs = ExecGraph_collate(this.logs, mvName, viewData);
                    break;
                case "ti.sysbios.knl.Task.Basic":
                    ExecGraph_taskInfo(this.threadDB, viewData);
                    break;
                case "ti.sysbios.knl.Swi.Basic":
                    ExecGraph_swiInfo(this.threadDB, viewData);
                    break;
//              case "ti.sysbios.family.arm.v8m.Hwi.Basic":
//                  ExecGraph_hwiInfo(this.threadDB, viewData);
//                  break;
                case "xdc.runtime.LoggerBuf.Records":
                    this.logs = ExecGraph_collate(this.logs, mvName, viewData);
                    break;
            }
        }
}

        /* if all data requests have been received, render the graph */
        var render = true;

        for (var i = 0; i < this.views.length; i++) {
            render = (!this.views[i].done ? false : render);
        }

        if (render) {
            ExecGraph_computePath(this.exec_data);
            this.plotNewData();
        }
    },

    /*
     *  ======== onRefresh ========
     *  Called when user clicks the 'Refresh' button on this view,
     *  or 'Refresh all' or 'Repeat refresh' buttons on the ROV toolbar
     */
    onRefresh: function()
    {
        /*  Request data from an ROV view.
         *
         *  This is an asynchronous call; getViewCallback() will be called
         *  with the result in its viewData parameter.
         *  getViewCallback() returns null if a previous call from
         *  this view is pending.
         *
         *  this.viewRefresh hides/shows the refresh button and the
         *  refresh spinner.
         */
        this.viewRefresh = true;

        /* create new execution data object */
        this.exec_data = {
            ts_freq: 0,
            adapter: {
                count: 0
            },
            threads: {
                running: 0,
                y_val: 0,
                tasks: [{
                    label: "Kernel",
                    hndl: "0x00000000",
                    type: 0,
                    pri: -1,
                    stack: ["kernel"]
                }]
            },
            filters: [],
            logs: []
        };

        /* delete old data */
        this.logs = new Array();
        this.threadDB = {};
        this.threadDB.state = 1; /* 1 - task, 2 - swi, 3 - hwi */
        this.threadDB.thr_h = "0x00000000";
        this.threadDB.tasks = new Array();
        this.threadDB.tasks.push({
            label: "kernel",
            hndl: "0x00000000",
            pri: -1,
            type: 0
        });
        this.threadDB.stack = new Array();
        this.threadDB.lastTask_h = "";
        this.threadDB.mode = "NS";

        /* construct list of contributing views */
        var viewList = this.rovPanel.getViewList();
        this.views = new Array(); /* TODO - make this a map */
//      var has_instrumentation = false;

        /* look for ExecGraphAdapter module, build requests from there */
        var settings = "Global.ExecGraphSettings";
        if (settings in viewList) {
            this.views.push({ name: settings, tab: "Module", done: false });
        }
        else {
            /* issue and error */
            this.rovPanel.showStatus(
                "Must include ExecGraphSettings in configuration", 'error');
        }
        var adapter = "Global.ExecGraphAdapter";
        if (adapter in viewList) {
            this.views.push({ name: adapter, tab: "Module", done: false });
            this.views.push({ name: adapter, tab: "Instance", done: false });
        }
        else {
            /* issue and error */
            this.rovPanel.showStatus(
                "Must include ExecGraphAdapter in configuration", 'error');
        }
        var filter = "Global.ExecGraphFilter";
        if (filter in viewList) {
            this.views.push({ name: filter, tab: "Instance", done: false });
        }
        else {
            /* issue and error */
            this.rovPanel.showStatus(
                "Must include ExecGraphFilter in configuration", 'error');
        }

//      for (var p in viewList) {
//          if (p.match(/^ti\.sysbios\.family\..*\.Hwi/)) {
//              this.views.push({ name: p, tab: "Basic", done: false });
//              continue;
//          }
//          switch (p) {
//              case "C.ExecGraphAdapter":
//                  this.views.push({ name: p, tab: "Module", done: false });
//                  has_instrumentation = true;
//                  break;
//              case "C.LoggerText":
//                  /* TOD0 - replace with INSTANCE_DATA */
//                  this.views.push({ name: p, tab: "Instance", done: false });
//                  break;
//              case "C.LoggerNDK":
//                  this.views.push({ name: p, tab: "Records", done: false });
//                  break;
//              case "ti.sysbios.knl.Task":
//              case "ti.sysbios.knl.Swi":
//                  this.views.push({ name: p, tab: "Basic", done: false });
//                  break;
//              case "xdc.runtime.LoggerBuf":
//                  this.views.push({ name: p, tab: "Records", done: false });
//                  break;
//          }
//      }

        /* request view data */
        for (var i = 0; i < this.views.length; i++) {
            this.rovData.getView(this.views[i].name, this.views[i].tab, this);
        }
    },

   /*
    *  ======== onSaveView ========
    *  Called when user clicks the 'Download' button on this view,
    *  or the 'Download all' button on the ROV toolbar
    */
    onSaveView: function () {
        /* return the graph's current data set */
        return (this.logs);
    },

    /*
     *  ======== attached ========
     *  Called by Polymer after the element is attached to the document.
     *  Can be called multiple times during the lifetime of an element.
     *  The first `attached` callback is guaranteed not to fire until
     *  after `ready`.
     *
     *  ti-rov-panel is the global container element for ROV.  It has
     *  already retrieved the modules and view list, so we can get these
     *  directly from there instead of making a redundant asynchronous
     *  calls into ti-rov-data.
     *
     *  To get the ti-rov-panel element:
     *        var rovPanel = document.querySelector('ti-rov-panel');
     */
    attached: function () {
        if (this.trace != null) {
            return;
        }

        this.threadDB.stack = new Array();
        this.threadDB.runningTask = -1;

        var graph = this.$.graph;
        graph.style.width = this.graphWidth + 'px';
        graph.style.height = this.graphHeight + 'px';

        /* initialize all our state */
        this.logs = [];

        this.trace = {
            x: [0],
            y: [0],
            type: 'scatter',
            mode: 'lines+markers',
            line: { shape: 'hv' }, // horizontal to next value
            name: 'Running'
        };

        this.layout = {
          xaxis: {
            title: 'Time (msec)',
            showgrid: true,
            type: 'linear',
            range: [0, this.VISIBLE_ROWS - 1],
            tickmode: 'auto',
//          nticks: 11,
            nticks: 9,
            tickfont: {size: 11},
            titlefont: {size: 11},
            exponentformat: "none"
          },
          yaxis: {
//          title: 'Execution Context',
            type: 'linear',
            range: [0, 5],
            fixedrange: true,
            tickmode: 'array',
            tickvals: [],
            ticktext: [],
            tickfont: {size: 11},
            titlefont: {size: 11},
            automargin: true
          },
          width: this.graphWidth,
          height: this.graphHeight,
          autosize: true,
          hoverinfo: "text",
          margin: {
            l: 150,
            r: 20,
            b: 50,
            t: 0,
            autoexpand: true
          },
          showlegend: true,
          legend: {
            x: 1,
            y: 1,
            'orientation': 'v',
            'yanchor': 'bottom',
            'xanchor': 'right',
            font: {size: 11}
          },
          dragmode: 'pan'
        };

        this.plotly_config = {
            displayModeBar: false,
            scrollZoom: false
        };

        this.viewport = {
            min_ts: 0,                  /* min timestamp of viewport */
            max_ts: 0                   /* max timestamp of viewport */
        };

        this.exec_data = {
            ts_freq: 0,
            adapter: {
                count: 0
            },
            threads: {
                running: 0,
                y_val: 0,
                tasks: [{
                    label: "Kernel",
                    hndl: "0x00000000",
                    type: 0,
                    pri: -1,
                    stack: ["kernel"]
                }]
            },
            filters: [],
            logs: []
        };

        this.on_zoom = this.onZoom;

        Plotly.newPlot(graph, [this.trace], this.layout, this.plotly_config);

        /* working with hack */
//      rovBehaviors.addOnBehaviors.viewObj = this;
//      graph.on('plotly_relayout', this.onZoom);

        let viewObj = this;

        graph.on('plotly_relayout', function(evt) {
            viewObj.on_zoom(evt, viewObj); });
//          ExGraph_onZoom(evt, viewObj); });

//      this.onZoom.viewObj = this;
//      graph.on('plotly_relayout', this.onZoom);

        this.onRefresh();
    },

    /*
     *  ======== resized ========
     *  Callback initially passed to rovUtils.initRovResize() from
     *  resizerClicked(). Called after the view has been resized,
     *  enabling us to resize the graph accordingly
     */
    resized: function () {
        /* get viewPaperCard defined via the element in execution_graph.html */
        var viewPaperCard = this.$.viewPaperCard;
        var graph = this.$.graph;

        /* get the new width and height from viewPaperCard's style */
        var width = Number(viewPaperCard.style.width.slice(0, -2));
        var height = Number(viewPaperCard.style.height.slice(0, -2));

        /* get viewPaperCard minWidth and minHeight */
        var minWidth = Number(viewPaperCard.style.minWidth.slice(0, -2));
        var minHeight = Number(viewPaperCard.style.minHeight.slice(0, -2));

        /* Don't let new size go below the defined minimums */
        this.graphWidth = Math.max(width, minWidth) * .95;
        this.graphHeight = Math.max(height, minHeight) * .85;

        /* Scale the graph accordingly */
        graph.style.width = this.graphWidth + 'px';
        graph.style.height = this.graphHeight + 'px';

        /* update the graph and redisply it */
        this.layout.width = this.graphWidth;
        this.layout.height = this.graphHeight;
        this.VISIBLE_ROWS = Math.floor((this.graphWidth - 80) / 3.1) + 1;
        this.vp_time_width = (this.VISIBLE_ROWS - 1) * 1000000;
        Plotly.update(this.$.graph, [this.trace], this.layout);
    },

    /*
     *  ======== resizerClicked ========
     *  Called on a bottom corner on-mousedown event
     *
     *  See execution_graph.html resizer div.
     */
    resizerClicked: function (e) {
        /*
         *  call polymerUI/src/rovUtils.js initRovResize(), passing the
         *  viewPaperCard element that is being resized. Also pass callback
         *  object in order to be notified via this.resized() when the element
         *  is done being is resized, so we can make any required size related
         *  changes to the graph itself
         */
        rovUtils.initRovResize(e, this.$.viewPaperCard, {elem: this,
            fxn: this.resized});
    },

    /*
     *  ======== zoomIn ========
     */
    zoomIn: function(e)
    {
//      var vo = rovBehaviors.addOnBehaviors.viewObj;
        var vo = this;
        var ts_hz = this.exec_data.ts_freq;
        var vp = vo.viewport;
        var min_usec = 0; // usec
        var max_usec = 0; // usec

        if (vo.vp_time_unit == 0) { // timestamp
            min_usec = vo.layout.xaxis.range[0] * (1000000 / ts_hz);
            max_usec = vo.layout.xaxis.range[1] * (1000000 / ts_hz);
//          vp.min_ts = vo.layout.xaxis.range[0];
//          vp.max_ts = vo.layout.xaxis.range[1];
        }
        else if (vo.vp_time_unit == 1000000) { // usec
            min_usec = vo.layout.xaxis.range[0];
            max_usec = vo.layout.xaxis.range[1];
//          vp.min_ts = vo.layout.xaxis.range[0] * (ts_hz / 1000000);
//          vp.max_ts = vo.layout.xaxis.range[1] * (ts_hz / 1000000);
        }
        else if (vo.vp_time_unit == 1000) { // msec
            min_usec = vo.layout.xaxis.range[0] * 1000;
            max_usec = vo.layout.xaxis.range[1] * 1000;
//          vp.min_ts = vo.layout.xaxis.range[0] * (ts_hz / 1000);
//          vp.max_ts = vo.layout.xaxis.range[1] * (ts_hz / 1000);
        }
        else { // sec
            min_usec = vo.layout.xaxis.range[0] * 1000000;
            max_usec = vo.layout.xaxis.range[1] * 1000000;
//          vp.min_ts = vo.layout.xaxis.range[0] * ts_hz;
//          vp.max_ts = vo.layout.xaxis.range[1] * ts_hz;
        }

        /* zoom in by 50% */
        vo.data_time_width = (max_usec - min_usec) / 2; // usec
        var step = (vp.max_ts - vp.min_ts) / 4;
        vp.min_ts += step;
        vp.max_ts -= step;

        scale_data(vo);

        /* update graph's x-axis (if necessary) and redraw it */
        Plotly.update(vo.$.graph, [vo.trace], vo.layout);
    },

    /*
     *  ======== zoomOut ========
     */
    zoomOut: function(e)
    {
//      var vo = rovBehaviors.addOnBehaviors.viewObj;
        var vo = this;
        var ts_hz = this.exec_data.ts_freq;
        var vp = vo.viewport;
        var min_usec = 0; // usec
        var max_usec = 0; // usec

        if (vo.vp_time_unit == 0) { // timestamp
            min_usec = vo.layout.xaxis.range[0] * (1000000 / ts_hz);
            max_usec = vo.layout.xaxis.range[1] * (1000000 / ts_hz);
//          vp.min_ts = vo.layout.xaxis.range[0];
//          vp.max_ts = vo.layout.xaxis.range[1];
        }
        else if (vo.vp_time_unit == 1000000) { // usec
            min_usec = vo.layout.xaxis.range[0];
            max_usec = vo.layout.xaxis.range[1];
//          vp.min_ts = vo.layout.xaxis.range[0] * (ts_hz / 1000000);
//          vp.max_ts = vo.layout.xaxis.range[1] * (ts_hz / 1000000);
        }
        else if (vo.vp_time_unit == 1000) { // msec
            min_usec = vo.layout.xaxis.range[0] * 1000;
            max_usec = vo.layout.xaxis.range[1] * 1000;
//          vp.min_ts = vo.layout.xaxis.range[0] * (ts_hz / 1000);
//          vp.max_ts = vo.layout.xaxis.range[1] * (ts_hz / 1000);
        }
        else { // sec
            min_usec = vo.layout.xaxis.range[0] * 1000000;
            max_usec = vo.layout.xaxis.range[1] * 1000000;
//          vp.min_ts = vo.layout.xaxis.range[0] * ts_hz;
//          vp.max_ts = vo.layout.xaxis.range[1] * ts_hz;
        }

        /* zoom out by 200% */
        vo.data_time_width = (max_usec - min_usec) * 2; // usec
        var step = (vp.max_ts - vp.min_ts) / 2;
        vp.min_ts -= step;
        vp.max_ts += step;

        scale_data(vo);

        /* update graph's x-axis (if necessary) and redraw it */
        Plotly.update(vo.$.graph, [vo.trace], vo.layout);
    },

    /*
     *  ======== zoomReset ========
     */
    zoomReset: function(e)
    {
//      var vo = rovBehaviors.addOnBehaviors.viewObj;
        var vo = this;
        var ts_hz = this.exec_data.ts_freq;

        /* save min and max timestamp of new data */
        vo.viewport.min_ts = vo.exec_data.logs[0].ts;
        vo.viewport.max_ts = vo.exec_data.logs[vo.exec_data.logs.length - 1].ts;

        /* compute viewport time scale (timestamp, usec, msec, sec) */
        var min_usec = vo.exec_data.logs[0].ts * (1000000 / ts_hz);
        var max_usec = vo.exec_data.logs[vo.exec_data.logs.length - 1].ts
            * (1000000 / ts_hz);

        vo.data_time_width = max_usec - min_usec; // usec

        if (vo.data_time_width < 500) { // < 500 usec, use timestamp
            vo.vp_time_scale = 1;
            vo.vp_time_unit = 0; // timestamp
            vo.layout.xaxis.title = 'Time (timestamp)';
        }
        else if (vo.data_time_width < 20 * 1000) { // < 20 msec, use usec
            vo.vp_time_scale = 1000000 / ts_hz;
            vo.vp_time_unit = 1000000; // usec
            vo.layout.xaxis.title = 'Time (usec)';
        }
        else if (vo.data_time_width < 100000) {  // < 1 sec, use msec
            vo.vp_time_scale = 1000 * ts_hz;
            vo.vp_time_unit = 1000; // msec
            vo.layout.xaxis.title = 'Time (msec)';
        }
        else {  // use sec
            vo.vp_time_scale = 1 / ts_hz;
            vo.vp_time_unit = 1; // sec
            vo.layout.xaxis.title = 'Time (sec)';
        }

        scale_data(vo);

        /* update graph's x-axis (if necessary) and redraw it */
        Plotly.update(vo.$.graph, [vo.trace], vo.layout);
    }

});

/*
 *  ======== scale_data ========
 *  Scale data to match viewport zoom level
 */
function scale_data(vo)
{
    var ts_hz = vo.exec_data.ts_freq;
    var viewport = vo.viewport;

    /* compute time unit and x-scale based on data width */
    if (vo.data_time_width < 500) { // < 500 usec, use timestamp
        vo.vp_time_scale = 1;
        vo.vp_time_unit = 0; // timestamp
        vo.layout.xaxis.title = 'Time (timestamp)';
    }
    else if (vo.data_time_width < 5 * 1000) { // < 5 msec, use usec
        vo.vp_time_scale = 1000000 / ts_hz;
        vo.vp_time_unit = 1000000; // usec
        vo.layout.xaxis.title = 'Time (usec)';
    }
    else if (vo.data_time_width < 3000000) {  // < 3 sec, use msec
        vo.vp_time_scale = 1000 / ts_hz;
        vo.vp_time_unit = 1000; // msec
        vo.layout.xaxis.title = 'Time (msec)';
    }
    else {  // use sec
        vo.vp_time_scale = 1 / ts_hz;
        vo.vp_time_unit = 1; // sec
        vo.layout.xaxis.title = 'Time (sec)';
    }

    /* add data to trace object, scale x-axis current time unit */
    for (var i = 0; i < vo.exec_data.logs.length; i++) {
        if (vo.vp_time_unit == 0) {
            vo.trace.x[i] = vo.exec_data.logs[i].ts;
        }
        else {
            vo.trace.x[i] = vo.exec_data.logs[i].ts * vo.vp_time_scale;
        }
    }

    vo.layout.xaxis.range = [
        vo.viewport.min_ts * vo.vp_time_scale,
        vo.viewport.max_ts * vo.vp_time_scale
    ];
}
