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

public class ti_sysbios
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
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.family");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios", new Value.Obj("ti.sysbios", pkgP));
    }

    void BIOS$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.BIOS.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.BIOS", new Value.Obj("ti.sysbios.BIOS", po));
        pkgV.bind("BIOS", vo);
        // decls 
        om.bind("ti.sysbios.BIOS.ThreadType", new Proto.Enm("ti.sysbios.BIOS.ThreadType"));
        om.bind("ti.sysbios.BIOS.RtsLockType", new Proto.Enm("ti.sysbios.BIOS.RtsLockType"));
        om.bind("ti.sysbios.BIOS.LibType", new Proto.Enm("ti.sysbios.BIOS.LibType"));
        spo = (Proto.Obj)om.bind("ti.sysbios.BIOS$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.BIOS.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.BIOS$$ErrorView", new Proto.Obj());
        om.bind("ti.sysbios.BIOS.ErrorView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.BIOS$$intSize", new Proto.Obj());
        om.bind("ti.sysbios.BIOS.intSize", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.BIOS$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.BIOS.Module_State", new Proto.Str(spo, false));
    }

    void Build$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.Build.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.Build", new Value.Obj("ti.sysbios.Build", po));
        pkgV.bind("Build", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.Build$$Component", new Proto.Obj());
        om.bind("ti.sysbios.Build.Component", new Proto.Str(spo, true));
    }

    void BIOS_RtsGateProxy$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.BIOS_RtsGateProxy.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.BIOS_RtsGateProxy", new Value.Obj("ti.sysbios.BIOS_RtsGateProxy", po));
        pkgV.bind("BIOS_RtsGateProxy", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.sysbios.BIOS_RtsGateProxy.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.BIOS_RtsGateProxy$$Object", new Proto.Obj());
        om.bind("ti.sysbios.BIOS_RtsGateProxy.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.BIOS_RtsGateProxy$$Params", new Proto.Obj());
        om.bind("ti.sysbios.BIOS_RtsGateProxy.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.BIOS_RtsGateProxy$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.BIOS_RtsGateProxy.Instance_State", new Proto.Str(po, false));
        om.bind("ti.sysbios.BIOS_RtsGateProxy.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.BIOS_RtsGateProxy.Object", om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Instance_State", "ti.sysbios"));
        }//isROV
    }

    void BIOS$$CONSTS()
    {
        // module BIOS
        om.bind("ti.sysbios.BIOS.ThreadType_Hwi", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.ThreadType", "ti.sysbios"), "ti.sysbios.BIOS.ThreadType_Hwi", 0));
        om.bind("ti.sysbios.BIOS.ThreadType_Swi", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.ThreadType", "ti.sysbios"), "ti.sysbios.BIOS.ThreadType_Swi", 1));
        om.bind("ti.sysbios.BIOS.ThreadType_Task", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.ThreadType", "ti.sysbios"), "ti.sysbios.BIOS.ThreadType_Task", 2));
        om.bind("ti.sysbios.BIOS.ThreadType_Main", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.ThreadType", "ti.sysbios"), "ti.sysbios.BIOS.ThreadType_Main", 3));
        om.bind("ti.sysbios.BIOS.NoLocking", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.RtsLockType", "ti.sysbios"), "ti.sysbios.BIOS.NoLocking", 0));
        om.bind("ti.sysbios.BIOS.GateHwi", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.RtsLockType", "ti.sysbios"), "ti.sysbios.BIOS.GateHwi", 1));
        om.bind("ti.sysbios.BIOS.GateSwi", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.RtsLockType", "ti.sysbios"), "ti.sysbios.BIOS.GateSwi", 2));
        om.bind("ti.sysbios.BIOS.GateMutex", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.RtsLockType", "ti.sysbios"), "ti.sysbios.BIOS.GateMutex", 3));
        om.bind("ti.sysbios.BIOS.GateMutexPri", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.RtsLockType", "ti.sysbios"), "ti.sysbios.BIOS.GateMutexPri", 4));
        om.bind("ti.sysbios.BIOS.LibType_Instrumented", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.LibType", "ti.sysbios"), "ti.sysbios.BIOS.LibType_Instrumented", 0));
        om.bind("ti.sysbios.BIOS.LibType_NonInstrumented", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.LibType", "ti.sysbios"), "ti.sysbios.BIOS.LibType_NonInstrumented", 1));
        om.bind("ti.sysbios.BIOS.LibType_Custom", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.LibType", "ti.sysbios"), "ti.sysbios.BIOS.LibType_Custom", 2));
        om.bind("ti.sysbios.BIOS.LibType_Debug", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.BIOS.LibType", "ti.sysbios"), "ti.sysbios.BIOS.LibType_Debug", 3));
        om.bind("ti.sysbios.BIOS.WAIT_FOREVER", Global.eval("~(0)"));
        om.bind("ti.sysbios.BIOS.NO_WAIT", 0L);
        om.bind("ti.sysbios.BIOS.version", 0x68104L);
        om.bind("ti.sysbios.BIOS.linkedWithIncorrectBootLibrary", new Extern("ti_sysbios_BIOS_linkedWithIncorrectBootLibrary__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.BIOS.start", new Extern("ti_sysbios_BIOS_start__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.BIOS.exit", new Extern("ti_sysbios_BIOS_exit__E", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.BIOS.getThreadType", new Extern("ti_sysbios_BIOS_getThreadType__E", "ti_sysbios_BIOS_ThreadType(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.BIOS.setThreadType", new Extern("ti_sysbios_BIOS_setThreadType__E", "ti_sysbios_BIOS_ThreadType(*)(ti_sysbios_BIOS_ThreadType)", true, false));
        om.bind("ti.sysbios.BIOS.setCpuFreq", new Extern("ti_sysbios_BIOS_setCpuFreq__E", "xdc_Void(*)(xdc_runtime_Types_FreqHz*)", true, false));
        om.bind("ti.sysbios.BIOS.getCpuFreq", new Extern("ti_sysbios_BIOS_getCpuFreq__E", "xdc_Void(*)(xdc_runtime_Types_FreqHz*)", true, false));
        om.bind("ti.sysbios.BIOS.errorRaiseHook", new Extern("ti_sysbios_BIOS_errorRaiseHook__I", "xdc_Void(*)(xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.BIOS.startFunc", new Extern("ti_sysbios_BIOS_startFunc__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.BIOS.atExitFunc", new Extern("ti_sysbios_BIOS_atExitFunc__I", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.BIOS.exitFunc", new Extern("ti_sysbios_BIOS_exitFunc__I", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.BIOS.registerRTSLock", new Extern("ti_sysbios_BIOS_registerRTSLock__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.BIOS.removeRTSLock", new Extern("ti_sysbios_BIOS_removeRTSLock__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.BIOS.rtsLock", new Extern("ti_sysbios_BIOS_rtsLock__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.BIOS.rtsUnlock", new Extern("ti_sysbios_BIOS_rtsUnlock__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.BIOS.nullFunc", new Extern("ti_sysbios_BIOS_nullFunc__I", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void Build$$CONSTS()
    {
        // module Build
    }

    void BIOS_RtsGateProxy$$CONSTS()
    {
        // module BIOS_RtsGateProxy
        om.bind("ti.sysbios.BIOS_RtsGateProxy.query", new Extern("ti_sysbios_BIOS_RtsGateProxy_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void BIOS$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Build$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void BIOS_RtsGateProxy$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$BIOS_RtsGateProxy$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.BIOS_RtsGateProxy.create() called before xdc.useModule('ti.sysbios.BIOS_RtsGateProxy')\");\n");
                sb.append("}\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.BIOS_RtsGateProxy$$create", new Proto.Fxn(om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Module", "ti.sysbios"), om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Instance", "ti.sysbios"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Params", "ti.sysbios"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$BIOS_RtsGateProxy$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.BIOS_RtsGateProxy'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.BIOS_RtsGateProxy.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.BIOS_RtsGateProxy'].Instance_State);\n");
                sb.append("if (!__mod.delegate$) {\n");
                    sb.append("throw new Error(\"Unbound proxy module: ti.sysbios.BIOS_RtsGateProxy\");\n");
                sb.append("}\n");
                sb.append("var __dmod = __mod.delegate$.$orig;\n");
                sb.append("var __dinst = __dmod.create(__params);\n");
                sb.append("__inst.$$bind('delegate$', __dinst);\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.BIOS_RtsGateProxy'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void BIOS$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn BIOS.addUserStartupFunction
        fxn = (Proto.Fxn)om.bind("ti.sysbios.BIOS$$addUserStartupFunction", new Proto.Fxn(om.findStrict("ti.sysbios.BIOS.Module", "ti.sysbios"), null, 1, 1, false));
                fxn.addArg(0, "func", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF);
        // fxn BIOS.getCpuFrequency
        fxn = (Proto.Fxn)om.bind("ti.sysbios.BIOS$$getCpuFrequency", new Proto.Fxn(om.findStrict("ti.sysbios.BIOS.Module", "ti.sysbios"), Proto.Elm.newCNum("(xdc_UInt64)"), 0, 0, false));
        // fxn BIOS.getCpuFreqMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.BIOS$$getCpuFreqMeta", new Proto.Fxn(om.findStrict("ti.sysbios.BIOS.Module", "ti.sysbios"), (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios"), 0, 0, false));
        // fxn BIOS.getTimestampFrequency
        fxn = (Proto.Fxn)om.bind("ti.sysbios.BIOS$$getTimestampFrequency", new Proto.Fxn(om.findStrict("ti.sysbios.BIOS.Module", "ti.sysbios"), Proto.Elm.newCNum("(xdc_UInt64)"), 0, 0, false));
        // fxn BIOS.getTimestampFreqMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.BIOS$$getTimestampFreqMeta", new Proto.Fxn(om.findStrict("ti.sysbios.BIOS.Module", "ti.sysbios"), (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios"), 0, 0, false));
        // fxn BIOS.getDefaultTimestampProvider
        fxn = (Proto.Fxn)om.bind("ti.sysbios.BIOS$$getDefaultTimestampProvider", new Proto.Fxn(om.findStrict("ti.sysbios.BIOS.Module", "ti.sysbios"), $$T_Str, 0, 0, false));
        // fxn BIOS.getCCOpts
        fxn = (Proto.Fxn)om.bind("ti.sysbios.BIOS$$getCCOpts", new Proto.Fxn(om.findStrict("ti.sysbios.BIOS.Module", "ti.sysbios"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "target", $$T_Str, $$UNDEF);
        // fxn BIOS.fireFrequencyUpdate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.BIOS$$fireFrequencyUpdate", new Proto.Fxn(om.findStrict("ti.sysbios.BIOS.Module", "ti.sysbios"), null, 0, -1, false));
    }

    void Build$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Build.getDefaultCustomCCOpts
        fxn = (Proto.Fxn)om.bind("ti.sysbios.Build$$getDefaultCustomCCOpts", new Proto.Fxn(om.findStrict("ti.sysbios.Build.Module", "ti.sysbios"), $$T_Str, 0, 0, false));
        // fxn Build.getDefs
        fxn = (Proto.Fxn)om.bind("ti.sysbios.Build$$getDefs", new Proto.Fxn(om.findStrict("ti.sysbios.Build.Module", "ti.sysbios"), $$T_Str, 0, 0, false));
        // fxn Build.getCFiles
        fxn = (Proto.Fxn)om.bind("ti.sysbios.Build$$getCFiles", new Proto.Fxn(om.findStrict("ti.sysbios.Build.Module", "ti.sysbios"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "target", $$T_Str, $$UNDEF);
        // fxn Build.getAsmFiles
        fxn = (Proto.Fxn)om.bind("ti.sysbios.Build$$getAsmFiles", new Proto.Fxn(om.findStrict("ti.sysbios.Build.Module", "ti.sysbios"), $$T_Obj, 1, 1, false));
                fxn.addArg(0, "target", $$T_Str, $$UNDEF);
        // fxn Build.getCommandLineDefs
        fxn = (Proto.Fxn)om.bind("ti.sysbios.Build$$getCommandLineDefs", new Proto.Fxn(om.findStrict("ti.sysbios.Build.Module", "ti.sysbios"), $$T_Str, 0, 0, false));
        // fxn Build.getIncludePaths
        fxn = (Proto.Fxn)om.bind("ti.sysbios.Build$$getIncludePaths", new Proto.Fxn(om.findStrict("ti.sysbios.Build.Module", "ti.sysbios"), $$T_Str, 0, 0, false));
        // fxn Build.getCcArgs
        fxn = (Proto.Fxn)om.bind("ti.sysbios.Build$$getCcArgs", new Proto.Fxn(om.findStrict("ti.sysbios.Build.Module", "ti.sysbios"), $$T_Str, 0, 0, false));
        // fxn Build.buildLibs
        fxn = (Proto.Fxn)om.bind("ti.sysbios.Build$$buildLibs", new Proto.Fxn(om.findStrict("ti.sysbios.Build.Module", "ti.sysbios"), null, 0, -1, false));
        // fxn Build.getLibs
        fxn = (Proto.Fxn)om.bind("ti.sysbios.Build$$getLibs", new Proto.Fxn(om.findStrict("ti.sysbios.Build.Module", "ti.sysbios"), null, 0, -1, false));
    }

    void BIOS_RtsGateProxy$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void BIOS$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.BIOS.intSize", "ti.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("intSize", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.BIOS.intSize']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.BIOS.intSize']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.BIOS.intSize'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.BIOS.Module_State", "ti.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("cpuFreq", "Sxdc.runtime.Types;FreqHz"));
        sizes.add(Global.newArray("rtsGateCount", "UInt"));
        sizes.add(Global.newArray("rtsGateKey", "TIArg"));
        sizes.add(Global.newArray("rtsGate", "UPtr"));
        sizes.add(Global.newArray("threadType", "Nti.sysbios.BIOS.ThreadType;;;;;"));
        sizes.add(Global.newArray("smpThreadType", "UPtr"));
        sizes.add(Global.newArray("startFunc", "UFxn"));
        sizes.add(Global.newArray("exitFunc", "UFxn"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.BIOS.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.BIOS.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.BIOS.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Build$$SIZES()
    {
    }

    void BIOS_RtsGateProxy$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void BIOS$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/BIOS.xs");
        om.bind("ti.sysbios.BIOS$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS.Module", "ti.sysbios");
        po.init("ti.sysbios.BIOS.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
                po.addFld("NO_WAIT", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
                po.addFld("version", Proto.Elm.newCNum("(xdc_UInt32)"), 0x68104L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios"), $$UNDEF, "wh");
            po.addFld("libType", (Proto)om.findStrict("ti.sysbios.BIOS.LibType", "ti.sysbios"), om.find("ti.sysbios.BIOS.LibType_Instrumented"), "wh");
            po.addFld("customCCOpts", $$T_Str, $$UNDEF, "wh");
            po.addFld("includeXdcRuntime", $$T_Bool, false, "wh");
            po.addFld("smpEnabled", $$T_Bool, false, "w");
            po.addFld("psaEnabled", $$T_Bool, false, "wh");
            po.addFld("cpuFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios"), $$DEFAULT, "w");
            po.addFld("runtimeCreatesEnabled", $$T_Bool, true, "w");
            po.addFld("taskEnabled", $$T_Bool, true, "w");
            po.addFld("swiEnabled", $$T_Bool, true, "w");
            po.addFld("clockEnabled", $$T_Bool, true, "w");
            po.addFld("assertsEnabled", $$T_Bool, true, "wh");
            po.addFld("logsEnabled", $$T_Bool, true, "wh");
            po.addFld("heapSize", Proto.Elm.newCNum("(xdc_SizeT)"), 0x1000L, "w");
            po.addFld("heapSection", $$T_Str, null, "w");
            po.addFld("heapTrackEnabled", $$T_Bool, false, "w");
            po.addFld("setupSecureContext", $$T_Bool, false, "w");
            po.addFld("useSK", $$T_Bool, false, "w");
            po.addFld("rtsGateType", (Proto)om.findStrict("ti.sysbios.BIOS.RtsLockType", "ti.sysbios"), $$UNDEF, "wh");
            po.addFld("startupFxns", new Proto.Arr(new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), false), Global.newArray(new Object[]{}), "wh");
            po.addFld("buildingAppLib", $$T_Bool, true, "wh");
            po.addFld("libDir", $$T_Str, null, "wh");
            po.addFld("codeCoverageEnabled", $$T_Bool, false, "wh");
            po.addFld("bitsPerInt", Proto.Elm.newCNum("(xdc_Char)"), $$UNDEF, "wh");
            po.addFld("installedErrorHook", new Proto.Adr("xdc_Void(*)(xdc_runtime_Error_Block*)", "PFv"), $$UNDEF, "w");
            po.addFldV("RtsGateProxy", (Proto)om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios"), null, "wh", $$delegGet, $$delegSet);
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.BIOS$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.BIOS$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.BIOS$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.BIOS$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("addUserStartupFunction", (Proto.Fxn)om.findStrict("ti.sysbios.BIOS$$addUserStartupFunction", "ti.sysbios"), Global.get(cap, "addUserStartupFunction"));
                po.addFxn("getCpuFrequency", (Proto.Fxn)om.findStrict("ti.sysbios.BIOS$$getCpuFrequency", "ti.sysbios"), Global.get(cap, "getCpuFrequency"));
                po.addFxn("getCpuFreqMeta", (Proto.Fxn)om.findStrict("ti.sysbios.BIOS$$getCpuFreqMeta", "ti.sysbios"), Global.get(cap, "getCpuFreqMeta"));
                po.addFxn("getTimestampFrequency", (Proto.Fxn)om.findStrict("ti.sysbios.BIOS$$getTimestampFrequency", "ti.sysbios"), Global.get(cap, "getTimestampFrequency"));
                po.addFxn("getTimestampFreqMeta", (Proto.Fxn)om.findStrict("ti.sysbios.BIOS$$getTimestampFreqMeta", "ti.sysbios"), Global.get(cap, "getTimestampFreqMeta"));
                po.addFxn("getDefaultTimestampProvider", (Proto.Fxn)om.findStrict("ti.sysbios.BIOS$$getDefaultTimestampProvider", "ti.sysbios"), Global.get(cap, "getDefaultTimestampProvider"));
                po.addFxn("getCCOpts", (Proto.Fxn)om.findStrict("ti.sysbios.BIOS$$getCCOpts", "ti.sysbios"), Global.get(cap, "getCCOpts"));
                po.addFxn("fireFrequencyUpdate", (Proto.Fxn)om.findStrict("ti.sysbios.BIOS$$fireFrequencyUpdate", "ti.sysbios"), Global.get(cap, "fireFrequencyUpdate"));
        // typedef BIOS.StartupFuncPtr
        om.bind("ti.sysbios.BIOS.StartupFuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"));
        // struct BIOS.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS$$ModuleView", "ti.sysbios");
        po.init("ti.sysbios.BIOS.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("currentThreadType", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("rtsGateType", $$T_Str, $$UNDEF, "w");
                po.addFld("cpuFreqLow", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("cpuFreqHigh", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("clockEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("swiEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("taskEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("startFunc", $$T_Str, $$UNDEF, "w");
        // struct BIOS.ErrorView
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS$$ErrorView", "ti.sysbios");
        po.init("ti.sysbios.BIOS.ErrorView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("mod", $$T_Str, $$UNDEF, "w");
                po.addFld("tab", $$T_Str, $$UNDEF, "w");
                po.addFld("inst", $$T_Str, $$UNDEF, "w");
                po.addFld("field", $$T_Str, $$UNDEF, "w");
                po.addFld("message", $$T_Str, $$UNDEF, "w");
        // struct BIOS.intSize
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS$$intSize", "ti.sysbios");
        po.init("ti.sysbios.BIOS.intSize", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("intSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // typedef BIOS.StartFuncPtr
        om.bind("ti.sysbios.BIOS.StartFuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"));
        // typedef BIOS.ExitFuncPtr
        om.bind("ti.sysbios.BIOS.ExitFuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"));
        // struct BIOS.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS$$Module_State", "ti.sysbios");
        po.init("ti.sysbios.BIOS.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("cpuFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios"), $$DEFAULT, "w");
                po.addFld("rtsGateCount", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("rtsGateKey", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("rtsGate", (Proto)om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Handle", "ti.sysbios"), $$UNDEF, "w");
                po.addFld("threadType", (Proto)om.findStrict("ti.sysbios.BIOS.ThreadType", "ti.sysbios"), $$UNDEF, "w");
                po.addFld("smpThreadType", new Proto.Arr((Proto)om.findStrict("ti.sysbios.BIOS.ThreadType", "ti.sysbios"), false), $$DEFAULT, "w");
                po.addFld("startFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "w");
                po.addFld("exitFunc", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"), $$UNDEF, "w");
    }

    void Build$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/Build.xs");
        om.bind("ti.sysbios.Build$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.Build.Module", "ti.sysbios");
        po.init("ti.sysbios.Build.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("buildROM", $$T_Bool, false, "wh");
        po.addFld("buildROMApp", $$T_Bool, false, "wh");
        po.addFld("includePaths", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("ccArgs", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("annex", new Proto.Arr((Proto)om.findStrict("ti.sysbios.Build.Component", "ti.sysbios"), false), $$DEFAULT, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.Build$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.Build$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.Build$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getDefaultCustomCCOpts", (Proto.Fxn)om.findStrict("ti.sysbios.Build$$getDefaultCustomCCOpts", "ti.sysbios"), Global.get(cap, "getDefaultCustomCCOpts"));
                po.addFxn("getDefs", (Proto.Fxn)om.findStrict("ti.sysbios.Build$$getDefs", "ti.sysbios"), Global.get(cap, "getDefs"));
                po.addFxn("getCFiles", (Proto.Fxn)om.findStrict("ti.sysbios.Build$$getCFiles", "ti.sysbios"), Global.get(cap, "getCFiles"));
                po.addFxn("getAsmFiles", (Proto.Fxn)om.findStrict("ti.sysbios.Build$$getAsmFiles", "ti.sysbios"), Global.get(cap, "getAsmFiles"));
                po.addFxn("getCommandLineDefs", (Proto.Fxn)om.findStrict("ti.sysbios.Build$$getCommandLineDefs", "ti.sysbios"), Global.get(cap, "getCommandLineDefs"));
                po.addFxn("getIncludePaths", (Proto.Fxn)om.findStrict("ti.sysbios.Build$$getIncludePaths", "ti.sysbios"), Global.get(cap, "getIncludePaths"));
                po.addFxn("getCcArgs", (Proto.Fxn)om.findStrict("ti.sysbios.Build$$getCcArgs", "ti.sysbios"), Global.get(cap, "getCcArgs"));
                po.addFxn("buildLibs", (Proto.Fxn)om.findStrict("ti.sysbios.Build$$buildLibs", "ti.sysbios"), Global.get(cap, "buildLibs"));
                po.addFxn("getLibs", (Proto.Fxn)om.findStrict("ti.sysbios.Build$$getLibs", "ti.sysbios"), Global.get(cap, "getLibs"));
        // struct Build.Component
        po = (Proto.Obj)om.findStrict("ti.sysbios.Build$$Component", "ti.sysbios");
        po.init("ti.sysbios.Build.Component", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("repo", $$T_Str, $$UNDEF, "w");
                po.addFld("incs", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("files", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
    }

    void BIOS_RtsGateProxy$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Module", "ti.sysbios");
        po.init("ti.sysbios.BIOS_RtsGateProxy.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios"));
                po.addFld("delegate$", (Proto)om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios"), null, "wh");
                po.addFld("abstractInstances$", $$T_Bool, false, "wh");
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.BIOS_RtsGateProxy$$create", "ti.sysbios"), Global.get("ti$sysbios$BIOS_RtsGateProxy$$create"));
        }//isCFG
                po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Instance", "ti.sysbios");
        po.init("ti.sysbios.BIOS_RtsGateProxy.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS_RtsGateProxy$$Params", "ti.sysbios");
        po.init("ti.sysbios.BIOS_RtsGateProxy.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS_RtsGateProxy$$Object", "ti.sysbios");
        po.init("ti.sysbios.BIOS_RtsGateProxy.Object", om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Instance", "ti.sysbios"));
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS_RtsGateProxy$$Instance_State", "ti.sysbios");
        po.init("ti.sysbios.BIOS_RtsGateProxy.Instance_State", null);
        po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void BIOS$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.BIOS", "ti.sysbios");
        vo.bind("intSize$fetchDesc", Global.newObject("type", "ti.sysbios.BIOS.intSize", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS$$intSize", "ti.sysbios");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.BIOS.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS$$Module_State", "ti.sysbios");
        po.bind("smpThreadType$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_ThreadType", "isScalar", true));
    }

    void Build$$ROV()
    {
    }

    void BIOS_RtsGateProxy$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.BIOS_RtsGateProxy", "ti.sysbios");
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS_RtsGateProxy$$Instance_State", "ti.sysbios");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.");
        pkgV.bind("$vers", Global.newArray(2, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.family", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.aem4',\n");
            sb.append("'lib/debug/ti.sysbios.am4',\n");
            sb.append("'lib/debug/ti.sysbios.am4g',\n");
            sb.append("'lib/debug/ti.sysbios.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/debug/ti.sysbios.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/debug/ti.sysbios.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/debug/ti.sysbios.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void BIOS$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.BIOS", "ti.sysbios");
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS.Module", "ti.sysbios");
        vo.init2(po, "ti.sysbios.BIOS", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.BIOS$$capsule", "ti.sysbios"));
        vo.bind("$package", om.findStrict("ti.sysbios", "ti.sysbios"));
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
        vo.bind("ThreadType", om.findStrict("ti.sysbios.BIOS.ThreadType", "ti.sysbios"));
        vo.bind("RtsLockType", om.findStrict("ti.sysbios.BIOS.RtsLockType", "ti.sysbios"));
        vo.bind("LibType", om.findStrict("ti.sysbios.BIOS.LibType", "ti.sysbios"));
        vo.bind("StartupFuncPtr", om.findStrict("ti.sysbios.BIOS.StartupFuncPtr", "ti.sysbios"));
        vo.bind("ModuleView", om.findStrict("ti.sysbios.BIOS.ModuleView", "ti.sysbios"));
        tdefs.add(om.findStrict("ti.sysbios.BIOS.ModuleView", "ti.sysbios"));
        vo.bind("ErrorView", om.findStrict("ti.sysbios.BIOS.ErrorView", "ti.sysbios"));
        tdefs.add(om.findStrict("ti.sysbios.BIOS.ErrorView", "ti.sysbios"));
        mcfgs.add("smpEnabled");
        mcfgs.add("cpuFreq");
        mcfgs.add("runtimeCreatesEnabled");
        mcfgs.add("taskEnabled");
        mcfgs.add("swiEnabled");
        mcfgs.add("clockEnabled");
        mcfgs.add("heapSize");
        mcfgs.add("heapSection");
        mcfgs.add("heapTrackEnabled");
        mcfgs.add("setupSecureContext");
        mcfgs.add("useSK");
        icfgs.add("buildingAppLib");
        icfgs.add("libDir");
        icfgs.add("codeCoverageEnabled");
        vo.bind("intSize", om.findStrict("ti.sysbios.BIOS.intSize", "ti.sysbios"));
        tdefs.add(om.findStrict("ti.sysbios.BIOS.intSize", "ti.sysbios"));
        icfgs.add("bitsPerInt");
        mcfgs.add("installedErrorHook");
        icfgs.add("installedErrorHook");
        vo.bind("RtsGateProxy$proxy", om.findStrict("ti.sysbios.BIOS_RtsGateProxy", "ti.sysbios"));
        proxies.add("RtsGateProxy");
        vo.bind("StartFuncPtr", om.findStrict("ti.sysbios.BIOS.StartFuncPtr", "ti.sysbios"));
        vo.bind("ExitFuncPtr", om.findStrict("ti.sysbios.BIOS.ExitFuncPtr", "ti.sysbios"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.BIOS.Module_State", "ti.sysbios"));
        tdefs.add(om.findStrict("ti.sysbios.BIOS.Module_State", "ti.sysbios"));
        vo.bind("ThreadType_Hwi", om.findStrict("ti.sysbios.BIOS.ThreadType_Hwi", "ti.sysbios"));
        vo.bind("ThreadType_Swi", om.findStrict("ti.sysbios.BIOS.ThreadType_Swi", "ti.sysbios"));
        vo.bind("ThreadType_Task", om.findStrict("ti.sysbios.BIOS.ThreadType_Task", "ti.sysbios"));
        vo.bind("ThreadType_Main", om.findStrict("ti.sysbios.BIOS.ThreadType_Main", "ti.sysbios"));
        vo.bind("NoLocking", om.findStrict("ti.sysbios.BIOS.NoLocking", "ti.sysbios"));
        vo.bind("GateHwi", om.findStrict("ti.sysbios.BIOS.GateHwi", "ti.sysbios"));
        vo.bind("GateSwi", om.findStrict("ti.sysbios.BIOS.GateSwi", "ti.sysbios"));
        vo.bind("GateMutex", om.findStrict("ti.sysbios.BIOS.GateMutex", "ti.sysbios"));
        vo.bind("GateMutexPri", om.findStrict("ti.sysbios.BIOS.GateMutexPri", "ti.sysbios"));
        vo.bind("LibType_Instrumented", om.findStrict("ti.sysbios.BIOS.LibType_Instrumented", "ti.sysbios"));
        vo.bind("LibType_NonInstrumented", om.findStrict("ti.sysbios.BIOS.LibType_NonInstrumented", "ti.sysbios"));
        vo.bind("LibType_Custom", om.findStrict("ti.sysbios.BIOS.LibType_Custom", "ti.sysbios"));
        vo.bind("LibType_Debug", om.findStrict("ti.sysbios.BIOS.LibType_Debug", "ti.sysbios"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("linkedWithIncorrectBootLibrary", om.findStrict("ti.sysbios.BIOS.linkedWithIncorrectBootLibrary", "ti.sysbios"));
        vo.bind("start", om.findStrict("ti.sysbios.BIOS.start", "ti.sysbios"));
        vo.bind("exit", om.findStrict("ti.sysbios.BIOS.exit", "ti.sysbios"));
        vo.bind("getThreadType", om.findStrict("ti.sysbios.BIOS.getThreadType", "ti.sysbios"));
        vo.bind("setThreadType", om.findStrict("ti.sysbios.BIOS.setThreadType", "ti.sysbios"));
        vo.bind("setCpuFreq", om.findStrict("ti.sysbios.BIOS.setCpuFreq", "ti.sysbios"));
        vo.bind("getCpuFreq", om.findStrict("ti.sysbios.BIOS.getCpuFreq", "ti.sysbios"));
        vo.bind("errorRaiseHook", om.findStrict("ti.sysbios.BIOS.errorRaiseHook", "ti.sysbios"));
        vo.bind("startFunc", om.findStrict("ti.sysbios.BIOS.startFunc", "ti.sysbios"));
        vo.bind("atExitFunc", om.findStrict("ti.sysbios.BIOS.atExitFunc", "ti.sysbios"));
        vo.bind("exitFunc", om.findStrict("ti.sysbios.BIOS.exitFunc", "ti.sysbios"));
        vo.bind("registerRTSLock", om.findStrict("ti.sysbios.BIOS.registerRTSLock", "ti.sysbios"));
        vo.bind("removeRTSLock", om.findStrict("ti.sysbios.BIOS.removeRTSLock", "ti.sysbios"));
        vo.bind("rtsLock", om.findStrict("ti.sysbios.BIOS.rtsLock", "ti.sysbios"));
        vo.bind("rtsUnlock", om.findStrict("ti.sysbios.BIOS.rtsUnlock", "ti.sysbios"));
        vo.bind("nullFunc", om.findStrict("ti.sysbios.BIOS.nullFunc", "ti.sysbios"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_BIOS_Module__startupDone__E", "ti_sysbios_BIOS_linkedWithIncorrectBootLibrary__E", "ti_sysbios_BIOS_start__E", "ti_sysbios_BIOS_exit__E", "ti_sysbios_BIOS_getThreadType__E", "ti_sysbios_BIOS_setThreadType__E", "ti_sysbios_BIOS_setCpuFreq__E", "ti_sysbios_BIOS_getCpuFreq__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", "./BIOS.xdt");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./BIOS.xdt");
        pkgV.bind("BIOS", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("BIOS");
    }

    void Build$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.Build", "ti.sysbios");
        po = (Proto.Obj)om.findStrict("ti.sysbios.Build.Module", "ti.sysbios");
        vo.init2(po, "ti.sysbios.Build", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.Build$$capsule", "ti.sysbios"));
        vo.bind("$package", om.findStrict("ti.sysbios", "ti.sysbios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Component", om.findStrict("ti.sysbios.Build.Component", "ti.sysbios"));
        tdefs.add(om.findStrict("ti.sysbios.Build.Component", "ti.sysbios"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.sysbios.Build$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./Build.xdt");
        atmap.seal("length");
        vo.bind("TEMPLATE$", "./Build.xdt");
        pkgV.bind("Build", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Build");
    }

    void BIOS_RtsGateProxy$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.BIOS_RtsGateProxy", "ti.sysbios");
        po = (Proto.Obj)om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Module", "ti.sysbios");
        vo.init2(po, "ti.sysbios.BIOS_RtsGateProxy", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Instance", "ti.sysbios"));
        vo.bind("Params", om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Params", "ti.sysbios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Params", "ti.sysbios")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Handle", "ti.sysbios"));
        vo.bind("$package", om.findStrict("ti.sysbios", "ti.sysbios"));
        tdefs.clear();
        proxies.clear();
        proxies.add("delegate$");
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 0);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
            vo.bind("__initObject", Global.get("ti$sysbios$BIOS_RtsGateProxy$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.BIOS_RtsGateProxy.query", "ti.sysbios"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_BIOS_RtsGateProxy_DELEGATE__Handle__label", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__Module__startupDone", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__Object__create", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__Object__delete", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__Object__get", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__Object__first", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__Object__next", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__Params__init", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__Proxy__abstract", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__Proxy__delegate", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__queryMeta", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__query", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__enter", "ti_sysbios_BIOS_RtsGateProxy_DELEGATE__leave"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Object", "ti.sysbios"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.BIOS_RtsGateProxy.Instance_State", "ti.sysbios"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 1);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("BIOS_RtsGateProxy", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("BIOS_RtsGateProxy");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.sysbios")).findStrict("PARAMS", "ti.sysbios");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.BIOS_RtsGateProxy", "ti.sysbios")).findStrict("PARAMS", "ti.sysbios");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.BIOS", "ti.sysbios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.Build", "ti.sysbios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.BIOS_RtsGateProxy", "ti.sysbios"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.BIOS", "ti.sysbios");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")}), Global.newArray(new Object[]{"Scan for errors...", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitErrorScan", "structName", "ErrorView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.BIOS")).bless();
        ((Value.Obj)om.getv("ti.sysbios.Build")).bless();
        ((Value.Obj)om.getv("ti.sysbios.BIOS_RtsGateProxy")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios")).add(pkgV);
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
        BIOS$$OBJECTS();
        Build$$OBJECTS();
        BIOS_RtsGateProxy$$OBJECTS();
        BIOS$$CONSTS();
        Build$$CONSTS();
        BIOS_RtsGateProxy$$CONSTS();
        BIOS$$CREATES();
        Build$$CREATES();
        BIOS_RtsGateProxy$$CREATES();
        BIOS$$FUNCTIONS();
        Build$$FUNCTIONS();
        BIOS_RtsGateProxy$$FUNCTIONS();
        BIOS$$SIZES();
        Build$$SIZES();
        BIOS_RtsGateProxy$$SIZES();
        BIOS$$TYPES();
        Build$$TYPES();
        BIOS_RtsGateProxy$$TYPES();
        if (isROV) {
            BIOS$$ROV();
            Build$$ROV();
            BIOS_RtsGateProxy$$ROV();
        }//isROV
        $$SINGLETONS();
        BIOS$$SINGLETONS();
        Build$$SINGLETONS();
        BIOS_RtsGateProxy$$SINGLETONS();
        $$INITIALIZATION();
    }
}
