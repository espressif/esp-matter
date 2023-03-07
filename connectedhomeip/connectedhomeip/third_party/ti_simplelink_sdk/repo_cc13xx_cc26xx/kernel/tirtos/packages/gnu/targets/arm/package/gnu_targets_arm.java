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

public class gnu_targets_arm
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
        pkgP = (Proto.Obj)om.bind("gnu.targets.arm.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("gnu.targets.arm", new Value.Obj("gnu.targets.arm", pkgP));
    }

    void ITarget$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.ITarget.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.ITarget", new Value.Obj("gnu.targets.arm.ITarget", po));
        pkgV.bind("ITarget", vo);
        // decls 
        om.bind("gnu.targets.arm.ITarget.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.ITarget.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void GCArmv5T$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.GCArmv5T.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.GCArmv5T", new Value.Obj("gnu.targets.arm.GCArmv5T", po));
        pkgV.bind("GCArmv5T", vo);
        // decls 
        om.bind("gnu.targets.arm.GCArmv5T.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv5T.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void GCArmv6$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.GCArmv6.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.GCArmv6", new Value.Obj("gnu.targets.arm.GCArmv6", po));
        pkgV.bind("GCArmv6", vo);
        // decls 
        om.bind("gnu.targets.arm.GCArmv6.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv6.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void GCArmv7A$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.GCArmv7A.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.GCArmv7A", new Value.Obj("gnu.targets.arm.GCArmv7A", po));
        pkgV.bind("GCArmv7A", vo);
        // decls 
        om.bind("gnu.targets.arm.GCArmv7A.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7A.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void GCArmv7AF$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.GCArmv7AF.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.GCArmv7AF", new Value.Obj("gnu.targets.arm.GCArmv7AF", po));
        pkgV.bind("GCArmv7AF", vo);
        // decls 
        om.bind("gnu.targets.arm.GCArmv7AF.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.GCArmv7AF.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void IM$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.IM.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.IM", new Value.Obj("gnu.targets.arm.IM", po));
        pkgV.bind("IM", vo);
        // decls 
        om.bind("gnu.targets.arm.IM.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.IM.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void M0$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.M0.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.M0", new Value.Obj("gnu.targets.arm.M0", po));
        pkgV.bind("M0", vo);
        // decls 
        om.bind("gnu.targets.arm.M0.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M0.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void M3$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.M3.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.M3", new Value.Obj("gnu.targets.arm.M3", po));
        pkgV.bind("M3", vo);
        // decls 
        om.bind("gnu.targets.arm.M3.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M3.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void M4$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.M4.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.M4", new Value.Obj("gnu.targets.arm.M4", po));
        pkgV.bind("M4", vo);
        // decls 
        om.bind("gnu.targets.arm.M4.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void M4F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.M4F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.M4F", new Value.Obj("gnu.targets.arm.M4F", po));
        pkgV.bind("M4F", vo);
        // decls 
        om.bind("gnu.targets.arm.M4F.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M4F.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void M33F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.M33F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.M33F", new Value.Obj("gnu.targets.arm.M33F", po));
        pkgV.bind("M33F", vo);
        // decls 
        om.bind("gnu.targets.arm.M33F.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.M33F.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void A8F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.A8F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.A8F", new Value.Obj("gnu.targets.arm.A8F", po));
        pkgV.bind("A8F", vo);
        // decls 
        om.bind("gnu.targets.arm.A8F.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A8F.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void A9F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.A9F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.A9F", new Value.Obj("gnu.targets.arm.A9F", po));
        pkgV.bind("A9F", vo);
        // decls 
        om.bind("gnu.targets.arm.A9F.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A9F.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void A15F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.A15F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.A15F", new Value.Obj("gnu.targets.arm.A15F", po));
        pkgV.bind("A15F", vo);
        // decls 
        om.bind("gnu.targets.arm.A15F.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A15F.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void A53F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.A53F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.A53F", new Value.Obj("gnu.targets.arm.A53F", po));
        pkgV.bind("A53F", vo);
        // decls 
        om.bind("gnu.targets.arm.A53F.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A53F.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void A72F$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("gnu.targets.arm.A72F.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("gnu.targets.arm.A72F", new Value.Obj("gnu.targets.arm.A72F", po));
        pkgV.bind("A72F", vo);
        // decls 
        om.bind("gnu.targets.arm.A72F.Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        om.bind("gnu.targets.arm.A72F.Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
    }

    void ITarget$$CONSTS()
    {
        // interface ITarget
    }

    void GCArmv5T$$CONSTS()
    {
        // module GCArmv5T
    }

    void GCArmv6$$CONSTS()
    {
        // module GCArmv6
    }

    void GCArmv7A$$CONSTS()
    {
        // module GCArmv7A
    }

    void GCArmv7AF$$CONSTS()
    {
        // module GCArmv7AF
    }

    void IM$$CONSTS()
    {
        // interface IM
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

    void M33F$$CONSTS()
    {
        // module M33F
    }

    void A8F$$CONSTS()
    {
        // module A8F
    }

    void A9F$$CONSTS()
    {
        // module A9F
    }

    void A15F$$CONSTS()
    {
        // module A15F
    }

    void A53F$$CONSTS()
    {
        // module A53F
    }

    void A72F$$CONSTS()
    {
        // module A72F
    }

    void ITarget$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void GCArmv5T$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void GCArmv6$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void GCArmv7A$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void GCArmv7AF$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IM$$CREATES()
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

    void M33F$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void A8F$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void A9F$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void A15F$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void A53F$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void A72F$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ITarget$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ITarget.initVers
        fxn = (Proto.Fxn)om.bind("gnu.targets.arm.ITarget$$initVers", new Proto.Fxn(om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"), $$T_Str, 0, 0, false));
        // fxn ITarget.asmName
        fxn = (Proto.Fxn)om.bind("gnu.targets.arm.ITarget$$asmName", new Proto.Fxn(om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "CName", $$T_Str, $$UNDEF);
    }

    void GCArmv5T$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GCArmv6$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GCArmv7A$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GCArmv7AF$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IM$$FUNCTIONS()
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

    void M33F$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void A8F$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void A9F$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void A15F$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void A53F$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void A72F$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITarget$$SIZES()
    {
    }

    void GCArmv5T$$SIZES()
    {
    }

    void GCArmv6$$SIZES()
    {
    }

    void GCArmv7A$$SIZES()
    {
    }

    void GCArmv7AF$$SIZES()
    {
    }

    void IM$$SIZES()
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

    void M33F$$SIZES()
    {
    }

    void A8F$$SIZES()
    {
    }

    void A9F$$SIZES()
    {
    }

    void A15F$$SIZES()
    {
    }

    void A53F$$SIZES()
    {
    }

    void A72F$$SIZES()
    {
    }

    void ITarget$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/ITarget.xs");
        om.bind("gnu.targets.arm.ITarget$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.ITarget.Module", om.findStrict("xdc.bld.ITarget3.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("GCCVERS", $$T_Str, null, "wh");
        po.addFld("BINVERS", $$T_Str, null, "wh");
        po.addFld("GCCTARG", $$T_Str, null, "wh");
        po.addFld("LONGNAME", $$T_Str, null, "wh");
        po.addFld("remoteHost", $$T_Str, $$UNDEF, "wh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-ar", "opts", "cr"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc", "opts", ""), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", ""), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler", "opts", ""), "rh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"), "linkOpts", "-g")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-O2 -ffunction-sections"), "linkOpts", "-Wl,--gc-sections")}), Global.newArray(new Object[]{"profile", Global.newObject("compileOpts", Global.newObject("copts", "-g -pg"), "linkOpts", "-pg")}), Global.newArray(new Object[]{"coverage", Global.newObject("compileOpts", Global.newObject("copts", "-fprofile-arcs -ftest-coverage"), "linkOpts", "-fprofile-arcs -ftest-coverage")})}), "wh");
        po.addFld("includeOpts", $$T_Str, "", "wh");
        po.addFld("binaryParser", $$T_Str, "xdc.targets.omf.Elf", "wh");
        po.addFld("bspLib", $$T_Str, null, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.ITarget$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.ITarget$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.ITarget$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void GCArmv5T$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("gnu.targets.arm.GCArmv5T.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.GCArmv5T.Module", om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "GCArmv5T", "rh");
        po.addFld("os", $$T_Str, "Linux", "rh");
        po.addFld("suffix", $$T_Str, "v5T", "rh");
        po.addFld("isa", $$T_Str, "v5T", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"), Global.newObject("endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv5T", "rh");
        po.addFld("platform", $$T_Str, "host.platforms.arm", "wh");
        po.addFld("stdInclude", $$T_Str, "gnu/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-march=armv5t"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-fPIC -Wunused", "suffix", "-Dfar= "), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler", "opts", "-march=armv5t"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "", "suffix", "-Wl,-Map=$(XDCCFGDIR)/$@.map -lstdc++ -L$(rootDir)/$(GCCTARG)/lib"), "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"470MV", "v5t"}), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 4L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 4L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 4L), "t_LLong", Global.newObject("size", 8L, "align", 4L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), $$UNDEF);
    }

    void GCArmv6$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/GCArmv6.xs");
        om.bind("gnu.targets.arm.GCArmv6$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.GCArmv6.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.GCArmv6.Module", om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "GCArmv6", "rh");
        po.addFld("os", $$T_Str, "Linux", "rh");
        po.addFld("suffix", $$T_Str, "v6", "rh");
        po.addFld("isa", $$T_Str, "v6", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"), Global.newObject("endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv6", "rh");
        po.addFld("platform", $$T_Str, "host.platforms.arm", "wh");
        po.addFld("stdInclude", $$T_Str, "gnu/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-march=armv6"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-fPIC -Wunused", "suffix", "-Dfar= "), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler", "opts", "-march=armv6"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "", "suffix", "-Wl,-Map=$(XDCCFGDIR)/$@.map -lstdc++ -L$(rootDir)/$(GCCTARG)/lib"), "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"v5T", "470MV", "v5t"}), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 4L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 4L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 4L), "t_LLong", Global.newObject("size", 8L, "align", 4L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.GCArmv6$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.GCArmv6$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.GCArmv6$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void GCArmv7A$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/GCArmv7A.xs");
        om.bind("gnu.targets.arm.GCArmv7A$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.GCArmv7A.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.GCArmv7A.Module", om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "GCArmv7A", "rh");
        po.addFld("os", $$T_Str, "Linux", "rh");
        po.addFld("suffix", $$T_Str, "v7A", "rh");
        po.addFld("isa", $$T_Str, "v7A", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"), Global.newObject("endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv7A", "rh");
        po.addFld("platform", $$T_Str, "host.platforms.arm", "wh");
        po.addFld("stdInclude", $$T_Str, "gnu/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-march=armv7-a"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-fPIC -Wunused", "suffix", "-Dfar= "), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler", "opts", "-march=armv7-a"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "", "suffix", "-Wl,-Map=$(XDCCFGDIR)/$@.map -lstdc++ -L$(rootDir)/$(GCCTARG)/lib"), "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"v6", "v5T", "470MV", "v5t"}), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 4L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 4L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 4L), "t_LLong", Global.newObject("size", 8L, "align", 4L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.GCArmv7A$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.GCArmv7A$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.GCArmv7A$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void GCArmv7AF$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("gnu.targets.arm.GCArmv7AF.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.GCArmv7AF.Module", om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "GCArmv7AF", "rh");
        po.addFld("os", $$T_Str, "Linux", "rh");
        po.addFld("suffix", $$T_Str, "v7AF", "rh");
        po.addFld("isa", $$T_Str, "v7A", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"), Global.newObject("endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv7A", "rh");
        po.addFld("platform", $$T_Str, "host.platforms.arm", "wh");
        po.addFld("stdInclude", $$T_Str, "gnu/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-march=armv7-a -mfloat-abi=hard"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-fPIC -Wunused", "suffix", "-Dfar= "), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler", "opts", "-march=armv7-a -mfloat-abi=hard"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "", "suffix", "-Wl,-Map=$(XDCCFGDIR)/$@.map -lstdc++ -L$(rootDir)/$(GCCTARG)/lib"), "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{}), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 4L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 4L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 4L), "t_LLong", Global.newObject("size", 8L, "align", 4L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), $$UNDEF);
    }

    void IM$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/IM.xs");
        om.bind("gnu.targets.arm.IM$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.IM.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.IM.Module", om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"), Global.newObject("endian", "little", "codeModel", "thumb2", "shortEnums", true), "rh");
        po.addFld("alignDirectiveSupported", $$T_Bool, true, "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv7M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.tiva:TM4C1294NCPDT", "wh");
        po.addFld("GCCTARG", $$T_Str, "arm-none-eabi", "wh");
        po.addFld("stdInclude", $$T_Str, "gnu/targets/arm/std.h", "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-Wunused -Wunknown-pragmas -ffunction-sections -fdata-sections ", "suffix", "-Dfar= -D__DYNAMIC_REENT__ "), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "$(ccOpts.prefix)", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("arBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-ar ", "opts", ""), "rh");
        po.addFld("bspLib", $$T_Str, "nosys", "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"), "linkOpts", "-g")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", " -O2 "), "linkOpts", " ")})}), "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{}), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.IM$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.IM$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.IM$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void M0$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/M0.xs");
        om.bind("gnu.targets.arm.M0$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.M0.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.M0.Module", om.findStrict("gnu.targets.arm.IM.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M0", "rh");
        po.addFld("suffix", $$T_Str, "m0g", "rh");
        po.addFld("isa", $$T_Str, "v6M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.simCM3", "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-m0plus -mabi=aapcs -g"), "rh");
        po.addFld("ccBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-m0plus -mabi=aapcs -g"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m0plus"), "rh");
        po.addFld("asmBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m0plus"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-mcpu=cortex-m0 -mthumb -nostartfiles -Wl,-static -Wl,--gc-sections", "suffix", "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib/thumb/v6-m/nofp -Wl,--start-group -lgcc -lc -lm -Wl,--end-group --specs=nano.specs -Wl,-Map=$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(packageBase)/libs/install-native/$(GCCTARG)/include/newlib-nano -I$(packageBase)/libs/install-native/$(GCCTARG)/include", "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.M0$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.M0$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.M0$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void M3$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/M3.xs");
        om.bind("gnu.targets.arm.M3$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.M3.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.M3.Module", om.findStrict("gnu.targets.arm.IM.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M3", "rh");
        po.addFld("suffix", $$T_Str, "m3g", "rh");
        po.addFld("isa", $$T_Str, "v7M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.tiva:TM4C1294NCPDT", "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-m3 -mthumb -mabi=aapcs -g"), "rh");
        po.addFld("ccBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-m3 -mthumb -mabi=aapcs -g"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m3 -mthumb"), "rh");
        po.addFld("asmBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m3 -mthumb"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-mcpu=cortex-m3 -mthumb -nostartfiles -Wl,-static -Wl,--gc-sections", "suffix", "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib/thumb/v7-m/nofp -Wl,--start-group -lgcc -lc -lm -Wl,--end-group --specs=nano.specs -Wl,-Map=$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(packageBase)/libs/install-native/$(GCCTARG)/include/newlib-nano -I$(packageBase)/libs/install-native/$(GCCTARG)/include", "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.M3$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.M3$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.M3$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void M4$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("gnu.targets.arm.M4.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.M4.Module", om.findStrict("gnu.targets.arm.IM.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M4", "rh");
        po.addFld("suffix", $$T_Str, "m4g", "rh");
        po.addFld("isa", $$T_Str, "v7M4", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft -mabi=aapcs -g"), "rh");
        po.addFld("ccBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft -mabi=aapcs -g"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m4 -mthumb"), "rh");
        po.addFld("asmBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m4 -mthumb"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft -nostartfiles -Wl,-static -Wl,--gc-sections ", "suffix", "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib/thumb/v7e-m/nofp -Wl,--start-group -lgcc -lc -lm -Wl,--end-group --specs=nano.specs -Wl,-Map=$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(packageBase)/libs/install-native/$(GCCTARG)/include/newlib-nano -I$(packageBase)/libs/install-native/$(GCCTARG)/include", "wh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), $$UNDEF);
    }

    void M4F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("gnu.targets.arm.M4F.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.M4F.Module", om.findStrict("gnu.targets.arm.IM.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M4F", "rh");
        po.addFld("suffix", $$T_Str, "m4fg", "rh");
        po.addFld("isa", $$T_Str, "v7M4", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mabi=aapcs -g"), "rh");
        po.addFld("ccBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mabi=aapcs -g"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 "), "rh");
        po.addFld("asmBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 "), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostartfiles -Wl,-static -Wl,--gc-sections ", "suffix", "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib/thumb/v7e-m/hard -Wl,--start-group -lgcc -lc -lm -Wl,--end-group --specs=nano.specs -Wl,-Map=$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(packageBase)/libs/install-native/$(GCCTARG)/include/newlib-nano -I$(packageBase)/libs/install-native/$(GCCTARG)/include", "wh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), $$UNDEF);
    }

    void M33F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("gnu.targets.arm.M33F.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.M33F.Module", om.findStrict("gnu.targets.arm.IM.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "M33F", "rh");
        po.addFld("suffix", $$T_Str, "m33fg", "rh");
        po.addFld("isa", $$T_Str, "v8M", "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv8M", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexM:FVP_MPS2", "wh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-march=armv8-m.main+dsp -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mabi=aapcs -g"), "rh");
        po.addFld("ccBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -MD -MF $@.dep", "opts", "-march=armv8-m.main+dsp -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mabi=aapcs -g"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-Wunused -Wunknown-pragmas -ffunction-sections -fdata-sections ", "suffix", "-Dfar= "), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "$(ccOpts.prefix)", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(packageBase)/libs/install-native/$(GCCTARG)/include/newlib-nano -I$(packageBase)/libs/install-native/$(GCCTARG)/include", "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp", "opts", "-march=armv8-m.main+dsp -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 "), "rh");
        po.addFld("asmBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -x assembler-with-cpp", "opts", "-march=armv8-m.main+dsp -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 "), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-march=armv8-m.main+dsp -mtune=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -nostartfiles -Wl,-static -Wl,--gc-sections ", "suffix", "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib/thumb/v8-m/hard -Wl,--start-group -lgcc -lc -lm -Wl,--end-group --specs=nano.specs -Wl,-Map=$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("bspLib", $$T_Str, "nosys", "wh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), $$UNDEF);
                po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), $$UNDEF);
    }

    void A8F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/A8F.xs");
        om.bind("gnu.targets.arm.A8F$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.A8F.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.A8F.Module", om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "A8F", "rh");
        po.addFld("suffix", $$T_Str, "a8fg", "rh");
        po.addFld("isa", $$T_Str, "v7A", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"), Global.newObject("endian", "little", "shortEnums", true), "rh");
        po.addFld("alignDirectiveSupported", $$T_Bool, true, "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv7A", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.evmAM3359", "wh");
        po.addFld("GCCTARG", $$T_Str, "arm-none-eabi", "wh");
        po.addFld("stdInclude", $$T_Str, "gnu/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -g"), "rh");
        po.addFld("ccBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -g"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-Wunused -Wunknown-pragmas -ffunction-sections -fdata-sections ", "suffix", "-Dfar= -D__DYNAMIC_REENT__ "), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "$(ccOpts.prefix)", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard"), "rh");
        po.addFld("asmBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-mcpu=cortex-a8 -mfloat-abi=hard -mfpu=neon -nostartfiles -Wl,-static -Wl,--gc-sections", "suffix", "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib/thumb/v7-a/hard -Wl,--start-group -lgcc -lc -lm -Wl,--end-group -Wl,-Map=$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("arBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-ar ", "opts", ""), "rh");
        po.addFld("bspLib", $$T_Str, "nosys", "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(packageBase)/libs/install-native/$(GCCTARG)/include", "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"), "linkOpts", "-g")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", " -O2 "), "linkOpts", " ")})}), "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{}), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.A8F$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.A8F$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.A8F$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void A9F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/A9F.xs");
        om.bind("gnu.targets.arm.A9F$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.A9F.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.A9F.Module", om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "A9F", "rh");
        po.addFld("suffix", $$T_Str, "a9fg", "rh");
        po.addFld("isa", $$T_Str, "v7A9", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"), Global.newObject("endian", "little", "shortEnums", true), "rh");
        po.addFld("alignDirectiveSupported", $$T_Bool, true, "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv7A", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.sdp4430", "wh");
        po.addFld("GCCTARG", $$T_Str, "arm-none-eabi", "wh");
        po.addFld("stdInclude", $$T_Str, "gnu/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -g"), "rh");
        po.addFld("ccBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -g"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-Wunused -Wunknown-pragmas -ffunction-sections -fdata-sections ", "suffix", "-Dfar= -D__DYNAMIC_REENT__ "), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "$(ccOpts.prefix)", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard"), "rh");
        po.addFld("asmBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-mcpu=cortex-a9 -mfloat-abi=hard -mfpu=neon -nostartfiles -Wl,-static -Wl,--gc-sections", "suffix", "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib/thumb/v7-a/hard -Wl,--start-group -lgcc -lc -lm -Wl,--end-group -Wl,-Map=$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("arBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-ar ", "opts", ""), "rh");
        po.addFld("bspLib", $$T_Str, "nosys", "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(packageBase)/libs/install-native/$(GCCTARG)/include", "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"), "linkOpts", "-g")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", " -O2 "), "linkOpts", " ")})}), "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{}), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.A9F$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.A9F$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.A9F$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void A15F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/A15F.xs");
        om.bind("gnu.targets.arm.A15F$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.A15F.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.A15F.Module", om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "A15F", "rh");
        po.addFld("suffix", $$T_Str, "a15fg", "rh");
        po.addFld("isa", $$T_Str, "v7A15", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"), Global.newObject("endian", "little", "shortEnums", true), "rh");
        po.addFld("alignDirectiveSupported", $$T_Bool, true, "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv7A", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.sdp5430", "wh");
        po.addFld("GCCTARG", $$T_Str, "arm-none-eabi", "wh");
        po.addFld("stdInclude", $$T_Str, "gnu/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-a15 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -g"), "rh");
        po.addFld("ccBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-a15 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -g"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-Wunused -Wunknown-pragmas -ffunction-sections -fdata-sections ", "suffix", "-Dfar= -D__DYNAMIC_REENT__ "), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "$(ccOpts.prefix)", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-a15 -mfpu=neon -mfloat-abi=hard"), "rh");
        po.addFld("asmBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-a15 -mfpu=neon -mfloat-abi=hard"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-mcpu=cortex-a15 -mfpu=neon -mfloat-abi=hard -nostartfiles -Wl,-static -Wl,--gc-sections", "suffix", "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib/thumb/v7-a/hard -Wl,--start-group -lgcc -lc -lm -Wl,--end-group -Wl,-Map=$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("arBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/arm-none-eabi-ar ", "opts", ""), "rh");
        po.addFld("bspLib", $$T_Str, "nosys", "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(packageBase)/libs/install-native/$(GCCTARG)/include", "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"), "linkOpts", "-g")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", " -O2 "), "linkOpts", " ")})}), "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{}), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.A15F$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.A15F$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.A15F$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void A53F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/A53F.xs");
        om.bind("gnu.targets.arm.A53F$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.A53F.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.A53F.Module", om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "A53F", "rh");
        po.addFld("suffix", $$T_Str, "a53fg", "rh");
        po.addFld("isa", $$T_Str, "v8A", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"), Global.newObject("endian", "little", "shortEnums", false), "rh");
        po.addFld("alignDirectiveSupported", $$T_Bool, true, "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv8A", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexA:AM65X", "wh");
        po.addFld("GCCTARG", $$T_Str, "aarch64-none-elf", "wh");
        po.addFld("binaryParser", $$T_Str, "xdc.targets.omf.Elf", "wh");
        po.addFld("stdInclude", $$T_Str, "gnu/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-a53+fp+simd -mabi=lp64 -mcmodel=large -mstrict-align -mfix-cortex-a53-835769 -mfix-cortex-a53-843419 -g"), "rh");
        po.addFld("ccBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/aarch64-none-elf-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-a53+fp+simd -mabi=lp64 -mcmodel=large -mstrict-align -mfix-cortex-a53-835769 -mfix-cortex-a53-843419 -g"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-Wunused -Wunknown-pragmas -ffunction-sections -fdata-sections ", "suffix", "-Dfar= -D__DYNAMIC_REENT__ "), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "$(ccOpts.prefix)", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-a53+fp+simd -mabi=lp64 -mcmodel=large -mstrict-align -mfix-cortex-a53-835769 -mfix-cortex-a53-843419"), "rh");
        po.addFld("asmBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/aarch64-none-elf-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-a53+fp+simd -mabi=lp64 -mcmodel=large -mstrict-align -mfix-cortex-a53-835769 -mfix-cortex-a53-843419"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-mabi=lp64 -nostartfiles -Wl,-static -Wl,--gc-sections", "suffix", "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib -Wl,--start-group -lgcc -lc -lm -Wl,--end-group -Wl,-Map=$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("arBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/aarch64-none-elf-ar ", "opts", ""), "rh");
        po.addFld("bspLib", $$T_Str, "nosys", "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(packageBase)/libs/install-native/$(GCCTARG)/include", "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"), "linkOpts", "-g")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", " -O2 "), "linkOpts", " ")})}), "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{}), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 8L, "align", 8L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 8L, "align", 8L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 8L, "align", 8L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 8L, "align", 8L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 8L, "align", 8L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.A53F$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.A53F$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.A53F$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void A72F$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/A72F.xs");
        om.bind("gnu.targets.arm.A72F$$capsule", cap);
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.A72F.Module", "gnu.targets.arm");
        po.init("gnu.targets.arm.A72F.Module", om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "A72F", "rh");
        po.addFld("suffix", $$T_Str, "a72fg", "rh");
        po.addFld("isa", $$T_Str, "v8A", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"), Global.newObject("endian", "little", "shortEnums", false), "rh");
        po.addFld("alignDirectiveSupported", $$T_Bool, true, "rh");
        po.addFld("rts", $$T_Str, "gnu.targets.arm.rtsv8A", "rh");
        po.addFld("platform", $$T_Str, "ti.platforms.cortexA:J721E", "wh");
        po.addFld("GCCTARG", $$T_Str, "aarch64-none-elf", "wh");
        po.addFld("binaryParser", $$T_Str, "xdc.targets.omf.Elf", "wh");
        po.addFld("stdInclude", $$T_Str, "gnu/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-a72+fp+simd -mabi=lp64 -mcmodel=large -mstrict-align -g"), "rh");
        po.addFld("ccBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/aarch64-none-elf-gcc -c -MD -MF $@.dep", "opts", "-mcpu=cortex-a72+fp+simd -mabi=lp64 -mcmodel=large -mstrict-align -g"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-Wunused -Wunknown-pragmas -ffunction-sections -fdata-sections ", "suffix", "-Dfar= -D__DYNAMIC_REENT__ "), "wh");
        po.addFld("ccConfigOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "$(ccOpts.prefix)", "suffix", "$(ccOpts.suffix)"), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-a72+fp+simd -mabi=lp64 -mcmodel=large -mstrict-align"), "rh");
        po.addFld("asmBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/aarch64-none-elf-gcc -c -x assembler-with-cpp", "opts", "-mcpu=cortex-a72+fp+simd -mabi=lp64 -mcmodel=large -mstrict-align"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"), Global.newObject("prefix", "-mabi=lp64 -nostartfiles -Wl,-static -Wl,--gc-sections", "suffix", "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib -Wl,--start-group -lgcc -lc -lm -Wl,--end-group -Wl,-Map=$(XDCCFGDIR)/$@.map"), "wh");
        po.addFld("arBin", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"), Global.newObject("cmd", "bin/aarch64-none-elf-ar ", "opts", ""), "rh");
        po.addFld("bspLib", $$T_Str, "nosys", "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(packageBase)/libs/install-native/$(GCCTARG)/include", "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"), "linkOpts", "-g")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", " -O2 "), "linkOpts", " ")})}), "wh");
        po.addFld("compatibleSuffixes", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{}), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 8L, "align", 8L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 8L, "align", 8L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 8L, "align", 8L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_LLong", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 8L, "align", 8L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 8L, "align", 8L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("gnu.targets.arm.A72F$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("gnu.targets.arm.A72F$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("gnu.targets.arm.A72F$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "initVers");
                if (fxn != null) po.addFxn("initVers", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$initVers", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "asmName");
                if (fxn != null) po.addFxn("asmName", (Proto.Fxn)om.findStrict("gnu.targets.arm.ITarget$$asmName", "gnu.targets.arm"), fxn);
    }

    void ITarget$$ROV()
    {
    }

    void GCArmv5T$$ROV()
    {
    }

    void GCArmv6$$ROV()
    {
    }

    void GCArmv7A$$ROV()
    {
    }

    void GCArmv7AF$$ROV()
    {
    }

    void IM$$ROV()
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

    void M33F$$ROV()
    {
    }

    void A8F$$ROV()
    {
    }

    void A9F$$ROV()
    {
    }

    void A15F$$ROV()
    {
    }

    void A53F$$ROV()
    {
    }

    void A72F$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("gnu.targets.arm.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "gnu.targets.arm"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "gnu/targets/arm/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "gnu.targets.arm"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "gnu.targets.arm"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "gnu.targets.arm", Value.DEFAULT, false);
        pkgV.bind("$name", "gnu.targets.arm");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "gnu.targets.arm.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.bld", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['gnu.targets.arm'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('gnu.targets.arm$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['gnu.targets.arm$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['gnu.targets.arm$$stat$root'];\n");
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

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.ITarget", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.ITarget.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.ITarget", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.ITarget$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
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

    void GCArmv5T$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.GCArmv5T", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.GCArmv5T.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.GCArmv5T", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.GCArmv5T$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("GCArmv5T", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GCArmv5T");
    }

    void GCArmv6$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.GCArmv6", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.GCArmv6.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.GCArmv6", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.GCArmv6$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.GCArmv6$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("GCArmv6", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GCArmv6");
    }

    void GCArmv7A$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.GCArmv7A", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.GCArmv7A.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.GCArmv7A", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.GCArmv7A$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.GCArmv7A$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("GCArmv7A", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GCArmv7A");
    }

    void GCArmv7AF$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.GCArmv7AF", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.GCArmv7AF.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.GCArmv7AF", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.GCArmv7AF$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("GCArmv7AF", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GCArmv7AF");
    }

    void IM$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.IM", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.IM.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.IM", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.IM$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IM", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IM");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void M0$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.M0", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.M0.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.M0", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.M0$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.M0$$instance$static$init", null) ? 1 : 0);
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

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.M3", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.M3.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.M3", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.M3$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.M3$$instance$static$init", null) ? 1 : 0);
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

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.M4", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.M4.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.M4", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.M4$$instance$static$init", null) ? 1 : 0);
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

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.M4F", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.M4F.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.M4F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.M4F$$instance$static$init", null) ? 1 : 0);
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

    void M33F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.M33F", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.M33F.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.M33F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.M33F$$instance$static$init", null) ? 1 : 0);
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

    void A8F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.A8F", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.A8F.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.A8F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.A8F$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.A8F$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("A8F", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("A8F");
    }

    void A9F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.A9F", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.A9F.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.A9F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.A9F$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.A9F$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("A9F", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("A9F");
    }

    void A15F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.A15F", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.A15F.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.A15F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.A15F$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.A15F$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("A15F", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("A15F");
    }

    void A53F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.A53F", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.A53F.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.A53F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.A53F$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.A53F$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("A53F", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("A53F");
    }

    void A72F$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("gnu.targets.arm.A72F", "gnu.targets.arm");
        po = (Proto.Obj)om.findStrict("gnu.targets.arm.A72F.Module", "gnu.targets.arm");
        vo.init2(po, "gnu.targets.arm.A72F", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("gnu.targets.arm.A72F$$capsule", "gnu.targets.arm"));
        vo.bind("$package", om.findStrict("gnu.targets.arm", "gnu.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "gnu.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "gnu.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "gnu.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "gnu.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "gnu.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "gnu.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "gnu.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "gnu.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "gnu.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "gnu.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "gnu.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "gnu.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "gnu.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "gnu.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("gnu.targets.arm");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "gnu.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("gnu.targets.arm.A72F$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("A72F", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("A72F");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.GCArmv5T", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.GCArmv6", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.GCArmv7A", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.GCArmv7AF", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.M0", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.M3", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.M4", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.M4F", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.M33F", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.A8F", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.A9F", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.A15F", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.A53F", "gnu.targets.arm"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("gnu.targets.arm.A72F", "gnu.targets.arm"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("gnu.targets.arm.ITarget")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.GCArmv5T")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.GCArmv6")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.GCArmv7A")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.GCArmv7AF")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.IM")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.M0")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.M3")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.M4")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.M4F")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.M33F")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.A8F")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.A9F")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.A15F")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.A53F")).bless();
        ((Value.Obj)om.getv("gnu.targets.arm.A72F")).bless();
        ((Value.Arr)om.findStrict("$packages", "gnu.targets.arm")).add(pkgV);
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
        GCArmv5T$$OBJECTS();
        GCArmv6$$OBJECTS();
        GCArmv7A$$OBJECTS();
        GCArmv7AF$$OBJECTS();
        IM$$OBJECTS();
        M0$$OBJECTS();
        M3$$OBJECTS();
        M4$$OBJECTS();
        M4F$$OBJECTS();
        M33F$$OBJECTS();
        A8F$$OBJECTS();
        A9F$$OBJECTS();
        A15F$$OBJECTS();
        A53F$$OBJECTS();
        A72F$$OBJECTS();
        ITarget$$CONSTS();
        GCArmv5T$$CONSTS();
        GCArmv6$$CONSTS();
        GCArmv7A$$CONSTS();
        GCArmv7AF$$CONSTS();
        IM$$CONSTS();
        M0$$CONSTS();
        M3$$CONSTS();
        M4$$CONSTS();
        M4F$$CONSTS();
        M33F$$CONSTS();
        A8F$$CONSTS();
        A9F$$CONSTS();
        A15F$$CONSTS();
        A53F$$CONSTS();
        A72F$$CONSTS();
        ITarget$$CREATES();
        GCArmv5T$$CREATES();
        GCArmv6$$CREATES();
        GCArmv7A$$CREATES();
        GCArmv7AF$$CREATES();
        IM$$CREATES();
        M0$$CREATES();
        M3$$CREATES();
        M4$$CREATES();
        M4F$$CREATES();
        M33F$$CREATES();
        A8F$$CREATES();
        A9F$$CREATES();
        A15F$$CREATES();
        A53F$$CREATES();
        A72F$$CREATES();
        ITarget$$FUNCTIONS();
        GCArmv5T$$FUNCTIONS();
        GCArmv6$$FUNCTIONS();
        GCArmv7A$$FUNCTIONS();
        GCArmv7AF$$FUNCTIONS();
        IM$$FUNCTIONS();
        M0$$FUNCTIONS();
        M3$$FUNCTIONS();
        M4$$FUNCTIONS();
        M4F$$FUNCTIONS();
        M33F$$FUNCTIONS();
        A8F$$FUNCTIONS();
        A9F$$FUNCTIONS();
        A15F$$FUNCTIONS();
        A53F$$FUNCTIONS();
        A72F$$FUNCTIONS();
        ITarget$$SIZES();
        GCArmv5T$$SIZES();
        GCArmv6$$SIZES();
        GCArmv7A$$SIZES();
        GCArmv7AF$$SIZES();
        IM$$SIZES();
        M0$$SIZES();
        M3$$SIZES();
        M4$$SIZES();
        M4F$$SIZES();
        M33F$$SIZES();
        A8F$$SIZES();
        A9F$$SIZES();
        A15F$$SIZES();
        A53F$$SIZES();
        A72F$$SIZES();
        ITarget$$TYPES();
        GCArmv5T$$TYPES();
        GCArmv6$$TYPES();
        GCArmv7A$$TYPES();
        GCArmv7AF$$TYPES();
        IM$$TYPES();
        M0$$TYPES();
        M3$$TYPES();
        M4$$TYPES();
        M4F$$TYPES();
        M33F$$TYPES();
        A8F$$TYPES();
        A9F$$TYPES();
        A15F$$TYPES();
        A53F$$TYPES();
        A72F$$TYPES();
        if (isROV) {
            ITarget$$ROV();
            GCArmv5T$$ROV();
            GCArmv6$$ROV();
            GCArmv7A$$ROV();
            GCArmv7AF$$ROV();
            IM$$ROV();
            M0$$ROV();
            M3$$ROV();
            M4$$ROV();
            M4F$$ROV();
            M33F$$ROV();
            A8F$$ROV();
            A9F$$ROV();
            A15F$$ROV();
            A53F$$ROV();
            A72F$$ROV();
        }//isROV
        $$SINGLETONS();
        ITarget$$SINGLETONS();
        GCArmv5T$$SINGLETONS();
        GCArmv6$$SINGLETONS();
        GCArmv7A$$SINGLETONS();
        GCArmv7AF$$SINGLETONS();
        IM$$SINGLETONS();
        M0$$SINGLETONS();
        M3$$SINGLETONS();
        M4$$SINGLETONS();
        M4F$$SINGLETONS();
        M33F$$SINGLETONS();
        A8F$$SINGLETONS();
        A9F$$SINGLETONS();
        A15F$$SINGLETONS();
        A53F$$SINGLETONS();
        A72F$$SINGLETONS();
        $$INITIALIZATION();
    }
}
