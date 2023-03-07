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

public class ti_sysbios_rom
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
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.rom.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.rom", new Value.Obj("ti.sysbios.rom", pkgP));
    }

    void ROM$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.rom.ROM.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.rom.ROM", new Value.Obj("ti.sysbios.rom.ROM", po));
        pkgV.bind("ROM", vo);
        // decls 
        om.bind("ti.sysbios.rom.ROM.RomName", new Proto.Enm("ti.sysbios.rom.ROM.RomName"));
        spo = (Proto.Obj)om.bind("ti.sysbios.rom.ROM$$ExFunc", new Proto.Obj());
        om.bind("ti.sysbios.rom.ROM.ExFunc", new Proto.Str(spo, true));
    }

    void ROM$$CONSTS()
    {
        // module ROM
        om.bind("ti.sysbios.rom.ROM.NO_ROM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.NO_ROM", 0));
        om.bind("ti.sysbios.rom.ROM.CC2650", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.CC2650", 1));
        om.bind("ti.sysbios.rom.ROM.CC2640R2F", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.CC2640R2F", 2));
        om.bind("ti.sysbios.rom.ROM.CC1350", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.CC1350", 3));
        om.bind("ti.sysbios.rom.ROM.F28004x", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.F28004x", 4));
        om.bind("ti.sysbios.rom.ROM.CC26X2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.CC26X2", 5));
        om.bind("ti.sysbios.rom.ROM.CC26X2V2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.CC26X2V2", 6));
        om.bind("ti.sysbios.rom.ROM.CC13X2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.CC13X2", 7));
        om.bind("ti.sysbios.rom.ROM.CC13X2V2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.CC13X2V2", 8));
        om.bind("ti.sysbios.rom.ROM.CC26X2_NO_OAD", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.CC26X2_NO_OAD", 9));
        om.bind("ti.sysbios.rom.ROM.CC1352", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.CC1352", 10));
        om.bind("ti.sysbios.rom.ROM.CC2652", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), "ti.sysbios.rom.ROM.CC2652", 11));
    }

    void ROM$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ROM$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ROM.getOtherLibs
        fxn = (Proto.Fxn)om.bind("ti.sysbios.rom.ROM$$getOtherLibs", new Proto.Fxn(om.findStrict("ti.sysbios.rom.ROM.Module", "ti.sysbios.rom"), $$T_Str, 0, 0, false));
        // fxn ROM.getOtherLibDefs
        fxn = (Proto.Fxn)om.bind("ti.sysbios.rom.ROM$$getOtherLibDefs", new Proto.Fxn(om.findStrict("ti.sysbios.rom.ROM.Module", "ti.sysbios.rom"), $$T_Str, 0, 0, false));
        // fxn ROM.getExternDefs
        fxn = (Proto.Fxn)om.bind("ti.sysbios.rom.ROM$$getExternDefs", new Proto.Fxn(om.findStrict("ti.sysbios.rom.ROM.Module", "ti.sysbios.rom"), $$T_Str, 0, 0, false));
        // fxn ROM.getGeneratedDefs
        fxn = (Proto.Fxn)om.bind("ti.sysbios.rom.ROM$$getGeneratedDefs", new Proto.Fxn(om.findStrict("ti.sysbios.rom.ROM.Module", "ti.sysbios.rom"), $$T_Str, 0, 0, false));
        // fxn ROM.getConfigDefs
        fxn = (Proto.Fxn)om.bind("ti.sysbios.rom.ROM$$getConfigDefs", new Proto.Fxn(om.findStrict("ti.sysbios.rom.ROM.Module", "ti.sysbios.rom"), $$T_Str, 0, 0, false));
        // fxn ROM.getExterns
        fxn = (Proto.Fxn)om.bind("ti.sysbios.rom.ROM$$getExterns", new Proto.Fxn(om.findStrict("ti.sysbios.rom.ROM.Module", "ti.sysbios.rom"), $$T_Str, 0, 0, false));
        // fxn ROM.makeExternsFile
        fxn = (Proto.Fxn)om.bind("ti.sysbios.rom.ROM$$makeExternsFile", new Proto.Fxn(om.findStrict("ti.sysbios.rom.ROM.Module", "ti.sysbios.rom"), null, 0, -1, false));
    }

    void ROM$$SIZES()
    {
    }

    void ROM$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rom/ROM.xs");
        om.bind("ti.sysbios.rom.ROM$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.rom.ROM.Module", "ti.sysbios.rom");
        po.init("ti.sysbios.rom.ROM.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("romName", (Proto)om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"), om.find("ti.sysbios.rom.ROM.NO_ROM"), "wh");
        po.addFld("configDefs", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("excludedFuncs", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("generatedFuncs", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("otherLibs", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("otherLibIncludes", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("otherLibFuncs", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("makePatch", $$T_Bool, false, "wh");
        po.addFld("groupFuncPtrs", $$T_Bool, false, "wh");
        po.addFld("constStructAddr", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("dataStructAddr", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("externFuncStructAddr", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("excludeFuncs", new Proto.Arr((Proto)om.findStrict("ti.sysbios.rom.ROM.ExFunc", "ti.sysbios.rom"), false), $$DEFAULT, "wh");
        po.addFld("excludeIncludes", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.rom.ROM$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.rom.ROM$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.rom.ROM$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getOtherLibs", (Proto.Fxn)om.findStrict("ti.sysbios.rom.ROM$$getOtherLibs", "ti.sysbios.rom"), Global.get(cap, "getOtherLibs"));
                po.addFxn("getOtherLibDefs", (Proto.Fxn)om.findStrict("ti.sysbios.rom.ROM$$getOtherLibDefs", "ti.sysbios.rom"), Global.get(cap, "getOtherLibDefs"));
                po.addFxn("getExternDefs", (Proto.Fxn)om.findStrict("ti.sysbios.rom.ROM$$getExternDefs", "ti.sysbios.rom"), Global.get(cap, "getExternDefs"));
                po.addFxn("getGeneratedDefs", (Proto.Fxn)om.findStrict("ti.sysbios.rom.ROM$$getGeneratedDefs", "ti.sysbios.rom"), Global.get(cap, "getGeneratedDefs"));
                po.addFxn("getConfigDefs", (Proto.Fxn)om.findStrict("ti.sysbios.rom.ROM$$getConfigDefs", "ti.sysbios.rom"), Global.get(cap, "getConfigDefs"));
                po.addFxn("getExterns", (Proto.Fxn)om.findStrict("ti.sysbios.rom.ROM$$getExterns", "ti.sysbios.rom"), Global.get(cap, "getExterns"));
                po.addFxn("makeExternsFile", (Proto.Fxn)om.findStrict("ti.sysbios.rom.ROM$$makeExternsFile", "ti.sysbios.rom"), Global.get(cap, "makeExternsFile"));
        // struct ROM.ExFunc
        po = (Proto.Obj)om.findStrict("ti.sysbios.rom.ROM$$ExFunc", "ti.sysbios.rom");
        po.init("ti.sysbios.rom.ROM.ExFunc", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("generated", $$T_Bool, $$UNDEF, "w");
                po.addFld("type", $$T_Str, $$UNDEF, "w");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("args", $$T_Str, $$UNDEF, "w");
    }

    void ROM$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.rom.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.rom"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.sysbios.rom", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.rom");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.rom.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.rom'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.rom$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.rom$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.rom$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void ROM$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rom.ROM", "ti.sysbios.rom");
        po = (Proto.Obj)om.findStrict("ti.sysbios.rom.ROM.Module", "ti.sysbios.rom");
        vo.init2(po, "ti.sysbios.rom.ROM", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.rom.ROM$$capsule", "ti.sysbios.rom"));
        vo.bind("$package", om.findStrict("ti.sysbios.rom", "ti.sysbios.rom"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("RomName", om.findStrict("ti.sysbios.rom.ROM.RomName", "ti.sysbios.rom"));
        vo.bind("ExFunc", om.findStrict("ti.sysbios.rom.ROM.ExFunc", "ti.sysbios.rom"));
        tdefs.add(om.findStrict("ti.sysbios.rom.ROM.ExFunc", "ti.sysbios.rom"));
        vo.bind("NO_ROM", om.findStrict("ti.sysbios.rom.ROM.NO_ROM", "ti.sysbios.rom"));
        vo.bind("CC2650", om.findStrict("ti.sysbios.rom.ROM.CC2650", "ti.sysbios.rom"));
        vo.bind("CC2640R2F", om.findStrict("ti.sysbios.rom.ROM.CC2640R2F", "ti.sysbios.rom"));
        vo.bind("CC1350", om.findStrict("ti.sysbios.rom.ROM.CC1350", "ti.sysbios.rom"));
        vo.bind("F28004x", om.findStrict("ti.sysbios.rom.ROM.F28004x", "ti.sysbios.rom"));
        vo.bind("CC26X2", om.findStrict("ti.sysbios.rom.ROM.CC26X2", "ti.sysbios.rom"));
        vo.bind("CC26X2V2", om.findStrict("ti.sysbios.rom.ROM.CC26X2V2", "ti.sysbios.rom"));
        vo.bind("CC13X2", om.findStrict("ti.sysbios.rom.ROM.CC13X2", "ti.sysbios.rom"));
        vo.bind("CC13X2V2", om.findStrict("ti.sysbios.rom.ROM.CC13X2V2", "ti.sysbios.rom"));
        vo.bind("CC26X2_NO_OAD", om.findStrict("ti.sysbios.rom.ROM.CC26X2_NO_OAD", "ti.sysbios.rom"));
        vo.bind("CC1352", om.findStrict("ti.sysbios.rom.ROM.CC1352", "ti.sysbios.rom"));
        vo.bind("CC2652", om.findStrict("ti.sysbios.rom.ROM.CC2652", "ti.sysbios.rom"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.rom")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.sysbios.rom.ROM$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./ROM.xdt");
        atmap.seal("length");
        vo.bind("TEMPLATE$", "./ROM.xdt");
        pkgV.bind("ROM", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ROM");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.rom.ROM", "ti.sysbios.rom"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.rom.ROM")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.rom")).add(pkgV);
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
        ROM$$OBJECTS();
        ROM$$CONSTS();
        ROM$$CREATES();
        ROM$$FUNCTIONS();
        ROM$$SIZES();
        ROM$$TYPES();
        if (isROV) {
            ROM$$ROV();
        }//isROV
        $$SINGLETONS();
        ROM$$SINGLETONS();
        $$INITIALIZATION();
    }
}
