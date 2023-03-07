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

public class iar_targets_arm
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
        Global.callFxn("loadPackage", xdcO, "xdc.bld");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("iar.targets.arm.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("iar.targets.arm", new Value.Obj("iar.targets.arm", pkgP));
    }

    void ITarget$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.targets.arm.ITarget.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.targets.arm.ITarget", new Value.Obj("iar.targets.arm.ITarget", po));
        pkgV.bind("ITarget", vo);
        // decls 
        om.bind("iar.targets.arm.ITarget.Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        om.bind("iar.targets.arm.ITarget.Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
    }

    void M0$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.targets.arm.M0.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.targets.arm.M0", new Value.Obj("iar.targets.arm.M0", po));
        pkgV.bind("M0", vo);
        // decls 
        om.bind("iar.targets.arm.M0.Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        om.bind("iar.targets.arm.M0.Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
    }

    void M3$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.targets.arm.M3.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.targets.arm.M3", new Value.Obj("iar.targets.arm.M3", po));
        pkgV.bind("M3", vo);
        // decls 
        om.bind("iar.targets.arm.M3.Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        om.bind("iar.targets.arm.M3.Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
    }

    void M4$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.targets.arm.M4.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.targets.arm.M4", new Value.Obj("iar.targets.arm.M4", po));
        pkgV.bind("M4", vo);
        // decls 
        om.bind("iar.targets.arm.M4.Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4.Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
    }

    void M4F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.targets.arm.M4F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.targets.arm.M4F", new Value.Obj("iar.targets.arm.M4F", po));
        pkgV.bind("M4F", vo);
        // decls 
        om.bind("iar.targets.arm.M4F.Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        om.bind("iar.targets.arm.M4F.Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
    }

    void M33$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.targets.arm.M33.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.targets.arm.M33", new Value.Obj("iar.targets.arm.M33", po));
        pkgV.bind("M33", vo);
        // decls 
        om.bind("iar.targets.arm.M33.Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33.Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
    }

    void M33F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.targets.arm.M33F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.targets.arm.M33F", new Value.Obj("iar.targets.arm.M33F", po));
        pkgV.bind("M33F", vo);
        // decls 
        om.bind("iar.targets.arm.M33F.Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        om.bind("iar.targets.arm.M33F.Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
    }

    void ITarget$$CONSTS()
    {
        // interface ITarget
    }

    void M0$$CONSTS()
    {
        // module M0
    }

    void M3$$CONSTS()
    {
        // module M3
    }

    void M4$$CONSTS()
    {
        // module M4
    }

    void M4F$$CONSTS()
    {
        // module M4F
    }

    void M33$$CONSTS()
    {
        // module M33
    }

    void M33F$$CONSTS()
    {
        // module M33F
    }

    void ITarget$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void M0$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void M3$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void M4$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void M4F$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void M33$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void M33F$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ITarget$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void M0$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void M3$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void M4$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void M4F$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void M33$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void M33F$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITarget$$SIZES()
    {
    }

    void M0$$SIZES()
    {
    }

    void M3$$SIZES()
    {
    }

    void M4$$SIZES()
    {
    }

    void M4F$$SIZES()
    {
    }

    void M33$$SIZES()
    {
    }

    void M33F$$SIZES()
    {
    }

    void ITarget$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "iar/targets/arm/ITarget.xs");
        om.bind("iar.targets.arm.ITarget$$capsule", cap);
        po = (Proto.Obj)om.findStrict("iar.targets.arm.ITarget.Module", "iar.targets.arm");
        po.init("iar.targets.arm.ITarget.Module", om.findStrict("xdc.bld.ITarget3.Module", "iar.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("rts", $$T_Str, "iar.targets.arm.rts", "rh");
        po.addFld("stdInclude", $$T_Str, "iar/targets/arm/std.h", "wh");
        po.addFld("binDir", $$T_Str, "$(rootDir)/bin/", "wh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"), Global.newObject("endian", "little", "codeModel", "thumb2", "shortEnums", true), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("iar.targets.arm.ITarget.Options", "iar.targets.arm"), Global.newObject("prefix", "--silent", "suffix", ""), "wh");
        po.addFld("asmOpts", (Proto)om.findStrict("iar.targets.arm.ITarget.Options", "iar.targets.arm"), Global.newObject("prefix", "-S", "suffix", ""), "wh");
        po.addFld("ar", (Proto)om.findStrict("iar.targets.arm.ITarget.Command", "iar.targets.arm"), Global.newObject("cmd", "iarchive", "opts", ""), "rh");
        po.addFld("arOpts", (Proto)om.findStrict("iar.targets.arm.ITarget.Options", "iar.targets.arm"), Global.newObject("prefix", "--silent", "suffix", ""), "wh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"), Global.newObject("prefix", "--silent", "suffix", "--map $(XDCCFGDIR)/$@.map  --redirect _Printf=_PrintfSmall --redirect _Scanf=_ScanfSmall "), "wh");
        po.addFld("vers", (Proto)om.findStrict("iar.targets.arm.ITarget.Command", "iar.targets.arm"), Global.newObject("cmd", "iccarm", "opts", "-v"), "rh");
        po.addFld("extensions", new Proto.Map((Proto)om.findStrict("iar.targets.arm.ITarget.Extension", "iar.targets.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{".asm", Global.newObject("suf", ".asm", "typ", "asm")}), Global.newArray(new Object[]{".c", Global.newObject("suf", ".c", "typ", "c")}), Global.newArray(new Object[]{".cpp", Global.newObject("suf", ".cpp", "typ", "cpp")}), Global.newArray(new Object[]{".cxx", Global.newObject("suf", ".cxx", "typ", "cpp")}), Global.newArray(new Object[]{".C", Global.newObject("suf", ".C", "typ", "cpp")}), Global.newArray(new Object[]{".cc", Global.newObject("suf", ".cc", "typ", "cpp")}), Global.newArray(new Object[]{".s", Global.newObject("suf", ".s", "typ", "asm")}), Global.newArray(new Object[]{".sv6M", Global.newObject("suf", ".sv6M", "typ", "asm")}), Global.newArray(new Object[]{".sv7M", Global.newObject("suf", ".sv7M", "typ", "asm")}), Global.newArray(new Object[]{".sv8M", Global.newObject("suf", ".sv8M", "typ", "asm")})}), "wh");
        po.addFld("includeOpts", $$T_Str, "", "wh");
        po.addFld("cmdPrefix", $$T_Str, "LC_ALL=C ", "wh");
        po.addFld("alignDirectiveSupported", $$T_Bool, true, "rh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 8L)), "rh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "--debug --dlib_config $(rootDir)/inc/c/DLib_Config_Normal.h"), "linkOpts", "--semihosting=iar_breakpoint")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "--debug -Ohs --dlib_config $(rootDir)/inc/c/DLib_Config_Normal.h"), "linkOpts", "--semihosting=iar_breakpoint")}), Global.newArray(new Object[]{"debug_full", Global.newObject("compileOpts", Global.newObject("copts", "--debug --dlib_config $(rootDir)/inc/c/DLib_Config_Full.h"), "linkOpts", "--semihosting=iar_breakpoint")}), Global.newArray(new Object[]{"release_full", Global.newObject("compileOpts", Global.newObject("copts", "--debug -Ohs --dlib_config $(rootDir)/inc/c/DLib_Config_Full.h"), "linkOpts", "--semihosting=iar_breakpoint")})}), "wh");
        po.addFld("binaryParser", $$T_Str, "xdc.targets.omf.Elf", "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("iar.targets.arm.ITarget$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("iar.targets.arm.ITarget$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("iar.targets.arm.ITarget$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "iar.targets.arm"), fxn);
    }

    void M0$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("iar.targets.arm.M0.Module", "iar.targets.arm");
        po.init("iar.targets.arm.M0.Module", om.findStrict("iar.targets.arm.ITarget.Module", "iar.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M0", "rh");
        po.addFld("suffix", $$T_Str, "rm0", "rh");
        po.addFld("isa", $$T_Str, "v6M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.simCM3", "wh");
        po.addFld("cc", (Proto)om.findStrict("iar.targets.arm.M0.Command", "iar.targets.arm"), Global.newObject("cmd", "iccarm", "opts", "--aeabi --cpu=Cortex-M0 --diag_suppress=Pa050,Go005 --endian=little -e --thumb"), "rh");
        po.addFld("asm", (Proto)om.findStrict("iar.targets.arm.M0.Command", "iar.targets.arm"), Global.newObject("cmd", "iasmarm", "opts", "--cpu Cortex-M0 --endian little"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("iar.targets.arm.M0.Command", "iar.targets.arm"), Global.newObject("cmd", "ilinkarm", "opts", "--cpu=Cortex-M0"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "iar.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "iar.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "iar.targets.arm"), $$UNDEF);
    }

    void M3$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("iar.targets.arm.M3.Module", "iar.targets.arm");
        po.init("iar.targets.arm.M3.Module", om.findStrict("iar.targets.arm.ITarget.Module", "iar.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M3", "rh");
        po.addFld("suffix", $$T_Str, "rm3", "rh");
        po.addFld("isa", $$T_Str, "v7M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.tiva:TM4C1294NCPDT:1", "wh");
        po.addFld("cc", (Proto)om.findStrict("iar.targets.arm.M3.Command", "iar.targets.arm"), Global.newObject("cmd", "iccarm", "opts", "--aeabi --cpu=Cortex-M3 --diag_suppress=Pa050,Go005 --endian=little -e --thumb"), "rh");
        po.addFld("asm", (Proto)om.findStrict("iar.targets.arm.M3.Command", "iar.targets.arm"), Global.newObject("cmd", "iasmarm", "opts", "--cpu Cortex-M3 --endian little"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("iar.targets.arm.M3.Command", "iar.targets.arm"), Global.newObject("cmd", "ilinkarm", "opts", "--cpu=Cortex-M3"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "iar.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "iar.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "iar.targets.arm"), $$UNDEF);
    }

    void M4$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("iar.targets.arm.M4.Module", "iar.targets.arm");
        po.init("iar.targets.arm.M4.Module", om.findStrict("iar.targets.arm.ITarget.Module", "iar.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M4", "rh");
        po.addFld("suffix", $$T_Str, "rm4", "rh");
        po.addFld("isa", $$T_Str, "v7M4", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.tiva:TM4C1294NCPDT:1", "wh");
        po.addFld("cc", (Proto)om.findStrict("iar.targets.arm.M4.Command", "iar.targets.arm"), Global.newObject("cmd", "iccarm", "opts", "--aeabi --cpu=Cortex-M4 --diag_suppress=Pa050,Go005 --endian=little -e --thumb"), "rh");
        po.addFld("asm", (Proto)om.findStrict("iar.targets.arm.M4.Command", "iar.targets.arm"), Global.newObject("cmd", "iasmarm", "opts", "--cpu Cortex-M4 --endian little"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("iar.targets.arm.M4.Command", "iar.targets.arm"), Global.newObject("cmd", "ilinkarm", "opts", "--cpu=Cortex-M4"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "iar.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "iar.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "iar.targets.arm"), $$UNDEF);
    }

    void M4F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("iar.targets.arm.M4F.Module", "iar.targets.arm");
        po.init("iar.targets.arm.M4F.Module", om.findStrict("iar.targets.arm.ITarget.Module", "iar.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M4F", "rh");
        po.addFld("suffix", $$T_Str, "rm4f", "rh");
        po.addFld("isa", $$T_Str, "v7M4", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.tiva:TM4C1294NCPDT:1", "wh");
        po.addFld("cc", (Proto)om.findStrict("iar.targets.arm.M4F.Command", "iar.targets.arm"), Global.newObject("cmd", "iccarm", "opts", "--aeabi --cpu=Cortex-M4F --diag_suppress=Pa050,Go005 --endian=little -e --fpu=VFPv4_sp --thumb"), "rh");
        po.addFld("asm", (Proto)om.findStrict("iar.targets.arm.M4F.Command", "iar.targets.arm"), Global.newObject("cmd", "iasmarm", "opts", "--cpu Cortex-M4F --endian little --fpu VFPv4_sp "), "rh");
        po.addFld("lnk", (Proto)om.findStrict("iar.targets.arm.M4F.Command", "iar.targets.arm"), Global.newObject("cmd", "ilinkarm", "opts", "--cpu=Cortex-M4F"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "iar.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "iar.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "iar.targets.arm"), $$UNDEF);
    }

    void M33$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("iar.targets.arm.M33.Module", "iar.targets.arm");
        po.init("iar.targets.arm.M33.Module", om.findStrict("iar.targets.arm.ITarget.Module", "iar.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M33", "rh");
        po.addFld("suffix", $$T_Str, "rm33", "rh");
        po.addFld("isa", $$T_Str, "v8M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexM:FVP_MPS2:1", "wh");
        po.addFld("cc", (Proto)om.findStrict("iar.targets.arm.M33.Command", "iar.targets.arm"), Global.newObject("cmd", "iccarm", "opts", "--aeabi --cpu=Cortex-M33 --diag_suppress=Pa050,Go005 --endian=little -e --thumb"), "rh");
        po.addFld("asm", (Proto)om.findStrict("iar.targets.arm.M33.Command", "iar.targets.arm"), Global.newObject("cmd", "iasmarm", "opts", "--cpu Cortex-M33 --endian little "), "rh");
        po.addFld("lnk", (Proto)om.findStrict("iar.targets.arm.M33.Command", "iar.targets.arm"), Global.newObject("cmd", "ilinkarm", "opts", "--cpu=Cortex-M33"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "iar.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "iar.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "iar.targets.arm"), $$UNDEF);
    }

    void M33F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("iar.targets.arm.M33F.Module", "iar.targets.arm");
        po.init("iar.targets.arm.M33F.Module", om.findStrict("iar.targets.arm.ITarget.Module", "iar.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M33F", "rh");
        po.addFld("suffix", $$T_Str, "rm33f", "rh");
        po.addFld("isa", $$T_Str, "v8M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexM:FVP_MPS2:2", "wh");
        po.addFld("cc", (Proto)om.findStrict("iar.targets.arm.M33F.Command", "iar.targets.arm"), Global.newObject("cmd", "iccarm", "opts", "--aeabi --cpu=Cortex-M33.fp --fpu=FPv5_sp --diag_suppress=Pa050,Go005 --endian=little -e --thumb"), "rh");
        po.addFld("asm", (Proto)om.findStrict("iar.targets.arm.M33F.Command", "iar.targets.arm"), Global.newObject("cmd", "iasmarm", "opts", "--cpu Cortex-M33.fp --fpu FPv5_sp --endian little "), "rh");
        po.addFld("lnk", (Proto)om.findStrict("iar.targets.arm.M33F.Command", "iar.targets.arm"), Global.newObject("cmd", "ilinkarm", "opts", "--cpu=Cortex-M33.fp"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "iar.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "iar.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "iar.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "iar.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "iar.targets.arm"), $$UNDEF);
    }

    void ITarget$$ROV()
    {
    }

    void M0$$ROV()
    {
    }

    void M3$$ROV()
    {
    }

    void M4$$ROV()
    {
    }

    void M4F$$ROV()
    {
    }

    void M33$$ROV()
    {
    }

    void M33F$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("iar.targets.arm.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "iar.targets.arm"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "iar/targets/arm/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "iar.targets.arm"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "iar.targets.arm"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "iar.targets.arm", Value.DEFAULT, false);
        pkgV.bind("$name", "iar.targets.arm");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "iar.targets.arm.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['iar.targets.arm'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('iar.targets.arm$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['iar.targets.arm$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['iar.targets.arm$$stat$root'];\n");
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

        vo = (Value.Obj)om.findStrict("iar.targets.arm.ITarget", "iar.targets.arm");
        po = (Proto.Obj)om.findStrict("iar.targets.arm.ITarget.Module", "iar.targets.arm");
        vo.init2(po, "iar.targets.arm.ITarget", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("iar.targets.arm.ITarget$$capsule", "iar.targets.arm"));
        vo.bind("$package", om.findStrict("iar.targets.arm", "iar.targets.arm"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "iar.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
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

    void M0$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.targets.arm.M0", "iar.targets.arm");
        po = (Proto.Obj)om.findStrict("iar.targets.arm.M0.Module", "iar.targets.arm");
        vo.init2(po, "iar.targets.arm.M0", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("iar.targets.arm", "iar.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "iar.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("iar.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.targets.arm.M0$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("M0", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("M0");
    }

    void M3$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.targets.arm.M3", "iar.targets.arm");
        po = (Proto.Obj)om.findStrict("iar.targets.arm.M3.Module", "iar.targets.arm");
        vo.init2(po, "iar.targets.arm.M3", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("iar.targets.arm", "iar.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "iar.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("iar.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.targets.arm.M3$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("M3", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("M3");
    }

    void M4$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.targets.arm.M4", "iar.targets.arm");
        po = (Proto.Obj)om.findStrict("iar.targets.arm.M4.Module", "iar.targets.arm");
        vo.init2(po, "iar.targets.arm.M4", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("iar.targets.arm", "iar.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "iar.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("iar.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.targets.arm.M4$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("M4", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("M4");
    }

    void M4F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.targets.arm.M4F", "iar.targets.arm");
        po = (Proto.Obj)om.findStrict("iar.targets.arm.M4F.Module", "iar.targets.arm");
        vo.init2(po, "iar.targets.arm.M4F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("iar.targets.arm", "iar.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "iar.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("iar.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.targets.arm.M4F$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("M4F", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("M4F");
    }

    void M33$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.targets.arm.M33", "iar.targets.arm");
        po = (Proto.Obj)om.findStrict("iar.targets.arm.M33.Module", "iar.targets.arm");
        vo.init2(po, "iar.targets.arm.M33", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("iar.targets.arm", "iar.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "iar.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("iar.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.targets.arm.M33$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("M33", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("M33");
    }

    void M33F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.targets.arm.M33F", "iar.targets.arm");
        po = (Proto.Obj)om.findStrict("iar.targets.arm.M33F.Module", "iar.targets.arm");
        vo.init2(po, "iar.targets.arm.M33F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("iar.targets.arm", "iar.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "iar.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "iar.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "iar.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "iar.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "iar.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "iar.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "iar.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "iar.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "iar.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "iar.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "iar.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "iar.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "iar.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "iar.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("iar.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.targets.arm.M33F$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("M33F", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("M33F");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.targets.arm.M0", "iar.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.targets.arm.M3", "iar.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.targets.arm.M4", "iar.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.targets.arm.M4F", "iar.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.targets.arm.M33", "iar.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.targets.arm.M33F", "iar.targets.arm"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("iar.targets.arm.ITarget")).bless();
        ((Value.Obj)om.getv("iar.targets.arm.M0")).bless();
        ((Value.Obj)om.getv("iar.targets.arm.M3")).bless();
        ((Value.Obj)om.getv("iar.targets.arm.M4")).bless();
        ((Value.Obj)om.getv("iar.targets.arm.M4F")).bless();
        ((Value.Obj)om.getv("iar.targets.arm.M33")).bless();
        ((Value.Obj)om.getv("iar.targets.arm.M33F")).bless();
        ((Value.Arr)om.findStrict("$packages", "iar.targets.arm")).add(pkgV);
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
        M0$$OBJECTS();
        M3$$OBJECTS();
        M4$$OBJECTS();
        M4F$$OBJECTS();
        M33$$OBJECTS();
        M33F$$OBJECTS();
        ITarget$$CONSTS();
        M0$$CONSTS();
        M3$$CONSTS();
        M4$$CONSTS();
        M4F$$CONSTS();
        M33$$CONSTS();
        M33F$$CONSTS();
        ITarget$$CREATES();
        M0$$CREATES();
        M3$$CREATES();
        M4$$CREATES();
        M4F$$CREATES();
        M33$$CREATES();
        M33F$$CREATES();
        ITarget$$FUNCTIONS();
        M0$$FUNCTIONS();
        M3$$FUNCTIONS();
        M4$$FUNCTIONS();
        M4F$$FUNCTIONS();
        M33$$FUNCTIONS();
        M33F$$FUNCTIONS();
        ITarget$$SIZES();
        M0$$SIZES();
        M3$$SIZES();
        M4$$SIZES();
        M4F$$SIZES();
        M33$$SIZES();
        M33F$$SIZES();
        ITarget$$TYPES();
        M0$$TYPES();
        M3$$TYPES();
        M4$$TYPES();
        M4F$$TYPES();
        M33$$TYPES();
        M33F$$TYPES();
        if (isROV) {
            ITarget$$ROV();
            M0$$ROV();
            M3$$ROV();
            M4$$ROV();
            M4F$$ROV();
            M33$$ROV();
            M33F$$ROV();
        }//isROV
        $$SINGLETONS();
        ITarget$$SINGLETONS();
        M0$$SINGLETONS();
        M3$$SINGLETONS();
        M4$$SINGLETONS();
        M4F$$SINGLETONS();
        M33$$SINGLETONS();
        M33F$$SINGLETONS();
        $$INITIALIZATION();
    }
}
