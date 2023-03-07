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

public class ti_sysbios_rts_gnu
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
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.rts.gnu.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.rts.gnu", new Value.Obj("ti.sysbios.rts.gnu", pkgP));
    }

    void ReentSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.rts.gnu.ReentSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.rts.gnu.ReentSupport", new Value.Obj("ti.sysbios.rts.gnu.ReentSupport", po));
        pkgV.bind("ReentSupport", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.rts.gnu.ReentSupport$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.rts.gnu.ReentSupport.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.rts.gnu.ReentSupport$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.rts.gnu.ReentSupport.Module_State", new Proto.Str(spo, false));
    }

    void SemiHostSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.rts.gnu.SemiHostSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.rts.gnu.SemiHostSupport", new Value.Obj("ti.sysbios.rts.gnu.SemiHostSupport", po));
        pkgV.bind("SemiHostSupport", vo);
        // decls 
    }

    void ReentSupport$$CONSTS()
    {
        // module ReentSupport
        om.bind("ti.sysbios.rts.gnu.ReentSupport.getReent", new Extern("ti_sysbios_rts_gnu_ReentSupport_getReent__I", "xdc_Ptr(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.rts.gnu.ReentSupport.initGlobalReent", new Extern("ti_sysbios_rts_gnu_ReentSupport_initGlobalReent__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.rts.gnu.ReentSupport.taskCreateHook", new Extern("ti_sysbios_rts_gnu_ReentSupport_taskCreateHook__I", "xdc_Void(*)(ti_sysbios_knl_Task_Handle,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.rts.gnu.ReentSupport.taskDeleteHook", new Extern("ti_sysbios_rts_gnu_ReentSupport_taskDeleteHook__I", "xdc_Void(*)(ti_sysbios_knl_Task_Handle)", true, false));
        om.bind("ti.sysbios.rts.gnu.ReentSupport.taskRegHook", new Extern("ti_sysbios_rts_gnu_ReentSupport_taskRegHook__I", "xdc_Void(*)(xdc_Int)", true, false));
    }

    void SemiHostSupport$$CONSTS()
    {
        // module SemiHostSupport
        om.bind("ti.sysbios.rts.gnu.SemiHostSupport.startup", new Extern("ti_sysbios_rts_gnu_SemiHostSupport_startup__I", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void ReentSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void SemiHostSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ReentSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SemiHostSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ReentSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.rts.gnu.ReentSupport.Module_State", "ti.sysbios.rts.gnu");
        sizes.clear();
        sizes.add(Global.newArray("taskHId", "TInt"));
        sizes.add(Global.newArray("lock", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.rts.gnu.ReentSupport.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.rts.gnu.ReentSupport.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.rts.gnu.ReentSupport.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void SemiHostSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void ReentSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rts/gnu/ReentSupport.xs");
        om.bind("ti.sysbios.rts.gnu.ReentSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.gnu.ReentSupport.Module", "ti.sysbios.rts.gnu");
        po.init("ti.sysbios.rts.gnu.ReentSupport.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.rts.gnu"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.rts.gnu"), $$UNDEF, "wh");
            po.addFld("enableReentSupport", $$T_Bool, true, "w");
            po.addFld("A_badThreadType", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.rts.gnu"), Global.newObject("msg", "A_badThreadType: Cannot call a C runtime library API from a Hwi or Swi thread."), "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.rts.gnu.ReentSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.rts.gnu.ReentSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.rts.gnu.ReentSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.rts.gnu.ReentSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct ReentSupport.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.gnu.ReentSupport$$ModuleView", "ti.sysbios.rts.gnu");
        po.init("ti.sysbios.rts.gnu.ReentSupport.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("enableReentSupport", $$T_Bool, $$UNDEF, "w");
        // struct ReentSupport.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.gnu.ReentSupport$$Module_State", "ti.sysbios.rts.gnu");
        po.init("ti.sysbios.rts.gnu.ReentSupport.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("taskHId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("lock", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Handle", "ti.sysbios.rts.gnu"), $$UNDEF, "w");
    }

    void SemiHostSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rts/gnu/SemiHostSupport.xs");
        om.bind("ti.sysbios.rts.gnu.SemiHostSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.gnu.SemiHostSupport.Module", "ti.sysbios.rts.gnu");
        po.init("ti.sysbios.rts.gnu.SemiHostSupport.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.rts.gnu"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.rts.gnu.SemiHostSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.rts.gnu.SemiHostSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.rts.gnu.SemiHostSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.rts.gnu.SemiHostSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void ReentSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rts.gnu.ReentSupport", "ti.sysbios.rts.gnu");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.rts.gnu.ReentSupport.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.gnu.ReentSupport$$Module_State", "ti.sysbios.rts.gnu");
    }

    void SemiHostSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rts.gnu.SemiHostSupport", "ti.sysbios.rts.gnu");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.rts.gnu.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.rts.gnu"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rts/gnu/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.rts.gnu"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.rts.gnu"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.rts.gnu"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.rts.gnu"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.rts.gnu"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.rts.gnu"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.rts.gnu", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.rts.gnu");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.rts.gnu.");
        pkgV.bind("$vers", Global.newArray(2, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.rts.gnu'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.rts.gnu$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.rts.gnu$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.rts.gnu$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.rts.gnu.am4g',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.rts.gnu.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void ReentSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rts.gnu.ReentSupport", "ti.sysbios.rts.gnu");
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.gnu.ReentSupport.Module", "ti.sysbios.rts.gnu");
        vo.init2(po, "ti.sysbios.rts.gnu.ReentSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.rts.gnu.ReentSupport$$capsule", "ti.sysbios.rts.gnu"));
        vo.bind("$package", om.findStrict("ti.sysbios.rts.gnu", "ti.sysbios.rts.gnu"));
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
        vo.bind("ModuleView", om.findStrict("ti.sysbios.rts.gnu.ReentSupport.ModuleView", "ti.sysbios.rts.gnu"));
        tdefs.add(om.findStrict("ti.sysbios.rts.gnu.ReentSupport.ModuleView", "ti.sysbios.rts.gnu"));
        mcfgs.add("enableReentSupport");
        mcfgs.add("A_badThreadType");
        vo.bind("Module_State", om.findStrict("ti.sysbios.rts.gnu.ReentSupport.Module_State", "ti.sysbios.rts.gnu"));
        tdefs.add(om.findStrict("ti.sysbios.rts.gnu.ReentSupport.Module_State", "ti.sysbios.rts.gnu"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.rts.gnu")).add(vo);
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
        vo.bind("getReent", om.findStrict("ti.sysbios.rts.gnu.ReentSupport.getReent", "ti.sysbios.rts.gnu"));
        vo.bind("initGlobalReent", om.findStrict("ti.sysbios.rts.gnu.ReentSupport.initGlobalReent", "ti.sysbios.rts.gnu"));
        vo.bind("taskCreateHook", om.findStrict("ti.sysbios.rts.gnu.ReentSupport.taskCreateHook", "ti.sysbios.rts.gnu"));
        vo.bind("taskDeleteHook", om.findStrict("ti.sysbios.rts.gnu.ReentSupport.taskDeleteHook", "ti.sysbios.rts.gnu"));
        vo.bind("taskRegHook", om.findStrict("ti.sysbios.rts.gnu.ReentSupport.taskRegHook", "ti.sysbios.rts.gnu"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_rts_gnu_ReentSupport_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_badThreadType"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", "./ReentSupport.xdt");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./ReentSupport.xdt");
        pkgV.bind("ReentSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ReentSupport");
    }

    void SemiHostSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rts.gnu.SemiHostSupport", "ti.sysbios.rts.gnu");
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.gnu.SemiHostSupport.Module", "ti.sysbios.rts.gnu");
        vo.init2(po, "ti.sysbios.rts.gnu.SemiHostSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.rts.gnu.SemiHostSupport$$capsule", "ti.sysbios.rts.gnu"));
        vo.bind("$package", om.findStrict("ti.sysbios.rts.gnu", "ti.sysbios.rts.gnu"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.rts.gnu")).add(vo);
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
        vo.bind("startup", om.findStrict("ti.sysbios.rts.gnu.SemiHostSupport.startup", "ti.sysbios.rts.gnu"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_rts_gnu_SemiHostSupport_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./SemiHostSupport.xdt");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./SemiHostSupport.xdt");
        pkgV.bind("SemiHostSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SemiHostSupport");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.rts.gnu.ReentSupport", "ti.sysbios.rts.gnu"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.rts.gnu.SemiHostSupport", "ti.sysbios.rts.gnu"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.rts.gnu.ReentSupport", "ti.sysbios.rts.gnu");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.rts.gnu.ReentSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.rts.gnu.SemiHostSupport")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.rts.gnu")).add(pkgV);
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
        ReentSupport$$OBJECTS();
        SemiHostSupport$$OBJECTS();
        ReentSupport$$CONSTS();
        SemiHostSupport$$CONSTS();
        ReentSupport$$CREATES();
        SemiHostSupport$$CREATES();
        ReentSupport$$FUNCTIONS();
        SemiHostSupport$$FUNCTIONS();
        ReentSupport$$SIZES();
        SemiHostSupport$$SIZES();
        ReentSupport$$TYPES();
        SemiHostSupport$$TYPES();
        if (isROV) {
            ReentSupport$$ROV();
            SemiHostSupport$$ROV();
        }//isROV
        $$SINGLETONS();
        ReentSupport$$SINGLETONS();
        SemiHostSupport$$SINGLETONS();
        $$INITIALIZATION();
    }
}
