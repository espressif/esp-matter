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

public class ti_sysbios_family_arm_lm4
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
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.family.arm.m3");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.family.arm.lm4", new Value.Obj("ti.sysbios.family.arm.lm4", pkgP));
    }

    void Seconds$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Seconds.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.lm4.Seconds", new Value.Obj("ti.sysbios.family.arm.lm4.Seconds", po));
        pkgV.bind("Seconds", vo);
        // decls 
        om.bind("ti.sysbios.family.arm.lm4.Seconds.Time", om.findStrict("ti.sysbios.interfaces.ISeconds.Time", "ti.sysbios.family.arm.lm4"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Seconds$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.lm4.Seconds.Module_State", new Proto.Str(spo, false));
    }

    void TimestampProvider$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.TimestampProvider.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.lm4.TimestampProvider", new Value.Obj("ti.sysbios.family.arm.lm4.TimestampProvider", po));
        pkgV.bind("TimestampProvider", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.TimestampProvider$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.lm4.TimestampProvider.Module_State", new Proto.Str(spo, false));
    }

    void Timer$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Timer.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.lm4.Timer", new Value.Obj("ti.sysbios.family.arm.lm4.Timer", po));
        pkgV.bind("Timer", vo);
        // decls 
        om.bind("ti.sysbios.family.arm.lm4.Timer.StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.family.arm.lm4"));
        om.bind("ti.sysbios.family.arm.lm4.Timer.RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.family.arm.lm4"));
        om.bind("ti.sysbios.family.arm.lm4.Timer.Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.family.arm.lm4"));
        om.bind("ti.sysbios.family.arm.lm4.Timer.PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.family.arm.lm4"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Timer$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.lm4.Timer.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Timer$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.lm4.Timer.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Timer$$DeviceView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.lm4.Timer.DeviceView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Timer$$TimerDevice", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.lm4.Timer.TimerDevice", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Timer$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.lm4.Timer.Instance_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Timer$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.lm4.Timer.Module_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.family.arm.lm4.Timer.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Timer$$Object", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.lm4.Timer.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.lm4.Timer$$Params", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.lm4.Timer.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.family.arm.lm4.Timer.Object", om.findStrict("ti.sysbios.family.arm.lm4.Timer.Instance_State", "ti.sysbios.family.arm.lm4"));
        }//isROV
    }

    void Seconds$$CONSTS()
    {
        // module Seconds
        om.bind("ti.sysbios.family.arm.lm4.Seconds.get", new Extern("ti_sysbios_family_arm_lm4_Seconds_get__E", "xdc_UInt32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Seconds.getTime", new Extern("ti_sysbios_family_arm_lm4_Seconds_getTime__E", "xdc_UInt32(*)(ti_sysbios_interfaces_ISeconds_Time*)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Seconds.set", new Extern("ti_sysbios_family_arm_lm4_Seconds_set__E", "xdc_Void(*)(xdc_UInt32)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Seconds.setTime", new Extern("ti_sysbios_family_arm_lm4_Seconds_setTime__E", "xdc_UInt32(*)(ti_sysbios_interfaces_ISeconds_Time*)", true, false));
    }

    void TimestampProvider$$CONSTS()
    {
        // module TimestampProvider
        om.bind("ti.sysbios.family.arm.lm4.TimestampProvider.get32", new Extern("ti_sysbios_family_arm_lm4_TimestampProvider_get32__E", "xdc_Bits32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.TimestampProvider.get64", new Extern("ti_sysbios_family_arm_lm4_TimestampProvider_get64__E", "xdc_Void(*)(xdc_runtime_Types_Timestamp64*)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.TimestampProvider.getFreq", new Extern("ti_sysbios_family_arm_lm4_TimestampProvider_getFreq__E", "xdc_Void(*)(xdc_runtime_Types_FreqHz*)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.TimestampProvider.startTimer", new Extern("ti_sysbios_family_arm_lm4_TimestampProvider_startTimer__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.TimestampProvider.rolloverFunc", new Extern("ti_sysbios_family_arm_lm4_TimestampProvider_rolloverFunc__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.TimestampProvider.initTimerHandle", new Extern("ti_sysbios_family_arm_lm4_TimestampProvider_initTimerHandle__I", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void Timer$$CONSTS()
    {
        // module Timer
        om.bind("ti.sysbios.family.arm.lm4.Timer.MAX_PERIOD", 0xFFFFFFFFL);
        om.bind("ti.sysbios.family.arm.lm4.Timer.TIMER_CLOCK_DIVIDER", 1L);
        om.bind("ti.sysbios.family.arm.lm4.Timer.MIN_SWEEP_PERIOD", 1L);
        om.bind("ti.sysbios.family.arm.lm4.Timer.getNumTimers", new Extern("ti_sysbios_family_arm_lm4_Timer_getNumTimers__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.getStatus", new Extern("ti_sysbios_family_arm_lm4_Timer_getStatus__E", "ti_sysbios_interfaces_ITimer_Status(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.startup", new Extern("ti_sysbios_family_arm_lm4_Timer_startup__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.getHandle", new Extern("ti_sysbios_family_arm_lm4_Timer_getHandle__E", "ti_sysbios_family_arm_lm4_Timer_Handle(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.getAvailMask", new Extern("ti_sysbios_family_arm_lm4_Timer_getAvailMask__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.setAvailMask", new Extern("ti_sysbios_family_arm_lm4_Timer_setAvailMask__E", "xdc_Bool(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.isrStub", new Extern("ti_sysbios_family_arm_lm4_Timer_isrStub__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.enableCC26xx", new Extern("ti_sysbios_family_arm_lm4_Timer_enableCC26xx__I", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.enableCC3200", new Extern("ti_sysbios_family_arm_lm4_Timer_enableCC3200__I", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.disableCC26xx", new Extern("ti_sysbios_family_arm_lm4_Timer_disableCC26xx__I", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.disableCC3200", new Extern("ti_sysbios_family_arm_lm4_Timer_disableCC3200__I", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.enableTiva", new Extern("ti_sysbios_family_arm_lm4_Timer_enableTiva__I", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.disableTiva", new Extern("ti_sysbios_family_arm_lm4_Timer_disableTiva__I", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.initDevice", new Extern("ti_sysbios_family_arm_lm4_Timer_initDevice__I", "xdc_Void(*)(ti_sysbios_family_arm_lm4_Timer_Object*)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.postInit", new Extern("ti_sysbios_family_arm_lm4_Timer_postInit__I", "xdc_Int(*)(ti_sysbios_family_arm_lm4_Timer_Object*,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.enableTimers", new Extern("ti_sysbios_family_arm_lm4_Timer_enableTimers__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.masterDisable", new Extern("ti_sysbios_family_arm_lm4_Timer_masterDisable__I", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.masterEnable", new Extern("ti_sysbios_family_arm_lm4_Timer_masterEnable__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.lm4.Timer.write", new Extern("ti_sysbios_family_arm_lm4_Timer_write__I", "xdc_Void(*)(xdc_Bool,xdc_UInt32*,xdc_UInt32)", true, false));
    }

    void Seconds$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void TimestampProvider$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Timer$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$lm4$Timer$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.family.arm.lm4.Timer.create() called before xdc.useModule('ti.sysbios.family.arm.lm4.Timer')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.id, inst.$args.tickFxn, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.lm4.Timer$$create", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.lm4.Timer.Module", "ti.sysbios.family.arm.lm4"), om.findStrict("ti.sysbios.family.arm.lm4.Timer.Instance", "ti.sysbios.family.arm.lm4"), 3, 2, false));
                        fxn.addArg(0, "id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(1, "tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.lm4.Timer.Params", "ti.sysbios.family.arm.lm4"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$lm4$Timer$$create = function( id, tickFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.lm4.Timer'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.family.arm.lm4.Timer.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.family.arm.lm4']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {id:id, tickFxn:tickFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.family.arm.lm4.Timer'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [id, tickFxn]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.lm4.Timer'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.lm4.Timer$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.lm4.Timer.Module", "ti.sysbios.family.arm.lm4"), null, 4, 2, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$Object", "ti.sysbios.family.arm.lm4"), null);
                        fxn.addArg(1, "id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(2, "tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(3, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.lm4.Timer.Params", "ti.sysbios.family.arm.lm4"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$lm4$Timer$$construct = function( __obj, id, tickFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.lm4.Timer'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {id:id, tickFxn:tickFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.family.arm.lm4.Timer'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.lm4.Timer'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Seconds$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TimestampProvider$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Timer$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Seconds$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.lm4.Seconds.Module_State", "ti.sysbios.family.arm.lm4");
        sizes.clear();
        sizes.add(Global.newArray("setSecondsHi", "UInt32"));
        sizes.add(Global.newArray("setSeconds", "UInt32"));
        sizes.add(Global.newArray("refSeconds", "UInt32"));
        sizes.add(Global.newArray("deltaNSecs", "UInt32"));
        sizes.add(Global.newArray("deltaSecs", "TInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.lm4.Seconds.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.lm4.Seconds.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.lm4.Seconds.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void TimestampProvider$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.Module_State", "ti.sysbios.family.arm.lm4");
        sizes.clear();
        sizes.add(Global.newArray("timer", "UPtr"));
        sizes.add(Global.newArray("hi", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.lm4.TimestampProvider.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.lm4.TimestampProvider.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.lm4.TimestampProvider.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Timer$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.lm4.Timer.TimerDevice", "ti.sysbios.family.arm.lm4");
        sizes.clear();
        sizes.add(Global.newArray("intNum", "UInt"));
        sizes.add(Global.newArray("baseAddr", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.lm4.Timer.TimerDevice']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.lm4.Timer.TimerDevice']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.lm4.Timer.TimerDevice'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.lm4.Timer.Instance_State", "ti.sysbios.family.arm.lm4");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("staticInst", "UShort"));
        sizes.add(Global.newArray("id", "TInt"));
        sizes.add(Global.newArray("runMode", "Nti.sysbios.interfaces.ITimer.RunMode;;;;"));
        sizes.add(Global.newArray("startMode", "Nti.sysbios.interfaces.ITimer.StartMode;;;"));
        sizes.add(Global.newArray("period", "UInt"));
        sizes.add(Global.newArray("periodType", "Nti.sysbios.interfaces.ITimer.PeriodType;;;"));
        sizes.add(Global.newArray("intNum", "UInt"));
        sizes.add(Global.newArray("arg", "UIArg"));
        sizes.add(Global.newArray("tickFxn", "UFxn"));
        sizes.add(Global.newArray("extFreq", "Sxdc.runtime.Types;FreqHz"));
        sizes.add(Global.newArray("hwi", "UPtr"));
        sizes.add(Global.newArray("prevThreshold", "UInt"));
        sizes.add(Global.newArray("rollovers", "UInt"));
        sizes.add(Global.newArray("savedCurrCount", "UInt"));
        sizes.add(Global.newArray("altclk", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.lm4.Timer.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.lm4.Timer.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.lm4.Timer.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.lm4.Timer.Module_State", "ti.sysbios.family.arm.lm4");
        sizes.clear();
        sizes.add(Global.newArray("availMask", "UInt"));
        sizes.add(Global.newArray("device", "UPtr"));
        sizes.add(Global.newArray("handles", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.lm4.Timer.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.lm4.Timer.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.lm4.Timer.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Seconds$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/lm4/Seconds.xs");
        om.bind("ti.sysbios.family.arm.lm4.Seconds$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Seconds.Module", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Seconds.Module", om.findStrict("ti.sysbios.interfaces.ISeconds.Module", "ti.sysbios.family.arm.lm4"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.Seconds$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.Seconds$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.Seconds$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.Seconds$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct Seconds.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Seconds$$Module_State", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Seconds.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("setSecondsHi", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("setSeconds", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("refSeconds", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("deltaNSecs", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("deltaSecs", Proto.Elm.newCNum("(xdc_Int32)"), $$UNDEF, "w");
    }

    void TimestampProvider$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/lm4/TimestampProvider.xs");
        om.bind("ti.sysbios.family.arm.lm4.TimestampProvider$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.Module", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.TimestampProvider.Module", om.findStrict("ti.sysbios.interfaces.ITimestamp.Module", "ti.sysbios.family.arm.lm4"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("useClockTimer", $$T_Bool, false, "w");
            po.addFld("timerId", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.TimestampProvider$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.TimestampProvider$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.TimestampProvider$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.TimestampProvider$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "getFreqMeta");
                if (fxn != null) po.addFxn("getFreqMeta", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimestamp$$getFreqMeta", "ti.sysbios.family.arm.lm4"), fxn);
        // struct TimestampProvider.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider$$Module_State", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.TimestampProvider.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("timer", (Proto)om.findStrict("ti.sysbios.family.arm.lm4.Timer.Handle", "ti.sysbios.family.arm.lm4"), $$UNDEF, "w");
                po.addFld("hi", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
    }

    void Timer$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/lm4/Timer.xs");
        om.bind("ti.sysbios.family.arm.lm4.Timer$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer.Module", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Timer.Module", om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.family.arm.lm4"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
                po.addFld("MAX_PERIOD", Proto.Elm.newCNum("(xdc_UInt)"), 0xFFFFFFFFL, "rh");
                po.addFld("TIMER_CLOCK_DIVIDER", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("MIN_SWEEP_PERIOD", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
        if (isCFG) {
            po.addFld("supportsDynamic", $$T_Bool, true, "wh");
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.family.arm.lm4"), $$UNDEF, "wh");
            po.addFld("E_invalidTimer", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.lm4"), Global.newObject("msg", "E_invalidTimer: Invalid Timer Id %d"), "w");
            po.addFld("E_notAvailable", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.lm4"), Global.newObject("msg", "E_notAvailable: Timer not available %d"), "w");
            po.addFld("E_cannotSupport", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.lm4"), Global.newObject("msg", "E_cannotSupport: Timer cannot support requested period %d"), "w");
            po.addFld("E_noaltclk", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.lm4"), Global.newObject("msg", "E_noaltclk: Timer does not support altclk"), "w");
            po.addFld("anyMask", Proto.Elm.newCNum("(xdc_UInt)"), 0x3FL, "w");
            po.addFld("supportsAltclk", $$T_Bool, true, "w");
            po.addFld("enableFunc", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"), null, "w");
            po.addFld("disableFunc", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"), null, "w");
            po.addFld("startupNeeded", Proto.Elm.newCNum("(xdc_UInt)"), false, "w");
            po.addFld("numTimerDevices", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$create", "ti.sysbios.family.arm.lm4"), Global.get("ti$sysbios$family$arm$lm4$Timer$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$construct", "ti.sysbios.family.arm.lm4"), Global.get("ti$sysbios$family$arm$lm4$Timer$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.Timer$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.Timer$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.Timer$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.Timer$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.Timer$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.lm4.Timer$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "viewGetCurrentClockTick");
                if (fxn != null) po.addFxn("viewGetCurrentClockTick", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimer$$viewGetCurrentClockTick", "ti.sysbios.family.arm.lm4"), fxn);
                fxn = Global.get(cap, "getFreqMeta");
                if (fxn != null) po.addFxn("getFreqMeta", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimer$$getFreqMeta", "ti.sysbios.family.arm.lm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer.Instance", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Timer.Instance", om.findStrict("ti.sysbios.interfaces.ITimer.Instance", "ti.sysbios.family.arm.lm4"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
                po.addFld("MAX_PERIOD", Proto.Elm.newCNum("(xdc_UInt)"), 0xFFFFFFFFL, "rh");
                po.addFld("TIMER_CLOCK_DIVIDER", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("MIN_SWEEP_PERIOD", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
        if (isCFG) {
            po.addFld("hwiParams", new Proto.Adr("ti_sysbios_family_arm_m3_Hwi_Params*", "PS"), null, "w");
            po.addFld("prevThreshold", Proto.Elm.newCNum("(xdc_UInt)"), 0xFFFFFFFFL, "w");
            po.addFld("altclk", $$T_Bool, false, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.lm4"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$Params", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Timer.Params", om.findStrict("ti.sysbios.interfaces.ITimer$$Params", "ti.sysbios.family.arm.lm4"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
                po.addFld("MAX_PERIOD", Proto.Elm.newCNum("(xdc_UInt)"), 0xFFFFFFFFL, "rh");
                po.addFld("TIMER_CLOCK_DIVIDER", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("MIN_SWEEP_PERIOD", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
        if (isCFG) {
            po.addFld("hwiParams", new Proto.Adr("ti_sysbios_family_arm_m3_Hwi_Params*", "PS"), null, "w");
            po.addFld("prevThreshold", Proto.Elm.newCNum("(xdc_UInt)"), 0xFFFFFFFFL, "w");
            po.addFld("altclk", $$T_Bool, false, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.lm4"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$Object", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Timer.Object", om.findStrict("ti.sysbios.family.arm.lm4.Timer.Instance", "ti.sysbios.family.arm.lm4"));
        // struct Timer.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$BasicView", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Timer.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("halTimerHandle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("id", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("device", $$T_Str, $$UNDEF, "w");
                po.addFld("startMode", $$T_Str, $$UNDEF, "w");
                po.addFld("runMode", $$T_Str, $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("periodType", $$T_Str, $$UNDEF, "w");
                po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("tickFxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("extFreq", $$T_Str, $$UNDEF, "w");
                po.addFld("hwiHandle", $$T_Str, $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // struct Timer.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$ModuleView", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Timer.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("availMask", $$T_Str, $$UNDEF, "w");
        // struct Timer.DeviceView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$DeviceView", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Timer.DeviceView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("id", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("device", $$T_Str, $$UNDEF, "w");
                po.addFld("devAddr", $$T_Str, $$UNDEF, "w");
                po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("runMode", $$T_Str, $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("currCount", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("remainingCount", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("prevThreshold", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("altclk", $$T_Bool, $$UNDEF, "w");
                po.addFld("state", $$T_Str, $$UNDEF, "w");
        // typedef Timer.TimerEnableFuncPtr
        om.bind("ti.sysbios.family.arm.lm4.Timer.TimerEnableFuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"));
        // typedef Timer.TimerDisableFuncPtr
        om.bind("ti.sysbios.family.arm.lm4.Timer.TimerDisableFuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"));
        // struct Timer.TimerDevice
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$TimerDevice", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Timer.TimerDevice", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("baseAddr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        // struct Timer.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$Instance_State", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Timer.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("staticInst", $$T_Bool, $$UNDEF, "w");
                po.addFld("id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("runMode", (Proto)om.findStrict("ti.sysbios.family.arm.lm4.Timer.RunMode", "ti.sysbios.family.arm.lm4"), $$UNDEF, "w");
                po.addFld("startMode", (Proto)om.findStrict("ti.sysbios.family.arm.lm4.Timer.StartMode", "ti.sysbios.family.arm.lm4"), $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("periodType", (Proto)om.findStrict("ti.sysbios.family.arm.lm4.Timer.PeriodType", "ti.sysbios.family.arm.lm4"), $$UNDEF, "w");
                po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF, "w");
                po.addFld("extFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios.family.arm.lm4"), $$DEFAULT, "w");
                po.addFld("hwi", (Proto)om.findStrict("ti.sysbios.family.arm.m3.Hwi.Handle", "ti.sysbios.family.arm.lm4"), $$UNDEF, "w");
                po.addFld("prevThreshold", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("rollovers", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("savedCurrCount", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("altclk", $$T_Bool, $$UNDEF, "w");
        // struct Timer.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$Module_State", "ti.sysbios.family.arm.lm4");
        po.init("ti.sysbios.family.arm.lm4.Timer.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("availMask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("device", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.arm.lm4.Timer.TimerDevice", "ti.sysbios.family.arm.lm4"), false), $$DEFAULT, "w");
                po.addFld("handles", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.arm.lm4.Timer.Handle", "ti.sysbios.family.arm.lm4"), false), $$DEFAULT, "w");
    }

    void Seconds$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Seconds", "ti.sysbios.family.arm.lm4");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.lm4.Seconds.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Seconds$$Module_State", "ti.sysbios.family.arm.lm4");
    }

    void TimestampProvider$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider", "ti.sysbios.family.arm.lm4");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.lm4.TimestampProvider.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider$$Module_State", "ti.sysbios.family.arm.lm4");
    }

    void Timer$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer", "ti.sysbios.family.arm.lm4");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$Instance_State", "ti.sysbios.family.arm.lm4");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("TimerDevice$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.lm4.Timer.TimerDevice", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$TimerDevice", "ti.sysbios.family.arm.lm4");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.lm4.Timer.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$Instance_State", "ti.sysbios.family.arm.lm4");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.lm4.Timer.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer$$Module_State", "ti.sysbios.family.arm.lm4");
        po.bind("device$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.lm4.Timer.TimerDevice", "isScalar", false));
        po.bind("handles$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.family.arm.lm4.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.family.arm.lm4"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/lm4/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.family.arm.lm4"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.family.arm.lm4"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.family.arm.lm4"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.family.arm.lm4"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.family.arm.lm4"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.family.arm.lm4"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.family.arm.lm4", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.family.arm.lm4");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.family.arm.lm4.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.family.arm.lm4'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.family.arm.lm4$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.family.arm.lm4$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.family.arm.lm4$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.family.arm.lm4.aem4',\n");
            sb.append("'lib/debug/ti.sysbios.family.arm.lm4.am4',\n");
            sb.append("'lib/debug/ti.sysbios.family.arm.lm4.am4g',\n");
            sb.append("'lib/debug/ti.sysbios.family.arm.lm4.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.family.arm.lm4.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/debug/ti.sysbios.family.arm.lm4.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/debug/ti.sysbios.family.arm.lm4.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/debug/ti.sysbios.family.arm.lm4.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Seconds$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Seconds", "ti.sysbios.family.arm.lm4");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Seconds.Module", "ti.sysbios.family.arm.lm4");
        vo.init2(po, "ti.sysbios.family.arm.lm4.Seconds", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.lm4.Seconds$$capsule", "ti.sysbios.family.arm.lm4"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.lm4", "ti.sysbios.family.arm.lm4"));
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
        vo.bind("Time", om.findStrict("ti.sysbios.interfaces.ISeconds.Time", "ti.sysbios.family.arm.lm4"));
        tdefs.add(om.findStrict("ti.sysbios.interfaces.ISeconds.Time", "ti.sysbios.family.arm.lm4"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.arm.lm4.Seconds.Module_State", "ti.sysbios.family.arm.lm4"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.lm4.Seconds.Module_State", "ti.sysbios.family.arm.lm4"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.lm4")).add(vo);
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
        vo.bind("get", om.findStrict("ti.sysbios.family.arm.lm4.Seconds.get", "ti.sysbios.family.arm.lm4"));
        vo.bind("getTime", om.findStrict("ti.sysbios.family.arm.lm4.Seconds.getTime", "ti.sysbios.family.arm.lm4"));
        vo.bind("set", om.findStrict("ti.sysbios.family.arm.lm4.Seconds.set", "ti.sysbios.family.arm.lm4"));
        vo.bind("setTime", om.findStrict("ti.sysbios.family.arm.lm4.Seconds.setTime", "ti.sysbios.family.arm.lm4"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_lm4_Seconds_Module__startupDone__E", "ti_sysbios_family_arm_lm4_Seconds_get__E", "ti_sysbios_family_arm_lm4_Seconds_getTime__E", "ti_sysbios_family_arm_lm4_Seconds_set__E", "ti_sysbios_family_arm_lm4_Seconds_setTime__E"));
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
        pkgV.bind("Seconds", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Seconds");
    }

    void TimestampProvider$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider", "ti.sysbios.family.arm.lm4");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.Module", "ti.sysbios.family.arm.lm4");
        vo.init2(po, "ti.sysbios.family.arm.lm4.TimestampProvider", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider$$capsule", "ti.sysbios.family.arm.lm4"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.lm4", "ti.sysbios.family.arm.lm4"));
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
        mcfgs.add("useClockTimer");
        mcfgs.add("timerId");
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.Module_State", "ti.sysbios.family.arm.lm4"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.Module_State", "ti.sysbios.family.arm.lm4"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.lm4")).add(vo);
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
        vo.bind("get32", om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.get32", "ti.sysbios.family.arm.lm4"));
        vo.bind("get64", om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.get64", "ti.sysbios.family.arm.lm4"));
        vo.bind("getFreq", om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.getFreq", "ti.sysbios.family.arm.lm4"));
        vo.bind("startTimer", om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.startTimer", "ti.sysbios.family.arm.lm4"));
        vo.bind("rolloverFunc", om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.rolloverFunc", "ti.sysbios.family.arm.lm4"));
        vo.bind("initTimerHandle", om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider.initTimerHandle", "ti.sysbios.family.arm.lm4"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_lm4_TimestampProvider_Module__startupDone__E", "ti_sysbios_family_arm_lm4_TimestampProvider_get32__E", "ti_sysbios_family_arm_lm4_TimestampProvider_get64__E", "ti_sysbios_family_arm_lm4_TimestampProvider_getFreq__E", "ti_sysbios_family_arm_lm4_TimestampProvider_startTimer__E", "ti_sysbios_family_arm_lm4_TimestampProvider_rolloverFunc__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("TimestampProvider", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TimestampProvider");
    }

    void Timer$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer", "ti.sysbios.family.arm.lm4");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer.Module", "ti.sysbios.family.arm.lm4");
        vo.init2(po, "ti.sysbios.family.arm.lm4.Timer", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.lm4.Timer$$capsule", "ti.sysbios.family.arm.lm4"));
        vo.bind("Instance", om.findStrict("ti.sysbios.family.arm.lm4.Timer.Instance", "ti.sysbios.family.arm.lm4"));
        vo.bind("Params", om.findStrict("ti.sysbios.family.arm.lm4.Timer.Params", "ti.sysbios.family.arm.lm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.family.arm.lm4.Timer.Params", "ti.sysbios.family.arm.lm4")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.family.arm.lm4.Timer.Handle", "ti.sysbios.family.arm.lm4"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.lm4", "ti.sysbios.family.arm.lm4"));
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
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.interfaces.ITimer.FuncPtr", "ti.sysbios.family.arm.lm4"));
        vo.bind("StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.family.arm.lm4"));
        vo.bind("RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.family.arm.lm4"));
        vo.bind("Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.family.arm.lm4"));
        vo.bind("PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.family.arm.lm4"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.family.arm.lm4.Timer.BasicView", "ti.sysbios.family.arm.lm4"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.lm4.Timer.BasicView", "ti.sysbios.family.arm.lm4"));
        vo.bind("ModuleView", om.findStrict("ti.sysbios.family.arm.lm4.Timer.ModuleView", "ti.sysbios.family.arm.lm4"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.lm4.Timer.ModuleView", "ti.sysbios.family.arm.lm4"));
        vo.bind("DeviceView", om.findStrict("ti.sysbios.family.arm.lm4.Timer.DeviceView", "ti.sysbios.family.arm.lm4"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.lm4.Timer.DeviceView", "ti.sysbios.family.arm.lm4"));
        vo.bind("TimerEnableFuncPtr", om.findStrict("ti.sysbios.family.arm.lm4.Timer.TimerEnableFuncPtr", "ti.sysbios.family.arm.lm4"));
        vo.bind("TimerDisableFuncPtr", om.findStrict("ti.sysbios.family.arm.lm4.Timer.TimerDisableFuncPtr", "ti.sysbios.family.arm.lm4"));
        mcfgs.add("E_invalidTimer");
        mcfgs.add("E_notAvailable");
        mcfgs.add("E_cannotSupport");
        mcfgs.add("E_noaltclk");
        mcfgs.add("anyMask");
        mcfgs.add("supportsAltclk");
        mcfgs.add("enableFunc");
        mcfgs.add("disableFunc");
        mcfgs.add("startupNeeded");
        icfgs.add("startupNeeded");
        vo.bind("TimerDevice", om.findStrict("ti.sysbios.family.arm.lm4.Timer.TimerDevice", "ti.sysbios.family.arm.lm4"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.lm4.Timer.TimerDevice", "ti.sysbios.family.arm.lm4"));
        mcfgs.add("numTimerDevices");
        icfgs.add("numTimerDevices");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.family.arm.lm4.Timer.Instance_State", "ti.sysbios.family.arm.lm4"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.lm4.Timer.Instance_State", "ti.sysbios.family.arm.lm4"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.arm.lm4.Timer.Module_State", "ti.sysbios.family.arm.lm4"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.lm4.Timer.Module_State", "ti.sysbios.family.arm.lm4"));
        vo.bind("StartMode_AUTO", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode_AUTO", "ti.sysbios.family.arm.lm4"));
        vo.bind("StartMode_USER", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode_USER", "ti.sysbios.family.arm.lm4"));
        vo.bind("RunMode_CONTINUOUS", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS", "ti.sysbios.family.arm.lm4"));
        vo.bind("RunMode_ONESHOT", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_ONESHOT", "ti.sysbios.family.arm.lm4"));
        vo.bind("RunMode_DYNAMIC", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_DYNAMIC", "ti.sysbios.family.arm.lm4"));
        vo.bind("Status_INUSE", om.findStrict("ti.sysbios.interfaces.ITimer.Status_INUSE", "ti.sysbios.family.arm.lm4"));
        vo.bind("Status_FREE", om.findStrict("ti.sysbios.interfaces.ITimer.Status_FREE", "ti.sysbios.family.arm.lm4"));
        vo.bind("PeriodType_MICROSECS", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS", "ti.sysbios.family.arm.lm4"));
        vo.bind("PeriodType_COUNTS", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType_COUNTS", "ti.sysbios.family.arm.lm4"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.lm4")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.family.arm.lm4.Timer$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$family$arm$lm4$Timer$$__initObject"));
        }//isCFG
        vo.bind("getNumTimers", om.findStrict("ti.sysbios.family.arm.lm4.Timer.getNumTimers", "ti.sysbios.family.arm.lm4"));
        vo.bind("getStatus", om.findStrict("ti.sysbios.family.arm.lm4.Timer.getStatus", "ti.sysbios.family.arm.lm4"));
        vo.bind("startup", om.findStrict("ti.sysbios.family.arm.lm4.Timer.startup", "ti.sysbios.family.arm.lm4"));
        vo.bind("getHandle", om.findStrict("ti.sysbios.family.arm.lm4.Timer.getHandle", "ti.sysbios.family.arm.lm4"));
        vo.bind("getAvailMask", om.findStrict("ti.sysbios.family.arm.lm4.Timer.getAvailMask", "ti.sysbios.family.arm.lm4"));
        vo.bind("setAvailMask", om.findStrict("ti.sysbios.family.arm.lm4.Timer.setAvailMask", "ti.sysbios.family.arm.lm4"));
        vo.bind("isrStub", om.findStrict("ti.sysbios.family.arm.lm4.Timer.isrStub", "ti.sysbios.family.arm.lm4"));
        vo.bind("enableCC26xx", om.findStrict("ti.sysbios.family.arm.lm4.Timer.enableCC26xx", "ti.sysbios.family.arm.lm4"));
        vo.bind("enableCC3200", om.findStrict("ti.sysbios.family.arm.lm4.Timer.enableCC3200", "ti.sysbios.family.arm.lm4"));
        vo.bind("disableCC26xx", om.findStrict("ti.sysbios.family.arm.lm4.Timer.disableCC26xx", "ti.sysbios.family.arm.lm4"));
        vo.bind("disableCC3200", om.findStrict("ti.sysbios.family.arm.lm4.Timer.disableCC3200", "ti.sysbios.family.arm.lm4"));
        vo.bind("enableTiva", om.findStrict("ti.sysbios.family.arm.lm4.Timer.enableTiva", "ti.sysbios.family.arm.lm4"));
        vo.bind("disableTiva", om.findStrict("ti.sysbios.family.arm.lm4.Timer.disableTiva", "ti.sysbios.family.arm.lm4"));
        vo.bind("initDevice", om.findStrict("ti.sysbios.family.arm.lm4.Timer.initDevice", "ti.sysbios.family.arm.lm4"));
        vo.bind("postInit", om.findStrict("ti.sysbios.family.arm.lm4.Timer.postInit", "ti.sysbios.family.arm.lm4"));
        vo.bind("enableTimers", om.findStrict("ti.sysbios.family.arm.lm4.Timer.enableTimers", "ti.sysbios.family.arm.lm4"));
        vo.bind("masterDisable", om.findStrict("ti.sysbios.family.arm.lm4.Timer.masterDisable", "ti.sysbios.family.arm.lm4"));
        vo.bind("masterEnable", om.findStrict("ti.sysbios.family.arm.lm4.Timer.masterEnable", "ti.sysbios.family.arm.lm4"));
        vo.bind("write", om.findStrict("ti.sysbios.family.arm.lm4.Timer.write", "ti.sysbios.family.arm.lm4"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_lm4_Timer_Handle__label__E", "ti_sysbios_family_arm_lm4_Timer_Module__startupDone__E", "ti_sysbios_family_arm_lm4_Timer_Object__create__E", "ti_sysbios_family_arm_lm4_Timer_Object__delete__E", "ti_sysbios_family_arm_lm4_Timer_Object__get__E", "ti_sysbios_family_arm_lm4_Timer_Object__first__E", "ti_sysbios_family_arm_lm4_Timer_Object__next__E", "ti_sysbios_family_arm_lm4_Timer_Params__init__E", "ti_sysbios_family_arm_lm4_Timer_getNumTimers__E", "ti_sysbios_family_arm_lm4_Timer_getStatus__E", "ti_sysbios_family_arm_lm4_Timer_startup__E", "ti_sysbios_family_arm_lm4_Timer_getMaxTicks__E", "ti_sysbios_family_arm_lm4_Timer_setNextTick__E", "ti_sysbios_family_arm_lm4_Timer_start__E", "ti_sysbios_family_arm_lm4_Timer_stop__E", "ti_sysbios_family_arm_lm4_Timer_setPeriod__E", "ti_sysbios_family_arm_lm4_Timer_setPeriodMicroSecs__E", "ti_sysbios_family_arm_lm4_Timer_getPeriod__E", "ti_sysbios_family_arm_lm4_Timer_getCount__E", "ti_sysbios_family_arm_lm4_Timer_getFreq__E", "ti_sysbios_family_arm_lm4_Timer_getFunc__E", "ti_sysbios_family_arm_lm4_Timer_setFunc__E", "ti_sysbios_family_arm_lm4_Timer_trigger__E", "ti_sysbios_family_arm_lm4_Timer_getExpiredCounts__E", "ti_sysbios_family_arm_lm4_Timer_getExpiredTicks__E", "ti_sysbios_family_arm_lm4_Timer_getCurrentTick__E", "ti_sysbios_family_arm_lm4_Timer_getHandle__E", "ti_sysbios_family_arm_lm4_Timer_getAvailMask__E", "ti_sysbios_family_arm_lm4_Timer_setAvailMask__E", "ti_sysbios_family_arm_lm4_Timer_isrStub__E", "ti_sysbios_family_arm_lm4_Timer_reconfig__E", "ti_sysbios_family_arm_lm4_Timer_getExpiredCounts64__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray("E_invalidTimer", "E_notAvailable", "E_cannotSupport", "E_noaltclk"));
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.family.arm.lm4.Timer.Object", "ti.sysbios.family.arm.lm4"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Timer", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Timer");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.sysbios.family.arm.lm4")).findStrict("PARAMS", "ti.sysbios.family.arm.lm4");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.family.arm.lm4.Timer", "ti.sysbios.family.arm.lm4")).findStrict("PARAMS", "ti.sysbios.family.arm.lm4");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.lm4.Seconds", "ti.sysbios.family.arm.lm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.lm4.TimestampProvider", "ti.sysbios.family.arm.lm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.lm4.Timer", "ti.sysbios.family.arm.lm4"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.lm4.Timer", "ti.sysbios.family.arm.lm4");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Device", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitDevice", "structName", "DeviceView")}), Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.family.arm.lm4.Seconds")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.lm4.TimestampProvider")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.lm4.Timer")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.family.arm.lm4")).add(pkgV);
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
        Seconds$$OBJECTS();
        TimestampProvider$$OBJECTS();
        Timer$$OBJECTS();
        Seconds$$CONSTS();
        TimestampProvider$$CONSTS();
        Timer$$CONSTS();
        Seconds$$CREATES();
        TimestampProvider$$CREATES();
        Timer$$CREATES();
        Seconds$$FUNCTIONS();
        TimestampProvider$$FUNCTIONS();
        Timer$$FUNCTIONS();
        Seconds$$SIZES();
        TimestampProvider$$SIZES();
        Timer$$SIZES();
        Seconds$$TYPES();
        TimestampProvider$$TYPES();
        Timer$$TYPES();
        if (isROV) {
            Seconds$$ROV();
            TimestampProvider$$ROV();
            Timer$$ROV();
        }//isROV
        $$SINGLETONS();
        Seconds$$SINGLETONS();
        TimestampProvider$$SINGLETONS();
        Timer$$SINGLETONS();
        $$INITIALIZATION();
    }
}
