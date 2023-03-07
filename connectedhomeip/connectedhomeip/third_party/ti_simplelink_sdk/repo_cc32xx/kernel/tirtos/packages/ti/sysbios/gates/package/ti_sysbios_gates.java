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

public class ti_sysbios_gates
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
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.interfaces");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.gates.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.gates", new Value.Obj("ti.sysbios.gates", pkgP));
    }

    void GateHwi$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateHwi.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.gates.GateHwi", new Value.Obj("ti.sysbios.gates.GateHwi", po));
        pkgV.bind("GateHwi", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateHwi$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateHwi.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateHwi$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateHwi.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.gates.GateHwi.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateHwi$$Object", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateHwi.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateHwi$$Params", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateHwi.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.gates.GateHwi.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.gates.GateHwi.Object", om.findStrict("ti.sysbios.gates.GateHwi.Instance_State", "ti.sysbios.gates"));
        }//isROV
    }

    void GateAll$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateAll.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.gates.GateAll", new Value.Obj("ti.sysbios.gates.GateAll", po));
        pkgV.bind("GateAll", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateAll$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateAll.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateAll$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateAll.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.gates.GateAll.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateAll$$Object", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateAll.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateAll$$Params", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateAll.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.gates.GateAll.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.gates.GateAll.Object", om.findStrict("ti.sysbios.gates.GateAll.Instance_State", "ti.sysbios.gates"));
        }//isROV
    }

    void GateSwi$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateSwi.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.gates.GateSwi", new Value.Obj("ti.sysbios.gates.GateSwi", po));
        pkgV.bind("GateSwi", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateSwi$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateSwi.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateSwi$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateSwi.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.gates.GateSwi.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateSwi$$Object", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateSwi.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateSwi$$Params", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateSwi.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.gates.GateSwi.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.gates.GateSwi.Object", om.findStrict("ti.sysbios.gates.GateSwi.Instance_State", "ti.sysbios.gates"));
        }//isROV
    }

    void GateTask$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateTask.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.gates.GateTask", new Value.Obj("ti.sysbios.gates.GateTask", po));
        pkgV.bind("GateTask", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateTask$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateTask.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateTask$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateTask.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.gates.GateTask.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateTask$$Object", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateTask.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateTask$$Params", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateTask.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.gates.GateTask.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.gates.GateTask.Object", om.findStrict("ti.sysbios.gates.GateTask.Instance_State", "ti.sysbios.gates"));
        }//isROV
    }

    void GateTest$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateTest.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.gates.GateTest", new Value.Obj("ti.sysbios.gates.GateTest", po));
        pkgV.bind("GateTest", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateTest$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateTest.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.gates.GateTest.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateTest$$Object", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateTest.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateTest$$Params", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateTest.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.gates.GateTest.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.gates.GateTest.Object", om.findStrict("ti.sysbios.gates.GateTest.Instance_State", "ti.sysbios.gates"));
        }//isROV
    }

    void GateMutexPri$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutexPri.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.gates.GateMutexPri", new Value.Obj("ti.sysbios.gates.GateMutexPri", po));
        pkgV.bind("GateMutexPri", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutexPri$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateMutexPri.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutexPri$$DetailedView", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateMutexPri.DetailedView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutexPri$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateMutexPri.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.gates.GateMutexPri.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutexPri$$Object", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateMutexPri.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutexPri$$Params", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateMutexPri.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.gates.GateMutexPri.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.gates.GateMutexPri.Object", om.findStrict("ti.sysbios.gates.GateMutexPri.Instance_State", "ti.sysbios.gates"));
        }//isROV
    }

    void GateMutex$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutex.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.gates.GateMutex", new Value.Obj("ti.sysbios.gates.GateMutex", po));
        pkgV.bind("GateMutex", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutex$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateMutex.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutex$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateMutex.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.gates.GateMutex.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutex$$Object", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateMutex.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateMutex$$Params", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateMutex.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.gates.GateMutex.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.gates.GateMutex.Object", om.findStrict("ti.sysbios.gates.GateMutex.Instance_State", "ti.sysbios.gates"));
        }//isROV
    }

    void GateSpinlock$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateSpinlock.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.gates.GateSpinlock", new Value.Obj("ti.sysbios.gates.GateSpinlock", po));
        pkgV.bind("GateSpinlock", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateSpinlock$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateSpinlock.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.gates.GateSpinlock$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateSpinlock.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.gates.GateSpinlock.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateSpinlock$$Object", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateSpinlock.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.gates.GateSpinlock$$Params", new Proto.Obj());
        om.bind("ti.sysbios.gates.GateSpinlock.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.gates.GateSpinlock.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.gates.GateSpinlock.Object", om.findStrict("ti.sysbios.gates.GateSpinlock.Instance_State", "ti.sysbios.gates"));
        }//isROV
    }

    void GateHwi$$CONSTS()
    {
        // module GateHwi
        om.bind("ti.sysbios.gates.GateHwi.query", new Extern("ti_sysbios_gates_GateHwi_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void GateAll$$CONSTS()
    {
        // module GateAll
        om.bind("ti.sysbios.gates.GateAll.query", new Extern("ti_sysbios_gates_GateAll_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void GateSwi$$CONSTS()
    {
        // module GateSwi
        om.bind("ti.sysbios.gates.GateSwi.query", new Extern("ti_sysbios_gates_GateSwi_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void GateTask$$CONSTS()
    {
        // module GateTask
        om.bind("ti.sysbios.gates.GateTask.query", new Extern("ti_sysbios_gates_GateTask_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void GateTest$$CONSTS()
    {
        // module GateTest
        om.bind("ti.sysbios.gates.GateTest.query", new Extern("ti_sysbios_gates_GateTest_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void GateMutexPri$$CONSTS()
    {
        // module GateMutexPri
        om.bind("ti.sysbios.gates.GateMutexPri.query", new Extern("ti_sysbios_gates_GateMutexPri_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.gates.GateMutexPri.insertPri", new Extern("ti_sysbios_gates_GateMutexPri_insertPri__I", "xdc_Void(*)(ti_sysbios_knl_Queue_Object*,ti_sysbios_knl_Queue_Elem*,xdc_Int)", true, false));
    }

    void GateMutex$$CONSTS()
    {
        // module GateMutex
        om.bind("ti.sysbios.gates.GateMutex.query", new Extern("ti_sysbios_gates_GateMutex_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void GateSpinlock$$CONSTS()
    {
        // module GateSpinlock
        om.bind("ti.sysbios.gates.GateSpinlock.query", new Extern("ti_sysbios_gates_GateSpinlock_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void GateHwi$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateHwi$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.gates.GateHwi.create() called before xdc.useModule('ti.sysbios.gates.GateHwi')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateHwi$$create", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateHwi.Module", "ti.sysbios.gates"), om.findStrict("ti.sysbios.gates.GateHwi.Instance", "ti.sysbios.gates"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateHwi.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateHwi$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateHwi'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.gates.GateHwi.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.gates']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.gates.GateHwi'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateHwi'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateHwi$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateHwi.Module", "ti.sysbios.gates"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.gates.GateHwi$$Object", "ti.sysbios.gates"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateHwi.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateHwi$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateHwi'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.gates.GateHwi'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateHwi'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void GateAll$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateAll$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.gates.GateAll.create() called before xdc.useModule('ti.sysbios.gates.GateAll')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateAll$$create", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateAll.Module", "ti.sysbios.gates"), om.findStrict("ti.sysbios.gates.GateAll.Instance", "ti.sysbios.gates"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateAll.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateAll$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateAll'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.gates.GateAll.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.gates']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.gates.GateAll'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateAll'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateAll$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateAll.Module", "ti.sysbios.gates"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.gates.GateAll$$Object", "ti.sysbios.gates"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateAll.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateAll$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateAll'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.gates.GateAll'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateAll'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void GateSwi$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateSwi$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.gates.GateSwi.create() called before xdc.useModule('ti.sysbios.gates.GateSwi')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateSwi$$create", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateSwi.Module", "ti.sysbios.gates"), om.findStrict("ti.sysbios.gates.GateSwi.Instance", "ti.sysbios.gates"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateSwi.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateSwi$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateSwi'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.gates.GateSwi.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.gates']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.gates.GateSwi'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateSwi'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateSwi$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateSwi.Module", "ti.sysbios.gates"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.gates.GateSwi$$Object", "ti.sysbios.gates"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateSwi.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateSwi$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateSwi'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.gates.GateSwi'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateSwi'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void GateTask$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateTask$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.gates.GateTask.create() called before xdc.useModule('ti.sysbios.gates.GateTask')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateTask$$create", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateTask.Module", "ti.sysbios.gates"), om.findStrict("ti.sysbios.gates.GateTask.Instance", "ti.sysbios.gates"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateTask.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateTask$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateTask'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.gates.GateTask.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.gates']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.gates.GateTask'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateTask'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateTask$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateTask.Module", "ti.sysbios.gates"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.gates.GateTask$$Object", "ti.sysbios.gates"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateTask.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateTask$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateTask'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.gates.GateTask'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateTask'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void GateTest$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateTest$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.gates.GateTest.create() called before xdc.useModule('ti.sysbios.gates.GateTest')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateTest$$create", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateTest.Module", "ti.sysbios.gates"), om.findStrict("ti.sysbios.gates.GateTest.Instance", "ti.sysbios.gates"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateTest.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateTest$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateTest'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.gates.GateTest.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.gates']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.gates.GateTest'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateTest'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateTest$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateTest.Module", "ti.sysbios.gates"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.gates.GateTest$$Object", "ti.sysbios.gates"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateTest.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateTest$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateTest'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.gates.GateTest'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateTest'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void GateMutexPri$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateMutexPri$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.gates.GateMutexPri.create() called before xdc.useModule('ti.sysbios.gates.GateMutexPri')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateMutexPri$$create", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateMutexPri.Module", "ti.sysbios.gates"), om.findStrict("ti.sysbios.gates.GateMutexPri.Instance", "ti.sysbios.gates"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateMutexPri.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateMutexPri$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateMutexPri'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.gates.GateMutexPri.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.gates']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.gates.GateMutexPri'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateMutexPri'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateMutexPri$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateMutexPri.Module", "ti.sysbios.gates"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.gates.GateMutexPri$$Object", "ti.sysbios.gates"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateMutexPri.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateMutexPri$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateMutexPri'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.gates.GateMutexPri'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateMutexPri'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void GateMutex$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateMutex$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.gates.GateMutex.create() called before xdc.useModule('ti.sysbios.gates.GateMutex')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateMutex$$create", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateMutex.Module", "ti.sysbios.gates"), om.findStrict("ti.sysbios.gates.GateMutex.Instance", "ti.sysbios.gates"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateMutex.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateMutex$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateMutex'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.gates.GateMutex.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.gates']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.gates.GateMutex'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateMutex'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateMutex$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateMutex.Module", "ti.sysbios.gates"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.gates.GateMutex$$Object", "ti.sysbios.gates"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateMutex.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateMutex$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateMutex'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.gates.GateMutex'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateMutex'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void GateSpinlock$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateSpinlock$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.gates.GateSpinlock.create() called before xdc.useModule('ti.sysbios.gates.GateSpinlock')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateSpinlock$$create", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateSpinlock.Module", "ti.sysbios.gates"), om.findStrict("ti.sysbios.gates.GateSpinlock.Instance", "ti.sysbios.gates"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateSpinlock.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateSpinlock$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateSpinlock'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.gates.GateSpinlock.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.gates']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.gates.GateSpinlock'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateSpinlock'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.gates.GateSpinlock$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.gates.GateSpinlock.Module", "ti.sysbios.gates"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.gates.GateSpinlock$$Object", "ti.sysbios.gates"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.gates.GateSpinlock.Params", "ti.sysbios.gates"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$gates$GateSpinlock$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.gates.GateSpinlock'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.gates.GateSpinlock'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.gates.GateSpinlock'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void GateHwi$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateAll$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateSwi$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateTask$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateTest$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateMutexPri$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateMutex$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateSpinlock$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateHwi$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.gates.GateHwi.Instance_State", "ti.sysbios.gates");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.gates.GateHwi.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.gates.GateHwi.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.gates.GateHwi.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void GateAll$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.gates.GateAll.Instance_State", "ti.sysbios.gates");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("entered", "UShort"));
        sizes.add(Global.newArray("hwiKey", "UInt"));
        sizes.add(Global.newArray("swiKey", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.gates.GateAll.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.gates.GateAll.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.gates.GateAll.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void GateSwi$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.gates.GateSwi.Instance_State", "ti.sysbios.gates");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.gates.GateSwi.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.gates.GateSwi.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.gates.GateSwi.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void GateTask$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.gates.GateTask.Instance_State", "ti.sysbios.gates");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.gates.GateTask.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.gates.GateTask.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.gates.GateTask.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void GateTest$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.gates.GateTest.Instance_State", "ti.sysbios.gates");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("enterCount", "TInt"));
        sizes.add(Global.newArray("leaveCount", "TInt"));
        sizes.add(Global.newArray("createCount", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.gates.GateTest.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.gates.GateTest.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.gates.GateTest.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void GateMutexPri$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.gates.GateMutexPri.Instance_State", "ti.sysbios.gates");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("mutexCnt", "UInt"));
        sizes.add(Global.newArray("ownerOrigPri", "TInt"));
        sizes.add(Global.newArray("owner", "UPtr"));
        sizes.add(Global.newArray("pendQ", "Sti.sysbios.knl.Queue;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.gates.GateMutexPri.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.gates.GateMutexPri.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.gates.GateMutexPri.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void GateMutex$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.gates.GateMutex.Instance_State", "ti.sysbios.gates");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("owner", "UPtr"));
        sizes.add(Global.newArray("sem", "Sti.sysbios.knl.Semaphore;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.gates.GateMutex.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.gates.GateMutex.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.gates.GateMutex.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void GateSpinlock$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.gates.GateSpinlock.Instance_State", "ti.sysbios.gates");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("hwiKey", "UInt"));
        sizes.add(Global.newArray("owner", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.gates.GateSpinlock.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.gates.GateSpinlock.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.gates.GateSpinlock.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void GateHwi$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/gates/GateHwi.xs");
        om.bind("ti.sysbios.gates.GateHwi$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateHwi.Module", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateHwi.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.gates"), $$UNDEF, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateHwi$$create", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateHwi$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateHwi$$construct", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateHwi$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.gates.GateHwi$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateHwi$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateHwi$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateHwi$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.gates.GateHwi$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateHwi$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.gates"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateHwi.Instance", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateHwi.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateHwi$$Params", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateHwi.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateHwi$$Object", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateHwi.Object", om.findStrict("ti.sysbios.gates.GateHwi.Instance", "ti.sysbios.gates"));
        // struct GateHwi.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateHwi$$BasicView", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateHwi.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
        // struct GateHwi.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateHwi$$Instance_State", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateHwi.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void GateAll$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/gates/GateAll.xs");
        om.bind("ti.sysbios.gates.GateAll$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateAll.Module", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateAll.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.gates"), $$UNDEF, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateAll$$create", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateAll$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateAll$$construct", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateAll$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.gates.GateAll$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateAll$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateAll$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateAll$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.gates.GateAll$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateAll$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.gates"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateAll.Instance", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateAll.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateAll$$Params", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateAll.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateAll$$Object", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateAll.Object", om.findStrict("ti.sysbios.gates.GateAll.Instance", "ti.sysbios.gates"));
        // struct GateAll.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateAll$$BasicView", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateAll.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("entered", $$T_Bool, $$UNDEF, "w");
        // struct GateAll.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateAll$$Instance_State", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateAll.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("entered", $$T_Bool, $$UNDEF, "w");
                po.addFld("hwiKey", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("swiKey", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
    }

    void GateSwi$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/gates/GateSwi.xs");
        om.bind("ti.sysbios.gates.GateSwi$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSwi.Module", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSwi.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.gates"), $$UNDEF, "wh");
            po.addFld("A_badContext", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.gates"), Global.newObject("msg", "A_badContext: bad calling context. May not be entered from a hardware interrupt thread."), "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateSwi$$create", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateSwi$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateSwi$$construct", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateSwi$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSwi$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSwi$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSwi$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSwi$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSwi$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSwi$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.gates"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSwi.Instance", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSwi.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSwi$$Params", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSwi.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSwi$$Object", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSwi.Object", om.findStrict("ti.sysbios.gates.GateSwi.Instance", "ti.sysbios.gates"));
        // struct GateSwi.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSwi$$BasicView", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSwi.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
        // struct GateSwi.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSwi$$Instance_State", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSwi.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void GateTask$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/gates/GateTask.xs");
        om.bind("ti.sysbios.gates.GateTask$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTask.Module", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTask.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.gates"), $$UNDEF, "wh");
            po.addFld("A_badContext", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.gates"), Global.newObject("msg", "A_badContext: bad calling context. May not be entered from a software or hardware interrupt thread."), "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateTask$$create", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateTask$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateTask$$construct", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateTask$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTask$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTask$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTask$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTask$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTask$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTask$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.gates"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTask.Instance", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTask.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTask$$Params", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTask.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTask$$Object", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTask.Object", om.findStrict("ti.sysbios.gates.GateTask.Instance", "ti.sysbios.gates"));
        // struct GateTask.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTask$$BasicView", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTask.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
        // struct GateTask.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTask$$Instance_State", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTask.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void GateTest$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/gates/GateTest.xs");
        om.bind("ti.sysbios.gates.GateTest$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTest.Module", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTest.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateTest$$create", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateTest$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateTest$$construct", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateTest$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTest$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTest$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTest$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTest$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTest$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateTest$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.gates"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTest.Instance", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTest.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTest$$Params", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTest.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTest$$Object", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTest.Object", om.findStrict("ti.sysbios.gates.GateTest.Instance", "ti.sysbios.gates"));
        // struct GateTest.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTest$$Instance_State", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateTest.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enterCount", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("leaveCount", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("createCount", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
    }

    void GateMutexPri$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/gates/GateMutexPri.xs");
        om.bind("ti.sysbios.gates.GateMutexPri$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri.Module", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutexPri.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.gates"), $$UNDEF, "wh");
            po.addFld("A_badContext", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.gates"), Global.newObject("msg", "A_badContext: bad calling context. See GateMutexPri API doc for details."), "w");
            po.addFld("A_enterTaskDisabled", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.gates"), Global.newObject("msg", "A_enterTaskDisabled: Cannot call GateMutexPri_enter() while the Task or Swi scheduler is disabled."), "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateMutexPri$$create", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateMutexPri$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateMutexPri$$construct", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateMutexPri$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutexPri$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutexPri$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutexPri$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutexPri$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutexPri$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutexPri$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.gates"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri.Instance", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutexPri.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri$$Params", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutexPri.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri$$Object", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutexPri.Object", om.findStrict("ti.sysbios.gates.GateMutexPri.Instance", "ti.sysbios.gates"));
        // struct GateMutexPri.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri$$BasicView", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutexPri.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("status", $$T_Str, $$UNDEF, "w");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("mutexCnt", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("owner", $$T_Str, $$UNDEF, "w");
                po.addFld("ownerOrigPri", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("ownerCurrPri", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // struct GateMutexPri.DetailedView
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri$$DetailedView", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutexPri.DetailedView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("status", $$T_Str, $$UNDEF, "w");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("mutexCnt", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("owner", $$T_Str, $$UNDEF, "w");
                po.addFld("ownerOrigPri", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("ownerCurrPri", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("pendedTasks", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
        // struct GateMutexPri.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri$$Instance_State", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutexPri.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("mutexCnt", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("ownerOrigPri", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("owner", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.gates"), $$UNDEF, "w");
                po.addFldV("pendQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.gates"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_pendQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.gates"), $$DEFAULT, "w");
    }

    void GateMutex$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/gates/GateMutex.xs");
        om.bind("ti.sysbios.gates.GateMutex$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutex.Module", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutex.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.gates"), $$UNDEF, "wh");
            po.addFld("A_badContext", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.gates"), Global.newObject("msg", "A_badContext: bad calling context. See GateMutex API doc for details."), "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateMutex$$create", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateMutex$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateMutex$$construct", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateMutex$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutex$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutex$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutex$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutex$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutex$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateMutex$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.gates"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutex.Instance", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutex.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutex$$Params", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutex.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutex$$Object", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutex.Object", om.findStrict("ti.sysbios.gates.GateMutex.Instance", "ti.sysbios.gates"));
        // struct GateMutex.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutex$$BasicView", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutex.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("status", $$T_Str, $$UNDEF, "w");
                po.addFld("owner", $$T_Str, $$UNDEF, "w");
                po.addFld("pendedTasks", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
        // struct GateMutex.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutex$$Instance_State", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateMutex.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("owner", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.gates"), $$UNDEF, "w");
                po.addFldV("sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.gates"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.gates"), $$DEFAULT, "w");
    }

    void GateSpinlock$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/gates/GateSpinlock.xs");
        om.bind("ti.sysbios.gates.GateSpinlock$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock.Module", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSpinlock.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.gates"), $$UNDEF, "wh");
            po.addFld("A_badContext", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.gates"), Global.newObject("msg", "A_badContext: bad calling context. See GateSpinlock API doc for details."), "w");
            po.addFld("A_invalidQuality", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.gates"), Global.newObject("msg", "A_invalidQuality: See GateSpinlock API doc for details."), "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateSpinlock$$create", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateSpinlock$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.gates.GateSpinlock$$construct", "ti.sysbios.gates"), Global.get("ti$sysbios$gates$GateSpinlock$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSpinlock$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSpinlock$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSpinlock$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSpinlock$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSpinlock$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.gates.GateSpinlock$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.gates"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock.Instance", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSpinlock.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock$$Params", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSpinlock.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.sysbios.gates"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.gates"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock$$Object", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSpinlock.Object", om.findStrict("ti.sysbios.gates.GateSpinlock.Instance", "ti.sysbios.gates"));
        // struct GateSpinlock.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock$$BasicView", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSpinlock.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("status", $$T_Str, $$UNDEF, "w");
                po.addFld("owner", $$T_Str, $$UNDEF, "w");
        // struct GateSpinlock.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock$$Instance_State", "ti.sysbios.gates");
        po.init("ti.sysbios.gates.GateSpinlock.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("hwiKey", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("owner", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.gates"), $$UNDEF, "w");
    }

    void GateHwi$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateHwi", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateHwi$$Instance_State", "ti.sysbios.gates");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.gates.GateHwi.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateHwi$$Instance_State", "ti.sysbios.gates");
    }

    void GateAll$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateAll", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateAll$$Instance_State", "ti.sysbios.gates");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.gates.GateAll.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateAll$$Instance_State", "ti.sysbios.gates");
    }

    void GateSwi$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateSwi", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSwi$$Instance_State", "ti.sysbios.gates");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.gates.GateSwi.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSwi$$Instance_State", "ti.sysbios.gates");
    }

    void GateTask$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateTask", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTask$$Instance_State", "ti.sysbios.gates");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.gates.GateTask.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTask$$Instance_State", "ti.sysbios.gates");
    }

    void GateTest$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateTest", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTest$$Instance_State", "ti.sysbios.gates");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.gates.GateTest.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTest$$Instance_State", "ti.sysbios.gates");
    }

    void GateMutexPri$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri$$Instance_State", "ti.sysbios.gates");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.gates.GateMutexPri.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri$$Instance_State", "ti.sysbios.gates");
    }

    void GateMutex$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateMutex", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutex$$Instance_State", "ti.sysbios.gates");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.gates.GateMutex.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutex$$Instance_State", "ti.sysbios.gates");
    }

    void GateSpinlock$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock$$Instance_State", "ti.sysbios.gates");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.gates.GateSpinlock.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock$$Instance_State", "ti.sysbios.gates");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.gates.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.gates"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/gates/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.gates"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.gates"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.gates"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.gates"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.gates"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.gates"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.gates", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.gates");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.gates.");
        pkgV.bind("$vers", Global.newArray(2, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        imports.add(Global.newArray("ti.sysbios.knl", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.gates'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.gates$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.gates$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.gates$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.gates.aem4',\n");
            sb.append("'lib/debug/ti.sysbios.gates.am4',\n");
            sb.append("'lib/debug/ti.sysbios.gates.am4g',\n");
            sb.append("'lib/debug/ti.sysbios.gates.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.gates.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/debug/ti.sysbios.gates.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/debug/ti.sysbios.gates.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/debug/ti.sysbios.gates.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void GateHwi$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateHwi", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateHwi.Module", "ti.sysbios.gates");
        vo.init2(po, "ti.sysbios.gates.GateHwi", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.gates.GateHwi$$capsule", "ti.sysbios.gates"));
        vo.bind("Instance", om.findStrict("ti.sysbios.gates.GateHwi.Instance", "ti.sysbios.gates"));
        vo.bind("Params", om.findStrict("ti.sysbios.gates.GateHwi.Params", "ti.sysbios.gates"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.gates.GateHwi.Params", "ti.sysbios.gates")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.gates.GateHwi.Handle", "ti.sysbios.gates"));
        vo.bind("$package", om.findStrict("ti.sysbios.gates", "ti.sysbios.gates"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.gates.GateHwi.BasicView", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateHwi.BasicView", "ti.sysbios.gates"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.gates.GateHwi.Instance_State", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateHwi.Instance_State", "ti.sysbios.gates"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.gates")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.gates.GateHwi$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$gates$GateHwi$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.gates.GateHwi.query", "ti.sysbios.gates"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_gates_GateHwi_Handle__label__E", "ti_sysbios_gates_GateHwi_Module__startupDone__E", "ti_sysbios_gates_GateHwi_Object__create__E", "ti_sysbios_gates_GateHwi_Object__delete__E", "ti_sysbios_gates_GateHwi_Object__get__E", "ti_sysbios_gates_GateHwi_Object__first__E", "ti_sysbios_gates_GateHwi_Object__next__E", "ti_sysbios_gates_GateHwi_Params__init__E", "ti_sysbios_gates_GateHwi_query__E", "ti_sysbios_gates_GateHwi_enter__E", "ti_sysbios_gates_GateHwi_leave__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.gates.GateHwi.Object", "ti.sysbios.gates"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateHwi", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateHwi");
    }

    void GateAll$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateAll", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateAll.Module", "ti.sysbios.gates");
        vo.init2(po, "ti.sysbios.gates.GateAll", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.gates.GateAll$$capsule", "ti.sysbios.gates"));
        vo.bind("Instance", om.findStrict("ti.sysbios.gates.GateAll.Instance", "ti.sysbios.gates"));
        vo.bind("Params", om.findStrict("ti.sysbios.gates.GateAll.Params", "ti.sysbios.gates"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.gates.GateAll.Params", "ti.sysbios.gates")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.gates.GateAll.Handle", "ti.sysbios.gates"));
        vo.bind("$package", om.findStrict("ti.sysbios.gates", "ti.sysbios.gates"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.gates.GateAll.BasicView", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateAll.BasicView", "ti.sysbios.gates"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.gates.GateAll.Instance_State", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateAll.Instance_State", "ti.sysbios.gates"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.gates")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.gates.GateAll$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$gates$GateAll$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.gates.GateAll.query", "ti.sysbios.gates"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_gates_GateAll_Handle__label__E", "ti_sysbios_gates_GateAll_Module__startupDone__E", "ti_sysbios_gates_GateAll_Object__create__E", "ti_sysbios_gates_GateAll_Object__delete__E", "ti_sysbios_gates_GateAll_Object__get__E", "ti_sysbios_gates_GateAll_Object__first__E", "ti_sysbios_gates_GateAll_Object__next__E", "ti_sysbios_gates_GateAll_Params__init__E", "ti_sysbios_gates_GateAll_query__E", "ti_sysbios_gates_GateAll_enter__E", "ti_sysbios_gates_GateAll_leave__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.gates.GateAll.Object", "ti.sysbios.gates"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateAll", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateAll");
    }

    void GateSwi$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateSwi", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSwi.Module", "ti.sysbios.gates");
        vo.init2(po, "ti.sysbios.gates.GateSwi", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.gates.GateSwi$$capsule", "ti.sysbios.gates"));
        vo.bind("Instance", om.findStrict("ti.sysbios.gates.GateSwi.Instance", "ti.sysbios.gates"));
        vo.bind("Params", om.findStrict("ti.sysbios.gates.GateSwi.Params", "ti.sysbios.gates"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.gates.GateSwi.Params", "ti.sysbios.gates")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.gates.GateSwi.Handle", "ti.sysbios.gates"));
        vo.bind("$package", om.findStrict("ti.sysbios.gates", "ti.sysbios.gates"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.gates.GateSwi.BasicView", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateSwi.BasicView", "ti.sysbios.gates"));
        mcfgs.add("A_badContext");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.gates.GateSwi.Instance_State", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateSwi.Instance_State", "ti.sysbios.gates"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.gates")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.gates.GateSwi$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$gates$GateSwi$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.gates.GateSwi.query", "ti.sysbios.gates"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_gates_GateSwi_Handle__label__E", "ti_sysbios_gates_GateSwi_Module__startupDone__E", "ti_sysbios_gates_GateSwi_Object__create__E", "ti_sysbios_gates_GateSwi_Object__delete__E", "ti_sysbios_gates_GateSwi_Object__get__E", "ti_sysbios_gates_GateSwi_Object__first__E", "ti_sysbios_gates_GateSwi_Object__next__E", "ti_sysbios_gates_GateSwi_Params__init__E", "ti_sysbios_gates_GateSwi_query__E", "ti_sysbios_gates_GateSwi_enter__E", "ti_sysbios_gates_GateSwi_leave__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_badContext"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.gates.GateSwi.Object", "ti.sysbios.gates"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateSwi", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateSwi");
    }

    void GateTask$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateTask", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTask.Module", "ti.sysbios.gates");
        vo.init2(po, "ti.sysbios.gates.GateTask", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.gates.GateTask$$capsule", "ti.sysbios.gates"));
        vo.bind("Instance", om.findStrict("ti.sysbios.gates.GateTask.Instance", "ti.sysbios.gates"));
        vo.bind("Params", om.findStrict("ti.sysbios.gates.GateTask.Params", "ti.sysbios.gates"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.gates.GateTask.Params", "ti.sysbios.gates")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.gates.GateTask.Handle", "ti.sysbios.gates"));
        vo.bind("$package", om.findStrict("ti.sysbios.gates", "ti.sysbios.gates"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.gates.GateTask.BasicView", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateTask.BasicView", "ti.sysbios.gates"));
        mcfgs.add("A_badContext");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.gates.GateTask.Instance_State", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateTask.Instance_State", "ti.sysbios.gates"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.gates")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.gates.GateTask$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$gates$GateTask$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.gates.GateTask.query", "ti.sysbios.gates"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_gates_GateTask_Handle__label__E", "ti_sysbios_gates_GateTask_Module__startupDone__E", "ti_sysbios_gates_GateTask_Object__create__E", "ti_sysbios_gates_GateTask_Object__delete__E", "ti_sysbios_gates_GateTask_Object__get__E", "ti_sysbios_gates_GateTask_Object__first__E", "ti_sysbios_gates_GateTask_Object__next__E", "ti_sysbios_gates_GateTask_Params__init__E", "ti_sysbios_gates_GateTask_query__E", "ti_sysbios_gates_GateTask_enter__E", "ti_sysbios_gates_GateTask_leave__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_badContext"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.gates.GateTask.Object", "ti.sysbios.gates"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateTask", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateTask");
    }

    void GateTest$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateTest", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateTest.Module", "ti.sysbios.gates");
        vo.init2(po, "ti.sysbios.gates.GateTest", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.gates.GateTest$$capsule", "ti.sysbios.gates"));
        vo.bind("Instance", om.findStrict("ti.sysbios.gates.GateTest.Instance", "ti.sysbios.gates"));
        vo.bind("Params", om.findStrict("ti.sysbios.gates.GateTest.Params", "ti.sysbios.gates"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.gates.GateTest.Params", "ti.sysbios.gates")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.gates.GateTest.Handle", "ti.sysbios.gates"));
        vo.bind("$package", om.findStrict("ti.sysbios.gates", "ti.sysbios.gates"));
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
        vo.bind("Instance_State", om.findStrict("ti.sysbios.gates.GateTest.Instance_State", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateTest.Instance_State", "ti.sysbios.gates"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.gates")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.gates.GateTest$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$gates$GateTest$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.gates.GateTest.query", "ti.sysbios.gates"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_gates_GateTest_Handle__label__E", "ti_sysbios_gates_GateTest_Module__startupDone__E", "ti_sysbios_gates_GateTest_Object__create__E", "ti_sysbios_gates_GateTest_Object__delete__E", "ti_sysbios_gates_GateTest_Object__get__E", "ti_sysbios_gates_GateTest_Object__first__E", "ti_sysbios_gates_GateTest_Object__next__E", "ti_sysbios_gates_GateTest_Params__init__E", "ti_sysbios_gates_GateTest_query__E", "ti_sysbios_gates_GateTest_enter__E", "ti_sysbios_gates_GateTest_leave__E", "ti_sysbios_gates_GateTest_getEnterCount__E", "ti_sysbios_gates_GateTest_getLeaveCount__E", "ti_sysbios_gates_GateTest_getCreateCount__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.gates.GateTest.Object", "ti.sysbios.gates"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateTest", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateTest");
    }

    void GateMutexPri$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri.Module", "ti.sysbios.gates");
        vo.init2(po, "ti.sysbios.gates.GateMutexPri", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.gates.GateMutexPri$$capsule", "ti.sysbios.gates"));
        vo.bind("Instance", om.findStrict("ti.sysbios.gates.GateMutexPri.Instance", "ti.sysbios.gates"));
        vo.bind("Params", om.findStrict("ti.sysbios.gates.GateMutexPri.Params", "ti.sysbios.gates"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.gates.GateMutexPri.Params", "ti.sysbios.gates")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.gates.GateMutexPri.Handle", "ti.sysbios.gates"));
        vo.bind("$package", om.findStrict("ti.sysbios.gates", "ti.sysbios.gates"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.gates.GateMutexPri.BasicView", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateMutexPri.BasicView", "ti.sysbios.gates"));
        vo.bind("DetailedView", om.findStrict("ti.sysbios.gates.GateMutexPri.DetailedView", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateMutexPri.DetailedView", "ti.sysbios.gates"));
        mcfgs.add("A_badContext");
        mcfgs.add("A_enterTaskDisabled");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.gates.GateMutexPri.Instance_State", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateMutexPri.Instance_State", "ti.sysbios.gates"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.gates")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.gates.GateMutexPri$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$gates$GateMutexPri$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.gates.GateMutexPri.query", "ti.sysbios.gates"));
        vo.bind("insertPri", om.findStrict("ti.sysbios.gates.GateMutexPri.insertPri", "ti.sysbios.gates"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_gates_GateMutexPri_Handle__label__E", "ti_sysbios_gates_GateMutexPri_Module__startupDone__E", "ti_sysbios_gates_GateMutexPri_Object__create__E", "ti_sysbios_gates_GateMutexPri_Object__delete__E", "ti_sysbios_gates_GateMutexPri_Object__get__E", "ti_sysbios_gates_GateMutexPri_Object__first__E", "ti_sysbios_gates_GateMutexPri_Object__next__E", "ti_sysbios_gates_GateMutexPri_Params__init__E", "ti_sysbios_gates_GateMutexPri_query__E", "ti_sysbios_gates_GateMutexPri_enter__E", "ti_sysbios_gates_GateMutexPri_leave__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_badContext", "A_enterTaskDisabled"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.gates.GateMutexPri.Object", "ti.sysbios.gates"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateMutexPri", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateMutexPri");
    }

    void GateMutex$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateMutex", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateMutex.Module", "ti.sysbios.gates");
        vo.init2(po, "ti.sysbios.gates.GateMutex", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.gates.GateMutex$$capsule", "ti.sysbios.gates"));
        vo.bind("Instance", om.findStrict("ti.sysbios.gates.GateMutex.Instance", "ti.sysbios.gates"));
        vo.bind("Params", om.findStrict("ti.sysbios.gates.GateMutex.Params", "ti.sysbios.gates"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.gates.GateMutex.Params", "ti.sysbios.gates")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.gates.GateMutex.Handle", "ti.sysbios.gates"));
        vo.bind("$package", om.findStrict("ti.sysbios.gates", "ti.sysbios.gates"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.gates.GateMutex.BasicView", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateMutex.BasicView", "ti.sysbios.gates"));
        mcfgs.add("A_badContext");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.gates.GateMutex.Instance_State", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateMutex.Instance_State", "ti.sysbios.gates"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.gates")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.gates.GateMutex$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$gates$GateMutex$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.gates.GateMutex.query", "ti.sysbios.gates"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_gates_GateMutex_Handle__label__E", "ti_sysbios_gates_GateMutex_Module__startupDone__E", "ti_sysbios_gates_GateMutex_Object__create__E", "ti_sysbios_gates_GateMutex_Object__delete__E", "ti_sysbios_gates_GateMutex_Object__get__E", "ti_sysbios_gates_GateMutex_Object__first__E", "ti_sysbios_gates_GateMutex_Object__next__E", "ti_sysbios_gates_GateMutex_Params__init__E", "ti_sysbios_gates_GateMutex_query__E", "ti_sysbios_gates_GateMutex_enter__E", "ti_sysbios_gates_GateMutex_leave__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_badContext"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.gates.GateMutex.Object", "ti.sysbios.gates"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateMutex", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateMutex");
    }

    void GateSpinlock$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock", "ti.sysbios.gates");
        po = (Proto.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock.Module", "ti.sysbios.gates");
        vo.init2(po, "ti.sysbios.gates.GateSpinlock", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.gates.GateSpinlock$$capsule", "ti.sysbios.gates"));
        vo.bind("Instance", om.findStrict("ti.sysbios.gates.GateSpinlock.Instance", "ti.sysbios.gates"));
        vo.bind("Params", om.findStrict("ti.sysbios.gates.GateSpinlock.Params", "ti.sysbios.gates"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.gates.GateSpinlock.Params", "ti.sysbios.gates")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.gates.GateSpinlock.Handle", "ti.sysbios.gates"));
        vo.bind("$package", om.findStrict("ti.sysbios.gates", "ti.sysbios.gates"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.gates.GateSpinlock.BasicView", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateSpinlock.BasicView", "ti.sysbios.gates"));
        mcfgs.add("A_badContext");
        mcfgs.add("A_invalidQuality");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.gates.GateSpinlock.Instance_State", "ti.sysbios.gates"));
        tdefs.add(om.findStrict("ti.sysbios.gates.GateSpinlock.Instance_State", "ti.sysbios.gates"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.gates")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.gates.GateSpinlock$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$gates$GateSpinlock$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.gates.GateSpinlock.query", "ti.sysbios.gates"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_gates_GateSpinlock_Handle__label__E", "ti_sysbios_gates_GateSpinlock_Module__startupDone__E", "ti_sysbios_gates_GateSpinlock_Object__create__E", "ti_sysbios_gates_GateSpinlock_Object__delete__E", "ti_sysbios_gates_GateSpinlock_Object__get__E", "ti_sysbios_gates_GateSpinlock_Object__first__E", "ti_sysbios_gates_GateSpinlock_Object__next__E", "ti_sysbios_gates_GateSpinlock_Params__init__E", "ti_sysbios_gates_GateSpinlock_query__E", "ti_sysbios_gates_GateSpinlock_enter__E", "ti_sysbios_gates_GateSpinlock_enterHwi__E", "ti_sysbios_gates_GateSpinlock_leave__E", "ti_sysbios_gates_GateSpinlock_leaveHwi__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_badContext", "A_invalidQuality"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.gates.GateSpinlock.Object", "ti.sysbios.gates"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateSpinlock", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateSpinlock");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.sysbios.gates")).findStrict("PARAMS", "ti.sysbios.gates");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.gates.GateHwi", "ti.sysbios.gates")).findStrict("PARAMS", "ti.sysbios.gates");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.gates.GateAll", "ti.sysbios.gates")).findStrict("PARAMS", "ti.sysbios.gates");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.gates.GateSwi", "ti.sysbios.gates")).findStrict("PARAMS", "ti.sysbios.gates");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.gates.GateTask", "ti.sysbios.gates")).findStrict("PARAMS", "ti.sysbios.gates");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.gates.GateTest", "ti.sysbios.gates")).findStrict("PARAMS", "ti.sysbios.gates");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.gates.GateMutexPri", "ti.sysbios.gates")).findStrict("PARAMS", "ti.sysbios.gates");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.gates.GateMutex", "ti.sysbios.gates")).findStrict("PARAMS", "ti.sysbios.gates");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.gates.GateSpinlock", "ti.sysbios.gates")).findStrict("PARAMS", "ti.sysbios.gates");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.gates.GateHwi", "ti.sysbios.gates"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.gates.GateAll", "ti.sysbios.gates"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.gates.GateSwi", "ti.sysbios.gates"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.gates.GateTask", "ti.sysbios.gates"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.gates.GateTest", "ti.sysbios.gates"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.gates.GateMutexPri", "ti.sysbios.gates"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.gates.GateMutex", "ti.sysbios.gates"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.gates.GateSpinlock", "ti.sysbios.gates"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateHwi", "ti.sysbios.gates");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateAll", "ti.sysbios.gates");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateSwi", "ti.sysbios.gates");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateTask", "ti.sysbios.gates");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateMutexPri", "ti.sysbios.gates");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Detailed", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitDetailed", "structName", "DetailedView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateMutex", "ti.sysbios.gates");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.gates.GateSpinlock", "ti.sysbios.gates");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.gates.GateHwi")).bless();
        ((Value.Obj)om.getv("ti.sysbios.gates.GateAll")).bless();
        ((Value.Obj)om.getv("ti.sysbios.gates.GateSwi")).bless();
        ((Value.Obj)om.getv("ti.sysbios.gates.GateTask")).bless();
        ((Value.Obj)om.getv("ti.sysbios.gates.GateTest")).bless();
        ((Value.Obj)om.getv("ti.sysbios.gates.GateMutexPri")).bless();
        ((Value.Obj)om.getv("ti.sysbios.gates.GateMutex")).bless();
        ((Value.Obj)om.getv("ti.sysbios.gates.GateSpinlock")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.gates")).add(pkgV);
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
        GateHwi$$OBJECTS();
        GateAll$$OBJECTS();
        GateSwi$$OBJECTS();
        GateTask$$OBJECTS();
        GateTest$$OBJECTS();
        GateMutexPri$$OBJECTS();
        GateMutex$$OBJECTS();
        GateSpinlock$$OBJECTS();
        GateHwi$$CONSTS();
        GateAll$$CONSTS();
        GateSwi$$CONSTS();
        GateTask$$CONSTS();
        GateTest$$CONSTS();
        GateMutexPri$$CONSTS();
        GateMutex$$CONSTS();
        GateSpinlock$$CONSTS();
        GateHwi$$CREATES();
        GateAll$$CREATES();
        GateSwi$$CREATES();
        GateTask$$CREATES();
        GateTest$$CREATES();
        GateMutexPri$$CREATES();
        GateMutex$$CREATES();
        GateSpinlock$$CREATES();
        GateHwi$$FUNCTIONS();
        GateAll$$FUNCTIONS();
        GateSwi$$FUNCTIONS();
        GateTask$$FUNCTIONS();
        GateTest$$FUNCTIONS();
        GateMutexPri$$FUNCTIONS();
        GateMutex$$FUNCTIONS();
        GateSpinlock$$FUNCTIONS();
        GateHwi$$SIZES();
        GateAll$$SIZES();
        GateSwi$$SIZES();
        GateTask$$SIZES();
        GateTest$$SIZES();
        GateMutexPri$$SIZES();
        GateMutex$$SIZES();
        GateSpinlock$$SIZES();
        GateHwi$$TYPES();
        GateAll$$TYPES();
        GateSwi$$TYPES();
        GateTask$$TYPES();
        GateTest$$TYPES();
        GateMutexPri$$TYPES();
        GateMutex$$TYPES();
        GateSpinlock$$TYPES();
        if (isROV) {
            GateHwi$$ROV();
            GateAll$$ROV();
            GateSwi$$ROV();
            GateTask$$ROV();
            GateTest$$ROV();
            GateMutexPri$$ROV();
            GateMutex$$ROV();
            GateSpinlock$$ROV();
        }//isROV
        $$SINGLETONS();
        GateHwi$$SINGLETONS();
        GateAll$$SINGLETONS();
        GateSwi$$SINGLETONS();
        GateTask$$SINGLETONS();
        GateTest$$SINGLETONS();
        GateMutexPri$$SINGLETONS();
        GateMutex$$SINGLETONS();
        GateSpinlock$$SINGLETONS();
        $$INITIALIZATION();
    }
}
