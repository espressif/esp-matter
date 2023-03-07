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

public class ti_sysbios_rts_ti
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
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.rts.ti.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.rts.ti", new Value.Obj("ti.sysbios.rts.ti", pkgP));
    }

    void ThreadLocalStorage$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.rts.ti.ThreadLocalStorage.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.rts.ti.ThreadLocalStorage", new Value.Obj("ti.sysbios.rts.ti.ThreadLocalStorage", po));
        pkgV.bind("ThreadLocalStorage", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.rts.ti.ThreadLocalStorage$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.rts.ti.ThreadLocalStorage.Module_State", new Proto.Str(spo, false));
    }

    void ReentSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.rts.ti.ReentSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.rts.ti.ReentSupport", new Value.Obj("ti.sysbios.rts.ti.ReentSupport", po));
        pkgV.bind("ReentSupport", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.rts.ti.ReentSupport$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.rts.ti.ReentSupport.Module_State", new Proto.Str(spo, false));
    }

    void ThreadLocalStorage$$CONSTS()
    {
        // module ThreadLocalStorage
        om.bind("ti.sysbios.rts.ti.ThreadLocalStorage.createFxn", new Extern("ti_sysbios_rts_ti_ThreadLocalStorage_createFxn__I", "xdc_Void(*)(ti_sysbios_knl_Task_Handle,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.rts.ti.ThreadLocalStorage.switchFxn", new Extern("ti_sysbios_rts_ti_ThreadLocalStorage_switchFxn__I", "xdc_Void(*)(ti_sysbios_knl_Task_Handle,ti_sysbios_knl_Task_Handle)", true, false));
        om.bind("ti.sysbios.rts.ti.ThreadLocalStorage.deleteFxn", new Extern("ti_sysbios_rts_ti_ThreadLocalStorage_deleteFxn__I", "xdc_Void(*)(ti_sysbios_knl_Task_Handle)", true, false));
        om.bind("ti.sysbios.rts.ti.ThreadLocalStorage.registerFxn", new Extern("ti_sysbios_rts_ti_ThreadLocalStorage_registerFxn__I", "xdc_Void(*)(xdc_Int)", true, false));
    }

    void ReentSupport$$CONSTS()
    {
        // module ReentSupport
        om.bind("ti.sysbios.rts.ti.ReentSupport.getReent", new Extern("ti_sysbios_rts_ti_ReentSupport_getReent__E", "xdc_Int*(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.rts.ti.ReentSupport.taskRegHook", new Extern("ti_sysbios_rts_ti_ReentSupport_taskRegHook__I", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.rts.ti.ReentSupport.getTlsAddr", new Extern("ti_sysbios_rts_ti_ReentSupport_getTlsAddr__I", "xdc_Int*(*)(xdc_Void)", true, false));
    }

    void ThreadLocalStorage$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ReentSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ThreadLocalStorage$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ReentSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ThreadLocalStorage$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage.Module_State", "ti.sysbios.rts.ti");
        sizes.clear();
        sizes.add(Global.newArray("currentTP", "UPtr"));
        sizes.add(Global.newArray("contextId", "UInt"));
        sizes.add(Global.newArray("heapHandle", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.rts.ti.ThreadLocalStorage.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.rts.ti.ThreadLocalStorage.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.rts.ti.ThreadLocalStorage.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void ReentSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.rts.ti.ReentSupport.Module_State", "ti.sysbios.rts.ti");
        sizes.clear();
        sizes.add(Global.newArray("taskHId", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.rts.ti.ReentSupport.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.rts.ti.ReentSupport.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.rts.ti.ReentSupport.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void ThreadLocalStorage$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rts/ti/ThreadLocalStorage.xs");
        om.bind("ti.sysbios.rts.ti.ThreadLocalStorage$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage.Module", "ti.sysbios.rts.ti");
        po.init("ti.sysbios.rts.ti.ThreadLocalStorage.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.rts.ti"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("heapHandle", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.rts.ti"), null, "w");
            po.addFld("enableTLSSupport", $$T_Bool, false, "w");
            po.addFld("TItlsSectMemory", $$T_Str, null, "w");
            po.addFld("TItls_initSectMemory", $$T_Str, null, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.rts.ti.ThreadLocalStorage$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.rts.ti.ThreadLocalStorage$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.rts.ti.ThreadLocalStorage$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.rts.ti.ThreadLocalStorage$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct ThreadLocalStorage.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage$$Module_State", "ti.sysbios.rts.ti");
        po.init("ti.sysbios.rts.ti.ThreadLocalStorage.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("currentTP", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("contextId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("heapHandle", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.rts.ti"), $$UNDEF, "w");
    }

    void ReentSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rts/ti/ReentSupport.xs");
        om.bind("ti.sysbios.rts.ti.ReentSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.ti.ReentSupport.Module", "ti.sysbios.rts.ti");
        po.init("ti.sysbios.rts.ti.ReentSupport.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.rts.ti"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("A_badThreadType", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.rts.ti"), Global.newObject("msg", "A_badThreadType: Cannot call a C runtime library API from a Hwi or Swi thread."), "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.rts.ti.ReentSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.rts.ti.ReentSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.rts.ti.ReentSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.rts.ti.ReentSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct ReentSupport.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.ti.ReentSupport$$Module_State", "ti.sysbios.rts.ti");
        po.init("ti.sysbios.rts.ti.ReentSupport.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("taskHId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
    }

    void ThreadLocalStorage$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage", "ti.sysbios.rts.ti");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.rts.ti.ThreadLocalStorage.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage$$Module_State", "ti.sysbios.rts.ti");
    }

    void ReentSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rts.ti.ReentSupport", "ti.sysbios.rts.ti");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.rts.ti.ReentSupport.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.ti.ReentSupport$$Module_State", "ti.sysbios.rts.ti");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.rts.ti.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.rts.ti"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rts/ti/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.rts.ti"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.rts.ti"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.rts.ti"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.rts.ti"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.rts.ti"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.rts.ti"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.rts.ti", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.rts.ti");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.rts.ti.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.rts.ti'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.rts.ti$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.rts.ti$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.rts.ti$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.rts.ti.aem4',\n");
            sb.append("'lib/debug/ti.sysbios.rts.ti.am4',\n");
            sb.append("'lib/debug/ti.sysbios.rts.ti.am4g',\n");
            sb.append("'lib/debug/ti.sysbios.rts.ti.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.rts.ti.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/debug/ti.sysbios.rts.ti.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/debug/ti.sysbios.rts.ti.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/debug/ti.sysbios.rts.ti.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void ThreadLocalStorage$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage", "ti.sysbios.rts.ti");
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage.Module", "ti.sysbios.rts.ti");
        vo.init2(po, "ti.sysbios.rts.ti.ThreadLocalStorage", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage$$capsule", "ti.sysbios.rts.ti"));
        vo.bind("$package", om.findStrict("ti.sysbios.rts.ti", "ti.sysbios.rts.ti"));
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
        mcfgs.add("heapHandle");
        mcfgs.add("enableTLSSupport");
        mcfgs.add("TItlsSectMemory");
        mcfgs.add("TItls_initSectMemory");
        vo.bind("Module_State", om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage.Module_State", "ti.sysbios.rts.ti"));
        tdefs.add(om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage.Module_State", "ti.sysbios.rts.ti"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.rts.ti")).add(vo);
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
        vo.bind("createFxn", om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage.createFxn", "ti.sysbios.rts.ti"));
        vo.bind("switchFxn", om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage.switchFxn", "ti.sysbios.rts.ti"));
        vo.bind("deleteFxn", om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage.deleteFxn", "ti.sysbios.rts.ti"));
        vo.bind("registerFxn", om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage.registerFxn", "ti.sysbios.rts.ti"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_rts_ti_ThreadLocalStorage_Module__startupDone__E"));
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
        pkgV.bind("ThreadLocalStorage", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ThreadLocalStorage");
    }

    void ReentSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rts.ti.ReentSupport", "ti.sysbios.rts.ti");
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.ti.ReentSupport.Module", "ti.sysbios.rts.ti");
        vo.init2(po, "ti.sysbios.rts.ti.ReentSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.rts.ti.ReentSupport$$capsule", "ti.sysbios.rts.ti"));
        vo.bind("$package", om.findStrict("ti.sysbios.rts.ti", "ti.sysbios.rts.ti"));
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
        mcfgs.add("A_badThreadType");
        vo.bind("Module_State", om.findStrict("ti.sysbios.rts.ti.ReentSupport.Module_State", "ti.sysbios.rts.ti"));
        tdefs.add(om.findStrict("ti.sysbios.rts.ti.ReentSupport.Module_State", "ti.sysbios.rts.ti"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.rts.ti")).add(vo);
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
        vo.bind("getReent", om.findStrict("ti.sysbios.rts.ti.ReentSupport.getReent", "ti.sysbios.rts.ti"));
        vo.bind("taskRegHook", om.findStrict("ti.sysbios.rts.ti.ReentSupport.taskRegHook", "ti.sysbios.rts.ti"));
        vo.bind("getTlsAddr", om.findStrict("ti.sysbios.rts.ti.ReentSupport.getTlsAddr", "ti.sysbios.rts.ti"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_rts_ti_ReentSupport_Module__startupDone__E", "ti_sysbios_rts_ti_ReentSupport_getReent__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_badThreadType"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./ReentSupport.xdt");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "getReent", "entry", "", "exit", "%p"));
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./ReentSupport.xdt");
        pkgV.bind("ReentSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ReentSupport");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.rts.ti.ThreadLocalStorage", "ti.sysbios.rts.ti"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.rts.ti.ReentSupport", "ti.sysbios.rts.ti"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.rts.ti.ThreadLocalStorage")).bless();
        ((Value.Obj)om.getv("ti.sysbios.rts.ti.ReentSupport")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.rts.ti")).add(pkgV);
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
        ThreadLocalStorage$$OBJECTS();
        ReentSupport$$OBJECTS();
        ThreadLocalStorage$$CONSTS();
        ReentSupport$$CONSTS();
        ThreadLocalStorage$$CREATES();
        ReentSupport$$CREATES();
        ThreadLocalStorage$$FUNCTIONS();
        ReentSupport$$FUNCTIONS();
        ThreadLocalStorage$$SIZES();
        ReentSupport$$SIZES();
        ThreadLocalStorage$$TYPES();
        ReentSupport$$TYPES();
        if (isROV) {
            ThreadLocalStorage$$ROV();
            ReentSupport$$ROV();
        }//isROV
        $$SINGLETONS();
        ThreadLocalStorage$$SINGLETONS();
        ReentSupport$$SINGLETONS();
        $$INITIALIZATION();
    }
}
