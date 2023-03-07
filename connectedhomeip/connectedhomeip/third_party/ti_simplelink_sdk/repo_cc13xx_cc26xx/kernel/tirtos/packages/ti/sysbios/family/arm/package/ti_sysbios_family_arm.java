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

public class ti_sysbios_family_arm
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
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.family.arm.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.family.arm", new Value.Obj("ti.sysbios.family.arm", pkgP));
    }

    void MPU$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.MPU.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.MPU", new Value.Obj("ti.sysbios.family.arm.MPU", po));
        pkgV.bind("MPU", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.MPU$$RegionAttrsView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.MPU.RegionAttrsView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.MPU$$DeviceRegs", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.MPU.DeviceRegs", new Proto.Str(spo, false));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize", new Proto.Enm("ti.sysbios.family.arm.MPU.RegionSize"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.MPU$$RegionAttrs", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.MPU.RegionAttrs", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.MPU$$RegionEntry", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.MPU.RegionEntry", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.MPU$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.MPU.Module_State", new Proto.Str(spo, false));
    }

    void Settings$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.Settings.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.Settings", new Value.Obj("ti.sysbios.family.arm.Settings", po));
        pkgV.bind("Settings", vo);
        // decls 
    }

    void MPU$$CONSTS()
    {
        // module MPU
        om.bind("ti.sysbios.family.arm.MPU.deviceRegs", new Extern("ti_sysbios_family_arm_MPU_deviceRegs", "ti_sysbios_family_arm_MPU_DeviceRegs*", false, false));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_32", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_32", xdc.services.intern.xsr.Enum.intValue(0x8L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_64", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_64", xdc.services.intern.xsr.Enum.intValue(0xAL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_128", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_128", xdc.services.intern.xsr.Enum.intValue(0xCL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_256", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_256", xdc.services.intern.xsr.Enum.intValue(0xEL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_512", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_512", xdc.services.intern.xsr.Enum.intValue(0x10L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_1K", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_1K", xdc.services.intern.xsr.Enum.intValue(0x12L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_2K", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_2K", xdc.services.intern.xsr.Enum.intValue(0x14L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_4K", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_4K", xdc.services.intern.xsr.Enum.intValue(0x16L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_8K", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_8K", xdc.services.intern.xsr.Enum.intValue(0x18L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_16K", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_16K", xdc.services.intern.xsr.Enum.intValue(0x1AL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_32K", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_32K", xdc.services.intern.xsr.Enum.intValue(0x1CL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_64K", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_64K", xdc.services.intern.xsr.Enum.intValue(0x1EL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_128K", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_128K", xdc.services.intern.xsr.Enum.intValue(0x20L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_256K", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_256K", xdc.services.intern.xsr.Enum.intValue(0x22L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_512K", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_512K", xdc.services.intern.xsr.Enum.intValue(0x24L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_1M", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_1M", xdc.services.intern.xsr.Enum.intValue(0x26L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_2M", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_2M", xdc.services.intern.xsr.Enum.intValue(0x28L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_4M", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_4M", xdc.services.intern.xsr.Enum.intValue(0x2AL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_8M", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_8M", xdc.services.intern.xsr.Enum.intValue(0x2CL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_16M", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_16M", xdc.services.intern.xsr.Enum.intValue(0x2EL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_32M", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_32M", xdc.services.intern.xsr.Enum.intValue(0x30L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_64M", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_64M", xdc.services.intern.xsr.Enum.intValue(0x32L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_128M", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_128M", xdc.services.intern.xsr.Enum.intValue(0x34L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_256M", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_256M", xdc.services.intern.xsr.Enum.intValue(0x36L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_512M", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_512M", xdc.services.intern.xsr.Enum.intValue(0x38L)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_1G", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_1G", xdc.services.intern.xsr.Enum.intValue(0x3AL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_2G", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_2G", xdc.services.intern.xsr.Enum.intValue(0x3CL)+0));
        om.bind("ti.sysbios.family.arm.MPU.RegionSize_4G", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), "ti.sysbios.family.arm.MPU.RegionSize_4G", xdc.services.intern.xsr.Enum.intValue(0x3EL)+0));
        om.bind("ti.sysbios.family.arm.MPU.disable", new Extern("ti_sysbios_family_arm_MPU_disable__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.enable", new Extern("ti_sysbios_family_arm_MPU_enable__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.disableBR", new Extern("ti_sysbios_family_arm_MPU_disableBR__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.enableBR", new Extern("ti_sysbios_family_arm_MPU_enableBR__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.initRegionAttrs", new Extern("ti_sysbios_family_arm_MPU_initRegionAttrs__E", "xdc_Void(*)(ti_sysbios_family_arm_MPU_RegionAttrs*)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.isEnabled", new Extern("ti_sysbios_family_arm_MPU_isEnabled__E", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.setRegion", new Extern("ti_sysbios_family_arm_MPU_setRegion__E", "xdc_Void(*)(xdc_UInt8,xdc_Ptr,ti_sysbios_family_arm_MPU_RegionSize,ti_sysbios_family_arm_MPU_RegionAttrs*)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.setRegionRaw", new Extern("ti_sysbios_family_arm_MPU_setRegionRaw__E", "xdc_Void(*)(xdc_UInt32,xdc_UInt32)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.startup", new Extern("ti_sysbios_family_arm_MPU_startup__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.disableAsm", new Extern("ti_sysbios_family_arm_MPU_disableAsm__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.enableAsm", new Extern("ti_sysbios_family_arm_MPU_enableAsm__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.disableBRAsm", new Extern("ti_sysbios_family_arm_MPU_disableBRAsm__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.enableBRAsm", new Extern("ti_sysbios_family_arm_MPU_enableBRAsm__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.isEnabledAsm", new Extern("ti_sysbios_family_arm_MPU_isEnabledAsm__I", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.MPU.setRegionAsm", new Extern("ti_sysbios_family_arm_MPU_setRegionAsm__I", "xdc_Void(*)(xdc_UInt8,xdc_UInt32,xdc_UInt32,xdc_UInt32)", true, false));
    }

    void Settings$$CONSTS()
    {
        // module Settings
    }

    void MPU$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Settings$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void MPU$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn MPU.initRegionAttrsMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.MPU$$initRegionAttrsMeta", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.MPU.Module", "ti.sysbios.family.arm"), null, 1, 1, false));
                fxn.addArg(0, "regionAttrs", new Proto.Adr("ti_sysbios_family_arm_MPU_RegionAttrs*", "PS"), $$UNDEF);
        // fxn MPU.setRegionMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.MPU$$setRegionMeta", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.MPU.Module", "ti.sysbios.family.arm"), null, 4, 4, false));
                fxn.addArg(0, "regionId", Proto.Elm.newCNum("(xdc_UInt8)"), $$UNDEF);
                fxn.addArg(1, "regionBaseAddr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF);
                fxn.addArg(2, "regionSize", (Proto)om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"), $$UNDEF);
                fxn.addArg(3, "attrs", (Proto)om.findStrict("ti.sysbios.family.arm.MPU.RegionAttrs", "ti.sysbios.family.arm"), $$DEFAULT);
    }

    void Settings$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void MPU$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.MPU.DeviceRegs", "ti.sysbios.family.arm");
        sizes.clear();
        sizes.add(Global.newArray("TYPE", "UInt32"));
        sizes.add(Global.newArray("CTRL", "UInt32"));
        sizes.add(Global.newArray("RNR", "UInt32"));
        sizes.add(Global.newArray("RBAR", "UInt32"));
        sizes.add(Global.newArray("RASR", "UInt32"));
        sizes.add(Global.newArray("RBAR_A1", "UInt32"));
        sizes.add(Global.newArray("RASR_A1", "UInt32"));
        sizes.add(Global.newArray("RBAR_A2", "UInt32"));
        sizes.add(Global.newArray("RASR_A2", "UInt32"));
        sizes.add(Global.newArray("RBAR_A3", "UInt32"));
        sizes.add(Global.newArray("RASR_A3", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.MPU.DeviceRegs']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.MPU.DeviceRegs']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.MPU.DeviceRegs'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.MPU.RegionAttrs", "ti.sysbios.family.arm");
        sizes.clear();
        sizes.add(Global.newArray("enable", "UShort"));
        sizes.add(Global.newArray("bufferable", "UShort"));
        sizes.add(Global.newArray("cacheable", "UShort"));
        sizes.add(Global.newArray("shareable", "UShort"));
        sizes.add(Global.newArray("noExecute", "UShort"));
        sizes.add(Global.newArray("accPerm", "UInt8"));
        sizes.add(Global.newArray("tex", "UInt8"));
        sizes.add(Global.newArray("subregionDisableMask", "UInt8"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.MPU.RegionAttrs']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.MPU.RegionAttrs']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.MPU.RegionAttrs'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.MPU.RegionEntry", "ti.sysbios.family.arm");
        sizes.clear();
        sizes.add(Global.newArray("baseAddress", "UInt32"));
        sizes.add(Global.newArray("sizeAndEnable", "UInt32"));
        sizes.add(Global.newArray("regionAttrs", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.MPU.RegionEntry']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.MPU.RegionEntry']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.MPU.RegionEntry'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.MPU.Module_State", "ti.sysbios.family.arm");
        sizes.clear();
        sizes.add(Global.newArray("regionEntry", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.MPU.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.MPU.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.MPU.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Settings$$SIZES()
    {
    }

    void MPU$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/MPU.xs");
        om.bind("ti.sysbios.family.arm.MPU$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU.Module", "ti.sysbios.family.arm");
        po.init("ti.sysbios.family.arm.MPU.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.family.arm"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.family.arm"), $$UNDEF, "wh");
            po.addFld("defaultAttrs", (Proto)om.findStrict("ti.sysbios.family.arm.MPU.RegionAttrs", "ti.sysbios.family.arm"), Global.newObject("enable", true, "bufferable", false, "cacheable", false, "shareable", false, "noExecute", false, "accPerm", 1L, "tex", 1L, "subregionDisableMask", 0L), "w");
            po.addFld("A_nullPointer", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.family.arm"), Global.newObject("msg", "A_nullPointer: Pointer is null"), "w");
            po.addFld("A_invalidRegionId", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.family.arm"), Global.newObject("msg", "A_invalidRegionId: MPU Region number passed is invalid."), "w");
            po.addFld("A_unalignedBaseAddr", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.family.arm"), Global.newObject("msg", "A_unalignedBaseAddr: MPU region base address not aligned to size."), "w");
            po.addFld("A_reservedAttrs", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.family.arm"), Global.newObject("msg", "A_reservedAttrs: MPU region attributes set to reserved value."), "w");
            po.addFld("enableMPU", $$T_Bool, false, "w");
            po.addFld("enableBackgroundRegion", $$T_Bool, true, "w");
            po.addFld("numRegions", Proto.Elm.newCNum("(xdc_UInt8)"), $$UNDEF, "w");
            po.addFld("regionEntry", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.arm.MPU.RegionEntry", "ti.sysbios.family.arm"), false), $$DEFAULT, "w");
            po.addFld("isMemoryMapped", $$T_Bool, $$UNDEF, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.MPU$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.MPU$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.MPU$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.MPU$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("initRegionAttrsMeta", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.MPU$$initRegionAttrsMeta", "ti.sysbios.family.arm"), Global.get(cap, "initRegionAttrsMeta"));
                po.addFxn("setRegionMeta", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.MPU$$setRegionMeta", "ti.sysbios.family.arm"), Global.get(cap, "setRegionMeta"));
        // struct MPU.RegionAttrsView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU$$RegionAttrsView", "ti.sysbios.family.arm");
        po.init("ti.sysbios.family.arm.MPU.RegionAttrsView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("RegionIdx", Proto.Elm.newCNum("(xdc_UInt8)"), $$UNDEF, "w");
                po.addFld("Enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("BaseAddress", $$T_Str, $$UNDEF, "w");
                po.addFld("Size", $$T_Str, $$UNDEF, "w");
                po.addFld("Bufferable", $$T_Bool, $$UNDEF, "w");
                po.addFld("Cacheable", $$T_Bool, $$UNDEF, "w");
                po.addFld("Shareable", $$T_Bool, $$UNDEF, "w");
                po.addFld("Noexecute", $$T_Bool, $$UNDEF, "w");
                po.addFld("AccessPerm", $$T_Str, $$UNDEF, "w");
                po.addFld("Tex", $$T_Str, $$UNDEF, "w");
                po.addFld("SubregionDisableMask", $$T_Str, $$UNDEF, "w");
        // struct MPU.DeviceRegs
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU$$DeviceRegs", "ti.sysbios.family.arm");
        po.init("ti.sysbios.family.arm.MPU.DeviceRegs", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("TYPE", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("CTRL", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("RNR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("RBAR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("RASR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("RBAR_A1", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("RASR_A1", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("RBAR_A2", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("RASR_A2", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("RBAR_A3", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("RASR_A3", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
        // struct MPU.RegionAttrs
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU$$RegionAttrs", "ti.sysbios.family.arm");
        po.init("ti.sysbios.family.arm.MPU.RegionAttrs", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enable", $$T_Bool, $$UNDEF, "w");
                po.addFld("bufferable", $$T_Bool, $$UNDEF, "w");
                po.addFld("cacheable", $$T_Bool, $$UNDEF, "w");
                po.addFld("shareable", $$T_Bool, $$UNDEF, "w");
                po.addFld("noExecute", $$T_Bool, $$UNDEF, "w");
                po.addFld("accPerm", Proto.Elm.newCNum("(xdc_UInt8)"), $$UNDEF, "w");
                po.addFld("tex", Proto.Elm.newCNum("(xdc_UInt8)"), $$UNDEF, "w");
                po.addFld("subregionDisableMask", Proto.Elm.newCNum("(xdc_UInt8)"), $$UNDEF, "w");
        // struct MPU.RegionEntry
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU$$RegionEntry", "ti.sysbios.family.arm");
        po.init("ti.sysbios.family.arm.MPU.RegionEntry", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("baseAddress", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("sizeAndEnable", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("regionAttrs", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
        // struct MPU.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU$$Module_State", "ti.sysbios.family.arm");
        po.init("ti.sysbios.family.arm.MPU.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("regionEntry", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.arm.MPU.RegionEntry", "ti.sysbios.family.arm"), false), $$DEFAULT, "w");
    }

    void Settings$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/Settings.xs");
        om.bind("ti.sysbios.family.arm.Settings$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.Settings.Module", "ti.sysbios.family.arm");
        po.init("ti.sysbios.family.arm.Settings.Module", om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.family.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.Settings$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.Settings$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.Settings$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "getDefaultCoreDelegate");
                if (fxn != null) po.addFxn("getDefaultCoreDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultCoreDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultHwiDelegate");
                if (fxn != null) po.addFxn("getDefaultHwiDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultHwiDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultTimerDelegate");
                if (fxn != null) po.addFxn("getDefaultTimerDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultTimerDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultClockTimerDelegate");
                if (fxn != null) po.addFxn("getDefaultClockTimerDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultClockTimerDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultTimerSupportDelegate");
                if (fxn != null) po.addFxn("getDefaultTimerSupportDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultTimerSupportDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultTimestampDelegate");
                if (fxn != null) po.addFxn("getDefaultTimestampDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultTimestampDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultTaskSupportDelegate");
                if (fxn != null) po.addFxn("getDefaultTaskSupportDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultTaskSupportDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultIntrinsicsSupportDelegate");
                if (fxn != null) po.addFxn("getDefaultIntrinsicsSupportDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultIntrinsicsSupportDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultCacheDelegate");
                if (fxn != null) po.addFxn("getDefaultCacheDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultCacheDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultPowerDelegate");
                if (fxn != null) po.addFxn("getDefaultPowerDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultPowerDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultSecondsDelegate");
                if (fxn != null) po.addFxn("getDefaultSecondsDelegate", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultSecondsDelegate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultBootModule");
                if (fxn != null) po.addFxn("getDefaultBootModule", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultBootModule", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultMmuModule");
                if (fxn != null) po.addFxn("getDefaultMmuModule", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultMmuModule", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getFamilySettingsXml");
                if (fxn != null) po.addFxn("getFamilySettingsXml", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getFamilySettingsXml", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getDefaultClockTickPeriod");
                if (fxn != null) po.addFxn("getDefaultClockTickPeriod", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ISettings$$getDefaultClockTickPeriod", "ti.sysbios.family.arm"), fxn);
    }

    void MPU$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.MPU", "ti.sysbios.family.arm");
        vo.bind("DeviceRegs$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.MPU.DeviceRegs", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU$$DeviceRegs", "ti.sysbios.family.arm");
        vo.bind("RegionAttrs$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.MPU.RegionAttrs", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU$$RegionAttrs", "ti.sysbios.family.arm");
        vo.bind("RegionEntry$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.MPU.RegionEntry", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU$$RegionEntry", "ti.sysbios.family.arm");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.MPU.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU$$Module_State", "ti.sysbios.family.arm");
        po.bind("regionEntry$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.MPU.RegionEntry", "isScalar", false));
    }

    void Settings$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.family.arm.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.family.arm"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.family.arm"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.family.arm"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.family.arm", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.family.arm");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.family.arm.");
        pkgV.bind("$vers", Global.newArray(2, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.family.arm'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.family.arm$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.family.arm$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.family.arm$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.family.arm.aem4f',\n");
            sb.append("'lib/debug/ti.sysbios.family.arm.am4fg',\n");
            sb.append("'lib/debug/ti.sysbios.family.arm.arm4f',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.family.arm.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/debug/ti.sysbios.family.arm.am4fg', {target: 'gnu.targets.arm.M4F', suffix: 'm4fg'}],\n");
            sb.append("['lib/debug/ti.sysbios.family.arm.arm4f', {target: 'iar.targets.arm.M4F', suffix: 'rm4f'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void MPU$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.MPU", "ti.sysbios.family.arm");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.MPU.Module", "ti.sysbios.family.arm");
        vo.init2(po, "ti.sysbios.family.arm.MPU", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.MPU$$capsule", "ti.sysbios.family.arm"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm", "ti.sysbios.family.arm"));
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
        vo.bind("RegionAttrsView", om.findStrict("ti.sysbios.family.arm.MPU.RegionAttrsView", "ti.sysbios.family.arm"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.MPU.RegionAttrsView", "ti.sysbios.family.arm"));
        vo.bind("DeviceRegs", om.findStrict("ti.sysbios.family.arm.MPU.DeviceRegs", "ti.sysbios.family.arm"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.MPU.DeviceRegs", "ti.sysbios.family.arm"));
        vo.bind("deviceRegs", om.findStrict("ti.sysbios.family.arm.MPU.deviceRegs", "ti.sysbios.family.arm"));
        vo.bind("RegionSize", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize", "ti.sysbios.family.arm"));
        vo.bind("RegionAttrs", om.findStrict("ti.sysbios.family.arm.MPU.RegionAttrs", "ti.sysbios.family.arm"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.MPU.RegionAttrs", "ti.sysbios.family.arm"));
        mcfgs.add("defaultAttrs");
        mcfgs.add("A_nullPointer");
        mcfgs.add("A_invalidRegionId");
        mcfgs.add("A_unalignedBaseAddr");
        mcfgs.add("A_reservedAttrs");
        mcfgs.add("enableMPU");
        mcfgs.add("enableBackgroundRegion");
        mcfgs.add("numRegions");
        vo.bind("RegionEntry", om.findStrict("ti.sysbios.family.arm.MPU.RegionEntry", "ti.sysbios.family.arm"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.MPU.RegionEntry", "ti.sysbios.family.arm"));
        mcfgs.add("regionEntry");
        icfgs.add("regionEntry");
        mcfgs.add("isMemoryMapped");
        icfgs.add("isMemoryMapped");
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.arm.MPU.Module_State", "ti.sysbios.family.arm"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.MPU.Module_State", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_32", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_32", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_64", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_64", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_128", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_128", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_256", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_256", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_512", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_512", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_1K", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_1K", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_2K", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_2K", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_4K", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_4K", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_8K", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_8K", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_16K", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_16K", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_32K", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_32K", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_64K", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_64K", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_128K", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_128K", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_256K", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_256K", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_512K", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_512K", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_1M", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_1M", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_2M", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_2M", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_4M", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_4M", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_8M", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_8M", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_16M", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_16M", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_32M", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_32M", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_64M", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_64M", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_128M", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_128M", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_256M", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_256M", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_512M", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_512M", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_1G", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_1G", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_2G", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_2G", "ti.sysbios.family.arm"));
        vo.bind("RegionSize_4G", om.findStrict("ti.sysbios.family.arm.MPU.RegionSize_4G", "ti.sysbios.family.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm")).add(vo);
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
        vo.bind("disable", om.findStrict("ti.sysbios.family.arm.MPU.disable", "ti.sysbios.family.arm"));
        vo.bind("enable", om.findStrict("ti.sysbios.family.arm.MPU.enable", "ti.sysbios.family.arm"));
        vo.bind("disableBR", om.findStrict("ti.sysbios.family.arm.MPU.disableBR", "ti.sysbios.family.arm"));
        vo.bind("enableBR", om.findStrict("ti.sysbios.family.arm.MPU.enableBR", "ti.sysbios.family.arm"));
        vo.bind("initRegionAttrs", om.findStrict("ti.sysbios.family.arm.MPU.initRegionAttrs", "ti.sysbios.family.arm"));
        vo.bind("isEnabled", om.findStrict("ti.sysbios.family.arm.MPU.isEnabled", "ti.sysbios.family.arm"));
        vo.bind("setRegion", om.findStrict("ti.sysbios.family.arm.MPU.setRegion", "ti.sysbios.family.arm"));
        vo.bind("setRegionRaw", om.findStrict("ti.sysbios.family.arm.MPU.setRegionRaw", "ti.sysbios.family.arm"));
        vo.bind("startup", om.findStrict("ti.sysbios.family.arm.MPU.startup", "ti.sysbios.family.arm"));
        vo.bind("disableAsm", om.findStrict("ti.sysbios.family.arm.MPU.disableAsm", "ti.sysbios.family.arm"));
        vo.bind("enableAsm", om.findStrict("ti.sysbios.family.arm.MPU.enableAsm", "ti.sysbios.family.arm"));
        vo.bind("disableBRAsm", om.findStrict("ti.sysbios.family.arm.MPU.disableBRAsm", "ti.sysbios.family.arm"));
        vo.bind("enableBRAsm", om.findStrict("ti.sysbios.family.arm.MPU.enableBRAsm", "ti.sysbios.family.arm"));
        vo.bind("isEnabledAsm", om.findStrict("ti.sysbios.family.arm.MPU.isEnabledAsm", "ti.sysbios.family.arm"));
        vo.bind("setRegionAsm", om.findStrict("ti.sysbios.family.arm.MPU.setRegionAsm", "ti.sysbios.family.arm"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_MPU_Module__startupDone__E", "ti_sysbios_family_arm_MPU_disable__E", "ti_sysbios_family_arm_MPU_enable__E", "ti_sysbios_family_arm_MPU_disableBR__E", "ti_sysbios_family_arm_MPU_enableBR__E", "ti_sysbios_family_arm_MPU_initRegionAttrs__E", "ti_sysbios_family_arm_MPU_isEnabled__E", "ti_sysbios_family_arm_MPU_setRegion__E", "ti_sysbios_family_arm_MPU_setRegionRaw__E", "ti_sysbios_family_arm_MPU_startup__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_nullPointer", "A_invalidRegionId", "A_unalignedBaseAddr", "A_reservedAttrs"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("MPU", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("MPU");
    }

    void Settings$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.Settings", "ti.sysbios.family.arm");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.Settings.Module", "ti.sysbios.family.arm");
        vo.init2(po, "ti.sysbios.family.arm.Settings", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.Settings$$capsule", "ti.sysbios.family.arm"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm", "ti.sysbios.family.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.sysbios.family.arm.Settings$$instance$static$init", null) ? 1 : 0);
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
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.MPU", "ti.sysbios.family.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.Settings", "ti.sysbios.family.arm"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.MPU", "ti.sysbios.family.arm");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"MpuRegionAttrsView", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewMpuRegionAttrs", "structName", "RegionAttrsView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.family.arm.MPU")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.Settings")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.family.arm")).add(pkgV);
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
        MPU$$OBJECTS();
        Settings$$OBJECTS();
        MPU$$CONSTS();
        Settings$$CONSTS();
        MPU$$CREATES();
        Settings$$CREATES();
        MPU$$FUNCTIONS();
        Settings$$FUNCTIONS();
        MPU$$SIZES();
        Settings$$SIZES();
        MPU$$TYPES();
        Settings$$TYPES();
        if (isROV) {
            MPU$$ROV();
            Settings$$ROV();
        }//isROV
        $$SINGLETONS();
        MPU$$SINGLETONS();
        Settings$$SINGLETONS();
        $$INITIALIZATION();
    }
}
