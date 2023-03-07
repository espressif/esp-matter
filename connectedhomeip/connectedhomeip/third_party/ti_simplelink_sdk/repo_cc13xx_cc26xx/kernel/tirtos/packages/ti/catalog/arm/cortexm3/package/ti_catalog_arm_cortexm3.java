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

public class ti_catalog_arm_cortexm3
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
        Global.callFxn("loadPackage", xdcO, "xdc.platform");
        Global.callFxn("loadPackage", xdcO, "ti.catalog.peripherals.hdvicp2");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "ti.catalog");
        Global.callFxn("loadPackage", xdcO, "ti.catalog.arm.peripherals.timers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.catalog.arm.cortexm3", new Value.Obj("ti.catalog.arm.cortexm3", pkgP));
    }

    void OMAP4430$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.OMAP4430.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.OMAP4430", new Value.Obj("ti.catalog.arm.cortexm3.OMAP4430", po));
        pkgV.bind("OMAP4430", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.OMAP4430.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.OMAP4430$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.OMAP4430.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.OMAP4430$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.OMAP4430.Params", new Proto.Str(po, true));
    }

    void ITI8168$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI8168.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.ITI8168", new Value.Obj("ti.catalog.arm.cortexm3.ITI8168", po));
        pkgV.bind("ITI8168", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.ITI8168.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI8168$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.ITI8168.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI8168$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.ITI8168.Params", new Proto.Str(po, true));
    }

    void TMS320DM8168$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320DM8168.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320DM8168", new Value.Obj("ti.catalog.arm.cortexm3.TMS320DM8168", po));
        pkgV.bind("TMS320DM8168", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.TMS320DM8168.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320DM8168$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320DM8168.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320DM8168$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320DM8168.Params", new Proto.Str(po, true));
    }

    void TMS320TI816X$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI816X.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI816X", new Value.Obj("ti.catalog.arm.cortexm3.TMS320TI816X", po));
        pkgV.bind("TMS320TI816X", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.TMS320TI816X.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI816X$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320TI816X.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI816X$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320TI816X.Params", new Proto.Str(po, true));
    }

    void TMS320C6A8168$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8168.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8168", new Value.Obj("ti.catalog.arm.cortexm3.TMS320C6A8168", po));
        pkgV.bind("TMS320C6A8168", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.TMS320C6A8168.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8168$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320C6A8168.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8168$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320C6A8168.Params", new Proto.Str(po, true));
    }

    void ITI8148$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI8148.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.ITI8148", new Value.Obj("ti.catalog.arm.cortexm3.ITI8148", po));
        pkgV.bind("ITI8148", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.ITI8148.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI8148$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.ITI8148.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI8148$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.ITI8148.Params", new Proto.Str(po, true));
    }

    void TMS320DM8148$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320DM8148.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320DM8148", new Value.Obj("ti.catalog.arm.cortexm3.TMS320DM8148", po));
        pkgV.bind("TMS320DM8148", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.TMS320DM8148.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320DM8148$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320DM8148.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320DM8148$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320DM8148.Params", new Proto.Str(po, true));
    }

    void TMS320TI814X$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI814X.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI814X", new Value.Obj("ti.catalog.arm.cortexm3.TMS320TI814X", po));
        pkgV.bind("TMS320TI814X", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.TMS320TI814X.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI814X$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320TI814X.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI814X$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320TI814X.Params", new Proto.Str(po, true));
    }

    void CC13xx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CC13xx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.CC13xx", new Value.Obj("ti.catalog.arm.cortexm3.CC13xx", po));
        pkgV.bind("CC13xx", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.CC13xx.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CC13xx$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.CC13xx.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CC13xx$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.CC13xx.Params", new Proto.Str(po, true));
    }

    void CC26xx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CC26xx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.CC26xx", new Value.Obj("ti.catalog.arm.cortexm3.CC26xx", po));
        pkgV.bind("CC26xx", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.CC26xx.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CC26xx$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.CC26xx.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CC26xx$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.CC26xx.Params", new Proto.Str(po, true));
    }

    void F28M35x$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.F28M35x.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.F28M35x", new Value.Obj("ti.catalog.arm.cortexm3.F28M35x", po));
        pkgV.bind("F28M35x", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.F28M35x.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.F28M35x$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.F28M35x.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.F28M35x$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.F28M35x.Params", new Proto.Str(po, true));
    }

    void Tiva$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.Tiva.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.Tiva", new Value.Obj("ti.catalog.arm.cortexm3.Tiva", po));
        pkgV.bind("Tiva", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.Tiva.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.Tiva$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.Tiva.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.Tiva$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.Tiva.Params", new Proto.Str(po, true));
    }

    void CortexM$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CortexM.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.CortexM", new Value.Obj("ti.catalog.arm.cortexm3.CortexM", po));
        pkgV.bind("CortexM", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.CortexM.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CortexM$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.CortexM.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CortexM$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.CortexM.Params", new Proto.Str(po, true));
    }

    void CortexM3$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CortexM3.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.CortexM3", new Value.Obj("ti.catalog.arm.cortexm3.CortexM3", po));
        pkgV.bind("CortexM3", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.CortexM3.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CortexM3$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.CortexM3.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.CortexM3$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.CortexM3.Params", new Proto.Str(po, true));
    }

    void ITI813X$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI813X.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.ITI813X", new Value.Obj("ti.catalog.arm.cortexm3.ITI813X", po));
        pkgV.bind("ITI813X", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.ITI813X.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI813X$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.ITI813X.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI813X$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.ITI813X.Params", new Proto.Str(po, true));
    }

    void TMS320TI813X$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI813X.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI813X", new Value.Obj("ti.catalog.arm.cortexm3.TMS320TI813X", po));
        pkgV.bind("TMS320TI813X", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.TMS320TI813X.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI813X$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320TI813X.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI813X$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320TI813X.Params", new Proto.Str(po, true));
    }

    void ITI811X$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI811X.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.ITI811X", new Value.Obj("ti.catalog.arm.cortexm3.ITI811X", po));
        pkgV.bind("ITI811X", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.ITI811X.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI811X$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.ITI811X.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.ITI811X$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.ITI811X.Params", new Proto.Str(po, true));
    }

    void TMS320TI811X$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI811X.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI811X", new Value.Obj("ti.catalog.arm.cortexm3.TMS320TI811X", po));
        pkgV.bind("TMS320TI811X", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.TMS320TI811X.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI811X$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320TI811X.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320TI811X$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320TI811X.Params", new Proto.Str(po, true));
    }

    void TMS320C6A8149$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8149.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8149", new Value.Obj("ti.catalog.arm.cortexm3.TMS320C6A8149", po));
        pkgV.bind("TMS320C6A8149", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.cortexm3.TMS320C6A8149.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8149$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320C6A8149.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8149$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.cortexm3.TMS320C6A8149.Params", new Proto.Str(po, true));
    }

    void OMAP4430$$CONSTS()
    {
        // module OMAP4430
    }

    void ITI8168$$CONSTS()
    {
        // interface ITI8168
    }

    void TMS320DM8168$$CONSTS()
    {
        // module TMS320DM8168
    }

    void TMS320TI816X$$CONSTS()
    {
        // module TMS320TI816X
    }

    void TMS320C6A8168$$CONSTS()
    {
        // module TMS320C6A8168
    }

    void ITI8148$$CONSTS()
    {
        // interface ITI8148
    }

    void TMS320DM8148$$CONSTS()
    {
        // module TMS320DM8148
    }

    void TMS320TI814X$$CONSTS()
    {
        // module TMS320TI814X
    }

    void CC13xx$$CONSTS()
    {
        // module CC13xx
    }

    void CC26xx$$CONSTS()
    {
        // module CC26xx
    }

    void F28M35x$$CONSTS()
    {
        // module F28M35x
    }

    void Tiva$$CONSTS()
    {
        // module Tiva
    }

    void CortexM$$CONSTS()
    {
        // module CortexM
    }

    void CortexM3$$CONSTS()
    {
        // module CortexM3
    }

    void ITI813X$$CONSTS()
    {
        // interface ITI813X
    }

    void TMS320TI813X$$CONSTS()
    {
        // module TMS320TI813X
    }

    void ITI811X$$CONSTS()
    {
        // interface ITI811X
    }

    void TMS320TI811X$$CONSTS()
    {
        // module TMS320TI811X
    }

    void TMS320C6A8149$$CONSTS()
    {
        // module TMS320C6A8149
    }

    void OMAP4430$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.OMAP4430$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$OMAP4430$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.OMAP4430'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.OMAP4430.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.OMAP4430$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$OMAP4430$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.OMAP4430'];\n");
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

    void ITI8168$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void TMS320DM8168$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320DM8168$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320DM8168$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320DM8168'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.TMS320DM8168.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
            sb.append("__inst.hdvicp1 = __mod.PARAMS.hdvicp1;\n");
            sb.append("__inst.hdvicp2 = __mod.PARAMS.hdvicp2;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320DM8168$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320DM8168$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320DM8168'];\n");
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
            sb.append("__inst.hdvicp1 = __mod.PARAMS.hdvicp1;\n");
            sb.append("__inst.hdvicp2 = __mod.PARAMS.hdvicp2;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void TMS320TI816X$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320TI816X$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320TI816X$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320TI816X'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.TMS320TI816X.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
            sb.append("__inst.hdvicp1 = __mod.PARAMS.hdvicp1;\n");
            sb.append("__inst.hdvicp2 = __mod.PARAMS.hdvicp2;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320TI816X$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320TI816X$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320TI816X'];\n");
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
            sb.append("__inst.hdvicp1 = __mod.PARAMS.hdvicp1;\n");
            sb.append("__inst.hdvicp2 = __mod.PARAMS.hdvicp2;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void TMS320C6A8168$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8168$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320C6A8168$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320C6A8168'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.TMS320C6A8168.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
            sb.append("__inst.hdvicp1 = __mod.PARAMS.hdvicp1;\n");
            sb.append("__inst.hdvicp2 = __mod.PARAMS.hdvicp2;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8168$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320C6A8168$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320C6A8168'];\n");
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
            sb.append("__inst.hdvicp1 = __mod.PARAMS.hdvicp1;\n");
            sb.append("__inst.hdvicp2 = __mod.PARAMS.hdvicp2;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void ITI8148$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void TMS320DM8148$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320DM8148$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320DM8148$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320DM8148'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.TMS320DM8148.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320DM8148$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320DM8148$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320DM8148'];\n");
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

    void TMS320TI814X$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320TI814X$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320TI814X$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320TI814X'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.TMS320TI814X.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320TI814X$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320TI814X$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320TI814X'];\n");
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

    void CC13xx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.CC13xx$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$CC13xx$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.CC13xx'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.CC13xx.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.CC13xx$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CC13xx$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$CC13xx$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.CC13xx'];\n");
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

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.CC26xx$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$CC26xx$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.CC26xx'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.CC26xx.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.CC26xx$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CC26xx$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$CC26xx$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.CC26xx'];\n");
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

    void F28M35x$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.F28M35x$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$F28M35x$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.F28M35x'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.F28M35x.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
            sb.append("__inst.timer0 = __mod.PARAMS.timer0;\n");
            sb.append("__inst.timer1 = __mod.PARAMS.timer1;\n");
            sb.append("__inst.timer2 = __mod.PARAMS.timer2;\n");
            sb.append("__inst.timer3 = __mod.PARAMS.timer3;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [revision]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.F28M35x$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.F28M35x$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$F28M35x$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.F28M35x'];\n");
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
            sb.append("__inst.timer0 = __mod.PARAMS.timer0;\n");
            sb.append("__inst.timer1 = __mod.PARAMS.timer1;\n");
            sb.append("__inst.timer2 = __mod.PARAMS.timer2;\n");
            sb.append("__inst.timer3 = __mod.PARAMS.timer3;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void Tiva$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.Tiva$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.Tiva.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.Tiva.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.Tiva.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$Tiva$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.Tiva'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.Tiva.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.Tiva$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.Tiva.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.Tiva$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.Tiva.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$Tiva$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.Tiva'];\n");
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

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.CortexM$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.CortexM.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.CortexM.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CortexM.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$CortexM$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.CortexM'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.CortexM.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.CortexM$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.CortexM.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CortexM$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CortexM.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$CortexM$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.CortexM'];\n");
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

    void CortexM3$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.CortexM3$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$CortexM3$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.CortexM3'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.CortexM3.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.CortexM3$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CortexM3$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$CortexM3$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.CortexM3'];\n");
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

    void ITI813X$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void TMS320TI813X$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320TI813X$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320TI813X$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320TI813X'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.TMS320TI813X.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320TI813X$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320TI813X$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320TI813X'];\n");
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

    void ITI811X$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void TMS320TI811X$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320TI811X$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320TI811X$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320TI811X'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.TMS320TI811X.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320TI811X$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320TI811X$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320TI811X'];\n");
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

    void TMS320C6A8149$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8149$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Module", "ti.catalog.arm.cortexm3"), om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Instance", "ti.catalog.arm.cortexm3"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320C6A8149$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320C6A8149'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.cortexm3.TMS320C6A8149.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.cortexm3']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.cortexm3.TMS320C6A8149$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Module", "ti.catalog.arm.cortexm3"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149$$Object", "ti.catalog.arm.cortexm3"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Params", "ti.catalog.arm.cortexm3"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$cortexm3$TMS320C6A8149$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.cortexm3.TMS320C6A8149'];\n");
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

    void OMAP4430$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITI8168$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320DM8168$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320TI816X$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320C6A8168$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITI8148$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320DM8148$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320TI814X$$FUNCTIONS()
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

    void F28M35x$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Tiva$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CortexM$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CortexM3$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITI813X$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320TI813X$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITI811X$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320TI811X$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320C6A8149$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP4430$$SIZES()
    {
    }

    void ITI8168$$SIZES()
    {
    }

    void TMS320DM8168$$SIZES()
    {
    }

    void TMS320TI816X$$SIZES()
    {
    }

    void TMS320C6A8168$$SIZES()
    {
    }

    void ITI8148$$SIZES()
    {
    }

    void TMS320DM8148$$SIZES()
    {
    }

    void TMS320TI814X$$SIZES()
    {
    }

    void CC13xx$$SIZES()
    {
    }

    void CC26xx$$SIZES()
    {
    }

    void F28M35x$$SIZES()
    {
    }

    void Tiva$$SIZES()
    {
    }

    void CortexM$$SIZES()
    {
    }

    void CortexM3$$SIZES()
    {
    }

    void ITI813X$$SIZES()
    {
    }

    void TMS320TI813X$$SIZES()
    {
    }

    void ITI811X$$SIZES()
    {
    }

    void TMS320TI811X$$SIZES()
    {
    }

    void TMS320C6A8149$$SIZES()
    {
    }

    void OMAP4430$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/OMAP4430.xs");
        om.bind("ti.catalog.arm.cortexm3.OMAP4430$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.OMAP4430.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$OMAP4430$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$OMAP4430$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.OMAP4430$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.OMAP4430$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.OMAP4430$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.OMAP4430$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.OMAP4430.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00010000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.OMAP4430.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00010000L)})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.OMAP4430.Object", om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Instance", "ti.catalog.arm.cortexm3"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
    }

    void ITI8168$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/ITI8168.xs");
        om.bind("ti.catalog.arm.cortexm3.ITI8168$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI8168.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI8168$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI8168$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI8168$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI8168$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI8168.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("hdvicp1", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("hdvicp2", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_BOOT", Global.newObject("name", "L2_BOOT", "base", 0x00000000L, "len", 0x4000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20004000L, "len", 0x3C000L)}), Global.newArray(new Object[]{"OCMC_0", Global.newObject("name", "OCMC_0", "base", 0x00300000L, "len", 0x40000L)}), Global.newArray(new Object[]{"OCMC_1", Global.newObject("name", "OCMC_1", "base", 0x00400000L, "len", 0x40000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI8168$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI8168.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("hdvicp1", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("hdvicp2", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_BOOT", Global.newObject("name", "L2_BOOT", "base", 0x00000000L, "len", 0x4000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20004000L, "len", 0x3C000L)}), Global.newArray(new Object[]{"OCMC_0", Global.newObject("name", "OCMC_0", "base", 0x00300000L, "len", 0x40000L)}), Global.newArray(new Object[]{"OCMC_1", Global.newObject("name", "OCMC_1", "base", 0x00400000L, "len", 0x40000L)})}), "wh");
    }

    void TMS320DM8168$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320DM8168.Module", om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320DM8168$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320DM8168$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320DM8168.Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320DM8168.Params", om.findStrict("ti.catalog.arm.cortexm3.ITI8168$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320DM8168.Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Instance", "ti.catalog.arm.cortexm3"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
    }

    void TMS320TI816X$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI816X.Module", om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320TI816X$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320TI816X$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI816X.Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI816X.Params", om.findStrict("ti.catalog.arm.cortexm3.ITI8168$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI816X.Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Instance", "ti.catalog.arm.cortexm3"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
    }

    void TMS320C6A8168$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320C6A8168.Module", om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320C6A8168$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320C6A8168$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320C6A8168.Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320C6A8168.Params", om.findStrict("ti.catalog.arm.cortexm3.ITI8168$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320C6A8168.Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Instance", "ti.catalog.arm.cortexm3"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
    }

    void ITI8148$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/ITI8148.xs");
        om.bind("ti.catalog.arm.cortexm3.ITI8148$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI8148.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI8148$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI8148$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI8148$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI8148$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI8148.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_BOOT", Global.newObject("name", "L2_BOOT", "base", 0x00000000L, "len", 0x4000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20004000L, "len", 0x3C000L)}), Global.newArray(new Object[]{"OCMC", Global.newObject("name", "OCMC", "base", 0x00300000L, "len", 0x20000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI8148$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI8148.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_BOOT", Global.newObject("name", "L2_BOOT", "base", 0x00000000L, "len", 0x4000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20004000L, "len", 0x3C000L)}), Global.newArray(new Object[]{"OCMC", Global.newObject("name", "OCMC", "base", 0x00300000L, "len", 0x20000L)})}), "wh");
    }

    void TMS320DM8148$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320DM8148.Module", om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320DM8148$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320DM8148$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320DM8148.Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320DM8148.Params", om.findStrict("ti.catalog.arm.cortexm3.ITI8148$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320DM8148.Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Instance", "ti.catalog.arm.cortexm3"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
    }

    void TMS320TI814X$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI814X.Module", om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320TI814X$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320TI814X$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI814X.Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI814X.Params", om.findStrict("ti.catalog.arm.cortexm3.ITI8148$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI814X.Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Instance", "ti.catalog.arm.cortexm3"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
    }

    void CC13xx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/CC13xx.xs");
        om.bind("ti.catalog.arm.cortexm3.CC13xx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CC13xx.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.CC13xx$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$CC13xx$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.CC13xx$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$CC13xx$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CC13xx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CC13xx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CC13xx$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CC13xx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CC13xx.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC13xx$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CC13xx.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC13xx$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CC13xx.Object", om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Instance", "ti.catalog.arm.cortexm3"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
    }

    void CC26xx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/CC26xx.xs");
        om.bind("ti.catalog.arm.cortexm3.CC26xx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CC26xx.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.CC26xx$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$CC26xx$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.CC26xx$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$CC26xx$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CC26xx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CC26xx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CC26xx$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CC26xx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CC26xx.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC26xx$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CC26xx.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC26xx$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CC26xx.Object", om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Instance", "ti.catalog.arm.cortexm3"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
    }

    void F28M35x$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/F28M35x.xs");
        om.bind("ti.catalog.arm.cortexm3.F28M35x$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.F28M35x.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.F28M35x$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$F28M35x$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.F28M35x$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$F28M35x$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.F28M35x$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.F28M35x$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.F28M35x$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.F28M35x$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.F28M35x.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("timer0", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("timer1", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("timer2", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("timer3", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"BOOTROM", Global.newObject("comment", "On-Chip Boot ROM", "name", "BOOTROM", "base", 0x0L, "len", 0x10000L, "space", "code", "access", "RX")}), Global.newArray(new Object[]{"FLASH_BOOT", Global.newObject("comment", "Branch for boot from Flash", "name", "FLASH_BOOT", "base", 0x200030L, "len", 0x4L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"FLASH", Global.newObject("comment", "512KB Flash memory", "name", "FLASH", "base", 0x200034L, "len", 0x7FFC8L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"OTP", Global.newObject("comment", "4KB OTP memory", "name", "OTP", "base", 0x680000L, "len", 0x1000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"C03SRAM", Global.newObject("comment", "32KB On-Chip RAM Memory", "name", "C03SRAM", "base", 0x20000000L, "len", 0x8000L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"S07SHRAM", Global.newObject("comment", "64KB On-Chip Shared RAM Memory", "name", "S07SHRAM", "base", 0x20008000L, "len", 0x10000L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"CTOMMSGRAM", Global.newObject("comment", "C28 to M3 MSG RAM Memory", "name", "CTOMMSGRAM", "base", 0x2007F000L, "len", 0x800L, "space", "data", "access", "R")}), Global.newArray(new Object[]{"MTOCMSGRAM", Global.newObject("comment", "M3 to C28 MSG RAM Memory", "name", "MTOCMSGRAM", "base", 0x2007F800L, "len", 0x800L, "space", "data", "access", "RW")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.F28M35x$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.F28M35x.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("timer0", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("timer1", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("timer2", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("timer3", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"BOOTROM", Global.newObject("comment", "On-Chip Boot ROM", "name", "BOOTROM", "base", 0x0L, "len", 0x10000L, "space", "code", "access", "RX")}), Global.newArray(new Object[]{"FLASH_BOOT", Global.newObject("comment", "Branch for boot from Flash", "name", "FLASH_BOOT", "base", 0x200030L, "len", 0x4L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"FLASH", Global.newObject("comment", "512KB Flash memory", "name", "FLASH", "base", 0x200034L, "len", 0x7FFC8L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"OTP", Global.newObject("comment", "4KB OTP memory", "name", "OTP", "base", 0x680000L, "len", 0x1000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"C03SRAM", Global.newObject("comment", "32KB On-Chip RAM Memory", "name", "C03SRAM", "base", 0x20000000L, "len", 0x8000L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"S07SHRAM", Global.newObject("comment", "64KB On-Chip Shared RAM Memory", "name", "S07SHRAM", "base", 0x20008000L, "len", 0x10000L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"CTOMMSGRAM", Global.newObject("comment", "C28 to M3 MSG RAM Memory", "name", "CTOMMSGRAM", "base", 0x2007F000L, "len", 0x800L, "space", "data", "access", "R")}), Global.newArray(new Object[]{"MTOCMSGRAM", Global.newObject("comment", "M3 to C28 MSG RAM Memory", "name", "MTOCMSGRAM", "base", 0x2007F800L, "len", 0x800L, "space", "data", "access", "RW")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.F28M35x$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.F28M35x.Object", om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Instance", "ti.catalog.arm.cortexm3"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
    }

    void Tiva$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/Tiva.xs");
        om.bind("ti.catalog.arm.cortexm3.Tiva$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.Tiva.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.Tiva.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.Tiva$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$Tiva$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.Tiva$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$Tiva$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.Tiva$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.Tiva$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.Tiva$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.Tiva$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.Tiva.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.Tiva.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.Tiva$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.Tiva.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.Tiva$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.Tiva.Object", om.findStrict("ti.catalog.arm.cortexm3.Tiva.Instance", "ti.catalog.arm.cortexm3"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
    }

    void CortexM$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/CortexM.xs");
        om.bind("ti.catalog.arm.cortexm3.CortexM$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CortexM.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.CortexM$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$CortexM$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.CortexM$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$CortexM$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CortexM$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CortexM$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CortexM$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CortexM$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CortexM.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "M3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CortexM.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "M3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CortexM.Object", om.findStrict("ti.catalog.arm.cortexm3.CortexM.Instance", "ti.catalog.arm.cortexm3"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
    }

    void CortexM3$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/CortexM3.xs");
        om.bind("ti.catalog.arm.cortexm3.CortexM3$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CortexM3.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.CortexM3$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$CortexM3$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.CortexM3$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$CortexM3$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CortexM3$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CortexM3$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CortexM3$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.CortexM3$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CortexM3.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM3$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CortexM3.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM3$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.CortexM3.Object", om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Instance", "ti.catalog.arm.cortexm3"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
    }

    void ITI813X$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/ITI813X.xs");
        om.bind("ti.catalog.arm.cortexm3.ITI813X$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI813X.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI813X.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI813X$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI813X$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI813X$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI813X$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI813X.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI813X.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_BOOT", Global.newObject("name", "L2_BOOT", "base", 0x00000000L, "len", 0x4000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20004000L, "len", 0x3C000L)}), Global.newArray(new Object[]{"OCMC", Global.newObject("name", "OCMC", "base", 0x00300000L, "len", 0x20000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI813X$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI813X.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_BOOT", Global.newObject("name", "L2_BOOT", "base", 0x00000000L, "len", 0x4000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20004000L, "len", 0x3C000L)}), Global.newArray(new Object[]{"OCMC", Global.newObject("name", "OCMC", "base", 0x00300000L, "len", 0x20000L)})}), "wh");
    }

    void TMS320TI813X$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI813X.Module", om.findStrict("ti.catalog.arm.cortexm3.ITI813X.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320TI813X$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320TI813X$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI813X.Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI813X.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI813X.Params", om.findStrict("ti.catalog.arm.cortexm3.ITI813X$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI813X.Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Instance", "ti.catalog.arm.cortexm3"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
    }

    void ITI811X$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/cortexm3/ITI811X.xs");
        om.bind("ti.catalog.arm.cortexm3.ITI811X$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI811X.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI811X.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI811X$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI811X$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI811X$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.cortexm3.ITI811X$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI811X.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI811X.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_BOOT", Global.newObject("name", "L2_BOOT", "base", 0x00000000L, "len", 0x4000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20004000L, "len", 0x3C000L)}), Global.newArray(new Object[]{"OCMC", Global.newObject("name", "OCMC", "base", 0x00300000L, "len", 0x20000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI811X$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.ITI811X.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("hdvicp0", (Proto)om.findStrict("ti.catalog.peripherals.hdvicp2.HDVICP2.Instance", "ti.catalog.arm.cortexm3"), $$UNDEF, "wh");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm.cortexm3")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_BOOT", Global.newObject("name", "L2_BOOT", "base", 0x00000000L, "len", 0x4000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20004000L, "len", 0x3C000L)}), Global.newArray(new Object[]{"OCMC", Global.newObject("name", "OCMC", "base", 0x00300000L, "len", 0x20000L)})}), "wh");
    }

    void TMS320TI811X$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI811X.Module", om.findStrict("ti.catalog.arm.cortexm3.ITI811X.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320TI811X$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320TI811X$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI811X.Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI811X.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI811X.Params", om.findStrict("ti.catalog.arm.cortexm3.ITI811X$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320TI811X.Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Instance", "ti.catalog.arm.cortexm3"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
    }

    void TMS320C6A8149$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Module", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320C6A8149.Module", om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Module", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149$$create", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320C6A8149$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149$$construct", "ti.catalog.arm.cortexm3"), Global.get("ti$catalog$arm$cortexm3$TMS320C6A8149$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Instance", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320C6A8149.Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Instance", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149$$Params", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320C6A8149.Params", om.findStrict("ti.catalog.arm.cortexm3.ITI8148$$Params", "ti.catalog.arm.cortexm3"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149$$Object", "ti.catalog.arm.cortexm3");
        po.init("ti.catalog.arm.cortexm3.TMS320C6A8149.Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Instance", "ti.catalog.arm.cortexm3"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm.cortexm3"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm.cortexm3"), $$UNDEF);
    }

    void OMAP4430$$ROV()
    {
    }

    void ITI8168$$ROV()
    {
    }

    void TMS320DM8168$$ROV()
    {
    }

    void TMS320TI816X$$ROV()
    {
    }

    void TMS320C6A8168$$ROV()
    {
    }

    void ITI8148$$ROV()
    {
    }

    void TMS320DM8148$$ROV()
    {
    }

    void TMS320TI814X$$ROV()
    {
    }

    void CC13xx$$ROV()
    {
    }

    void CC26xx$$ROV()
    {
    }

    void F28M35x$$ROV()
    {
    }

    void Tiva$$ROV()
    {
    }

    void CortexM$$ROV()
    {
    }

    void CortexM3$$ROV()
    {
    }

    void ITI813X$$ROV()
    {
    }

    void TMS320TI813X$$ROV()
    {
    }

    void ITI811X$$ROV()
    {
    }

    void TMS320TI811X$$ROV()
    {
    }

    void TMS320C6A8149$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.catalog.arm.cortexm3.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.catalog.arm.cortexm3"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.catalog.arm.cortexm3", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.catalog.arm.cortexm3");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.catalog.arm.cortexm3.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.platform", Global.newArray()));
        imports.add(Global.newArray("ti.catalog.peripherals.hdvicp2", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.catalog.arm.cortexm3'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.catalog.arm.cortexm3$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.catalog.arm.cortexm3$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.catalog.arm.cortexm3$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void OMAP4430$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.OMAP4430", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.OMAP4430$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.OMAP4430$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.OMAP4430.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("OMAP4430", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP4430");
    }

    void ITI8168$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI8168", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.ITI8168", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.ITI8168$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.ITI8168.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITI8168", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITI8168");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void TMS320DM8168$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.TMS320DM8168", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm.cortexm3");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.TMS320DM8168$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("TMS320DM8168", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320DM8168");
    }

    void TMS320TI816X$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.TMS320TI816X", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm.cortexm3");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.TMS320TI816X$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("TMS320TI816X", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320TI816X");
    }

    void TMS320C6A8168$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.TMS320C6A8168", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm.cortexm3");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.TMS320C6A8168$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("TMS320C6A8168", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320C6A8168");
    }

    void ITI8148$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI8148", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.ITI8148", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.ITI8148$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.ITI8148.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITI8148", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITI8148");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void TMS320DM8148$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.TMS320DM8148", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm.cortexm3");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.TMS320DM8148$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("TMS320DM8148", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320DM8148");
    }

    void TMS320TI814X$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.TMS320TI814X", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm.cortexm3");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.TMS320TI814X$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("TMS320TI814X", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320TI814X");
    }

    void CC13xx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC13xx", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.CC13xx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.CC13xx$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.CC13xx$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.CC13xx.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("CC13xx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CC13xx");
    }

    void CC26xx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC26xx", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.CC26xx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.CC26xx$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.CC26xx$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.CC26xx.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("CC26xx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CC26xx");
    }

    void F28M35x$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.F28M35x", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.F28M35x", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.F28M35x$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.F28M35x$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.F28M35x.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("F28M35x", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("F28M35x");
    }

    void Tiva$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.Tiva", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.Tiva.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.Tiva", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.Tiva$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.Tiva.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.Tiva.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.Tiva.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.Tiva$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.Tiva.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("Tiva", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Tiva");
    }

    void CortexM$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.CortexM", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.CortexM$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.CortexM.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.CortexM.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.CortexM.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.CortexM$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.CortexM.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("CortexM", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CortexM");
    }

    void CortexM3$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM3", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.CortexM3", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.CortexM3$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.CortexM3$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.CortexM3.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("CortexM3", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CortexM3");
    }

    void ITI813X$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI813X", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI813X.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.ITI813X", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.ITI813X$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI813X.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.ITI813X.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.ITI813X.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITI813X", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITI813X");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void TMS320TI813X$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.TMS320TI813X", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm.cortexm3");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.TMS320TI813X$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("TMS320TI813X", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320TI813X");
    }

    void ITI811X$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI811X", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.ITI811X.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.ITI811X", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.cortexm3.ITI811X$$capsule", "ti.catalog.arm.cortexm3"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.ITI811X.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.ITI811X.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.ITI811X.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITI811X", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITI811X");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void TMS320TI811X$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.TMS320TI811X", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm.cortexm3");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.TMS320TI811X$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("TMS320TI811X", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320TI811X");
    }

    void TMS320C6A8149$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149", "ti.catalog.arm.cortexm3");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Module", "ti.catalog.arm.cortexm3");
        vo.init2(po, "ti.catalog.arm.cortexm3.TMS320C6A8149", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Instance", "ti.catalog.arm.cortexm3"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Params", "ti.catalog.arm.cortexm3"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Params", "ti.catalog.arm.cortexm3")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.cortexm3", "ti.catalog.arm.cortexm3"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm.cortexm3");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.cortexm3")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.cortexm3.TMS320C6A8149$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149.Object", "ti.catalog.arm.cortexm3"));
        pkgV.bind("TMS320C6A8149", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320C6A8149");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.OMAP4430", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8168", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI816X", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8168", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.TMS320DM8148", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI814X", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.CC13xx", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.CC26xx", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.F28M35x", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.Tiva", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.CortexM", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.CortexM3", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI813X", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.TMS320TI811X", "ti.catalog.arm.cortexm3"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.cortexm3.TMS320C6A8149", "ti.catalog.arm.cortexm3"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.OMAP4430")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.ITI8168")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.TMS320DM8168")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.TMS320TI816X")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.TMS320C6A8168")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.ITI8148")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.TMS320DM8148")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.TMS320TI814X")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.CC13xx")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.CC26xx")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.F28M35x")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.Tiva")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.CortexM")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.CortexM3")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.ITI813X")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.TMS320TI813X")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.ITI811X")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.TMS320TI811X")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.cortexm3.TMS320C6A8149")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.catalog.arm.cortexm3")).add(pkgV);
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
        OMAP4430$$OBJECTS();
        ITI8168$$OBJECTS();
        TMS320DM8168$$OBJECTS();
        TMS320TI816X$$OBJECTS();
        TMS320C6A8168$$OBJECTS();
        ITI8148$$OBJECTS();
        TMS320DM8148$$OBJECTS();
        TMS320TI814X$$OBJECTS();
        CC13xx$$OBJECTS();
        CC26xx$$OBJECTS();
        F28M35x$$OBJECTS();
        Tiva$$OBJECTS();
        CortexM$$OBJECTS();
        CortexM3$$OBJECTS();
        ITI813X$$OBJECTS();
        TMS320TI813X$$OBJECTS();
        ITI811X$$OBJECTS();
        TMS320TI811X$$OBJECTS();
        TMS320C6A8149$$OBJECTS();
        OMAP4430$$CONSTS();
        ITI8168$$CONSTS();
        TMS320DM8168$$CONSTS();
        TMS320TI816X$$CONSTS();
        TMS320C6A8168$$CONSTS();
        ITI8148$$CONSTS();
        TMS320DM8148$$CONSTS();
        TMS320TI814X$$CONSTS();
        CC13xx$$CONSTS();
        CC26xx$$CONSTS();
        F28M35x$$CONSTS();
        Tiva$$CONSTS();
        CortexM$$CONSTS();
        CortexM3$$CONSTS();
        ITI813X$$CONSTS();
        TMS320TI813X$$CONSTS();
        ITI811X$$CONSTS();
        TMS320TI811X$$CONSTS();
        TMS320C6A8149$$CONSTS();
        OMAP4430$$CREATES();
        ITI8168$$CREATES();
        TMS320DM8168$$CREATES();
        TMS320TI816X$$CREATES();
        TMS320C6A8168$$CREATES();
        ITI8148$$CREATES();
        TMS320DM8148$$CREATES();
        TMS320TI814X$$CREATES();
        CC13xx$$CREATES();
        CC26xx$$CREATES();
        F28M35x$$CREATES();
        Tiva$$CREATES();
        CortexM$$CREATES();
        CortexM3$$CREATES();
        ITI813X$$CREATES();
        TMS320TI813X$$CREATES();
        ITI811X$$CREATES();
        TMS320TI811X$$CREATES();
        TMS320C6A8149$$CREATES();
        OMAP4430$$FUNCTIONS();
        ITI8168$$FUNCTIONS();
        TMS320DM8168$$FUNCTIONS();
        TMS320TI816X$$FUNCTIONS();
        TMS320C6A8168$$FUNCTIONS();
        ITI8148$$FUNCTIONS();
        TMS320DM8148$$FUNCTIONS();
        TMS320TI814X$$FUNCTIONS();
        CC13xx$$FUNCTIONS();
        CC26xx$$FUNCTIONS();
        F28M35x$$FUNCTIONS();
        Tiva$$FUNCTIONS();
        CortexM$$FUNCTIONS();
        CortexM3$$FUNCTIONS();
        ITI813X$$FUNCTIONS();
        TMS320TI813X$$FUNCTIONS();
        ITI811X$$FUNCTIONS();
        TMS320TI811X$$FUNCTIONS();
        TMS320C6A8149$$FUNCTIONS();
        OMAP4430$$SIZES();
        ITI8168$$SIZES();
        TMS320DM8168$$SIZES();
        TMS320TI816X$$SIZES();
        TMS320C6A8168$$SIZES();
        ITI8148$$SIZES();
        TMS320DM8148$$SIZES();
        TMS320TI814X$$SIZES();
        CC13xx$$SIZES();
        CC26xx$$SIZES();
        F28M35x$$SIZES();
        Tiva$$SIZES();
        CortexM$$SIZES();
        CortexM3$$SIZES();
        ITI813X$$SIZES();
        TMS320TI813X$$SIZES();
        ITI811X$$SIZES();
        TMS320TI811X$$SIZES();
        TMS320C6A8149$$SIZES();
        OMAP4430$$TYPES();
        ITI8168$$TYPES();
        TMS320DM8168$$TYPES();
        TMS320TI816X$$TYPES();
        TMS320C6A8168$$TYPES();
        ITI8148$$TYPES();
        TMS320DM8148$$TYPES();
        TMS320TI814X$$TYPES();
        CC13xx$$TYPES();
        CC26xx$$TYPES();
        F28M35x$$TYPES();
        Tiva$$TYPES();
        CortexM$$TYPES();
        CortexM3$$TYPES();
        ITI813X$$TYPES();
        TMS320TI813X$$TYPES();
        ITI811X$$TYPES();
        TMS320TI811X$$TYPES();
        TMS320C6A8149$$TYPES();
        if (isROV) {
            OMAP4430$$ROV();
            ITI8168$$ROV();
            TMS320DM8168$$ROV();
            TMS320TI816X$$ROV();
            TMS320C6A8168$$ROV();
            ITI8148$$ROV();
            TMS320DM8148$$ROV();
            TMS320TI814X$$ROV();
            CC13xx$$ROV();
            CC26xx$$ROV();
            F28M35x$$ROV();
            Tiva$$ROV();
            CortexM$$ROV();
            CortexM3$$ROV();
            ITI813X$$ROV();
            TMS320TI813X$$ROV();
            ITI811X$$ROV();
            TMS320TI811X$$ROV();
            TMS320C6A8149$$ROV();
        }//isROV
        $$SINGLETONS();
        OMAP4430$$SINGLETONS();
        ITI8168$$SINGLETONS();
        TMS320DM8168$$SINGLETONS();
        TMS320TI816X$$SINGLETONS();
        TMS320C6A8168$$SINGLETONS();
        ITI8148$$SINGLETONS();
        TMS320DM8148$$SINGLETONS();
        TMS320TI814X$$SINGLETONS();
        CC13xx$$SINGLETONS();
        CC26xx$$SINGLETONS();
        F28M35x$$SINGLETONS();
        Tiva$$SINGLETONS();
        CortexM$$SINGLETONS();
        CortexM3$$SINGLETONS();
        ITI813X$$SINGLETONS();
        TMS320TI813X$$SINGLETONS();
        ITI811X$$SINGLETONS();
        TMS320TI811X$$SINGLETONS();
        TMS320C6A8149$$SINGLETONS();
        $$INITIALIZATION();
    }
}
