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

public class ti_targets
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
        Global.callFxn("loadPackage", xdcO, "xdc.bld");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.targets.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.targets", new Value.Obj("ti.targets", pkgP));
    }

    void ITarget$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.ITarget.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.ITarget", new Value.Obj("ti.targets.ITarget", po));
        pkgV.bind("ITarget", vo);
        // decls 
        om.bind("ti.targets.ITarget.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        om.bind("ti.targets.ITarget.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        om.bind("ti.targets.ITarget.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        om.bind("ti.targets.ITarget.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        om.bind("ti.targets.ITarget.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        om.bind("ti.targets.ITarget.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        om.bind("ti.targets.ITarget.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        om.bind("ti.targets.ITarget.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        om.bind("ti.targets.ITarget.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        om.bind("ti.targets.ITarget.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        om.bind("ti.targets.ITarget.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        om.bind("ti.targets.ITarget.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        om.bind("ti.targets.ITarget.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
    }

    void C28$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.C28.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.C28", new Value.Obj("ti.targets.C28", po));
        pkgV.bind("C28", vo);
        // decls 
        om.bind("ti.targets.C28.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        om.bind("ti.targets.C28.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        om.bind("ti.targets.C28.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        om.bind("ti.targets.C28.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        om.bind("ti.targets.C28.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        om.bind("ti.targets.C28.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        om.bind("ti.targets.C28.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        om.bind("ti.targets.C28.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        om.bind("ti.targets.C28.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        om.bind("ti.targets.C28.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        om.bind("ti.targets.C28.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        om.bind("ti.targets.C28.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        om.bind("ti.targets.C28.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
    }

    void C28_large$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.C28_large.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.C28_large", new Value.Obj("ti.targets.C28_large", po));
        pkgV.bind("C28_large", vo);
        // decls 
        om.bind("ti.targets.C28_large.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        om.bind("ti.targets.C28_large.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        om.bind("ti.targets.C28_large.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        om.bind("ti.targets.C28_large.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        om.bind("ti.targets.C28_large.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        om.bind("ti.targets.C28_large.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        om.bind("ti.targets.C28_large.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        om.bind("ti.targets.C28_large.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        om.bind("ti.targets.C28_large.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        om.bind("ti.targets.C28_large.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        om.bind("ti.targets.C28_large.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        om.bind("ti.targets.C28_large.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        om.bind("ti.targets.C28_large.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
    }

    void C28_float$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.C28_float.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.C28_float", new Value.Obj("ti.targets.C28_float", po));
        pkgV.bind("C28_float", vo);
        // decls 
        om.bind("ti.targets.C28_float.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        om.bind("ti.targets.C28_float.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        om.bind("ti.targets.C28_float.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        om.bind("ti.targets.C28_float.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        om.bind("ti.targets.C28_float.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        om.bind("ti.targets.C28_float.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        om.bind("ti.targets.C28_float.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        om.bind("ti.targets.C28_float.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        om.bind("ti.targets.C28_float.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        om.bind("ti.targets.C28_float.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        om.bind("ti.targets.C28_float.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        om.bind("ti.targets.C28_float.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        om.bind("ti.targets.C28_float.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
    }

    void C64P$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.C64P.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.C64P", new Value.Obj("ti.targets.C64P", po));
        pkgV.bind("C64P", vo);
        // decls 
        om.bind("ti.targets.C64P.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        om.bind("ti.targets.C64P.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        om.bind("ti.targets.C64P.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        om.bind("ti.targets.C64P.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        om.bind("ti.targets.C64P.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        om.bind("ti.targets.C64P.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        om.bind("ti.targets.C64P.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        om.bind("ti.targets.C64P.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        om.bind("ti.targets.C64P.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        om.bind("ti.targets.C64P.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        om.bind("ti.targets.C64P.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        om.bind("ti.targets.C64P.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        om.bind("ti.targets.C64P.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
    }

    void C674$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.C674.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.C674", new Value.Obj("ti.targets.C674", po));
        pkgV.bind("C674", vo);
        // decls 
        om.bind("ti.targets.C674.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        om.bind("ti.targets.C674.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        om.bind("ti.targets.C674.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        om.bind("ti.targets.C674.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        om.bind("ti.targets.C674.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        om.bind("ti.targets.C674.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        om.bind("ti.targets.C674.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        om.bind("ti.targets.C674.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        om.bind("ti.targets.C674.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        om.bind("ti.targets.C674.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        om.bind("ti.targets.C674.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        om.bind("ti.targets.C674.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        om.bind("ti.targets.C674.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
    }

    void C66$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.C66.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.C66", new Value.Obj("ti.targets.C66", po));
        pkgV.bind("C66", vo);
        // decls 
        om.bind("ti.targets.C66.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        om.bind("ti.targets.C66.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        om.bind("ti.targets.C66.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        om.bind("ti.targets.C66.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        om.bind("ti.targets.C66.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        om.bind("ti.targets.C66.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        om.bind("ti.targets.C66.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        om.bind("ti.targets.C66.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        om.bind("ti.targets.C66.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        om.bind("ti.targets.C66.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        om.bind("ti.targets.C66.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        om.bind("ti.targets.C66.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        om.bind("ti.targets.C66.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
    }

    void ITarget$$CONSTS()
    {
        // interface ITarget
    }

    void C28$$CONSTS()
    {
        // module C28
    }

    void C28_large$$CONSTS()
    {
        // module C28_large
    }

    void C28_float$$CONSTS()
    {
        // module C28_float
    }

    void C64P$$CONSTS()
    {
        // module C64P
    }

    void C674$$CONSTS()
    {
        // module C674
    }

    void C66$$CONSTS()
    {
        // module C66
    }

    void ITarget$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void C28$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void C28_large$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void C28_float$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void C64P$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void C674$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void C66$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ITarget$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ITarget.asmName
        fxn = (Proto.Fxn)om.bind("ti.targets.ITarget$$asmName", new Proto.Fxn(om.findStrict("ti.targets.ITarget.Module", "ti.targets"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "CName", $$T_Str, $$UNDEF);
    }

    void C28$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void C28_large$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void C28_float$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void C64P$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void C674$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void C66$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITarget$$SIZES()
    {
    }

    void C28$$SIZES()
    {
    }

    void C28_large$$SIZES()
    {
    }

    void C28_float$$SIZES()
    {
    }

    void C64P$$SIZES()
    {
    }

    void C674$$SIZES()
    {
    }

    void C66$$SIZES()
    {
    }

    void ITarget$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/targets/ITarget.xs");
        om.bind("ti.targets.ITarget$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.targets.ITarget.Module", "ti.targets");
        po.init("ti.targets.ITarget.Module", om.findStrict("xdc.bld.ITarget3.Module", "ti.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("stdInclude", $$T_Str, "ti/targets/std.h", "rh");
        po.addFld("binDir", $$T_Str, "$(rootDir)/bin/", "wh");
        po.addFld("pathPrefix", $$T_Str, "", "wh");
        po.addFld("binaryParser", $$T_Str, "ti.targets.omf.cof.Coff", "wh");
        po.addFld("debugGen", (Proto)om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"), Global.newObject("execTemplate", null, "execPattern", null, "packageTemplate", null, "packagePattern", null), "wh");
        po.addFld("extensions", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.Extension", "ti.targets")), Global.newArray(new Object[]{Global.newArray(new Object[]{".asm", Global.newObject("suf", ".asm", "typ", "asm")}), Global.newArray(new Object[]{".c", Global.newObject("suf", ".c", "typ", "c")}), Global.newArray(new Object[]{".cpp", Global.newObject("suf", ".cpp", "typ", "cpp")}), Global.newArray(new Object[]{".cxx", Global.newObject("suf", ".cxx", "typ", "cpp")}), Global.newArray(new Object[]{".C", Global.newObject("suf", ".C", "typ", "cpp")}), Global.newArray(new Object[]{".cc", Global.newObject("suf", ".cc", "typ", "cpp")})}), "wh");
        po.addFld("splitMap", new Proto.Map($$T_Bool), $$DEFAULT, "rh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"))}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-O2"))})}), "wh");
        po.addFld("versionMap", new Proto.Map($$T_Str), Global.newArray(new Object[]{Global.newArray(new Object[]{"TMS320C6x_4.32", "1,0,4.32,0"}), Global.newArray(new Object[]{"TMS320C2000_3.07", "1,0,3.07,0"})}), "wh");
        po.addFld("alignDirectiveSupported", $$T_Bool, true, "rh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "$(ccOpts.prefix) -mo", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-q -u _c_int00", "suffix", "-w -c -m $(XDCCFGDIR)/$@.map -l $(rootDir)/lib/libc.a"), "wh");
        po.addFld("vers", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), $$DEFAULT, "rh");
        po.addFld("version", $$T_Str, $$UNDEF, "wh");
        po.addFld("rawVersion", $$T_Str, $$UNDEF, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.targets.ITarget$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.targets.ITarget$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.targets.ITarget$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets"), fxn);
    }

    void C28$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.C28.Module", "ti.targets");
        po.init("ti.targets.C28.Module", om.findStrict("ti.targets.ITarget.Module", "ti.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "C28", "rh");
        po.addFld("suffix", $$T_Str, "28", "rh");
        po.addFld("isa", $$T_Str, "28", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets"), Global.newObject("endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "ti.targets.rts2800", "rh");
        po.addFld("alignDirectiveSupported", $$T_Bool, false, "rh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "ar2000", "opts", "rq"), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000 -c", "opts", "-v28"), "rh");
        po.addFld("vers", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000", "opts", "--compiler_revision"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000 -c", "opts", "-v28"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000", "opts", "-z"), "rh");
        po.addFld("asmOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq", "suffix", ""), "wh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq -pdsw225", "suffix", ""), "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"))}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-O2"))}), Global.newArray(new Object[]{"profile", Global.newObject("compileOpts", Global.newObject("copts", "-g --gen_profile_info"))}), Global.newArray(new Object[]{"coverage", Global.newObject("compileOpts", Global.newObject("copts", "-g --gen_profile_info"))})}), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/include", "wh");
        po.addFld("sectMap", new Proto.Map($$T_Str), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", "code"}), Global.newArray(new Object[]{".switch", "code"}), Global.newArray(new Object[]{".data", "data"}), Global.newArray(new Object[]{".cinit", "code"}), Global.newArray(new Object[]{".bss", "data"}), Global.newArray(new Object[]{".ebss", "data"}), Global.newArray(new Object[]{".econst", "code"}), Global.newArray(new Object[]{".const", "code"}), Global.newArray(new Object[]{".stack", "stack"}), Global.newArray(new Object[]{".sysmem", "data"}), Global.newArray(new Object[]{".esysmem", "data"}), Global.newArray(new Object[]{".pinit", "code"}), Global.newArray(new Object[]{".args", "data"}), Global.newArray(new Object[]{".cio", "data"})}), "rh");
        po.addFld("splitMap", new Proto.Map($$T_Bool), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", true}), Global.newArray(new Object[]{".const", true}), Global.newArray(new Object[]{".econst", true}), Global.newArray(new Object[]{".ebss", true}), Global.newArray(new Object[]{".data", true}), Global.newArray(new Object[]{".switch", true}), Global.newArray(new Object[]{".far", true}), Global.newArray(new Object[]{".args", true}), Global.newArray(new Object[]{".cio", true})}), "rh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"), Global.newObject("t_IArg", Global.newObject("size", 2L, "align", 2L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 2L, "align", 2L), "t_Float", Global.newObject("size", 2L, "align", 2L), "t_Fxn", Global.newObject("size", 2L, "align", 1L), "t_Int", Global.newObject("size", 1L, "align", 1L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 1L, "align", 1L), "t_Int32", Global.newObject("size", 2L, "align", 2L), "t_Int64", Global.newObject("size", 4L, "align", 2L), "t_Long", Global.newObject("size", 2L, "align", 2L), "t_LDouble", Global.newObject("size", 2L, "align", 2L), "t_LLong", Global.newObject("size", 4L, "align", 2L), "t_Ptr", Global.newObject("size", 1L, "align", 1L), "t_Short", Global.newObject("size", 1L, "align", 1L), "t_Size", Global.newObject("size", 2L, "align", 2L)), "rh");
        po.addFld("bitsPerChar", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets"), $$UNDEF);
    }

    void C28_large$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/targets/C28_large.xs");
        om.bind("ti.targets.C28_large$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.targets.C28_large.Module", "ti.targets");
        po.init("ti.targets.C28_large.Module", om.findStrict("ti.targets.ITarget.Module", "ti.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "C28_large", "rh");
        po.addFld("suffix", $$T_Str, "28L", "rh");
        po.addFld("isa", $$T_Str, "28", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets"), Global.newObject("dataModel", "large", "endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "ti.targets.rts2800", "rh");
        po.addFld("alignDirectiveSupported", $$T_Bool, false, "rh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "ar2000", "opts", "rq"), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000 -c", "opts", "-v28 -DLARGE_MODEL=1 -ml"), "rh");
        po.addFld("vers", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000", "opts", "--compiler_revision"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000 -c", "opts", "-v28 -ml -DLARGE_MODEL=1"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000", "opts", "-z"), "rh");
        po.addFld("asmOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq", "suffix", ""), "wh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq -pdsw225 -Dfar= ", "suffix", ""), "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"))}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-O2"))}), Global.newArray(new Object[]{"profile", Global.newObject("compileOpts", Global.newObject("copts", "-g --gen_profile_info"))}), Global.newArray(new Object[]{"coverage", Global.newObject("compileOpts", Global.newObject("copts", "-g --gen_profile_info"))})}), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/include", "wh");
        po.addFld("sectMap", new Proto.Map($$T_Str), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", "code"}), Global.newArray(new Object[]{".switch", "code"}), Global.newArray(new Object[]{".data", "data"}), Global.newArray(new Object[]{".cinit", "code"}), Global.newArray(new Object[]{".bss", "data"}), Global.newArray(new Object[]{".ebss", "data"}), Global.newArray(new Object[]{".econst", "code"}), Global.newArray(new Object[]{".const", "code"}), Global.newArray(new Object[]{".stack", "stack"}), Global.newArray(new Object[]{".sysmem", "data"}), Global.newArray(new Object[]{".esysmem", "data"}), Global.newArray(new Object[]{".pinit", "code"}), Global.newArray(new Object[]{".args", "data"}), Global.newArray(new Object[]{".cio", "data"})}), "rh");
        po.addFld("splitMap", new Proto.Map($$T_Bool), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", true}), Global.newArray(new Object[]{".const", true}), Global.newArray(new Object[]{".econst", true}), Global.newArray(new Object[]{".ebss", true}), Global.newArray(new Object[]{".data", true}), Global.newArray(new Object[]{".switch", true}), Global.newArray(new Object[]{".far", true}), Global.newArray(new Object[]{".args", true}), Global.newArray(new Object[]{".cio", true})}), "rh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"), Global.newObject("t_IArg", Global.newObject("size", 2L, "align", 2L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 2L, "align", 2L), "t_Float", Global.newObject("size", 2L, "align", 2L), "t_Fxn", Global.newObject("size", 2L, "align", 2L), "t_Int", Global.newObject("size", 1L, "align", 1L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 1L, "align", 1L), "t_Int32", Global.newObject("size", 2L, "align", 2L), "t_Int64", Global.newObject("size", 4L, "align", 2L), "t_Long", Global.newObject("size", 2L, "align", 2L), "t_LDouble", Global.newObject("size", 2L, "align", 2L), "t_LLong", Global.newObject("size", 4L, "align", 2L), "t_Ptr", Global.newObject("size", 2L, "align", 2L), "t_Short", Global.newObject("size", 1L, "align", 1L), "t_Size", Global.newObject("size", 2L, "align", 2L)), "rh");
        po.addFld("bitsPerChar", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.targets.C28_large$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.targets.C28_large$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.targets.C28_large$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets"), fxn);
    }

    void C28_float$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/targets/C28_float.xs");
        om.bind("ti.targets.C28_float$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.targets.C28_float.Module", "ti.targets");
        po.init("ti.targets.C28_float.Module", om.findStrict("ti.targets.ITarget.Module", "ti.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "C28_float", "rh");
        po.addFld("suffix", $$T_Str, "28FP", "rh");
        po.addFld("isa", $$T_Str, "28FP", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets"), Global.newObject("dataModel", "large", "endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "ti.targets.rts2800", "rh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"28L"}), "wh");
        po.addFld("alignDirectiveSupported", $$T_Bool, false, "rh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "ar2000", "opts", "rq"), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000 -c", "opts", "-v28 -DLARGE_MODEL=1 -ml --float_support=fpu32"), "rh");
        po.addFld("vers", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000", "opts", "--compiler_revision"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000 -c", "opts", "-v28 -ml -DLARGE_MODEL=1 --float_support=fpu32"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl2000", "opts", "-z"), "rh");
        po.addFld("asmOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq", "suffix", ""), "wh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq -pdsw225 -Dfar= ", "suffix", ""), "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"))}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-O2"))}), Global.newArray(new Object[]{"profile", Global.newObject("compileOpts", Global.newObject("copts", "-g --gen_profile_info"))}), Global.newArray(new Object[]{"coverage", Global.newObject("compileOpts", Global.newObject("copts", "-g --gen_profile_info"))})}), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/include", "wh");
        po.addFld("sectMap", new Proto.Map($$T_Str), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", "code"}), Global.newArray(new Object[]{".switch", "code"}), Global.newArray(new Object[]{".data", "data"}), Global.newArray(new Object[]{".cinit", "code"}), Global.newArray(new Object[]{".bss", "data"}), Global.newArray(new Object[]{".ebss", "data"}), Global.newArray(new Object[]{".econst", "code"}), Global.newArray(new Object[]{".const", "code"}), Global.newArray(new Object[]{".stack", "stack"}), Global.newArray(new Object[]{".sysmem", "data"}), Global.newArray(new Object[]{".esysmem", "data"}), Global.newArray(new Object[]{".pinit", "code"}), Global.newArray(new Object[]{".args", "data"}), Global.newArray(new Object[]{".cio", "data"})}), "rh");
        po.addFld("splitMap", new Proto.Map($$T_Bool), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", true}), Global.newArray(new Object[]{".const", true}), Global.newArray(new Object[]{".econst", true}), Global.newArray(new Object[]{".ebss", true}), Global.newArray(new Object[]{".data", true}), Global.newArray(new Object[]{".switch", true}), Global.newArray(new Object[]{".far", true}), Global.newArray(new Object[]{".args", true}), Global.newArray(new Object[]{".cio", true})}), "rh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"), Global.newObject("t_IArg", Global.newObject("size", 2L, "align", 2L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 2L, "align", 2L), "t_Float", Global.newObject("size", 2L, "align", 2L), "t_Fxn", Global.newObject("size", 2L, "align", 2L), "t_Int", Global.newObject("size", 1L, "align", 1L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 1L, "align", 1L), "t_Int32", Global.newObject("size", 2L, "align", 2L), "t_Int64", Global.newObject("size", 4L, "align", 2L), "t_Long", Global.newObject("size", 2L, "align", 2L), "t_LDouble", Global.newObject("size", 2L, "align", 2L), "t_LLong", Global.newObject("size", 4L, "align", 2L), "t_Ptr", Global.newObject("size", 2L, "align", 2L), "t_Short", Global.newObject("size", 1L, "align", 1L), "t_Size", Global.newObject("size", 2L, "align", 2L)), "rh");
        po.addFld("bitsPerChar", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.targets.C28_float$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.targets.C28_float$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.targets.C28_float$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets"), fxn);
    }

    void C64P$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.C64P.Module", "ti.targets");
        po.init("ti.targets.C64P.Module", om.findStrict("ti.targets.ITarget.Module", "ti.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "C64P", "rh");
        po.addFld("suffix", $$T_Str, "64P", "rh");
        po.addFld("isa", $$T_Str, "64P", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets"), Global.newObject("endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "ti.targets.rts6000", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.sim6xxx:TMS320CDM420", "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"64", "62"}), "wh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "ar6x", "opts", "rq"), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x -c", "opts", "-mv64p"), "rh");
        po.addFld("vers", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x", "opts", "--compiler_revision"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x -c", "opts", "-mv64p"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x", "opts", "-z"), "rh");
        po.addFld("asmOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq", "suffix", ""), "wh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq -pdsw225", "suffix", ""), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "$(ccOpts.prefix) -mo --no_compress", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/include", "wh");
        po.addFld("sectMap", new Proto.Map($$T_Str), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", "code"}), Global.newArray(new Object[]{".stack", "stack"}), Global.newArray(new Object[]{".bss", "data"}), Global.newArray(new Object[]{".cinit", "data"}), Global.newArray(new Object[]{".pinit", "data"}), Global.newArray(new Object[]{".const", "data"}), Global.newArray(new Object[]{".data", "data"}), Global.newArray(new Object[]{".switch", "data"}), Global.newArray(new Object[]{".sysmem", "data"}), Global.newArray(new Object[]{".far", "data"}), Global.newArray(new Object[]{".args", "data"}), Global.newArray(new Object[]{".cio", "data"})}), "rh");
        po.addFld("splitMap", new Proto.Map($$T_Bool), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", true}), Global.newArray(new Object[]{".const", true}), Global.newArray(new Object[]{".data", true}), Global.newArray(new Object[]{".switch", true}), Global.newArray(new Object[]{".far", true}), Global.newArray(new Object[]{".args", true}), Global.newArray(new Object[]{".cio", true})}), "rh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int40", Global.newObject("size", 8L, "align", 8L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 8L, "align", 8L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        po.addFld("bitsPerChar", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets"), $$UNDEF);
    }

    void C674$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.C674.Module", "ti.targets");
        po.init("ti.targets.C674.Module", om.findStrict("ti.targets.ITarget.Module", "ti.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "C674", "rh");
        po.addFld("suffix", $$T_Str, "674", "rh");
        po.addFld("isa", $$T_Str, "674", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets"), Global.newObject("endian", "little"), "rh");
        po.addFld("base", (Proto)om.findStrict("xdc.bld.ITarget.Module", "ti.targets"), om.find("ti.targets.C64P"), "rh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"64P", "64", "62"}), "wh");
        po.addFld("rts", $$T_Str, "ti.targets.rts6000", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.evmDA830", "wh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "ar6x", "opts", "rq"), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x -c", "opts", "-mv6740"), "rh");
        po.addFld("vers", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x", "opts", "--compiler_revision"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x -c", "opts", "-mv6740"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x", "opts", "-z"), "rh");
        po.addFld("asmOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq", "suffix", ""), "wh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq -pdsw225", "suffix", ""), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "$(ccOpts.prefix) -mo --no_compress", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/include", "wh");
        po.addFld("sectMap", new Proto.Map($$T_Str), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", "code"}), Global.newArray(new Object[]{".stack", "stack"}), Global.newArray(new Object[]{".bss", "data"}), Global.newArray(new Object[]{".cinit", "data"}), Global.newArray(new Object[]{".pinit", "data"}), Global.newArray(new Object[]{".const", "data"}), Global.newArray(new Object[]{".data", "data"}), Global.newArray(new Object[]{".switch", "data"}), Global.newArray(new Object[]{".sysmem", "data"}), Global.newArray(new Object[]{".far", "data"}), Global.newArray(new Object[]{".args", "data"}), Global.newArray(new Object[]{".cio", "data"})}), "rh");
        po.addFld("splitMap", new Proto.Map($$T_Bool), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", true}), Global.newArray(new Object[]{".const", true}), Global.newArray(new Object[]{".data", true}), Global.newArray(new Object[]{".switch", true}), Global.newArray(new Object[]{".far", true}), Global.newArray(new Object[]{".args", true}), Global.newArray(new Object[]{".cio", true})}), "rh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int40", Global.newObject("size", 8L, "align", 8L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 8L, "align", 8L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        po.addFld("bitsPerChar", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets"), $$UNDEF);
    }

    void C66$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.C66.Module", "ti.targets");
        po.init("ti.targets.C66.Module", om.findStrict("ti.targets.ITarget.Module", "ti.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "C66", "rh");
        po.addFld("suffix", $$T_Str, "66", "rh");
        po.addFld("isa", $$T_Str, "66", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets"), Global.newObject("endian", "little"), "rh");
        po.addFld("base", (Proto)om.findStrict("xdc.bld.ITarget.Module", "ti.targets"), om.find("ti.targets.C64P"), "rh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"674", "64P"}), "wh");
        po.addFld("rts", $$T_Str, "ti.targets.rts6000", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.simTCI6616", "wh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "ar6x", "opts", "rq"), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x -c", "opts", "-mv6600 --abi=coffabi"), "rh");
        po.addFld("vers", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x", "opts", "--compiler_revision"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x -c", "opts", "-mv6600 --abi=coffabi"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"), Global.newObject("cmd", "cl6x", "opts", "--abi=coffabi -z"), "rh");
        po.addFld("asmOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq", "suffix", ""), "wh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "-qq -pdsw225", "suffix", ""), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"), Global.newObject("prefix", "$(ccOpts.prefix) -mo --no_compress", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/include", "wh");
        po.addFld("sectMap", new Proto.Map($$T_Str), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", "code"}), Global.newArray(new Object[]{".stack", "stack"}), Global.newArray(new Object[]{".bss", "data"}), Global.newArray(new Object[]{".cinit", "data"}), Global.newArray(new Object[]{".pinit", "data"}), Global.newArray(new Object[]{".const", "data"}), Global.newArray(new Object[]{".data", "data"}), Global.newArray(new Object[]{".switch", "data"}), Global.newArray(new Object[]{".sysmem", "data"}), Global.newArray(new Object[]{".far", "data"}), Global.newArray(new Object[]{".args", "data"}), Global.newArray(new Object[]{".cio", "data"})}), "rh");
        po.addFld("splitMap", new Proto.Map($$T_Bool), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", true}), Global.newArray(new Object[]{".const", true}), Global.newArray(new Object[]{".data", true}), Global.newArray(new Object[]{".switch", true}), Global.newArray(new Object[]{".far", true}), Global.newArray(new Object[]{".args", true}), Global.newArray(new Object[]{".cio", true})}), "rh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int40", Global.newObject("size", 8L, "align", 8L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 8L, "align", 8L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        po.addFld("bitsPerChar", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets"), $$UNDEF);
    }

    void ITarget$$ROV()
    {
    }

    void C28$$ROV()
    {
    }

    void C28_large$$ROV()
    {
    }

    void C28_float$$ROV()
    {
    }

    void C64P$$ROV()
    {
    }

    void C674$$ROV()
    {
    }

    void C66$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.targets.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.targets"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/targets/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.targets"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.targets"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.targets"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.targets"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.targets"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.targets"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.targets", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.targets");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.targets.");
        pkgV.bind("$vers", Global.newArray(1, 0, 3));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.bld", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.targets'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.targets$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.targets$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.targets$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void ITarget$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.ITarget", "ti.targets");
        po = (Proto.Obj)om.findStrict("ti.targets.ITarget.Module", "ti.targets");
        vo.init2(po, "ti.targets.ITarget", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("ti.targets.ITarget$$capsule", "ti.targets"));
        vo.bind("$package", om.findStrict("ti.targets", "ti.targets"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITarget", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITarget");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void C28$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.C28", "ti.targets");
        po = (Proto.Obj)om.findStrict("ti.targets.C28.Module", "ti.targets");
        vo.init2(po, "ti.targets.C28", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets", "ti.targets"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.C28$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("C28", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("C28");
    }

    void C28_large$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.C28_large", "ti.targets");
        po = (Proto.Obj)om.findStrict("ti.targets.C28_large.Module", "ti.targets");
        vo.init2(po, "ti.targets.C28_large", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.targets.C28_large$$capsule", "ti.targets"));
        vo.bind("$package", om.findStrict("ti.targets", "ti.targets"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.C28_large$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("C28_large", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("C28_large");
    }

    void C28_float$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.C28_float", "ti.targets");
        po = (Proto.Obj)om.findStrict("ti.targets.C28_float.Module", "ti.targets");
        vo.init2(po, "ti.targets.C28_float", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.targets.C28_float$$capsule", "ti.targets"));
        vo.bind("$package", om.findStrict("ti.targets", "ti.targets"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.C28_float$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("C28_float", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("C28_float");
    }

    void C64P$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.C64P", "ti.targets");
        po = (Proto.Obj)om.findStrict("ti.targets.C64P.Module", "ti.targets");
        vo.init2(po, "ti.targets.C64P", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets", "ti.targets"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.C64P$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("C64P", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("C64P");
    }

    void C674$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.C674", "ti.targets");
        po = (Proto.Obj)om.findStrict("ti.targets.C674.Module", "ti.targets");
        vo.init2(po, "ti.targets.C674", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets", "ti.targets"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.C674$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("C674", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("C674");
    }

    void C66$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.C66", "ti.targets");
        po = (Proto.Obj)om.findStrict("ti.targets.C66.Module", "ti.targets");
        vo.init2(po, "ti.targets.C66", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets", "ti.targets"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.C66$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("C66", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("C66");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.C28", "ti.targets"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.C28_large", "ti.targets"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.C28_float", "ti.targets"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.C64P", "ti.targets"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.C674", "ti.targets"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.C66", "ti.targets"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.targets.ITarget")).bless();
        ((Value.Obj)om.getv("ti.targets.C28")).bless();
        ((Value.Obj)om.getv("ti.targets.C28_large")).bless();
        ((Value.Obj)om.getv("ti.targets.C28_float")).bless();
        ((Value.Obj)om.getv("ti.targets.C64P")).bless();
        ((Value.Obj)om.getv("ti.targets.C674")).bless();
        ((Value.Obj)om.getv("ti.targets.C66")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.targets")).add(pkgV);
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
        ITarget$$OBJECTS();
        C28$$OBJECTS();
        C28_large$$OBJECTS();
        C28_float$$OBJECTS();
        C64P$$OBJECTS();
        C674$$OBJECTS();
        C66$$OBJECTS();
        ITarget$$CONSTS();
        C28$$CONSTS();
        C28_large$$CONSTS();
        C28_float$$CONSTS();
        C64P$$CONSTS();
        C674$$CONSTS();
        C66$$CONSTS();
        ITarget$$CREATES();
        C28$$CREATES();
        C28_large$$CREATES();
        C28_float$$CREATES();
        C64P$$CREATES();
        C674$$CREATES();
        C66$$CREATES();
        ITarget$$FUNCTIONS();
        C28$$FUNCTIONS();
        C28_large$$FUNCTIONS();
        C28_float$$FUNCTIONS();
        C64P$$FUNCTIONS();
        C674$$FUNCTIONS();
        C66$$FUNCTIONS();
        ITarget$$SIZES();
        C28$$SIZES();
        C28_large$$SIZES();
        C28_float$$SIZES();
        C64P$$SIZES();
        C674$$SIZES();
        C66$$SIZES();
        ITarget$$TYPES();
        C28$$TYPES();
        C28_large$$TYPES();
        C28_float$$TYPES();
        C64P$$TYPES();
        C674$$TYPES();
        C66$$TYPES();
        if (isROV) {
            ITarget$$ROV();
            C28$$ROV();
            C28_large$$ROV();
            C28_float$$ROV();
            C64P$$ROV();
            C674$$ROV();
            C66$$ROV();
        }//isROV
        $$SINGLETONS();
        ITarget$$SINGLETONS();
        C28$$SINGLETONS();
        C28_large$$SINGLETONS();
        C28_float$$SINGLETONS();
        C64P$$SINGLETONS();
        C674$$SINGLETONS();
        C66$$SINGLETONS();
        $$INITIALIZATION();
    }
}
