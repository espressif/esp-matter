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

public class ti_catalog
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
        pkgP = (Proto.Obj)om.bind("ti.catalog.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.catalog", new Value.Obj("ti.catalog", pkgP));
    }

    void ICpuDataSheet$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.catalog.ICpuDataSheet.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.catalog.ICpuDataSheet", new Value.Obj("ti.catalog.ICpuDataSheet", po));
        pkgV.bind("ICpuDataSheet", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.catalog.ICpuDataSheet.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.catalog.ICpuDataSheet$$Object", new Proto.Obj());
        om.bind("ti.catalog.ICpuDataSheet.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.catalog.ICpuDataSheet$$Params", new Proto.Obj());
        om.bind("ti.catalog.ICpuDataSheet.Params", new Proto.Str(po, true));
    }

    void ICpuDataSheet$$CONSTS()
    {
        // interface ICpuDataSheet
    }

    void ICpuDataSheet$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ICpuDataSheet$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ICpuDataSheet$$SIZES()
    {
    }

    void ICpuDataSheet$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog");
        po.init("ti.catalog.ICpuDataSheet.Module", om.findStrict("xdc.platform.ICpuDataSheet.Module", "ti.catalog"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog");
        po.init("ti.catalog.ICpuDataSheet.Instance", om.findStrict("xdc.platform.ICpuDataSheet.Instance", "ti.catalog"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("isa", $$T_Str, $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.catalog.ICpuDataSheet$$Params", "ti.catalog");
        po.init("ti.catalog.ICpuDataSheet.Params", om.findStrict("xdc.platform.ICpuDataSheet$$Params", "ti.catalog"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("isa", $$T_Str, $$UNDEF, "wh");
    }

    void ICpuDataSheet$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.catalog.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.catalog"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.catalog", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.catalog");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.catalog.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.catalog'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.catalog$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.catalog$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.catalog$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void ICpuDataSheet$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.catalog.ICpuDataSheet", "ti.catalog");
        po = (Proto.Obj)om.findStrict("ti.catalog.ICpuDataSheet.Module", "ti.catalog");
        vo.init2(po, "ti.catalog.ICpuDataSheet", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.catalog.ICpuDataSheet.Instance", "ti.catalog"));
        vo.bind("Params", om.findStrict("ti.catalog.ICpuDataSheet.Params", "ti.catalog"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.catalog.ICpuDataSheet.Params", "ti.catalog")).newInstance());
        vo.bind("$package", om.findStrict("ti.catalog", "ti.catalog"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ICpuDataSheet", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ICpuDataSheet");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.catalog.ICpuDataSheet")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.catalog")).add(pkgV);
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
        ICpuDataSheet$$OBJECTS();
        ICpuDataSheet$$CONSTS();
        ICpuDataSheet$$CREATES();
        ICpuDataSheet$$FUNCTIONS();
        ICpuDataSheet$$SIZES();
        ICpuDataSheet$$TYPES();
        if (isROV) {
            ICpuDataSheet$$ROV();
        }//isROV
        $$SINGLETONS();
        ICpuDataSheet$$SINGLETONS();
        $$INITIALIZATION();
    }
}
