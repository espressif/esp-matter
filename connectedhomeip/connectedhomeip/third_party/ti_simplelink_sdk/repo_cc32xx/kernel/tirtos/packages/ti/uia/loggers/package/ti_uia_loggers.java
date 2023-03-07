/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class ti_uia_loggers
{
    static final String VERS = "@(#) xdc-K04\n";

    static final Proto.Elm $$T_Bool = Proto.Elm.newBool();
    static final Proto.Elm $$T_Num = Proto.Elm.newNum();
    static final Proto.Elm $$T_Str = Proto.Elm.newStr();
    static final Proto.Elm $$T_Obj = Proto.Elm.newObj();

    static final Proto.Fxn $$T_Met = new Proto.Fxn(null, null, 0, -1, false);
    static final Proto.Map $$T_Map = new Proto.Map($$T_Obj);
    static final Proto.Arr $$T_Vec = new Proto.Arr($$T_Obj);

    static final XScriptO $$DEFAULT = Value.DEFAULT;
    static final Object $$UNDEF = Undefined.instance;

    static final Proto.Obj $$Package = (Proto.Obj)Global.get("$$Package");
    static final Proto.Obj $$Module = (Proto.Obj)Global.get("$$Module");
    static final Proto.Obj $$Instance = (Proto.Obj)Global.get("$$Instance");
    static final Proto.Obj $$Params = (Proto.Obj)Global.get("$$Params");

    static final Object $$objFldGet = Global.get("$$objFldGet");
    static final Object $$objFldSet = Global.get("$$objFldSet");
    static final Object $$proxyGet = Global.get("$$proxyGet");
    static final Object $$proxySet = Global.get("$$proxySet");
    static final Object $$delegGet = Global.get("$$delegGet");
    static final Object $$delegSet = Global.get("$$delegSet");

    Scriptable xdcO;
    Session ses;
    Value.Obj om;

    boolean isROV;
    boolean isCFG;

    Proto.Obj pkgP;
    Value.Obj pkgV;

    ArrayList<Object> imports = new ArrayList<Object>();
    ArrayList<Object> loggables = new ArrayList<Object>();
    ArrayList<Object> mcfgs = new ArrayList<Object>();
    ArrayList<Object> icfgs = new ArrayList<Object>();
    ArrayList<String> inherits = new ArrayList<String>();
    ArrayList<Object> proxies = new ArrayList<Object>();
    ArrayList<Object> sizes = new ArrayList<Object>();
    ArrayList<Object> tdefs = new ArrayList<Object>();

    void $$IMPORTS()
    {
        Global.callFxn("loadPackage", xdcO, "xdc.rta");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.uia.runtime");
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.uia.loggers.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.uia.loggers", new Value.Obj("ti.uia.loggers", pkgP));
    }

    void LoggerStopMode$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.loggers.LoggerStopMode.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.loggers.LoggerStopMode", new Value.Obj("ti.uia.loggers.LoggerStopMode", po));
        pkgV.bind("LoggerStopMode", vo);
        // decls 
        om.bind("ti.uia.loggers.LoggerStopMode.TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.loggers"));
        om.bind("ti.uia.loggers.LoggerStopMode.Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.loggers"));
        om.bind("ti.uia.loggers.LoggerStopMode.MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.loggers"));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerStopMode$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerStopMode.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerStopMode$$InstanceView", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerStopMode.InstanceView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerStopMode$$RecordView", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerStopMode.RecordView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerStopMode$$RtaData", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerStopMode.RtaData", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerStopMode$$Module_State", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerStopMode.Module_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerStopMode$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerStopMode.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.uia.loggers.LoggerStopMode.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.loggers.LoggerStopMode$$Object", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerStopMode.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.loggers.LoggerStopMode$$Params", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerStopMode.Params", new Proto.Str(po, false));
        om.bind("ti.uia.loggers.LoggerStopMode.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.loggers.LoggerStopMode.Object", om.findStrict("ti.uia.loggers.LoggerStopMode.Instance_State", "ti.uia.loggers"));
        }//isROV
    }

    void LoggerRunMode$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.loggers.LoggerRunMode.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.loggers.LoggerRunMode", new Value.Obj("ti.uia.loggers.LoggerRunMode", po));
        pkgV.bind("LoggerRunMode", vo);
        // decls 
        om.bind("ti.uia.loggers.LoggerRunMode.TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.loggers"));
        om.bind("ti.uia.loggers.LoggerRunMode.Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.loggers"));
        om.bind("ti.uia.loggers.LoggerRunMode.MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.loggers"));
        om.bind("ti.uia.loggers.LoggerRunMode.TransportType", new Proto.Enm("ti.uia.loggers.LoggerRunMode.TransportType"));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerRunMode$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerRunMode.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerRunMode$$InstanceView", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerRunMode.InstanceView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerRunMode$$RecordView", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerRunMode.RecordView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerRunMode$$RtaData", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerRunMode.RtaData", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerRunMode$$Module_State", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerRunMode.Module_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerRunMode$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerRunMode.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.uia.loggers.LoggerRunMode.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.loggers.LoggerRunMode$$Object", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerRunMode.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.loggers.LoggerRunMode$$Params", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerRunMode.Params", new Proto.Str(po, false));
        om.bind("ti.uia.loggers.LoggerRunMode.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.loggers.LoggerRunMode.Object", om.findStrict("ti.uia.loggers.LoggerRunMode.Instance_State", "ti.uia.loggers"));
        }//isROV
    }

    void LoggerMin$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.loggers.LoggerMin.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.loggers.LoggerMin", new Value.Obj("ti.uia.loggers.LoggerMin", po));
        pkgV.bind("LoggerMin", vo);
        // decls 
        om.bind("ti.uia.loggers.LoggerMin.TimestampSize", new Proto.Enm("ti.uia.loggers.LoggerMin.TimestampSize"));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerMin$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerMin.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerMin$$RecordView", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerMin.RecordView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerMin$$RtaData", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerMin.RtaData", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerMin$$Module_State", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerMin.Module_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.loggers.LoggerMin$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerMin.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.uia.loggers.LoggerMin.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.loggers.LoggerMin$$Object", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerMin.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.loggers.LoggerMin$$Params", new Proto.Obj());
        om.bind("ti.uia.loggers.LoggerMin.Params", new Proto.Str(po, false));
        om.bind("ti.uia.loggers.LoggerMin.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.loggers.LoggerMin.Object", om.findStrict("ti.uia.loggers.LoggerMin.Instance_State", "ti.uia.loggers"));
        }//isROV
    }

    void LoggerStopMode$$CONSTS()
    {
        // module LoggerStopMode
        om.bind("ti.uia.loggers.LoggerStopMode.initBuffer", new Extern("ti_uia_loggers_LoggerStopMode_initBuffer__I", "xdc_Void(*)(ti_uia_loggers_LoggerStopMode_Object*,xdc_Ptr,xdc_UInt16)", true, false));
        om.bind("ti.uia.loggers.LoggerStopMode.filterOutEvent", new Extern("ti_uia_loggers_LoggerStopMode_filterOutEvent__I", "xdc_Bool(*)(xdc_Bits16)", true, false));
    }

    void LoggerRunMode$$CONSTS()
    {
        // module LoggerRunMode
        om.bind("ti.uia.loggers.LoggerRunMode.TransportType_JTAG", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.loggers.LoggerRunMode.TransportType", "ti.uia.loggers"), "ti.uia.loggers.LoggerRunMode.TransportType_JTAG", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.loggers.LoggerRunMode.TransportType_ETHERNET", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.loggers.LoggerRunMode.TransportType", "ti.uia.loggers"), "ti.uia.loggers.LoggerRunMode.TransportType_ETHERNET", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.loggers.LoggerRunMode.isUploadRequired", new Extern("ti_uia_loggers_LoggerRunMode_isUploadRequired__E", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.uia.loggers.LoggerRunMode.idleHook", new Extern("ti_uia_loggers_LoggerRunMode_idleHook__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.loggers.LoggerRunMode.filterOutEvent", new Extern("ti_uia_loggers_LoggerRunMode_filterOutEvent__I", "xdc_Bool(*)(xdc_Bits16)", true, false));
        om.bind("ti.uia.loggers.LoggerRunMode.writeStart", new Extern("ti_uia_loggers_LoggerRunMode_writeStart__I", "xdc_UArg*(*)(ti_uia_loggers_LoggerRunMode_Object*,xdc_runtime_Log_Event,xdc_Bits16,xdc_UInt16)", true, false));
    }

    void LoggerMin$$CONSTS()
    {
        // module LoggerMin
        om.bind("ti.uia.loggers.LoggerMin.TimestampSize_NONE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.loggers.LoggerMin.TimestampSize", "ti.uia.loggers"), "ti.uia.loggers.LoggerMin.TimestampSize_NONE", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.loggers.LoggerMin.TimestampSize_32b", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.loggers.LoggerMin.TimestampSize", "ti.uia.loggers"), "ti.uia.loggers.LoggerMin.TimestampSize_32b", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.loggers.LoggerMin.TimestampSize_64b", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.loggers.LoggerMin.TimestampSize", "ti.uia.loggers"), "ti.uia.loggers.LoggerMin.TimestampSize_64b", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.loggers.LoggerMin.flush", new Extern("ti_uia_loggers_LoggerMin_flush__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.loggers.LoggerMin.write", new Extern("ti_uia_loggers_LoggerMin_write__E", "xdc_Void(*)(xdc_runtime_Log_Event,xdc_Bits16,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg)", true, false));
        om.bind("ti.uia.loggers.LoggerMin.getContents", new Extern("ti_uia_loggers_LoggerMin_getContents__I", "xdc_Bool(*)(ti_uia_loggers_LoggerMin_Object*,xdc_Ptr,xdc_SizeT,xdc_SizeT*)", true, false));
        om.bind("ti.uia.loggers.LoggerMin.isEmpty", new Extern("ti_uia_loggers_LoggerMin_isEmpty__I", "xdc_Bool(*)(ti_uia_loggers_LoggerMin_Object*)", true, false));
        om.bind("ti.uia.loggers.LoggerMin.genTimestamp", new Extern("ti_uia_loggers_LoggerMin_genTimestamp__I", "xdc_Ptr(*)(xdc_Ptr)", true, false));
    }

    void LoggerStopMode$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$uia$loggers$LoggerStopMode$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.uia.loggers.LoggerStopMode.create() called before xdc.useModule('ti.uia.loggers.LoggerStopMode')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerStopMode$$create", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerStopMode.Module", "ti.uia.loggers"), om.findStrict("ti.uia.loggers.LoggerStopMode.Instance", "ti.uia.loggers"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.uia.loggers.LoggerStopMode.Params", "ti.uia.loggers"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$loggers$LoggerStopMode$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.loggers.LoggerStopMode'];\n");
                sb.append("var __inst = xdc.om['ti.uia.loggers.LoggerStopMode.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.uia.loggers']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.uia.loggers.LoggerStopMode'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.loggers.LoggerStopMode'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerStopMode$$construct", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerStopMode.Module", "ti.uia.loggers"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.uia.loggers.LoggerStopMode$$Object", "ti.uia.loggers"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.uia.loggers.LoggerStopMode.Params", "ti.uia.loggers"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$loggers$LoggerStopMode$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.loggers.LoggerStopMode'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.uia.loggers.LoggerStopMode'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.loggers.LoggerStopMode'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void LoggerRunMode$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$uia$loggers$LoggerRunMode$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.uia.loggers.LoggerRunMode.create() called before xdc.useModule('ti.uia.loggers.LoggerRunMode')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerRunMode$$create", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerRunMode.Module", "ti.uia.loggers"), om.findStrict("ti.uia.loggers.LoggerRunMode.Instance", "ti.uia.loggers"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.uia.loggers.LoggerRunMode.Params", "ti.uia.loggers"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$loggers$LoggerRunMode$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.loggers.LoggerRunMode'];\n");
                sb.append("var __inst = xdc.om['ti.uia.loggers.LoggerRunMode.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.uia.loggers']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.uia.loggers.LoggerRunMode'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.loggers.LoggerRunMode'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerRunMode$$construct", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerRunMode.Module", "ti.uia.loggers"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.uia.loggers.LoggerRunMode$$Object", "ti.uia.loggers"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.uia.loggers.LoggerRunMode.Params", "ti.uia.loggers"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$loggers$LoggerRunMode$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.loggers.LoggerRunMode'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.uia.loggers.LoggerRunMode'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.loggers.LoggerRunMode'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void LoggerMin$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$uia$loggers$LoggerMin$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.uia.loggers.LoggerMin.create() called before xdc.useModule('ti.uia.loggers.LoggerMin')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerMin$$create", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerMin.Module", "ti.uia.loggers"), om.findStrict("ti.uia.loggers.LoggerMin.Instance", "ti.uia.loggers"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.uia.loggers.LoggerMin.Params", "ti.uia.loggers"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$loggers$LoggerMin$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.loggers.LoggerMin'];\n");
                sb.append("var __inst = xdc.om['ti.uia.loggers.LoggerMin.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.uia.loggers']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.uia.loggers.LoggerMin'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.loggers.LoggerMin'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerMin$$construct", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerMin.Module", "ti.uia.loggers"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.uia.loggers.LoggerMin$$Object", "ti.uia.loggers"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.uia.loggers.LoggerMin.Params", "ti.uia.loggers"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$loggers$LoggerMin$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.loggers.LoggerMin'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.uia.loggers.LoggerMin'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.loggers.LoggerMin'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void LoggerStopMode$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn LoggerStopMode.getNumInstances
        fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerStopMode$$getNumInstances", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerStopMode.Module", "ti.uia.loggers"), Proto.Elm.newCNum("(xdc_Int)"), 0, 0, false));
        // fxn LoggerStopMode.initDecoder
        fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerStopMode$$initDecoder", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerStopMode.Module", "ti.uia.loggers"), null, 0, -1, false));
    }

    void LoggerRunMode$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn LoggerRunMode.getNumInstances
        fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerRunMode$$getNumInstances", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerRunMode.Module", "ti.uia.loggers"), Proto.Elm.newCNum("(xdc_Int)"), 0, 0, false));
        // fxn LoggerRunMode.initDecoder
        fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerRunMode$$initDecoder", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerRunMode.Module", "ti.uia.loggers"), null, 0, -1, false));
    }

    void LoggerMin$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn LoggerMin.getLoggerInstanceId
        fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerMin$$getLoggerInstanceId", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerMin.Module", "ti.uia.loggers"), null, 0, -1, false));
        // fxn LoggerMin.initDecoder
        fxn = (Proto.Fxn)om.bind("ti.uia.loggers.LoggerMin$$initDecoder", new Proto.Fxn(om.findStrict("ti.uia.loggers.LoggerMin.Module", "ti.uia.loggers"), null, 0, -1, false));
    }

    void LoggerStopMode$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.loggers.LoggerStopMode.Module_State", "ti.uia.loggers");
        sizes.clear();
        sizes.add(Global.newArray("enabled", "UShort"));
        sizes.add(Global.newArray("level1", "UInt16"));
        sizes.add(Global.newArray("level2", "UInt16"));
        sizes.add(Global.newArray("level3", "UInt16"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.loggers.LoggerStopMode.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.loggers.LoggerStopMode.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.loggers.LoggerStopMode.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.loggers.LoggerStopMode.Instance_State", "ti.uia.loggers");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("enabled", "UShort"));
        sizes.add(Global.newArray("instanceId", "TInt16"));
        sizes.add(Global.newArray("bufSize", "UInt32"));
        sizes.add(Global.newArray("buffer", "UPtr"));
        sizes.add(Global.newArray("write", "UPtr"));
        sizes.add(Global.newArray("end", "UPtr"));
        sizes.add(Global.newArray("maxEventSizeUArgs", "USize"));
        sizes.add(Global.newArray("maxEventSize", "USize"));
        sizes.add(Global.newArray("numBytesInPrevEvent", "UInt16"));
        sizes.add(Global.newArray("droppedEvents", "UInt32"));
        sizes.add(Global.newArray("eventSequenceNum", "UInt16"));
        sizes.add(Global.newArray("pktSequenceNum", "UInt16"));
        sizes.add(Global.newArray("hdr", "UPtr"));
        sizes.add(Global.newArray("packetArray", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.loggers.LoggerStopMode.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.loggers.LoggerStopMode.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.loggers.LoggerStopMode.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LoggerRunMode$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.loggers.LoggerRunMode.Module_State", "ti.uia.loggers");
        sizes.clear();
        sizes.add(Global.newArray("enabled", "UShort"));
        sizes.add(Global.newArray("level1", "UInt16"));
        sizes.add(Global.newArray("level2", "UInt16"));
        sizes.add(Global.newArray("level3", "UInt16"));
        sizes.add(Global.newArray("lastUploadTstamp", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.loggers.LoggerRunMode.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.loggers.LoggerRunMode.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.loggers.LoggerRunMode.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.loggers.LoggerRunMode.Instance_State", "ti.uia.loggers");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("enabled", "UShort"));
        sizes.add(Global.newArray("instanceId", "TInt16"));
        sizes.add(Global.newArray("primeStatus", "UShort"));
        sizes.add(Global.newArray("bufSize", "UInt32"));
        sizes.add(Global.newArray("buffer", "UPtr"));
        sizes.add(Global.newArray("write", "UPtr"));
        sizes.add(Global.newArray("end", "UPtr"));
        sizes.add(Global.newArray("maxEventSizeUArgs", "USize"));
        sizes.add(Global.newArray("maxEventSize", "USize"));
        sizes.add(Global.newArray("numBytesInPrevEvent", "UInt16"));
        sizes.add(Global.newArray("droppedEvents", "UInt32"));
        sizes.add(Global.newArray("eventSequenceNum", "UInt16"));
        sizes.add(Global.newArray("pktSequenceNum", "UInt16"));
        sizes.add(Global.newArray("hdr", "UPtr"));
        sizes.add(Global.newArray("packetArray", "UPtr"));
        sizes.add(Global.newArray("numPackets", "TInt"));
        sizes.add(Global.newArray("packetSize", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.loggers.LoggerRunMode.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.loggers.LoggerRunMode.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.loggers.LoggerRunMode.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LoggerMin$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.loggers.LoggerMin.Module_State", "ti.uia.loggers");
        sizes.clear();
        sizes.add(Global.newArray("enabled", "UShort"));
        sizes.add(Global.newArray("empty", "UShort"));
        sizes.add(Global.newArray("numBytesInPrevEvent", "UInt16"));
        sizes.add(Global.newArray("droppedEvents", "UInt16"));
        sizes.add(Global.newArray("packetBuffer", "UPtr"));
        sizes.add(Global.newArray("start", "UPtr"));
        sizes.add(Global.newArray("write", "UPtr"));
        sizes.add(Global.newArray("end", "UPtr"));
        sizes.add(Global.newArray("eventSequenceNum", "UInt16"));
        sizes.add(Global.newArray("pktSequenceNum", "UInt16"));
        sizes.add(Global.newArray("eventType", "Nti.uia.runtime.EventHdr.HdrType;;0;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;25;26;27;28;29;30;31"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.loggers.LoggerMin.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.loggers.LoggerMin.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.loggers.LoggerMin.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.loggers.LoggerMin.Instance_State", "ti.uia.loggers");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.loggers.LoggerMin.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.loggers.LoggerMin.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.loggers.LoggerMin.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LoggerStopMode$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/loggers/LoggerStopMode.xs");
        om.bind("ti.uia.loggers.LoggerStopMode$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode.Module", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerStopMode.Module", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Module", "ti.uia.loggers"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.loggers"), $$UNDEF, "wh");
            po.addFld("isTimestampEnabled", $$T_Bool, true, "w");
            po.addFld("supportLoggerDisable", $$T_Bool, false, "w");
            po.addFld("statusLogger", (Proto)om.findStrict("xdc.runtime.IFilterLogger.Handle", "ti.uia.loggers"), null, "wh");
            po.addFld("overflowLogger", (Proto)om.findStrict("xdc.runtime.ILogger.Handle", "ti.uia.loggers"), null, "wh");
            po.addFld("level1Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level2Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level3Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level4Mask", Proto.Elm.newCNum("(xdc_Bits16)"), Global.eval("0xFF9F & (~0x0010) & (~0x0008)"), "w");
            po.addFld("moduleToRouteToStatusLogger", $$T_Str, null, "wh");
            po.addFld("L_test", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.loggers"), Global.newObject("mask", 0x0100L, "msg", "LoggerStopMode Test"), "w");
            po.addFld("E_badLevel", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.uia.loggers"), Global.newObject("msg", "E_badLevel: Bad filter level value: %d"), "w");
            po.addFld("cacheLineSizeInMAUs", Proto.Elm.newCNum("(xdc_Int)"), 128L, "wh");
            po.addFld("numCores", Proto.Elm.newCNum("(xdc_Int)"), 1L, "w");
            po.addFld("maxId", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerStopMode$$create", "ti.uia.loggers"), Global.get("ti$uia$loggers$LoggerStopMode$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerStopMode$$construct", "ti.uia.loggers"), Global.get("ti$uia$loggers$LoggerStopMode$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerStopMode$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerStopMode$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerStopMode$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerStopMode$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerStopMode$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerStopMode$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getMetaArgs");
                if (fxn != null) po.addFxn("getMetaArgs", (Proto.Fxn)om.findStrict("xdc.runtime.ILogger$$getMetaArgs", "ti.uia.loggers"), fxn);
                fxn = Global.get(cap, "getPtrToQueueDescriptorMeta");
                if (fxn != null) po.addFxn("getPtrToQueueDescriptorMeta", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getPtrToQueueDescriptorMeta", "ti.uia.loggers"), fxn);
                fxn = Global.get(cap, "setPtrToQueueDescriptorMeta");
                if (fxn != null) po.addFxn("setPtrToQueueDescriptorMeta", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$setPtrToQueueDescriptorMeta", "ti.uia.loggers"), fxn);
                fxn = Global.get(cap, "getLoggerInstanceId");
                if (fxn != null) po.addFxn("getLoggerInstanceId", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getLoggerInstanceId", "ti.uia.loggers"), fxn);
                fxn = Global.get(cap, "getLoggerPriority");
                if (fxn != null) po.addFxn("getLoggerPriority", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getLoggerPriority", "ti.uia.loggers"), fxn);
                fxn = Global.get(cap, "setLoggerPriority");
                if (fxn != null) po.addFxn("setLoggerPriority", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$setLoggerPriority", "ti.uia.loggers"), fxn);
                po.addFxn("getNumInstances", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerStopMode$$getNumInstances", "ti.uia.loggers"), Global.get(cap, "getNumInstances"));
                po.addFxn("initDecoder", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerStopMode$$initDecoder", "ti.uia.loggers"), Global.get(cap, "initDecoder"));
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode.Instance", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerStopMode.Instance", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Instance", "ti.uia.loggers"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int16)"), 1L, "w");
            po.addFld("transferBufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), 512L, "w");
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1400L, "w");
            po.addFld("bufSection", $$T_Str, null, "wh");
            po.addFld("bufHeap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.uia.loggers"), null, "wh");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.loggers"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$Params", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerStopMode.Params", om.findStrict("ti.uia.runtime.ILoggerSnapshot$$Params", "ti.uia.loggers"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int16)"), 1L, "w");
            po.addFld("transferBufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), 512L, "w");
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1400L, "w");
            po.addFld("bufSection", $$T_Str, null, "wh");
            po.addFld("bufHeap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.uia.loggers"), null, "wh");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.loggers"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$Object", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerStopMode.Object", om.findStrict("ti.uia.loggers.LoggerStopMode.Instance", "ti.uia.loggers"));
        // struct LoggerStopMode.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$ModuleView", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerStopMode.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("isEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("isTimestampEnabled", $$T_Bool, $$UNDEF, "w");
        // struct LoggerStopMode.InstanceView
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$InstanceView", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerStopMode.InstanceView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("bufferSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
        // struct LoggerStopMode.RecordView
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$RecordView", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerStopMode.RecordView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("sequence", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("timestampRaw", Proto.Elm.newCNum("(xdc_Long)"), $$UNDEF, "w");
                po.addFld("modName", $$T_Str, $$UNDEF, "w");
                po.addFld("text", $$T_Str, $$UNDEF, "w");
                po.addFld("eventId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("eventName", $$T_Str, $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg2", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg3", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg4", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg5", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg6", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg7", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
        // struct LoggerStopMode.RtaData
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$RtaData", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerStopMode.RtaData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // struct LoggerStopMode.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$Module_State", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerStopMode.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("level1", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("level2", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("level3", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
        // struct LoggerStopMode.Instance_State
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$Instance_State", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerStopMode.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int16)"), $$UNDEF, "w");
                po.addFld("bufSize", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("buffer", new Proto.Adr("xdc_UArg*", "PPv"), $$UNDEF, "w");
                po.addFld("write", new Proto.Adr("xdc_UArg*", "PPv"), $$UNDEF, "w");
                po.addFld("end", new Proto.Adr("xdc_UArg*", "PPv"), $$UNDEF, "w");
                po.addFld("maxEventSizeUArgs", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("numBytesInPrevEvent", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFld("droppedEvents", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("eventSequenceNum", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFld("pktSequenceNum", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFld("hdr", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("packetArray", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
    }

    void LoggerRunMode$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/loggers/LoggerRunMode.xs");
        om.bind("ti.uia.loggers.LoggerRunMode$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode.Module", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerRunMode.Module", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Module", "ti.uia.loggers"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.loggers"), $$UNDEF, "wh");
            po.addFld("transportType", (Proto)om.findStrict("ti.uia.loggers.LoggerRunMode.TransportType", "ti.uia.loggers"), om.find("ti.uia.loggers.LoggerRunMode.TransportType_JTAG"), "wh");
            po.addFld("customTransportType", $$T_Str, null, "w");
            po.addFld("isTimestampEnabled", $$T_Bool, true, "w");
            po.addFld("supportLoggerDisable", $$T_Bool, false, "w");
            po.addFld("statusLogger", (Proto)om.findStrict("xdc.runtime.IFilterLogger.Handle", "ti.uia.loggers"), null, "wh");
            po.addFld("overflowLogger", (Proto)om.findStrict("xdc.runtime.ILogger.Handle", "ti.uia.loggers"), null, "wh");
            po.addFld("level1Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level2Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level3Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level4Mask", Proto.Elm.newCNum("(xdc_Bits16)"), Global.eval("0xFF9F & (~0x0010) & (~0x0008)"), "w");
            po.addFld("moduleToRouteToStatusLogger", $$T_Str, null, "wh");
            po.addFld("L_test", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.loggers"), Global.newObject("mask", 0x0100L, "msg", "LoggerRunMode Test"), "w");
            po.addFld("E_badLevel", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.uia.loggers"), Global.newObject("msg", "E_badLevel: Bad filter level value: %d"), "w");
            po.addFld("cacheLineSizeInMAUs", Proto.Elm.newCNum("(xdc_Int)"), 128L, "wh");
            po.addFld("numCores", Proto.Elm.newCNum("(xdc_Int)"), 1L, "w");
            po.addFld("cpuId", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
            po.addFld("enableAutoConfigOfIdleHook", $$T_Bool, true, "wh");
            po.addFld("maxId", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerRunMode$$create", "ti.uia.loggers"), Global.get("ti$uia$loggers$LoggerRunMode$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerRunMode$$construct", "ti.uia.loggers"), Global.get("ti$uia$loggers$LoggerRunMode$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerRunMode$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerRunMode$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerRunMode$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerRunMode$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerRunMode$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerRunMode$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getMetaArgs");
                if (fxn != null) po.addFxn("getMetaArgs", (Proto.Fxn)om.findStrict("xdc.runtime.ILogger$$getMetaArgs", "ti.uia.loggers"), fxn);
                fxn = Global.get(cap, "getPtrToQueueDescriptorMeta");
                if (fxn != null) po.addFxn("getPtrToQueueDescriptorMeta", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getPtrToQueueDescriptorMeta", "ti.uia.loggers"), fxn);
                fxn = Global.get(cap, "setPtrToQueueDescriptorMeta");
                if (fxn != null) po.addFxn("setPtrToQueueDescriptorMeta", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$setPtrToQueueDescriptorMeta", "ti.uia.loggers"), fxn);
                fxn = Global.get(cap, "getLoggerInstanceId");
                if (fxn != null) po.addFxn("getLoggerInstanceId", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getLoggerInstanceId", "ti.uia.loggers"), fxn);
                fxn = Global.get(cap, "getLoggerPriority");
                if (fxn != null) po.addFxn("getLoggerPriority", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getLoggerPriority", "ti.uia.loggers"), fxn);
                fxn = Global.get(cap, "setLoggerPriority");
                if (fxn != null) po.addFxn("setLoggerPriority", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$setLoggerPriority", "ti.uia.loggers"), fxn);
                po.addFxn("getNumInstances", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerRunMode$$getNumInstances", "ti.uia.loggers"), Global.get(cap, "getNumInstances"));
                po.addFxn("initDecoder", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerRunMode$$initDecoder", "ti.uia.loggers"), Global.get(cap, "initDecoder"));
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode.Instance", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerRunMode.Instance", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Instance", "ti.uia.loggers"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int16)"), 1L, "w");
            po.addFld("transferBufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), 512L, "w");
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1024L, "w");
            po.addFld("bufSection", $$T_Str, null, "wh");
            po.addFld("bufHeap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.uia.loggers"), null, "wh");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.loggers"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$Params", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerRunMode.Params", om.findStrict("ti.uia.runtime.ILoggerSnapshot$$Params", "ti.uia.loggers"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int16)"), 1L, "w");
            po.addFld("transferBufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), 512L, "w");
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1024L, "w");
            po.addFld("bufSection", $$T_Str, null, "wh");
            po.addFld("bufHeap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.uia.loggers"), null, "wh");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.loggers"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$Object", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerRunMode.Object", om.findStrict("ti.uia.loggers.LoggerRunMode.Instance", "ti.uia.loggers"));
        // struct LoggerRunMode.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$ModuleView", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerRunMode.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("isEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("isTimestampEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("transportType", $$T_Str, $$UNDEF, "w");
        // struct LoggerRunMode.InstanceView
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$InstanceView", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerRunMode.InstanceView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("bufferSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
        // struct LoggerRunMode.RecordView
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$RecordView", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerRunMode.RecordView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("sequence", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("timestampRaw", Proto.Elm.newCNum("(xdc_Long)"), $$UNDEF, "w");
                po.addFld("modName", $$T_Str, $$UNDEF, "w");
                po.addFld("text", $$T_Str, $$UNDEF, "w");
                po.addFld("eventId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("eventName", $$T_Str, $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg2", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg3", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg4", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg5", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg6", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg7", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
        // struct LoggerRunMode.RtaData
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$RtaData", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerRunMode.RtaData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // struct LoggerRunMode.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$Module_State", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerRunMode.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("level1", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("level2", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("level3", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("lastUploadTstamp", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
        // struct LoggerRunMode.Instance_State
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$Instance_State", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerRunMode.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int16)"), $$UNDEF, "w");
                po.addFld("primeStatus", $$T_Bool, $$UNDEF, "w");
                po.addFld("bufSize", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("buffer", new Proto.Adr("xdc_UArg*", "PPv"), $$UNDEF, "w");
                po.addFld("write", new Proto.Adr("xdc_UArg*", "PPv"), $$UNDEF, "w");
                po.addFld("end", new Proto.Adr("xdc_UArg*", "PPv"), $$UNDEF, "w");
                po.addFld("maxEventSizeUArgs", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("numBytesInPrevEvent", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFld("droppedEvents", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("eventSequenceNum", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFld("pktSequenceNum", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFld("hdr", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("packetArray", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("numPackets", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("packetSize", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
    }

    void LoggerMin$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/loggers/LoggerMin.xs");
        om.bind("ti.uia.loggers.LoggerMin$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin.Module", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerMin.Module", om.findStrict("xdc.runtime.ILogger.Module", "ti.uia.loggers"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.loggers"), $$UNDEF, "wh");
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 512L, "w");
            po.addFld("bufSection", $$T_Str, null, "wh");
            po.addFld("numCores", Proto.Elm.newCNum("(xdc_Int)"), 1L, "w");
            po.addFld("memoryAlignmentInMAUs", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
            po.addFld("timestampSize", (Proto)om.findStrict("ti.uia.loggers.LoggerMin.TimestampSize", "ti.uia.loggers"), om.find("ti.uia.loggers.LoggerMin.TimestampSize_32b"), "w");
            po.addFld("L_test", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.loggers"), Global.newObject("mask", 0x0100L, "msg", "LoggerMin Test"), "w");
            po.addFld("supportLoggerDisable", $$T_Bool, false, "w");
            po.addFld("endpointId", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("loggerInstanceId", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerMin$$create", "ti.uia.loggers"), Global.get("ti$uia$loggers$LoggerMin$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerMin$$construct", "ti.uia.loggers"), Global.get("ti$uia$loggers$LoggerMin$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerMin$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerMin$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerMin$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerMin$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerMin$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.uia.loggers.LoggerMin$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getMetaArgs");
                if (fxn != null) po.addFxn("getMetaArgs", (Proto.Fxn)om.findStrict("xdc.runtime.ILogger$$getMetaArgs", "ti.uia.loggers"), fxn);
                po.addFxn("getLoggerInstanceId", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerMin$$getLoggerInstanceId", "ti.uia.loggers"), Global.get(cap, "getLoggerInstanceId"));
                po.addFxn("initDecoder", (Proto.Fxn)om.findStrict("ti.uia.loggers.LoggerMin$$initDecoder", "ti.uia.loggers"), Global.get(cap, "initDecoder"));
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin.Instance", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerMin.Instance", om.findStrict("xdc.runtime.ILogger.Instance", "ti.uia.loggers"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.loggers"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin$$Params", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerMin.Params", om.findStrict("xdc.runtime.ILogger$$Params", "ti.uia.loggers"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.loggers"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin$$Object", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerMin.Object", om.findStrict("ti.uia.loggers.LoggerMin.Instance", "ti.uia.loggers"));
        // struct LoggerMin.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin$$ModuleView", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerMin.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("isEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("timestampSize", $$T_Str, $$UNDEF, "w");
                po.addFld("bufferSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // struct LoggerMin.RecordView
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin$$RecordView", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerMin.RecordView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("sequence", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("timestampRaw", Proto.Elm.newCNum("(xdc_Long)"), $$UNDEF, "w");
                po.addFld("modName", $$T_Str, $$UNDEF, "w");
                po.addFld("text", $$T_Str, $$UNDEF, "w");
                po.addFld("eventId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("eventName", $$T_Str, $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg2", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg3", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg4", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg5", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg6", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg7", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
        // struct LoggerMin.RtaData
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin$$RtaData", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerMin.RtaData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // struct LoggerMin.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin$$Module_State", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerMin.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("empty", $$T_Bool, $$UNDEF, "w");
                po.addFld("numBytesInPrevEvent", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFld("droppedEvents", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFld("packetBuffer", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("start", new Proto.Adr("xdc_UInt32*", "Pn"), $$UNDEF, "w");
                po.addFld("write", new Proto.Adr("xdc_UInt32*", "Pn"), $$UNDEF, "w");
                po.addFld("end", new Proto.Adr("xdc_UInt32*", "Pn"), $$UNDEF, "w");
                po.addFld("eventSequenceNum", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFld("pktSequenceNum", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFld("eventType", (Proto)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.loggers"), $$UNDEF, "w");
        // struct LoggerMin.Instance_State
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin$$Instance_State", "ti.uia.loggers");
        po.init("ti.uia.loggers.LoggerMin.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void LoggerStopMode$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode", "ti.uia.loggers");
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$Instance_State", "ti.uia.loggers");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.loggers.LoggerStopMode.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$Module_State", "ti.uia.loggers");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.uia.loggers.LoggerStopMode.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode$$Instance_State", "ti.uia.loggers");
        po.bind("hdr$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
        po.bind("packetArray$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
    }

    void LoggerRunMode$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode", "ti.uia.loggers");
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$Instance_State", "ti.uia.loggers");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.loggers.LoggerRunMode.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$Module_State", "ti.uia.loggers");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.uia.loggers.LoggerRunMode.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode$$Instance_State", "ti.uia.loggers");
        po.bind("hdr$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
        po.bind("packetArray$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
    }

    void LoggerMin$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.loggers.LoggerMin", "ti.uia.loggers");
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin$$Instance_State", "ti.uia.loggers");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.loggers.LoggerMin.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin$$Module_State", "ti.uia.loggers");
        po.bind("packetBuffer$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.uia.loggers.LoggerMin.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin$$Instance_State", "ti.uia.loggers");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.uia.loggers.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.uia.loggers"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.uia.loggers", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.uia.loggers");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.uia.loggers.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 2));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.rta", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.uia.loggers'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.uia.loggers$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.uia.loggers$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.uia.loggers$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/release/ti.uia.loggers.aem4',\n");
            sb.append("'lib/release/ti.uia.loggers.am4',\n");
            sb.append("'lib/release/ti.uia.loggers.am4g',\n");
            sb.append("'lib/release/ti.uia.loggers.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/release/ti.uia.loggers.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/release/ti.uia.loggers.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/release/ti.uia.loggers.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/release/ti.uia.loggers.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void LoggerStopMode$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode", "ti.uia.loggers");
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode.Module", "ti.uia.loggers");
        vo.init2(po, "ti.uia.loggers.LoggerStopMode", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.loggers.LoggerStopMode$$capsule", "ti.uia.loggers"));
        vo.bind("Instance", om.findStrict("ti.uia.loggers.LoggerStopMode.Instance", "ti.uia.loggers"));
        vo.bind("Params", om.findStrict("ti.uia.loggers.LoggerStopMode.Params", "ti.uia.loggers"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.loggers.LoggerStopMode.Params", "ti.uia.loggers")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.loggers.LoggerStopMode.Handle", "ti.uia.loggers"));
        vo.bind("$package", om.findStrict("ti.uia.loggers", "ti.uia.loggers"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("filterByLevel");
        vo.bind("TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.loggers"));
        vo.bind("Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.loggers"));
        vo.bind("MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.loggers"));
        vo.bind("ModuleView", om.findStrict("ti.uia.loggers.LoggerStopMode.ModuleView", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerStopMode.ModuleView", "ti.uia.loggers"));
        vo.bind("InstanceView", om.findStrict("ti.uia.loggers.LoggerStopMode.InstanceView", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerStopMode.InstanceView", "ti.uia.loggers"));
        vo.bind("RecordView", om.findStrict("ti.uia.loggers.LoggerStopMode.RecordView", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerStopMode.RecordView", "ti.uia.loggers"));
        vo.bind("RtaData", om.findStrict("ti.uia.loggers.LoggerStopMode.RtaData", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerStopMode.RtaData", "ti.uia.loggers"));
        mcfgs.add("isTimestampEnabled");
        mcfgs.add("supportLoggerDisable");
        mcfgs.add("level1Mask");
        mcfgs.add("level2Mask");
        mcfgs.add("level3Mask");
        mcfgs.add("level4Mask");
        mcfgs.add("L_test");
        mcfgs.add("E_badLevel");
        mcfgs.add("numCores");
        icfgs.add("maxId");
        vo.bind("Module_State", om.findStrict("ti.uia.loggers.LoggerStopMode.Module_State", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerStopMode.Module_State", "ti.uia.loggers"));
        vo.bind("Instance_State", om.findStrict("ti.uia.loggers.LoggerStopMode.Instance_State", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerStopMode.Instance_State", "ti.uia.loggers"));
        vo.bind("TransferType_RELIABLE", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_RELIABLE", "ti.uia.loggers"));
        vo.bind("TransferType_LOSSY", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_LOSSY", "ti.uia.loggers"));
        vo.bind("Priority_LOW", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_LOW", "ti.uia.loggers"));
        vo.bind("Priority_STANDARD", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_STANDARD", "ti.uia.loggers"));
        vo.bind("Priority_HIGH", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_HIGH", "ti.uia.loggers"));
        vo.bind("Priority_SYNC", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_SYNC", "ti.uia.loggers"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.runtime");
        inherits.add("ti.uia.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.loggers")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.uia.loggers.LoggerStopMode$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$uia$loggers$LoggerStopMode$$__initObject"));
        }//isCFG
        vo.bind("initBuffer", om.findStrict("ti.uia.loggers.LoggerStopMode.initBuffer", "ti.uia.loggers"));
        vo.bind("filterOutEvent", om.findStrict("ti.uia.loggers.LoggerStopMode.filterOutEvent", "ti.uia.loggers"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_loggers_LoggerStopMode_Handle__label__E", "ti_uia_loggers_LoggerStopMode_Module__startupDone__E", "ti_uia_loggers_LoggerStopMode_Object__create__E", "ti_uia_loggers_LoggerStopMode_Object__delete__E", "ti_uia_loggers_LoggerStopMode_Object__get__E", "ti_uia_loggers_LoggerStopMode_Object__first__E", "ti_uia_loggers_LoggerStopMode_Object__next__E", "ti_uia_loggers_LoggerStopMode_Params__init__E", "ti_uia_loggers_LoggerStopMode_enable__E", "ti_uia_loggers_LoggerStopMode_disable__E", "ti_uia_loggers_LoggerStopMode_getTransferType__E", "ti_uia_loggers_LoggerStopMode_getContents__E", "ti_uia_loggers_LoggerStopMode_isEmpty__E", "ti_uia_loggers_LoggerStopMode_getMaxLength__E", "ti_uia_loggers_LoggerStopMode_getInstanceId__E", "ti_uia_loggers_LoggerStopMode_getPriority__E", "ti_uia_loggers_LoggerStopMode_setPriority__E", "ti_uia_loggers_LoggerStopMode_reset__E", "ti_uia_loggers_LoggerStopMode_writeMemoryRange__E", "ti_uia_loggers_LoggerStopMode_flush__E", "ti_uia_loggers_LoggerStopMode_initQueueDescriptor__E", "ti_uia_loggers_LoggerStopMode_write0__E", "ti_uia_loggers_LoggerStopMode_write1__E", "ti_uia_loggers_LoggerStopMode_write2__E", "ti_uia_loggers_LoggerStopMode_write4__E", "ti_uia_loggers_LoggerStopMode_write8__E", "ti_uia_loggers_LoggerStopMode_setFilterLevel__E", "ti_uia_loggers_LoggerStopMode_getFilterLevel__E", "ti_uia_loggers_LoggerStopMode_getBufSize__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("L_test"));
        vo.bind("$$errorDescCfgs", Global.newArray("E_badLevel"));
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", "./LoggerStopMode.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.uia.loggers.LoggerStopMode.Object", "ti.uia.loggers"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./LoggerStopMode.xdt");
        pkgV.bind("LoggerStopMode", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerStopMode");
    }

    void LoggerRunMode$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode", "ti.uia.loggers");
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode.Module", "ti.uia.loggers");
        vo.init2(po, "ti.uia.loggers.LoggerRunMode", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.loggers.LoggerRunMode$$capsule", "ti.uia.loggers"));
        vo.bind("Instance", om.findStrict("ti.uia.loggers.LoggerRunMode.Instance", "ti.uia.loggers"));
        vo.bind("Params", om.findStrict("ti.uia.loggers.LoggerRunMode.Params", "ti.uia.loggers"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.loggers.LoggerRunMode.Params", "ti.uia.loggers")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.loggers.LoggerRunMode.Handle", "ti.uia.loggers"));
        vo.bind("$package", om.findStrict("ti.uia.loggers", "ti.uia.loggers"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("filterByLevel");
        vo.bind("TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.loggers"));
        vo.bind("Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.loggers"));
        vo.bind("MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.loggers"));
        vo.bind("TransportType", om.findStrict("ti.uia.loggers.LoggerRunMode.TransportType", "ti.uia.loggers"));
        vo.bind("ModuleView", om.findStrict("ti.uia.loggers.LoggerRunMode.ModuleView", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerRunMode.ModuleView", "ti.uia.loggers"));
        vo.bind("InstanceView", om.findStrict("ti.uia.loggers.LoggerRunMode.InstanceView", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerRunMode.InstanceView", "ti.uia.loggers"));
        vo.bind("RecordView", om.findStrict("ti.uia.loggers.LoggerRunMode.RecordView", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerRunMode.RecordView", "ti.uia.loggers"));
        vo.bind("RtaData", om.findStrict("ti.uia.loggers.LoggerRunMode.RtaData", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerRunMode.RtaData", "ti.uia.loggers"));
        mcfgs.add("customTransportType");
        mcfgs.add("isTimestampEnabled");
        mcfgs.add("supportLoggerDisable");
        mcfgs.add("level1Mask");
        mcfgs.add("level2Mask");
        mcfgs.add("level3Mask");
        mcfgs.add("level4Mask");
        mcfgs.add("L_test");
        mcfgs.add("E_badLevel");
        mcfgs.add("numCores");
        icfgs.add("maxId");
        vo.bind("Module_State", om.findStrict("ti.uia.loggers.LoggerRunMode.Module_State", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerRunMode.Module_State", "ti.uia.loggers"));
        vo.bind("Instance_State", om.findStrict("ti.uia.loggers.LoggerRunMode.Instance_State", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerRunMode.Instance_State", "ti.uia.loggers"));
        vo.bind("TransferType_RELIABLE", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_RELIABLE", "ti.uia.loggers"));
        vo.bind("TransferType_LOSSY", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_LOSSY", "ti.uia.loggers"));
        vo.bind("Priority_LOW", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_LOW", "ti.uia.loggers"));
        vo.bind("Priority_STANDARD", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_STANDARD", "ti.uia.loggers"));
        vo.bind("Priority_HIGH", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_HIGH", "ti.uia.loggers"));
        vo.bind("Priority_SYNC", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_SYNC", "ti.uia.loggers"));
        vo.bind("TransportType_JTAG", om.findStrict("ti.uia.loggers.LoggerRunMode.TransportType_JTAG", "ti.uia.loggers"));
        vo.bind("TransportType_ETHERNET", om.findStrict("ti.uia.loggers.LoggerRunMode.TransportType_ETHERNET", "ti.uia.loggers"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.runtime");
        inherits.add("ti.uia.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.loggers")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.uia.loggers.LoggerRunMode$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$uia$loggers$LoggerRunMode$$__initObject"));
        }//isCFG
        vo.bind("isUploadRequired", om.findStrict("ti.uia.loggers.LoggerRunMode.isUploadRequired", "ti.uia.loggers"));
        vo.bind("idleHook", om.findStrict("ti.uia.loggers.LoggerRunMode.idleHook", "ti.uia.loggers"));
        vo.bind("filterOutEvent", om.findStrict("ti.uia.loggers.LoggerRunMode.filterOutEvent", "ti.uia.loggers"));
        vo.bind("writeStart", om.findStrict("ti.uia.loggers.LoggerRunMode.writeStart", "ti.uia.loggers"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_loggers_LoggerRunMode_Handle__label__E", "ti_uia_loggers_LoggerRunMode_Module__startupDone__E", "ti_uia_loggers_LoggerRunMode_Object__create__E", "ti_uia_loggers_LoggerRunMode_Object__delete__E", "ti_uia_loggers_LoggerRunMode_Object__get__E", "ti_uia_loggers_LoggerRunMode_Object__first__E", "ti_uia_loggers_LoggerRunMode_Object__next__E", "ti_uia_loggers_LoggerRunMode_Params__init__E", "ti_uia_loggers_LoggerRunMode_enable__E", "ti_uia_loggers_LoggerRunMode_disable__E", "ti_uia_loggers_LoggerRunMode_getTransferType__E", "ti_uia_loggers_LoggerRunMode_getContents__E", "ti_uia_loggers_LoggerRunMode_isEmpty__E", "ti_uia_loggers_LoggerRunMode_getMaxLength__E", "ti_uia_loggers_LoggerRunMode_getInstanceId__E", "ti_uia_loggers_LoggerRunMode_getPriority__E", "ti_uia_loggers_LoggerRunMode_setPriority__E", "ti_uia_loggers_LoggerRunMode_reset__E", "ti_uia_loggers_LoggerRunMode_writeMemoryRange__E", "ti_uia_loggers_LoggerRunMode_isUploadRequired__E", "ti_uia_loggers_LoggerRunMode_idleHook__E", "ti_uia_loggers_LoggerRunMode_initBuffer__E", "ti_uia_loggers_LoggerRunMode_flush__E", "ti_uia_loggers_LoggerRunMode_prime__E", "ti_uia_loggers_LoggerRunMode_exchange__E", "ti_uia_loggers_LoggerRunMode_initQueueDescriptor__E", "ti_uia_loggers_LoggerRunMode_write0__E", "ti_uia_loggers_LoggerRunMode_write1__E", "ti_uia_loggers_LoggerRunMode_write2__E", "ti_uia_loggers_LoggerRunMode_write4__E", "ti_uia_loggers_LoggerRunMode_write8__E", "ti_uia_loggers_LoggerRunMode_setFilterLevel__E", "ti_uia_loggers_LoggerRunMode_getFilterLevel__E", "ti_uia_loggers_LoggerRunMode_getBufSize__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("L_test"));
        vo.bind("$$errorDescCfgs", Global.newArray("E_badLevel"));
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", "./LoggerRunMode.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.uia.loggers.LoggerRunMode.Object", "ti.uia.loggers"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./LoggerRunMode.xdt");
        pkgV.bind("LoggerRunMode", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerRunMode");
    }

    void LoggerMin$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.loggers.LoggerMin", "ti.uia.loggers");
        po = (Proto.Obj)om.findStrict("ti.uia.loggers.LoggerMin.Module", "ti.uia.loggers");
        vo.init2(po, "ti.uia.loggers.LoggerMin", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.loggers.LoggerMin$$capsule", "ti.uia.loggers"));
        vo.bind("Instance", om.findStrict("ti.uia.loggers.LoggerMin.Instance", "ti.uia.loggers"));
        vo.bind("Params", om.findStrict("ti.uia.loggers.LoggerMin.Params", "ti.uia.loggers"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.loggers.LoggerMin.Params", "ti.uia.loggers")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.loggers.LoggerMin.Handle", "ti.uia.loggers"));
        vo.bind("$package", om.findStrict("ti.uia.loggers", "ti.uia.loggers"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("TimestampSize", om.findStrict("ti.uia.loggers.LoggerMin.TimestampSize", "ti.uia.loggers"));
        vo.bind("ModuleView", om.findStrict("ti.uia.loggers.LoggerMin.ModuleView", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerMin.ModuleView", "ti.uia.loggers"));
        vo.bind("RecordView", om.findStrict("ti.uia.loggers.LoggerMin.RecordView", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerMin.RecordView", "ti.uia.loggers"));
        vo.bind("RtaData", om.findStrict("ti.uia.loggers.LoggerMin.RtaData", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerMin.RtaData", "ti.uia.loggers"));
        mcfgs.add("bufSize");
        mcfgs.add("numCores");
        mcfgs.add("timestampSize");
        mcfgs.add("L_test");
        mcfgs.add("supportLoggerDisable");
        mcfgs.add("endpointId");
        mcfgs.add("loggerInstanceId");
        vo.bind("Module_State", om.findStrict("ti.uia.loggers.LoggerMin.Module_State", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerMin.Module_State", "ti.uia.loggers"));
        vo.bind("Instance_State", om.findStrict("ti.uia.loggers.LoggerMin.Instance_State", "ti.uia.loggers"));
        tdefs.add(om.findStrict("ti.uia.loggers.LoggerMin.Instance_State", "ti.uia.loggers"));
        vo.bind("TimestampSize_NONE", om.findStrict("ti.uia.loggers.LoggerMin.TimestampSize_NONE", "ti.uia.loggers"));
        vo.bind("TimestampSize_32b", om.findStrict("ti.uia.loggers.LoggerMin.TimestampSize_32b", "ti.uia.loggers"));
        vo.bind("TimestampSize_64b", om.findStrict("ti.uia.loggers.LoggerMin.TimestampSize_64b", "ti.uia.loggers"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.loggers")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.uia.loggers.LoggerMin$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$uia$loggers$LoggerMin$$__initObject"));
        }//isCFG
        vo.bind("flush", om.findStrict("ti.uia.loggers.LoggerMin.flush", "ti.uia.loggers"));
        vo.bind("write", om.findStrict("ti.uia.loggers.LoggerMin.write", "ti.uia.loggers"));
        vo.bind("getContents", om.findStrict("ti.uia.loggers.LoggerMin.getContents", "ti.uia.loggers"));
        vo.bind("isEmpty", om.findStrict("ti.uia.loggers.LoggerMin.isEmpty", "ti.uia.loggers"));
        vo.bind("genTimestamp", om.findStrict("ti.uia.loggers.LoggerMin.genTimestamp", "ti.uia.loggers"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_loggers_LoggerMin_Handle__label__E", "ti_uia_loggers_LoggerMin_Module__startupDone__E", "ti_uia_loggers_LoggerMin_Object__create__E", "ti_uia_loggers_LoggerMin_Object__delete__E", "ti_uia_loggers_LoggerMin_Object__get__E", "ti_uia_loggers_LoggerMin_Object__first__E", "ti_uia_loggers_LoggerMin_Object__next__E", "ti_uia_loggers_LoggerMin_Params__init__E", "ti_uia_loggers_LoggerMin_initBuffer__E", "ti_uia_loggers_LoggerMin_flush__E", "ti_uia_loggers_LoggerMin_write__E", "ti_uia_loggers_LoggerMin_write0__E", "ti_uia_loggers_LoggerMin_write1__E", "ti_uia_loggers_LoggerMin_write2__E", "ti_uia_loggers_LoggerMin_write4__E", "ti_uia_loggers_LoggerMin_write8__E", "ti_uia_loggers_LoggerMin_enable__E", "ti_uia_loggers_LoggerMin_disable__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("L_test"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", "./LoggerMin.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.uia.loggers.LoggerMin.Object", "ti.uia.loggers"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./LoggerMin.xdt");
        pkgV.bind("LoggerMin", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerMin");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.uia.loggers")).findStrict("PARAMS", "ti.uia.loggers");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.uia.loggers.LoggerStopMode", "ti.uia.loggers")).findStrict("PARAMS", "ti.uia.loggers");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.uia.loggers.LoggerRunMode", "ti.uia.loggers")).findStrict("PARAMS", "ti.uia.loggers");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.uia.loggers.LoggerMin", "ti.uia.loggers")).findStrict("PARAMS", "ti.uia.loggers");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.loggers.LoggerStopMode", "ti.uia.loggers"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.loggers.LoggerRunMode", "ti.uia.loggers"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.loggers.LoggerMin", "ti.uia.loggers"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.loggers.LoggerStopMode", "ti.uia.loggers");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")}), Global.newArray(new Object[]{"Instances", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitInstances", "structName", "InstanceView")}), Global.newArray(new Object[]{"Records", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE_DATA"), "viewInitFxn", "viewInitRecords", "structName", "RecordView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.loggers.LoggerRunMode", "ti.uia.loggers");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")}), Global.newArray(new Object[]{"Instances", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitInstances", "structName", "InstanceView")}), Global.newArray(new Object[]{"Records", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE_DATA"), "viewInitFxn", "viewInitRecords", "structName", "RecordView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.loggers.LoggerMin", "ti.uia.loggers");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")}), Global.newArray(new Object[]{"Records", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitRecords", "structName", "RecordView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.uia.loggers.LoggerStopMode")).bless();
        ((Value.Obj)om.getv("ti.uia.loggers.LoggerRunMode")).bless();
        ((Value.Obj)om.getv("ti.uia.loggers.LoggerMin")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.uia.loggers")).add(pkgV);
    }

    public void exec( Scriptable xdcO, Session ses )
    {
        this.xdcO = xdcO;
        this.ses = ses;
        om = (Value.Obj)xdcO.get("om", null);

        Object o = om.geto("$name");
        String s = o instanceof String ? (String)o : null;
        isCFG = s != null && s.equals("cfg");
        isROV = s != null && s.equals("rov");

        $$IMPORTS();
        $$OBJECTS();
        LoggerStopMode$$OBJECTS();
        LoggerRunMode$$OBJECTS();
        LoggerMin$$OBJECTS();
        LoggerStopMode$$CONSTS();
        LoggerRunMode$$CONSTS();
        LoggerMin$$CONSTS();
        LoggerStopMode$$CREATES();
        LoggerRunMode$$CREATES();
        LoggerMin$$CREATES();
        LoggerStopMode$$FUNCTIONS();
        LoggerRunMode$$FUNCTIONS();
        LoggerMin$$FUNCTIONS();
        LoggerStopMode$$SIZES();
        LoggerRunMode$$SIZES();
        LoggerMin$$SIZES();
        LoggerStopMode$$TYPES();
        LoggerRunMode$$TYPES();
        LoggerMin$$TYPES();
        if (isROV) {
            LoggerStopMode$$ROV();
            LoggerRunMode$$ROV();
            LoggerMin$$ROV();
        }//isROV
        $$SINGLETONS();
        LoggerStopMode$$SINGLETONS();
        LoggerRunMode$$SINGLETONS();
        LoggerMin$$SINGLETONS();
        $$INITIALIZATION();
    }
}
