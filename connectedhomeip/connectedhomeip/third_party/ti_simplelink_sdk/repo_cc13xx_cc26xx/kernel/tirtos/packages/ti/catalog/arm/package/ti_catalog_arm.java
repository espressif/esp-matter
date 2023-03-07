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

public class ti_catalog_arm
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
        Global.callFxn("loadPackage", xdcO, "ti.catalog.arm.peripherals.timers");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "ti.catalog");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.catalog.arm.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.catalog.arm", new Value.Obj("ti.catalog.arm", pkgP));
    }

    void TMS470R10$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS470R10.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS470R10", new Value.Obj("ti.catalog.arm.TMS470R10", po));
        pkgV.bind("TMS470R10", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS470R10.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS470R10$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS470R10.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS470R10$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS470R10.Params", new Proto.Str(po, true));
    }

    void TMS320C5912$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320C5912.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320C5912", new Value.Obj("ti.catalog.arm.TMS320C5912", po));
        pkgV.bind("TMS320C5912", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320C5912.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320C5912$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320C5912.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320C5912$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320C5912.Params", new Proto.Str(po, true));
    }

    void TMS320C1030$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320C1030.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320C1030", new Value.Obj("ti.catalog.arm.TMS320C1030", po));
        pkgV.bind("TMS320C1030", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320C1030.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320C1030$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320C1030.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320C1030$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320C1030.Params", new Proto.Str(po, true));
    }

    void TMS320CDM6446$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM6446.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320CDM6446", new Value.Obj("ti.catalog.arm.TMS320CDM6446", po));
        pkgV.bind("TMS320CDM6446", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320CDM6446.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM6446$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320CDM6446.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM6446$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320CDM6446.Params", new Proto.Str(po, true));
    }

    void TMS320CDM6467$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM6467.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320CDM6467", new Value.Obj("ti.catalog.arm.TMS320CDM6467", po));
        pkgV.bind("TMS320CDM6467", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320CDM6467.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM6467$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320CDM6467.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM6467$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320CDM6467.Params", new Proto.Str(po, true));
    }

    void TMS320DM355$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DM355.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320DM355", new Value.Obj("ti.catalog.arm.TMS320DM355", po));
        pkgV.bind("TMS320DM355", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320DM355.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DM355$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DM355.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DM355$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DM355.Params", new Proto.Str(po, true));
    }

    void TMS320DM357$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DM357.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320DM357", new Value.Obj("ti.catalog.arm.TMS320DM357", po));
        pkgV.bind("TMS320DM357", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320DM357.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DM357$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DM357.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DM357$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DM357.Params", new Proto.Str(po, true));
    }

    void TMS320DM365$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DM365.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320DM365", new Value.Obj("ti.catalog.arm.TMS320DM365", po));
        pkgV.bind("TMS320DM365", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320DM365.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DM365$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DM365.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DM365$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DM365.Params", new Proto.Str(po, true));
    }

    void TMS320CDM510$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM510.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320CDM510", new Value.Obj("ti.catalog.arm.TMS320CDM510", po));
        pkgV.bind("TMS320CDM510", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320CDM510.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM510$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320CDM510.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM510$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320CDM510.Params", new Proto.Str(po, true));
    }

    void IOMAP2x3x$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.IOMAP2x3x.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.IOMAP2x3x", new Value.Obj("ti.catalog.arm.IOMAP2x3x", po));
        pkgV.bind("IOMAP2x3x", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.IOMAP2x3x.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.IOMAP2x3x$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.IOMAP2x3x.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.IOMAP2x3x$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.IOMAP2x3x.Params", new Proto.Str(po, true));
    }

    void OMAP2430$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2430.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP2430", new Value.Obj("ti.catalog.arm.OMAP2430", po));
        pkgV.bind("OMAP2430", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP2430.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2430$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP2430.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2430$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP2430.Params", new Proto.Str(po, true));
    }

    void OMAP2431$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2431.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP2431", new Value.Obj("ti.catalog.arm.OMAP2431", po));
        pkgV.bind("OMAP2431", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP2431.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2431$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP2431.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2431$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP2431.Params", new Proto.Str(po, true));
    }

    void OMAP2530$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2530.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP2530", new Value.Obj("ti.catalog.arm.OMAP2530", po));
        pkgV.bind("OMAP2530", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP2530.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2530$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP2530.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2530$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP2530.Params", new Proto.Str(po, true));
    }

    void OMAP2531$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2531.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP2531", new Value.Obj("ti.catalog.arm.OMAP2531", po));
        pkgV.bind("OMAP2531", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP2531.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2531$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP2531.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP2531$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP2531.Params", new Proto.Str(po, true));
    }

    void IOMAP3xxx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.IOMAP3xxx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.IOMAP3xxx", new Value.Obj("ti.catalog.arm.IOMAP3xxx", po));
        pkgV.bind("IOMAP3xxx", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.IOMAP3xxx.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.IOMAP3xxx$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.IOMAP3xxx.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.IOMAP3xxx$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.IOMAP3xxx.Params", new Proto.Str(po, true));
    }

    void OMAP3403$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3403.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP3403", new Value.Obj("ti.catalog.arm.OMAP3403", po));
        pkgV.bind("OMAP3403", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP3403.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3403$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3403.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3403$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3403.Params", new Proto.Str(po, true));
    }

    void OMAP3405$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3405.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP3405", new Value.Obj("ti.catalog.arm.OMAP3405", po));
        pkgV.bind("OMAP3405", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP3405.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3405$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3405.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3405$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3405.Params", new Proto.Str(po, true));
    }

    void OMAP3503$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3503.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP3503", new Value.Obj("ti.catalog.arm.OMAP3503", po));
        pkgV.bind("OMAP3503", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP3503.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3503$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3503.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3503$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3503.Params", new Proto.Str(po, true));
    }

    void OMAP3505$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3505.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP3505", new Value.Obj("ti.catalog.arm.OMAP3505", po));
        pkgV.bind("OMAP3505", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP3505.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3505$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3505.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3505$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3505.Params", new Proto.Str(po, true));
    }

    void OMAP3515$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3515.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP3515", new Value.Obj("ti.catalog.arm.OMAP3515", po));
        pkgV.bind("OMAP3515", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP3515.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3515$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3515.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3515$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3515.Params", new Proto.Str(po, true));
    }

    void OMAP3517$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3517.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP3517", new Value.Obj("ti.catalog.arm.OMAP3517", po));
        pkgV.bind("OMAP3517", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP3517.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3517$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3517.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3517$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3517.Params", new Proto.Str(po, true));
    }

    void OMAP3425$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3425.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP3425", new Value.Obj("ti.catalog.arm.OMAP3425", po));
        pkgV.bind("OMAP3425", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP3425.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3425$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3425.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3425$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3425.Params", new Proto.Str(po, true));
    }

    void OMAP3525$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3525.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP3525", new Value.Obj("ti.catalog.arm.OMAP3525", po));
        pkgV.bind("OMAP3525", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP3525.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3525$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3525.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3525$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3525.Params", new Proto.Str(po, true));
    }

    void TMS320C3430$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320C3430.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320C3430", new Value.Obj("ti.catalog.arm.TMS320C3430", po));
        pkgV.bind("TMS320C3430", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320C3430.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320C3430$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320C3430.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320C3430$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320C3430.Params", new Proto.Str(po, true));
    }

    void OMAP3530$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3530.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAP3530", new Value.Obj("ti.catalog.arm.OMAP3530", po));
        pkgV.bind("OMAP3530", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAP3530.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3530$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3530.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAP3530$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAP3530.Params", new Proto.Str(po, true));
    }

    void ITMS320DA8xx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.ITMS320DA8xx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.ITMS320DA8xx", new Value.Obj("ti.catalog.arm.ITMS320DA8xx", po));
        pkgV.bind("ITMS320DA8xx", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.ITMS320DA8xx.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.ITMS320DA8xx$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.ITMS320DA8xx.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.ITMS320DA8xx$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.ITMS320DA8xx.Params", new Proto.Str(po, true));
    }

    void TMS320DA828$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA828.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320DA828", new Value.Obj("ti.catalog.arm.TMS320DA828", po));
        pkgV.bind("TMS320DA828", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320DA828.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA828$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DA828.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA828$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DA828.Params", new Proto.Str(po, true));
    }

    void TMS320DA830$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA830.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320DA830", new Value.Obj("ti.catalog.arm.TMS320DA830", po));
        pkgV.bind("TMS320DA830", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320DA830.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA830$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DA830.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA830$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DA830.Params", new Proto.Str(po, true));
    }

    void OMAPL137$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL137.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAPL137", new Value.Obj("ti.catalog.arm.OMAPL137", po));
        pkgV.bind("OMAPL137", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAPL137.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL137$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAPL137.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL137$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAPL137.Params", new Proto.Str(po, true));
    }

    void CortexM3$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.CortexM3.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.CortexM3", new Value.Obj("ti.catalog.arm.CortexM3", po));
        pkgV.bind("CortexM3", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.CortexM3.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.CortexM3$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.CortexM3.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.CortexM3$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.CortexM3.Params", new Proto.Str(po, true));
    }

    void TMS570PSF762$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS570PSF762.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS570PSF762", new Value.Obj("ti.catalog.arm.TMS570PSF762", po));
        pkgV.bind("TMS570PSF762", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS570PSF762.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS570PSF762$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS570PSF762.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS570PSF762$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS570PSF762.Params", new Proto.Str(po, true));
    }

    void TMS570LS20216$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS570LS20216.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS570LS20216", new Value.Obj("ti.catalog.arm.TMS570LS20216", po));
        pkgV.bind("TMS570LS20216", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS570LS20216.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS570LS20216$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS570LS20216.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS570LS20216$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS570LS20216.Params", new Proto.Str(po, true));
    }

    void TMS320DA840$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA840.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320DA840", new Value.Obj("ti.catalog.arm.TMS320DA840", po));
        pkgV.bind("TMS320DA840", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320DA840.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA840$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DA840.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA840$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DA840.Params", new Proto.Str(po, true));
    }

    void TMS320DA850$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA850.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320DA850", new Value.Obj("ti.catalog.arm.TMS320DA850", po));
        pkgV.bind("TMS320DA850", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320DA850.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA850$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DA850.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320DA850$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320DA850.Params", new Proto.Str(po, true));
    }

    void AM1705$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1705.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.AM1705", new Value.Obj("ti.catalog.arm.AM1705", po));
        pkgV.bind("AM1705", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.AM1705.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1705$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.AM1705.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1705$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.AM1705.Params", new Proto.Str(po, true));
    }

    void AM1707$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1707.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.AM1707", new Value.Obj("ti.catalog.arm.AM1707", po));
        pkgV.bind("AM1707", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.AM1707.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1707$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.AM1707.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1707$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.AM1707.Params", new Proto.Str(po, true));
    }

    void AM1806$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1806.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.AM1806", new Value.Obj("ti.catalog.arm.AM1806", po));
        pkgV.bind("AM1806", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.AM1806.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1806$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.AM1806.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1806$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.AM1806.Params", new Proto.Str(po, true));
    }

    void AM1808$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1808.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.AM1808", new Value.Obj("ti.catalog.arm.AM1808", po));
        pkgV.bind("AM1808", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.AM1808.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1808$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.AM1808.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.AM1808$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.AM1808.Params", new Proto.Str(po, true));
    }

    void OMAPL108$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL108.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAPL108", new Value.Obj("ti.catalog.arm.OMAPL108", po));
        pkgV.bind("OMAPL108", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAPL108.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL108$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAPL108.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL108$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAPL108.Params", new Proto.Str(po, true));
    }

    void OMAPL118$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL118.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAPL118", new Value.Obj("ti.catalog.arm.OMAPL118", po));
        pkgV.bind("OMAPL118", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAPL118.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL118$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAPL118.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL118$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAPL118.Params", new Proto.Str(po, true));
    }

    void OMAPL138$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL138.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.OMAPL138", new Value.Obj("ti.catalog.arm.OMAPL138", po));
        pkgV.bind("OMAPL138", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.OMAPL138.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL138$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAPL138.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.OMAPL138$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.OMAPL138.Params", new Proto.Str(po, true));
    }

    void TMS320CDM730$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM730.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320CDM730", new Value.Obj("ti.catalog.arm.TMS320CDM730", po));
        pkgV.bind("TMS320CDM730", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320CDM730.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM730$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320CDM730.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM730$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320CDM730.Params", new Proto.Str(po, true));
    }

    void TMS320CDM740$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM740.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TMS320CDM740", new Value.Obj("ti.catalog.arm.TMS320CDM740", po));
        pkgV.bind("TMS320CDM740", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TMS320CDM740.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM740$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320CDM740.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TMS320CDM740$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TMS320CDM740.Params", new Proto.Str(po, true));
    }

    void TNETV107X$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.TNETV107X.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.TNETV107X", new Value.Obj("ti.catalog.arm.TNETV107X", po));
        pkgV.bind("TNETV107X", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.TNETV107X.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.TNETV107X$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.TNETV107X.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.TNETV107X$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.TNETV107X.Params", new Proto.Str(po, true));
    }

    void TMS470R10$$CONSTS()
    {
        // module TMS470R10
    }

    void TMS320C5912$$CONSTS()
    {
        // module TMS320C5912
    }

    void TMS320C1030$$CONSTS()
    {
        // module TMS320C1030
    }

    void TMS320CDM6446$$CONSTS()
    {
        // module TMS320CDM6446
    }

    void TMS320CDM6467$$CONSTS()
    {
        // module TMS320CDM6467
    }

    void TMS320DM355$$CONSTS()
    {
        // module TMS320DM355
    }

    void TMS320DM357$$CONSTS()
    {
        // module TMS320DM357
    }

    void TMS320DM365$$CONSTS()
    {
        // module TMS320DM365
    }

    void TMS320CDM510$$CONSTS()
    {
        // module TMS320CDM510
    }

    void IOMAP2x3x$$CONSTS()
    {
        // interface IOMAP2x3x
    }

    void OMAP2430$$CONSTS()
    {
        // module OMAP2430
    }

    void OMAP2431$$CONSTS()
    {
        // module OMAP2431
    }

    void OMAP2530$$CONSTS()
    {
        // module OMAP2530
    }

    void OMAP2531$$CONSTS()
    {
        // module OMAP2531
    }

    void IOMAP3xxx$$CONSTS()
    {
        // interface IOMAP3xxx
    }

    void OMAP3403$$CONSTS()
    {
        // module OMAP3403
    }

    void OMAP3405$$CONSTS()
    {
        // module OMAP3405
    }

    void OMAP3503$$CONSTS()
    {
        // module OMAP3503
    }

    void OMAP3505$$CONSTS()
    {
        // module OMAP3505
    }

    void OMAP3515$$CONSTS()
    {
        // module OMAP3515
    }

    void OMAP3517$$CONSTS()
    {
        // module OMAP3517
    }

    void OMAP3425$$CONSTS()
    {
        // module OMAP3425
    }

    void OMAP3525$$CONSTS()
    {
        // module OMAP3525
    }

    void TMS320C3430$$CONSTS()
    {
        // module TMS320C3430
    }

    void OMAP3530$$CONSTS()
    {
        // module OMAP3530
    }

    void ITMS320DA8xx$$CONSTS()
    {
        // interface ITMS320DA8xx
    }

    void TMS320DA828$$CONSTS()
    {
        // module TMS320DA828
    }

    void TMS320DA830$$CONSTS()
    {
        // module TMS320DA830
    }

    void OMAPL137$$CONSTS()
    {
        // module OMAPL137
    }

    void CortexM3$$CONSTS()
    {
        // module CortexM3
    }

    void TMS570PSF762$$CONSTS()
    {
        // module TMS570PSF762
    }

    void TMS570LS20216$$CONSTS()
    {
        // module TMS570LS20216
    }

    void TMS320DA840$$CONSTS()
    {
        // module TMS320DA840
    }

    void TMS320DA850$$CONSTS()
    {
        // module TMS320DA850
    }

    void AM1705$$CONSTS()
    {
        // module AM1705
    }

    void AM1707$$CONSTS()
    {
        // module AM1707
    }

    void AM1806$$CONSTS()
    {
        // module AM1806
    }

    void AM1808$$CONSTS()
    {
        // module AM1808
    }

    void OMAPL108$$CONSTS()
    {
        // module OMAPL108
    }

    void OMAPL118$$CONSTS()
    {
        // module OMAPL118
    }

    void OMAPL138$$CONSTS()
    {
        // module OMAPL138
    }

    void TMS320CDM730$$CONSTS()
    {
        // module TMS320CDM730
    }

    void TMS320CDM740$$CONSTS()
    {
        // module TMS320CDM740
    }

    void TNETV107X$$CONSTS()
    {
        // module TNETV107X
    }

    void TMS470R10$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS470R10$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS470R10.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS470R10.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS470R10.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS470R10$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS470R10'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS470R10.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS470R10$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS470R10.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS470R10$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS470R10.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS470R10$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS470R10'];\n");
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

    void TMS320C5912$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320C5912$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320C5912.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320C5912.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320C5912.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320C5912$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320C5912'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320C5912.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320C5912$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320C5912.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320C5912$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320C5912.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320C5912$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320C5912'];\n");
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

    void TMS320C1030$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320C1030$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320C1030.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320C1030.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320C1030.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320C1030$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320C1030'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320C1030.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320C1030$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320C1030.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320C1030$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320C1030.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320C1030$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320C1030'];\n");
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

    void TMS320CDM6446$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320CDM6446$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320CDM6446.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320CDM6446.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM6446.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320CDM6446$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320CDM6446'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320CDM6446.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320CDM6446$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320CDM6446.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM6446$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM6446.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320CDM6446$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320CDM6446'];\n");
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

    void TMS320CDM6467$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320CDM6467$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320CDM6467.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320CDM6467.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM6467.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320CDM6467$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320CDM6467'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320CDM6467.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320CDM6467$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320CDM6467.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM6467$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM6467.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320CDM6467$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320CDM6467'];\n");
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

    void TMS320DM355$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DM355$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DM355.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320DM355.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DM355.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DM355$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DM355'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320DM355.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DM355$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DM355.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320DM355$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DM355.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DM355$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DM355'];\n");
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

    void TMS320DM357$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DM357$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DM357.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320DM357.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DM357.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DM357$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DM357'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320DM357.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DM357$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DM357.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320DM357$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DM357.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DM357$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DM357'];\n");
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

    void TMS320DM365$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DM365$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DM365.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320DM365.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DM365.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DM365$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DM365'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320DM365.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DM365$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DM365.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320DM365$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DM365.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DM365$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DM365'];\n");
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

    void TMS320CDM510$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320CDM510$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320CDM510.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320CDM510.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM510.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320CDM510$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320CDM510'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320CDM510.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320CDM510$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320CDM510.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM510$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM510.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320CDM510$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320CDM510'];\n");
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

    void IOMAP2x3x$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void OMAP2430$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP2430$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP2430.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP2430.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP2430.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP2430$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP2430'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP2430.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP2430$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP2430.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP2430$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP2430.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP2430$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP2430'];\n");
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

    void OMAP2431$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP2431$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP2431.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP2431.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP2431.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP2431$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP2431'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP2431.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP2431$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP2431.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP2431$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP2431.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP2431$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP2431'];\n");
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

    void OMAP2530$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP2530$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP2530.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP2530.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP2530.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP2530$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP2530'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP2530.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP2530$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP2530.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP2530$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP2530.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP2530$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP2530'];\n");
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

    void OMAP2531$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP2531$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP2531.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP2531.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP2531.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP2531$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP2531'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP2531.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP2531$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP2531.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP2531$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP2531.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP2531$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP2531'];\n");
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

    void IOMAP3xxx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void OMAP3403$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3403$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3403.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP3403.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3403.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3403$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3403'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP3403.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3403$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3403.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP3403$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3403.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3403$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3403'];\n");
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

    void OMAP3405$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3405$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3405.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP3405.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3405.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3405$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3405'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP3405.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3405$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3405.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP3405$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3405.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3405$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3405'];\n");
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

    void OMAP3503$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3503$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3503.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP3503.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3503.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3503$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3503'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP3503.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3503$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3503.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP3503$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3503.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3503$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3503'];\n");
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

    void OMAP3505$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3505$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3505.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP3505.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3505.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3505$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3505'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP3505.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3505$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3505.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP3505$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3505.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3505$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3505'];\n");
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

    void OMAP3515$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3515$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3515.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP3515.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3515.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3515$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3515'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP3515.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3515$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3515.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP3515$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3515.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3515$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3515'];\n");
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

    void OMAP3517$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3517$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3517.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP3517.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3517.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3517$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3517'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP3517.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3517$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3517.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP3517$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3517.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3517$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3517'];\n");
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

    void OMAP3425$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3425$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3425.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP3425.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3425.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3425$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3425'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP3425.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3425$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3425.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP3425$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3425.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3425$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3425'];\n");
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

    void OMAP3525$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3525$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3525.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP3525.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3525.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3525$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3525'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP3525.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3525$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3525.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP3525$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3525.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3525$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3525'];\n");
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

    void TMS320C3430$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320C3430$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320C3430.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320C3430.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320C3430.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320C3430$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320C3430'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320C3430.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320C3430$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320C3430.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320C3430$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320C3430.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320C3430$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320C3430'];\n");
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

    void OMAP3530$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3530$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3530.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAP3530.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3530.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3530$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3530'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAP3530.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAP3530$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAP3530.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAP3530$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAP3530.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAP3530$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAP3530'];\n");
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

    void ITMS320DA8xx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void TMS320DA828$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DA828$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DA828.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320DA828.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DA828.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DA828$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DA828'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320DA828.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DA828$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DA828.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320DA828$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DA828.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DA828$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DA828'];\n");
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

    void TMS320DA830$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DA830$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DA830.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320DA830.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DA830.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DA830$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DA830'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320DA830.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DA830$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DA830.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320DA830$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DA830.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DA830$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DA830'];\n");
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

    void OMAPL137$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAPL137$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAPL137.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAPL137.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAPL137.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAPL137$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAPL137'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAPL137.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAPL137$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAPL137.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAPL137$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAPL137.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAPL137$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAPL137'];\n");
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

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.CortexM3$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.CortexM3.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.CortexM3.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.CortexM3.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$CortexM3$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.CortexM3'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.CortexM3.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.CortexM3$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.CortexM3.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.CortexM3$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.CortexM3.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$CortexM3$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.CortexM3'];\n");
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

    void TMS570PSF762$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS570PSF762$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS570PSF762.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS570PSF762.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS570PSF762.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS570PSF762$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS570PSF762'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS570PSF762.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS570PSF762$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS570PSF762.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS570PSF762$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS570PSF762.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS570PSF762$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS570PSF762'];\n");
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

    void TMS570LS20216$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS570LS20216$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS570LS20216.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS570LS20216.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS570LS20216.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS570LS20216$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS570LS20216'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS570LS20216.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS570LS20216$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS570LS20216.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS570LS20216$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS570LS20216.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS570LS20216$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS570LS20216'];\n");
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

    void TMS320DA840$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DA840$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DA840.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320DA840.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DA840.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DA840$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DA840'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320DA840.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DA840$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DA840.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320DA840$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DA840.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DA840$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DA840'];\n");
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

    void TMS320DA850$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DA850$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DA850.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320DA850.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DA850.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DA850$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DA850'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320DA850.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320DA850$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320DA850.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320DA850$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320DA850.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320DA850$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320DA850'];\n");
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

    void AM1705$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.AM1705$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.AM1705.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.AM1705.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.AM1705.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$AM1705$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.AM1705'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.AM1705.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.AM1705$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.AM1705.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.AM1705$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.AM1705.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$AM1705$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.AM1705'];\n");
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

    void AM1707$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.AM1707$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.AM1707.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.AM1707.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.AM1707.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$AM1707$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.AM1707'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.AM1707.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.AM1707$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.AM1707.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.AM1707$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.AM1707.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$AM1707$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.AM1707'];\n");
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

    void AM1806$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.AM1806$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.AM1806.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.AM1806.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.AM1806.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$AM1806$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.AM1806'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.AM1806.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.AM1806$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.AM1806.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.AM1806$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.AM1806.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$AM1806$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.AM1806'];\n");
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

    void AM1808$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.AM1808$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.AM1808.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.AM1808.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.AM1808.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$AM1808$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.AM1808'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.AM1808.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.AM1808$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.AM1808.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.AM1808$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.AM1808.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$AM1808$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.AM1808'];\n");
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

    void OMAPL108$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAPL108$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAPL108.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAPL108.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAPL108.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAPL108$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAPL108'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAPL108.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAPL108$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAPL108.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAPL108$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAPL108.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAPL108$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAPL108'];\n");
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

    void OMAPL118$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAPL118$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAPL118.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAPL118.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAPL118.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAPL118$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAPL118'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAPL118.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAPL118$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAPL118.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAPL118$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAPL118.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAPL118$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAPL118'];\n");
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

    void OMAPL138$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAPL138$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAPL138.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.OMAPL138.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAPL138.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAPL138$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAPL138'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.OMAPL138.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.OMAPL138$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.OMAPL138.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.OMAPL138$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.OMAPL138.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$OMAPL138$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.OMAPL138'];\n");
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

    void TMS320CDM730$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320CDM730$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320CDM730.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320CDM730.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM730.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320CDM730$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320CDM730'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320CDM730.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320CDM730$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320CDM730.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM730$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM730.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320CDM730$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320CDM730'];\n");
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

    void TMS320CDM740$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320CDM740$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320CDM740.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TMS320CDM740.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM740.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320CDM740$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320CDM740'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TMS320CDM740.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TMS320CDM740$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TMS320CDM740.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM740$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TMS320CDM740.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TMS320CDM740$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TMS320CDM740'];\n");
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

    void TNETV107X$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TNETV107X$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.TNETV107X.Module", "ti.catalog.arm"), om.findStrict("ti.catalog.arm.TNETV107X.Instance", "ti.catalog.arm"), 2, 1, false));
                fxn.addArg(0, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.TNETV107X.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TNETV107X$$create = function( revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TNETV107X'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.TNETV107X.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm']);\n");
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
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.TNETV107X$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.TNETV107X.Module", "ti.catalog.arm"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.TNETV107X$$Object", "ti.catalog.arm"), null);
                fxn.addArg(1, "revision", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.catalog.arm.TNETV107X.Params", "ti.catalog.arm"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$TNETV107X$$construct = function( __obj, revision, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.TNETV107X'];\n");
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

    void TMS470R10$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320C5912$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320C1030$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320CDM6446$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320CDM6467$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320DM355$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320DM357$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320DM365$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320CDM510$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IOMAP2x3x$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP2430$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP2431$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP2530$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP2531$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IOMAP3xxx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP3403$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP3405$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP3503$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP3505$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP3515$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP3517$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP3425$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP3525$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320C3430$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAP3530$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITMS320DA8xx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320DA828$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320DA830$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAPL137$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CortexM3$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS570PSF762$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS570LS20216$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320DA840$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320DA850$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void AM1705$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void AM1707$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void AM1806$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void AM1808$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAPL108$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAPL118$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void OMAPL138$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320CDM730$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS320CDM740$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TNETV107X$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TMS470R10$$SIZES()
    {
    }

    void TMS320C5912$$SIZES()
    {
    }

    void TMS320C1030$$SIZES()
    {
    }

    void TMS320CDM6446$$SIZES()
    {
    }

    void TMS320CDM6467$$SIZES()
    {
    }

    void TMS320DM355$$SIZES()
    {
    }

    void TMS320DM357$$SIZES()
    {
    }

    void TMS320DM365$$SIZES()
    {
    }

    void TMS320CDM510$$SIZES()
    {
    }

    void IOMAP2x3x$$SIZES()
    {
    }

    void OMAP2430$$SIZES()
    {
    }

    void OMAP2431$$SIZES()
    {
    }

    void OMAP2530$$SIZES()
    {
    }

    void OMAP2531$$SIZES()
    {
    }

    void IOMAP3xxx$$SIZES()
    {
    }

    void OMAP3403$$SIZES()
    {
    }

    void OMAP3405$$SIZES()
    {
    }

    void OMAP3503$$SIZES()
    {
    }

    void OMAP3505$$SIZES()
    {
    }

    void OMAP3515$$SIZES()
    {
    }

    void OMAP3517$$SIZES()
    {
    }

    void OMAP3425$$SIZES()
    {
    }

    void OMAP3525$$SIZES()
    {
    }

    void TMS320C3430$$SIZES()
    {
    }

    void OMAP3530$$SIZES()
    {
    }

    void ITMS320DA8xx$$SIZES()
    {
    }

    void TMS320DA828$$SIZES()
    {
    }

    void TMS320DA830$$SIZES()
    {
    }

    void OMAPL137$$SIZES()
    {
    }

    void CortexM3$$SIZES()
    {
    }

    void TMS570PSF762$$SIZES()
    {
    }

    void TMS570LS20216$$SIZES()
    {
    }

    void TMS320DA840$$SIZES()
    {
    }

    void TMS320DA850$$SIZES()
    {
    }

    void AM1705$$SIZES()
    {
    }

    void AM1707$$SIZES()
    {
    }

    void AM1806$$SIZES()
    {
    }

    void AM1808$$SIZES()
    {
    }

    void OMAPL108$$SIZES()
    {
    }

    void OMAPL118$$SIZES()
    {
    }

    void OMAPL138$$SIZES()
    {
    }

    void TMS320CDM730$$SIZES()
    {
    }

    void TMS320CDM740$$SIZES()
    {
    }

    void TNETV107X$$SIZES()
    {
    }

    void TMS470R10$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS470R10.xs");
        om.bind("ti.catalog.arm.TMS470R10$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS470R10.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS470R10.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS470R10$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS470R10$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS470R10$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS470R10$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS470R10$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS470R10$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS470R10$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS470R10$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS470R10.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS470R10.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "470", "wh");
        po.addFld("isa", $$T_Str, "v4T", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"P_MEM", Global.newObject("comment", "Program Memory (ROM)", "name", "P_MEM", "base", 0x00000000L, "len", 0x00030000L, "space", "code")}), Global.newArray(new Object[]{"D_MEM", Global.newObject("comment", "Data Memory (RAM)", "name", "D_MEM", "base", 0x00030000L, "len", 0x00050000L, "space", "data")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS470R10$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS470R10.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "470", "wh");
        po.addFld("isa", $$T_Str, "v4T", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"P_MEM", Global.newObject("comment", "Program Memory (ROM)", "name", "P_MEM", "base", 0x00000000L, "len", 0x00030000L, "space", "code")}), Global.newArray(new Object[]{"D_MEM", Global.newObject("comment", "Data Memory (RAM)", "name", "D_MEM", "base", 0x00030000L, "len", 0x00050000L, "space", "data")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS470R10$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS470R10.Object", om.findStrict("ti.catalog.arm.TMS470R10.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS320C5912$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS320C5912.xs");
        om.bind("ti.catalog.arm.TMS320C5912$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C5912.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C5912.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320C5912$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320C5912$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320C5912$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320C5912$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320C5912$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320C5912$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320C5912$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320C5912$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C5912.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C5912.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C5912$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C5912.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C5912$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C5912.Object", om.findStrict("ti.catalog.arm.TMS320C5912.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS320C1030$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS320C1030.xs");
        om.bind("ti.catalog.arm.TMS320C1030$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C1030.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C1030.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320C1030$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320C1030$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320C1030$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320C1030$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320C1030$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320C1030$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320C1030$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320C1030$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C1030.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C1030.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C1030$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C1030.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C1030$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C1030.Object", om.findStrict("ti.catalog.arm.TMS320C1030.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS320CDM6446$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS320CDM6446.xs");
        om.bind("ti.catalog.arm.TMS320CDM6446$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6446.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM6446.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320CDM6446$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320CDM6446$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320CDM6446$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320CDM6446$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM6446$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM6446$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM6446$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM6446$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6446.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM6446.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM0", Global.newObject("comment", "Internal ARM Instruction RAM0", "name", "IRAM0", "base", 0x00000000L, "len", 0x00002000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"IRAM1", Global.newObject("comment", "Internal ARM Instruction RAM1", "name", "IRAM1", "base", 0x00002000L, "len", 0x00002000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM0", Global.newObject("comment", "Internal ARM Data RAM0", "name", "DRAM0", "base", 0x00008000L, "len", 0x00002000L, "space", "data", "access", "RW")}), Global.newArray(new Object[]{"DRAM1", Global.newObject("comment", "Internal ARM Data RAM1", "name", "DRAM1", "base", 0x0000A000L, "len", 0x00002000L, "space", "data", "access", "RW")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6446$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM6446.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM0", Global.newObject("comment", "Internal ARM Instruction RAM0", "name", "IRAM0", "base", 0x00000000L, "len", 0x00002000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"IRAM1", Global.newObject("comment", "Internal ARM Instruction RAM1", "name", "IRAM1", "base", 0x00002000L, "len", 0x00002000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM0", Global.newObject("comment", "Internal ARM Data RAM0", "name", "DRAM0", "base", 0x00008000L, "len", 0x00002000L, "space", "data", "access", "RW")}), Global.newArray(new Object[]{"DRAM1", Global.newObject("comment", "Internal ARM Data RAM1", "name", "DRAM1", "base", 0x0000A000L, "len", 0x00002000L, "space", "data", "access", "RW")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6446$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM6446.Object", om.findStrict("ti.catalog.arm.TMS320CDM6446.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS320CDM6467$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS320CDM6467.xs");
        om.bind("ti.catalog.arm.TMS320CDM6467$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6467.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM6467.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320CDM6467$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320CDM6467$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320CDM6467$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320CDM6467$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM6467$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM6467$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM6467$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM6467$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6467.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM6467.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM0", Global.newObject("comment", "Internal ARM Instruction RAM0", "name", "IRAM0", "base", 0x00000000L, "len", 0x00002000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"IRAM1", Global.newObject("comment", "Internal ARM Instruction RAM1", "name", "IRAM1", "base", 0x00002000L, "len", 0x00002000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM0", Global.newObject("comment", "Internal ARM Data RAM0", "name", "DRAM0", "base", 0x00008000L, "len", 0x00002000L, "space", "data", "access", "RW")}), Global.newArray(new Object[]{"DRAM1", Global.newObject("comment", "Internal ARM Data RAM1", "name", "DRAM1", "base", 0x0000A000L, "len", 0x00002000L, "space", "data", "access", "RW")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6467$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM6467.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM0", Global.newObject("comment", "Internal ARM Instruction RAM0", "name", "IRAM0", "base", 0x00000000L, "len", 0x00002000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"IRAM1", Global.newObject("comment", "Internal ARM Instruction RAM1", "name", "IRAM1", "base", 0x00002000L, "len", 0x00002000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM0", Global.newObject("comment", "Internal ARM Data RAM0", "name", "DRAM0", "base", 0x00008000L, "len", 0x00002000L, "space", "data", "access", "RW")}), Global.newArray(new Object[]{"DRAM1", Global.newObject("comment", "Internal ARM Data RAM1", "name", "DRAM1", "base", 0x0000A000L, "len", 0x00002000L, "space", "data", "access", "RW")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6467$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM6467.Object", om.findStrict("ti.catalog.arm.TMS320CDM6467.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS320DM355$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS320DM355.xs");
        om.bind("ti.catalog.arm.TMS320DM355$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM355.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM355.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DM355$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DM355$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DM355$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DM355$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM355$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM355$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM355$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM355$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM355.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM355.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM0", Global.newObject("comment", "Internal ARM Instruction RAM0", "name", "IRAM0", "base", 0x00000000L, "len", 0x00004000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM1", Global.newObject("comment", "Internal ARM Data RAM1", "name", "DRAM1", "base", 0x00014000L, "len", 0x00004000L, "space", "data", "access", "RW")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM355$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM355.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM0", Global.newObject("comment", "Internal ARM Instruction RAM0", "name", "IRAM0", "base", 0x00000000L, "len", 0x00004000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM1", Global.newObject("comment", "Internal ARM Data RAM1", "name", "DRAM1", "base", 0x00014000L, "len", 0x00004000L, "space", "data", "access", "RW")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM355$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM355.Object", om.findStrict("ti.catalog.arm.TMS320DM355.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS320DM357$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS320DM357.xs");
        om.bind("ti.catalog.arm.TMS320DM357$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM357.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM357.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DM357$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DM357$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DM357$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DM357$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM357$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM357$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM357$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM357$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM357.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM357.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM0", Global.newObject("comment", "Internal ARM Instruction RAM0", "name", "IRAM0", "base", 0x00000000L, "len", 0x00004000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM1", Global.newObject("comment", "Internal ARM Data RAM1", "name", "DRAM1", "base", 0x00014000L, "len", 0x00004000L, "space", "data", "access", "RW")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM357$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM357.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM0", Global.newObject("comment", "Internal ARM Instruction RAM0", "name", "IRAM0", "base", 0x00000000L, "len", 0x00004000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM1", Global.newObject("comment", "Internal ARM Data RAM1", "name", "DRAM1", "base", 0x00014000L, "len", 0x00004000L, "space", "data", "access", "RW")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM357$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM357.Object", om.findStrict("ti.catalog.arm.TMS320DM357.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS320DM365$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS320DM365.xs");
        om.bind("ti.catalog.arm.TMS320DM365$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM365.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM365.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DM365$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DM365$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DM365$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DM365$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM365$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM365$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM365$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320DM365$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM365.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM365.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM0", Global.newObject("comment", "Internal ARM Instruction RAM0", "name", "IRAM0", "base", 0x00000000L, "len", 0x00004000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM1", Global.newObject("comment", "Internal ARM Data RAM1", "name", "DRAM1", "base", 0x00014000L, "len", 0x00004000L, "space", "data", "access", "RW")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM365$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM365.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM0", Global.newObject("comment", "Internal ARM Instruction RAM0", "name", "IRAM0", "base", 0x00000000L, "len", 0x00004000L, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM1", Global.newObject("comment", "Internal ARM Data RAM1", "name", "DRAM1", "base", 0x00014000L, "len", 0x00004000L, "space", "data", "access", "RW")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM365$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DM365.Object", om.findStrict("ti.catalog.arm.TMS320DM365.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS320CDM510$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS320CDM510.xs");
        om.bind("ti.catalog.arm.TMS320CDM510$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM510.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM510.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320CDM510$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320CDM510$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320CDM510$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320CDM510$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM510$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM510$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM510$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM510$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM510.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM510.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM", Global.newObject("comment", "Internal ARM Instruction RAM", "name", "IRAM", "base", 0x00000004L, "len", 0x00007ffcL, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM", Global.newObject("comment", "Internal ARM Data RAM", "name", "DRAM", "base", 0x00010004L, "len", 0x00007ffcL, "space", "data", "access", "RW")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM510$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM510.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm9", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM", Global.newObject("comment", "Internal ARM Instruction RAM", "name", "IRAM", "base", 0x00000004L, "len", 0x00007ffcL, "space", "code", "access", "RWX")}), Global.newArray(new Object[]{"DRAM", Global.newObject("comment", "Internal ARM Data RAM", "name", "DRAM", "base", 0x00010004L, "len", 0x00007ffcL, "space", "data", "access", "RW")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM510$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM510.Object", om.findStrict("ti.catalog.arm.TMS320CDM510.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void IOMAP2x3x$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/IOMAP2x3x.xs");
        om.bind("ti.catalog.arm.IOMAP2x3x$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.IOMAP2x3x.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.IOMAP2x3x.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.IOMAP2x3x$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.IOMAP2x3x$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.IOMAP2x3x$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.IOMAP2x3x$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.IOMAP2x3x.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.IOMAP2x3x.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "v6", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "2.1", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"SRAM", Global.newObject("comment", "On-Chip SRAM", "name", "SRAM", "base", 0x40200000L, "len", 0x0000F800L, "space", "code/data", "access", "RWX")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.IOMAP2x3x$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.IOMAP2x3x.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "v6", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "2.1", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"SRAM", Global.newObject("comment", "On-Chip SRAM", "name", "SRAM", "base", 0x40200000L, "len", 0x0000F800L, "space", "code/data", "access", "RWX")})}), "wh");
    }

    void OMAP2430$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2430.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2430.Module", om.findStrict("ti.catalog.arm.IOMAP2x3x.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP2430$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP2430$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP2430$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP2430$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2430.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2430.Instance", om.findStrict("ti.catalog.arm.IOMAP2x3x.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2430$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2430.Params", om.findStrict("ti.catalog.arm.IOMAP2x3x$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2430$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2430.Object", om.findStrict("ti.catalog.arm.OMAP2430.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP2431$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2431.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2431.Module", om.findStrict("ti.catalog.arm.IOMAP2x3x.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP2431$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP2431$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP2431$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP2431$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2431.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2431.Instance", om.findStrict("ti.catalog.arm.IOMAP2x3x.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2431$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2431.Params", om.findStrict("ti.catalog.arm.IOMAP2x3x$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2431$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2431.Object", om.findStrict("ti.catalog.arm.OMAP2431.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP2530$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2530.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2530.Module", om.findStrict("ti.catalog.arm.IOMAP2x3x.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP2530$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP2530$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP2530$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP2530$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2530.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2530.Instance", om.findStrict("ti.catalog.arm.IOMAP2x3x.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2530$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2530.Params", om.findStrict("ti.catalog.arm.IOMAP2x3x$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2530$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2530.Object", om.findStrict("ti.catalog.arm.OMAP2530.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP2531$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2531.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2531.Module", om.findStrict("ti.catalog.arm.IOMAP2x3x.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP2531$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP2531$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP2531$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP2531$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2531.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2531.Instance", om.findStrict("ti.catalog.arm.IOMAP2x3x.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2531$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2531.Params", om.findStrict("ti.catalog.arm.IOMAP2x3x$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2531$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP2531.Object", om.findStrict("ti.catalog.arm.OMAP2531.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void IOMAP3xxx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/IOMAP3xxx.xs");
        om.bind("ti.catalog.arm.IOMAP3xxx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.IOMAP3xxx.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.IOMAP3xxx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.IOMAP3xxx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.IOMAP3xxx$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.IOMAP3xxx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.IOMAP3xxx.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "v7A", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"SRAM", Global.newObject("comment", "On-Chip SRAM", "name", "SRAM", "base", 0x40200000L, "len", 0x00010000L, "space", "code/data", "access", "RWX")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.IOMAP3xxx.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "v7A", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"SRAM", Global.newObject("comment", "On-Chip SRAM", "name", "SRAM", "base", 0x40200000L, "len", 0x00010000L, "space", "code/data", "access", "RWX")})}), "wh");
    }

    void OMAP3403$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3403.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3403.Module", om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3403$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3403$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3403$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3403$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3403.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3403.Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3403$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3403.Params", om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3403$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3403.Object", om.findStrict("ti.catalog.arm.OMAP3403.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP3405$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3405.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3405.Module", om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3405$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3405$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3405$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3405$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3405.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3405.Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3405$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3405.Params", om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3405$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3405.Object", om.findStrict("ti.catalog.arm.OMAP3405.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP3503$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3503.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3503.Module", om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3503$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3503$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3503$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3503$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3503.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3503.Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3503$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3503.Params", om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3503$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3503.Object", om.findStrict("ti.catalog.arm.OMAP3503.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP3505$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3505.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3505.Module", om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3505$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3505$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3505$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3505$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3505.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3505.Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3505$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3505.Params", om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3505$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3505.Object", om.findStrict("ti.catalog.arm.OMAP3505.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP3515$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3515.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3515.Module", om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3515$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3515$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3515$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3515$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3515.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3515.Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3515$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3515.Params", om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3515$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3515.Object", om.findStrict("ti.catalog.arm.OMAP3515.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP3517$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3517.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3517.Module", om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3517$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3517$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3517$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3517$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3517.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3517.Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3517$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3517.Params", om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3517$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3517.Object", om.findStrict("ti.catalog.arm.OMAP3517.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP3425$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3425.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3425.Module", om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3425$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3425$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3425$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3425$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3425.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3425.Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3425$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3425.Params", om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3425$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3425.Object", om.findStrict("ti.catalog.arm.OMAP3425.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP3525$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3525.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3525.Module", om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3525$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3525$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3525$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3525$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3525.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3525.Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3525$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3525.Params", om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3525$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3525.Object", om.findStrict("ti.catalog.arm.OMAP3525.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void TMS320C3430$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C3430.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C3430.Module", om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320C3430$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320C3430$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320C3430$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320C3430$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C3430.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C3430.Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C3430$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C3430.Params", om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C3430$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320C3430.Object", om.findStrict("ti.catalog.arm.TMS320C3430.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAP3530$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3530.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3530.Module", om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3530$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3530$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAP3530$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAP3530$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3530.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3530.Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3530$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3530.Params", om.findStrict("ti.catalog.arm.IOMAP3xxx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3530$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAP3530.Object", om.findStrict("ti.catalog.arm.OMAP3530.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void ITMS320DA8xx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/ITMS320DA8xx.xs");
        om.bind("ti.catalog.arm.ITMS320DA8xx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.ITMS320DA8xx.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.ITMS320DA8xx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.ITMS320DA8xx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.ITMS320DA8xx$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.ITMS320DA8xx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.ITMS320DA8xx.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "v5T", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM", Global.newObject("comment", "8KB ARM local RAM", "name", "IRAM", "base", 0xFFFF0000L, "len", 0x00002000L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"L3_CBA_RAM", Global.newObject("comment", "128KB ARM/DSP local shared RAM", "name", "L3_CBA_RAM", "base", 0x80000000L, "len", 0x00020000L, "space", "code/data", "access", "RWX")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.ITMS320DA8xx.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "v5T", "wh");
        po.addFld("isa", $$T_Str, "v5T", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"IRAM", Global.newObject("comment", "8KB ARM local RAM", "name", "IRAM", "base", 0xFFFF0000L, "len", 0x00002000L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"L3_CBA_RAM", Global.newObject("comment", "128KB ARM/DSP local shared RAM", "name", "L3_CBA_RAM", "base", 0x80000000L, "len", 0x00020000L, "space", "code/data", "access", "RWX")})}), "wh");
    }

    void TMS320DA828$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA828.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA828.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DA828$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DA828$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DA828$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DA828$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA828.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA828.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA828$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA828.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA828$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA828.Object", om.findStrict("ti.catalog.arm.TMS320DA828.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void TMS320DA830$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA830.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA830.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DA830$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DA830$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DA830$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DA830$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA830.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA830.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA830$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA830.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA830$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA830.Object", om.findStrict("ti.catalog.arm.TMS320DA830.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAPL137$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL137.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL137.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAPL137$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAPL137$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAPL137$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAPL137$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL137.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL137.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL137$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL137.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL137$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL137.Object", om.findStrict("ti.catalog.arm.OMAPL137.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void CortexM3$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/CortexM3.xs");
        om.bind("ti.catalog.arm.CortexM3$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.CortexM3.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.CortexM3.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.CortexM3$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$CortexM3$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.CortexM3$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$CortexM3$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.CortexM3$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.CortexM3$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.CortexM3$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.CortexM3$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.CortexM3.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.CortexM3.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.CortexM3$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.CortexM3.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.CortexM3$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.CortexM3.Object", om.findStrict("ti.catalog.arm.CortexM3.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS570PSF762$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS570PSF762.xs");
        om.bind("ti.catalog.arm.TMS570PSF762$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS570PSF762.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS570PSF762.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS570PSF762$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS570PSF762$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS570PSF762$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS570PSF762$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS570PSF762$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS570PSF762$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS570PSF762$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS570PSF762$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS570PSF762.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS570PSF762.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "R4", "wh");
        po.addFld("isa", $$T_Str, "v7R", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"LO_FLASH", Global.newObject("name", "FLASH", "base", 0x00000000L, "len", 0x00080000L, "space", "code", "access", "RX")}), Global.newArray(new Object[]{"HI_IRAM", Global.newObject("name", "IRAM", "base", 0x08000000L, "len", 0x0000A000L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"HI_FLASH", Global.newObject("name", "FLASH", "base", 0x20000000L, "len", 0x00080000L, "space", "code", "access", "RX")}), Global.newArray(new Object[]{"LO_IRAM", Global.newObject("name", "IRAM", "base", 0x00000000L, "len", 0x0000A000L, "space", "code/data", "access", "RWX")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS570PSF762$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS570PSF762.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "R4", "wh");
        po.addFld("isa", $$T_Str, "v7R", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"LO_FLASH", Global.newObject("name", "FLASH", "base", 0x00000000L, "len", 0x00080000L, "space", "code", "access", "RX")}), Global.newArray(new Object[]{"HI_IRAM", Global.newObject("name", "IRAM", "base", 0x08000000L, "len", 0x0000A000L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"HI_FLASH", Global.newObject("name", "FLASH", "base", 0x20000000L, "len", 0x00080000L, "space", "code", "access", "RX")}), Global.newArray(new Object[]{"LO_IRAM", Global.newObject("name", "IRAM", "base", 0x00000000L, "len", 0x0000A000L, "space", "code/data", "access", "RWX")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS570PSF762$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS570PSF762.Object", om.findStrict("ti.catalog.arm.TMS570PSF762.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS570LS20216$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS570LS20216.xs");
        om.bind("ti.catalog.arm.TMS570LS20216$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS570LS20216.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS570LS20216.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS570LS20216$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS570LS20216$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS570LS20216$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS570LS20216$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS570LS20216$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS570LS20216$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS570LS20216$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS570LS20216$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS570LS20216.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS570LS20216.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "R4", "wh");
        po.addFld("isa", $$T_Str, "v7R", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"LO_FLASH", Global.newObject("name", "FLASH", "base", 0x00000000L, "len", 0x00200000L, "space", "code", "access", "RX")}), Global.newArray(new Object[]{"HI_IRAM", Global.newObject("name", "IRAM", "base", 0x08000000L, "len", 0x00028000L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"HI_FLASH", Global.newObject("name", "FLASH", "base", 0x20000000L, "len", 0x00080000L, "space", "code", "access", "RX")}), Global.newArray(new Object[]{"LO_IRAM", Global.newObject("name", "IRAM", "base", 0x00000000L, "len", 0x00028000L, "space", "code/data", "access", "RWX")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS570LS20216$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS570LS20216.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "R4", "wh");
        po.addFld("isa", $$T_Str, "v7R", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"LO_FLASH", Global.newObject("name", "FLASH", "base", 0x00000000L, "len", 0x00200000L, "space", "code", "access", "RX")}), Global.newArray(new Object[]{"HI_IRAM", Global.newObject("name", "IRAM", "base", 0x08000000L, "len", 0x00028000L, "space", "code/data", "access", "RWX")}), Global.newArray(new Object[]{"HI_FLASH", Global.newObject("name", "FLASH", "base", 0x20000000L, "len", 0x00080000L, "space", "code", "access", "RX")}), Global.newArray(new Object[]{"LO_IRAM", Global.newObject("name", "IRAM", "base", 0x00000000L, "len", 0x00028000L, "space", "code/data", "access", "RWX")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS570LS20216$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS570LS20216.Object", om.findStrict("ti.catalog.arm.TMS570LS20216.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS320DA840$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA840.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA840.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DA840$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DA840$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DA840$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DA840$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA840.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA840.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA840$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA840.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA840$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA840.Object", om.findStrict("ti.catalog.arm.TMS320DA840.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void TMS320DA850$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA850.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA850.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DA850$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DA850$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320DA850$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320DA850$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA850.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA850.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA850$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA850.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA850$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320DA850.Object", om.findStrict("ti.catalog.arm.TMS320DA850.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void AM1705$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1705.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1705.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.AM1705$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$AM1705$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.AM1705$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$AM1705$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1705.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1705.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1705$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1705.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1705$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1705.Object", om.findStrict("ti.catalog.arm.AM1705.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void AM1707$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1707.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1707.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.AM1707$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$AM1707$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.AM1707$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$AM1707$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1707.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1707.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1707$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1707.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1707$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1707.Object", om.findStrict("ti.catalog.arm.AM1707.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void AM1806$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1806.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1806.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.AM1806$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$AM1806$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.AM1806$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$AM1806$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1806.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1806.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1806$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1806.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1806$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1806.Object", om.findStrict("ti.catalog.arm.AM1806.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void AM1808$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1808.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1808.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.AM1808$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$AM1808$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.AM1808$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$AM1808$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1808.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1808.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1808$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1808.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1808$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.AM1808.Object", om.findStrict("ti.catalog.arm.AM1808.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAPL108$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL108.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL108.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAPL108$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAPL108$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAPL108$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAPL108$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL108.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL108.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL108$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL108.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL108$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL108.Object", om.findStrict("ti.catalog.arm.OMAPL108.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAPL118$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL118.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL118.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAPL118$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAPL118$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAPL118$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAPL118$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL118.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL118.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL118$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL118.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL118$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL118.Object", om.findStrict("ti.catalog.arm.OMAPL118.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void OMAPL138$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL138.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL138.Module", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAPL138$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAPL138$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.OMAPL138$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$OMAPL138$$construct"));
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL138.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL138.Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL138$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL138.Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL138$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.OMAPL138.Object", om.findStrict("ti.catalog.arm.OMAPL138.Instance", "ti.catalog.arm"));
                po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), $$UNDEF);
                po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), $$UNDEF);
    }

    void TMS320CDM730$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS320CDM730.xs");
        om.bind("ti.catalog.arm.TMS320CDM730$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM730.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM730.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320CDM730$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320CDM730$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320CDM730$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320CDM730$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM730$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM730$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM730$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM730$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM730.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM730.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00040000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM730$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM730.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00040000L)})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM730$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM730.Object", om.findStrict("ti.catalog.arm.TMS320CDM730.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS320CDM740$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TMS320CDM740.xs");
        om.bind("ti.catalog.arm.TMS320CDM740$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM740.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM740.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320CDM740$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320CDM740$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TMS320CDM740$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TMS320CDM740$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM740$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM740$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM740$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TMS320CDM740$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM740.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM740.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00040000L)})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM740$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM740.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "CM3", "wh");
        po.addFld("isa", $$T_Str, "v7M", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"L2_ROM", Global.newObject("name", "L2_ROM", "base", 0x00000000L, "len", 0x00004000L)}), Global.newArray(new Object[]{"L2_RAM", Global.newObject("name", "L2_RAM", "base", 0x20000000L, "len", 0x00040000L)})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM740$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TMS320CDM740.Object", om.findStrict("ti.catalog.arm.TMS320CDM740.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TNETV107X$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/TNETV107X.xs");
        om.bind("ti.catalog.arm.TNETV107X$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TNETV107X.Module", "ti.catalog.arm");
        po.init("ti.catalog.arm.TNETV107X.Module", om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.TNETV107X$$create", "ti.catalog.arm"), Global.get("ti$catalog$arm$TNETV107X$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.TNETV107X$$construct", "ti.catalog.arm"), Global.get("ti$catalog$arm$TNETV107X$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.catalog.arm.TNETV107X$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TNETV107X$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.catalog.arm.TNETV107X$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.catalog.arm.TNETV107X$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TNETV107X.Instance", "ti.catalog.arm");
        po.init("ti.catalog.arm.TNETV107X.Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm11", "wh");
        po.addFld("isa", $$T_Str, "v6", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"SRAM", Global.newObject("comment", "On-Chip SRAM", "name", "SRAM", "base", 0x20000000L, "len", 0x00010000L, "space", "code/data", "access", "RWX")})}), "wh");
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TNETV107X$$Params", "ti.catalog.arm");
        po.init("ti.catalog.arm.TNETV107X.Params", om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("cpuCore", $$T_Str, "Arm11", "wh");
        po.addFld("isa", $$T_Str, "v6", "wh");
        po.addFld("cpuCoreRevision", $$T_Str, "1.0", "wh");
        po.addFld("minProgUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("minDataUnitSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("dataWordSize", Proto.Elm.newCNum("(xdc_Int)"), 4L, "wh");
        po.addFld("memMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.catalog.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"SRAM", Global.newObject("comment", "On-Chip SRAM", "name", "SRAM", "base", 0x20000000L, "len", 0x00010000L, "space", "code/data", "access", "RWX")})}), "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TNETV107X$$Object", "ti.catalog.arm");
        po.init("ti.catalog.arm.TNETV107X.Object", om.findStrict("ti.catalog.arm.TNETV107X.Instance", "ti.catalog.arm"));
                fxn = Global.get(cap, "getMemoryMap");
                if (fxn != null) po.addFxn("getMemoryMap", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getMemoryMap", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getRegisterSet");
                if (fxn != null) po.addFxn("getRegisterSet", (Proto.Fxn)om.findStrict("xdc.platform.ICpuDataSheet$$getRegisterSet", "ti.catalog.arm"), fxn);
    }

    void TMS470R10$$ROV()
    {
    }

    void TMS320C5912$$ROV()
    {
    }

    void TMS320C1030$$ROV()
    {
    }

    void TMS320CDM6446$$ROV()
    {
    }

    void TMS320CDM6467$$ROV()
    {
    }

    void TMS320DM355$$ROV()
    {
    }

    void TMS320DM357$$ROV()
    {
    }

    void TMS320DM365$$ROV()
    {
    }

    void TMS320CDM510$$ROV()
    {
    }

    void IOMAP2x3x$$ROV()
    {
    }

    void OMAP2430$$ROV()
    {
    }

    void OMAP2431$$ROV()
    {
    }

    void OMAP2530$$ROV()
    {
    }

    void OMAP2531$$ROV()
    {
    }

    void IOMAP3xxx$$ROV()
    {
    }

    void OMAP3403$$ROV()
    {
    }

    void OMAP3405$$ROV()
    {
    }

    void OMAP3503$$ROV()
    {
    }

    void OMAP3505$$ROV()
    {
    }

    void OMAP3515$$ROV()
    {
    }

    void OMAP3517$$ROV()
    {
    }

    void OMAP3425$$ROV()
    {
    }

    void OMAP3525$$ROV()
    {
    }

    void TMS320C3430$$ROV()
    {
    }

    void OMAP3530$$ROV()
    {
    }

    void ITMS320DA8xx$$ROV()
    {
    }

    void TMS320DA828$$ROV()
    {
    }

    void TMS320DA830$$ROV()
    {
    }

    void OMAPL137$$ROV()
    {
    }

    void CortexM3$$ROV()
    {
    }

    void TMS570PSF762$$ROV()
    {
    }

    void TMS570LS20216$$ROV()
    {
    }

    void TMS320DA840$$ROV()
    {
    }

    void TMS320DA850$$ROV()
    {
    }

    void AM1705$$ROV()
    {
    }

    void AM1707$$ROV()
    {
    }

    void AM1806$$ROV()
    {
    }

    void AM1808$$ROV()
    {
    }

    void OMAPL108$$ROV()
    {
    }

    void OMAPL118$$ROV()
    {
    }

    void OMAPL138$$ROV()
    {
    }

    void TMS320CDM730$$ROV()
    {
    }

    void TMS320CDM740$$ROV()
    {
    }

    void TNETV107X$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.catalog.arm.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.catalog.arm"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/catalog/arm/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.catalog.arm"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.catalog.arm"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.catalog.arm", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.catalog.arm");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.catalog.arm.");
        pkgV.bind("$vers", Global.newArray(1, 0, 1));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.platform", Global.newArray()));
        imports.add(Global.newArray("ti.catalog.arm.peripherals.timers", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.catalog.arm'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.catalog.arm$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.catalog.arm$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.catalog.arm$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void TMS470R10$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS470R10", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS470R10.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS470R10", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS470R10$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS470R10.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS470R10.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS470R10.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS470R10$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS470R10.Object", "ti.catalog.arm"));
        pkgV.bind("TMS470R10", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS470R10");
    }

    void TMS320C5912$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320C5912", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C5912.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320C5912", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS320C5912$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320C5912.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320C5912.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320C5912.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320C5912$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320C5912.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320C5912", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320C5912");
    }

    void TMS320C1030$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320C1030", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C1030.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320C1030", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS320C1030$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320C1030.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320C1030.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320C1030.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320C1030$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320C1030.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320C1030", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320C1030");
    }

    void TMS320CDM6446$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6446", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6446.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320CDM6446", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS320CDM6446$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320CDM6446.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320CDM6446.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320CDM6446.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320CDM6446$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320CDM6446.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320CDM6446", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320CDM6446");
    }

    void TMS320CDM6467$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6467", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM6467.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320CDM6467", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS320CDM6467$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320CDM6467.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320CDM6467.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320CDM6467.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320CDM6467$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320CDM6467.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320CDM6467", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320CDM6467");
    }

    void TMS320DM355$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320DM355", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM355.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320DM355", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS320DM355$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320DM355.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320DM355.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320DM355.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320DM355$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320DM355.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320DM355", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320DM355");
    }

    void TMS320DM357$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320DM357", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM357.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320DM357", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS320DM357$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320DM357.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320DM357.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320DM357.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320DM357$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320DM357.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320DM357", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320DM357");
    }

    void TMS320DM365$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320DM365", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DM365.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320DM365", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS320DM365$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320DM365.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320DM365.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320DM365.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320DM365$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320DM365.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320DM365", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320DM365");
    }

    void TMS320CDM510$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320CDM510", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM510.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320CDM510", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS320CDM510$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320CDM510.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320CDM510.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320CDM510.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320CDM510$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320CDM510.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320CDM510", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320CDM510");
    }

    void IOMAP2x3x$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.IOMAP2x3x", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.IOMAP2x3x.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.IOMAP2x3x", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.IOMAP2x3x$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.IOMAP2x3x.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.IOMAP2x3x.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.IOMAP2x3x.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IOMAP2x3x", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IOMAP2x3x");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void OMAP2430$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP2430", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2430.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP2430", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP2430.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP2430.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP2430.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP2430$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP2430.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP2430", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP2430");
    }

    void OMAP2431$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP2431", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2431.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP2431", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP2431.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP2431.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP2431.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP2431$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP2431.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP2431", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP2431");
    }

    void OMAP2530$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP2530", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2530.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP2530", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP2530.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP2530.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP2530.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP2530$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP2530.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP2530", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP2530");
    }

    void OMAP2531$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP2531", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP2531.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP2531", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP2531.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP2531.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP2531.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP2531$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP2531.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP2531", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP2531");
    }

    void IOMAP3xxx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.IOMAP3xxx", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.IOMAP3xxx.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.IOMAP3xxx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.IOMAP3xxx$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.IOMAP3xxx.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.IOMAP3xxx.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.IOMAP3xxx.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IOMAP3xxx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IOMAP3xxx");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void OMAP3403$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP3403", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3403.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP3403", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP3403.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP3403.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP3403.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP3403$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP3403.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP3403", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP3403");
    }

    void OMAP3405$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP3405", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3405.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP3405", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP3405.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP3405.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP3405.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP3405$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP3405.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP3405", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP3405");
    }

    void OMAP3503$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP3503", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3503.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP3503", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP3503.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP3503.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP3503.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP3503$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP3503.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP3503", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP3503");
    }

    void OMAP3505$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP3505", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3505.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP3505", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP3505.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP3505.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP3505.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP3505$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP3505.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP3505", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP3505");
    }

    void OMAP3515$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP3515", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3515.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP3515", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP3515.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP3515.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP3515.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP3515$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP3515.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP3515", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP3515");
    }

    void OMAP3517$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP3517", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3517.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP3517", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP3517.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP3517.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP3517.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP3517$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP3517.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP3517", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP3517");
    }

    void OMAP3425$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP3425", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3425.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP3425", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP3425.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP3425.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP3425.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP3425$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP3425.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP3425", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP3425");
    }

    void OMAP3525$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP3525", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3525.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP3525", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP3525.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP3525.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP3525.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP3525$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP3525.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP3525", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP3525");
    }

    void TMS320C3430$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320C3430", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320C3430.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320C3430", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320C3430.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320C3430.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320C3430.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320C3430$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320C3430.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320C3430", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320C3430");
    }

    void OMAP3530$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAP3530", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAP3530.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAP3530", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAP3530.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAP3530.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAP3530.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAP3530$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAP3530.Object", "ti.catalog.arm"));
        pkgV.bind("OMAP3530", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAP3530");
    }

    void ITMS320DA8xx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.ITMS320DA8xx", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.ITMS320DA8xx.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.ITMS320DA8xx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.ITMS320DA8xx$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.ITMS320DA8xx.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.ITMS320DA8xx.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITMS320DA8xx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITMS320DA8xx");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void TMS320DA828$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320DA828", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA828.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320DA828", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320DA828.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320DA828.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320DA828.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320DA828$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320DA828.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320DA828", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320DA828");
    }

    void TMS320DA830$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320DA830", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA830.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320DA830", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320DA830.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320DA830.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320DA830.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320DA830$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320DA830.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320DA830", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320DA830");
    }

    void OMAPL137$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAPL137", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL137.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAPL137", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAPL137.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAPL137.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAPL137.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAPL137$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAPL137.Object", "ti.catalog.arm"));
        pkgV.bind("OMAPL137", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAPL137");
    }

    void CortexM3$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.CortexM3", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.CortexM3.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.CortexM3", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.CortexM3$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.CortexM3.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.CortexM3.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.CortexM3.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.CortexM3$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.CortexM3.Object", "ti.catalog.arm"));
        pkgV.bind("CortexM3", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CortexM3");
    }

    void TMS570PSF762$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS570PSF762", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS570PSF762.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS570PSF762", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS570PSF762$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS570PSF762.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS570PSF762.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS570PSF762.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS570PSF762$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS570PSF762.Object", "ti.catalog.arm"));
        pkgV.bind("TMS570PSF762", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS570PSF762");
    }

    void TMS570LS20216$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS570LS20216", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS570LS20216.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS570LS20216", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS570LS20216$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS570LS20216.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS570LS20216.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS570LS20216.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS570LS20216$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS570LS20216.Object", "ti.catalog.arm"));
        pkgV.bind("TMS570LS20216", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS570LS20216");
    }

    void TMS320DA840$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320DA840", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA840.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320DA840", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320DA840.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320DA840.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320DA840.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320DA840$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320DA840.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320DA840", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320DA840");
    }

    void TMS320DA850$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320DA850", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320DA850.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320DA850", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320DA850.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320DA850.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320DA850.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320DA850$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320DA850.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320DA850", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320DA850");
    }

    void AM1705$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.AM1705", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1705.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.AM1705", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.AM1705.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.AM1705.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.AM1705.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.AM1705$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.AM1705.Object", "ti.catalog.arm"));
        pkgV.bind("AM1705", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("AM1705");
    }

    void AM1707$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.AM1707", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1707.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.AM1707", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.AM1707.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.AM1707.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.AM1707.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.AM1707$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.AM1707.Object", "ti.catalog.arm"));
        pkgV.bind("AM1707", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("AM1707");
    }

    void AM1806$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.AM1806", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1806.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.AM1806", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.AM1806.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.AM1806.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.AM1806.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.AM1806$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.AM1806.Object", "ti.catalog.arm"));
        pkgV.bind("AM1806", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("AM1806");
    }

    void AM1808$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.AM1808", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.AM1808.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.AM1808", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.AM1808.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.AM1808.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.AM1808.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.AM1808$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.AM1808.Object", "ti.catalog.arm"));
        pkgV.bind("AM1808", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("AM1808");
    }

    void OMAPL108$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAPL108", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL108.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAPL108", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAPL108.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAPL108.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAPL108.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAPL108$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAPL108.Object", "ti.catalog.arm"));
        pkgV.bind("OMAPL108", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAPL108");
    }

    void OMAPL118$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAPL118", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL118.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAPL118", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAPL118.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAPL118.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAPL118.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAPL118$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAPL118.Object", "ti.catalog.arm"));
        pkgV.bind("OMAPL118", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAPL118");
    }

    void OMAPL138$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.OMAPL138", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.OMAPL138.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.OMAPL138", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.OMAPL138.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.OMAPL138.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.OMAPL138.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.catalog.arm");
        inherits.add("ti.catalog");
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.OMAPL138$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.OMAPL138.Object", "ti.catalog.arm"));
        pkgV.bind("OMAPL138", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OMAPL138");
    }

    void TMS320CDM730$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320CDM730", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM730.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320CDM730", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS320CDM730$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320CDM730.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320CDM730.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320CDM730.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320CDM730$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320CDM730.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320CDM730", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320CDM730");
    }

    void TMS320CDM740$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TMS320CDM740", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TMS320CDM740.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TMS320CDM740", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TMS320CDM740$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TMS320CDM740.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TMS320CDM740.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TMS320CDM740.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TMS320CDM740$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TMS320CDM740.Object", "ti.catalog.arm"));
        pkgV.bind("TMS320CDM740", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TMS320CDM740");
    }

    void TNETV107X$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.TNETV107X", "ti.catalog.arm");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.TNETV107X.Module", "ti.catalog.arm");
        vo.init2(po, "ti.catalog.arm.TNETV107X", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.catalog.arm.TNETV107X$$capsule", "ti.catalog.arm"));
        vo.bind("Instance", om.findStrict("ti.catalog.arm.TNETV107X.Instance", "ti.catalog.arm"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.TNETV107X.Params", "ti.catalog.arm"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.TNETV107X.Params", "ti.catalog.arm")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm", "ti.catalog.arm"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.TNETV107X$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.TNETV107X.Object", "ti.catalog.arm"));
        pkgV.bind("TNETV107X", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TNETV107X");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS470R10", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320C5912", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320C1030", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320CDM6446", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320CDM6467", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320DM355", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320DM357", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320DM365", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320CDM510", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP2430", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP2431", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP2530", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP2531", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP3403", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP3405", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP3503", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP3505", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP3515", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP3517", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP3425", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP3525", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320C3430", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAP3530", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320DA828", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320DA830", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAPL137", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.CortexM3", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS570PSF762", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS570LS20216", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320DA840", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320DA850", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.AM1705", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.AM1707", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.AM1806", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.AM1808", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAPL108", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAPL118", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.OMAPL138", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320CDM730", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TMS320CDM740", "ti.catalog.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.TNETV107X", "ti.catalog.arm"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.catalog.arm.TMS470R10")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320C5912")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320C1030")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320CDM6446")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320CDM6467")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320DM355")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320DM357")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320DM365")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320CDM510")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.IOMAP2x3x")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP2430")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP2431")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP2530")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP2531")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.IOMAP3xxx")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP3403")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP3405")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP3503")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP3505")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP3515")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP3517")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP3425")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP3525")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320C3430")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAP3530")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.ITMS320DA8xx")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320DA828")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320DA830")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAPL137")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.CortexM3")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS570PSF762")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS570LS20216")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320DA840")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320DA850")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.AM1705")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.AM1707")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.AM1806")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.AM1808")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAPL108")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAPL118")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.OMAPL138")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320CDM730")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TMS320CDM740")).bless();
        ((Value.Obj)om.getv("ti.catalog.arm.TNETV107X")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.catalog.arm")).add(pkgV);
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
        TMS470R10$$OBJECTS();
        TMS320C5912$$OBJECTS();
        TMS320C1030$$OBJECTS();
        TMS320CDM6446$$OBJECTS();
        TMS320CDM6467$$OBJECTS();
        TMS320DM355$$OBJECTS();
        TMS320DM357$$OBJECTS();
        TMS320DM365$$OBJECTS();
        TMS320CDM510$$OBJECTS();
        IOMAP2x3x$$OBJECTS();
        OMAP2430$$OBJECTS();
        OMAP2431$$OBJECTS();
        OMAP2530$$OBJECTS();
        OMAP2531$$OBJECTS();
        IOMAP3xxx$$OBJECTS();
        OMAP3403$$OBJECTS();
        OMAP3405$$OBJECTS();
        OMAP3503$$OBJECTS();
        OMAP3505$$OBJECTS();
        OMAP3515$$OBJECTS();
        OMAP3517$$OBJECTS();
        OMAP3425$$OBJECTS();
        OMAP3525$$OBJECTS();
        TMS320C3430$$OBJECTS();
        OMAP3530$$OBJECTS();
        ITMS320DA8xx$$OBJECTS();
        TMS320DA828$$OBJECTS();
        TMS320DA830$$OBJECTS();
        OMAPL137$$OBJECTS();
        CortexM3$$OBJECTS();
        TMS570PSF762$$OBJECTS();
        TMS570LS20216$$OBJECTS();
        TMS320DA840$$OBJECTS();
        TMS320DA850$$OBJECTS();
        AM1705$$OBJECTS();
        AM1707$$OBJECTS();
        AM1806$$OBJECTS();
        AM1808$$OBJECTS();
        OMAPL108$$OBJECTS();
        OMAPL118$$OBJECTS();
        OMAPL138$$OBJECTS();
        TMS320CDM730$$OBJECTS();
        TMS320CDM740$$OBJECTS();
        TNETV107X$$OBJECTS();
        TMS470R10$$CONSTS();
        TMS320C5912$$CONSTS();
        TMS320C1030$$CONSTS();
        TMS320CDM6446$$CONSTS();
        TMS320CDM6467$$CONSTS();
        TMS320DM355$$CONSTS();
        TMS320DM357$$CONSTS();
        TMS320DM365$$CONSTS();
        TMS320CDM510$$CONSTS();
        IOMAP2x3x$$CONSTS();
        OMAP2430$$CONSTS();
        OMAP2431$$CONSTS();
        OMAP2530$$CONSTS();
        OMAP2531$$CONSTS();
        IOMAP3xxx$$CONSTS();
        OMAP3403$$CONSTS();
        OMAP3405$$CONSTS();
        OMAP3503$$CONSTS();
        OMAP3505$$CONSTS();
        OMAP3515$$CONSTS();
        OMAP3517$$CONSTS();
        OMAP3425$$CONSTS();
        OMAP3525$$CONSTS();
        TMS320C3430$$CONSTS();
        OMAP3530$$CONSTS();
        ITMS320DA8xx$$CONSTS();
        TMS320DA828$$CONSTS();
        TMS320DA830$$CONSTS();
        OMAPL137$$CONSTS();
        CortexM3$$CONSTS();
        TMS570PSF762$$CONSTS();
        TMS570LS20216$$CONSTS();
        TMS320DA840$$CONSTS();
        TMS320DA850$$CONSTS();
        AM1705$$CONSTS();
        AM1707$$CONSTS();
        AM1806$$CONSTS();
        AM1808$$CONSTS();
        OMAPL108$$CONSTS();
        OMAPL118$$CONSTS();
        OMAPL138$$CONSTS();
        TMS320CDM730$$CONSTS();
        TMS320CDM740$$CONSTS();
        TNETV107X$$CONSTS();
        TMS470R10$$CREATES();
        TMS320C5912$$CREATES();
        TMS320C1030$$CREATES();
        TMS320CDM6446$$CREATES();
        TMS320CDM6467$$CREATES();
        TMS320DM355$$CREATES();
        TMS320DM357$$CREATES();
        TMS320DM365$$CREATES();
        TMS320CDM510$$CREATES();
        IOMAP2x3x$$CREATES();
        OMAP2430$$CREATES();
        OMAP2431$$CREATES();
        OMAP2530$$CREATES();
        OMAP2531$$CREATES();
        IOMAP3xxx$$CREATES();
        OMAP3403$$CREATES();
        OMAP3405$$CREATES();
        OMAP3503$$CREATES();
        OMAP3505$$CREATES();
        OMAP3515$$CREATES();
        OMAP3517$$CREATES();
        OMAP3425$$CREATES();
        OMAP3525$$CREATES();
        TMS320C3430$$CREATES();
        OMAP3530$$CREATES();
        ITMS320DA8xx$$CREATES();
        TMS320DA828$$CREATES();
        TMS320DA830$$CREATES();
        OMAPL137$$CREATES();
        CortexM3$$CREATES();
        TMS570PSF762$$CREATES();
        TMS570LS20216$$CREATES();
        TMS320DA840$$CREATES();
        TMS320DA850$$CREATES();
        AM1705$$CREATES();
        AM1707$$CREATES();
        AM1806$$CREATES();
        AM1808$$CREATES();
        OMAPL108$$CREATES();
        OMAPL118$$CREATES();
        OMAPL138$$CREATES();
        TMS320CDM730$$CREATES();
        TMS320CDM740$$CREATES();
        TNETV107X$$CREATES();
        TMS470R10$$FUNCTIONS();
        TMS320C5912$$FUNCTIONS();
        TMS320C1030$$FUNCTIONS();
        TMS320CDM6446$$FUNCTIONS();
        TMS320CDM6467$$FUNCTIONS();
        TMS320DM355$$FUNCTIONS();
        TMS320DM357$$FUNCTIONS();
        TMS320DM365$$FUNCTIONS();
        TMS320CDM510$$FUNCTIONS();
        IOMAP2x3x$$FUNCTIONS();
        OMAP2430$$FUNCTIONS();
        OMAP2431$$FUNCTIONS();
        OMAP2530$$FUNCTIONS();
        OMAP2531$$FUNCTIONS();
        IOMAP3xxx$$FUNCTIONS();
        OMAP3403$$FUNCTIONS();
        OMAP3405$$FUNCTIONS();
        OMAP3503$$FUNCTIONS();
        OMAP3505$$FUNCTIONS();
        OMAP3515$$FUNCTIONS();
        OMAP3517$$FUNCTIONS();
        OMAP3425$$FUNCTIONS();
        OMAP3525$$FUNCTIONS();
        TMS320C3430$$FUNCTIONS();
        OMAP3530$$FUNCTIONS();
        ITMS320DA8xx$$FUNCTIONS();
        TMS320DA828$$FUNCTIONS();
        TMS320DA830$$FUNCTIONS();
        OMAPL137$$FUNCTIONS();
        CortexM3$$FUNCTIONS();
        TMS570PSF762$$FUNCTIONS();
        TMS570LS20216$$FUNCTIONS();
        TMS320DA840$$FUNCTIONS();
        TMS320DA850$$FUNCTIONS();
        AM1705$$FUNCTIONS();
        AM1707$$FUNCTIONS();
        AM1806$$FUNCTIONS();
        AM1808$$FUNCTIONS();
        OMAPL108$$FUNCTIONS();
        OMAPL118$$FUNCTIONS();
        OMAPL138$$FUNCTIONS();
        TMS320CDM730$$FUNCTIONS();
        TMS320CDM740$$FUNCTIONS();
        TNETV107X$$FUNCTIONS();
        TMS470R10$$SIZES();
        TMS320C5912$$SIZES();
        TMS320C1030$$SIZES();
        TMS320CDM6446$$SIZES();
        TMS320CDM6467$$SIZES();
        TMS320DM355$$SIZES();
        TMS320DM357$$SIZES();
        TMS320DM365$$SIZES();
        TMS320CDM510$$SIZES();
        IOMAP2x3x$$SIZES();
        OMAP2430$$SIZES();
        OMAP2431$$SIZES();
        OMAP2530$$SIZES();
        OMAP2531$$SIZES();
        IOMAP3xxx$$SIZES();
        OMAP3403$$SIZES();
        OMAP3405$$SIZES();
        OMAP3503$$SIZES();
        OMAP3505$$SIZES();
        OMAP3515$$SIZES();
        OMAP3517$$SIZES();
        OMAP3425$$SIZES();
        OMAP3525$$SIZES();
        TMS320C3430$$SIZES();
        OMAP3530$$SIZES();
        ITMS320DA8xx$$SIZES();
        TMS320DA828$$SIZES();
        TMS320DA830$$SIZES();
        OMAPL137$$SIZES();
        CortexM3$$SIZES();
        TMS570PSF762$$SIZES();
        TMS570LS20216$$SIZES();
        TMS320DA840$$SIZES();
        TMS320DA850$$SIZES();
        AM1705$$SIZES();
        AM1707$$SIZES();
        AM1806$$SIZES();
        AM1808$$SIZES();
        OMAPL108$$SIZES();
        OMAPL118$$SIZES();
        OMAPL138$$SIZES();
        TMS320CDM730$$SIZES();
        TMS320CDM740$$SIZES();
        TNETV107X$$SIZES();
        TMS470R10$$TYPES();
        TMS320C5912$$TYPES();
        TMS320C1030$$TYPES();
        TMS320CDM6446$$TYPES();
        TMS320CDM6467$$TYPES();
        TMS320DM355$$TYPES();
        TMS320DM357$$TYPES();
        TMS320DM365$$TYPES();
        TMS320CDM510$$TYPES();
        IOMAP2x3x$$TYPES();
        OMAP2430$$TYPES();
        OMAP2431$$TYPES();
        OMAP2530$$TYPES();
        OMAP2531$$TYPES();
        IOMAP3xxx$$TYPES();
        OMAP3403$$TYPES();
        OMAP3405$$TYPES();
        OMAP3503$$TYPES();
        OMAP3505$$TYPES();
        OMAP3515$$TYPES();
        OMAP3517$$TYPES();
        OMAP3425$$TYPES();
        OMAP3525$$TYPES();
        TMS320C3430$$TYPES();
        OMAP3530$$TYPES();
        ITMS320DA8xx$$TYPES();
        TMS320DA828$$TYPES();
        TMS320DA830$$TYPES();
        OMAPL137$$TYPES();
        CortexM3$$TYPES();
        TMS570PSF762$$TYPES();
        TMS570LS20216$$TYPES();
        TMS320DA840$$TYPES();
        TMS320DA850$$TYPES();
        AM1705$$TYPES();
        AM1707$$TYPES();
        AM1806$$TYPES();
        AM1808$$TYPES();
        OMAPL108$$TYPES();
        OMAPL118$$TYPES();
        OMAPL138$$TYPES();
        TMS320CDM730$$TYPES();
        TMS320CDM740$$TYPES();
        TNETV107X$$TYPES();
        if (isROV) {
            TMS470R10$$ROV();
            TMS320C5912$$ROV();
            TMS320C1030$$ROV();
            TMS320CDM6446$$ROV();
            TMS320CDM6467$$ROV();
            TMS320DM355$$ROV();
            TMS320DM357$$ROV();
            TMS320DM365$$ROV();
            TMS320CDM510$$ROV();
            IOMAP2x3x$$ROV();
            OMAP2430$$ROV();
            OMAP2431$$ROV();
            OMAP2530$$ROV();
            OMAP2531$$ROV();
            IOMAP3xxx$$ROV();
            OMAP3403$$ROV();
            OMAP3405$$ROV();
            OMAP3503$$ROV();
            OMAP3505$$ROV();
            OMAP3515$$ROV();
            OMAP3517$$ROV();
            OMAP3425$$ROV();
            OMAP3525$$ROV();
            TMS320C3430$$ROV();
            OMAP3530$$ROV();
            ITMS320DA8xx$$ROV();
            TMS320DA828$$ROV();
            TMS320DA830$$ROV();
            OMAPL137$$ROV();
            CortexM3$$ROV();
            TMS570PSF762$$ROV();
            TMS570LS20216$$ROV();
            TMS320DA840$$ROV();
            TMS320DA850$$ROV();
            AM1705$$ROV();
            AM1707$$ROV();
            AM1806$$ROV();
            AM1808$$ROV();
            OMAPL108$$ROV();
            OMAPL118$$ROV();
            OMAPL138$$ROV();
            TMS320CDM730$$ROV();
            TMS320CDM740$$ROV();
            TNETV107X$$ROV();
        }//isROV
        $$SINGLETONS();
        TMS470R10$$SINGLETONS();
        TMS320C5912$$SINGLETONS();
        TMS320C1030$$SINGLETONS();
        TMS320CDM6446$$SINGLETONS();
        TMS320CDM6467$$SINGLETONS();
        TMS320DM355$$SINGLETONS();
        TMS320DM357$$SINGLETONS();
        TMS320DM365$$SINGLETONS();
        TMS320CDM510$$SINGLETONS();
        IOMAP2x3x$$SINGLETONS();
        OMAP2430$$SINGLETONS();
        OMAP2431$$SINGLETONS();
        OMAP2530$$SINGLETONS();
        OMAP2531$$SINGLETONS();
        IOMAP3xxx$$SINGLETONS();
        OMAP3403$$SINGLETONS();
        OMAP3405$$SINGLETONS();
        OMAP3503$$SINGLETONS();
        OMAP3505$$SINGLETONS();
        OMAP3515$$SINGLETONS();
        OMAP3517$$SINGLETONS();
        OMAP3425$$SINGLETONS();
        OMAP3525$$SINGLETONS();
        TMS320C3430$$SINGLETONS();
        OMAP3530$$SINGLETONS();
        ITMS320DA8xx$$SINGLETONS();
        TMS320DA828$$SINGLETONS();
        TMS320DA830$$SINGLETONS();
        OMAPL137$$SINGLETONS();
        CortexM3$$SINGLETONS();
        TMS570PSF762$$SINGLETONS();
        TMS570LS20216$$SINGLETONS();
        TMS320DA840$$SINGLETONS();
        TMS320DA850$$SINGLETONS();
        AM1705$$SINGLETONS();
        AM1707$$SINGLETONS();
        AM1806$$SINGLETONS();
        AM1808$$SINGLETONS();
        OMAPL108$$SINGLETONS();
        OMAPL118$$SINGLETONS();
        OMAPL138$$SINGLETONS();
        TMS320CDM730$$SINGLETONS();
        TMS320CDM740$$SINGLETONS();
        TNETV107X$$SINGLETONS();
        $$INITIALIZATION();
    }
}
