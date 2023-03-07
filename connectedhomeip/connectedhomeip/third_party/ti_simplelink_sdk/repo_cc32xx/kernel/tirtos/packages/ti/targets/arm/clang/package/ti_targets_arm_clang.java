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

public class ti_targets_arm_clang
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
        Global.callFxn("loadPackage", xdcO, "ti.targets");
        Global.callFxn("loadPackage", xdcO, "ti.targets.arm.elf");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.targets.arm.clang.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.targets.arm.clang", new Value.Obj("ti.targets.arm.clang", pkgP));
    }

    void ITarget$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.clang.ITarget.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.clang.ITarget", new Value.Obj("ti.targets.arm.clang.ITarget", po));
        pkgV.bind("ITarget", vo);
        // decls 
        om.bind("ti.targets.arm.clang.ITarget.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.ITarget.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
    }

    void M0$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.clang.M0.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.clang.M0", new Value.Obj("ti.targets.arm.clang.M0", po));
        pkgV.bind("M0", vo);
        // decls 
        om.bind("ti.targets.arm.clang.M0.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M0.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
    }

    void M3$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.clang.M3.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.clang.M3", new Value.Obj("ti.targets.arm.clang.M3", po));
        pkgV.bind("M3", vo);
        // decls 
        om.bind("ti.targets.arm.clang.M3.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M3.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
    }

    void M4$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.clang.M4.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.clang.M4", new Value.Obj("ti.targets.arm.clang.M4", po));
        pkgV.bind("M4", vo);
        // decls 
        om.bind("ti.targets.arm.clang.M4.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
    }

    void M4F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.clang.M4F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.clang.M4F", new Value.Obj("ti.targets.arm.clang.M4F", po));
        pkgV.bind("M4F", vo);
        // decls 
        om.bind("ti.targets.arm.clang.M4F.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M4F.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
    }

    void M33$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.clang.M33.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.clang.M33", new Value.Obj("ti.targets.arm.clang.M33", po));
        pkgV.bind("M33", vo);
        // decls 
        om.bind("ti.targets.arm.clang.M33.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
    }

    void M33F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.clang.M33F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.clang.M33F", new Value.Obj("ti.targets.arm.clang.M33F", po));
        pkgV.bind("M33F", vo);
        // decls 
        om.bind("ti.targets.arm.clang.M33F.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.M33F.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
    }

    void R5F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.clang.R5F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.clang.R5F", new Value.Obj("ti.targets.arm.clang.R5F", po));
        pkgV.bind("R5F", vo);
        // decls 
        om.bind("ti.targets.arm.clang.R5F.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        om.bind("ti.targets.arm.clang.R5F.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
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

    void R5F$$CONSTS()
    {
        // module R5F
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

    void R5F$$CREATES()
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

    void R5F$$FUNCTIONS()
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

    void R5F$$SIZES()
    {
    }

    void ITarget$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/targets/arm/clang/ITarget.xs");
        om.bind("ti.targets.arm.clang.ITarget$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.ITarget.Module", "ti.targets.arm.clang");
        po.init("ti.targets.arm.clang.ITarget.Module", om.findStrict("ti.targets.arm.elf.IArm.Module", "ti.targets.arm.clang"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("rts", $$T_Str, "ti.targets.arm.rtsarm", "rh");
        po.addFld("stdInclude", $$T_Str, "ti/targets/arm/clang/std.h", "wh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"), Global.newObject("endian", "little", "codeModel", "thumb2", "shortEnums", true), "rh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmar -c", "opts", "-q"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"), Global.newObject("prefix", "", "suffix", ""), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"), Global.newObject("prefix", "$(ccOpts.prefix)", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("asmOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"), Global.newObject("prefix", "", "suffix", ""), "wh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"), Global.newObject("prefix", "-Wl,-q -Wl,-u,_c_int00", "suffix", "-Wl,-w -Wl,-c -Wl,-m,$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("vers", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang", "opts", "--version"), "rh");
        po.addFld("includeOpts", $$T_Str, "", "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"))}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-g -Oz"))}), Global.newArray(new Object[]{"coverage", Global.newObject("compileOpts", Global.newObject("copts", "-g -fprofile-instr-generate -fcoverage-mapping"), "linkOpts", "-fprofile-instr-generate -fcoverage-mapping")})}), "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.targets.arm.clang.ITarget$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.targets.arm.clang.ITarget$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.targets.arm.clang.ITarget$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.clang"), fxn);
    }

    void M0$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M0.Module", "ti.targets.arm.clang");
        po.init("ti.targets.arm.clang.M0.Module", om.findStrict("ti.targets.arm.clang.ITarget.Module", "ti.targets.arm.clang"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M0", "rh");
        po.addFld("suffix", $$T_Str, "m0", "rh");
        po.addFld("isa", $$T_Str, "v6M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.CM3", "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c", "opts", "-mcpu=cortex-m0 -mfloat-abi=soft -mfpu=none"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m0 -mfloat-abi=soft -mfpu=none"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang", "opts", "-mcpu=cortex-m0 -mfloat-abi=soft -mfpu=none"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.clang"), $$UNDEF);
    }

    void M3$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M3.Module", "ti.targets.arm.clang");
        po.init("ti.targets.arm.clang.M3.Module", om.findStrict("ti.targets.arm.clang.ITarget.Module", "ti.targets.arm.clang"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M3", "rh");
        po.addFld("suffix", $$T_Str, "m3", "rh");
        po.addFld("isa", $$T_Str, "v7M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.tiva:TM4C1294NCPDT:1", "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c", "opts", "-mcpu=cortex-m3 -mfloat-abi=soft -mfpu=none"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m3 -mfloat-abi=soft -mfpu=none"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang", "opts", "-mcpu=cortex-m3 -mfloat-abi=soft -mfpu=none"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.clang"), $$UNDEF);
    }

    void M4$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M4.Module", "ti.targets.arm.clang");
        po.init("ti.targets.arm.clang.M4.Module", om.findStrict("ti.targets.arm.clang.ITarget.Module", "ti.targets.arm.clang"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M4", "rh");
        po.addFld("suffix", $$T_Str, "m4", "rh");
        po.addFld("isa", $$T_Str, "v7M4", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.tiva:TM4C1294NCPDT:1", "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c", "opts", "-mcpu=cortex-m4 -mfloat-abi=soft -mfpu=none"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m4 -mfloat-abi=soft -mfpu=none"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang", "opts", "-mcpu=cortex-m4 -mfloat-abi=soft -mfpu=none"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.clang"), $$UNDEF);
    }

    void M4F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M4F.Module", "ti.targets.arm.clang");
        po.init("ti.targets.arm.clang.M4F.Module", om.findStrict("ti.targets.arm.clang.ITarget.Module", "ti.targets.arm.clang"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M4F", "rh");
        po.addFld("suffix", $$T_Str, "m4f", "rh");
        po.addFld("isa", $$T_Str, "v7M4", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexM:MTL1_VSOC:1", "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c", "opts", "-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang", "opts", "-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.clang"), $$UNDEF);
    }

    void M33$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M33.Module", "ti.targets.arm.clang");
        po.init("ti.targets.arm.clang.M33.Module", om.findStrict("ti.targets.arm.clang.ITarget.Module", "ti.targets.arm.clang"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M33", "rh");
        po.addFld("suffix", $$T_Str, "m33", "rh");
        po.addFld("isa", $$T_Str, "v8M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexM:MTL1_VSOC:1", "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c", "opts", "-mcpu=cortex-m33 -msoft-float -mfpu=none"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m33 -msoft-float -mfpu=none"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang", "opts", "-mcpu=cortex-m33 -msoft-float -mfpu=none"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.clang"), $$UNDEF);
    }

    void M33F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M33F.Module", "ti.targets.arm.clang");
        po.init("ti.targets.arm.clang.M33F.Module", om.findStrict("ti.targets.arm.clang.ITarget.Module", "ti.targets.arm.clang"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M33F", "rh");
        po.addFld("suffix", $$T_Str, "m33f", "rh");
        po.addFld("isa", $$T_Str, "v8M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexM:MTL1_VSOC:1", "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c", "opts", "-mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang", "opts", "-mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.clang"), $$UNDEF);
    }

    void R5F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.R5F.Module", "ti.targets.arm.clang");
        po.init("ti.targets.arm.clang.R5F.Module", om.findStrict("ti.targets.arm.clang.ITarget.Module", "ti.targets.arm.clang"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "R5F", "rh");
        po.addFld("suffix", $$T_Str, "r5f", "rh");
        po.addFld("isa", $$T_Str, "v7R", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexR:J721E:MAIN:1", "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c", "opts", "-mcpu=cortex-r5 -mfloat-abi=hard -mfpu=vfpv3-d16"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang -c -x assembler-with-cpp", "opts", "-mcpu=cortex-r5 -mfloat-abi=hard -mfpu=vfpv3-d16"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"), Global.newObject("cmd", "tiarmclang", "opts", "-mcpu=cortex-r5 -mfloat-abi=hard -mfpu=vfpv3-d16"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.clang"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.clang"), $$UNDEF);
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

    void R5F$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.targets.arm.clang.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.targets.arm.clang"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/targets/arm/clang/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.targets.arm.clang"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.targets.arm.clang"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.targets.arm.clang", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.targets.arm.clang");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.targets.arm.clang.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.bld", Global.newArray()));
        imports.add(Global.newArray("ti.targets", Global.newArray()));
        imports.add(Global.newArray("ti.targets.arm.elf", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.targets.arm.clang'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.targets.arm.clang$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.targets.arm.clang$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.targets.arm.clang$$stat$root'];\n");
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

        vo = (Value.Obj)om.findStrict("ti.targets.arm.clang.ITarget", "ti.targets.arm.clang");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.ITarget.Module", "ti.targets.arm.clang");
        vo.init2(po, "ti.targets.arm.clang.ITarget", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("ti.targets.arm.clang.ITarget$$capsule", "ti.targets.arm.clang"));
        vo.bind("$package", om.findStrict("ti.targets.arm.clang", "ti.targets.arm.clang"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.clang"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
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

        vo = (Value.Obj)om.findStrict("ti.targets.arm.clang.M0", "ti.targets.arm.clang");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M0.Module", "ti.targets.arm.clang");
        vo.init2(po, "ti.targets.arm.clang.M0", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.clang", "ti.targets.arm.clang"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.clang"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.clang");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.clang")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.clang.M0$$instance$static$init", null) ? 1 : 0);
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

        vo = (Value.Obj)om.findStrict("ti.targets.arm.clang.M3", "ti.targets.arm.clang");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M3.Module", "ti.targets.arm.clang");
        vo.init2(po, "ti.targets.arm.clang.M3", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.clang", "ti.targets.arm.clang"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.clang"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.clang");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.clang")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.clang.M3$$instance$static$init", null) ? 1 : 0);
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

        vo = (Value.Obj)om.findStrict("ti.targets.arm.clang.M4", "ti.targets.arm.clang");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M4.Module", "ti.targets.arm.clang");
        vo.init2(po, "ti.targets.arm.clang.M4", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.clang", "ti.targets.arm.clang"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.clang"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.clang");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.clang")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.clang.M4$$instance$static$init", null) ? 1 : 0);
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

        vo = (Value.Obj)om.findStrict("ti.targets.arm.clang.M4F", "ti.targets.arm.clang");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M4F.Module", "ti.targets.arm.clang");
        vo.init2(po, "ti.targets.arm.clang.M4F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.clang", "ti.targets.arm.clang"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.clang"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.clang");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.clang")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.clang.M4F$$instance$static$init", null) ? 1 : 0);
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

        vo = (Value.Obj)om.findStrict("ti.targets.arm.clang.M33", "ti.targets.arm.clang");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M33.Module", "ti.targets.arm.clang");
        vo.init2(po, "ti.targets.arm.clang.M33", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.clang", "ti.targets.arm.clang"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.clang"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.clang");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.clang")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.clang.M33$$instance$static$init", null) ? 1 : 0);
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

        vo = (Value.Obj)om.findStrict("ti.targets.arm.clang.M33F", "ti.targets.arm.clang");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.M33F.Module", "ti.targets.arm.clang");
        vo.init2(po, "ti.targets.arm.clang.M33F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.clang", "ti.targets.arm.clang"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.clang"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.clang");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.clang")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.clang.M33F$$instance$static$init", null) ? 1 : 0);
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

    void R5F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.clang.R5F", "ti.targets.arm.clang");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.clang.R5F.Module", "ti.targets.arm.clang");
        vo.init2(po, "ti.targets.arm.clang.R5F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.clang", "ti.targets.arm.clang"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.clang"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.clang"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.clang"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.clang"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.clang"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.clang"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.clang"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.clang"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.clang"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.clang"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.clang"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.clang"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.clang"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.clang"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.clang");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.clang")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.clang.R5F$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("R5F", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("R5F");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.clang.M0", "ti.targets.arm.clang"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.clang.M3", "ti.targets.arm.clang"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.clang.M4", "ti.targets.arm.clang"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.clang.M4F", "ti.targets.arm.clang"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.clang.M33", "ti.targets.arm.clang"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.clang.M33F", "ti.targets.arm.clang"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.clang.R5F", "ti.targets.arm.clang"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.targets.arm.clang.ITarget")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.clang.M0")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.clang.M3")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.clang.M4")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.clang.M4F")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.clang.M33")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.clang.M33F")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.clang.R5F")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.targets.arm.clang")).add(pkgV);
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
        R5F$$OBJECTS();
        ITarget$$CONSTS();
        M0$$CONSTS();
        M3$$CONSTS();
        M4$$CONSTS();
        M4F$$CONSTS();
        M33$$CONSTS();
        M33F$$CONSTS();
        R5F$$CONSTS();
        ITarget$$CREATES();
        M0$$CREATES();
        M3$$CREATES();
        M4$$CREATES();
        M4F$$CREATES();
        M33$$CREATES();
        M33F$$CREATES();
        R5F$$CREATES();
        ITarget$$FUNCTIONS();
        M0$$FUNCTIONS();
        M3$$FUNCTIONS();
        M4$$FUNCTIONS();
        M4F$$FUNCTIONS();
        M33$$FUNCTIONS();
        M33F$$FUNCTIONS();
        R5F$$FUNCTIONS();
        ITarget$$SIZES();
        M0$$SIZES();
        M3$$SIZES();
        M4$$SIZES();
        M4F$$SIZES();
        M33$$SIZES();
        M33F$$SIZES();
        R5F$$SIZES();
        ITarget$$TYPES();
        M0$$TYPES();
        M3$$TYPES();
        M4$$TYPES();
        M4F$$TYPES();
        M33$$TYPES();
        M33F$$TYPES();
        R5F$$TYPES();
        if (isROV) {
            ITarget$$ROV();
            M0$$ROV();
            M3$$ROV();
            M4$$ROV();
            M4F$$ROV();
            M33$$ROV();
            M33F$$ROV();
            R5F$$ROV();
        }//isROV
        $$SINGLETONS();
        ITarget$$SINGLETONS();
        M0$$SINGLETONS();
        M3$$SINGLETONS();
        M4$$SINGLETONS();
        M4F$$SINGLETONS();
        M33$$SINGLETONS();
        M33F$$SINGLETONS();
        R5F$$SINGLETONS();
        $$INITIALIZATION();
    }
}
