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

public class ti_uia_family
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
        Global.callFxn("loadPackage", xdcO, "ti.uia.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.uia.family.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.uia.family", new Value.Obj("ti.uia.family", pkgP));
    }

    void Settings$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.family.Settings.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.family.Settings", new Value.Obj("ti.uia.family.Settings", po));
        pkgV.bind("Settings", vo);
        // decls 
    }

    void Settings$$CONSTS()
    {
        // module Settings
    }

    void Settings$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Settings$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Settings.getDefaultTransport
        fxn = (Proto.Fxn)om.bind("ti.uia.family.Settings$$getDefaultTransport", new Proto.Fxn(om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family"), (Proto)om.findStrict("ti.uia.runtime.ServiceMgr.TransportType", "ti.uia.family"), 0, 0, false));
        // fxn Settings.getServiceMgrEventPacketSize
        fxn = (Proto.Fxn)om.bind("ti.uia.family.Settings$$getServiceMgrEventPacketSize", new Proto.Fxn(om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family"), Proto.Elm.newCNum("(xdc_Int)"), 0, 0, false));
        // fxn Settings.getServiceMgrMsgPacketSize
        fxn = (Proto.Fxn)om.bind("ti.uia.family.Settings$$getServiceMgrMsgPacketSize", new Proto.Fxn(om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family"), Proto.Elm.newCNum("(xdc_Int)"), 0, 0, false));
        // fxn Settings.initFxn
        fxn = (Proto.Fxn)om.bind("ti.uia.family.Settings$$initFxn", new Proto.Fxn(om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family"), $$T_Str, 0, 0, false));
        // fxn Settings.startFxn
        fxn = (Proto.Fxn)om.bind("ti.uia.family.Settings$$startFxn", new Proto.Fxn(om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family"), $$T_Str, 0, 0, false));
        // fxn Settings.recvFxn
        fxn = (Proto.Fxn)om.bind("ti.uia.family.Settings$$recvFxn", new Proto.Fxn(om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family"), $$T_Str, 0, 0, false));
        // fxn Settings.sendFxn
        fxn = (Proto.Fxn)om.bind("ti.uia.family.Settings$$sendFxn", new Proto.Fxn(om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family"), $$T_Str, 0, 0, false));
        // fxn Settings.stopFxn
        fxn = (Proto.Fxn)om.bind("ti.uia.family.Settings$$stopFxn", new Proto.Fxn(om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family"), $$T_Str, 0, 0, false));
        // fxn Settings.exitFxn
        fxn = (Proto.Fxn)om.bind("ti.uia.family.Settings$$exitFxn", new Proto.Fxn(om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family"), $$T_Str, 0, 0, false));
        // fxn Settings.getSupportControl
        fxn = (Proto.Fxn)om.bind("ti.uia.family.Settings$$getSupportControl", new Proto.Fxn(om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family"), $$T_Bool, 0, 0, false));
    }

    void Settings$$SIZES()
    {
    }

    void Settings$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/family/Settings.xs");
        om.bind("ti.uia.family.Settings$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family");
        po.init("ti.uia.family.Settings.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.family.Settings$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.family.Settings$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.family.Settings$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getDefaultTransport", (Proto.Fxn)om.findStrict("ti.uia.family.Settings$$getDefaultTransport", "ti.uia.family"), Global.get(cap, "getDefaultTransport"));
                po.addFxn("getServiceMgrEventPacketSize", (Proto.Fxn)om.findStrict("ti.uia.family.Settings$$getServiceMgrEventPacketSize", "ti.uia.family"), Global.get(cap, "getServiceMgrEventPacketSize"));
                po.addFxn("getServiceMgrMsgPacketSize", (Proto.Fxn)om.findStrict("ti.uia.family.Settings$$getServiceMgrMsgPacketSize", "ti.uia.family"), Global.get(cap, "getServiceMgrMsgPacketSize"));
                po.addFxn("initFxn", (Proto.Fxn)om.findStrict("ti.uia.family.Settings$$initFxn", "ti.uia.family"), Global.get(cap, "initFxn"));
                po.addFxn("startFxn", (Proto.Fxn)om.findStrict("ti.uia.family.Settings$$startFxn", "ti.uia.family"), Global.get(cap, "startFxn"));
                po.addFxn("recvFxn", (Proto.Fxn)om.findStrict("ti.uia.family.Settings$$recvFxn", "ti.uia.family"), Global.get(cap, "recvFxn"));
                po.addFxn("sendFxn", (Proto.Fxn)om.findStrict("ti.uia.family.Settings$$sendFxn", "ti.uia.family"), Global.get(cap, "sendFxn"));
                po.addFxn("stopFxn", (Proto.Fxn)om.findStrict("ti.uia.family.Settings$$stopFxn", "ti.uia.family"), Global.get(cap, "stopFxn"));
                po.addFxn("exitFxn", (Proto.Fxn)om.findStrict("ti.uia.family.Settings$$exitFxn", "ti.uia.family"), Global.get(cap, "exitFxn"));
                po.addFxn("getSupportControl", (Proto.Fxn)om.findStrict("ti.uia.family.Settings$$getSupportControl", "ti.uia.family"), Global.get(cap, "getSupportControl"));
    }

    void Settings$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.uia.family.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.uia.family"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/family/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.uia.family"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.uia.family"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.uia.family"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.uia.family"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.uia.family"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.uia.family"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.uia.family", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.uia.family");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.uia.family.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.uia.family'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.uia.family$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.uia.family$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.uia.family$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/release/ti.uia.family.aem4',\n");
            sb.append("'lib/release/ti.uia.family.am4',\n");
            sb.append("'lib/release/ti.uia.family.am4g',\n");
            sb.append("'lib/release/ti.uia.family.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/release/ti.uia.family.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/release/ti.uia.family.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/release/ti.uia.family.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/release/ti.uia.family.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Settings$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.family.Settings", "ti.uia.family");
        po = (Proto.Obj)om.findStrict("ti.uia.family.Settings.Module", "ti.uia.family");
        vo.init2(po, "ti.uia.family.Settings", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.family.Settings$$capsule", "ti.uia.family"));
        vo.bind("$package", om.findStrict("ti.uia.family", "ti.uia.family"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.family")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.uia.family.Settings$$instance$static$init", null) ? 1 : 0);
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
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.family.Settings", "ti.uia.family"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.uia.family.Settings")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.uia.family")).add(pkgV);
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
        Settings$$OBJECTS();
        Settings$$CONSTS();
        Settings$$CREATES();
        Settings$$FUNCTIONS();
        Settings$$SIZES();
        Settings$$TYPES();
        if (isROV) {
            Settings$$ROV();
        }//isROV
        $$SINGLETONS();
        Settings$$SINGLETONS();
        $$INITIALIZATION();
    }
}
