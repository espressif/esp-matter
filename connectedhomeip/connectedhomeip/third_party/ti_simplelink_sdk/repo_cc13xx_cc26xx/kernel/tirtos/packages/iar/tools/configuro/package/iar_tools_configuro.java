/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-F07
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class iar_tools_configuro
{
    static final String VERS = "@(#) xdc-F07\n";

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
        Global.callFxn("loadPackage", xdcO, "xdc.tools");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("iar.tools.configuro.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("iar.tools.configuro", new Value.Obj("iar.tools.configuro", pkgP));
    }

    void Main$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.tools.configuro.Main.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.tools.configuro.Main", new Value.Obj("iar.tools.configuro.Main", po));
        pkgV.bind("Main", vo);
        // decls 
        // insts 
        Object insP = om.bind("iar.tools.configuro.Main.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("iar.tools.configuro.Main$$Object", new Proto.Obj());
        om.bind("iar.tools.configuro.Main.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("iar.tools.configuro.Main$$Params", new Proto.Obj());
        om.bind("iar.tools.configuro.Main.Params", new Proto.Str(po, true));
    }

    void Main$$CONSTS()
    {
        // module Main
    }

    void Main$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("iar.tools.configuro.Main$$create", new Proto.Fxn(om.findStrict("iar.tools.configuro.Main.Module", "iar.tools.configuro"), om.findStrict("iar.tools.configuro.Main.Instance", "iar.tools.configuro"), 1, 0, false));
                fxn.addArg(0, "__params", (Proto)om.findStrict("iar.tools.configuro.Main.Params", "iar.tools.configuro"), Global.newObject());
        sb = new StringBuilder();
        sb.append("iar$tools$configuro$Main$$create = function( __params ) {\n");
            sb.append("var __mod = xdc.om['iar.tools.configuro.Main'];\n");
            sb.append("var __inst = xdc.om['iar.tools.configuro.Main.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['iar.tools.configuro']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("iar.tools.configuro.Main$$construct", new Proto.Fxn(om.findStrict("iar.tools.configuro.Main.Module", "iar.tools.configuro"), null, 2, 0, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("iar.tools.configuro.Main$$Object", "iar.tools.configuro"), null);
                fxn.addArg(1, "__params", (Proto)om.findStrict("iar.tools.configuro.Main.Params", "iar.tools.configuro"), Global.newObject());
        sb = new StringBuilder();
        sb.append("iar$tools$configuro$Main$$construct = function( __obj, __params ) {\n");
            sb.append("var __mod = xdc.om['iar.tools.configuro.Main'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void Main$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Main$$SIZES()
    {
    }

    void Main$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "iar/tools/configuro/Main.xs");
        om.bind("iar.tools.configuro.Main$$capsule", cap);
        po = (Proto.Obj)om.findStrict("iar.tools.configuro.Main.Module", "iar.tools.configuro");
        po.init("iar.tools.configuro.Main.Module", om.findStrict("xdc.tools.ICmd.Module", "iar.tools.configuro"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("usage", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"[-o outdir (optional)]", "[-c codegen_dir]", "[--cc compiler_name_string]", "[--device device_name]", "[--cfgArgs args_string]", "[--compileOptions compile_options_string]", "[--linkOptions linker_options_string]", "[--profile profile]", "[--projFile project_file (optional)]", "infile.cfg (optional)"}), "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("iar.tools.configuro.Main$$create", "iar.tools.configuro"), Global.get("iar$tools$configuro$Main$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("iar.tools.configuro.Main$$construct", "iar.tools.configuro"), Global.get("iar$tools$configuro$Main$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("iar.tools.configuro.Main$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("iar.tools.configuro.Main$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("iar.tools.configuro.Main$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("iar.tools.configuro.Main$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "main");
                if (fxn != null) po.addFxn("main", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$main", "iar.tools.configuro"), fxn);
                fxn = Global.get(cap, "exec");
                if (fxn != null) po.addFxn("exec", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$exec", "iar.tools.configuro"), fxn);
        po = (Proto.Obj)om.findStrict("iar.tools.configuro.Main.Instance", "iar.tools.configuro");
        po.init("iar.tools.configuro.Main.Instance", om.findStrict("xdc.tools.ICmd.Instance", "iar.tools.configuro"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("output", $$T_Str, "", "wh");
        po.addFld("rootDir", $$T_Str, null, "wh");
        po.addFld("compiler", $$T_Str, "", "wh");
        po.addFld("device", $$T_Str, null, "wh");
        po.addFld("compileOptions", $$T_Str, null, "wh");
        po.addFld("linkOptions", $$T_Str, null, "wh");
        po.addFld("profile", $$T_Str, "", "wh");
        po.addFld("cfgArgs", $$T_Str, "", "wh");
        po.addFld("projFile", $$T_Str, "", "wh");
                fxn = Global.get(cap, "run");
                if (fxn != null) po.addFxn("run", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$run", "iar.tools.configuro"), fxn);
        po = (Proto.Obj)om.findStrict("iar.tools.configuro.Main$$Params", "iar.tools.configuro");
        po.init("iar.tools.configuro.Main.Params", om.findStrict("xdc.tools.ICmd$$Params", "iar.tools.configuro"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("output", $$T_Str, "", "wh");
        po.addFld("rootDir", $$T_Str, null, "wh");
        po.addFld("compiler", $$T_Str, "", "wh");
        po.addFld("device", $$T_Str, null, "wh");
        po.addFld("compileOptions", $$T_Str, null, "wh");
        po.addFld("linkOptions", $$T_Str, null, "wh");
        po.addFld("profile", $$T_Str, "", "wh");
        po.addFld("cfgArgs", $$T_Str, "", "wh");
        po.addFld("projFile", $$T_Str, "", "wh");
        po = (Proto.Obj)om.findStrict("iar.tools.configuro.Main$$Object", "iar.tools.configuro");
        po.init("iar.tools.configuro.Main.Object", om.findStrict("iar.tools.configuro.Main.Instance", "iar.tools.configuro"));
                fxn = Global.get(cap, "run");
                if (fxn != null) po.addFxn("run", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$run", "iar.tools.configuro"), fxn);
    }

    void Main$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("iar.tools.configuro.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "iar.tools.configuro"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "iar.tools.configuro", Value.DEFAULT, false);
        pkgV.bind("$name", "iar.tools.configuro");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "iar.tools.configuro.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['iar.tools.configuro'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('iar.tools.configuro$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['iar.tools.configuro$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['iar.tools.configuro$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Main$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.tools.configuro.Main", "iar.tools.configuro");
        po = (Proto.Obj)om.findStrict("iar.tools.configuro.Main.Module", "iar.tools.configuro");
        vo.init2(po, "iar.tools.configuro.Main", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("iar.tools.configuro.Main$$capsule", "iar.tools.configuro"));
        vo.bind("Instance", om.findStrict("iar.tools.configuro.Main.Instance", "iar.tools.configuro"));
        vo.bind("Params", om.findStrict("iar.tools.configuro.Main.Params", "iar.tools.configuro"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("iar.tools.configuro.Main.Params", "iar.tools.configuro")).newInstance());
        vo.bind("$package", om.findStrict("iar.tools.configuro", "iar.tools.configuro"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.tools");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.tools.configuro")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.tools.configuro.Main$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("iar.tools.configuro.Main.Object", "iar.tools.configuro"));
        pkgV.bind("Main", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Main");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.tools.configuro.Main", "iar.tools.configuro"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("iar.tools.configuro.Main")).bless();
        ((Value.Arr)om.findStrict("$packages", "iar.tools.configuro")).add(pkgV);
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
        Main$$OBJECTS();
        Main$$CONSTS();
        Main$$CREATES();
        Main$$FUNCTIONS();
        Main$$SIZES();
        Main$$TYPES();
        if (isROV) {
            Main$$ROV();
        }//isROV
        $$SINGLETONS();
        Main$$SINGLETONS();
        $$INITIALIZATION();
    }
}
