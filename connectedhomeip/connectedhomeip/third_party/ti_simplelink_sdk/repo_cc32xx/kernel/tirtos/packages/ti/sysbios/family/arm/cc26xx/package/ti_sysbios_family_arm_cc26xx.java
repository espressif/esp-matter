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

public class ti_sysbios_family_arm_cc26xx
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
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.interfaces");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.family.arm.m3");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.family.arm.cc26xx", new Value.Obj("ti.sysbios.family.arm.cc26xx", pkgP));
    }

    void Alarm$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Alarm.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Alarm", new Value.Obj("ti.sysbios.family.arm.cc26xx.Alarm", po));
        pkgV.bind("Alarm", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Alarm.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Alarm.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.family.arm.cc26xx.Alarm.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$Object", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Alarm.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$Params", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Alarm.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Alarm.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.family.arm.cc26xx.Alarm.Object", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Instance_State", "ti.sysbios.family.arm.cc26xx"));
        }//isROV
    }

    void Boot$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Boot.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Boot", new Value.Obj("ti.sysbios.family.arm.cc26xx.Boot", po));
        pkgV.bind("Boot", vo);
        // decls 
    }

    void Power$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Power.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Power", new Value.Obj("ti.sysbios.family.arm.cc26xx.Power", po));
        pkgV.bind("Power", vo);
        // decls 
    }

    void Seconds$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Seconds.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Seconds", new Value.Obj("ti.sysbios.family.arm.cc26xx.Seconds", po));
        pkgV.bind("Seconds", vo);
        // decls 
        om.bind("ti.sysbios.family.arm.cc26xx.Seconds.Time", om.findStrict("ti.sysbios.interfaces.ISeconds.Time", "ti.sysbios.family.arm.cc26xx"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Seconds$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Seconds.Module_State", new Proto.Str(spo, false));
    }

    void Timer$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Timer.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Timer", new Value.Obj("ti.sysbios.family.arm.cc26xx.Timer", po));
        pkgV.bind("Timer", vo);
        // decls 
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.family.arm.cc26xx"));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.family.arm.cc26xx"));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.family.arm.cc26xx"));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.family.arm.cc26xx"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Timer$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Timer$$DeviceView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.DeviceView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Timer$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Timer$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.Instance_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Timer$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.Module_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.family.arm.cc26xx.Timer.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Timer$$Object", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.Timer$$Params", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.family.arm.cc26xx.Timer.Object", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Instance_State", "ti.sysbios.family.arm.cc26xx"));
        }//isROV
    }

    void TimestampProvider$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider", new Value.Obj("ti.sysbios.family.arm.cc26xx.TimestampProvider", po));
        pkgV.bind("TimestampProvider", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider.Module_State", new Proto.Str(spo, false));
    }

    void Alarm$$CONSTS()
    {
        // module Alarm
        om.bind("ti.sysbios.family.arm.cc26xx.Alarm.getCount", new Extern("ti_sysbios_family_arm_cc26xx_Alarm_getCount__E", "xdc_UInt64(*)(xdc_Void)", true, false));
    }

    void Boot$$CONSTS()
    {
        // module Boot
        om.bind("ti.sysbios.family.arm.cc26xx.Boot.getBootReason", new Extern("ti_sysbios_family_arm_cc26xx_Boot_getBootReason__E", "xdc_UInt32(*)(xdc_Void)", true, false));
    }

    void Power$$CONSTS()
    {
        // module Power
    }

    void Seconds$$CONSTS()
    {
        // module Seconds
        om.bind("ti.sysbios.family.arm.cc26xx.Seconds.get", new Extern("ti_sysbios_family_arm_cc26xx_Seconds_get__E", "xdc_UInt32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Seconds.getTime", new Extern("ti_sysbios_family_arm_cc26xx_Seconds_getTime__E", "xdc_UInt32(*)(ti_sysbios_interfaces_ISeconds_Time*)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Seconds.set", new Extern("ti_sysbios_family_arm_cc26xx_Seconds_set__E", "xdc_Void(*)(xdc_UInt32)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Seconds.setTime", new Extern("ti_sysbios_family_arm_cc26xx_Seconds_setTime__E", "xdc_UInt32(*)(ti_sysbios_interfaces_ISeconds_Time*)", true, false));
    }

    void Timer$$CONSTS()
    {
        // module Timer
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.MAX_PERIOD", 0xFFFFFFFFL);
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.MIN_SWEEP_PERIOD", 1L);
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.getNumTimers", new Extern("ti_sysbios_family_arm_cc26xx_Timer_getNumTimers__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.getStatus", new Extern("ti_sysbios_family_arm_cc26xx_Timer_getStatus__E", "ti_sysbios_interfaces_ITimer_Status(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.startup", new Extern("ti_sysbios_family_arm_cc26xx_Timer_startup__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.dynamicStub", new Extern("ti_sysbios_family_arm_cc26xx_Timer_dynamicStub__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.dynamicMultiStub", new Extern("ti_sysbios_family_arm_cc26xx_Timer_dynamicMultiStub__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.periodicStub", new Extern("ti_sysbios_family_arm_cc26xx_Timer_periodicStub__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.getCount64", new Extern("ti_sysbios_family_arm_cc26xx_Timer_getCount64__E", "xdc_UInt64(*)(ti_sysbios_family_arm_cc26xx_Timer_Object*)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.getExpiredCounts64", new Extern("ti_sysbios_family_arm_cc26xx_Timer_getExpiredCounts64__E", "xdc_UInt64(*)(ti_sysbios_family_arm_cc26xx_Timer_Object*)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.getHandle", new Extern("ti_sysbios_family_arm_cc26xx_Timer_getHandle__E", "ti_sysbios_family_arm_cc26xx_Timer_Handle(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.initDevice", new Extern("ti_sysbios_family_arm_cc26xx_Timer_initDevice__I", "xdc_Void(*)(ti_sysbios_family_arm_cc26xx_Timer_Object*)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.postInit", new Extern("ti_sysbios_family_arm_cc26xx_Timer_postInit__I", "xdc_Int(*)(ti_sysbios_family_arm_cc26xx_Timer_Object*,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.Timer.setThreshold", new Extern("ti_sysbios_family_arm_cc26xx_Timer_setThreshold__I", "xdc_Void(*)(ti_sysbios_family_arm_cc26xx_Timer_Object*,xdc_UInt32,xdc_Bool)", true, false));
    }

    void TimestampProvider$$CONSTS()
    {
        // module TimestampProvider
        om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider.get32", new Extern("ti_sysbios_family_arm_cc26xx_TimestampProvider_get32__E", "xdc_Bits32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider.get64", new Extern("ti_sysbios_family_arm_cc26xx_TimestampProvider_get64__E", "xdc_Void(*)(xdc_runtime_Types_Timestamp64*)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider.getFreq", new Extern("ti_sysbios_family_arm_cc26xx_TimestampProvider_getFreq__E", "xdc_Void(*)(xdc_runtime_Types_FreqHz*)", true, false));
        om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider.startTimer", new Extern("ti_sysbios_family_arm_cc26xx_TimestampProvider_startTimer__E", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void Alarm$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$cc26xx$Alarm$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.family.arm.cc26xx.Alarm.create() called before xdc.useModule('ti.sysbios.family.arm.cc26xx.Alarm')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.alarmFxn, inst.$args.arg, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$create", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Module", "ti.sysbios.family.arm.cc26xx"), om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Instance", "ti.sysbios.family.arm.cc26xx"), 3, 2, false));
                        fxn.addArg(0, "alarmFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(1, "arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Params", "ti.sysbios.family.arm.cc26xx"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$cc26xx$Alarm$$create = function( alarmFxn, arg, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.cc26xx.Alarm'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.family.arm.cc26xx.Alarm.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.family.arm.cc26xx']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {alarmFxn:alarmFxn, arg:arg});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.family.arm.cc26xx.Alarm'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [alarmFxn, arg]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.cc26xx.Alarm'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Module", "ti.sysbios.family.arm.cc26xx"), null, 4, 2, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm$$Object", "ti.sysbios.family.arm.cc26xx"), null);
                        fxn.addArg(1, "alarmFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF);
                        fxn.addArg(3, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Params", "ti.sysbios.family.arm.cc26xx"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$cc26xx$Alarm$$construct = function( __obj, alarmFxn, arg, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.cc26xx.Alarm'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {alarmFxn:alarmFxn, arg:arg});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.family.arm.cc26xx.Alarm'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.cc26xx.Alarm'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Boot$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Power$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Seconds$$CREATES()
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
            sb.append("ti$sysbios$family$arm$cc26xx$Timer$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.family.arm.cc26xx.Timer.create() called before xdc.useModule('ti.sysbios.family.arm.cc26xx.Timer')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.id, inst.$args.tickFxn, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.cc26xx.Timer$$create", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Module", "ti.sysbios.family.arm.cc26xx"), om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Instance", "ti.sysbios.family.arm.cc26xx"), 3, 2, false));
                        fxn.addArg(0, "id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(1, "tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Params", "ti.sysbios.family.arm.cc26xx"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$cc26xx$Timer$$create = function( id, tickFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.cc26xx.Timer'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.family.arm.cc26xx.Timer.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.family.arm.cc26xx']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {id:id, tickFxn:tickFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.family.arm.cc26xx.Timer'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [id, tickFxn]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.cc26xx.Timer'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.cc26xx.Timer$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Module", "ti.sysbios.family.arm.cc26xx"), null, 4, 2, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$Object", "ti.sysbios.family.arm.cc26xx"), null);
                        fxn.addArg(1, "id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(2, "tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(3, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Params", "ti.sysbios.family.arm.cc26xx"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$cc26xx$Timer$$construct = function( __obj, id, tickFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.cc26xx.Timer'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {id:id, tickFxn:tickFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.family.arm.cc26xx.Timer'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.cc26xx.Timer'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void TimestampProvider$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Alarm$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Boot$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Power$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Seconds$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Timer$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TimestampProvider$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Alarm$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Instance_State", "ti.sysbios.family.arm.cc26xx");
        sizes.clear();
        sizes.add(Global.newArray("rtcCount", "UInt64"));
        sizes.add(Global.newArray("clockObj", "Sti.sysbios.knl.Clock;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.cc26xx.Alarm.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.cc26xx.Alarm.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.cc26xx.Alarm.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Boot$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Power$$SIZES()
    {
    }

    void Seconds$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds.Module_State", "ti.sysbios.family.arm.cc26xx");
        sizes.clear();
        sizes.add(Global.newArray("setSeconds", "UInt32"));
        sizes.add(Global.newArray("setSecondsHi", "UInt32"));
        sizes.add(Global.newArray("refSeconds", "UInt32"));
        sizes.add(Global.newArray("deltaNSecs", "UInt32"));
        sizes.add(Global.newArray("deltaSecs", "TInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.cc26xx.Seconds.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.cc26xx.Seconds.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.cc26xx.Seconds.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Timer$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Instance_State", "ti.sysbios.family.arm.cc26xx");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("staticInst", "UShort"));
        sizes.add(Global.newArray("id", "TInt"));
        sizes.add(Global.newArray("startMode", "Nti.sysbios.interfaces.ITimer.StartMode;;;"));
        sizes.add(Global.newArray("period", "UInt32"));
        sizes.add(Global.newArray("arg", "UIArg"));
        sizes.add(Global.newArray("tickFxn", "UFxn"));
        sizes.add(Global.newArray("frequency", "Sxdc.runtime.Types;FreqHz"));
        sizes.add(Global.newArray("hwi", "UPtr"));
        sizes.add(Global.newArray("period64", "UInt64"));
        sizes.add(Global.newArray("savedCurrCount", "UInt64"));
        sizes.add(Global.newArray("prevThreshold", "UInt64"));
        sizes.add(Global.newArray("nextThreshold", "UInt64"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.cc26xx.Timer.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.cc26xx.Timer.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.cc26xx.Timer.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Module_State", "ti.sysbios.family.arm.cc26xx");
        sizes.clear();
        sizes.add(Global.newArray("availMask", "UInt"));
        sizes.add(Global.newArray("handle", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.cc26xx.Timer.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.cc26xx.Timer.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.cc26xx.Timer.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void TimestampProvider$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider.Module_State", "ti.sysbios.family.arm.cc26xx");
        sizes.clear();
        sizes.add(Global.newArray("timer", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.cc26xx.TimestampProvider.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.cc26xx.TimestampProvider.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.cc26xx.TimestampProvider.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Alarm$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/cc26xx/Alarm.xs");
        om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Module", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Alarm.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.family.arm.cc26xx"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.family.arm.cc26xx"), $$UNDEF, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm$$create", "ti.sysbios.family.arm.cc26xx"), Global.get("ti$sysbios$family$arm$cc26xx$Alarm$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm$$construct", "ti.sysbios.family.arm.cc26xx"), Global.get("ti$sysbios$family$arm$cc26xx$Alarm$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Alarm$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Instance", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Alarm.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.cc26xx"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm$$Params", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Alarm.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.cc26xx"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm$$Object", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Alarm.Object", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Instance", "ti.sysbios.family.arm.cc26xx"));
        // struct Alarm.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm$$BasicView", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Alarm.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("timeout", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("rtcCount", Proto.Elm.newCNum("(xdc_UInt64)"), $$UNDEF, "w");
                po.addFld("rtcInterrupt", Proto.Elm.newCNum("(xdc_UInt64)"), $$UNDEF, "w");
                po.addFld("active", $$T_Bool, $$UNDEF, "w");
        // typedef Alarm.FuncPtr
        om.bind("ti.sysbios.family.arm.cc26xx.Alarm.FuncPtr", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"));
        // struct Alarm.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm$$Instance_State", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Alarm.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("rtcCount", Proto.Elm.newCNum("(xdc_UInt64)"), $$UNDEF, "w");
                po.addFldV("clockObj", (Proto)om.findStrict("ti.sysbios.knl.Clock.Object", "ti.sysbios.family.arm.cc26xx"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_clockObj", (Proto)om.findStrict("ti.sysbios.knl.Clock.Object", "ti.sysbios.family.arm.cc26xx"), $$DEFAULT, "w");
    }

    void Boot$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/cc26xx/Boot.xs");
        om.bind("ti.sysbios.family.arm.cc26xx.Boot$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Boot.Module", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Boot.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.family.arm.cc26xx"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("trimDevice", $$T_Bool, true, "wh");
            po.addFld("customerConfig", $$T_Bool, false, "wh");
            po.addFld("driverlibVersion", Proto.Elm.newCNum("(xdc_UInt)"), 2L, "wh");
            po.addFld("provideDriverlib", $$T_Bool, false, "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Boot$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Boot$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Boot$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Boot$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void Power$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/cc26xx/Power.xs");
        om.bind("ti.sysbios.family.arm.cc26xx.Power$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Power.Module", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Power.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("idle", $$T_Bool, false, "wh");
        po.addFld("resumeSTANDBY", Proto.Elm.newCNum("(xdc_UInt)"), 750L, "wh");
        po.addFld("resumeTicksSTANDBY", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("totalSTANDBY", Proto.Elm.newCNum("(xdc_UInt)"), 1000L, "wh");
        po.addFld("totalTicksSTANDBY", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("wakeDelaySTANDBY", Proto.Elm.newCNum("(xdc_UInt)"), 130L, "wh");
        po.addFld("initialWaitRCOSC_LF", Proto.Elm.newCNum("(xdc_UInt)"), 1000L, "wh");
        po.addFld("retryWaitRCOSC_LF", Proto.Elm.newCNum("(xdc_UInt)"), 1000L, "wh");
        po.addFld("initialWaitXOSC_HF", Proto.Elm.newCNum("(xdc_UInt)"), 50L, "wh");
        po.addFld("retryWaitXOSC_HF", Proto.Elm.newCNum("(xdc_UInt)"), 50L, "wh");
        po.addFld("initialWaitXOSC_LF", Proto.Elm.newCNum("(xdc_UInt)"), 10000L, "wh");
        po.addFld("retryWaitXOSC_LF", Proto.Elm.newCNum("(xdc_UInt)"), 5000L, "wh");
        po.addFld("calibrateRCOSC", $$T_Bool, true, "wh");
        po.addFld("calibrateRCOSC_LF", $$T_Bool, true, "wh");
        po.addFld("calibrateRCOSC_HF", $$T_Bool, true, "wh");
        po.addFld("clockFunc", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), null, "wh");
        po.addFld("policyFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), null, "wh");
        po.addFld("notifyTrapFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), null, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Power$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Power$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Power$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // typedef Power.FuncPtr
        om.bind("ti.sysbios.family.arm.cc26xx.Power.FuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"));
    }

    void Seconds$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/cc26xx/Seconds.xs");
        om.bind("ti.sysbios.family.arm.cc26xx.Seconds$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds.Module", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Seconds.Module", om.findStrict("ti.sysbios.interfaces.ISeconds.Module", "ti.sysbios.family.arm.cc26xx"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Seconds$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Seconds$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Seconds$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Seconds$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct Seconds.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds$$Module_State", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Seconds.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("setSeconds", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("setSecondsHi", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("refSeconds", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("deltaNSecs", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("deltaSecs", Proto.Elm.newCNum("(xdc_Int32)"), $$UNDEF, "w");
    }

    void Timer$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/cc26xx/Timer.xs");
        om.bind("ti.sysbios.family.arm.cc26xx.Timer$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Module", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Timer.Module", om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.family.arm.cc26xx"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
                po.addFld("MAX_PERIOD", Proto.Elm.newCNum("(xdc_UInt)"), 0xFFFFFFFFL, "rh");
                po.addFld("MIN_SWEEP_PERIOD", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
        if (isCFG) {
            po.addFld("supportsDynamic", $$T_Bool, true, "wh");
            po.addFld("defaultDynamic", $$T_Bool, true, "wh");
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.family.arm.cc26xx"), $$UNDEF, "wh");
            po.addFld("E_invalidTimer", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.cc26xx"), Global.newObject("msg", "E_invalidTimer: Invalid Timer Id %d"), "w");
            po.addFld("E_notAvailable", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.cc26xx"), Global.newObject("msg", "E_notAvailable: Timer not available %d"), "w");
            po.addFld("E_cannotSupport", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.cc26xx"), Global.newObject("msg", "E_cannotSupport: Timer cannot support requested period %d"), "w");
            po.addFld("anyMask", Proto.Elm.newCNum("(xdc_UInt)"), 0x1L, "w");
            po.addFld("funcHookCH1", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), null, "w");
            po.addFld("funcHookCH2", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), null, "w");
            po.addFld("startupNeeded", Proto.Elm.newCNum("(xdc_UInt)"), false, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$create", "ti.sysbios.family.arm.cc26xx"), Global.get("ti$sysbios$family$arm$cc26xx$Timer$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$construct", "ti.sysbios.family.arm.cc26xx"), Global.get("ti$sysbios$family$arm$cc26xx$Timer$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Timer$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Timer$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Timer$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Timer$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Timer$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.Timer$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "viewGetCurrentClockTick");
                if (fxn != null) po.addFxn("viewGetCurrentClockTick", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimer$$viewGetCurrentClockTick", "ti.sysbios.family.arm.cc26xx"), fxn);
                fxn = Global.get(cap, "getFreqMeta");
                if (fxn != null) po.addFxn("getFreqMeta", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimer$$getFreqMeta", "ti.sysbios.family.arm.cc26xx"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Instance", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Timer.Instance", om.findStrict("ti.sysbios.interfaces.ITimer.Instance", "ti.sysbios.family.arm.cc26xx"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
                po.addFld("MAX_PERIOD", Proto.Elm.newCNum("(xdc_UInt)"), 0xFFFFFFFFL, "rh");
                po.addFld("MIN_SWEEP_PERIOD", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
        if (isCFG) {
            po.addFld("hwiParams", new Proto.Adr("ti_sysbios_family_arm_m3_Hwi_Params*", "PS"), null, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.cc26xx"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$Params", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Timer.Params", om.findStrict("ti.sysbios.interfaces.ITimer$$Params", "ti.sysbios.family.arm.cc26xx"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
                po.addFld("MAX_PERIOD", Proto.Elm.newCNum("(xdc_UInt)"), 0xFFFFFFFFL, "rh");
                po.addFld("MIN_SWEEP_PERIOD", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
        if (isCFG) {
            po.addFld("hwiParams", new Proto.Adr("ti_sysbios_family_arm_m3_Hwi_Params*", "PS"), null, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.cc26xx"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$Object", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Timer.Object", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Instance", "ti.sysbios.family.arm.cc26xx"));
        // struct Timer.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$BasicView", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Timer.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("halTimerHandle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("id", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("startMode", $$T_Str, $$UNDEF, "w");
                po.addFld("tickFxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("hwiHandle", $$T_Str, $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // struct Timer.DeviceView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$DeviceView", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Timer.DeviceView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("id", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("device", $$T_Str, $$UNDEF, "w");
                po.addFld("devAddr", $$T_Str, $$UNDEF, "w");
                po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("currCount", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("nextCompareCount", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("remainingCount", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("state", $$T_Str, $$UNDEF, "w");
        // struct Timer.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$ModuleView", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Timer.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("availMask", $$T_Str, $$UNDEF, "w");
        // struct Timer.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$Instance_State", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Timer.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("staticInst", $$T_Bool, $$UNDEF, "w");
                po.addFld("id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("startMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.family.arm.cc26xx"), $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF, "w");
                po.addFld("frequency", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios.family.arm.cc26xx"), $$DEFAULT, "w");
                po.addFld("hwi", (Proto)om.findStrict("ti.sysbios.family.arm.m3.Hwi.Handle", "ti.sysbios.family.arm.cc26xx"), $$UNDEF, "w");
                po.addFld("period64", Proto.Elm.newCNum("(xdc_UInt64)"), $$UNDEF, "w");
                po.addFld("savedCurrCount", Proto.Elm.newCNum("(xdc_UInt64)"), $$UNDEF, "w");
                po.addFld("prevThreshold", Proto.Elm.newCNum("(xdc_UInt64)"), $$UNDEF, "w");
                po.addFld("nextThreshold", Proto.Elm.newCNum("(xdc_UInt64)"), $$UNDEF, "w");
        // struct Timer.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$Module_State", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.Timer.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("availMask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("handle", (Proto)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Handle", "ti.sysbios.family.arm.cc26xx"), $$UNDEF, "w");
    }

    void TimestampProvider$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/cc26xx/TimestampProvider.xs");
        om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider.Module", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.TimestampProvider.Module", om.findStrict("ti.sysbios.interfaces.ITimestamp.Module", "ti.sysbios.family.arm.cc26xx"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("timerId", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
            po.addFld("useClockTimer", $$T_Bool, $$UNDEF, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.cc26xx.TimestampProvider$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "getFreqMeta");
                if (fxn != null) po.addFxn("getFreqMeta", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimestamp$$getFreqMeta", "ti.sysbios.family.arm.cc26xx"), fxn);
        // struct TimestampProvider.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider$$Module_State", "ti.sysbios.family.arm.cc26xx");
        po.init("ti.sysbios.family.arm.cc26xx.TimestampProvider.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("timer", (Proto)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Handle", "ti.sysbios.family.arm.cc26xx"), $$UNDEF, "w");
    }

    void Alarm$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm", "ti.sysbios.family.arm.cc26xx");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.cc26xx.Alarm.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm$$Instance_State", "ti.sysbios.family.arm.cc26xx");
    }

    void Boot$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Boot", "ti.sysbios.family.arm.cc26xx");
    }

    void Power$$ROV()
    {
    }

    void Seconds$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds", "ti.sysbios.family.arm.cc26xx");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.cc26xx.Seconds.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds$$Module_State", "ti.sysbios.family.arm.cc26xx");
    }

    void Timer$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer", "ti.sysbios.family.arm.cc26xx");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$Instance_State", "ti.sysbios.family.arm.cc26xx");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.cc26xx.Timer.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$Instance_State", "ti.sysbios.family.arm.cc26xx");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.cc26xx.Timer.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$Module_State", "ti.sysbios.family.arm.cc26xx");
    }

    void TimestampProvider$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider", "ti.sysbios.family.arm.cc26xx");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.cc26xx.TimestampProvider.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider$$Module_State", "ti.sysbios.family.arm.cc26xx");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.family.arm.cc26xx.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.family.arm.cc26xx"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/cc26xx/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.family.arm.cc26xx"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.family.arm.cc26xx"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.family.arm.cc26xx"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.family.arm.cc26xx"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.family.arm.cc26xx"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.family.arm.cc26xx"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.family.arm.cc26xx", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.family.arm.cc26xx");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.family.arm.cc26xx.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.family.arm.cc26xx'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.family.arm.cc26xx$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.family.arm.cc26xx$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.family.arm.cc26xx$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Alarm$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm", "ti.sysbios.family.arm.cc26xx");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Module", "ti.sysbios.family.arm.cc26xx");
        vo.init2(po, "ti.sysbios.family.arm.cc26xx.Alarm", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm$$capsule", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("Instance", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Instance", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("Params", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Params", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Params", "ti.sysbios.family.arm.cc26xx")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Handle", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.cc26xx", "ti.sysbios.family.arm.cc26xx"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.BasicView", "ti.sysbios.family.arm.cc26xx"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.BasicView", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.FuncPtr", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Instance_State", "ti.sysbios.family.arm.cc26xx"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Instance_State", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.cc26xx")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.sysbios.family.arm.cc26xx.Alarm$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$family$arm$cc26xx$Alarm$$__initObject"));
        }//isCFG
        vo.bind("getCount", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.getCount", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_cc26xx_Alarm_Handle__label__E", "ti_sysbios_family_arm_cc26xx_Alarm_Module__startupDone__E", "ti_sysbios_family_arm_cc26xx_Alarm_Object__create__E", "ti_sysbios_family_arm_cc26xx_Alarm_Object__delete__E", "ti_sysbios_family_arm_cc26xx_Alarm_Object__get__E", "ti_sysbios_family_arm_cc26xx_Alarm_Object__first__E", "ti_sysbios_family_arm_cc26xx_Alarm_Object__next__E", "ti_sysbios_family_arm_cc26xx_Alarm_Params__init__E", "ti_sysbios_family_arm_cc26xx_Alarm_getCount__E", "ti_sysbios_family_arm_cc26xx_Alarm_set__E", "ti_sysbios_family_arm_cc26xx_Alarm_setFunc__E", "ti_sysbios_family_arm_cc26xx_Alarm_stop__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm.Object", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Alarm", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Alarm");
    }

    void Boot$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Boot", "ti.sysbios.family.arm.cc26xx");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Boot.Module", "ti.sysbios.family.arm.cc26xx");
        vo.init2(po, "ti.sysbios.family.arm.cc26xx.Boot", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.cc26xx.Boot$$capsule", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.cc26xx", "ti.sysbios.family.arm.cc26xx"));
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
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.cc26xx")).add(vo);
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
        vo.bind("getBootReason", om.findStrict("ti.sysbios.family.arm.cc26xx.Boot.getBootReason", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_cc26xx_Boot_Module__startupDone__E", "ti_sysbios_family_arm_cc26xx_Boot_getBootReason__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", "./Boot.xdt");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./Boot.xdt");
        pkgV.bind("Boot", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Boot");
    }

    void Power$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Power", "ti.sysbios.family.arm.cc26xx");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Power.Module", "ti.sysbios.family.arm.cc26xx");
        vo.init2(po, "ti.sysbios.family.arm.cc26xx.Power", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.cc26xx.Power$$capsule", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.cc26xx", "ti.sysbios.family.arm.cc26xx"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.family.arm.cc26xx.Power.FuncPtr", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.cc26xx")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.sysbios.family.arm.cc26xx.Power$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("Power", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Power");
    }

    void Seconds$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds", "ti.sysbios.family.arm.cc26xx");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds.Module", "ti.sysbios.family.arm.cc26xx");
        vo.init2(po, "ti.sysbios.family.arm.cc26xx.Seconds", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds$$capsule", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.cc26xx", "ti.sysbios.family.arm.cc26xx"));
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
        vo.bind("Time", om.findStrict("ti.sysbios.interfaces.ISeconds.Time", "ti.sysbios.family.arm.cc26xx"));
        tdefs.add(om.findStrict("ti.sysbios.interfaces.ISeconds.Time", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds.Module_State", "ti.sysbios.family.arm.cc26xx"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds.Module_State", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.cc26xx")).add(vo);
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
        vo.bind("get", om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds.get", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("getTime", om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds.getTime", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("set", om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds.set", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("setTime", om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds.setTime", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_cc26xx_Seconds_Module__startupDone__E", "ti_sysbios_family_arm_cc26xx_Seconds_get__E", "ti_sysbios_family_arm_cc26xx_Seconds_getTime__E", "ti_sysbios_family_arm_cc26xx_Seconds_set__E", "ti_sysbios_family_arm_cc26xx_Seconds_setTime__E"));
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

    void Timer$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer", "ti.sysbios.family.arm.cc26xx");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Module", "ti.sysbios.family.arm.cc26xx");
        vo.init2(po, "ti.sysbios.family.arm.cc26xx.Timer", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer$$capsule", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("Instance", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Instance", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("Params", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Params", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Params", "ti.sysbios.family.arm.cc26xx")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Handle", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.cc26xx", "ti.sysbios.family.arm.cc26xx"));
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
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.interfaces.ITimer.FuncPtr", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.BasicView", "ti.sysbios.family.arm.cc26xx"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.BasicView", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("DeviceView", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.DeviceView", "ti.sysbios.family.arm.cc26xx"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.DeviceView", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("ModuleView", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.ModuleView", "ti.sysbios.family.arm.cc26xx"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.ModuleView", "ti.sysbios.family.arm.cc26xx"));
        mcfgs.add("E_invalidTimer");
        mcfgs.add("E_notAvailable");
        mcfgs.add("E_cannotSupport");
        mcfgs.add("anyMask");
        mcfgs.add("funcHookCH1");
        mcfgs.add("funcHookCH2");
        mcfgs.add("startupNeeded");
        icfgs.add("startupNeeded");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Instance_State", "ti.sysbios.family.arm.cc26xx"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Instance_State", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Module_State", "ti.sysbios.family.arm.cc26xx"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Module_State", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("StartMode_AUTO", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode_AUTO", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("StartMode_USER", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode_USER", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("RunMode_CONTINUOUS", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("RunMode_ONESHOT", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_ONESHOT", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("RunMode_DYNAMIC", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_DYNAMIC", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("Status_INUSE", om.findStrict("ti.sysbios.interfaces.ITimer.Status_INUSE", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("Status_FREE", om.findStrict("ti.sysbios.interfaces.ITimer.Status_FREE", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("PeriodType_MICROSECS", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("PeriodType_COUNTS", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType_COUNTS", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.cc26xx")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.family.arm.cc26xx.Timer$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$family$arm$cc26xx$Timer$$__initObject"));
        }//isCFG
        vo.bind("getNumTimers", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.getNumTimers", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("getStatus", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.getStatus", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("startup", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.startup", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("dynamicStub", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.dynamicStub", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("dynamicMultiStub", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.dynamicMultiStub", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("periodicStub", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.periodicStub", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("getCount64", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.getCount64", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("getExpiredCounts64", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.getExpiredCounts64", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("getHandle", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.getHandle", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("initDevice", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.initDevice", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("postInit", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.postInit", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("setThreshold", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.setThreshold", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_cc26xx_Timer_Handle__label__E", "ti_sysbios_family_arm_cc26xx_Timer_Module__startupDone__E", "ti_sysbios_family_arm_cc26xx_Timer_Object__create__E", "ti_sysbios_family_arm_cc26xx_Timer_Object__delete__E", "ti_sysbios_family_arm_cc26xx_Timer_Object__get__E", "ti_sysbios_family_arm_cc26xx_Timer_Object__first__E", "ti_sysbios_family_arm_cc26xx_Timer_Object__next__E", "ti_sysbios_family_arm_cc26xx_Timer_Params__init__E", "ti_sysbios_family_arm_cc26xx_Timer_getNumTimers__E", "ti_sysbios_family_arm_cc26xx_Timer_getStatus__E", "ti_sysbios_family_arm_cc26xx_Timer_startup__E", "ti_sysbios_family_arm_cc26xx_Timer_getMaxTicks__E", "ti_sysbios_family_arm_cc26xx_Timer_setNextTick__E", "ti_sysbios_family_arm_cc26xx_Timer_start__E", "ti_sysbios_family_arm_cc26xx_Timer_stop__E", "ti_sysbios_family_arm_cc26xx_Timer_setPeriod__E", "ti_sysbios_family_arm_cc26xx_Timer_setPeriodMicroSecs__E", "ti_sysbios_family_arm_cc26xx_Timer_getPeriod__E", "ti_sysbios_family_arm_cc26xx_Timer_getCount__E", "ti_sysbios_family_arm_cc26xx_Timer_getFreq__E", "ti_sysbios_family_arm_cc26xx_Timer_getFunc__E", "ti_sysbios_family_arm_cc26xx_Timer_setFunc__E", "ti_sysbios_family_arm_cc26xx_Timer_trigger__E", "ti_sysbios_family_arm_cc26xx_Timer_getExpiredCounts__E", "ti_sysbios_family_arm_cc26xx_Timer_getExpiredTicks__E", "ti_sysbios_family_arm_cc26xx_Timer_getCurrentTick__E", "ti_sysbios_family_arm_cc26xx_Timer_dynamicStub__E", "ti_sysbios_family_arm_cc26xx_Timer_dynamicMultiStub__E", "ti_sysbios_family_arm_cc26xx_Timer_periodicStub__E", "ti_sysbios_family_arm_cc26xx_Timer_getCount64__E", "ti_sysbios_family_arm_cc26xx_Timer_getExpiredCounts64__E", "ti_sysbios_family_arm_cc26xx_Timer_getHandle__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray("E_invalidTimer", "E_notAvailable", "E_cannotSupport"));
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.family.arm.cc26xx.Timer.Object", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Timer", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Timer");
    }

    void TimestampProvider$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider", "ti.sysbios.family.arm.cc26xx");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider.Module", "ti.sysbios.family.arm.cc26xx");
        vo.init2(po, "ti.sysbios.family.arm.cc26xx.TimestampProvider", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider$$capsule", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.cc26xx", "ti.sysbios.family.arm.cc26xx"));
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
        mcfgs.add("timerId");
        mcfgs.add("useClockTimer");
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider.Module_State", "ti.sysbios.family.arm.cc26xx"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider.Module_State", "ti.sysbios.family.arm.cc26xx"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.cc26xx")).add(vo);
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
        vo.bind("get32", om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider.get32", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("get64", om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider.get64", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("getFreq", om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider.getFreq", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("startTimer", om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider.startTimer", "ti.sysbios.family.arm.cc26xx"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_cc26xx_TimestampProvider_Module__startupDone__E", "ti_sysbios_family_arm_cc26xx_TimestampProvider_get32__E", "ti_sysbios_family_arm_cc26xx_TimestampProvider_get64__E", "ti_sysbios_family_arm_cc26xx_TimestampProvider_getFreq__E", "ti_sysbios_family_arm_cc26xx_TimestampProvider_startTimer__E"));
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

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.sysbios.family.arm.cc26xx")).findStrict("PARAMS", "ti.sysbios.family.arm.cc26xx");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm", "ti.sysbios.family.arm.cc26xx")).findStrict("PARAMS", "ti.sysbios.family.arm.cc26xx");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer", "ti.sysbios.family.arm.cc26xx")).findStrict("PARAMS", "ti.sysbios.family.arm.cc26xx");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm", "ti.sysbios.family.arm.cc26xx"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.cc26xx.Boot", "ti.sysbios.family.arm.cc26xx"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.cc26xx.Power", "ti.sysbios.family.arm.cc26xx"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.cc26xx.Seconds", "ti.sysbios.family.arm.cc26xx"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer", "ti.sysbios.family.arm.cc26xx"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.cc26xx.TimestampProvider", "ti.sysbios.family.arm.cc26xx"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Alarm", "ti.sysbios.family.arm.cc26xx");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.cc26xx.Timer", "ti.sysbios.family.arm.cc26xx");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Device", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitDevice", "structName", "DeviceView")}), Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.family.arm.cc26xx.Alarm")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.cc26xx.Boot")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.cc26xx.Power")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.cc26xx.Seconds")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.cc26xx.Timer")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.cc26xx.TimestampProvider")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.family.arm.cc26xx")).add(pkgV);
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
        Alarm$$OBJECTS();
        Boot$$OBJECTS();
        Power$$OBJECTS();
        Seconds$$OBJECTS();
        Timer$$OBJECTS();
        TimestampProvider$$OBJECTS();
        Alarm$$CONSTS();
        Boot$$CONSTS();
        Power$$CONSTS();
        Seconds$$CONSTS();
        Timer$$CONSTS();
        TimestampProvider$$CONSTS();
        Alarm$$CREATES();
        Boot$$CREATES();
        Power$$CREATES();
        Seconds$$CREATES();
        Timer$$CREATES();
        TimestampProvider$$CREATES();
        Alarm$$FUNCTIONS();
        Boot$$FUNCTIONS();
        Power$$FUNCTIONS();
        Seconds$$FUNCTIONS();
        Timer$$FUNCTIONS();
        TimestampProvider$$FUNCTIONS();
        Alarm$$SIZES();
        Boot$$SIZES();
        Power$$SIZES();
        Seconds$$SIZES();
        Timer$$SIZES();
        TimestampProvider$$SIZES();
        Alarm$$TYPES();
        Boot$$TYPES();
        Power$$TYPES();
        Seconds$$TYPES();
        Timer$$TYPES();
        TimestampProvider$$TYPES();
        if (isROV) {
            Alarm$$ROV();
            Boot$$ROV();
            Power$$ROV();
            Seconds$$ROV();
            Timer$$ROV();
            TimestampProvider$$ROV();
        }//isROV
        $$SINGLETONS();
        Alarm$$SINGLETONS();
        Boot$$SINGLETONS();
        Power$$SINGLETONS();
        Seconds$$SINGLETONS();
        Timer$$SINGLETONS();
        TimestampProvider$$SINGLETONS();
        $$INITIALIZATION();
    }
}
