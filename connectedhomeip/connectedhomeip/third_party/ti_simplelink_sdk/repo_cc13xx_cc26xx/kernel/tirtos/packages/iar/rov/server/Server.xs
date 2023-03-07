/* --COPYRIGHT--,EPL
 *  Copyright (c) 2012-2020 Texas Instruments Incorporated
 *  This program and the accompanying materials are made available under the
 *  terms of the Eclipse Public License v1.0 and Eclipse Distribution License
 *  v. 1.0 which accompanies this distribution. The Eclipse Public License is
 *  available at http://www.eclipse.org/legal/epl-v10.html and the Eclipse
 *  Distribution License is available at
 *  http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * --/COPYRIGHT--*/
/* For return string packing */
var COL = "\x03";
var ERR = "\x05";
var ROW = "\x12";
var SEP = "\x13";
var TAB = "\x14";
var DEPTH = 4;

/*
 *  ======== errorString ========
 *  Returns a error string packed within delimiter ERR
 */
function errorString(str)
{
    return (ERR + str + ERR);
}

/*
 *  ======== formatTableView ========
 *  Returns a formatted string with key-value pairs
 *
 *  The only time this function is called for multiple views is when each
 *  view is an instance of a same module, so the headers are the same.
 *
 *  Parameters:
 *      views:  Data to be formatted
 *      col  :  Boolean - if true will generate column header
 *      label:  In some cases (InstDataView), a label will be added
 *              to row data. [Optional]
 *
 *  The format of string is:
 *
 *      COL Column1 SEP Column2 SEP Column3 ... COL
 *      ROW Row1 SEP Row2 SEP Row2 ROW
 */
function formatTableView(views, col, label)
{
    var retStr = "";

    if (views.length == 0) {
        return ("");
    }

    var headers = new Array();
    var rows = new Array();

    /* For each of the views... */
    for each (var view in views) {
        var values = new Array();

        var col = 0;

        /*
         * Address field is added explicitly because it doesn't show up in
         * the for/in loop.
         */
        if ('address' in view) {
            headers[col] = "address";
            values[col] = view.address;
            col++;
        }

        /* For each of the properties in the view... */
        for (var p in view) {
            /* If the status is undefined, don't include this field. */
            if (view.$status[p] === undefined) {
                continue;
            }
            /* If the status is non-null, there's an error to report. */
            else if (view.$status[p] != null) {
                throw("View reported error on field '" + p + "': "
                      + view.$status[p]);
            }

            /* Add the property to the column headers. */
            headers[col] = p;

            /* If the field is undefined... */
            if (view[p] == undefined) {
                values[col] = "";
            }
            /* If the field is an array... */
            else if (view[p].$category == 'Vector') {
                values[col] = "";

                /*
                 * Display each array element. Display their index and separate
                 * the elements with a semicolon.
                 */
                for (var i = 0; i < view[p].length; i++) {
                    values[col] += ((i > 0)? ", " : "") + view[p][i];
                }
            }
            /* If this is just a normal field. */
            else {
                values[col] = String(view[p]);
            }

            /* Move on to the next column. */
            col++;
        }

        /* Add the instance view to the rows array. */
        rows[rows.length] = values;
    }


    if (headers.length != rows[0].length) {
        return (errorString("IAR ROV server detected corrupted data"));
    }


    /* Write all the headers only if column is not false */
    if (col != false) {
        retStr = COL;
        for (var i = 0; i < headers.length; i++) {
            retStr = retStr + headers[i] + SEP;
        }
        retStr = retStr + COL;
    }

    /* If there is a label add a row */
    if (label !== undefined) {
        retStr = retStr + ROW + label + ROW;
    }

    /* Print out each of the instances. */
    for each (var values in rows) {
        retStr = retStr + ROW;
        for (var i = 0; i < values.length; i++) {
            retStr = retStr + values[i] + SEP;
        }
        retStr = retStr + ROW;
    }

    return (retStr);
}

/* This function generates <Object> */
function formatTreeView(obj, level)
{
    var objString = "";
    for (var prop in obj) {
        if (prop == "$status") {
           continue;
        }
        objString = objString + prop + TAB;
        if (typeof obj[prop] == 'object' && obj[prop] != null) {
            if (obj[prop].$category == "Addr") {
                objString = objString + "0x" + Number(obj[prop]).toString(16);
            }
            else {
                if (level == DEPTH) {
                    objString = objString + "<Maximum depth reached>";
                }
                else {
                    var outp = ROW + formatTreeView(obj[prop], level + 1)
                        + COL;
                    objString = objString + outp;
                }
            }
        }
        else if (typeof obj[prop] == 'number' && obj[prop] > 9) {
            objString = objString + "0x" + obj[prop].toString(16);
        }
        else {
            objString = objString + obj[prop];
        }
        objString = objString + SEP;
    }
    return (objString);
}

/* This function generates a finite tree from an object that we know is an
 * array of TreeNode structures.
 */
function formatTreeTableView(arr)
{
    function formatTableObject(obj) {
        var objString = "";

        /* Each TreeNode structure has arrays 'children' and 'properties' as
         * properties. For these two, we don't want to see their names, and we
         * don't care to distinguish between their elements. Each of their
         * elements is displayed as a property of the currently processed
         * object 'obj'.
         */
        for (var prop in obj) {
            if (typeof obj[prop] == 'object' && obj[prop].$category == "Vector"
                && (prop == "children" || prop == "properties")) {
                objString = objString + formatTreeTableView(obj[prop]);
                continue;
            }

            /* 'label' was already used to name the object 'obj'. */
            if (prop == "label" || prop == "$status") {
                continue;
            }

            objString = objString + prop + TAB;

            /* An element in 'properties' could be an object, and that's the
             * case processed here.
             */
            if (typeof obj[prop] == 'object' && obj[prop].$category == "Addr") {
                objString = objString + "0x" + Number(obj[prop]).toString(16);
            }
            else if (typeof obj[prop] == 'number' && obj[prop] > 9) {
                objString = objString + "0x" + obj[prop].toString(16);
            }
            else if (typeof obj[prop] == 'object' && obj[prop] != null) {
                objString = objString + ROW + formatTableObject(obj[prop])
                    + COL;
            }
            else {
                objString = objString + obj[prop];
            }
            objString = objString + SEP;
        }
        return (objString);
    }

    var ret = "";
    for (var i = 0; i < arr.length; i++) {
        /* 'elem' is either a TreeNode structure or Any */
        var elem = arr[i];
        if (typeof elem == 'object') {
            if ("label" in elem) {
                ret = ret + elem.label + TAB;
            }
            else if ("Module" in elem) {
                ret = ret + elem.Module + TAB;
            }
            else {
                ret = ret + i + TAB;
            }
            ret = ret + ROW + formatTableObject(elem) + COL + SEP;
        }
    }
    return (ret);
}

/*
 *  ======== retrieveData =========
 *  Returns data for the module and tab
 */
function retrieveData(modTab)
{
    var modName = "";
    var tabName = "";

    var retValue = "";

    /* Split and obtain module and tab in different strings */
    var index = modTab.indexOf(" ");
    modName = modTab.substring(0, index);
    tabName = modTab.substring(index + 1, modTab.length);

    try {
        if (modName != "" && tabName != "") {
            /* Reset the scanned data */
            Program.resetMods();

            /* if the module name contains Proxy, find the proxy module name */
            if (modName.match(/^Proxy/g)) {
                modName = retrieveProxyModName(modName);
            }

            /* retrieve module view */
            retValue = retrieveView(modName, tabName);

            /* if no data was returned */
            if (retValue === undefined || retValue == "") {
                retValue = errorString("No data to display");
            }
        }
        else {
             throw("Module View for " + modTab + " not found");
        }

    }
    catch (e) {
         /* Catch all exceptions and return a nice error message */
         retValue = errorString(e.message);
    }

    return (retValue);
}

/*
 *  ======== retrieveInstanceDataView ========
 *  Retrieve the specified instance-data-type view
 *
 *  Returns a formatted string representing the view.
 */
function retrieveInstanceDataView(modName, tabName)
{
    var view = "";

    /* Scan instance data type view */
    var instDataArr = Program.scanInstanceDataView(modName, tabName);

    /* Loop through array and format each array data */
    var col = true;
    for each (var instData in instDataArr) {
        if (instData != null) {
            view = view + formatTableView(instData.elements, col,
                                          instData.label);
            col = false;
        }
    }

    return (view);
}

/*
 *  ======== retrieveInstanceView ========
 *  Retrieve the specified instance-type view
 *
 *  Returns a formatted string representing the view.
 */
function retrieveInstanceView(modName, tabName)
{
    var view = "";

    /* Scan instance view */
    var instView = Program.scanInstanceView(modName, tabName);

    /* Pass the view to get a formatted string */
    view = formatTableView(instView);

    return (view);
}

/*
 *  ======== retrieveModuleDataView ========
 *  Retrieve the specified module-data-type view
 *
 *  Returns a formatted string representing the view.
 */
function retrieveModuleDataView(modName, tabName)
{
    var view = "";

    /* Scan the module data type  view*/
    var modDataView = Program.scanModuleDataView(modName, tabName);

    /* pass the view to get a formatted string */
    if (modDataView !== undefined && modDataView != null) {
        view = formatTableView(modDataView.elements);
    }

    return (view);
}

/*
 *  ======== retrieveModuleView ========
 *  Retrieve the specified module-type view
 *
 *  Returns a formatted string representing the view.
 */
function retrieveModuleView(modName, tabName)
{
    var view = "";

    /* Scan the module view */
    var modView = new Array();
    modView[0] = Program.scanModuleView(modName, tabName);

    /* Pass the view to get a formatted string */
    view = formatTableView(modView);

    return (view);
}

/*
 *  ======== retrieveRawView ========
 *  Retrieve the raw view
 *
 *  Returns a formatted string representing the view.
 *  ROW <Property> SEP <Property> SEP ... <Property> SEP
 *
 *  <Property> :: PropName TAB <PrimitiveValue>|<Object>
 *  <Object> :: ROW <Property> SEP <Property> SEP ... <Property> SEP COL
 */
function retrieveRawView(modName)
{
    var view = ROW;

    /* Scan the raw view. The returned object is RawView structure defined in
     * xdc.rov.Program. The structure contains properties 'modState',
     * 'instStates' and 'modCfg'.
     */
    var rawView = Program.scanRawView(modName);

    for (var prop in rawView) {
        // rawView contains modState, instStates, and modCfg
        if (prop == "instStates") {
            view = view + "Instance States";
        }

        if (prop == "modState") {
            view = view + "Module State";
        }
        else if (prop == "modCfg") {
            view = view + "Module Configuration Parameters";
        }
        view = view + TAB + ROW + formatTreeView(rawView[prop], 0) + COL + SEP;
    }
    return (view);
}

/*
 *  ======== retrieveTreeView ========
 *  Retrieve a tree view
 *
 *  Returns a formatted string representing the view.
 *  ROW <Property> SEP <Property> SEP ... <Property> SEP
 *
 *  <Property> :: PropName TAB <PrimitiveValue>|<Object>
 *  <Object> :: ROW <Property> SEP <Property> SEP ... <Property> SEP COL
 *
 *  The formatting is done by the same function used by retrieveRawView()
 *  because the raw view is just a special case of a tree view.
 */
function retrieveTreeView(modName, tabName)
{
    var view = ROW;
    /* scanTreeView returns Any */
    var treeView = Program.scanTreeView(modName, tabName);
    view = view + formatTreeView(treeView, 0);
    return (view);
}

/*
 *  ======== retrieveTreeTableView ========
 *  Retrieve a tree table view
 *
 *  Returns a formatted string representing the view.
 *  ROW <Property> SEP <Property> SEP ... <Property> SEP
 *
 *  <Property> :: PropName TAB <PrimitiveValue>|<Object>
 *  <Object> :: ROW <Property> SEP <Property> SEP ... <Property> SEP COL
 *
 *  The formatting is done by the same function used by retrieveRawView()
 *  because the raw view is just a special case of a tree view.
 */
function retrieveTreeTableView(modName, tabName)
{
    var view = ROW;
    /* scanTreeView returns xdc.rov.Program.TreeNodeArr, an array of TreeNode
     * elements.
     */
    var treeView = Program.scanTreeTableView(modName, tabName);

    view = view + formatTreeTableView(treeView);
    return (view);
}

/*
 *  ======== retrieveLimitedList ========
 *  Retrieve a list of table views
 *
 *  This function is used by the initial version of the plugin where only
 *  table views are displayed. All other views are filtered.
 */
function retrieveLimitedList()
{
    var retStr = ROW;
    var viewInfo = xdc.useModule('xdc.rov.ViewInfo');
    var rawStr = String(viewInfo.RAW);
    var treetableStr = String(viewInfo.TREE_TABLE);
    var treeStr = String(viewInfo.TREE);
    var moduleNames = [];

    /* Copy modules to local array */
    for (var i = 0; i < Program.moduleNames.length; i++) {
         moduleNames[moduleNames.length] = Program.moduleNames[i];
    }

    /* sort it out on the module name (not package name) */
    moduleNames.sort(function (a, b) {
        var s1 = a.substring(a.lastIndexOf('.') + 1).toLowerCase();
        var s2 = b.substring(b.lastIndexOf('.') + 1).toLowerCase();

        if (s1 < s2) {
            return (-1);
        }
        else if (s1 > s2) {
            return (1);
        }
        else {
            return (0);
        }
    });

    /* Loop through the modules and tabs list */
    for each(var module in moduleNames) {
        var tabs = Program.getSupportedTabs(module);
        var len = 0;
        var modflag = true;

        /* if only RAW view is supported, skip module */
        var tabzero = String(tabs[0].type);
        if (tabzero == rawStr) {
            print("Skipping module '" + module + "'...");
            continue;
        }

        /* pack the data into a string */
        while (len < tabs.length) {
            var tab = tabs[len];
            len++;

            var viewType = String(tab.type);
            /* Viewtypes RAW, TREE_TABLE and TREE are not supported */
            if ((viewType == rawStr) || (viewType == treetableStr)
               || (viewType == treeStr)) {
               continue;
            }

            /* Ugly boolean flag, should implement this logic better */
            if (modflag) {
                retStr += module + SEP;
                modflag = false;
            }
            retStr += tab.name + TAB;
        }
        retStr += ROW;
     }

     return (retStr);
}

/*
 *  ======== retrieveModuleList ========
 *  Retrieve the list of the modules
 *
 *  Returns a formatted string containing a list of modules and its views.
 *
 *  The format of string:
 *
 *  ^R mod1 ^S view1 ^T view2 ^T ... ^T ^R mod2 ^S view1 ^T ... ^T ^R
 */
function retrieveModuleList()
{
    var retStr = ROW;
    var viewInfo = xdc.useModule('xdc.rov.ViewInfo');
    var moduleNames = [];

    /* Copy modules to local array */
    for (var i = 0; i < Program.moduleNames.length; i++) {
         moduleNames[moduleNames.length] = Program.moduleNames[i];
    }

    /* sort it out on the module name (not package name) */
    moduleNames.sort(function (a, b) {
        var s1 = a.substring(a.lastIndexOf('.') + 1).toLowerCase();
        var s2 = b.substring(b.lastIndexOf('.') + 1).toLowerCase();

        if (s1 < s2) {
            return (-1);
        }
        else if (s1 > s2) {
            return (1);
        }
        else {
            return (0);
        }
    });

    /* Loop through the modules and tabs list */
    for each (var module in moduleNames) {
        var tabs = Program.getSupportedTabs(module);
        var len = 0;
        var modflag = true;

        /* pack the data into a string */
        while (len < tabs.length) {
            var tab = tabs[len];
            len++;

            /* Ugly boolean flag, should implement this logic better */
            if (modflag) {
                retStr += module + SEP;
                modflag = false;
            }
            retStr += tab.name + TAB + String(tab.type) + TAB;
        }
        retStr += ROW;
     }

     return (retStr);
}

/*
 *  ======== retrieveProxyModName ========
 *  Returns a proxy module's name
 *
 *  Looks in some known packages to find the proxy module name.
 */
function retrieveProxyModName(modName)
{
    var modProxyName = "";
    var pkgs = ["ti\.sysbios\.hal", "xdc\.runtime"];

    if (!modName.match(/^Proxy/g)) {
        throw ("Proxy module cannot be found for this module '"
               + modName + "'");
    }

    /* Get proxy module */
    for each (var pkgName in pkgs) {
        var mod = modName.replace(/Proxy/, pkgName);
        /* Get the configuration details for the module */
        var modCfg = Program.getModuleConfig(mod);

        /* If config not found, then check in other packages */
        if (modCfg == null || modCfg === undefined) {
            continue;
        }

        /* loop through fields and find the proxy field */
        for (var field in modCfg) {
             if (field.match(/Proxy/)) {
                 /* get the name for proxy module and break */
                 modProxyName = modCfg[field].$name;
                 break;
             }
        }

        /* Proxy name is found, no need to look in other packages */
        if (modProxyName != "") {
            break;
        }
    }

    /* Proxy module was not found, throw an exception */
    if (modProxyName == "") {
        throw ("No proxy module was found for "
               + modName.replace(/Proxy\./, "") + "!");
    }

    return (modProxyName);
}

/*
 *  ======== retrieveView ========
 */
function retrieveView(modName, tabName)
{
    var view = "";

    if (Program.getModuleDesc(modName).loadFailed) {
        throw ("ROV failed to load the module " + modName + ", most likely " +
               "because it, or a module it depends on, was not found along " +
               "the package path.\n\n" +

               Program.getModuleDesc(modName).loadFailedMsg + "\n\n" +

               "To set the default path settings, go to " +
               "SYS/BIOS->Preferences and ensure all package repositories" +
               "are set. "
               );
    }

    /* Get the view type to retrieve appropriate view */
    var viewType = Program.getViewType(modName, tabName);

    var ViewInfo = xdc.useModule('xdc.rov.ViewInfo');

    /*
     * The first step in any view is to call the appropriate Program.scan
     * function. We call those here in this try-catch block so the code for
     * handling the exceptions is all in one place.
     */
    var msgOk = "ROV detected errors in scan of";
    try {
        switch (String(viewType)) {
            case String(ViewInfo.MODULE):
                view = retrieveModuleView(modName, tabName);
                break;
            case String(ViewInfo.INSTANCE):
                view = retrieveInstanceView(modName, tabName);
                break;
            case String(ViewInfo.INSTANCE_DATA):
                view = retrieveInstanceDataView(modName, tabName);
                break;
            case String(ViewInfo.MODULE_DATA):
                view = retrieveModuleDataView(modName, tabName);
                break;
            case String(ViewInfo.RAW):
                view = retrieveRawView(modName);
                break;
            case String(ViewInfo.TREE):
                view = retrieveTreeView(modName, tabName);
                break;
            case String(ViewInfo.TREE_TABLE):
                view = retrieveTreeTableView(modName, tabName);
                break;
            default:
                throw (new Error("Undefined view type: " + viewType));
                break;
        }
    }
    catch (e) {
        /*
         * Program.scan* will intentionally throw exceptions to
         * force user to handle them.
         * If this is that type of excpetion, continue on.
         * Otherwise, report the exception directly instead of continuing.
         */
        if (!e.message.substring(0, msgOk.length).equals(msgOk)) {
            /* Strip the Java class name from the exception before re-throwing */
            var msg = e.message;
            if (e.message.indexOf("java.lang.Exception: ") == 0) {
                msg = msg.substring(21);
            }

            throw (new Error(msg));
        }
        else {
            throw (e);
        }
    }

    return (view);
}

/*
 *  ======== startModel ========
 *  Create the MemoryImage, SymbolTable, and CallBack instances, locate the
 *  recap file, and start the ROV Model.
 */
function startModel(executable)
{
    try {
        var Model = xdc.useModule('xdc.rov.Model');

        /* Get the Symbol Table instance */
        var ISymInst =  new Packages.iar.rov.server.SymbolTable();
        var symInst =
            xdc.useModule('iar.rov.server.SymbolTable').create(ISymInst);

        var recap = Model.getRecap(executable);

        /* Get the IMemoryImage instance. */
        var iMemInst = new Packages.iar.rov.server.MemoryImage();

        /* Get the target endianess and convert from string to enum. */
        var endianStr = recap.build.target.model.endian;
        var endian = Packages.xdc.rov.TargetType.strToEndianess(endianStr);

        /* Get the MAU size. */
        var bitsPerChar = recap.build.target.bitsPerChar;
        /* Create the MemoryImage instance */
        var memInst = new Packages.xdc.rov.MemoryImage(iMemInst, endian,
                                                       bitsPerChar);

        /* Get the Callback instance */
        var callBack = xdc.module('iar.rov.server.CallBack').create();

        /* Start the ROV model*/
        Model.start(this.modelVers, executable, recap, symInst, memInst,
                    callBack);

        var callStack = xdc.module('iar.rov.server.CallStack').create(iMemInst);
        Model.setICallStackInst(callStack);
    }
    catch (e) {
        return (errorString(e.message));
    }
}
