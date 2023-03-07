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

public class ti_sysbios_syncs
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
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime.knl");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.syncs.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.syncs", new Value.Obj("ti.sysbios.syncs", pkgP));
    }

    void SyncSem$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncSem.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.syncs.SyncSem", new Value.Obj("ti.sysbios.syncs.SyncSem", po));
        pkgV.bind("SyncSem", vo);
        // decls 
        om.bind("ti.sysbios.syncs.SyncSem.WaitStatus", om.findStrict("xdc.runtime.knl.ISync.WaitStatus", "ti.sysbios.syncs"));
        spo = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncSem$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncSem.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncSem$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncSem.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.syncs.SyncSem.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncSem$$Object", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncSem.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncSem$$Params", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncSem.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.syncs.SyncSem.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.syncs.SyncSem.Object", om.findStrict("ti.sysbios.syncs.SyncSem.Instance_State", "ti.sysbios.syncs"));
        }//isROV
    }

    void SyncSwi$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncSwi.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.syncs.SyncSwi", new Value.Obj("ti.sysbios.syncs.SyncSwi", po));
        pkgV.bind("SyncSwi", vo);
        // decls 
        om.bind("ti.sysbios.syncs.SyncSwi.WaitStatus", om.findStrict("xdc.runtime.knl.ISync.WaitStatus", "ti.sysbios.syncs"));
        spo = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncSwi$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncSwi.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncSwi$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncSwi.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.syncs.SyncSwi.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncSwi$$Object", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncSwi.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncSwi$$Params", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncSwi.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.syncs.SyncSwi.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.syncs.SyncSwi.Object", om.findStrict("ti.sysbios.syncs.SyncSwi.Instance_State", "ti.sysbios.syncs"));
        }//isROV
    }

    void SyncEvent$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncEvent.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.syncs.SyncEvent", new Value.Obj("ti.sysbios.syncs.SyncEvent", po));
        pkgV.bind("SyncEvent", vo);
        // decls 
        om.bind("ti.sysbios.syncs.SyncEvent.WaitStatus", om.findStrict("xdc.runtime.knl.ISync.WaitStatus", "ti.sysbios.syncs"));
        spo = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncEvent$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncEvent.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncEvent$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncEvent.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.syncs.SyncEvent.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncEvent$$Object", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncEvent.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.syncs.SyncEvent$$Params", new Proto.Obj());
        om.bind("ti.sysbios.syncs.SyncEvent.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.syncs.SyncEvent.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.syncs.SyncEvent.Object", om.findStrict("ti.sysbios.syncs.SyncEvent.Instance_State", "ti.sysbios.syncs"));
        }//isROV
    }

    void SyncSem$$CONSTS()
    {
        // module SyncSem
    }

    void SyncSwi$$CONSTS()
    {
        // module SyncSwi
    }

    void SyncEvent$$CONSTS()
    {
        // module SyncEvent
    }

    void SyncSem$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$syncs$SyncSem$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.syncs.SyncSem.create() called before xdc.useModule('ti.sysbios.syncs.SyncSem')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.syncs.SyncSem$$create", new Proto.Fxn(om.findStrict("ti.sysbios.syncs.SyncSem.Module", "ti.sysbios.syncs"), om.findStrict("ti.sysbios.syncs.SyncSem.Instance", "ti.sysbios.syncs"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.syncs.SyncSem.Params", "ti.sysbios.syncs"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$syncs$SyncSem$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.syncs.SyncSem'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.syncs.SyncSem.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.syncs']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.syncs.SyncSem'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.syncs.SyncSem'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.syncs.SyncSem$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.syncs.SyncSem.Module", "ti.sysbios.syncs"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.syncs.SyncSem$$Object", "ti.sysbios.syncs"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.syncs.SyncSem.Params", "ti.sysbios.syncs"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$syncs$SyncSem$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.syncs.SyncSem'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.syncs.SyncSem'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.syncs.SyncSem'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void SyncSwi$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$syncs$SyncSwi$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.syncs.SyncSwi.create() called before xdc.useModule('ti.sysbios.syncs.SyncSwi')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.syncs.SyncSwi$$create", new Proto.Fxn(om.findStrict("ti.sysbios.syncs.SyncSwi.Module", "ti.sysbios.syncs"), om.findStrict("ti.sysbios.syncs.SyncSwi.Instance", "ti.sysbios.syncs"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.syncs.SyncSwi.Params", "ti.sysbios.syncs"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$syncs$SyncSwi$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.syncs.SyncSwi'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.syncs.SyncSwi.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.syncs']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.syncs.SyncSwi'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.syncs.SyncSwi'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.syncs.SyncSwi$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.syncs.SyncSwi.Module", "ti.sysbios.syncs"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.syncs.SyncSwi$$Object", "ti.sysbios.syncs"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.syncs.SyncSwi.Params", "ti.sysbios.syncs"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$syncs$SyncSwi$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.syncs.SyncSwi'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.syncs.SyncSwi'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.syncs.SyncSwi'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void SyncEvent$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$syncs$SyncEvent$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.syncs.SyncEvent.create() called before xdc.useModule('ti.sysbios.syncs.SyncEvent')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.syncs.SyncEvent$$create", new Proto.Fxn(om.findStrict("ti.sysbios.syncs.SyncEvent.Module", "ti.sysbios.syncs"), om.findStrict("ti.sysbios.syncs.SyncEvent.Instance", "ti.sysbios.syncs"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.syncs.SyncEvent.Params", "ti.sysbios.syncs"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$syncs$SyncEvent$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.syncs.SyncEvent'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.syncs.SyncEvent.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.syncs']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.syncs.SyncEvent'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.syncs.SyncEvent'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.syncs.SyncEvent$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.syncs.SyncEvent.Module", "ti.sysbios.syncs"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.syncs.SyncEvent$$Object", "ti.sysbios.syncs"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.syncs.SyncEvent.Params", "ti.sysbios.syncs"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$syncs$SyncEvent$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.syncs.SyncEvent'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.syncs.SyncEvent'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.syncs.SyncEvent'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void SyncSem$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SyncSwi$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SyncEvent$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SyncSem$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.syncs.SyncSem.Instance_State", "ti.sysbios.syncs");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("userSem", "UShort"));
        sizes.add(Global.newArray("sem", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.syncs.SyncSem.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.syncs.SyncSem.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.syncs.SyncSem.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void SyncSwi$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.syncs.SyncSwi.Instance_State", "ti.sysbios.syncs");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("swi", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.syncs.SyncSwi.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.syncs.SyncSwi.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.syncs.SyncSwi.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void SyncEvent$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.syncs.SyncEvent.Instance_State", "ti.sysbios.syncs");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("evt", "UPtr"));
        sizes.add(Global.newArray("evtId", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.syncs.SyncEvent.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.syncs.SyncEvent.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.syncs.SyncEvent.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void SyncSem$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/syncs/SyncSem.xs");
        om.bind("ti.sysbios.syncs.SyncSem$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSem.Module", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSem.Module", om.findStrict("xdc.runtime.knl.ISync.Module", "ti.sysbios.syncs"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
                po.addFld("NO_WAIT", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.syncs"), $$UNDEF, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.syncs.SyncSem$$create", "ti.sysbios.syncs"), Global.get("ti$sysbios$syncs$SyncSem$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.syncs.SyncSem$$construct", "ti.sysbios.syncs"), Global.get("ti$sysbios$syncs$SyncSem$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSem$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSem$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSem$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSem$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSem$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSem$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSem.Instance", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSem.Instance", om.findStrict("xdc.runtime.knl.ISync.Instance", "ti.sysbios.syncs"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
                po.addFld("NO_WAIT", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
            po.addFld("sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Handle", "ti.sysbios.syncs"), null, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.syncs"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSem$$Params", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSem.Params", om.findStrict("xdc.runtime.knl.ISync$$Params", "ti.sysbios.syncs"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
                po.addFld("NO_WAIT", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
            po.addFld("sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Handle", "ti.sysbios.syncs"), null, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.syncs"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSem$$Object", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSem.Object", om.findStrict("ti.sysbios.syncs.SyncSem.Instance", "ti.sysbios.syncs"));
        // struct SyncSem.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSem$$BasicView", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSem.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("SemaphoreHandle", $$T_Str, $$UNDEF, "w");
                po.addFld("pendedTasks", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
        // struct SyncSem.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSem$$Instance_State", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSem.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("userSem", $$T_Bool, $$UNDEF, "w");
                po.addFld("sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Handle", "ti.sysbios.syncs"), $$UNDEF, "w");
    }

    void SyncSwi$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/syncs/SyncSwi.xs");
        om.bind("ti.sysbios.syncs.SyncSwi$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi.Module", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSwi.Module", om.findStrict("xdc.runtime.knl.ISync.Module", "ti.sysbios.syncs"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
                po.addFld("NO_WAIT", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.syncs"), $$UNDEF, "wh");
            po.addFld("A_nullHandle", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.syncs"), Global.newObject("msg", "A_nullHandle: Null handle passed to create"), "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.syncs.SyncSwi$$create", "ti.sysbios.syncs"), Global.get("ti$sysbios$syncs$SyncSwi$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.syncs.SyncSwi$$construct", "ti.sysbios.syncs"), Global.get("ti$sysbios$syncs$SyncSwi$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSwi$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSwi$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSwi$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSwi$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSwi$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncSwi$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi.Instance", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSwi.Instance", om.findStrict("xdc.runtime.knl.ISync.Instance", "ti.sysbios.syncs"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
                po.addFld("NO_WAIT", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
            po.addFld("swi", (Proto)om.findStrict("ti.sysbios.knl.Swi.Handle", "ti.sysbios.syncs"), null, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.syncs"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi$$Params", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSwi.Params", om.findStrict("xdc.runtime.knl.ISync$$Params", "ti.sysbios.syncs"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
                po.addFld("NO_WAIT", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
            po.addFld("swi", (Proto)om.findStrict("ti.sysbios.knl.Swi.Handle", "ti.sysbios.syncs"), null, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.syncs"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi$$Object", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSwi.Object", om.findStrict("ti.sysbios.syncs.SyncSwi.Instance", "ti.sysbios.syncs"));
        // struct SyncSwi.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi$$BasicView", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSwi.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("SwiHandle", $$T_Str, $$UNDEF, "w");
                po.addFld("fxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
        // struct SyncSwi.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi$$Instance_State", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncSwi.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("swi", (Proto)om.findStrict("ti.sysbios.knl.Swi.Handle", "ti.sysbios.syncs"), $$UNDEF, "w");
    }

    void SyncEvent$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/syncs/SyncEvent.xs");
        om.bind("ti.sysbios.syncs.SyncEvent$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent.Module", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncEvent.Module", om.findStrict("xdc.runtime.knl.ISync.Module", "ti.sysbios.syncs"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
                po.addFld("NO_WAIT", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.syncs"), $$UNDEF, "wh");
            po.addFld("A_nullHandle", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.syncs"), Global.newObject("msg", "A_nullHandle: Null handle passed to create"), "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.syncs.SyncEvent$$create", "ti.sysbios.syncs"), Global.get("ti$sysbios$syncs$SyncEvent$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.syncs.SyncEvent$$construct", "ti.sysbios.syncs"), Global.get("ti$sysbios$syncs$SyncEvent$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncEvent$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncEvent$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncEvent$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncEvent$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncEvent$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.syncs.SyncEvent$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent.Instance", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncEvent.Instance", om.findStrict("xdc.runtime.knl.ISync.Instance", "ti.sysbios.syncs"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
                po.addFld("NO_WAIT", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
            po.addFld("event", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.syncs"), null, "w");
            po.addFld("eventId", Proto.Elm.newCNum("(xdc_UInt)"), 0x1L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.syncs"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent$$Params", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncEvent.Params", om.findStrict("xdc.runtime.knl.ISync$$Params", "ti.sysbios.syncs"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
                po.addFld("NO_WAIT", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
            po.addFld("event", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.syncs"), null, "w");
            po.addFld("eventId", Proto.Elm.newCNum("(xdc_UInt)"), 0x1L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.syncs"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent$$Object", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncEvent.Object", om.findStrict("ti.sysbios.syncs.SyncEvent.Instance", "ti.sysbios.syncs"));
        // struct SyncEvent.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent$$BasicView", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncEvent.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("EventHandle", $$T_Str, $$UNDEF, "w");
                po.addFld("EventId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("pendedTask", $$T_Str, $$UNDEF, "w");
        // struct SyncEvent.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent$$Instance_State", "ti.sysbios.syncs");
        po.init("ti.sysbios.syncs.SyncEvent.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("evt", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.syncs"), $$UNDEF, "w");
                po.addFld("evtId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
    }

    void SyncSem$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.syncs.SyncSem", "ti.sysbios.syncs");
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSem$$Instance_State", "ti.sysbios.syncs");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.syncs.SyncSem.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSem$$Instance_State", "ti.sysbios.syncs");
    }

    void SyncSwi$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi", "ti.sysbios.syncs");
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi$$Instance_State", "ti.sysbios.syncs");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.syncs.SyncSwi.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi$$Instance_State", "ti.sysbios.syncs");
    }

    void SyncEvent$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent", "ti.sysbios.syncs");
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent$$Instance_State", "ti.sysbios.syncs");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.syncs.SyncEvent.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent$$Instance_State", "ti.sysbios.syncs");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.syncs.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.syncs"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/syncs/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.syncs"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.syncs"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.syncs"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.syncs"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.syncs"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.syncs"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.syncs", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.syncs");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.syncs.");
        pkgV.bind("$vers", Global.newArray(2, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.syncs'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.syncs$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.syncs$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.syncs$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.syncs.aem4',\n");
            sb.append("'lib/debug/ti.sysbios.syncs.am4',\n");
            sb.append("'lib/debug/ti.sysbios.syncs.am4g',\n");
            sb.append("'lib/debug/ti.sysbios.syncs.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.syncs.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/debug/ti.sysbios.syncs.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/debug/ti.sysbios.syncs.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/debug/ti.sysbios.syncs.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void SyncSem$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.syncs.SyncSem", "ti.sysbios.syncs");
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSem.Module", "ti.sysbios.syncs");
        vo.init2(po, "ti.sysbios.syncs.SyncSem", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.syncs.SyncSem$$capsule", "ti.sysbios.syncs"));
        vo.bind("Instance", om.findStrict("ti.sysbios.syncs.SyncSem.Instance", "ti.sysbios.syncs"));
        vo.bind("Params", om.findStrict("ti.sysbios.syncs.SyncSem.Params", "ti.sysbios.syncs"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.syncs.SyncSem.Params", "ti.sysbios.syncs")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.syncs.SyncSem.Handle", "ti.sysbios.syncs"));
        vo.bind("$package", om.findStrict("ti.sysbios.syncs", "ti.sysbios.syncs"));
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
        vo.bind("WaitStatus", om.findStrict("xdc.runtime.knl.ISync.WaitStatus", "ti.sysbios.syncs"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.syncs.SyncSem.BasicView", "ti.sysbios.syncs"));
        tdefs.add(om.findStrict("ti.sysbios.syncs.SyncSem.BasicView", "ti.sysbios.syncs"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.syncs.SyncSem.Instance_State", "ti.sysbios.syncs"));
        tdefs.add(om.findStrict("ti.sysbios.syncs.SyncSem.Instance_State", "ti.sysbios.syncs"));
        vo.bind("WaitStatus_ERROR", om.findStrict("xdc.runtime.knl.ISync.WaitStatus_ERROR", "ti.sysbios.syncs"));
        vo.bind("WaitStatus_TIMEOUT", om.findStrict("xdc.runtime.knl.ISync.WaitStatus_TIMEOUT", "ti.sysbios.syncs"));
        vo.bind("WaitStatus_SUCCESS", om.findStrict("xdc.runtime.knl.ISync.WaitStatus_SUCCESS", "ti.sysbios.syncs"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.syncs")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.syncs.SyncSem$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$syncs$SyncSem$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_syncs_SyncSem_Handle__label__E", "ti_sysbios_syncs_SyncSem_Module__startupDone__E", "ti_sysbios_syncs_SyncSem_Object__create__E", "ti_sysbios_syncs_SyncSem_Object__delete__E", "ti_sysbios_syncs_SyncSem_Object__get__E", "ti_sysbios_syncs_SyncSem_Object__first__E", "ti_sysbios_syncs_SyncSem_Object__next__E", "ti_sysbios_syncs_SyncSem_Params__init__E", "ti_sysbios_syncs_SyncSem_query__E", "ti_sysbios_syncs_SyncSem_signal__E", "ti_sysbios_syncs_SyncSem_wait__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.syncs.SyncSem.Object", "ti.sysbios.syncs"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("SyncSem", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SyncSem");
    }

    void SyncSwi$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi", "ti.sysbios.syncs");
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi.Module", "ti.sysbios.syncs");
        vo.init2(po, "ti.sysbios.syncs.SyncSwi", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.syncs.SyncSwi$$capsule", "ti.sysbios.syncs"));
        vo.bind("Instance", om.findStrict("ti.sysbios.syncs.SyncSwi.Instance", "ti.sysbios.syncs"));
        vo.bind("Params", om.findStrict("ti.sysbios.syncs.SyncSwi.Params", "ti.sysbios.syncs"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.syncs.SyncSwi.Params", "ti.sysbios.syncs")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.syncs.SyncSwi.Handle", "ti.sysbios.syncs"));
        vo.bind("$package", om.findStrict("ti.sysbios.syncs", "ti.sysbios.syncs"));
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
        vo.bind("WaitStatus", om.findStrict("xdc.runtime.knl.ISync.WaitStatus", "ti.sysbios.syncs"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.syncs.SyncSwi.BasicView", "ti.sysbios.syncs"));
        tdefs.add(om.findStrict("ti.sysbios.syncs.SyncSwi.BasicView", "ti.sysbios.syncs"));
        mcfgs.add("A_nullHandle");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.syncs.SyncSwi.Instance_State", "ti.sysbios.syncs"));
        tdefs.add(om.findStrict("ti.sysbios.syncs.SyncSwi.Instance_State", "ti.sysbios.syncs"));
        vo.bind("WaitStatus_ERROR", om.findStrict("xdc.runtime.knl.ISync.WaitStatus_ERROR", "ti.sysbios.syncs"));
        vo.bind("WaitStatus_TIMEOUT", om.findStrict("xdc.runtime.knl.ISync.WaitStatus_TIMEOUT", "ti.sysbios.syncs"));
        vo.bind("WaitStatus_SUCCESS", om.findStrict("xdc.runtime.knl.ISync.WaitStatus_SUCCESS", "ti.sysbios.syncs"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.syncs")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.syncs.SyncSwi$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$syncs$SyncSwi$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_syncs_SyncSwi_Handle__label__E", "ti_sysbios_syncs_SyncSwi_Module__startupDone__E", "ti_sysbios_syncs_SyncSwi_Object__create__E", "ti_sysbios_syncs_SyncSwi_Object__delete__E", "ti_sysbios_syncs_SyncSwi_Object__get__E", "ti_sysbios_syncs_SyncSwi_Object__first__E", "ti_sysbios_syncs_SyncSwi_Object__next__E", "ti_sysbios_syncs_SyncSwi_Params__init__E", "ti_sysbios_syncs_SyncSwi_query__E", "ti_sysbios_syncs_SyncSwi_signal__E", "ti_sysbios_syncs_SyncSwi_wait__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_nullHandle"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.syncs.SyncSwi.Object", "ti.sysbios.syncs"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("SyncSwi", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SyncSwi");
    }

    void SyncEvent$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent", "ti.sysbios.syncs");
        po = (Proto.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent.Module", "ti.sysbios.syncs");
        vo.init2(po, "ti.sysbios.syncs.SyncEvent", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.syncs.SyncEvent$$capsule", "ti.sysbios.syncs"));
        vo.bind("Instance", om.findStrict("ti.sysbios.syncs.SyncEvent.Instance", "ti.sysbios.syncs"));
        vo.bind("Params", om.findStrict("ti.sysbios.syncs.SyncEvent.Params", "ti.sysbios.syncs"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.syncs.SyncEvent.Params", "ti.sysbios.syncs")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.syncs.SyncEvent.Handle", "ti.sysbios.syncs"));
        vo.bind("$package", om.findStrict("ti.sysbios.syncs", "ti.sysbios.syncs"));
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
        vo.bind("WaitStatus", om.findStrict("xdc.runtime.knl.ISync.WaitStatus", "ti.sysbios.syncs"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.syncs.SyncEvent.BasicView", "ti.sysbios.syncs"));
        tdefs.add(om.findStrict("ti.sysbios.syncs.SyncEvent.BasicView", "ti.sysbios.syncs"));
        mcfgs.add("A_nullHandle");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.syncs.SyncEvent.Instance_State", "ti.sysbios.syncs"));
        tdefs.add(om.findStrict("ti.sysbios.syncs.SyncEvent.Instance_State", "ti.sysbios.syncs"));
        vo.bind("WaitStatus_ERROR", om.findStrict("xdc.runtime.knl.ISync.WaitStatus_ERROR", "ti.sysbios.syncs"));
        vo.bind("WaitStatus_TIMEOUT", om.findStrict("xdc.runtime.knl.ISync.WaitStatus_TIMEOUT", "ti.sysbios.syncs"));
        vo.bind("WaitStatus_SUCCESS", om.findStrict("xdc.runtime.knl.ISync.WaitStatus_SUCCESS", "ti.sysbios.syncs"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime.knl");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.syncs")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.syncs.SyncEvent$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$syncs$SyncEvent$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_syncs_SyncEvent_Handle__label__E", "ti_sysbios_syncs_SyncEvent_Module__startupDone__E", "ti_sysbios_syncs_SyncEvent_Object__create__E", "ti_sysbios_syncs_SyncEvent_Object__delete__E", "ti_sysbios_syncs_SyncEvent_Object__get__E", "ti_sysbios_syncs_SyncEvent_Object__first__E", "ti_sysbios_syncs_SyncEvent_Object__next__E", "ti_sysbios_syncs_SyncEvent_Params__init__E", "ti_sysbios_syncs_SyncEvent_query__E", "ti_sysbios_syncs_SyncEvent_signal__E", "ti_sysbios_syncs_SyncEvent_wait__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_nullHandle"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.syncs.SyncEvent.Object", "ti.sysbios.syncs"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("SyncEvent", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SyncEvent");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.sysbios.syncs")).findStrict("PARAMS", "ti.sysbios.syncs");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.syncs.SyncSem", "ti.sysbios.syncs")).findStrict("PARAMS", "ti.sysbios.syncs");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.syncs.SyncSwi", "ti.sysbios.syncs")).findStrict("PARAMS", "ti.sysbios.syncs");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.syncs.SyncEvent", "ti.sysbios.syncs")).findStrict("PARAMS", "ti.sysbios.syncs");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.syncs.SyncSem", "ti.sysbios.syncs"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.syncs.SyncSwi", "ti.sysbios.syncs"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.syncs.SyncEvent", "ti.sysbios.syncs"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.syncs.SyncSem", "ti.sysbios.syncs");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.syncs.SyncSwi", "ti.sysbios.syncs");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.syncs.SyncEvent", "ti.sysbios.syncs");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.syncs.SyncSem")).bless();
        ((Value.Obj)om.getv("ti.sysbios.syncs.SyncSwi")).bless();
        ((Value.Obj)om.getv("ti.sysbios.syncs.SyncEvent")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.syncs")).add(pkgV);
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
        SyncSem$$OBJECTS();
        SyncSwi$$OBJECTS();
        SyncEvent$$OBJECTS();
        SyncSem$$CONSTS();
        SyncSwi$$CONSTS();
        SyncEvent$$CONSTS();
        SyncSem$$CREATES();
        SyncSwi$$CREATES();
        SyncEvent$$CREATES();
        SyncSem$$FUNCTIONS();
        SyncSwi$$FUNCTIONS();
        SyncEvent$$FUNCTIONS();
        SyncSem$$SIZES();
        SyncSwi$$SIZES();
        SyncEvent$$SIZES();
        SyncSem$$TYPES();
        SyncSwi$$TYPES();
        SyncEvent$$TYPES();
        if (isROV) {
            SyncSem$$ROV();
            SyncSwi$$ROV();
            SyncEvent$$ROV();
        }//isROV
        $$SINGLETONS();
        SyncSem$$SINGLETONS();
        SyncSwi$$SINGLETONS();
        SyncEvent$$SINGLETONS();
        $$INITIALIZATION();
    }
}
