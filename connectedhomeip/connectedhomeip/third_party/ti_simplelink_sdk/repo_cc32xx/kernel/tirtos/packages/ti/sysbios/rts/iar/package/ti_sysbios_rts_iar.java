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

public class ti_sysbios_rts_iar
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
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.rts.iar.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.rts.iar", new Value.Obj("ti.sysbios.rts.iar", pkgP));
    }

    void MultithreadSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.rts.iar.MultithreadSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.rts.iar.MultithreadSupport", new Value.Obj("ti.sysbios.rts.iar.MultithreadSupport", po));
        pkgV.bind("MultithreadSupport", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.rts.iar.MultithreadSupport$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.Module_State", new Proto.Str(spo, false));
    }

    void MultithreadSupport$$CONSTS()
    {
        // module MultithreadSupport
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.perThreadAccess", new Extern("ti_sysbios_rts_iar_MultithreadSupport_perThreadAccess__I", "xdc_Void*(*)(xdc_Void*)", true, false));
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.getTlsPtr", new Extern("ti_sysbios_rts_iar_MultithreadSupport_getTlsPtr__I", "xdc_Void*(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.getTlsAddr", new Extern("ti_sysbios_rts_iar_MultithreadSupport_getTlsAddr__I", "xdc_Void*(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.initLock", new Extern("ti_sysbios_rts_iar_MultithreadSupport_initLock__I", "xdc_Void(*)(xdc_Void**)", true, false));
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.destroyLock", new Extern("ti_sysbios_rts_iar_MultithreadSupport_destroyLock__I", "xdc_Void(*)(xdc_Void**)", true, false));
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.acquireLock", new Extern("ti_sysbios_rts_iar_MultithreadSupport_acquireLock__I", "xdc_Void(*)(xdc_Void**)", true, false));
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.releaseLock", new Extern("ti_sysbios_rts_iar_MultithreadSupport_releaseLock__I", "xdc_Void(*)(xdc_Void**)", true, false));
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.taskCreateHook", new Extern("ti_sysbios_rts_iar_MultithreadSupport_taskCreateHook__I", "xdc_Void(*)(ti_sysbios_knl_Task_Handle,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.taskDeleteHook", new Extern("ti_sysbios_rts_iar_MultithreadSupport_taskDeleteHook__I", "xdc_Void(*)(ti_sysbios_knl_Task_Handle)", true, false));
        om.bind("ti.sysbios.rts.iar.MultithreadSupport.taskRegHook", new Extern("ti_sysbios_rts_iar_MultithreadSupport_taskRegHook__I", "xdc_Void(*)(xdc_Int)", true, false));
    }

    void MultithreadSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void MultithreadSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void MultithreadSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.Module_State", "ti.sysbios.rts.iar");
        sizes.clear();
        sizes.add(Global.newArray("taskHId", "TInt"));
        sizes.add(Global.newArray("deletedTaskTLSPtr", "UPtr"));
        sizes.add(Global.newArray("curTaskHandle", "UPtr"));
        sizes.add(Global.newArray("lock", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.rts.iar.MultithreadSupport.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.rts.iar.MultithreadSupport.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.rts.iar.MultithreadSupport.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void MultithreadSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rts/iar/MultithreadSupport.xs");
        om.bind("ti.sysbios.rts.iar.MultithreadSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.Module", "ti.sysbios.rts.iar");
        po.init("ti.sysbios.rts.iar.MultithreadSupport.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.rts.iar"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("enableMultithreadSupport", $$T_Bool, true, "w");
            po.addFld("A_badThreadType", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.rts.iar"), Global.newObject("msg", "A_badThreadType: Cannot call a C runtime library API from a Hwi or Swi thread."), "w");
            po.addFld("A_badLockRelease", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.rts.iar"), Global.newObject("msg", "A_badLockRelease: Trying to release a lock not owned by this thread."), "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.rts.iar.MultithreadSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.rts.iar.MultithreadSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.rts.iar.MultithreadSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.rts.iar.MultithreadSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct MultithreadSupport.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.iar.MultithreadSupport$$Module_State", "ti.sysbios.rts.iar");
        po.init("ti.sysbios.rts.iar.MultithreadSupport.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("taskHId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("deletedTaskTLSPtr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("curTaskHandle", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.rts.iar"), $$UNDEF, "w");
                po.addFld("lock", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Handle", "ti.sysbios.rts.iar"), $$UNDEF, "w");
    }

    void MultithreadSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rts.iar.MultithreadSupport", "ti.sysbios.rts.iar");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.rts.iar.MultithreadSupport.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.iar.MultithreadSupport$$Module_State", "ti.sysbios.rts.iar");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.rts.iar.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.rts.iar"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rts/iar/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.rts.iar"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.rts.iar"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.rts.iar"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.rts.iar"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.rts.iar"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.rts.iar"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.rts.iar", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.rts.iar");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.rts.iar.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.rts.iar'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.rts.iar$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.rts.iar$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.rts.iar$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.rts.iar.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.rts.iar.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void MultithreadSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rts.iar.MultithreadSupport", "ti.sysbios.rts.iar");
        po = (Proto.Obj)om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.Module", "ti.sysbios.rts.iar");
        vo.init2(po, "ti.sysbios.rts.iar.MultithreadSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport$$capsule", "ti.sysbios.rts.iar"));
        vo.bind("$package", om.findStrict("ti.sysbios.rts.iar", "ti.sysbios.rts.iar"));
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
        mcfgs.add("enableMultithreadSupport");
        mcfgs.add("A_badThreadType");
        mcfgs.add("A_badLockRelease");
        vo.bind("Module_State", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.Module_State", "ti.sysbios.rts.iar"));
        tdefs.add(om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.Module_State", "ti.sysbios.rts.iar"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.rts.iar")).add(vo);
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
        vo.bind("perThreadAccess", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.perThreadAccess", "ti.sysbios.rts.iar"));
        vo.bind("getTlsPtr", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.getTlsPtr", "ti.sysbios.rts.iar"));
        vo.bind("getTlsAddr", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.getTlsAddr", "ti.sysbios.rts.iar"));
        vo.bind("initLock", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.initLock", "ti.sysbios.rts.iar"));
        vo.bind("destroyLock", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.destroyLock", "ti.sysbios.rts.iar"));
        vo.bind("acquireLock", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.acquireLock", "ti.sysbios.rts.iar"));
        vo.bind("releaseLock", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.releaseLock", "ti.sysbios.rts.iar"));
        vo.bind("taskCreateHook", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.taskCreateHook", "ti.sysbios.rts.iar"));
        vo.bind("taskDeleteHook", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.taskDeleteHook", "ti.sysbios.rts.iar"));
        vo.bind("taskRegHook", om.findStrict("ti.sysbios.rts.iar.MultithreadSupport.taskRegHook", "ti.sysbios.rts.iar"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_rts_iar_MultithreadSupport_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_badThreadType", "A_badLockRelease"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./MultithreadSupport.xdt");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./MultithreadSupport.xdt");
        pkgV.bind("MultithreadSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("MultithreadSupport");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.rts.iar.MultithreadSupport", "ti.sysbios.rts.iar"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.rts.iar.MultithreadSupport")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.rts.iar")).add(pkgV);
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
        MultithreadSupport$$OBJECTS();
        MultithreadSupport$$CONSTS();
        MultithreadSupport$$CREATES();
        MultithreadSupport$$FUNCTIONS();
        MultithreadSupport$$SIZES();
        MultithreadSupport$$TYPES();
        if (isROV) {
            MultithreadSupport$$ROV();
        }//isROV
        $$SINGLETONS();
        MultithreadSupport$$SINGLETONS();
        $$INITIALIZATION();
    }
}
