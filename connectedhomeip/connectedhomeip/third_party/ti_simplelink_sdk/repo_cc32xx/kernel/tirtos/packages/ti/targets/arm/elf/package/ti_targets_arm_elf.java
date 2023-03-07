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

public class ti_targets_arm_elf
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
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.targets.arm.elf.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.targets.arm.elf", new Value.Obj("ti.targets.arm.elf", pkgP));
    }

    void IArm$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.IArm.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.IArm", new Value.Obj("ti.targets.arm.elf.IArm", po));
        pkgV.bind("IArm", vo);
        // decls 
        om.bind("ti.targets.arm.elf.IArm.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IArm.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void Arm9$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.Arm9.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.Arm9", new Value.Obj("ti.targets.arm.elf.Arm9", po));
        pkgV.bind("Arm9", vo);
        // decls 
        om.bind("ti.targets.arm.elf.Arm9.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.Arm9.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void IM0$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.IM0.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.IM0", new Value.Obj("ti.targets.arm.elf.IM0", po));
        pkgV.bind("IM0", vo);
        // decls 
        om.bind("ti.targets.arm.elf.IM0.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM0.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void M0$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.M0.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.M0", new Value.Obj("ti.targets.arm.elf.M0", po));
        pkgV.bind("M0", vo);
        // decls 
        om.bind("ti.targets.arm.elf.M0.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M0.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void IM3$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.IM3.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.IM3", new Value.Obj("ti.targets.arm.elf.IM3", po));
        pkgV.bind("IM3", vo);
        // decls 
        om.bind("ti.targets.arm.elf.IM3.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM3.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void M3$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.M3.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.M3", new Value.Obj("ti.targets.arm.elf.M3", po));
        pkgV.bind("M3", vo);
        // decls 
        om.bind("ti.targets.arm.elf.M3.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M3.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void IM4$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.IM4.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.IM4", new Value.Obj("ti.targets.arm.elf.IM4", po));
        pkgV.bind("IM4", vo);
        // decls 
        om.bind("ti.targets.arm.elf.IM4.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IM4.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void M4$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.M4.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.M4", new Value.Obj("ti.targets.arm.elf.M4", po));
        pkgV.bind("M4", vo);
        // decls 
        om.bind("ti.targets.arm.elf.M4.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void M4F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.M4F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.M4F", new Value.Obj("ti.targets.arm.elf.M4F", po));
        pkgV.bind("M4F", vo);
        // decls 
        om.bind("ti.targets.arm.elf.M4F.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.M4F.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void IR4$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.IR4.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.IR4", new Value.Obj("ti.targets.arm.elf.IR4", po));
        pkgV.bind("IR4", vo);
        // decls 
        om.bind("ti.targets.arm.elf.IR4.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR4.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void R4F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.R4F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.R4F", new Value.Obj("ti.targets.arm.elf.R4F", po));
        pkgV.bind("R4F", vo);
        // decls 
        om.bind("ti.targets.arm.elf.R4F.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4F.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void R4Ft$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.R4Ft.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.R4Ft", new Value.Obj("ti.targets.arm.elf.R4Ft", po));
        pkgV.bind("R4Ft", vo);
        // decls 
        om.bind("ti.targets.arm.elf.R4Ft.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R4Ft.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void IR5$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.IR5.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.IR5", new Value.Obj("ti.targets.arm.elf.IR5", po));
        pkgV.bind("IR5", vo);
        // decls 
        om.bind("ti.targets.arm.elf.IR5.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.IR5.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void R5F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.R5F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.R5F", new Value.Obj("ti.targets.arm.elf.R5F", po));
        pkgV.bind("R5F", vo);
        // decls 
        om.bind("ti.targets.arm.elf.R5F.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5F.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void R5Ft$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.targets.arm.elf.R5Ft.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.targets.arm.elf.R5Ft", new Value.Obj("ti.targets.arm.elf.R5Ft", po));
        pkgV.bind("R5Ft", vo);
        // decls 
        om.bind("ti.targets.arm.elf.R5Ft.Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        om.bind("ti.targets.arm.elf.R5Ft.Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
    }

    void IArm$$CONSTS()
    {
        // interface IArm
    }

    void Arm9$$CONSTS()
    {
        // module Arm9
    }

    void IM0$$CONSTS()
    {
        // interface IM0
    }

    void M0$$CONSTS()
    {
        // module M0
    }

    void IM3$$CONSTS()
    {
        // interface IM3
    }

    void M3$$CONSTS()
    {
        // module M3
    }

    void IM4$$CONSTS()
    {
        // interface IM4
    }

    void M4$$CONSTS()
    {
        // module M4
    }

    void M4F$$CONSTS()
    {
        // module M4F
    }

    void IR4$$CONSTS()
    {
        // interface IR4
    }

    void R4F$$CONSTS()
    {
        // module R4F
    }

    void R4Ft$$CONSTS()
    {
        // module R4Ft
    }

    void IR5$$CONSTS()
    {
        // interface IR5
    }

    void R5F$$CONSTS()
    {
        // module R5F
    }

    void R5Ft$$CONSTS()
    {
        // module R5Ft
    }

    void IArm$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Arm9$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IM0$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void M0$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IM3$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void M3$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IM4$$CREATES()
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

    void IR4$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void R4F$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void R4Ft$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IR5$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void R5F$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void R5Ft$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IArm$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Arm9$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IM0$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void M0$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IM3$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void M3$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IM4$$FUNCTIONS()
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

    void IR4$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void R4F$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void R4Ft$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IR5$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void R5F$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void R5Ft$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IArm$$SIZES()
    {
    }

    void Arm9$$SIZES()
    {
    }

    void IM0$$SIZES()
    {
    }

    void M0$$SIZES()
    {
    }

    void IM3$$SIZES()
    {
    }

    void M3$$SIZES()
    {
    }

    void IM4$$SIZES()
    {
    }

    void M4$$SIZES()
    {
    }

    void M4F$$SIZES()
    {
    }

    void IR4$$SIZES()
    {
    }

    void R4F$$SIZES()
    {
    }

    void R4Ft$$SIZES()
    {
    }

    void IR5$$SIZES()
    {
    }

    void R5F$$SIZES()
    {
    }

    void R5Ft$$SIZES()
    {
    }

    void IArm$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/targets/arm/elf/IArm.xs");
        om.bind("ti.targets.arm.elf.IArm$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IArm.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.IArm.Module", om.findStrict("ti.targets.ITarget.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("stdInclude", $$T_Str, "ti/targets/arm/elf/std.h", "wh");
        po.addFld("platform", $$T_Str, "ti.platforms.sim470xx", "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/include ", "wh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"), Global.newObject("shortEnums", true), "rh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armar", "opts", "rq"), "rh");
        po.addFld("vers", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl", "opts", "--compiler_revision"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl", "opts", "-z"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"), Global.newObject("prefix", "-qq -pdsw225", "suffix", ""), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"), Global.newObject("prefix", "$(ccOpts.prefix) -ms --fp_mode=strict", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("asmOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"), Global.newObject("prefix", "-qq", "suffix", ""), "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "--symdebug:dwarf", "defs", "-D_DEBUG_=1"))}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-O2"))}), Global.newArray(new Object[]{"profile", Global.newObject("compileOpts", Global.newObject("copts", "--symdebug:dwarf"))}), Global.newArray(new Object[]{"coverage", Global.newObject("compileOpts", Global.newObject("copts", "--symdebug:dwarf"))})}), "wh");
        po.addFld("sectMap", new Proto.Map($$T_Str), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", "code"}), Global.newArray(new Object[]{".stack", "stack"}), Global.newArray(new Object[]{".bss", "data"}), Global.newArray(new Object[]{".binit", "code"}), Global.newArray(new Object[]{".cinit", "code"}), Global.newArray(new Object[]{".init_array", "code"}), Global.newArray(new Object[]{".const", "code"}), Global.newArray(new Object[]{".data", "data"}), Global.newArray(new Object[]{".rodata", "data"}), Global.newArray(new Object[]{".neardata", "data"}), Global.newArray(new Object[]{".fardata", "data"}), Global.newArray(new Object[]{".switch", "data"}), Global.newArray(new Object[]{".sysmem", "data"}), Global.newArray(new Object[]{".far", "data"}), Global.newArray(new Object[]{".args", "data"}), Global.newArray(new Object[]{".cio", "data"}), Global.newArray(new Object[]{".ARM.exidx", "data"}), Global.newArray(new Object[]{".ARM.extab", "data"})}), "rh");
        po.addFld("splitMap", new Proto.Map($$T_Bool), Global.newArray(new Object[]{Global.newArray(new Object[]{".text", true}), Global.newArray(new Object[]{".const", true}), Global.newArray(new Object[]{".data", true}), Global.newArray(new Object[]{".fardata", true}), Global.newArray(new Object[]{".switch", true}), Global.newArray(new Object[]{".far", true}), Global.newArray(new Object[]{".args", true}), Global.newArray(new Object[]{".cio", true}), Global.newArray(new Object[]{".ARM.extab", true})}), "rh");
        po.addFld("bitsPerChar", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        po.addFld("binaryParser", $$T_Str, "xdc.targets.omf.Elf", "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.targets.arm.elf.IArm$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.targets.arm.elf.IArm$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.targets.arm.elf.IArm$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.elf"), fxn);
    }

    void Arm9$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.Arm9.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.Arm9.Module", om.findStrict("ti.targets.arm.elf.IArm.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "Arm9", "rh");
        po.addFld("suffix", $$T_Str, "e9", "rh");
        po.addFld("isa", $$T_Str, "v5T", "rh");
        po.addFld("rts", $$T_Str, "ti.targets.arm.rtsarm", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"), Global.newObject("endian", "little", "shortEnums", true), "rh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"7", "470", "9t"}), "wh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armar", "opts", "rq"), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "-me -mv5e --abi=eabi"), "rh");
        po.addFld("vers", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl", "opts", "--compiler_revision"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "-me -mv5e --abi=eabi"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl", "opts", "--silicon_version=5e -z --strict_compatibility=on"), "rh");
        po.addFld("asmOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"), Global.newObject("prefix", "-qq", "suffix", ""), "wh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"), Global.newObject("prefix", "-qq -pdsw225", "suffix", ""), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"), Global.newObject("prefix", "$(ccOpts.prefix) -ms", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/include ", "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 4L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 4L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 4L), "t_LLong", Global.newObject("size", 8L, "align", 4L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.elf"), $$UNDEF);
    }

    void IM0$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IM0.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.IM0.Module", om.findStrict("ti.targets.arm.elf.IArm.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("isa", $$T_Str, "v6M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.simCM3", "wh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"), Global.newObject("endian", "little", "codeModel", "thumb2", "shortEnums", true), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl", "opts", "--silicon_version=6M0 -z --strict_compatibility=on"), "rh");
    }

    void M0$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.M0.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.M0.Module", om.findStrict("ti.targets.arm.elf.IM0.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M0", "rh");
        po.addFld("suffix", $$T_Str, "em0", "rh");
        po.addFld("rts", $$T_Str, "ti.targets.arm.rtsarm", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--endian=little -mv6M0 --abi=eabi"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--endian=little -mv6M0 --abi=eabi"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.elf"), $$UNDEF);
    }

    void IM3$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IM3.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.IM3.Module", om.findStrict("ti.targets.arm.elf.IArm.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("isa", $$T_Str, "v7M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.simCM3", "wh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"), Global.newObject("endian", "little", "codeModel", "thumb2", "shortEnums", true), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl", "opts", "--silicon_version=7M3 -z --strict_compatibility=on"), "rh");
    }

    void M3$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.M3.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.M3.Module", om.findStrict("ti.targets.arm.elf.IM3.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M3", "rh");
        po.addFld("suffix", $$T_Str, "em3", "rh");
        po.addFld("rts", $$T_Str, "ti.targets.arm.rtsarm", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--endian=little -mv7M3 --abi=eabi"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--endian=little -mv7M3 --abi=eabi"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.elf"), $$UNDEF);
    }

    void IM4$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IM4.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.IM4.Module", om.findStrict("ti.targets.arm.elf.IArm.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("isa", $$T_Str, "v7M4", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.tiva:TM4C1294NCPDT:1", "wh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"), Global.newObject("endian", "little", "codeModel", "thumb2", "shortEnums", true), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl", "opts", "--silicon_version=7M4 -z --strict_compatibility=on"), "rh");
    }

    void M4$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.M4.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.M4.Module", om.findStrict("ti.targets.arm.elf.IM4.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M4", "rh");
        po.addFld("suffix", $$T_Str, "em4", "rh");
        po.addFld("rts", $$T_Str, "ti.targets.arm.rtsarm", "rh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"em3"}), "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--endian=little -mv7M4 --float_support=vfplib --abi=eabi"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--endian=little -mv7M4 --float_support=vfplib --abi=eabi"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.elf"), $$UNDEF);
    }

    void M4F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.M4F.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.M4F.Module", om.findStrict("ti.targets.arm.elf.IM4.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M4F", "rh");
        po.addFld("suffix", $$T_Str, "em4f", "rh");
        po.addFld("rts", $$T_Str, "ti.targets.arm.rtsarm", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--endian=little -mv7M4 --abi=eabi --float_support=fpv4spd16"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--endian=little -mv7M4 --abi=eabi --float_support=fpv4spd16"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.elf"), $$UNDEF);
    }

    void IR4$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IR4.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.IR4.Module", om.findStrict("ti.targets.arm.elf.IArm.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("isa", $$T_Str, "v7R", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexR:AWR14XX:1", "wh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armlnk", "opts", "--silicon_version=7R4 --strict_compatibility=on"), "rh");
    }

    void R4F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.R4F.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.R4F.Module", om.findStrict("ti.targets.arm.elf.IR4.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "R4F", "rh");
        po.addFld("suffix", $$T_Str, "er4f", "rh");
        po.addFld("rts", $$T_Str, "ti.targets.arm.rtsarm", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"), Global.newObject("endian", "little", "shortEnums", true), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--float_support=vfpv3d16 --endian=little -mv7R4 --abi=eabi"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--float_support=vfpv3d16 --endian=little -mv7R4 --abi=eabi"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.elf"), $$UNDEF);
    }

    void R4Ft$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.R4Ft.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.R4Ft.Module", om.findStrict("ti.targets.arm.elf.IR4.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "R4Ft", "rh");
        po.addFld("suffix", $$T_Str, "er4ft", "rh");
        po.addFld("rts", $$T_Str, "ti.targets.arm.rtsarm", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"), Global.newObject("endian", "little", "codeModel", "thumb2", "shortEnums", true), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--code_state=16 --float_support=vfpv3d16 --endian=little -mv7R4 --abi=eabi"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--code_state=16 --float_support=vfpv3d16 --endian=little -mv7R4 --abi=eabi"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.elf"), $$UNDEF);
    }

    void IR5$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IR5.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.IR5.Module", om.findStrict("ti.targets.arm.elf.IArm.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("isa", $$T_Str, "v7R", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexR:RM57L8XX:1", "wh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl", "opts", "--abi=eabi -mv7R5 -z --strict_compatibility=on"), "rh");
    }

    void R5F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.R5F.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.R5F.Module", om.findStrict("ti.targets.arm.elf.IR5.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "R5F", "rh");
        po.addFld("suffix", $$T_Str, "er5f", "rh");
        po.addFld("rts", $$T_Str, "ti.targets.arm.rtsarm", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"), Global.newObject("endian", "little", "shortEnums", true), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--float_support=vfpv3d16 --endian=little -mv7R5 --abi=eabi"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--float_support=vfpv3d16 --endian=little -mv7R5 --abi=eabi"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.elf"), $$UNDEF);
    }

    void R5Ft$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.R5Ft.Module", "ti.targets.arm.elf");
        po.init("ti.targets.arm.elf.R5Ft.Module", om.findStrict("ti.targets.arm.elf.IR5.Module", "ti.targets.arm.elf"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "R5Ft", "rh");
        po.addFld("suffix", $$T_Str, "er5ft", "rh");
        po.addFld("rts", $$T_Str, "ti.targets.arm.rtsarm", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"), Global.newObject("endian", "little", "codeModel", "thumb2", "shortEnums", true), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--code_state=16 --float_support=vfpv3d16 --endian=little -mv7R5 --abi=eabi"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"), Global.newObject("cmd", "armcl -c", "opts", "--code_state=16 --float_support=vfpv3d16 --endian=little -mv7R5 --abi=eabi"), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "ti.targets.arm.elf"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("ti.targets.ITarget$$asmName", "ti.targets.arm.elf"), $$UNDEF);
    }

    void IArm$$ROV()
    {
    }

    void Arm9$$ROV()
    {
    }

    void IM0$$ROV()
    {
    }

    void M0$$ROV()
    {
    }

    void IM3$$ROV()
    {
    }

    void M3$$ROV()
    {
    }

    void IM4$$ROV()
    {
    }

    void M4$$ROV()
    {
    }

    void M4F$$ROV()
    {
    }

    void IR4$$ROV()
    {
    }

    void R4F$$ROV()
    {
    }

    void R4Ft$$ROV()
    {
    }

    void IR5$$ROV()
    {
    }

    void R5F$$ROV()
    {
    }

    void R5Ft$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.targets.arm.elf.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.targets.arm.elf"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/targets/arm/elf/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.targets.arm.elf"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.targets.arm.elf"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.targets.arm.elf", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.targets.arm.elf");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.targets.arm.elf.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.bld", Global.newArray()));
        imports.add(Global.newArray("ti.targets", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.targets.arm.elf'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.targets.arm.elf$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.targets.arm.elf$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.targets.arm.elf$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void IArm$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.IArm", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IArm.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.IArm", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("ti.targets.arm.elf.IArm$$capsule", "ti.targets.arm.elf"));
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IArm", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IArm");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void Arm9$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.Arm9", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.Arm9.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.Arm9", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.elf")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.elf.Arm9$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("Arm9", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Arm9");
    }

    void IM0$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.IM0", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IM0.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.IM0", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IM0", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IM0");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void M0$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.M0", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.M0.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.M0", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.elf")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.elf.M0$$instance$static$init", null) ? 1 : 0);
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

    void IM3$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.IM3", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IM3.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.IM3", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IM3", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IM3");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void M3$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.M3", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.M3.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.M3", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.elf")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.elf.M3$$instance$static$init", null) ? 1 : 0);
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

    void IM4$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.IM4", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IM4.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.IM4", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IM4", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IM4");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void M4$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.M4", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.M4.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.M4", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.elf")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.elf.M4$$instance$static$init", null) ? 1 : 0);
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

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.M4F", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.M4F.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.M4F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.elf")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.elf.M4F$$instance$static$init", null) ? 1 : 0);
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

    void IR4$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.IR4", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IR4.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.IR4", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IR4", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IR4");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void R4F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.R4F", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.R4F.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.R4F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.elf")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.elf.R4F$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("R4F", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("R4F");
    }

    void R4Ft$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.R4Ft", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.R4Ft.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.R4Ft", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.elf")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.elf.R4Ft$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("R4Ft", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("R4Ft");
    }

    void IR5$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.IR5", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.IR5.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.IR5", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IR5", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IR5");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void R5F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.R5F", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.R5F.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.R5F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.elf")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.elf.R5F$$instance$static$init", null) ? 1 : 0);
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

    void R5Ft$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.targets.arm.elf.R5Ft", "ti.targets.arm.elf");
        po = (Proto.Obj)om.findStrict("ti.targets.arm.elf.R5Ft.Module", "ti.targets.arm.elf");
        vo.init2(po, "ti.targets.arm.elf.R5Ft", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.targets.arm.elf", "ti.targets.arm.elf"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "ti.targets.arm.elf"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "ti.targets.arm.elf"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "ti.targets.arm.elf"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "ti.targets.arm.elf"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "ti.targets.arm.elf"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "ti.targets.arm.elf"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "ti.targets.arm.elf"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "ti.targets.arm.elf"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "ti.targets.arm.elf"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "ti.targets.arm.elf"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "ti.targets.arm.elf"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "ti.targets.arm.elf"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "ti.targets.arm.elf"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "ti.targets.arm.elf"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets.arm.elf");
        inherits.add("ti.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.targets.arm.elf")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.targets.arm.elf.R5Ft$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("R5Ft", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("R5Ft");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.elf.Arm9", "ti.targets.arm.elf"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.elf.M0", "ti.targets.arm.elf"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.elf.M3", "ti.targets.arm.elf"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.elf.M4", "ti.targets.arm.elf"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.elf.M4F", "ti.targets.arm.elf"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.elf.R4F", "ti.targets.arm.elf"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.elf.R4Ft", "ti.targets.arm.elf"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.elf.R5F", "ti.targets.arm.elf"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.targets.arm.elf.R5Ft", "ti.targets.arm.elf"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.targets.arm.elf.IArm")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.Arm9")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.IM0")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.M0")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.IM3")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.M3")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.IM4")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.M4")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.M4F")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.IR4")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.R4F")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.R4Ft")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.IR5")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.R5F")).bless();
        ((Value.Obj)om.getv("ti.targets.arm.elf.R5Ft")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.targets.arm.elf")).add(pkgV);
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
        IArm$$OBJECTS();
        Arm9$$OBJECTS();
        IM0$$OBJECTS();
        M0$$OBJECTS();
        IM3$$OBJECTS();
        M3$$OBJECTS();
        IM4$$OBJECTS();
        M4$$OBJECTS();
        M4F$$OBJECTS();
        IR4$$OBJECTS();
        R4F$$OBJECTS();
        R4Ft$$OBJECTS();
        IR5$$OBJECTS();
        R5F$$OBJECTS();
        R5Ft$$OBJECTS();
        IArm$$CONSTS();
        Arm9$$CONSTS();
        IM0$$CONSTS();
        M0$$CONSTS();
        IM3$$CONSTS();
        M3$$CONSTS();
        IM4$$CONSTS();
        M4$$CONSTS();
        M4F$$CONSTS();
        IR4$$CONSTS();
        R4F$$CONSTS();
        R4Ft$$CONSTS();
        IR5$$CONSTS();
        R5F$$CONSTS();
        R5Ft$$CONSTS();
        IArm$$CREATES();
        Arm9$$CREATES();
        IM0$$CREATES();
        M0$$CREATES();
        IM3$$CREATES();
        M3$$CREATES();
        IM4$$CREATES();
        M4$$CREATES();
        M4F$$CREATES();
        IR4$$CREATES();
        R4F$$CREATES();
        R4Ft$$CREATES();
        IR5$$CREATES();
        R5F$$CREATES();
        R5Ft$$CREATES();
        IArm$$FUNCTIONS();
        Arm9$$FUNCTIONS();
        IM0$$FUNCTIONS();
        M0$$FUNCTIONS();
        IM3$$FUNCTIONS();
        M3$$FUNCTIONS();
        IM4$$FUNCTIONS();
        M4$$FUNCTIONS();
        M4F$$FUNCTIONS();
        IR4$$FUNCTIONS();
        R4F$$FUNCTIONS();
        R4Ft$$FUNCTIONS();
        IR5$$FUNCTIONS();
        R5F$$FUNCTIONS();
        R5Ft$$FUNCTIONS();
        IArm$$SIZES();
        Arm9$$SIZES();
        IM0$$SIZES();
        M0$$SIZES();
        IM3$$SIZES();
        M3$$SIZES();
        IM4$$SIZES();
        M4$$SIZES();
        M4F$$SIZES();
        IR4$$SIZES();
        R4F$$SIZES();
        R4Ft$$SIZES();
        IR5$$SIZES();
        R5F$$SIZES();
        R5Ft$$SIZES();
        IArm$$TYPES();
        Arm9$$TYPES();
        IM0$$TYPES();
        M0$$TYPES();
        IM3$$TYPES();
        M3$$TYPES();
        IM4$$TYPES();
        M4$$TYPES();
        M4F$$TYPES();
        IR4$$TYPES();
        R4F$$TYPES();
        R4Ft$$TYPES();
        IR5$$TYPES();
        R5F$$TYPES();
        R5Ft$$TYPES();
        if (isROV) {
            IArm$$ROV();
            Arm9$$ROV();
            IM0$$ROV();
            M0$$ROV();
            IM3$$ROV();
            M3$$ROV();
            IM4$$ROV();
            M4$$ROV();
            M4F$$ROV();
            IR4$$ROV();
            R4F$$ROV();
            R4Ft$$ROV();
            IR5$$ROV();
            R5F$$ROV();
            R5Ft$$ROV();
        }//isROV
        $$SINGLETONS();
        IArm$$SINGLETONS();
        Arm9$$SINGLETONS();
        IM0$$SINGLETONS();
        M0$$SINGLETONS();
        IM3$$SINGLETONS();
        M3$$SINGLETONS();
        IM4$$SINGLETONS();
        M4$$SINGLETONS();
        M4F$$SINGLETONS();
        IR4$$SINGLETONS();
        R4F$$SINGLETONS();
        R4Ft$$SINGLETONS();
        IR5$$SINGLETONS();
        R5F$$SINGLETONS();
        R5Ft$$SINGLETONS();
        $$INITIALIZATION();
    }
}
