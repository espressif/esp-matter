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

public class ti_sysbios_interfaces
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
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.interfaces.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.interfaces", new Value.Obj("ti.sysbios.interfaces", pkgP));
    }

    void ICore$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ICore.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.ICore", new Value.Obj("ti.sysbios.interfaces.ICore", po));
        pkgV.bind("ICore", vo);
        // decls 
    }

    void IHwi$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.IHwi.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.IHwi", new Value.Obj("ti.sysbios.interfaces.IHwi", po));
        pkgV.bind("IHwi", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.interfaces.IHwi$$HookSet", new Proto.Obj());
        om.bind("ti.sysbios.interfaces.IHwi.HookSet", new Proto.Str(spo, false));
        om.bind("ti.sysbios.interfaces.IHwi.MaskingOption", new Proto.Enm("ti.sysbios.interfaces.IHwi.MaskingOption"));
        spo = (Proto.Obj)om.bind("ti.sysbios.interfaces.IHwi$$StackInfo", new Proto.Obj());
        om.bind("ti.sysbios.interfaces.IHwi.StackInfo", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.interfaces.IHwi.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.IHwi$$Object", new Proto.Obj());
        om.bind("ti.sysbios.interfaces.IHwi.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.IHwi$$Params", new Proto.Obj());
        om.bind("ti.sysbios.interfaces.IHwi.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.IHwi$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.interfaces.IHwi.Instance_State", new Proto.Str(po, false));
        om.bind("ti.sysbios.interfaces.IHwi.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.interfaces.IHwi.Object", om.findStrict("ti.sysbios.interfaces.IHwi.Instance_State", "ti.sysbios.interfaces"));
        }//isROV
    }

    void ITaskSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ITaskSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.ITaskSupport", new Value.Obj("ti.sysbios.interfaces.ITaskSupport", po));
        pkgV.bind("ITaskSupport", vo);
        // decls 
    }

    void ITimer$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ITimer.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.ITimer", new Value.Obj("ti.sysbios.interfaces.ITimer", po));
        pkgV.bind("ITimer", vo);
        // decls 
        om.bind("ti.sysbios.interfaces.ITimer.StartMode", new Proto.Enm("ti.sysbios.interfaces.ITimer.StartMode"));
        om.bind("ti.sysbios.interfaces.ITimer.RunMode", new Proto.Enm("ti.sysbios.interfaces.ITimer.RunMode"));
        om.bind("ti.sysbios.interfaces.ITimer.Status", new Proto.Enm("ti.sysbios.interfaces.ITimer.Status"));
        om.bind("ti.sysbios.interfaces.ITimer.PeriodType", new Proto.Enm("ti.sysbios.interfaces.ITimer.PeriodType"));
        // insts 
        Object insP = om.bind("ti.sysbios.interfaces.ITimer.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ITimer$$Object", new Proto.Obj());
        om.bind("ti.sysbios.interfaces.ITimer.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ITimer$$Params", new Proto.Obj());
        om.bind("ti.sysbios.interfaces.ITimer.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ITimer$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.interfaces.ITimer.Instance_State", new Proto.Str(po, false));
        om.bind("ti.sysbios.interfaces.ITimer.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.interfaces.ITimer.Object", om.findStrict("ti.sysbios.interfaces.ITimer.Instance_State", "ti.sysbios.interfaces"));
        }//isROV
    }

    void ITimerSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ITimerSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.ITimerSupport", new Value.Obj("ti.sysbios.interfaces.ITimerSupport", po));
        pkgV.bind("ITimerSupport", vo);
        // decls 
    }

    void ITimestamp$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ITimestamp.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.ITimestamp", new Value.Obj("ti.sysbios.interfaces.ITimestamp", po));
        pkgV.bind("ITimestamp", vo);
        // decls 
    }

    void IIntrinsicsSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.IIntrinsicsSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.IIntrinsicsSupport", new Value.Obj("ti.sysbios.interfaces.IIntrinsicsSupport", po));
        pkgV.bind("IIntrinsicsSupport", vo);
        // decls 
    }

    void ICache$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ICache.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.ICache", new Value.Obj("ti.sysbios.interfaces.ICache", po));
        pkgV.bind("ICache", vo);
        // decls 
        om.bind("ti.sysbios.interfaces.ICache.Type", new Proto.Enm("ti.sysbios.interfaces.ICache.Type"));
    }

    void ISettings$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ISettings.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.ISettings", new Value.Obj("ti.sysbios.interfaces.ISettings", po));
        pkgV.bind("ISettings", vo);
        // decls 
    }

    void IPower$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.IPower.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.IPower", new Value.Obj("ti.sysbios.interfaces.IPower", po));
        pkgV.bind("IPower", vo);
        // decls 
    }

    void IRomDevice$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.IRomDevice.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.IRomDevice", new Value.Obj("ti.sysbios.interfaces.IRomDevice", po));
        pkgV.bind("IRomDevice", vo);
        // decls 
    }

    void ISeconds$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.interfaces.ISeconds.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.interfaces.ISeconds", new Value.Obj("ti.sysbios.interfaces.ISeconds", po));
        pkgV.bind("ISeconds", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.interfaces.ISeconds$$Time", new Proto.Obj());
        om.bind("ti.sysbios.interfaces.ISeconds.Time", new Proto.Str(spo, false));
    }

    void ICore$$CONSTS()
    {
        // interface ICore
    }

    void IHwi$$CONSTS()
    {
        // interface IHwi
        om.bind("ti.sysbios.interfaces.IHwi.MaskingOption_NONE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.IHwi.MaskingOption_NONE", 0));
        om.bind("ti.sysbios.interfaces.IHwi.MaskingOption_ALL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.IHwi.MaskingOption_ALL", 1));
        om.bind("ti.sysbios.interfaces.IHwi.MaskingOption_SELF", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.IHwi.MaskingOption_SELF", 2));
        om.bind("ti.sysbios.interfaces.IHwi.MaskingOption_BITMASK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.IHwi.MaskingOption_BITMASK", 3));
        om.bind("ti.sysbios.interfaces.IHwi.MaskingOption_LOWER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.IHwi.MaskingOption_LOWER", 4));
    }

    void ITaskSupport$$CONSTS()
    {
        // interface ITaskSupport
    }

    void ITimer$$CONSTS()
    {
        // interface ITimer
        om.bind("ti.sysbios.interfaces.ITimer.ANY", Global.eval("~0"));
        om.bind("ti.sysbios.interfaces.ITimer.StartMode_AUTO", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ITimer.StartMode_AUTO", 0));
        om.bind("ti.sysbios.interfaces.ITimer.StartMode_USER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ITimer.StartMode_USER", 1));
        om.bind("ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS", 0));
        om.bind("ti.sysbios.interfaces.ITimer.RunMode_ONESHOT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ITimer.RunMode_ONESHOT", 1));
        om.bind("ti.sysbios.interfaces.ITimer.RunMode_DYNAMIC", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ITimer.RunMode_DYNAMIC", 2));
        om.bind("ti.sysbios.interfaces.ITimer.Status_INUSE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ITimer.Status_INUSE", 0));
        om.bind("ti.sysbios.interfaces.ITimer.Status_FREE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ITimer.Status_FREE", 1));
        om.bind("ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS", 0));
        om.bind("ti.sysbios.interfaces.ITimer.PeriodType_COUNTS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ITimer.PeriodType_COUNTS", 1));
    }

    void ITimerSupport$$CONSTS()
    {
        // interface ITimerSupport
    }

    void ITimestamp$$CONSTS()
    {
        // interface ITimestamp
    }

    void IIntrinsicsSupport$$CONSTS()
    {
        // interface IIntrinsicsSupport
    }

    void ICache$$CONSTS()
    {
        // interface ICache
        om.bind("ti.sysbios.interfaces.ICache.Type_L1P", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ICache.Type", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ICache.Type_L1P", xdc.services.intern.xsr.Enum.intValue(0x1L)+0));
        om.bind("ti.sysbios.interfaces.ICache.Type_L1D", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ICache.Type", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ICache.Type_L1D", xdc.services.intern.xsr.Enum.intValue(0x2L)+0));
        om.bind("ti.sysbios.interfaces.ICache.Type_L1", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ICache.Type", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ICache.Type_L1", xdc.services.intern.xsr.Enum.intValue(0x3L)+0));
        om.bind("ti.sysbios.interfaces.ICache.Type_L2P", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ICache.Type", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ICache.Type_L2P", xdc.services.intern.xsr.Enum.intValue(0x4L)+0));
        om.bind("ti.sysbios.interfaces.ICache.Type_L2D", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ICache.Type", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ICache.Type_L2D", xdc.services.intern.xsr.Enum.intValue(0x8L)+0));
        om.bind("ti.sysbios.interfaces.ICache.Type_L2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ICache.Type", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ICache.Type_L2", xdc.services.intern.xsr.Enum.intValue(0xCL)+0));
        om.bind("ti.sysbios.interfaces.ICache.Type_ALLP", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ICache.Type", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ICache.Type_ALLP", xdc.services.intern.xsr.Enum.intValue(0x5L)+0));
        om.bind("ti.sysbios.interfaces.ICache.Type_ALLD", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ICache.Type", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ICache.Type_ALLD", xdc.services.intern.xsr.Enum.intValue(0xAL)+0));
        om.bind("ti.sysbios.interfaces.ICache.Type_ALL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.interfaces.ICache.Type", "ti.sysbios.interfaces"), "ti.sysbios.interfaces.ICache.Type_ALL", xdc.services.intern.xsr.Enum.intValue(0x7fffL)+0));
    }

    void ISettings$$CONSTS()
    {
        // interface ISettings
    }

    void IPower$$CONSTS()
    {
        // interface IPower
    }

    void IRomDevice$$CONSTS()
    {
        // interface IRomDevice
    }

    void ISeconds$$CONSTS()
    {
        // interface ISeconds
    }

    void ICore$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IHwi$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ITaskSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ITimer$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ITimerSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ITimestamp$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IIntrinsicsSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ICache$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ISettings$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IPower$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IRomDevice$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ISeconds$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ICore$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IHwi$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn IHwi.addHookSet
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.IHwi$$addHookSet", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.IHwi.Module", "ti.sysbios.interfaces"), null, 1, 1, false));
                fxn.addArg(0, "hook", (Proto)om.findStrict("ti.sysbios.interfaces.IHwi.HookSet", "ti.sysbios.interfaces"), $$DEFAULT);
        // fxn IHwi.viewGetStackInfo
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.IHwi$$viewGetStackInfo", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.IHwi.Module", "ti.sysbios.interfaces"), (Proto)om.findStrict("ti.sysbios.interfaces.IHwi.StackInfo", "ti.sysbios.interfaces"), 0, 0, false));
    }

    void ITaskSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ITaskSupport.stackUsed$view
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ITaskSupport$$stackUsed$view", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ITaskSupport.Module", "ti.sysbios.interfaces"), Proto.Elm.newCNum("(xdc_SizeT)"), 1, 1, false));
                fxn.addArg(0, "stack", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt8)"), false), $$DEFAULT);
        // fxn ITaskSupport.getCallStack$view
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ITaskSupport$$getCallStack$view", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ITaskSupport.Module", "ti.sysbios.interfaces"), null, 0, -1, false));
    }

    void ITimer$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ITimer.viewGetCurrentClockTick
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ITimer$$viewGetCurrentClockTick", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.interfaces"), Proto.Elm.newCNum("(xdc_UInt32)"), 0, 0, false));
        // fxn ITimer.getFreqMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ITimer$$getFreqMeta", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.interfaces"), (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios.interfaces"), 1, 1, false));
                fxn.addArg(0, "id", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF);
    }

    void ITimerSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITimestamp$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ITimestamp.getFreqMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ITimestamp$$getFreqMeta", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ITimestamp.Module", "ti.sysbios.interfaces"), (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios.interfaces"), 0, 0, false));
    }

    void IIntrinsicsSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ICache$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ISettings$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ISettings.getDefaultCoreDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultCoreDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultHwiDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultHwiDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultTimerDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultTimerDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultClockTimerDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultClockTimerDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultTimerSupportDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultTimerSupportDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultTimestampDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultTimestampDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultTaskSupportDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultTaskSupportDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultIntrinsicsSupportDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultIntrinsicsSupportDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultCacheDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultCacheDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultPowerDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultPowerDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultSecondsDelegate
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultSecondsDelegate", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultBootModule
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultBootModule", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultMmuModule
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultMmuModule", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getFamilySettingsXml
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getFamilySettingsXml", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), $$T_Str, 0, 0, false));
        // fxn ISettings.getDefaultClockTickPeriod
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.ISettings$$getDefaultClockTickPeriod", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces"), Proto.Elm.newCNum("(xdc_UInt32)"), 0, 0, false));
    }

    void IPower$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IRomDevice$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn IRomDevice.loadAppConfig
        fxn = (Proto.Fxn)om.bind("ti.sysbios.interfaces.IRomDevice$$loadAppConfig", new Proto.Fxn(om.findStrict("ti.sysbios.interfaces.IRomDevice.Module", "ti.sysbios.interfaces"), null, 0, -1, false));
    }

    void ISeconds$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ICore$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void IHwi$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.interfaces.IHwi.HookSet", "ti.sysbios.interfaces");
        sizes.clear();
        sizes.add(Global.newArray("registerFxn", "UFxn"));
        sizes.add(Global.newArray("createFxn", "UFxn"));
        sizes.add(Global.newArray("beginFxn", "UFxn"));
        sizes.add(Global.newArray("endFxn", "UFxn"));
        sizes.add(Global.newArray("deleteFxn", "UFxn"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.interfaces.IHwi.HookSet']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.interfaces.IHwi.HookSet']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.interfaces.IHwi.HookSet'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.interfaces.IHwi.StackInfo", "ti.sysbios.interfaces");
        sizes.clear();
        sizes.add(Global.newArray("hwiStackPeak", "USize"));
        sizes.add(Global.newArray("hwiStackSize", "USize"));
        sizes.add(Global.newArray("hwiStackBase", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.interfaces.IHwi.StackInfo']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.interfaces.IHwi.StackInfo']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.interfaces.IHwi.StackInfo'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void ITaskSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void ITimer$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void ITimerSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void ITimestamp$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void IIntrinsicsSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void ICache$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void ISettings$$SIZES()
    {
    }

    void IPower$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void IRomDevice$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void ISeconds$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.interfaces.ISeconds.Time", "ti.sysbios.interfaces");
        sizes.clear();
        sizes.add(Global.newArray("secsHi", "UInt32"));
        sizes.add(Global.newArray("secs", "UInt32"));
        sizes.add(Global.newArray("nsecs", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.interfaces.ISeconds.Time']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.interfaces.ISeconds.Time']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.interfaces.ISeconds.Time'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void ICore$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ICore.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ICore.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("numCores", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        }//isCFG
    }

    void IHwi$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IHwi.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.IHwi.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("dispatcherAutoNestingSupport", $$T_Bool, true, "w");
            po.addFld("dispatcherSwiSupport", $$T_Bool, $$UNDEF, "w");
            po.addFld("dispatcherTaskSupport", $$T_Bool, $$UNDEF, "w");
            po.addFld("dispatcherIrpTrackingSupport", $$T_Bool, true, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IHwi.Instance", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.IHwi.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("maskSetting", (Proto)om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption", "ti.sysbios.interfaces"), om.find("ti.sysbios.interfaces.IHwi.MaskingOption_SELF"), "w");
            po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("enableInt", $$T_Bool, true, "w");
            po.addFld("eventId", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-1"), "w");
            po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-1"), "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IHwi$$Params", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.IHwi.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("maskSetting", (Proto)om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption", "ti.sysbios.interfaces"), om.find("ti.sysbios.interfaces.IHwi.MaskingOption_SELF"), "w");
            po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("enableInt", $$T_Bool, true, "w");
            po.addFld("eventId", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-1"), "w");
            po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-1"), "w");
        }//isCFG
        // typedef IHwi.FuncPtr
        om.bind("ti.sysbios.interfaces.IHwi.FuncPtr", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"));
        // typedef IHwi.Irp
        om.bind("ti.sysbios.interfaces.IHwi.Irp", new Proto.Adr("xdc_UArg", "Pv"));
        // struct IHwi.HookSet
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IHwi$$HookSet", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.IHwi.HookSet", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("registerFxn", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"), $$UNDEF, "w");
                po.addFld("createFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle,xdc_runtime_Error_Block*)", "PFv"), $$UNDEF, "w");
                po.addFld("beginFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle)", "PFv"), $$UNDEF, "w");
                po.addFld("endFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle)", "PFv"), $$UNDEF, "w");
                po.addFld("deleteFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle)", "PFv"), $$UNDEF, "w");
        // struct IHwi.StackInfo
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IHwi$$StackInfo", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.IHwi.StackInfo", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("hwiStackPeak", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("hwiStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("hwiStackBase", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
    }

    void ITaskSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ITaskSupport.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ITaskSupport.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("defaultStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "r");
            po.addFld("stackAlignment", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "r");
        }//isCFG
        // typedef ITaskSupport.FuncPtr
        om.bind("ti.sysbios.interfaces.ITaskSupport.FuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"));
    }

    void ITimer$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ITimer.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
        if (isCFG) {
            po.addFld("supportsDynamic", $$T_Bool, false, "wh");
            po.addFld("defaultDynamic", $$T_Bool, false, "wh");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ITimer.Instance", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ITimer.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
        if (isCFG) {
            po.addFld("runMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.interfaces"), om.find("ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS"), "w");
            po.addFld("startMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.interfaces"), om.find("ti.sysbios.interfaces.ITimer.StartMode_AUTO"), "w");
            po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), null, "w");
            po.addFld("period", Proto.Elm.newCNum("(xdc_UInt32)"), 0L, "w");
            po.addFld("periodType", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.interfaces"), om.find("ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS"), "w");
            po.addFld("extFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios.interfaces"), Global.newObject("lo", 0L, "hi", 0L), "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ITimer$$Params", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ITimer.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
        if (isCFG) {
            po.addFld("runMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.interfaces"), om.find("ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS"), "w");
            po.addFld("startMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.interfaces"), om.find("ti.sysbios.interfaces.ITimer.StartMode_AUTO"), "w");
            po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), null, "w");
            po.addFld("period", Proto.Elm.newCNum("(xdc_UInt32)"), 0L, "w");
            po.addFld("periodType", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.interfaces"), om.find("ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS"), "w");
            po.addFld("extFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios.interfaces"), Global.newObject("lo", 0L, "hi", 0L), "w");
        }//isCFG
        // typedef ITimer.FuncPtr
        om.bind("ti.sysbios.interfaces.ITimer.FuncPtr", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"));
    }

    void ITimerSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ITimerSupport.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ITimerSupport.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void ITimestamp$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ITimestamp.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ITimestamp.Module", om.findStrict("xdc.runtime.ITimestampProvider.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void IIntrinsicsSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IIntrinsicsSupport.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.IIntrinsicsSupport.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void ICache$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ICache.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ICache.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void ISettings$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ISettings.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
    }

    void IPower$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IPower.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.IPower.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("idle", $$T_Bool, $$UNDEF, "w");
        }//isCFG
    }

    void IRomDevice$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IRomDevice.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.IRomDevice.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("libDir", $$T_Str, $$UNDEF, "wh");
            po.addFld("templateName", $$T_Str, $$UNDEF, "wh");
        }//isCFG
    }

    void ISeconds$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ISeconds.Module", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ISeconds.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.interfaces"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        // struct ISeconds.Time
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ISeconds$$Time", "ti.sysbios.interfaces");
        po.init("ti.sysbios.interfaces.ISeconds.Time", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("secsHi", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("secs", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("nsecs", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
    }

    void ICore$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ICore", "ti.sysbios.interfaces");
    }

    void IHwi$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.IHwi", "ti.sysbios.interfaces");
        vo.bind("HookSet$fetchDesc", Global.newObject("type", "ti.sysbios.interfaces.IHwi.HookSet", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IHwi$$HookSet", "ti.sysbios.interfaces");
        vo.bind("StackInfo$fetchDesc", Global.newObject("type", "ti.sysbios.interfaces.IHwi.StackInfo", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IHwi$$StackInfo", "ti.sysbios.interfaces");
    }

    void ITaskSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ITaskSupport", "ti.sysbios.interfaces");
    }

    void ITimer$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ITimer", "ti.sysbios.interfaces");
    }

    void ITimerSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ITimerSupport", "ti.sysbios.interfaces");
    }

    void ITimestamp$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ITimestamp", "ti.sysbios.interfaces");
    }

    void IIntrinsicsSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.IIntrinsicsSupport", "ti.sysbios.interfaces");
    }

    void ICache$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ICache", "ti.sysbios.interfaces");
    }

    void ISettings$$ROV()
    {
    }

    void IPower$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.IPower", "ti.sysbios.interfaces");
    }

    void IRomDevice$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.IRomDevice", "ti.sysbios.interfaces");
    }

    void ISeconds$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ISeconds", "ti.sysbios.interfaces");
        vo.bind("Time$fetchDesc", Global.newObject("type", "ti.sysbios.interfaces.ISeconds.Time", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ISeconds$$Time", "ti.sysbios.interfaces");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.interfaces.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.interfaces"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.sysbios.interfaces", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.interfaces");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.interfaces.");
        pkgV.bind("$vers", Global.newArray(2, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.interfaces'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.interfaces$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.interfaces$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.interfaces$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void ICore$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ICore", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ICore.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.ICore", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ICore", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ICore");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IHwi$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.IHwi", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IHwi.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.IHwi", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.sysbios.interfaces.IHwi.Instance", "ti.sysbios.interfaces"));
        vo.bind("Params", om.findStrict("ti.sysbios.interfaces.IHwi.Params", "ti.sysbios.interfaces"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.interfaces.IHwi.Params", "ti.sysbios.interfaces")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.interfaces.IHwi.Handle", "ti.sysbios.interfaces"));
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.interfaces.IHwi.FuncPtr", "ti.sysbios.interfaces"));
        vo.bind("Irp", om.findStrict("ti.sysbios.interfaces.IHwi.Irp", "ti.sysbios.interfaces"));
        vo.bind("HookSet", om.findStrict("ti.sysbios.interfaces.IHwi.HookSet", "ti.sysbios.interfaces"));
        tdefs.add(om.findStrict("ti.sysbios.interfaces.IHwi.HookSet", "ti.sysbios.interfaces"));
        vo.bind("MaskingOption", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption", "ti.sysbios.interfaces"));
        vo.bind("StackInfo", om.findStrict("ti.sysbios.interfaces.IHwi.StackInfo", "ti.sysbios.interfaces"));
        tdefs.add(om.findStrict("ti.sysbios.interfaces.IHwi.StackInfo", "ti.sysbios.interfaces"));
        vo.bind("MaskingOption_NONE", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption_NONE", "ti.sysbios.interfaces"));
        vo.bind("MaskingOption_ALL", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption_ALL", "ti.sysbios.interfaces"));
        vo.bind("MaskingOption_SELF", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption_SELF", "ti.sysbios.interfaces"));
        vo.bind("MaskingOption_BITMASK", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption_BITMASK", "ti.sysbios.interfaces"));
        vo.bind("MaskingOption_LOWER", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption_LOWER", "ti.sysbios.interfaces"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IHwi", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IHwi");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void ITaskSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ITaskSupport", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ITaskSupport.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.ITaskSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.interfaces.ITaskSupport.FuncPtr", "ti.sysbios.interfaces"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITaskSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITaskSupport");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void ITimer$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ITimer", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.ITimer", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.sysbios.interfaces.ITimer.Instance", "ti.sysbios.interfaces"));
        vo.bind("Params", om.findStrict("ti.sysbios.interfaces.ITimer.Params", "ti.sysbios.interfaces"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.interfaces.ITimer.Params", "ti.sysbios.interfaces")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.interfaces.ITimer.Handle", "ti.sysbios.interfaces"));
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.interfaces.ITimer.FuncPtr", "ti.sysbios.interfaces"));
        vo.bind("StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.interfaces"));
        vo.bind("RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.interfaces"));
        vo.bind("Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.interfaces"));
        vo.bind("PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.interfaces"));
        vo.bind("StartMode_AUTO", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode_AUTO", "ti.sysbios.interfaces"));
        vo.bind("StartMode_USER", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode_USER", "ti.sysbios.interfaces"));
        vo.bind("RunMode_CONTINUOUS", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS", "ti.sysbios.interfaces"));
        vo.bind("RunMode_ONESHOT", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_ONESHOT", "ti.sysbios.interfaces"));
        vo.bind("RunMode_DYNAMIC", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_DYNAMIC", "ti.sysbios.interfaces"));
        vo.bind("Status_INUSE", om.findStrict("ti.sysbios.interfaces.ITimer.Status_INUSE", "ti.sysbios.interfaces"));
        vo.bind("Status_FREE", om.findStrict("ti.sysbios.interfaces.ITimer.Status_FREE", "ti.sysbios.interfaces"));
        vo.bind("PeriodType_MICROSECS", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS", "ti.sysbios.interfaces"));
        vo.bind("PeriodType_COUNTS", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType_COUNTS", "ti.sysbios.interfaces"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITimer", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITimer");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void ITimerSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ITimerSupport", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ITimerSupport.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.ITimerSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITimerSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITimerSupport");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void ITimestamp$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ITimestamp", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ITimestamp.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.ITimestamp", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITimestamp", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITimestamp");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IIntrinsicsSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.IIntrinsicsSupport", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IIntrinsicsSupport.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.IIntrinsicsSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IIntrinsicsSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IIntrinsicsSupport");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void ICache$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ICache", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ICache.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.ICache", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Type", om.findStrict("ti.sysbios.interfaces.ICache.Type", "ti.sysbios.interfaces"));
        vo.bind("Type_L1P", om.findStrict("ti.sysbios.interfaces.ICache.Type_L1P", "ti.sysbios.interfaces"));
        vo.bind("Type_L1D", om.findStrict("ti.sysbios.interfaces.ICache.Type_L1D", "ti.sysbios.interfaces"));
        vo.bind("Type_L1", om.findStrict("ti.sysbios.interfaces.ICache.Type_L1", "ti.sysbios.interfaces"));
        vo.bind("Type_L2P", om.findStrict("ti.sysbios.interfaces.ICache.Type_L2P", "ti.sysbios.interfaces"));
        vo.bind("Type_L2D", om.findStrict("ti.sysbios.interfaces.ICache.Type_L2D", "ti.sysbios.interfaces"));
        vo.bind("Type_L2", om.findStrict("ti.sysbios.interfaces.ICache.Type_L2", "ti.sysbios.interfaces"));
        vo.bind("Type_ALLP", om.findStrict("ti.sysbios.interfaces.ICache.Type_ALLP", "ti.sysbios.interfaces"));
        vo.bind("Type_ALLD", om.findStrict("ti.sysbios.interfaces.ICache.Type_ALLD", "ti.sysbios.interfaces"));
        vo.bind("Type_ALL", om.findStrict("ti.sysbios.interfaces.ICache.Type_ALL", "ti.sysbios.interfaces"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ICache", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ICache");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void ISettings$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ISettings", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ISettings.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.ISettings", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ISettings", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ISettings");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IPower$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.IPower", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IPower.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.IPower", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IPower", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IPower");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IRomDevice$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.IRomDevice", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.IRomDevice.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.IRomDevice", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IRomDevice", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IRomDevice");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void ISeconds$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.interfaces.ISeconds", "ti.sysbios.interfaces");
        po = (Proto.Obj)om.findStrict("ti.sysbios.interfaces.ISeconds.Module", "ti.sysbios.interfaces");
        vo.init2(po, "ti.sysbios.interfaces.ISeconds", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.interfaces", "ti.sysbios.interfaces"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Time", om.findStrict("ti.sysbios.interfaces.ISeconds.Time", "ti.sysbios.interfaces"));
        tdefs.add(om.findStrict("ti.sysbios.interfaces.ISeconds.Time", "ti.sysbios.interfaces"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ISeconds", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ISeconds");
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
        ((Value.Obj)om.getv("ti.sysbios.interfaces.ICore")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.IHwi")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.ITaskSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.ITimer")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.ITimerSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.ITimestamp")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.IIntrinsicsSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.ICache")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.ISettings")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.IPower")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.IRomDevice")).bless();
        ((Value.Obj)om.getv("ti.sysbios.interfaces.ISeconds")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.interfaces")).add(pkgV);
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
        ICore$$OBJECTS();
        IHwi$$OBJECTS();
        ITaskSupport$$OBJECTS();
        ITimer$$OBJECTS();
        ITimerSupport$$OBJECTS();
        ITimestamp$$OBJECTS();
        IIntrinsicsSupport$$OBJECTS();
        ICache$$OBJECTS();
        ISettings$$OBJECTS();
        IPower$$OBJECTS();
        IRomDevice$$OBJECTS();
        ISeconds$$OBJECTS();
        ICore$$CONSTS();
        IHwi$$CONSTS();
        ITaskSupport$$CONSTS();
        ITimer$$CONSTS();
        ITimerSupport$$CONSTS();
        ITimestamp$$CONSTS();
        IIntrinsicsSupport$$CONSTS();
        ICache$$CONSTS();
        ISettings$$CONSTS();
        IPower$$CONSTS();
        IRomDevice$$CONSTS();
        ISeconds$$CONSTS();
        ICore$$CREATES();
        IHwi$$CREATES();
        ITaskSupport$$CREATES();
        ITimer$$CREATES();
        ITimerSupport$$CREATES();
        ITimestamp$$CREATES();
        IIntrinsicsSupport$$CREATES();
        ICache$$CREATES();
        ISettings$$CREATES();
        IPower$$CREATES();
        IRomDevice$$CREATES();
        ISeconds$$CREATES();
        ICore$$FUNCTIONS();
        IHwi$$FUNCTIONS();
        ITaskSupport$$FUNCTIONS();
        ITimer$$FUNCTIONS();
        ITimerSupport$$FUNCTIONS();
        ITimestamp$$FUNCTIONS();
        IIntrinsicsSupport$$FUNCTIONS();
        ICache$$FUNCTIONS();
        ISettings$$FUNCTIONS();
        IPower$$FUNCTIONS();
        IRomDevice$$FUNCTIONS();
        ISeconds$$FUNCTIONS();
        ICore$$SIZES();
        IHwi$$SIZES();
        ITaskSupport$$SIZES();
        ITimer$$SIZES();
        ITimerSupport$$SIZES();
        ITimestamp$$SIZES();
        IIntrinsicsSupport$$SIZES();
        ICache$$SIZES();
        ISettings$$SIZES();
        IPower$$SIZES();
        IRomDevice$$SIZES();
        ISeconds$$SIZES();
        ICore$$TYPES();
        IHwi$$TYPES();
        ITaskSupport$$TYPES();
        ITimer$$TYPES();
        ITimerSupport$$TYPES();
        ITimestamp$$TYPES();
        IIntrinsicsSupport$$TYPES();
        ICache$$TYPES();
        ISettings$$TYPES();
        IPower$$TYPES();
        IRomDevice$$TYPES();
        ISeconds$$TYPES();
        if (isROV) {
            ICore$$ROV();
            IHwi$$ROV();
            ITaskSupport$$ROV();
            ITimer$$ROV();
            ITimerSupport$$ROV();
            ITimestamp$$ROV();
            IIntrinsicsSupport$$ROV();
            ICache$$ROV();
            ISettings$$ROV();
            IPower$$ROV();
            IRomDevice$$ROV();
            ISeconds$$ROV();
        }//isROV
        $$SINGLETONS();
        ICore$$SINGLETONS();
        IHwi$$SINGLETONS();
        ITaskSupport$$SINGLETONS();
        ITimer$$SINGLETONS();
        ITimerSupport$$SINGLETONS();
        ITimestamp$$SINGLETONS();
        IIntrinsicsSupport$$SINGLETONS();
        ICache$$SINGLETONS();
        ISettings$$SINGLETONS();
        IPower$$SINGLETONS();
        IRomDevice$$SINGLETONS();
        ISeconds$$SINGLETONS();
        $$INITIALIZATION();
    }
}
