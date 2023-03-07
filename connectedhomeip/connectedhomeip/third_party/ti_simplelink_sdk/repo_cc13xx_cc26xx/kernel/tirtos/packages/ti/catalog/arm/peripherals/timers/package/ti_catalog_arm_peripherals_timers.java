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

public class ti_catalog_arm_peripherals_timers
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
        Global.callFxn("loadPackage", xdcO, "xdc.platform");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.catalog.arm.peripherals.timers.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.catalog.arm.peripherals.timers", new Value.Obj("ti.catalog.arm.peripherals.timers", pkgP));
    }

    void Timer$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.arm.peripherals.timers.Timer.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.arm.peripherals.timers.Timer", new Value.Obj("ti.catalog.arm.peripherals.timers.Timer", po));
        pkgV.bind("Timer", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.arm.peripherals.timers.Timer.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.arm.peripherals.timers.Timer$$Object", new Proto.Obj());
        om.bind("ti.catalog.arm.peripherals.timers.Timer.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.arm.peripherals.timers.Timer$$Params", new Proto.Obj());
        om.bind("ti.catalog.arm.peripherals.timers.Timer.Params", new Proto.Str(po, true));
    }

    void Timer$$CONSTS()
    {
        // module Timer
    }

    void Timer$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.peripherals.timers.Timer$$create", new Proto.Fxn(om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Module", "ti.catalog.arm.peripherals.timers"), om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.peripherals.timers"), 1, 0, false));
                fxn.addArg(0, "__params", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Params", "ti.catalog.arm.peripherals.timers"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$peripherals$timers$Timer$$create = function( __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.peripherals.timers.Timer'];\n");
            sb.append("var __inst = xdc.om['ti.catalog.arm.peripherals.timers.Timer.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.catalog.arm.peripherals.timers']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.name = __mod.PARAMS.name;\n");
            sb.append("__inst.owner = __mod.PARAMS.owner;\n");
            sb.append("__inst.baseAddr = __mod.PARAMS.baseAddr;\n");
            sb.append("__inst.intNum = __mod.PARAMS.intNum;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.catalog.arm.peripherals.timers.Timer$$construct", new Proto.Fxn(om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Module", "ti.catalog.arm.peripherals.timers"), null, 2, 0, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer$$Object", "ti.catalog.arm.peripherals.timers"), null);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Params", "ti.catalog.arm.peripherals.timers"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$catalog$arm$peripherals$timers$Timer$$construct = function( __obj, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.catalog.arm.peripherals.timers.Timer'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.name = __mod.PARAMS.name;\n");
            sb.append("__inst.owner = __mod.PARAMS.owner;\n");
            sb.append("__inst.baseAddr = __mod.PARAMS.baseAddr;\n");
            sb.append("__inst.intNum = __mod.PARAMS.intNum;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void Timer$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Timer$$SIZES()
    {
    }

    void Timer$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Module", "ti.catalog.arm.peripherals.timers");
        po.init("ti.catalog.arm.peripherals.timers.Timer.Module", om.findStrict("xdc.platform.IPeripheral.Module", "ti.catalog.arm.peripherals.timers"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.catalog.arm.peripherals.timers.Timer$$create", "ti.catalog.arm.peripherals.timers"), Global.get("ti$catalog$arm$peripherals$timers$Timer$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.catalog.arm.peripherals.timers.Timer$$construct", "ti.catalog.arm.peripherals.timers"), Global.get("ti$catalog$arm$peripherals$timers$Timer$$construct"));
                po.addFxn("addPeripheralsMap", (Proto.Fxn)om.findStrict("xdc.platform.IPeripheral$$addPeripheralsMap", "ti.catalog.arm.peripherals.timers"), $$UNDEF);
                po.addFxn("getAll", (Proto.Fxn)om.findStrict("xdc.platform.IPeripheral$$getAll", "ti.catalog.arm.peripherals.timers"), $$UNDEF);
                po.addFxn("getRegisters", (Proto.Fxn)om.findStrict("xdc.platform.IPeripheral$$getRegisters", "ti.catalog.arm.peripherals.timers"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.peripherals.timers");
        po.init("ti.catalog.arm.peripherals.timers.Timer.Instance", om.findStrict("xdc.platform.IPeripheral.Instance", "ti.catalog.arm.peripherals.timers"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("baseAddr", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.peripherals.timers.Timer$$Params", "ti.catalog.arm.peripherals.timers");
        po.init("ti.catalog.arm.peripherals.timers.Timer.Params", om.findStrict("xdc.platform.IPeripheral$$Params", "ti.catalog.arm.peripherals.timers"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("baseAddr", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.peripherals.timers.Timer$$Object", "ti.catalog.arm.peripherals.timers");
        po.init("ti.catalog.arm.peripherals.timers.Timer.Object", om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.peripherals.timers"));
    }

    void Timer$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.catalog.arm.peripherals.timers.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.catalog.arm.peripherals.timers"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.catalog.arm.peripherals.timers", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.catalog.arm.peripherals.timers");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.catalog.arm.peripherals.timers.");
        pkgV.bind("$vers", Global.newArray());
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.catalog.arm.peripherals.timers'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.catalog.arm.peripherals.timers$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.catalog.arm.peripherals.timers$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.catalog.arm.peripherals.timers$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Timer$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.arm.peripherals.timers.Timer", "ti.catalog.arm.peripherals.timers");
        po = (Proto.Obj)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Module", "ti.catalog.arm.peripherals.timers");
        vo.init2(po, "ti.catalog.arm.peripherals.timers.Timer", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Instance", "ti.catalog.arm.peripherals.timers"));
        vo.bind("Params", om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Params", "ti.catalog.arm.peripherals.timers"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Params", "ti.catalog.arm.peripherals.timers")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog.arm.peripherals.timers", "ti.catalog.arm.peripherals.timers"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("IPeripheralArray", om.findStrict("xdc.platform.IPeripheral.IPeripheralArray", "ti.catalog.arm.peripherals.timers"));
        vo.bind("StringArray", om.findStrict("xdc.platform.IPeripheral.StringArray", "ti.catalog.arm.peripherals.timers"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.catalog.arm.peripherals.timers")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.catalog.arm.peripherals.timers.Timer$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.catalog.arm.peripherals.timers.Timer.Object", "ti.catalog.arm.peripherals.timers"));
        pkgV.bind("Timer", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Timer");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.catalog.arm.peripherals.timers.Timer", "ti.catalog.arm.peripherals.timers"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.catalog.arm.peripherals.timers.Timer")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.catalog.arm.peripherals.timers")).add(pkgV);
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
        Timer$$OBJECTS();
        Timer$$CONSTS();
        Timer$$CREATES();
        Timer$$FUNCTIONS();
        Timer$$SIZES();
        Timer$$TYPES();
        if (isROV) {
            Timer$$ROV();
        }//isROV
        $$SINGLETONS();
        Timer$$SINGLETONS();
        $$INITIALIZATION();
    }
}
