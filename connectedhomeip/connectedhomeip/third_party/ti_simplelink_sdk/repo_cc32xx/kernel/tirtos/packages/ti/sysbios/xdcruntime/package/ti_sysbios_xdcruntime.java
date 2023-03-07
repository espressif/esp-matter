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

public class ti_sysbios_xdcruntime
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
        Global.callFxn("loadPackage", xdcO, "xdc.runtime.knl");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.gates");
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.xdcruntime", new Value.Obj("ti.sysbios.xdcruntime", pkgP));
    }

    void GateThreadSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.GateThreadSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.xdcruntime.GateThreadSupport", new Value.Obj("ti.sysbios.xdcruntime.GateThreadSupport", po));
        pkgV.bind("GateThreadSupport", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.GateThreadSupport.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.xdcruntime.GateThreadSupport.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$Object", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.GateThreadSupport.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$Params", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.GateThreadSupport.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.xdcruntime.GateThreadSupport.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.xdcruntime.GateThreadSupport.Object", om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Instance_State", "ti.sysbios.xdcruntime"));
        }//isROV
    }

    void GateProcessSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.GateProcessSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.xdcruntime.GateProcessSupport", new Value.Obj("ti.sysbios.xdcruntime.GateProcessSupport", po));
        pkgV.bind("GateProcessSupport", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.GateProcessSupport.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.xdcruntime.GateProcessSupport.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$Object", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.GateProcessSupport.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$Params", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.GateProcessSupport.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.xdcruntime.GateProcessSupport.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.xdcruntime.GateProcessSupport.Object", om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Instance_State", "ti.sysbios.xdcruntime"));
        }//isROV
    }

    void SemThreadSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.SemThreadSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.xdcruntime.SemThreadSupport", new Value.Obj("ti.sysbios.xdcruntime.SemThreadSupport", po));
        pkgV.bind("SemThreadSupport", vo);
        // decls 
        om.bind("ti.sysbios.xdcruntime.SemThreadSupport.PendStatus", om.findStrict("xdc.runtime.knl.ISemaphore.PendStatus", "ti.sysbios.xdcruntime"));
        om.bind("ti.sysbios.xdcruntime.SemThreadSupport.Mode", om.findStrict("xdc.runtime.knl.ISemaphore.Mode", "ti.sysbios.xdcruntime"));
        spo = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.SemThreadSupport.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.SemThreadSupport.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.xdcruntime.SemThreadSupport.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$Object", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.SemThreadSupport.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$Params", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.SemThreadSupport.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.xdcruntime.SemThreadSupport.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.xdcruntime.SemThreadSupport.Object", om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Instance_State", "ti.sysbios.xdcruntime"));
        }//isROV
    }

    void SemProcessSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.SemProcessSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.xdcruntime.SemProcessSupport", new Value.Obj("ti.sysbios.xdcruntime.SemProcessSupport", po));
        pkgV.bind("SemProcessSupport", vo);
        // decls 
        om.bind("ti.sysbios.xdcruntime.SemProcessSupport.PendStatus", om.findStrict("xdc.runtime.knl.ISemaphore.PendStatus", "ti.sysbios.xdcruntime"));
        om.bind("ti.sysbios.xdcruntime.SemProcessSupport.Mode", om.findStrict("xdc.runtime.knl.ISemaphore.Mode", "ti.sysbios.xdcruntime"));
        spo = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.SemProcessSupport.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.xdcruntime.SemProcessSupport.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$Object", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.SemProcessSupport.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$Params", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.SemProcessSupport.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.xdcruntime.SemProcessSupport.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.xdcruntime.SemProcessSupport.Object", om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Instance_State", "ti.sysbios.xdcruntime"));
        }//isROV
    }

    void ThreadSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.ThreadSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.xdcruntime.ThreadSupport", new Value.Obj("ti.sysbios.xdcruntime.ThreadSupport", po));
        pkgV.bind("ThreadSupport", vo);
        // decls 
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.Priority", om.findStrict("xdc.runtime.knl.IThreadSupport.Priority", "ti.sysbios.xdcruntime"));
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.CompStatus", om.findStrict("xdc.runtime.knl.IThreadSupport.CompStatus", "ti.sysbios.xdcruntime"));
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.Stat", om.findStrict("xdc.runtime.knl.IThreadSupport.Stat", "ti.sysbios.xdcruntime"));
        spo = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.ThreadSupport$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.xdcruntime.ThreadSupport.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.ThreadSupport$$Object", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.ThreadSupport$$Params", new Proto.Obj());
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.xdcruntime.ThreadSupport.Object", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Instance_State", "ti.sysbios.xdcruntime"));
        }//isROV
    }

    void CacheSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.CacheSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.xdcruntime.CacheSupport", new Value.Obj("ti.sysbios.xdcruntime.CacheSupport", po));
        pkgV.bind("CacheSupport", vo);
        // decls 
    }

    void Settings$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.xdcruntime.Settings.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.xdcruntime.Settings", new Value.Obj("ti.sysbios.xdcruntime.Settings", po));
        pkgV.bind("Settings", vo);
        // decls 
    }

    void GateThreadSupport$$CONSTS()
    {
        // module GateThreadSupport
        om.bind("ti.sysbios.xdcruntime.GateThreadSupport.query", new Extern("ti_sysbios_xdcruntime_GateThreadSupport_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void GateProcessSupport$$CONSTS()
    {
        // module GateProcessSupport
        om.bind("ti.sysbios.xdcruntime.GateProcessSupport.query", new Extern("ti_sysbios_xdcruntime_GateProcessSupport_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void SemThreadSupport$$CONSTS()
    {
        // module SemThreadSupport
    }

    void SemProcessSupport$$CONSTS()
    {
        // module SemProcessSupport
    }

    void ThreadSupport$$CONSTS()
    {
        // module ThreadSupport
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.PRI_FAILURE", 1L);
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.TASK_FAILURE", 2L);
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.self", new Extern("ti_sysbios_xdcruntime_ThreadSupport_self__E", "xdc_runtime_knl_IThreadSupport_Handle(*)(xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.start", new Extern("ti_sysbios_xdcruntime_ThreadSupport_start__E", "xdc_Bool(*)(xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.yield", new Extern("ti_sysbios_xdcruntime_ThreadSupport_yield__E", "xdc_Bool(*)(xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.compareOsPriorities", new Extern("ti_sysbios_xdcruntime_ThreadSupport_compareOsPriorities__E", "xdc_Int(*)(xdc_Int,xdc_Int,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.sleep", new Extern("ti_sysbios_xdcruntime_ThreadSupport_sleep__E", "xdc_Bool(*)(xdc_UInt,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.xdcruntime.ThreadSupport.runStub", new Extern("ti_sysbios_xdcruntime_ThreadSupport_runStub__I", "xdc_Void(*)(xdc_UArg,xdc_UArg)", true, false));
    }

    void CacheSupport$$CONSTS()
    {
        // module CacheSupport
        om.bind("ti.sysbios.xdcruntime.CacheSupport.inv", new Extern("ti_sysbios_xdcruntime_CacheSupport_inv__E", "xdc_Bool(*)(xdc_Ptr,xdc_SizeT,xdc_Bool,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.xdcruntime.CacheSupport.wb", new Extern("ti_sysbios_xdcruntime_CacheSupport_wb__E", "xdc_Bool(*)(xdc_Ptr,xdc_SizeT,xdc_Bool,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.xdcruntime.CacheSupport.wbInv", new Extern("ti_sysbios_xdcruntime_CacheSupport_wbInv__E", "xdc_Bool(*)(xdc_Ptr,xdc_SizeT,xdc_Bool,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.xdcruntime.CacheSupport.wait", new Extern("ti_sysbios_xdcruntime_CacheSupport_wait__E", "xdc_Bool(*)(xdc_runtime_Error_Block*)", true, false));
    }

    void Settings$$CONSTS()
    {
        // module Settings
    }

    void GateThreadSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$GateThreadSupport$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.xdcruntime.GateThreadSupport.create() called before xdc.useModule('ti.sysbios.xdcruntime.GateThreadSupport')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$create", new Proto.Fxn(om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Module", "ti.sysbios.xdcruntime"), om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Instance", "ti.sysbios.xdcruntime"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Params", "ti.sysbios.xdcruntime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$GateThreadSupport$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.xdcruntime.GateThreadSupport'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.xdcruntime.GateThreadSupport.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.xdcruntime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.xdcruntime.GateThreadSupport'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.xdcruntime.GateThreadSupport'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Module", "ti.sysbios.xdcruntime"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport$$Object", "ti.sysbios.xdcruntime"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Params", "ti.sysbios.xdcruntime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$GateThreadSupport$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.xdcruntime.GateThreadSupport'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.xdcruntime.GateThreadSupport'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.xdcruntime.GateThreadSupport'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void GateProcessSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$GateProcessSupport$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.xdcruntime.GateProcessSupport.create() called before xdc.useModule('ti.sysbios.xdcruntime.GateProcessSupport')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$create", new Proto.Fxn(om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Module", "ti.sysbios.xdcruntime"), om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Instance", "ti.sysbios.xdcruntime"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Params", "ti.sysbios.xdcruntime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$GateProcessSupport$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.xdcruntime.GateProcessSupport'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.xdcruntime.GateProcessSupport.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.xdcruntime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.xdcruntime.GateProcessSupport'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.xdcruntime.GateProcessSupport'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Module", "ti.sysbios.xdcruntime"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport$$Object", "ti.sysbios.xdcruntime"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Params", "ti.sysbios.xdcruntime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$GateProcessSupport$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.xdcruntime.GateProcessSupport'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.xdcruntime.GateProcessSupport'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.xdcruntime.GateProcessSupport'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void SemThreadSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$SemThreadSupport$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.xdcruntime.SemThreadSupport.create() called before xdc.useModule('ti.sysbios.xdcruntime.SemThreadSupport')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.count, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$create", new Proto.Fxn(om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Module", "ti.sysbios.xdcruntime"), om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Instance", "ti.sysbios.xdcruntime"), 2, 1, false));
                        fxn.addArg(0, "count", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Params", "ti.sysbios.xdcruntime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$SemThreadSupport$$create = function( count, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.xdcruntime.SemThreadSupport'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.xdcruntime.SemThreadSupport.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.xdcruntime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {count:count});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.xdcruntime.SemThreadSupport'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [count]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.xdcruntime.SemThreadSupport'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Module", "ti.sysbios.xdcruntime"), null, 3, 1, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport$$Object", "ti.sysbios.xdcruntime"), null);
                        fxn.addArg(1, "count", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Params", "ti.sysbios.xdcruntime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$SemThreadSupport$$construct = function( __obj, count, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.xdcruntime.SemThreadSupport'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {count:count});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.xdcruntime.SemThreadSupport'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.xdcruntime.SemThreadSupport'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void SemProcessSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$SemProcessSupport$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.xdcruntime.SemProcessSupport.create() called before xdc.useModule('ti.sysbios.xdcruntime.SemProcessSupport')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.count, inst.$args.key, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$create", new Proto.Fxn(om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Module", "ti.sysbios.xdcruntime"), om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Instance", "ti.sysbios.xdcruntime"), 3, 2, false));
                        fxn.addArg(0, "count", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(1, "key", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Params", "ti.sysbios.xdcruntime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$SemProcessSupport$$create = function( count, key, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.xdcruntime.SemProcessSupport'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.xdcruntime.SemProcessSupport.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.xdcruntime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {count:count, key:key});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.xdcruntime.SemProcessSupport'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [count, key]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.xdcruntime.SemProcessSupport'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Module", "ti.sysbios.xdcruntime"), null, 4, 2, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport$$Object", "ti.sysbios.xdcruntime"), null);
                        fxn.addArg(1, "count", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(2, "key", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(3, "__params", (Proto)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Params", "ti.sysbios.xdcruntime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$SemProcessSupport$$construct = function( __obj, count, key, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.xdcruntime.SemProcessSupport'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {count:count, key:key});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.xdcruntime.SemProcessSupport'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.xdcruntime.SemProcessSupport'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void ThreadSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$ThreadSupport$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.xdcruntime.ThreadSupport.create() called before xdc.useModule('ti.sysbios.xdcruntime.ThreadSupport')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.fxn, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.xdcruntime.ThreadSupport$$create", new Proto.Fxn(om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Module", "ti.sysbios.xdcruntime"), om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Instance", "ti.sysbios.xdcruntime"), 2, 1, false));
                        fxn.addArg(0, "fxn", new Proto.Adr("xdc_Void(*)(xdc_IArg)", "PFv"), $$UNDEF);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Params", "ti.sysbios.xdcruntime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$ThreadSupport$$create = function( fxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.xdcruntime.ThreadSupport'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.xdcruntime.ThreadSupport.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.xdcruntime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {fxn:fxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.xdcruntime.ThreadSupport'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [fxn]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.xdcruntime.ThreadSupport'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.xdcruntime.ThreadSupport$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Module", "ti.sysbios.xdcruntime"), null, 3, 1, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport$$Object", "ti.sysbios.xdcruntime"), null);
                        fxn.addArg(1, "fxn", new Proto.Adr("xdc_Void(*)(xdc_IArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Params", "ti.sysbios.xdcruntime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$xdcruntime$ThreadSupport$$construct = function( __obj, fxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.xdcruntime.ThreadSupport'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {fxn:fxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.xdcruntime.ThreadSupport'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.xdcruntime.ThreadSupport'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void CacheSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Settings$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void GateThreadSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateProcessSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SemThreadSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SemProcessSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ThreadSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CacheSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Settings$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateThreadSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Instance_State", "ti.sysbios.xdcruntime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("gate", "Sti.sysbios.gates.GateMutexPri;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.xdcruntime.GateThreadSupport.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.xdcruntime.GateThreadSupport.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.xdcruntime.GateThreadSupport.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void GateProcessSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Instance_State", "ti.sysbios.xdcruntime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("gate", "Sti.sysbios.gates.GateMutexPri;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.xdcruntime.GateProcessSupport.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.xdcruntime.GateProcessSupport.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.xdcruntime.GateProcessSupport.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void SemThreadSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Instance_State", "ti.sysbios.xdcruntime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("sem", "Sti.sysbios.knl.Semaphore;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.xdcruntime.SemThreadSupport.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.xdcruntime.SemThreadSupport.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.xdcruntime.SemThreadSupport.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void SemProcessSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Instance_State", "ti.sysbios.xdcruntime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("sem", "Sti.sysbios.knl.Semaphore;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.xdcruntime.SemProcessSupport.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.xdcruntime.SemProcessSupport.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.xdcruntime.SemProcessSupport.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void ThreadSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Instance_State", "ti.sysbios.xdcruntime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("task", "UPtr"));
        sizes.add(Global.newArray("tls", "UPtr"));
        sizes.add(Global.newArray("startFxn", "UFxn"));
        sizes.add(Global.newArray("startFxnArg", "TIArg"));
        sizes.add(Global.newArray("join_sem", "Sti.sysbios.knl.Semaphore;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.xdcruntime.ThreadSupport.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.xdcruntime.ThreadSupport.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.xdcruntime.ThreadSupport.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void CacheSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Settings$$SIZES()
    {
    }

    void GateThreadSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/xdcruntime/GateThreadSupport.xs");
        om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Module", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.GateThreadSupport.Module", om.findStrict("xdc.runtime.knl.IGateThreadSupport.Module", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport$$create", "ti.sysbios.xdcruntime"), Global.get("ti$sysbios$xdcruntime$GateThreadSupport$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport$$construct", "ti.sysbios.xdcruntime"), Global.get("ti$sysbios$xdcruntime$GateThreadSupport$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateThreadSupport$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.xdcruntime"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Instance", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.GateThreadSupport.Instance", om.findStrict("xdc.runtime.knl.IGateThreadSupport.Instance", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport$$Params", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.GateThreadSupport.Params", om.findStrict("xdc.runtime.knl.IGateThreadSupport$$Params", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport$$Object", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.GateThreadSupport.Object", om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Instance", "ti.sysbios.xdcruntime"));
        // struct GateThreadSupport.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport$$Instance_State", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.GateThreadSupport.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFldV("gate", (Proto)om.findStrict("ti.sysbios.gates.GateMutexPri.Object", "ti.sysbios.xdcruntime"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_gate", (Proto)om.findStrict("ti.sysbios.gates.GateMutexPri.Object", "ti.sysbios.xdcruntime"), $$DEFAULT, "w");
    }

    void GateProcessSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/xdcruntime/GateProcessSupport.xs");
        om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Module", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.GateProcessSupport.Module", om.findStrict("xdc.runtime.knl.IGateProcessSupport.Module", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
                po.addFld("GETREFCOUNT_FAILED", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-1"), "rh");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport$$create", "ti.sysbios.xdcruntime"), Global.get("ti$sysbios$xdcruntime$GateProcessSupport$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport$$construct", "ti.sysbios.xdcruntime"), Global.get("ti$sysbios$xdcruntime$GateProcessSupport$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.GateProcessSupport$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.xdcruntime"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Instance", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.GateProcessSupport.Instance", om.findStrict("xdc.runtime.knl.IGateProcessSupport.Instance", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
                po.addFld("GETREFCOUNT_FAILED", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-1"), "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport$$Params", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.GateProcessSupport.Params", om.findStrict("xdc.runtime.knl.IGateProcessSupport$$Params", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
                po.addFld("GETREFCOUNT_FAILED", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-1"), "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport$$Object", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.GateProcessSupport.Object", om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Instance", "ti.sysbios.xdcruntime"));
        // struct GateProcessSupport.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport$$Instance_State", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.GateProcessSupport.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFldV("gate", (Proto)om.findStrict("ti.sysbios.gates.GateMutexPri.Object", "ti.sysbios.xdcruntime"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_gate", (Proto)om.findStrict("ti.sysbios.gates.GateMutexPri.Object", "ti.sysbios.xdcruntime"), $$DEFAULT, "w");
    }

    void SemThreadSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/xdcruntime/SemThreadSupport.xs");
        om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Module", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemThreadSupport.Module", om.findStrict("xdc.runtime.knl.ISemThreadSupport.Module", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.xdcruntime"), $$UNDEF, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport$$create", "ti.sysbios.xdcruntime"), Global.get("ti$sysbios$xdcruntime$SemThreadSupport$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport$$construct", "ti.sysbios.xdcruntime"), Global.get("ti$sysbios$xdcruntime$SemThreadSupport$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemThreadSupport$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Instance", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemThreadSupport.Instance", om.findStrict("xdc.runtime.knl.ISemThreadSupport.Instance", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport$$Params", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemThreadSupport.Params", om.findStrict("xdc.runtime.knl.ISemThreadSupport$$Params", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport$$Object", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemThreadSupport.Object", om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Instance", "ti.sysbios.xdcruntime"));
        // struct SemThreadSupport.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport$$BasicView", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemThreadSupport.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("SemaphoreHandle", $$T_Str, $$UNDEF, "w");
                po.addFld("pendedTasks", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
        // struct SemThreadSupport.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport$$Instance_State", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemThreadSupport.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFldV("sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.xdcruntime"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.xdcruntime"), $$DEFAULT, "w");
    }

    void SemProcessSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/xdcruntime/SemProcessSupport.xs");
        om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Module", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemProcessSupport.Module", om.findStrict("xdc.runtime.knl.ISemProcessSupport.Module", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport$$create", "ti.sysbios.xdcruntime"), Global.get("ti$sysbios$xdcruntime$SemProcessSupport$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport$$construct", "ti.sysbios.xdcruntime"), Global.get("ti$sysbios$xdcruntime$SemProcessSupport$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.SemProcessSupport$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Instance", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemProcessSupport.Instance", om.findStrict("xdc.runtime.knl.ISemProcessSupport.Instance", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport$$Params", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemProcessSupport.Params", om.findStrict("xdc.runtime.knl.ISemProcessSupport$$Params", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport$$Object", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemProcessSupport.Object", om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Instance", "ti.sysbios.xdcruntime"));
        // struct SemProcessSupport.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport$$Instance_State", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.SemProcessSupport.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFldV("sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.xdcruntime"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.xdcruntime"), $$DEFAULT, "w");
    }

    void ThreadSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/xdcruntime/ThreadSupport.xs");
        om.bind("ti.sysbios.xdcruntime.ThreadSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Module", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.ThreadSupport.Module", om.findStrict("xdc.runtime.knl.IThreadSupport.Module", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("INVALID_OS_PRIORITY", Proto.Elm.newCNum("(xdc_Int)"), 0L, "rh");
                po.addFld("GETPRI_FAILED", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-2"), "rh");
                po.addFld("PRI_FAILURE", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("TASK_FAILURE", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
            po.addFld("E_priority", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.xdcruntime"), Global.newObject("msg", "E_priority: Thread priority is invalid %d"), "w");
            po.addFld("L_start", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.xdcruntime"), Global.newObject("mask", 0x0004L, "msg", "<-- start: (%p)"), "w");
            po.addFld("L_finish", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.xdcruntime"), Global.newObject("mask", 0x0004L, "msg", "--> finish: (%p)"), "w");
            po.addFld("L_join", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.xdcruntime"), Global.newObject("mask", 0x0004L, "msg", "--> join: (%p)"), "w");
            po.addFld("lowestPriority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
            po.addFld("belowNormalPriority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
            po.addFld("normalPriority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
            po.addFld("aboveNormalPriority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
            po.addFld("highestPriority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport$$create", "ti.sysbios.xdcruntime"), Global.get("ti$sysbios$xdcruntime$ThreadSupport$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport$$construct", "ti.sysbios.xdcruntime"), Global.get("ti$sysbios$xdcruntime$ThreadSupport$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.ThreadSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.ThreadSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.ThreadSupport$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.ThreadSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.ThreadSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.ThreadSupport$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Instance", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.ThreadSupport.Instance", om.findStrict("xdc.runtime.knl.IThreadSupport.Instance", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("INVALID_OS_PRIORITY", Proto.Elm.newCNum("(xdc_Int)"), 0L, "rh");
                po.addFld("GETPRI_FAILED", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-2"), "rh");
                po.addFld("PRI_FAILURE", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("TASK_FAILURE", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport$$Params", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.ThreadSupport.Params", om.findStrict("xdc.runtime.knl.IThreadSupport$$Params", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("INVALID_OS_PRIORITY", Proto.Elm.newCNum("(xdc_Int)"), 0L, "rh");
                po.addFld("GETPRI_FAILED", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-2"), "rh");
                po.addFld("PRI_FAILURE", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("TASK_FAILURE", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport$$Object", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.ThreadSupport.Object", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Instance", "ti.sysbios.xdcruntime"));
        // struct ThreadSupport.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport$$Instance_State", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.ThreadSupport.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("task", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.xdcruntime"), $$UNDEF, "w");
                po.addFld("tls", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("startFxn", new Proto.Adr("xdc_Void(*)(xdc_IArg)", "PFv"), $$UNDEF, "w");
                po.addFld("startFxnArg", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFldV("join_sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.xdcruntime"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_join_sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.xdcruntime"), $$DEFAULT, "w");
    }

    void CacheSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/xdcruntime/CacheSupport.xs");
        om.bind("ti.sysbios.xdcruntime.CacheSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.CacheSupport.Module", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.CacheSupport.Module", om.findStrict("xdc.runtime.knl.ICacheSupport.Module", "ti.sysbios.xdcruntime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.CacheSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.CacheSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.CacheSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.CacheSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void Settings$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/xdcruntime/Settings.xs");
        om.bind("ti.sysbios.xdcruntime.Settings$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.Settings.Module", "ti.sysbios.xdcruntime");
        po.init("ti.sysbios.xdcruntime.Settings.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.Settings$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.Settings$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.xdcruntime.Settings$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void GateThreadSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport$$Instance_State", "ti.sysbios.xdcruntime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.xdcruntime.GateThreadSupport.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport$$Instance_State", "ti.sysbios.xdcruntime");
    }

    void GateProcessSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport$$Instance_State", "ti.sysbios.xdcruntime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.xdcruntime.GateProcessSupport.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport$$Instance_State", "ti.sysbios.xdcruntime");
    }

    void SemThreadSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport$$Instance_State", "ti.sysbios.xdcruntime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.xdcruntime.SemThreadSupport.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport$$Instance_State", "ti.sysbios.xdcruntime");
    }

    void SemProcessSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport$$Instance_State", "ti.sysbios.xdcruntime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.xdcruntime.SemProcessSupport.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport$$Instance_State", "ti.sysbios.xdcruntime");
    }

    void ThreadSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport$$Instance_State", "ti.sysbios.xdcruntime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.xdcruntime.ThreadSupport.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport$$Instance_State", "ti.sysbios.xdcruntime");
    }

    void CacheSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.CacheSupport", "ti.sysbios.xdcruntime");
    }

    void Settings$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.xdcruntime.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.xdcruntime"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/xdcruntime/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.xdcruntime"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.xdcruntime"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.xdcruntime"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.xdcruntime"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.xdcruntime"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.xdcruntime"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.xdcruntime", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.xdcruntime");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.xdcruntime.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.runtime.knl", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.xdcruntime'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.xdcruntime$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.xdcruntime$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.xdcruntime$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.xdcruntime.aem4',\n");
            sb.append("'lib/debug/ti.sysbios.xdcruntime.am4',\n");
            sb.append("'lib/debug/ti.sysbios.xdcruntime.am4g',\n");
            sb.append("'lib/debug/ti.sysbios.xdcruntime.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.xdcruntime.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/debug/ti.sysbios.xdcruntime.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/debug/ti.sysbios.xdcruntime.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/debug/ti.sysbios.xdcruntime.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void GateThreadSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Module", "ti.sysbios.xdcruntime");
        vo.init2(po, "ti.sysbios.xdcruntime.GateThreadSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport$$capsule", "ti.sysbios.xdcruntime"));
        vo.bind("Instance", om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Instance", "ti.sysbios.xdcruntime"));
        vo.bind("Params", om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Params", "ti.sysbios.xdcruntime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Params", "ti.sysbios.xdcruntime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Handle", "ti.sysbios.xdcruntime"));
        vo.bind("$package", om.findStrict("ti.sysbios.xdcruntime", "ti.sysbios.xdcruntime"));
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
        vo.bind("Instance_State", om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Instance_State", "ti.sysbios.xdcruntime"));
        tdefs.add(om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Instance_State", "ti.sysbios.xdcruntime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.xdcruntime")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.xdcruntime.GateThreadSupport$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$xdcruntime$GateThreadSupport$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.query", "ti.sysbios.xdcruntime"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_xdcruntime_GateThreadSupport_Handle__label__E", "ti_sysbios_xdcruntime_GateThreadSupport_Module__startupDone__E", "ti_sysbios_xdcruntime_GateThreadSupport_Object__create__E", "ti_sysbios_xdcruntime_GateThreadSupport_Object__delete__E", "ti_sysbios_xdcruntime_GateThreadSupport_Object__get__E", "ti_sysbios_xdcruntime_GateThreadSupport_Object__first__E", "ti_sysbios_xdcruntime_GateThreadSupport_Object__next__E", "ti_sysbios_xdcruntime_GateThreadSupport_Params__init__E", "ti_sysbios_xdcruntime_GateThreadSupport_query__E", "ti_sysbios_xdcruntime_GateThreadSupport_enter__E", "ti_sysbios_xdcruntime_GateThreadSupport_leave__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport.Object", "ti.sysbios.xdcruntime"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateThreadSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateThreadSupport");
    }

    void GateProcessSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Module", "ti.sysbios.xdcruntime");
        vo.init2(po, "ti.sysbios.xdcruntime.GateProcessSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport$$capsule", "ti.sysbios.xdcruntime"));
        vo.bind("Instance", om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Instance", "ti.sysbios.xdcruntime"));
        vo.bind("Params", om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Params", "ti.sysbios.xdcruntime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Params", "ti.sysbios.xdcruntime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Handle", "ti.sysbios.xdcruntime"));
        vo.bind("$package", om.findStrict("ti.sysbios.xdcruntime", "ti.sysbios.xdcruntime"));
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
        vo.bind("Instance_State", om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Instance_State", "ti.sysbios.xdcruntime"));
        tdefs.add(om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Instance_State", "ti.sysbios.xdcruntime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.xdcruntime")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.xdcruntime.GateProcessSupport$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$xdcruntime$GateProcessSupport$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.query", "ti.sysbios.xdcruntime"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_xdcruntime_GateProcessSupport_Handle__label__E", "ti_sysbios_xdcruntime_GateProcessSupport_Module__startupDone__E", "ti_sysbios_xdcruntime_GateProcessSupport_Object__create__E", "ti_sysbios_xdcruntime_GateProcessSupport_Object__delete__E", "ti_sysbios_xdcruntime_GateProcessSupport_Object__get__E", "ti_sysbios_xdcruntime_GateProcessSupport_Object__first__E", "ti_sysbios_xdcruntime_GateProcessSupport_Object__next__E", "ti_sysbios_xdcruntime_GateProcessSupport_Params__init__E", "ti_sysbios_xdcruntime_GateProcessSupport_query__E", "ti_sysbios_xdcruntime_GateProcessSupport_enter__E", "ti_sysbios_xdcruntime_GateProcessSupport_leave__E", "ti_sysbios_xdcruntime_GateProcessSupport_getReferenceCount__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport.Object", "ti.sysbios.xdcruntime"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateProcessSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateProcessSupport");
    }

    void SemThreadSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Module", "ti.sysbios.xdcruntime");
        vo.init2(po, "ti.sysbios.xdcruntime.SemThreadSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport$$capsule", "ti.sysbios.xdcruntime"));
        vo.bind("Instance", om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Instance", "ti.sysbios.xdcruntime"));
        vo.bind("Params", om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Params", "ti.sysbios.xdcruntime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Params", "ti.sysbios.xdcruntime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Handle", "ti.sysbios.xdcruntime"));
        vo.bind("$package", om.findStrict("ti.sysbios.xdcruntime", "ti.sysbios.xdcruntime"));
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
        vo.bind("PendStatus", om.findStrict("xdc.runtime.knl.ISemaphore.PendStatus", "ti.sysbios.xdcruntime"));
        vo.bind("Mode", om.findStrict("xdc.runtime.knl.ISemaphore.Mode", "ti.sysbios.xdcruntime"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.BasicView", "ti.sysbios.xdcruntime"));
        tdefs.add(om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.BasicView", "ti.sysbios.xdcruntime"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Instance_State", "ti.sysbios.xdcruntime"));
        tdefs.add(om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Instance_State", "ti.sysbios.xdcruntime"));
        vo.bind("PendStatus_ERROR", om.findStrict("xdc.runtime.knl.ISemaphore.PendStatus_ERROR", "ti.sysbios.xdcruntime"));
        vo.bind("PendStatus_TIMEOUT", om.findStrict("xdc.runtime.knl.ISemaphore.PendStatus_TIMEOUT", "ti.sysbios.xdcruntime"));
        vo.bind("PendStatus_SUCCESS", om.findStrict("xdc.runtime.knl.ISemaphore.PendStatus_SUCCESS", "ti.sysbios.xdcruntime"));
        vo.bind("Mode_COUNTING", om.findStrict("xdc.runtime.knl.ISemaphore.Mode_COUNTING", "ti.sysbios.xdcruntime"));
        vo.bind("Mode_BINARY", om.findStrict("xdc.runtime.knl.ISemaphore.Mode_BINARY", "ti.sysbios.xdcruntime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.xdcruntime")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.xdcruntime.SemThreadSupport$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$xdcruntime$SemThreadSupport$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_xdcruntime_SemThreadSupport_Handle__label__E", "ti_sysbios_xdcruntime_SemThreadSupport_Module__startupDone__E", "ti_sysbios_xdcruntime_SemThreadSupport_Object__create__E", "ti_sysbios_xdcruntime_SemThreadSupport_Object__delete__E", "ti_sysbios_xdcruntime_SemThreadSupport_Object__get__E", "ti_sysbios_xdcruntime_SemThreadSupport_Object__first__E", "ti_sysbios_xdcruntime_SemThreadSupport_Object__next__E", "ti_sysbios_xdcruntime_SemThreadSupport_Params__init__E", "ti_sysbios_xdcruntime_SemThreadSupport_pend__E", "ti_sysbios_xdcruntime_SemThreadSupport_post__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport.Object", "ti.sysbios.xdcruntime"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("SemThreadSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SemThreadSupport");
    }

    void SemProcessSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Module", "ti.sysbios.xdcruntime");
        vo.init2(po, "ti.sysbios.xdcruntime.SemProcessSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport$$capsule", "ti.sysbios.xdcruntime"));
        vo.bind("Instance", om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Instance", "ti.sysbios.xdcruntime"));
        vo.bind("Params", om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Params", "ti.sysbios.xdcruntime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Params", "ti.sysbios.xdcruntime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Handle", "ti.sysbios.xdcruntime"));
        vo.bind("$package", om.findStrict("ti.sysbios.xdcruntime", "ti.sysbios.xdcruntime"));
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
        vo.bind("PendStatus", om.findStrict("xdc.runtime.knl.ISemaphore.PendStatus", "ti.sysbios.xdcruntime"));
        vo.bind("Mode", om.findStrict("xdc.runtime.knl.ISemaphore.Mode", "ti.sysbios.xdcruntime"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Instance_State", "ti.sysbios.xdcruntime"));
        tdefs.add(om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Instance_State", "ti.sysbios.xdcruntime"));
        vo.bind("PendStatus_ERROR", om.findStrict("xdc.runtime.knl.ISemaphore.PendStatus_ERROR", "ti.sysbios.xdcruntime"));
        vo.bind("PendStatus_TIMEOUT", om.findStrict("xdc.runtime.knl.ISemaphore.PendStatus_TIMEOUT", "ti.sysbios.xdcruntime"));
        vo.bind("PendStatus_SUCCESS", om.findStrict("xdc.runtime.knl.ISemaphore.PendStatus_SUCCESS", "ti.sysbios.xdcruntime"));
        vo.bind("Mode_COUNTING", om.findStrict("xdc.runtime.knl.ISemaphore.Mode_COUNTING", "ti.sysbios.xdcruntime"));
        vo.bind("Mode_BINARY", om.findStrict("xdc.runtime.knl.ISemaphore.Mode_BINARY", "ti.sysbios.xdcruntime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.xdcruntime")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.xdcruntime.SemProcessSupport$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$xdcruntime$SemProcessSupport$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_xdcruntime_SemProcessSupport_Handle__label__E", "ti_sysbios_xdcruntime_SemProcessSupport_Module__startupDone__E", "ti_sysbios_xdcruntime_SemProcessSupport_Object__create__E", "ti_sysbios_xdcruntime_SemProcessSupport_Object__delete__E", "ti_sysbios_xdcruntime_SemProcessSupport_Object__get__E", "ti_sysbios_xdcruntime_SemProcessSupport_Object__first__E", "ti_sysbios_xdcruntime_SemProcessSupport_Object__next__E", "ti_sysbios_xdcruntime_SemProcessSupport_Params__init__E", "ti_sysbios_xdcruntime_SemProcessSupport_pend__E", "ti_sysbios_xdcruntime_SemProcessSupport_post__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport.Object", "ti.sysbios.xdcruntime"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("SemProcessSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SemProcessSupport");
    }

    void ThreadSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Module", "ti.sysbios.xdcruntime");
        vo.init2(po, "ti.sysbios.xdcruntime.ThreadSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport$$capsule", "ti.sysbios.xdcruntime"));
        vo.bind("Instance", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Instance", "ti.sysbios.xdcruntime"));
        vo.bind("Params", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Params", "ti.sysbios.xdcruntime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Params", "ti.sysbios.xdcruntime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Handle", "ti.sysbios.xdcruntime"));
        vo.bind("$package", om.findStrict("ti.sysbios.xdcruntime", "ti.sysbios.xdcruntime"));
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
        vo.bind("Priority", om.findStrict("xdc.runtime.knl.IThreadSupport.Priority", "ti.sysbios.xdcruntime"));
        vo.bind("CompStatus", om.findStrict("xdc.runtime.knl.IThreadSupport.CompStatus", "ti.sysbios.xdcruntime"));
        vo.bind("RunFxn", om.findStrict("xdc.runtime.knl.IThreadSupport.RunFxn", "ti.sysbios.xdcruntime"));
        vo.bind("Stat", om.findStrict("xdc.runtime.knl.IThreadSupport.Stat", "ti.sysbios.xdcruntime"));
        tdefs.add(om.findStrict("xdc.runtime.knl.IThreadSupport.Stat", "ti.sysbios.xdcruntime"));
        mcfgs.add("E_priority");
        mcfgs.add("L_start");
        mcfgs.add("L_finish");
        mcfgs.add("L_join");
        mcfgs.add("lowestPriority");
        icfgs.add("lowestPriority");
        mcfgs.add("belowNormalPriority");
        icfgs.add("belowNormalPriority");
        mcfgs.add("normalPriority");
        icfgs.add("normalPriority");
        mcfgs.add("aboveNormalPriority");
        icfgs.add("aboveNormalPriority");
        mcfgs.add("highestPriority");
        icfgs.add("highestPriority");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Instance_State", "ti.sysbios.xdcruntime"));
        tdefs.add(om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Instance_State", "ti.sysbios.xdcruntime"));
        vo.bind("Priority_INVALID", om.findStrict("xdc.runtime.knl.IThreadSupport.Priority_INVALID", "ti.sysbios.xdcruntime"));
        vo.bind("Priority_LOWEST", om.findStrict("xdc.runtime.knl.IThreadSupport.Priority_LOWEST", "ti.sysbios.xdcruntime"));
        vo.bind("Priority_BELOW_NORMAL", om.findStrict("xdc.runtime.knl.IThreadSupport.Priority_BELOW_NORMAL", "ti.sysbios.xdcruntime"));
        vo.bind("Priority_NORMAL", om.findStrict("xdc.runtime.knl.IThreadSupport.Priority_NORMAL", "ti.sysbios.xdcruntime"));
        vo.bind("Priority_ABOVE_NORMAL", om.findStrict("xdc.runtime.knl.IThreadSupport.Priority_ABOVE_NORMAL", "ti.sysbios.xdcruntime"));
        vo.bind("Priority_HIGHEST", om.findStrict("xdc.runtime.knl.IThreadSupport.Priority_HIGHEST", "ti.sysbios.xdcruntime"));
        vo.bind("CompStatus_ERROR", om.findStrict("xdc.runtime.knl.IThreadSupport.CompStatus_ERROR", "ti.sysbios.xdcruntime"));
        vo.bind("CompStatus_LOWER", om.findStrict("xdc.runtime.knl.IThreadSupport.CompStatus_LOWER", "ti.sysbios.xdcruntime"));
        vo.bind("CompStatus_EQUAL", om.findStrict("xdc.runtime.knl.IThreadSupport.CompStatus_EQUAL", "ti.sysbios.xdcruntime"));
        vo.bind("CompStatus_HIGHER", om.findStrict("xdc.runtime.knl.IThreadSupport.CompStatus_HIGHER", "ti.sysbios.xdcruntime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.xdcruntime")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.xdcruntime.ThreadSupport$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$xdcruntime$ThreadSupport$$__initObject"));
        }//isCFG
        vo.bind("self", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.self", "ti.sysbios.xdcruntime"));
        vo.bind("start", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.start", "ti.sysbios.xdcruntime"));
        vo.bind("yield", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.yield", "ti.sysbios.xdcruntime"));
        vo.bind("compareOsPriorities", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.compareOsPriorities", "ti.sysbios.xdcruntime"));
        vo.bind("sleep", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.sleep", "ti.sysbios.xdcruntime"));
        vo.bind("runStub", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.runStub", "ti.sysbios.xdcruntime"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_xdcruntime_ThreadSupport_Handle__label__E", "ti_sysbios_xdcruntime_ThreadSupport_Module__startupDone__E", "ti_sysbios_xdcruntime_ThreadSupport_Object__create__E", "ti_sysbios_xdcruntime_ThreadSupport_Object__delete__E", "ti_sysbios_xdcruntime_ThreadSupport_Object__get__E", "ti_sysbios_xdcruntime_ThreadSupport_Object__first__E", "ti_sysbios_xdcruntime_ThreadSupport_Object__next__E", "ti_sysbios_xdcruntime_ThreadSupport_Params__init__E", "ti_sysbios_xdcruntime_ThreadSupport_self__E", "ti_sysbios_xdcruntime_ThreadSupport_start__E", "ti_sysbios_xdcruntime_ThreadSupport_yield__E", "ti_sysbios_xdcruntime_ThreadSupport_compareOsPriorities__E", "ti_sysbios_xdcruntime_ThreadSupport_sleep__E", "ti_sysbios_xdcruntime_ThreadSupport_join__E", "ti_sysbios_xdcruntime_ThreadSupport_getPriority__E", "ti_sysbios_xdcruntime_ThreadSupport_setPriority__E", "ti_sysbios_xdcruntime_ThreadSupport_getOsPriority__E", "ti_sysbios_xdcruntime_ThreadSupport_setOsPriority__E", "ti_sysbios_xdcruntime_ThreadSupport_getOsHandle__E", "ti_sysbios_xdcruntime_ThreadSupport_getTls__E", "ti_sysbios_xdcruntime_ThreadSupport_setTls__E", "ti_sysbios_xdcruntime_ThreadSupport_stat__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("L_start", "L_finish", "L_join"));
        vo.bind("$$errorDescCfgs", Global.newArray("E_priority"));
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.xdcruntime.ThreadSupport.Object", "ti.sysbios.xdcruntime"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("ThreadSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ThreadSupport");
    }

    void CacheSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.CacheSupport", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.CacheSupport.Module", "ti.sysbios.xdcruntime");
        vo.init2(po, "ti.sysbios.xdcruntime.CacheSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.xdcruntime.CacheSupport$$capsule", "ti.sysbios.xdcruntime"));
        vo.bind("$package", om.findStrict("ti.sysbios.xdcruntime", "ti.sysbios.xdcruntime"));
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
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.xdcruntime")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("inv", om.findStrict("ti.sysbios.xdcruntime.CacheSupport.inv", "ti.sysbios.xdcruntime"));
        vo.bind("wb", om.findStrict("ti.sysbios.xdcruntime.CacheSupport.wb", "ti.sysbios.xdcruntime"));
        vo.bind("wbInv", om.findStrict("ti.sysbios.xdcruntime.CacheSupport.wbInv", "ti.sysbios.xdcruntime"));
        vo.bind("wait", om.findStrict("ti.sysbios.xdcruntime.CacheSupport.wait", "ti.sysbios.xdcruntime"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_xdcruntime_CacheSupport_Module__startupDone__E", "ti_sysbios_xdcruntime_CacheSupport_inv__E", "ti_sysbios_xdcruntime_CacheSupport_wb__E", "ti_sysbios_xdcruntime_CacheSupport_wbInv__E", "ti_sysbios_xdcruntime_CacheSupport_wait__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("CacheSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CacheSupport");
    }

    void Settings$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.Settings", "ti.sysbios.xdcruntime");
        po = (Proto.Obj)om.findStrict("ti.sysbios.xdcruntime.Settings.Module", "ti.sysbios.xdcruntime");
        vo.init2(po, "ti.sysbios.xdcruntime.Settings", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.xdcruntime.Settings$$capsule", "ti.sysbios.xdcruntime"));
        vo.bind("$package", om.findStrict("ti.sysbios.xdcruntime", "ti.sysbios.xdcruntime"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.xdcruntime")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.sysbios.xdcruntime.Settings$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("Settings", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Settings");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.sysbios.xdcruntime")).findStrict("PARAMS", "ti.sysbios.xdcruntime");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport", "ti.sysbios.xdcruntime")).findStrict("PARAMS", "ti.sysbios.xdcruntime");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport", "ti.sysbios.xdcruntime")).findStrict("PARAMS", "ti.sysbios.xdcruntime");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport", "ti.sysbios.xdcruntime")).findStrict("PARAMS", "ti.sysbios.xdcruntime");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport", "ti.sysbios.xdcruntime")).findStrict("PARAMS", "ti.sysbios.xdcruntime");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport", "ti.sysbios.xdcruntime")).findStrict("PARAMS", "ti.sysbios.xdcruntime");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.xdcruntime.GateThreadSupport", "ti.sysbios.xdcruntime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.xdcruntime.GateProcessSupport", "ti.sysbios.xdcruntime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport", "ti.sysbios.xdcruntime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.xdcruntime.SemProcessSupport", "ti.sysbios.xdcruntime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.xdcruntime.ThreadSupport", "ti.sysbios.xdcruntime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.xdcruntime.CacheSupport", "ti.sysbios.xdcruntime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.xdcruntime.Settings", "ti.sysbios.xdcruntime"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.xdcruntime.SemThreadSupport", "ti.sysbios.xdcruntime");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.xdcruntime.GateThreadSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.xdcruntime.GateProcessSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.xdcruntime.SemThreadSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.xdcruntime.SemProcessSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.xdcruntime.ThreadSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.xdcruntime.CacheSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.xdcruntime.Settings")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.xdcruntime")).add(pkgV);
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
        GateThreadSupport$$OBJECTS();
        GateProcessSupport$$OBJECTS();
        SemThreadSupport$$OBJECTS();
        SemProcessSupport$$OBJECTS();
        ThreadSupport$$OBJECTS();
        CacheSupport$$OBJECTS();
        Settings$$OBJECTS();
        GateThreadSupport$$CONSTS();
        GateProcessSupport$$CONSTS();
        SemThreadSupport$$CONSTS();
        SemProcessSupport$$CONSTS();
        ThreadSupport$$CONSTS();
        CacheSupport$$CONSTS();
        Settings$$CONSTS();
        GateThreadSupport$$CREATES();
        GateProcessSupport$$CREATES();
        SemThreadSupport$$CREATES();
        SemProcessSupport$$CREATES();
        ThreadSupport$$CREATES();
        CacheSupport$$CREATES();
        Settings$$CREATES();
        GateThreadSupport$$FUNCTIONS();
        GateProcessSupport$$FUNCTIONS();
        SemThreadSupport$$FUNCTIONS();
        SemProcessSupport$$FUNCTIONS();
        ThreadSupport$$FUNCTIONS();
        CacheSupport$$FUNCTIONS();
        Settings$$FUNCTIONS();
        GateThreadSupport$$SIZES();
        GateProcessSupport$$SIZES();
        SemThreadSupport$$SIZES();
        SemProcessSupport$$SIZES();
        ThreadSupport$$SIZES();
        CacheSupport$$SIZES();
        Settings$$SIZES();
        GateThreadSupport$$TYPES();
        GateProcessSupport$$TYPES();
        SemThreadSupport$$TYPES();
        SemProcessSupport$$TYPES();
        ThreadSupport$$TYPES();
        CacheSupport$$TYPES();
        Settings$$TYPES();
        if (isROV) {
            GateThreadSupport$$ROV();
            GateProcessSupport$$ROV();
            SemThreadSupport$$ROV();
            SemProcessSupport$$ROV();
            ThreadSupport$$ROV();
            CacheSupport$$ROV();
            Settings$$ROV();
        }//isROV
        $$SINGLETONS();
        GateThreadSupport$$SINGLETONS();
        GateProcessSupport$$SINGLETONS();
        SemThreadSupport$$SINGLETONS();
        SemProcessSupport$$SINGLETONS();
        ThreadSupport$$SINGLETONS();
        CacheSupport$$SINGLETONS();
        Settings$$SINGLETONS();
        $$INITIALIZATION();
    }
}
