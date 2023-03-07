/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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
 * */

/*
 *  ======== UIAMetaData.xs ========
 */
var UIAMetaData;
var xmlFile;
var isEnableDebugPrintf = false;
var linuxHFile;
var isLinuxHFileOpen = false;
var isLoggingSetupActive = false;
var isLogSyncActive = false;
var Program;
var transportArray = new Array();
var uploadMode = new Object();

/* global variable used to store the full module name of the LogSync module */
var nameOfLogSyncModule = "ti.uia.runtime.LogSync";
var nameOfLoggingSetupModule = "ti.uia.sysbios.LoggingSetup";

/*
 * ======== setLogSyncInfo ========
 */
function setLogSyncInfo(moduleName,isEnabled) {
    nameOfLogSyncModule = moduleName;
    isLogSyncActive = isEnabled;
}

/*
 *  ======== setTransportFields ========
 *  Called by the loggers and ServiceMgr.
 */
function setTransportFields(isMultiCoreTransport, mode, transportType, format,
        supportControl, customFields)
{
    var transportObj = new Object();

    transportObj.mode = mode;
    transportObj.isMultiCoreTransport = isMultiCoreTransport;
    transportObj.transportType = transportType;
    transportObj.format = format;
    transportObj.supportControl = supportControl;
    if (transportType == "ETHERNET") {
        transportObj.isNDK = true;
    }
    else {
        transportObj.isNDK = false;
    }
    transportObj.customFields = customFields;

    var index = transportArray.length;
    transportArray.length++;
    transportArray[index] = transportObj;
}

/* ======== setLoggingSetupConfigured ========
 * The LoggingSetup module is a metaonly module that
 * configures the transport used to upload events, as well
 * as many other logging-related options.  Since it is
 * metaonly, it is not included in the list of Program.targetModules()
 * so this function is called by the LoggingSetup.module$use function
 * to 'register' it with the UIAMetaData script.
 */
function setLoggingSetupConfigured() {
    isLoggingSetupActive = true;
}

/* ======== isLoggingSetupConfigured ========
 * The LoggingSetup module is a metaonly module that
 * configures the transport used to upload events, as well
 * as many other logging-related options.  Since it is
 * metaonly, it is not included in the list of Program.targetModules()
 * so this function can be called by other modules to check
 * if LoggingSetup can be used to determine which transport to use.
 */
function isLoggingSetupConfigured() {
    return isLoggingSetupActive;
}

/*
 *  ======== module$use ========
 */
function module$use() {
    var Log = xdc.useModule('xdc.runtime.Log');
    var Text = xdc.useModule('xdc.runtime.Text');
    var Diags = xdc.useModule('xdc.runtime.Diags');
    var xdc_runtime_Log = xdc.useModule('xdc.runtime.Log');
    var ILogger = xdc.useModule('xdc.runtime.ILogger');
    var DvtTypes = xdc.useModule('ti.uia.events.DvtTypes');
    UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');
    var IUIATransfer = xdc.useModule('ti.uia.runtime.IUIATransfer');
    Program = xdc.module('xdc.cfg.Program');

    var BIOS = undefined;
    try {
        BIOS = xdc.module('ti.sysbios.BIOS');
    }
    catch (e) {
    }

    if (BIOS == undefined) {
        /*
         *  Not a BIOS program.  In that case, make sure the cpu and
         *  timestamp frequencies have been configured.
         */
        if (UIAMetaData.cpuFreq.lo == undefined) {
            this.$logError("You must set the UIAMetaData cpu frequency for " +
                    "non-BIOS programs. For example, if the PLL is being " +
                    "configured for 100MHz, add this to your application .cfg " +
                    "script: UIAMetaData.cpuFreq.lo = 100000000",
                    this, "cpuFreq.log");
        }
        if (UIAMetaData.timestampFreq.lo == undefined) {
            this.$logError("You must set the UIAMetaData timestamp frequency for " +
                    "non-BIOS programs. For example, if the PLL is being " +
                    "configured for 100MHz, add this to your application .cfg " +
                    "script: UIAMetaData.timestampFreq.lo = 100000000",
                    this, "timestampFreq.log");
        }
    }

    if ((UIAMetaData.cpuFreq.lo != undefined) || (UIAMetaData.cpuFreq.hi != undefined)) {
        dbgprint("UIA: module$use(): Setting overrideCpuFreq to true");
        print("***** UIA: module$use(): Setting overrideCpuFreq to true");
        UIAMetaData.overrideCpuFreq = true;
    }
    if ((UIAMetaData.timestampFreq.lo != undefined) || (UIAMetaData.timestampFreq.hi != undefined)) {
        dbgprint("UIA: module$use(): Setting overrideTimestampFreq to true");
        print("***** UIA: module$use(): Setting overrideTimestampFreq to true");
        UIAMetaData.overrideTimestampFreq = true;
    }

    /* Cannot leave these unset, or we'll get a build error. */
    if (UIAMetaData.cpuFreq.lo == undefined) {
        UIAMetaData.cpuFreq.lo = 0;
    }
    if (UIAMetaData.timestampFreq.lo == undefined) {
        UIAMetaData.timestampFreq.lo = 0;
    }
    if (UIAMetaData.cpuFreq.hi == undefined) {
        UIAMetaData.cpuFreq.hi = 0;
    }
    if (UIAMetaData.timestampFreq.hi == undefined) {
        UIAMetaData.timestampFreq.hi = 0;
    }
}

/*
 *  ======== toHex ========
 */
function toHex( d) {
    var r = d % 16;
    var result;
    if (d - r == 0)
        result = toChar(r);
    else
        result = toHex((d - r) / 16) + toChar(r);
    return result;
}

/*
 *  ======== toChar ========
 */
function toChar( n) {
    const alpha = "0123456789ABCDEF";
    return alpha.charAt(n);
}

/*
 *  ======== getXmlStr ========
 */
function getXmlStr( str) {
    var ch = ' ';
    var i = 0;
    var strHex = 0;
    var xmlStr = "";
    var newStr = str;
    /* str.replace(/&/g, "&amp;").replace(/\"/g, "&quot;").replace(/</g, "&lt;").replace(/>/g, "&gt;"); */
    for (index = 0; index < str.length; index++) {
        i = str.charCodeAt(index);
        if ((i < 0x20) || ((i >= 0x22) && (i <= 0x29)) || (i == 0x2C) || (i
                == 0x2F) || ((i >= 0x3A) && (i <= 0x3E)) || (i == 0x5C) || (i
                        > 0x7E)) {
            strHex = "%" + toHex(i);
            xmlStr = xmlStr + strHex;
        } else {
            if (i != 0x20) {
                xmlStr = xmlStr + str.charAt(index);
            } else {
                xmlStr = xmlStr + "+";
            }
        }

    }
    return xmlStr;
}

/*
 *  ======== genXmlEntryLeadSpace ========
 */
function genXmlEntryLeadSpace( indent, file) {
    var space = ' ';
    for (var i = 0; i < indent; i++) {
        xmlFile.write(space);
    }
}

/*bc: New for Linux [...                                  */

/*
 *  ======== openLinuxHFile ========
 */
function openLinuxHFile(moduleLongName,moduleShortName,rootDir){
    var longDashName = moduleLongName.replace(/\./g,"_");
    if (isEnableDebugPrintf)
        print("\nUIA openLinuxHFile - checking "+moduleLongName+"...");
    if (moduleLongName.indexOf("ti.uia.events") >= 0) {
        if (isEnableDebugPrintf)
            print("UIA Linux events for: "+moduleShortName);
        isLinuxHFileOpen = true;
        linuxHFile = new java.io.FileWriter(rootDir+"/ti/uia/events/"+
                moduleShortName+".h");
        linuxHFile.write("/* Autogenerated file - do not edit! */\n");
        linuxHFile.write("#ifndef "+ longDashName +"__include\n");
        linuxHFile.write("#define "+ longDashName +"__include\n\n");

        linuxHFile.write("#ifdef __cplusplus\n");
        linuxHFile.write("#define __extern extern \"C\"\n");
        linuxHFile.write("#else\n");
        linuxHFile.write("#define __extern extern\n");
        linuxHFile.write("#endif\n\n");

        linuxHFile.write("#define ti_uia_events_"+moduleShortName+
                "___VERS 150\n\n");
        linuxHFile.write("/*\n");
        linuxHFile.write(" * ======== INCLUDES ========\n");
        linuxHFile.write(" */\n\n");

        /*      linuxHFile.write("#include <xdc/std.h>\n\n");     */

        /*      linuxHFile.write("#include <xdc/runtime/xdc.h>\n"); */
        /*      linuxHFile.write("#include <xdc/runtime/Types.h>\n\n"); */

    } else if (moduleLongName.indexOf("ti.uia.runtime.Log") >= 0) {
        /* common code for both Log, LogCtxChg, and LogSnapshot modules... */
        /* (NOTE: module must be in the module$use section)  */
        if (isEnableDebugPrintf)
            print("UIA Linux events for: "+moduleShortName);
        isLinuxHFileOpen = true;
        linuxHFile = new java.io.FileWriter(rootDir+"/ti/uia/runtime/"+
                moduleShortName+".h");
        linuxHFile.write("/* Autogenerated file - do not edit! */\n");
        linuxHFile.write("#ifndef "+ longDashName +"__include\n");
        linuxHFile.write("#define "+ longDashName +"__include\n\n");

        linuxHFile.write("#ifdef __cplusplus\n");
        linuxHFile.write("#define __extern extern \"C\"\n");
        linuxHFile.write("#else\n");
        linuxHFile.write("#define __extern extern\n");
        linuxHFile.write("#endif\n\n");

        linuxHFile.write("#define ti_uia_events_"+moduleShortName+
                "___VERS 150\n\n");
        linuxHFile.write("/*\n");
        linuxHFile.write(" * ======== INCLUDES ========\n");
        linuxHFile.write(" */\n\n");

        linuxHFile.write("#include <xdc/std.h>\n\n");

        linuxHFile.write("#include <xdc/runtime/xdc.h>\n");
        linuxHFile.write("#include <xdc/runtime/Types.h>\n");
        linuxHFile.write("#include <ti/uia/runtime/"+moduleShortName+
                "__prologue.h>\n");
        linuxHFile.write("#include <ti/uia/runtime/package/package.defs.h>\n\n");

        linuxHFile.write("/*\n");
        linuxHFile.write(" * ======== EPILOGUE ========\n");
        linuxHFile.write(" */\n\n");

        linuxHFile.write("#include <ti/uia/runtime/"+moduleShortName+
                "__epilogue.h>\n\n");
        linuxHFile.write("/*\n");
        linuxHFile.write(" * ======== GENERATED EVENT CONSTANTS ========\n");
        linuxHFile.write(" */\n\n");

    } else {
        isLinuxHFileOpen = false;
    }
}

/*
 *  ======== genLinuxHFileEntry ========
 */
function genLinuxHFileEntry(longEventName,shortEventName,strEventId,msg){
    if (isLinuxHFileOpen){
        var longDashName = longEventName.replace(/\./g,"_");
        linuxHFile.write("#define "+ longDashName +" ("+strEventId+")\n" );
        linuxHFile.write("#define "+ longDashName+"__msg \""+msg+"\" \n" );
        linuxHFile.write("#define "+ shortEventName +" ("+strEventId+")\n" );
        linuxHFile.write("#define "+ shortEventName+"__msg \""+msg+"\" \n" );
    }
}

/*
 *  ======== closeLinuxHFile ========
 */
function closeLinuxHFile(){
    if (isLinuxHFileOpen) {
        linuxHFile.write("\n#endif\n");
        linuxHFile.flush();
        linuxHFile.close();
    }
    isLinuxHFileOpen = false;
}
/*...]                                                    */

/*
 *  ======== genXmlEntry ========
 *  write an entry into an xml file
 *
 *  e.g. <myTag>myValue</myTag>\n
 */
function genXmlEntry( indent, tag, value) {
    var strValue = "";
    strValue = value; /* ensure that value is converted into a string */
    genXmlEntryLeadSpace(indent);
    xmlFile.write("<");
    xmlFile.write(tag);
    xmlFile.write(">");
    xmlFile.write(strValue);
    xmlFile.write("</");
    xmlFile.write(tag);
    xmlFile.write(">\n");
}

/*
 *  ======== genXmlEntryOpen ========
 *  write an entry into an xml file
 *  e.g. <myTag>
 */
function genXmlEntryOpen( indent, tag) {
    genXmlEntryLeadSpace(indent);
    xmlFile.write("<");
    xmlFile.write(tag);
    xmlFile.write(">\n");
}

/*
 *  ======== genXmlEntryOpenWithKey ========
 *  write an entry into an xml file with a key field
 *
 *  e.g. <myTag myKeyName=myKeyValue>
 */
function genXmlEntryOpenWithKey( indent, tag, keyName, keyValue, value) {
    var strKeyValue = "";
    strKeyValue = keyValue; /* ensure that keyValue is converted into a string*/
    genXmlEntryLeadSpace(indent);
    xmlFile.write("<");
    xmlFile.write(tag);
    xmlFile.write(" ");
    xmlFile.write(keyName);
    xmlFile.write("=\"");
    xmlFile.write(strKeyValue);
    xmlFile.write("\">\n");
}

/*
 *  ======== genXmlEntryClose ========
 *  write an entry into an xml file with a key field
 *
 *  e.g. </myTag>\n
 */
function genXmlEntryClose( indent, tag) {
    genXmlEntryLeadSpace(indent);
    xmlFile.write("</");
    xmlFile.write(tag);
    xmlFile.write(">\n");
}
/*
 *  ======== genXmlEntryWithKey ========
 *  write an entry into an xml file with a key field
 *
 *  e.g. <myTag myKeyName=myKeyValue>myValue</myTag>\n
 */
function genXmlEntryWithKey( indent, tag, keyName, keyValue, value) {
    var strKeyValue = "";
    var strValue = "";
    strKeyValue = keyValue; /* ensure that keyValue is converted into a string*/
    strValue = value; /* ensure that value is converted into a string */
    genXmlEntryLeadSpace(indent);
    xmlFile.write("<");
    xmlFile.write(tag);
    xmlFile.write(" ");
    xmlFile.write(keyName);
    xmlFile.write("=");
    xmlFile.write(strKeyValue);
    xmlFile.write(">");
    xmlFile.write(value);
    xmlFile.write("</");
    xmlFile.write(tag);
    xmlFile.write(">\n");
}
/*
 * ======== genXmlComment ========
 * add an XML style comment to the active xml file
 */
function genXmlComment( indent, comment) {
    genXmlEntryLeadSpace(indent);
    xmlFile.write("<!-- " + comment + " -->\n");
}

function genHdrXml() {
    genXmlEntryOpen(3, "hdr");
    genXmlEntryOpen(6, "hdrType");
    genXmlEntry(9, "word", "0");
    genXmlEntry(9, "width", "5");
    genXmlEntry(9, "lsb", "27");
    genXmlEntryOpenWithKey(9, "enumValue", "value", "0");
    genXmlEntry(12, "name", "Event");
    genXmlEntryClose(9, "enumValue");
    genXmlEntryOpenWithKey(9, "enumValue", "value", "1");
    genXmlEntry(12, "name", "EventWithTimestamp");
    genXmlEntryClose(9, "enumValue");
    genXmlEntryOpenWithKey(9, "enumValue", "value", "2");
    genXmlEntry(12, "name", "EventWithSnapshotId");
    genXmlEntryClose(9, "enumValue");
    genXmlEntryOpenWithKey(9, "enumValue", "value", "3");
    genXmlEntry(12, "name", "EventWithSnapshotIdAndTimestamp");
    genXmlEntryClose(9, "enumValue");
    genXmlEntryClose(6, "hdrType");
    genXmlEntryOpen(6, "hdrLayout");
    genXmlComment(9,
            "Header for an event with no timestamps or calling address");
    genXmlEntryOpenWithKey(9, "header", "name", "Event");
    genXmlEntry(12, "numWords", "2");
    genXmlEntryOpenWithKey(12, "word", "index", "0");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "lengthInMAUs");
    genXmlEntry(18, "width", "11");
    genXmlEntry(18, "lsb", "16");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "sequenceID");
    genXmlEntry(18, "width", "16");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryOpenWithKey(12, "word", "index", "1");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "eventID");
    genXmlEntry(15, "width", "16");
    genXmlEntry(15, "lsb", "16");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "moduleID");
    genXmlEntry(15, "width", "16");
    genXmlEntry(15, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryClose(9, "header");

    genXmlComment(9, "Header for an event with 64b timestamp");
    genXmlEntryOpenWithKey(9, "header", "name", "EventWithTimestamp");
    genXmlEntry(12, "numWords", "4");
    genXmlEntryOpenWithKey(12, "word", "index", "0");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "lengthInMAUs");
    genXmlEntry(18, "width", "11");
    genXmlEntry(18, "lsb", "16");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "sequenceID");
    genXmlEntry(18, "width", "16");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryOpenWithKey(12, "word", "index", "1");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "timestampLSW");
    genXmlEntry(18, "width", "32");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryOpenWithKey(12, "word", "index", "2");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "timestampMSW");
    genXmlEntry(18, "width", "32");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryOpenWithKey(12, "word", "index", "3");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "eventID");
    genXmlEntry(18, "width", "16");
    genXmlEntry(18, "lsb", "16");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "moduleID");
    genXmlEntry(18, "width", "16");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryClose(9, "header");

    genXmlComment(9, "Header for a snapshot event (no timestamps)");
    genXmlEntryOpenWithKey(9, "header", "name", "EventWithSnapshotId");
    genXmlEntry(12, "numWords", "2");
    genXmlEntryOpenWithKey(12, "word", "index", "0");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "lengthInMAUs");
    genXmlEntry(18, "width", "11");
    genXmlEntry(18, "lsb", "16");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "sequenceID");
    genXmlEntry(18, "width", "16");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryOpenWithKey(12, "word", "index", "1");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "eventID");
    genXmlEntry(18, "width", "16");
    genXmlEntry(18, "lsb", "16");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "moduleID");
    genXmlEntry(18, "width", "16");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryClose(9, "header");

    genXmlComment(9, "Header for a snapshot event with 64b timestamps");
    genXmlEntryOpenWithKey(9, "header", "name", "EventWithSnapshotIdAndTimestamp");
    genXmlEntry(12, "numWords", "4");
    genXmlEntryOpenWithKey(12, "word", "index", "0");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "lengthInMAUs");
    genXmlEntry(18, "width", "11");
    genXmlEntry(18, "lsb", "16");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "sequenceID");
    genXmlEntry(18, "width", "16");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryOpenWithKey(12, "word", "index", "1");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "timestampLSW");
    genXmlEntry(18, "width", "32");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryOpenWithKey(12, "word", "index", "2");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "timestampMSW");
    genXmlEntry(18, "width", "32");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryOpenWithKey(12, "word", "index", "3");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "eventID");
    genXmlEntry(18, "width", "16");
    genXmlEntry(18, "lsb", "16");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryOpenWithKey(15, "bitfield", "name", "moduleID");
    genXmlEntry(18, "width", "16");
    genXmlEntry(18, "lsb", "0");
    genXmlEntryClose(15, "bitfield");
    genXmlEntryClose(12, "word");
    genXmlEntryClose(9, "header");
    genXmlEntryClose(6, "hdrLayout");
    genXmlEntryClose(3, "hdr");
}

/*
 *  ======== generateServiceMetaData ========
 * generates XML metadata describing the services
 */
function generateServiceMetaData(indent)
{
    var ServiceMgr = xdc.module("ti.uia.runtime.ServiceMgr");

    genXmlEntryOpen(indent,"services");

    for each (var mod in Program.targetModules()) {
        strModulePackageName = mod.$package.$name;
        if (strModulePackageName.indexOf(".services") > 0){
            for each (var prop in mod) {
                if (prop instanceof ServiceMgr.ServiceId) {
                    genXmlEntryOpen(indent+3,"service");
                    genXmlEntry(indent+6,"name",mod.$name);
                    genXmlEntry(indent+6,"serviceId",prop.val.toString());
                    genXmlEntryClose(indent+3,"service");
                    break;
                }
            }
        }
    }
    genXmlEntryClose(indent,"services");
}

/*
 *  ======== returnUploadMetaData ========
 *  returns the uplaod mode string
 */
function returnUploadMetaData(mode, indent)
{
    var mode;

    switch (mode) {
        case UIAMetaData.UploadMode_SIMULATOR:
        case UIAMetaData.UploadMode_PROBEPOINT:
            mode = "SIMULATOR";
            break;

        case UIAMetaData.UploadMode_JTAGSTOPMODE:
            mode = "JTAGSTOPMODE";
            break;

        case UIAMetaData.UploadMode_JTAGRUNMODE:
            mode = "JTAGRUNMODE";
            break;

        case UIAMetaData.UploadMode_NONJTAGTRANSPORT:
            mode = "NONJTAGTRANSPORT";
            break;

        case UIAMetaData.UploadMode_CUSTOM:
            mode = "CUSTOM";
            break;

        default:
            mode = "NONE";
    }

    return (mode);
}

/*
 *  ======== generateTransportMetaData ========
 * generates XML metadata describing the transports used to upload event data
 */
function generateTransportMetaData(indent)
{
    for each (var transportObj in transportArray) {

        /* Hack until CCS supports stopmode/runmode and other transports */
        if (transportArray.length > 1) {
            if ((transportObj.mode == UIAMetaData.UploadMode_JTAGSTOPMODE) ||
                    (transportObj.mode == UIAMetaData.UploadMode_JTAGRUNMODE)) {
                continue;
            }
        }

        genXmlEntryOpen(indent, "transport2");
        genXmlEntry(indent+3,"uploadMode", returnUploadMetaData(transportObj.mode));
        genXmlEntry(indent+3,"isMultiCoreTransport", transportObj.isMultiCoreTransport);
        genXmlEntry(indent+3,"supportControl", transportObj.supportControl.toString());
        genXmlEntry(indent+3,"format", transportObj.format);
        genXmlEntry(indent+3,"transportType", transportObj.transportType);
        if (transportObj.customFields != null) {
            if (transportObj.customFields.indexOf(",") > 0) {
                genXmlEntryOpen(indent+3,"customFields");
                var array = transportObj.customFields.split(',');
                for each (var entry in array) {
                    if (entry.indexOf("=") > 0){
                        var field = entry.split('=');
                        genXmlEntry(indent+6, field[0], field[1]);
                    }
                }
                genXmlEntryClose(indent+3, "customFields");
            }
        }
        genXmlEntryClose(indent, "transport2");
    }

    genXmlEntryOpen(indent, "transport");
    genXmlComment(indent, "This tag must be here for DVT to work. All data is in transport2");
    genXmlEntryClose(indent, "transport");
}

/*
 *  ======== printModuleSpecInfo ========
 *  Function that can be called to print out info about a particular module
 *  (for debugging purposes)
 */
function printModuleSpecInfo(mod, description){
    var modName ="undefined";

    try{
        modName = mod.$name;
    }
    catch(ex){
    }

    print("\nModuleSpecInfo for module " + modName + "["+description+"]");
    print ("mod: "+mod);
    var hasSpec = false;
    try{
        if (mod.$spec != null){
            hasSpec = true;
        }
    }
    catch(ex){

    }
    if (hasSpec){
        var fxns = mod.$spec.getFxns().toArray();
        for (var i = 0; i < fxns.length; i++) {
            var f = fxns[i];
            var strSig = "";
            if (f.getType() != null)
                strSig = f.getType().tsig();
            print("Function:"+f.getName() + ": " + strSig);
        }
        var modProxies = mod.$spec.getProxies().toArray();
        for (var i = 0; i < modProxies.length; i++) {
            var m = modProxies[i];
            print("Proxy:"+m.getName());
        }
        var modConfigs = mod.$spec.getConfigs().toArray();
        for (var i = 0; i < modConfigs.length; i++) {
            var c = modConfigs[i];
            print("Config:"+c.getName());
        }
        print("All specs:[...");
        var i = 0;
        for each (var item in mod.$spec){
            print("UIA: "+i.toString()+": "+item+"\n");
            i++;
        }
        print("...]");
    } else {
        print("mod.$spec = null.");
    }
    print ("End of info for module "+description+"\n");
}

/*
 *  ======== generateCpuClockEntry ========
 */
function generateCpuClockEntry(cpuClkMSW, cpuClkLSW, canFrequencyBeChanged)
{
    var indent = 3;

    /* CPU clock settings */
    genXmlEntryOpen(indent,"cpuClock");
    indent += 3;
    genXmlEntry(indent, "maxClockFreqMSW", "" + cpuClkMSW);
    genXmlEntry(indent, "maxClockFreqLSW", "" + cpuClkLSW);
    genXmlEntry(indent, "canFrequencyBeChanged", canFrequencyBeChanged);
    indent -= 3;
    genXmlEntryClose(indent,"cpuClock");
}

/*
 *  ======== generateTargetDataEntry ========
 */
function generateTargetDataEntry()
{
    var indent = 3;
    var ptrSize = Program.build.target.stdTypes.t_Ptr.size;
    var argSize = Program.build.target.stdTypes.t_IArg.size;
    var wordSize = Program.build.target.stdTypes.t_Int.size;

    /* Target size of UArg and Int (bytes) */
    genXmlEntryOpen(indent,"targetData");
    indent += 3;
    genXmlEntry(indent, "ptrSize", "" + ptrSize);
    genXmlEntry(indent, "argSize", "" + argSize);
    genXmlEntry(indent, "wordSize", "" + wordSize);
    indent -= 3;
    genXmlEntryClose(indent,"targetData");
}

/*
 *  ======== generateTimestampEntry ========
 */
function generateTimestampEntry(deviceName, ctsMod, timerMSW, timerLSW, cyclesPerTick, eventCorrelation)
{
    var indent = 3;

    dbgprint("UIA: generateTimestampEntry: timerLSW = " + timerLSW + ", cyclesPerTick = " +
            cyclesPerTick + ", eventCorrelation = " + eventCorrelation);

    /* Event correlation settings */
    if (!isLogSyncActive){
        dbgprint("UIA: isLogSyncActive = false, generating timestamp entry");
        genXmlEntryOpen(indent,"eventCorrelation");
        indent += 3;
        genXmlEntry(indent,"isEnabled", "" +eventCorrelation);

        /* CPU timestamp settings */
        if (deviceName != "") {
            genXmlEntryOpen(indent,"cpuTimestamp");
            indent += 3;
            genXmlComment(indent, deviceName + " device uses an internal timestamp");
            genXmlEntry(indent,"maxTimerFreqMSW", "" + timerMSW);
            genXmlEntry(indent,"maxTimerFreqLSW", "" + timerLSW);
            genXmlEntry(indent,"cyclesPerTick", "" + cyclesPerTick);
            indent -= 3;
            genXmlEntryClose(indent,"cpuTimestamp");
        }
        else {
            genXmlEntryOpen(indent,"cpuTimestamp");
            indent += 3;
            genXmlComment(indent,"Not a supported device. Uses a timestamp proxy.  Frequency unknown.");
            genXmlEntry(indent,"moduleName", ctsMod.delegate$.$name);
            genXmlEntry(indent,"moduleId", ctsMod.delegate$.Module__id.toString());
            indent -= 3;
            genXmlEntryClose(indent,"cpuTimestamp");
            print("remark: No frequency info found for CPU timestamp.  See ti.uia.runtime.LogSync module for help on how to provide this info for System Analyzer.");
        }
        indent -= 3;
        genXmlEntryClose(indent,"eventCorrelation");
    }
    else {
        dbgprint("UIA: isLogSyncActive = true, not generating timestamp entry");
    }
}

/*
 *  ======== generate ========
 */
function generate(isGenerateLinuxFiles, linuxFilesRootDir) {
    var Text = xdc.module('xdc.runtime.Text');
    var Diags = xdc.module('xdc.runtime.Diags');
    var xdc_runtime_Log = xdc.module('xdc.runtime.Log');
    var ti_uia_runtime_LogSnapshot = xdc.module('ti.uia.runtime.LogSnapshot');
    var ILogger = xdc.module('xdc.runtime.ILogger');
    var DvtTypes = xdc.module('ti.uia.events.DvtTypes');
    var numModules = 0;
    var dvtEventDescriptor;

    if (isEnableDebugPrintf) {
        dbgprint("UIA: ti.uia.runtime.UIAMetaData.xs generate() function called:"
                + " isGenerateLinuxFiles = " + isGenerateLinuxFiles +
                ", LinuxFileRootDir = " + linuxFilesRootDir);
    }

    /*
     * Write out an XML file that contains the
     * module IDs, event IDs and diag mask info
     */
    var strEventId = "";
    var strEventMsg = "";
    var strEventName = "";
    var strModuleId = "";
    var strModuleName = "";
    var strModulePackageName = "";
    var strPkgVersion = "";
    var strModuleSlashName = "";
    var strModulePackageBase = "";
    var strModuleCFileName = "";
    var strModuleCFilePath = "";
    var strShortName = "";
    var outputDir;
    var outputDirExists = false;

    if (Program == undefined) {
        Program = xdc.module('xdc.cfg.Program');
    }
    if (!isGenerateLinuxFiles) {
        /*
         * Specify the absolute path to the UIA xml file. This addresses an
         * issue with gconf where the validation is done from a different
         * directory.
         */
        var foldername = xdc.om.$homepkg.$spec.baseDir + "/package/cfg";
        dbgprint("***** UIAMetaData.generate(): foldername = " + foldername);
        var filename = foldername + "/" + Program.name.replace('.', '_') + ".uia.xml";
        outputDirExists = (new java.io.File(foldername)).exists();

        if (outputDirExists){
            dbgprint("UIA: ti.uia.runtime.UIAMetaData.generateXML: creating " +
                    filename);
            xmlFile = new java.io.FileWriter(filename);
        } else {
            dbgprint("UIA: ti.uia.runtime.UIAMetaData.generateXML: output folder does not exist: " +
                    foldername);
        }

    } else {
        outputDirExists = true;
        dbgprint("UIA: ti.uia.runtime.UIAMetaData.generateLinuxFiles: " +
                "creating " + linuxFilesRootDir + "/uiaEventData.xml");
        outputDir = new java.io.File(linuxFilesRootDir);

        /* if the directory does not exist, create it    */
        if (!outputDir.exists()) {
            dbgprint("ti.uia.runtime.UIAMetaData.generateLinuxFiles: " +
                    "creating directory: " + linuxFilesRootDir);
            outputDir.mkdir();
        }
        /* also create sub directories if required       */
        outputDir = new java.io.File(linuxFilesRootDir + "/ti");
        if (!outputDir.exists()) {
            outputDir.mkdir();
        }
        outputDir = new java.io.File(linuxFilesRootDir + "/ti/uia");
        if (!outputDir.exists()) {
            outputDir.mkdir();
        }
        outputDir = new java.io.File(linuxFilesRootDir + "/ti/uia/events");
        if (!outputDir.exists()) {
            outputDir.mkdir();
        }
        outputDir = new java.io.File(linuxFilesRootDir + "/ti/uia/runtime");
        if (!outputDir.exists()) {
            outputDir.mkdir();
        }
        outputDir = new java.io.File(linuxFilesRootDir + "/xdc");
        if (!outputDir.exists()) {
            outputDir.mkdir();
        }
        outputDir = new java.io.File(linuxFilesRootDir + "/xdc/runtime");
        if (!outputDir.exists()) {
            outputDir.mkdir();
        }
        xmlFile = new java.io.FileWriter(linuxFilesRootDir + "/uiaEventData.xml");
    }

    var flag = false;
    var lastDotIndex = 0;
    var strMetaEventName = "";
    var strMetaEvent = "";
    var is64bTimestampIncluded = true;
    var is32bCallingAdrsIncluded = false;
    var logger = ti_uia_runtime_LogSnapshot.loggerObj;
    /*bc: NEW for Linux [...                              */
    var strModuleShortName = "";
    var strModuleLongName = "";
    /*...]                                                */

    if (outputDirExists) {
        xmlFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        xmlFile.write("<uiaEventData>\n");
        genHdrXml();

        for each (var mod in Program.targetModules()) {

            if (mod.PROXY$) {
                continue;
            }

            /* create an Id for all Log_Event's declared in the module */
            if ('$$logEvtCfgs' in mod && mod.$$logEvtCfgs.length > 0){
                numModules++;
                lastDotIndex = mod.$name.lastIndexOf('.') + 1;
                /*bc: NEW for Linux [...                      */
                if (isGenerateLinuxFiles) {
                    strModuleShortName = mod.$name.substring(lastDotIndex,
                            mod.$name.length);

                    strModuleLongName = mod.$name;
                    openLinuxHFile(strModuleLongName,strModuleShortName,
                            linuxFilesRootDir);
                }
                /*...]                                        */
                for each (var cn in mod.$$logEvtCfgs) {
                    var desc = mod[cn];
                    strEventId = desc.$private.id.toString();
                    strEventMsg = desc.msg;
                    strEventMsg.replace("%$F","%s, Line %d");
                    strEventMsg.replace("%$S","%s");
                    strEventName = mod.$name + '.' + cn;
                    strShortName =
                            mod.$name.substring(lastDotIndex,mod.$name.length) + '_' + cn;
                    strMetaEventName = "metaEvent"+cn.substring(0,1).toUpperCase()+
                            cn.substring(1,cn.length);
                    strModuleId = mod.Module__id.toString();
                    genXmlEntryOpenWithKey(3,"evtMap","id",strEventId);
                    genXmlEntry(6,"longName",strEventName);
                    genXmlEntry(6,"shortName",strShortName);
                    genXmlEntry(6,"moduleId",strModuleId);
                    genXmlEntry(6,"diagMask",desc.mask.toString());
                    genXmlEntry(6,"msg",getXmlStr(strEventMsg));
                    /*bc: NEW for Linux [...                  */
                    if (isGenerateLinuxFiles) {
                        genLinuxHFileEntry(strEventName, strShortName,
                                strEventId, strEventMsg);
                    }
                    /*...]                                    */
                    var strEventType = "";
                    flag = false;
                    if ((desc.mask & Diags.STATUS) != 0) {
                        if ((desc.level & (Diags.LEVEL)) == (Diags.EMERGENCY)) {
                            strEventType = strEventType + "EMERGENCY";
                            flag = true;
                        }
                        if ((desc.level & (Diags.LEVEL)) == (Diags.CRITICAL)) {
                            strEventType = strEventType + "CRITICAL";
                            flag = true;
                        }
                        if ((desc.level & (Diags.LEVEL)) == (Diags.ERROR)) {
                            strEventType = strEventType + "ERROR";
                            flag = true;
                        }
                        if ((desc.level & Diags.LEVEL) == (Diags.WARNING)) {
                            if (flag) strEventType = strEventType + " | ";
                            flag = true;
                            strEventType = strEventType + "WARNING";
                        }
                    }
                    if ((desc.mask & Diags.INFO) != 0) {
                        if ((desc.level & Diags.LEVEL) != (Diags.LEVEL4)) {
                            if (flag) strEventType = strEventType + " | ";
                            flag = true;
                            strEventType = strEventType + "INFO";
                        } else {
                            if (flag) strEventType = strEventType + " | ";
                            flag = true;
                            strEventType = strEventType + "DETAILS";
                        }
                    }
                    if ((desc.mask & Diags.ANALYSIS) != 0) {
                        if (flag) strEventType = strEventType + " | ";
                        flag = true;
                        strEventType = strEventType + "ANALYSIS";
                    }
                    if ((desc.mask & Diags.ENTRY) != 0) {
                        if (flag) strEventType = strEventType + " | ";
                        flag = true;
                        strEventType = strEventType + "ENTRY";
                    }
                    if ((desc.mask & Diags.EXIT) != 0) {
                        if (flag) strEventType = strEventType + " | ";
                        flag = true;
                        strEventType = strEventType + "EXIT";
                    }
                    if ((desc.mask & Diags.LIFECYCLE) != 0) {
                        if (flag) strEventType = strEventType + " | ";
                        flag = true;
                        strEventType = strEventType + "LIFECYCLE";
                    }
                    if ((desc.mask & Diags.INTERNAL) != 0) {
                        if (flag) strEventType = strEventType + " | ";
                        flag = true;
                        strEventType = strEventType + "INTERNAL";
                    }
                    if ((desc.mask & Diags.ASSERT) != 0) {
                        if (flag) strEventType = strEventType + " | ";
                        flag = true;
                        strEventType = strEventType + "ASSERT";
                    }
                    if (isEnableDebugPrintf) {
                        if ((strShortName.indexOf("Log_L_") >= 0) || (strEventType.length > 0)) {
                            print("UIA "+ strShortName + ": strEventType = "
                                    + strEventType);
                        }
                    }

                    if (strEventType.length > 0) {
                        genXmlEntry(6,"eventType",strEventType);
                    }

                    if ((desc.mask & Diags.ANALYSIS) != 0) {
                        if (strMetaEventName in mod) {
                            dvtEventDescriptor = mod[strMetaEventName];
                            if (dvtEventDescriptor != undefined) {
                                if (dvtEventDescriptor instanceof
                                        DvtTypes.MetaEventDescriptor) {
                                    genXmlEntryOpenWithKey(6,"dvtEventDescriptor",
                                            "name",strMetaEventName);
                                    genXmlEntry(9,"versionId",
                                            dvtEventDescriptor.versionId);
                                    genXmlEntry(9,"analysisType",
                                            dvtEventDescriptor.analysisType);

                                    if (dvtEventDescriptor.analysisType ===
                                            DvtTypes.DvtAnalysisType_CUSTOM) {
                                        var strCustomType = "null";
                                        if ("strCustomAnalysisType" in
                                                dvtEventDescriptor) {
                                            if (dvtEventDescriptor.
                                                    strCustomAnalysisType !=
                                                            undefined) {
                                                strCustomType = dvtEventDescriptor.
                                                        strCustomAnalysisType;
                                            }
                                        }
                                        genXmlEntry(9,"strCustomAnalysisType",
                                                strCustomType);
                                    }

                                    genXmlEntry(9,"displayText",
                                            dvtEventDescriptor.displayText);
                                    genXmlEntry(9,"tooltipText",
                                            dvtEventDescriptor.tooltipText);
                                    var boolIsMultiEventRecItem = false;
                                    if ("isMultiEventRecordItem" in
                                            dvtEventDescriptor) {
                                        if (dvtEventDescriptor.isMultiEventRecordItem
                                                != undefined) {
                                            boolIsMultiEventRecItem =
                                                    dvtEventDescriptor.isMultiEventRecordItem;
                                        }
                                    }
                                    var strIsMultiEventRecItem = "";
                                    strIsMultiEventRecItem = boolIsMultiEventRecItem;
                                    genXmlEntry(9,"isMultiEventRecordItem",
                                            strIsMultiEventRecItem);
                                    var intNumParams = 0;
                                    var strNumParams = "";
                                    if ("numParameters" in dvtEventDescriptor) {
                                        if (dvtEventDescriptor.numParameters !=
                                                undefined) {
                                            strNumParams = dvtEventDescriptor.numParameters.toString();
                                        }
                                    }
                                    intNumParams = strNumParams;
                                    genXmlEntry(9,"numParameters",strNumParams);
                                    for (var pIndex =0; pIndex < intNumParams; pIndex++) {
                                        var paramInfo = dvtEventDescriptor.paramInfo[pIndex];
                                        genXmlEntryOpenWithKey(9,"parameter","index",
                                                pIndex.toString());
                                        genXmlEntry(12,"name",paramInfo.name);
                                        genXmlEntry(12,"dvtDataDesc",paramInfo.dataDesc);

                                        if (paramInfo.dataDesc ===
                                                DvtTypes.DvtDataDesc_CUSTOM) {
                                            var strCustomDataDesc = "null";
                                            if ("strCustomDataDesc" in paramInfo) {
                                                if (paramInfo.strCustomDataDesc != undefined) {
                                                    strCustomDataDesc =
                                                            paramInfo.strCustomDataDesc;
                                                }
                                            }
                                            genXmlEntry(12,"strCustomDataDesc",
                                                    strCustomDataDesc);
                                        }
                                        genXmlEntry(12,"dataTypeName",
                                                paramInfo.dataTypeName);
                                        genXmlEntry(12,"units",paramInfo.units);
                                        var strIsHidden = "false";
                                        if ("isHidden" in paramInfo) {
                                            if (paramInfo.isHidden != undefined) {
                                                strIsHidden = paramInfo.isHidden;
                                            }
                                        }
                                        genXmlEntry(12,"isHidden",strIsHidden);
                                        var strLsb = "0";
                                        if ("lsb" in paramInfo) {
                                            if (paramInfo.lsb != undefined) {
                                                strLsb = ""+paramInfo.lsb;
                                                genXmlEntry(12,"lsb",strLsb);
                                            }
                                        }
                                        genXmlEntryClose(9,"parameter");
                                    }
                                    genXmlEntryClose(6,"dvtEventDescriptor");
                                }
                            }
                        }
                    }
                    xmlFile.write("   </evtMap>\n");
                }
                /*bc: NEW for Linux [...                      */
                if (isGenerateLinuxFiles){
                    closeLinuxHFile();
                }
                /*...]                                        */
            }
        }

        for each (var mod in Program.targetModules()) {
            strModuleId = mod.Module__id.toString();
            strModuleName = mod.$name;

            strModulePackageName = mod.$package.$name;
            strPkgVersion = mod.$package.$vers.toString();
            strModuleSlashName = strModuleName.replace(/\./g,"/");
            strModulePackageBase = xdc.getPackageBase(strModulePackageName);
            strModuleCFilePath = xdc.findFile(strModuleSlashName+".c");
            if (strModuleCFilePath != null) {
                strModuleCFileName =
                        strModuleName.substring(strModuleName.lastIndexOf(".")+1)+".c";
            } else {
                strModuleCFileName = "null";
            }
            genXmlEntryOpenWithKey(3,"modIdMap","id",strModuleId);
            genXmlEntry(6,"moduleName",strModuleName);
            genXmlEntry(6,"packageVersion",strPkgVersion);
            genXmlEntry(6,"packageBase",strModulePackageBase);
            genXmlEntry(6,"cFileName",strModuleCFileName);
            genXmlEntryClose(3,"modIdMap");
        }

        /* Generate info about the transports */
        generateTransportMetaData(3);
        generateServiceMetaData(3);

        /* for all loggers that implement IUIAMetaProvider, output the metadata
         * provided by the module */
        var IUIAMetaProvider = xdc.module('ti.uia.events.IUIAMetaProvider');

        var Timestamp = xdc.module('xdc.runtime.Timestamp_SupportProxy');
        var isTimestampUsed = false;
        var ctsMod = null;
        for each (var mod in Program.targetModules()) {
            if ((mod instanceof IUIAMetaProvider.Module) && (mod.hasMetaData)){
                strModuleId = mod.Module__id.toString();
                strModuleName = mod.$name;
                numInstances = mod.$instances.length;
                dbgprint("UIA: ti.uia.runtime.UIAMetaData.generateXML: found " +
                        "IUIAMetaProvider instance: " + strModuleName+", " +
                        "numInstances=" + numInstances);

                if (numInstances > 0){
                    var instanceNum = 0;
                    for each (var cbInst in mod.$instances) {
                        if (cbInst != null){
                            mod.writeUIAMetaData(cbInst,instanceNum,3);
                            instanceNum++;
                        }
                    }
                } else {
                    mod.writeUIAMetaData(null,0,3);
                }
            }
            var LogSync = xdc.module('ti.uia.runtime.LogSync');
            if (mod instanceof LogSync.Module){
                isLogSyncActive = LogSync.isEnabled;
                dbgprint("UIA: ti.uia.runtime.UIAMetaData.generateXML: found " +
                        "LogSync instance");
            }
            if (mod instanceof Timestamp.Module){
                isTimestampUsed = true;

                ctsMod = mod;
                if (isEnableDebugPrintf) {
                    print("UIA: ti.uia.runtime.UIAMetaData.generateXML: found " +
                            "Timestamp instance: " + ctsMod);
                    if (!(ctsMod.delegate$ == undefined)){
                        print("Timestamp.delegate$.$name = "+ ctsMod.delegate$.$name+
                                ", module ID = "+ctsMod.delegate$.Module__id.toString());
                    }
                }
            }
        } /* for each (var mod in Program.targetModules()) */

        if (isLoggingSetupActive) {
            LoggingSetup = xdc.module(nameOfLoggingSetupModule);
            LoggingSetup.writeUIAMetaData(3);
        }

        /* Generate Clock and Timer frequency entries */

        var deviceName = Program.cpu.deviceName;
        var catalogName = Program.cpu.catalogName;
        catalogName = catalogName.substring(catalogName.lastIndexOf(".") + 1);

        var cpuFreq = UIAMetaData.cpuFreq;
        var timestampFreq = UIAMetaData.timestampFreq;

        var cpuClkLSW = 0;
        var cpuClkMSW = 0;
        var timerLSW = 0;
        var timerMSW = 0;

        var BIOS = undefined;
        try {
            BIOS = xdc.module('ti.sysbios.BIOS');
        }
        catch (e) {
            /* Not a BIOS program */
            dbgprint("UIA: Not a BIOS program.");
        }

        /* Use BIOS cpu and timestamp frequencies if not overridden. */
        if (BIOS != undefined) {
            if (!UIAMetaData.overrideCpuFreq) {
                dbgprint("UIA: Getting CPU frequency from BIOS.");
                if ("getCpuFreqMeta" in BIOS) {
                    dbgprint("UIA: Using BIOS.getCpuFreqMeta()");
                    cpuFreq = BIOS.getCpuFreqMeta();
                }
                else {
                    dbgprint("UIA: Using BIOS.getCpuFrequency()");
                    cpuFreq.lo = BIOS.getCpuFrequency();
                    cpuFreq.hi = 0;
                }
                dbgprint("UIA: CPU Frequency from BIOS.getCpuFrequency: lo = " + cpuFreq.lo + ", hi = " + cpuFreq.hi);
            }
            if (!UIAMetaData.overrideTimestampFreq) {
                dbgprint("UIA: Getting timestamp frequency from BIOS.");
                if ("getTimestampFreqMeta" in BIOS) {
                    dbgprint("UIA: Using BIOS.getTimestampFreqMeta()");
                    timestampFreq = BIOS.getTimestampFreqMeta();
                }
                else {
                    dbgprint("UIA: Using BIOS.getTimestampFrequency()");
                    timestampFreq.lo = BIOS.getTimestampFrequency();
                    timestampFreq.hi = 0;
                }
                dbgprint("UIA: TS Frequency from BIOS.getTimestampFrequency: lo = " +
                        timestampFreq.lo + ", hi = " + timestampFreq.hi);
                /*
                 *  If BIOS.getTimestampFrequency() returns 0, the user must set
                 *  UIAMetaData.timestampFreq.
                 */
                if (timestampFreq.lo == 0) {
                    UIAMetaData.$logWarning("BIOS could not determine the timestamp " +
                            "frequency of device. Setting timestamp frequency to cpu frequency." +
                            "To override this setting, set UIAMetaData.timestampFreq " +
                            "in your configuration (.cfg) file.",
                            this, "timestampFreq");
                    timestampFreq = cpuFreq;
                }
            }
        }

        cpuClkLSW = cpuFreq.lo;
        cpuClkMSW = cpuFreq.hi;
        timerLSW = timestampFreq.lo;
        timerMSW = timestampFreq.hi;

        genXmlComment(3, "Target settings for " + deviceName + " " + catalogName + " device.");
        generateTargetDataEntry();
        genXmlComment(3, "CPU Clock settings for " + deviceName + " " + catalogName + " device.");
        generateCpuClockEntry(0, cpuClkLSW, false);
        generateTimestampEntry(catalogName, null, 0, timerLSW, 1, false);

        xmlFile.write("</uiaEventData>");
        xmlFile.flush();
        xmlFile.close();
        dbgprint("UIA ti.uia.runtime.UIAMetaData.xs  closing file " +
                "uiaEventData.xml. numModules = " + numModules)//, numModules.toString());
    } /* if (outputDirExists) */
}

/* NOTE: this function should only be called once all of the configuration has
   been finalized - e.g. from the UIAMetaData.validate() method
 */
function generateLinuxFiles(){
    var UIAMetaData = xdc.module('ti.uia.runtime.UIAMetaData');

    if (isEnableDebugPrintf)
        print("UIA ti.uia.runtime.UIAMetaData.xs - generateLinuxFiles " +
                "function called");
    generate(true,UIAMetaData.rootDirForLinuxFiles);
}

/* NOTE: this function should only be called once all of the configuration has
   been finalized - e.g. from the UIAMetaData.validate() method
 */
function generateXML(){
    var UIAMetaData = xdc.module('ti.uia.runtime.UIAMetaData');

    if (isEnableDebugPrintf)
        print("UIA ti.uia.runtime.UIAMetaData.xs - generateXML " +
                "function called");

    generate(false,"");
    //   if (UIAMetaData.isLinuxFileGenerationEnabled)
    //       generateLinuxFiles();
}

/*
 *  ======== dbgprint ========
 */
function dbgprint(str)
{
    if (isEnableDebugPrintf) {
        print(str);
    }
}

/*
 *  ======== module$validate()  ========
 */
function module$validate() {
    if (isEnableDebugPrintf) {
        print("UIA: ti.uia.runtime.UIAMetaData.xs module$validate() function called");
    }
    if (isLogSyncActive) {
        if (isEnableDebugPrintf) {
            print("UIA: ti.uia.runtime.UIAMetaData.xs isLogSyncActive = true");
        }
        var LogSync = xdc.module(nameOfLogSyncModule);
        /* if LogSync is used, call it's validate method first!  */
        for each (var mod in Program.targetModules()) {
            if (mod instanceof LogSync.Module){
                LogSync.finalize();
                break;
            }
        }
    }

    var UIAMetaData = xdc.module('ti.uia.runtime.UIAMetaData');
    UIAMetaData.generateXML();
}
