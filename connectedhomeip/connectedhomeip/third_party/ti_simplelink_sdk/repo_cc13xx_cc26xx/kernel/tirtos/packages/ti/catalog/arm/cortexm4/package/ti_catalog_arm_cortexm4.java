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

public class ti_catalog_arm_cortexm4
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
        Global.callFxn("loadPackage", xdcO, "ti.catalog");
        Global.callFxn("loadPackage", xdcO, "ti.catalog.peripherals.hdvicp2");
        Global.callFxn("loadPackage", xdcO, "xdc.platform");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.catalog.arm.cortexm4", new Value.Obj("ti.catalog.arm.cortexm4", pkgP));
    }

    void Tiva$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.Tiva.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.Tiva", new Value.Obj("ti.catalog.arm.cortexm4.Tiva", po));
        pkgV.bind("Tiva", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.Tiva.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.Tiva$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.Tiva.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.Tiva$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.Tiva.Params", new Proto.Str(po, true));
    }

    void OMAP5430$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.OMAP5430.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.OMAP5430", new Value.Obj("ti.catalog.arm.cortexm4.OMAP5430", po));
        pkgV.bind("OMAP5430", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.OMAP5430.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.OMAP5430$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.OMAP5430.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.OMAP5430$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.OMAP5430.Params", new Proto.Str(po, true));
    }

    void Vayu$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.Vayu.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.Vayu", new Value.Obj("ti.catalog.arm.cortexm4.Vayu", po));
        pkgV.bind("Vayu", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.Vayu.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.Vayu$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.Vayu.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.Vayu$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.Vayu.Params", new Proto.Str(po, true));
    }

    void DRA7XX$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.DRA7XX.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.DRA7XX", new Value.Obj("ti.catalog.arm.cortexm4.DRA7XX", po));
        pkgV.bind("DRA7XX", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.DRA7XX.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.DRA7XX$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.DRA7XX.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.DRA7XX$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.DRA7XX.Params", new Proto.Str(po, true));
    }

    void TDA3XX$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.TDA3XX.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.TDA3XX", new Value.Obj("ti.catalog.arm.cortexm4.TDA3XX", po));
        pkgV.bind("TDA3XX", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.TDA3XX.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.TDA3XX$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.TDA3XX.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.TDA3XX$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.TDA3XX.Params", new Proto.Str(po, true));
    }

    void CC32xx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CC32xx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.CC32xx", new Value.Obj("ti.catalog.arm.cortexm4.CC32xx", po));
        pkgV.bind("CC32xx", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.CC32xx.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CC32xx$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.CC32xx.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CC32xx$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.CC32xx.Params", new Proto.Str(po, true));
    }

    void MSP432$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.MSP432.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.MSP432", new Value.Obj("ti.catalog.arm.cortexm4.MSP432", po));
        pkgV.bind("MSP432", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.MSP432.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.MSP432$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.MSP432.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.MSP432$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.MSP432.Params", new Proto.Str(po, true));
    }

    void MSP432E$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.MSP432E.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.MSP432E", new Value.Obj("ti.catalog.arm.cortexm4.MSP432E", po));
        pkgV.bind("MSP432E", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.MSP432E.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.MSP432E$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.MSP432E.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.MSP432E$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.MSP432E.Params", new Proto.Str(po, true));
    }

    void CC13xx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CC13xx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.CC13xx", new Value.Obj("ti.catalog.arm.cortexm4.CC13xx", po));
        pkgV.bind("CC13xx", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.CC13xx.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CC13xx$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.CC13xx.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CC13xx$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.CC13xx.Params", new Proto.Str(po, true));
    }

    void CC26xx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CC26xx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.CC26xx", new Value.Obj("ti.catalog.arm.cortexm4.CC26xx", po));
        pkgV.bind("CC26xx", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.CC26xx.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CC26xx$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.CC26xx.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CC26xx$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.CC26xx.Params", new Proto.Str(po, true));
    }

    void CortexM$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CortexM.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm4.CortexM", new Value.Obj("ti.catalog.arm.cortexm4.CortexM", po));
        pkgV.bind("CortexM", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm4.CortexM.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CortexM$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.CortexM.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm4.CortexM$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm4.CortexM.Params", new Proto.Str(po, true));
    }

    void Tiva$$CONSTS()
    {
        // module Tiva
    }

    void OMAP5430$$CONSTS()
    {
        // module OMAP5430
    }

    void Vayu$$CONSTS()
    {
        // module Vayu
    }

    void DRA7XX$$CONSTS()
    {
        // module DRA7XX
    }

    void TDA3XX$$CONSTS()
    {
        // module TDA3XX
    }

    void CC32xx$$CONSTS()
    {
        // module CC32xx
    }

    void MSP432$$CONSTS()
    {
        // module MSP432
    }

    void MSP432E$$CONSTS()
    {
        // module MSP432E
    }

    void CC13xx$$CONSTS()
    {
        // module CC13xx
    }

    void CC26xx$$CONSTS()
    {
        // module CC26xx
    }

    void CortexM$$CONSTS()
    {
        // module CortexM
    }

    void Tiva$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.Tiva$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.Tiva.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.Tiva.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.Tiva.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$Tiva$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.Tiva'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.Tiva.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.Tiva$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.Tiva.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.Tiva$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.Tiva.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$Tiva$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.Tiva'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void OMAP5430$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.OMAP5430$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$OMAP5430$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.OMAP5430'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.OMAP5430.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("__inst.hdvicp0 = __mod.PARAMS.hdvicp0;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.OMAP5430$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$OMAP5430$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.OMAP5430'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("__inst.hdvicp0 = __mod.PARAMS.hdvicp0;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void Vayu$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.Vayu$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.Vayu.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.Vayu.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.Vayu.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$Vayu$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.Vayu'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.Vayu.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("__inst.hdvicp0 = __mod.PARAMS.hdvicp0;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.Vayu$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.Vayu.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.Vayu$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.Vayu.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$Vayu$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.Vayu'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("__inst.hdvicp0 = __mod.PARAMS.hdvicp0;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void DRA7XX$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.DRA7XX$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$DRA7XX$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.DRA7XX'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.DRA7XX.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("__inst.hdvicp0 = __mod.PARAMS.hdvicp0;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.DRA7XX$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$DRA7XX$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.DRA7XX'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("__inst.hdvicp0 = __mod.PARAMS.hdvicp0;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void TDA3XX$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.TDA3XX$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$TDA3XX$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.TDA3XX'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.TDA3XX.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.TDA3XX$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$TDA3XX$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.TDA3XX'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void CC32xx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.CC32xx$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$CC32xx$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.CC32xx'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.CC32xx.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.CC32xx$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CC32xx$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$CC32xx$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.CC32xx'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void MSP432$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.MSP432$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.MSP432.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.MSP432.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.MSP432.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$MSP432$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.MSP432'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.MSP432.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.MSP432$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.MSP432.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.MSP432$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.MSP432.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$MSP432$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.MSP432'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void MSP432E$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.MSP432E$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$MSP432E$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.MSP432E'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.MSP432E.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.MSP432E$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.MSP432E$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$MSP432E$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.MSP432E'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void CC13xx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.CC13xx$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$CC13xx$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.CC13xx'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.CC13xx.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.CC13xx$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CC13xx$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$CC13xx$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.CC13xx'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void CC26xx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.CC26xx$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$CC26xx$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.CC26xx'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.CC26xx.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.CC26xx$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CC26xx$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$CC26xx$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.CC26xx'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void CortexM$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.CortexM$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.CortexM.Module", "ti.catalog.arm.cortexm4"), om.findStrict("ti.catalog.arm.cortexm4.CortexM.Instance", "ti.catalog.arm.cortexm4"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CortexM.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$CortexM$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.CortexM'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm4.CortexM.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm4']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm4.CortexM$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm4.CortexM.Module", "ti.catalog.arm.cortexm4"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CortexM$$Object", "ti.catalog.arm.cortexm4"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm4.CortexM.Params", "ti.catalog.arm.cortexm4"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm4$CortexM$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm4.CortexM'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {revision:revision});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.cpuCore = __mod.PARAMS.cpuCore;\n");
            sb.append("__inst.cpuCoreRevision = __mod.PARAMS.cpuCoreRevision;\n");
            sb.append("__inst.minProgUnitSize = __mod.PARAMS.minProgUnitSize;\n");
            sb.append("__inst.minDataUnitSize = __mod.PARAMS.minDataUnitSize;\n");
            sb.append("__inst.dataWordSize = __mod.PARAMS.dataWordSize;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("__inst.deviceHeader = __mod.PARAMS.deviceHeader;\n");
            sb.append("__inst.isa = __mod.PARAMS.isa;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void Tiva$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP5430$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Vayu$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void DRA7XX$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TDA3XX$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CC32xx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void MSP432$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void MSP432E$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CC13xx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CC26xx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CortexM$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Tiva$$SIZES()
    {
    }

    void OMAP5430$$SIZES()
    {
    }

    void Vayu$$SIZES()
    {
    }

    void DRA7XX$$SIZES()
    {
    }

    void TDA3XX$$SIZES()
    {
    }

    void CC32xx$$SIZES()
    {
    }

    void MSP432$$SIZES()
    {
    }

    void MSP432E$$SIZES()
    {
    }

    void CC13xx$$SIZES()
    {
    }

    void CC26xx$$SIZES()
    {
    }

    void CortexM$$SIZES()
    {
    }

    void Tiva$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/Tiva.xs");
        om.bind("ti.catalog.arm.cortexm4.Tiva$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.Tiva.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.Tiva.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.Tiva$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$Tiva$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.Tiva$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$Tiva$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.Tiva$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.Tiva$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.Tiva$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.Tiva$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.Tiva.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.Tiva.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.Tiva$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.Tiva.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.Tiva$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.Tiva.Object", om.findStrict("ti.catalog.arm.cortexm4.Tiva.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void OMAP5430$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/OMAP5430.xs");
        om.bind("ti.catalog.arm.cortexm4.OMAP5430$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.OMAP5430.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$OMAP5430$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$OMAP5430$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.OMAP5430$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.OMAP5430$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.OMAP5430$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.OMAP5430$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.OMAP5430.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm4"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm4")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00010000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.OMAP5430.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm4"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm4")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00010000L)})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.OMAP5430.Object", om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void Vayu$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/Vayu.xs");
        om.bind("ti.catalog.arm.cortexm4.Vayu$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.Vayu.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.Vayu.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.Vayu$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$Vayu$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.Vayu$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$Vayu$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.Vayu$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.Vayu$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.Vayu$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.Vayu$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.Vayu.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.Vayu.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm4"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm4")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00010000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.Vayu$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.Vayu.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm4"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm4")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00010000L)})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.Vayu$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.Vayu.Object", om.findStrict("ti.catalog.arm.cortexm4.Vayu.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void DRA7XX$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/DRA7XX.xs");
        om.bind("ti.catalog.arm.cortexm4.DRA7XX$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.DRA7XX.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$DRA7XX$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$DRA7XX$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.DRA7XX$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.DRA7XX$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.DRA7XX$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.DRA7XX$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.DRA7XX.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm4"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm4")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00010000L)}), Global.newArray(new Object[]{"OCMC_RAM1", Global.newObject("comment", "OCMC (On-chip RAM) Bank 1 (512KB)", "name", "OCMC_RAM1", "base", 0x40300000L, "len", 0x00080000L)}), Global.newArray(new Object[]{"OCMC_RAM2", Global.newObject("comment", "OCMC (On-chip RAM) Bank 2 (1MB)", "name", "OCMC_RAM2", "base", 0x40400000L, "len", 0x00100000L)}), Global.newArray(new Object[]{"OCMC_RAM3", Global.newObject("comment", "OCMC (On-chip RAM) Bank 3 (1MB)", "name", "OCMC_RAM3", "base", 0x40500000L, "len", 0x00100000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.DRA7XX.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm4"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm4")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00010000L)}), Global.newArray(new Object[]{"OCMC_RAM1", Global.newObject("comment", "OCMC (On-chip RAM) Bank 1 (512KB)", "name", "OCMC_RAM1", "base", 0x40300000L, "len", 0x00080000L)}), Global.newArray(new Object[]{"OCMC_RAM2", Global.newObject("comment", "OCMC (On-chip RAM) Bank 2 (1MB)", "name", "OCMC_RAM2", "base", 0x40400000L, "len", 0x00100000L)}), Global.newArray(new Object[]{"OCMC_RAM3", Global.newObject("comment", "OCMC (On-chip RAM) Bank 3 (1MB)", "name", "OCMC_RAM3", "base", 0x40500000L, "len", 0x00100000L)})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.DRA7XX.Object", om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void TDA3XX$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/TDA3XX.xs");
        om.bind("ti.catalog.arm.cortexm4.TDA3XX$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.TDA3XX.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$TDA3XX$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$TDA3XX$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.TDA3XX$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.TDA3XX$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.TDA3XX$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.TDA3XX$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.TDA3XX.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm4")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00010000L)}), Global.newArray(new Object[]{"OCMC_RAM", Global.newObject("name", "OCMC_RAM", "base", 0x40300000L, "len", 0x00080000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.TDA3XX.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm4")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00010000L)}), Global.newArray(new Object[]{"OCMC_RAM", Global.newObject("name", "OCMC_RAM", "base", 0x40300000L, "len", 0x00080000L)})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.TDA3XX.Object", om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void CC32xx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/CC32xx.xs");
        om.bind("ti.catalog.arm.cortexm4.CC32xx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC32xx.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.CC32xx$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$CC32xx$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.CC32xx$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$CC32xx$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC32xx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC32xx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC32xx$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC32xx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC32xx.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC32xx$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC32xx.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC32xx$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC32xx.Object", om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void MSP432$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/MSP432.xs");
        om.bind("ti.catalog.arm.cortexm4.MSP432$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.MSP432.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.MSP432$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$MSP432$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.MSP432$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$MSP432$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.MSP432$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.MSP432$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.MSP432$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.MSP432$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.MSP432.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.MSP432.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.MSP432.Object", om.findStrict("ti.catalog.arm.cortexm4.MSP432.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void MSP432E$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/MSP432E.xs");
        om.bind("ti.catalog.arm.cortexm4.MSP432E$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.MSP432E.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.MSP432E$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$MSP432E$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.MSP432E$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$MSP432E$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.MSP432E$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.MSP432E$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.MSP432E$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.MSP432E$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.MSP432E.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432E$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.MSP432E.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432E$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.MSP432E.Object", om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void CC13xx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/CC13xx.xs");
        om.bind("ti.catalog.arm.cortexm4.CC13xx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC13xx.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.CC13xx$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$CC13xx$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.CC13xx$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$CC13xx$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC13xx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC13xx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC13xx$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC13xx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC13xx.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC13xx$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC13xx.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC13xx$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC13xx.Object", om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void CC26xx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/CC26xx.xs");
        om.bind("ti.catalog.arm.cortexm4.CC26xx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC26xx.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.CC26xx$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$CC26xx$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.CC26xx$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$CC26xx$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC26xx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC26xx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC26xx$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CC26xx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC26xx.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC26xx$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC26xx.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC26xx$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CC26xx.Object", om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void CortexM$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm4/CortexM.xs");
        om.bind("ti.catalog.arm.cortexm4.CortexM$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CortexM.Module", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CortexM.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.CortexM$$create", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$CortexM$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm4.CortexM$$construct", "ti.catalog.arm.cortexm4"), Global.get("ti$catalog$arm$cortexm4$CortexM$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CortexM$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CortexM$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CortexM$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm4.CortexM$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CortexM.Instance", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CortexM.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "M4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CortexM$$Params", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CortexM.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm4"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "M4", "wh");
        po.addFld("isa", $$T_Str, "v7M4", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CortexM$$Object", "ti.catalog.arm.cortexm4");
        po.init("ti.catalog.arm.cortexm4.CortexM.Object", om.findStrict("ti.catalog.arm.cortexm4.CortexM.Instance", "ti.catalog.arm.cortexm4"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm4"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm4"), fxn);
    }

    void Tiva$$ROV()
    {
    }

    void OMAP5430$$ROV()
    {
    }

    void Vayu$$ROV()
    {
    }

    void DRA7XX$$ROV()
    {
    }

    void TDA3XX$$ROV()
    {
    }

    void CC32xx$$ROV()
    {
    }

    void MSP432$$ROV()
    {
    }

    void MSP432E$$ROV()
    {
    }

    void CC13xx$$ROV()
    {
    }

    void CC26xx$$ROV()
    {
    }

    void CortexM$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.catalog.arm.cortexm4.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.catalog.arm.cortexm4"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.catalog.arm.cortexm4", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.catalog.arm.cortexm4");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.catalog.arm.cortexm4.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.catalog.arm.cortexm4'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.catalog.arm.cortexm4$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.catalog.arm.cortexm4$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.catalog.arm.cortexm4$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Tiva$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.Tiva", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.Tiva.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.Tiva", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.Tiva$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.Tiva.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.Tiva.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.Tiva.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.Tiva$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.Tiva.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("Tiva", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Tiva");
    }

    void OMAP5430$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.OMAP5430", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.OMAP5430$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.OMAP5430$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.OMAP5430.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("OMAP5430", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP5430");
    }

    void Vayu$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.Vayu", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.Vayu.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.Vayu", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.Vayu$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.Vayu.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.Vayu.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.Vayu.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.Vayu$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.Vayu.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("Vayu", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Vayu");
    }

    void DRA7XX$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.DRA7XX", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.DRA7XX$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.DRA7XX$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.DRA7XX.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("DRA7XX", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("DRA7XX");
    }

    void TDA3XX$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.TDA3XX", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.TDA3XX$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.TDA3XX$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.TDA3XX.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("TDA3XX", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TDA3XX");
    }

    void CC32xx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC32xx", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.CC32xx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.CC32xx$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.CC32xx$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.CC32xx.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("CC32xx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CC32xx");
    }

    void MSP432$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.MSP432", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.MSP432$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.MSP432.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.MSP432.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.MSP432.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.MSP432$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.MSP432.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("MSP432", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("MSP432");
    }

    void MSP432E$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432E", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.MSP432E", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.MSP432E$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.MSP432E$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.MSP432E.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("MSP432E", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("MSP432E");
    }

    void CC13xx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC13xx", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.CC13xx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.CC13xx$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.CC13xx$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.CC13xx.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("CC13xx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CC13xx");
    }

    void CC26xx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC26xx", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.CC26xx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.CC26xx$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.CC26xx$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.CC26xx.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("CC26xx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CC26xx");
    }

    void CortexM$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm4.CortexM", "ti.catalog.arm.cortexm4");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm4.CortexM.Module", "ti.catalog.arm.cortexm4");
        vo.init2(po, "ti.catalog.arm.cortexm4.CortexM", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm4.CortexM$$capsule", "ti.catalog.arm.cortexm4"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm4.CortexM.Instance", "ti.catalog.arm.cortexm4"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm4.CortexM.Params", "ti.catalog.arm.cortexm4"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm4.CortexM.Params", "ti.catalog.arm.cortexm4")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm4", "ti.catalog.arm.cortexm4"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm4")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm4.CortexM$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm4.CortexM.Object", "ti.catalog.arm.cortexm4"));
        pkgV.bind("CortexM", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CortexM");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.Tiva", "ti.catalog.arm.cortexm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.OMAP5430", "ti.catalog.arm.cortexm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.Vayu", "ti.catalog.arm.cortexm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.DRA7XX", "ti.catalog.arm.cortexm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.TDA3XX", "ti.catalog.arm.cortexm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.CC32xx", "ti.catalog.arm.cortexm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.MSP432", "ti.catalog.arm.cortexm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.MSP432E", "ti.catalog.arm.cortexm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.CC13xx", "ti.catalog.arm.cortexm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.CC26xx", "ti.catalog.arm.cortexm4"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm4.CortexM", "ti.catalog.arm.cortexm4"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.Tiva")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.OMAP5430")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.Vayu")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.DRA7XX")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.TDA3XX")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.CC32xx")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.MSP432")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.MSP432E")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.CC13xx")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.CC26xx")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm4.CortexM")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.catalog.arm.cortexm4")).add(pkgV);
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
        Tiva$$OBJECTS();
        OMAP5430$$OBJECTS();
        Vayu$$OBJECTS();
        DRA7XX$$OBJECTS();
        TDA3XX$$OBJECTS();
        CC32xx$$OBJECTS();
        MSP432$$OBJECTS();
        MSP432E$$OBJECTS();
        CC13xx$$OBJECTS();
        CC26xx$$OBJECTS();
        CortexM$$OBJECTS();
        Tiva$$CONSTS();
        OMAP5430$$CONSTS();
        Vayu$$CONSTS();
        DRA7XX$$CONSTS();
        TDA3XX$$CONSTS();
        CC32xx$$CONSTS();
        MSP432$$CONSTS();
        MSP432E$$CONSTS();
        CC13xx$$CONSTS();
        CC26xx$$CONSTS();
        CortexM$$CONSTS();
        Tiva$$CREATES();
        OMAP5430$$CREATES();
        Vayu$$CREATES();
        DRA7XX$$CREATES();
        TDA3XX$$CREATES();
        CC32xx$$CREATES();
        MSP432$$CREATES();
        MSP432E$$CREATES();
        CC13xx$$CREATES();
        CC26xx$$CREATES();
        CortexM$$CREATES();
        Tiva$$FUNCTIONS();
        OMAP5430$$FUNCTIONS();
        Vayu$$FUNCTIONS();
        DRA7XX$$FUNCTIONS();
        TDA3XX$$FUNCTIONS();
        CC32xx$$FUNCTIONS();
        MSP432$$FUNCTIONS();
        MSP432E$$FUNCTIONS();
        CC13xx$$FUNCTIONS();
        CC26xx$$FUNCTIONS();
        CortexM$$FUNCTIONS();
        Tiva$$SIZES();
        OMAP5430$$SIZES();
        Vayu$$SIZES();
        DRA7XX$$SIZES();
        TDA3XX$$SIZES();
        CC32xx$$SIZES();
        MSP432$$SIZES();
        MSP432E$$SIZES();
        CC13xx$$SIZES();
        CC26xx$$SIZES();
        CortexM$$SIZES();
        Tiva$$TYPES();
        OMAP5430$$TYPES();
        Vayu$$TYPES();
        DRA7XX$$TYPES();
        TDA3XX$$TYPES();
        CC32xx$$TYPES();
        MSP432$$TYPES();
        MSP432E$$TYPES();
        CC13xx$$TYPES();
        CC26xx$$TYPES();
        CortexM$$TYPES();
        if (isROV) {
            Tiva$$ROV();
            OMAP5430$$ROV();
            Vayu$$ROV();
            DRA7XX$$ROV();
            TDA3XX$$ROV();
            CC32xx$$ROV();
            MSP432$$ROV();
            MSP432E$$ROV();
            CC13xx$$ROV();
            CC26xx$$ROV();
            CortexM$$ROV();
        }//isROV
        $$SINGLETONS();
        Tiva$$SINGLETONS();
        OMAP5430$$SINGLETONS();
        Vayu$$SINGLETONS();
        DRA7XX$$SINGLETONS();
        TDA3XX$$SINGLETONS();
        CC32xx$$SINGLETONS();
        MSP432$$SINGLETONS();
        MSP432E$$SINGLETONS();
        CC13xx$$SINGLETONS();
        CC26xx$$SINGLETONS();
        CortexM$$SINGLETONS();
        $$INITIALIZATION();
    }
}
