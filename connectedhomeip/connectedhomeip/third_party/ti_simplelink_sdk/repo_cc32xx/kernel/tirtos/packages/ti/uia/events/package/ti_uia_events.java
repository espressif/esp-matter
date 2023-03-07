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

public class ti_uia_events
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
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.uia.events.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.uia.events", new Value.Obj("ti.uia.events", pkgP));
    }

    void DvtTypes$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.DvtTypes.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.DvtTypes", new Value.Obj("ti.uia.events.DvtTypes", po));
        pkgV.bind("DvtTypes", vo);
        // decls 
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType", new Proto.Enm("ti.uia.events.DvtTypes.DvtAnalysisType"));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc", new Proto.Enm("ti.uia.events.DvtTypes.DvtDataDesc"));
        spo = (Proto.Obj)om.bind("ti.uia.events.DvtTypes$$ParamDescriptor", new Proto.Obj());
        om.bind("ti.uia.events.DvtTypes.ParamDescriptor", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.events.DvtTypes$$MetaEventDescriptor", new Proto.Obj());
        om.bind("ti.uia.events.DvtTypes.MetaEventDescriptor", new Proto.Str(spo, false));
    }

    void IUIAEvent$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.IUIAEvent.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.IUIAEvent", new Value.Obj("ti.uia.events.IUIAEvent", po));
        pkgV.bind("IUIAEvent", vo);
        // decls 
    }

    void IUIACtx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.IUIACtx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.IUIACtx", new Value.Obj("ti.uia.events.IUIACtx", po));
        pkgV.bind("IUIACtx", vo);
        // decls 
    }

    void UIAAppCtx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAAppCtx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAAppCtx", new Value.Obj("ti.uia.events.UIAAppCtx", po));
        pkgV.bind("UIAAppCtx", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.events.UIAAppCtx$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.events.UIAAppCtx.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.events.UIAAppCtx$$Module_State", new Proto.Obj());
        om.bind("ti.uia.events.UIAAppCtx.Module_State", new Proto.Str(spo, false));
    }

    void UIABenchmark$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIABenchmark.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIABenchmark", new Value.Obj("ti.uia.events.UIABenchmark", po));
        pkgV.bind("UIABenchmark", vo);
        // decls 
    }

    void UIAChanCtx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAChanCtx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAChanCtx", new Value.Obj("ti.uia.events.UIAChanCtx", po));
        pkgV.bind("UIAChanCtx", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.events.UIAChanCtx$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.events.UIAChanCtx.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.events.UIAChanCtx$$Module_State", new Proto.Obj());
        om.bind("ti.uia.events.UIAChanCtx.Module_State", new Proto.Str(spo, false));
    }

    void UIAErr$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAErr.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAErr", new Value.Obj("ti.uia.events.UIAErr", po));
        pkgV.bind("UIAErr", vo);
        // decls 
    }

    void UIAEvt$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAEvt.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAEvt", new Value.Obj("ti.uia.events.UIAEvt", po));
        pkgV.bind("UIAEvt", vo);
        // decls 
    }

    void UIAFrameCtx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAFrameCtx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAFrameCtx", new Value.Obj("ti.uia.events.UIAFrameCtx", po));
        pkgV.bind("UIAFrameCtx", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.events.UIAFrameCtx$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.events.UIAFrameCtx.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.events.UIAFrameCtx$$Module_State", new Proto.Obj());
        om.bind("ti.uia.events.UIAFrameCtx.Module_State", new Proto.Str(spo, false));
    }

    void UIAMessage$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAMessage.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAMessage", new Value.Obj("ti.uia.events.UIAMessage", po));
        pkgV.bind("UIAMessage", vo);
        // decls 
    }

    void UIAHWICtx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAHWICtx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAHWICtx", new Value.Obj("ti.uia.events.UIAHWICtx", po));
        pkgV.bind("UIAHWICtx", vo);
        // decls 
    }

    void UIAProfile$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAProfile.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAProfile", new Value.Obj("ti.uia.events.UIAProfile", po));
        pkgV.bind("UIAProfile", vo);
        // decls 
    }

    void UIARoundtrip$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIARoundtrip.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIARoundtrip", new Value.Obj("ti.uia.events.UIARoundtrip", po));
        pkgV.bind("UIARoundtrip", vo);
        // decls 
    }

    void UIASWICtx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIASWICtx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIASWICtx", new Value.Obj("ti.uia.events.UIASWICtx", po));
        pkgV.bind("UIASWICtx", vo);
        // decls 
    }

    void UIASnapshot$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIASnapshot.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIASnapshot", new Value.Obj("ti.uia.events.UIASnapshot", po));
        pkgV.bind("UIASnapshot", vo);
        // decls 
    }

    void UIAStatistic$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAStatistic.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAStatistic", new Value.Obj("ti.uia.events.UIAStatistic", po));
        pkgV.bind("UIAStatistic", vo);
        // decls 
    }

    void UIASync$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIASync.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIASync", new Value.Obj("ti.uia.events.UIASync", po));
        pkgV.bind("UIASync", vo);
        // decls 
    }

    void UIAThreadCtx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAThreadCtx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAThreadCtx", new Value.Obj("ti.uia.events.UIAThreadCtx", po));
        pkgV.bind("UIAThreadCtx", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.events.UIAThreadCtx$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.events.UIAThreadCtx.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.events.UIAThreadCtx$$Module_State", new Proto.Obj());
        om.bind("ti.uia.events.UIAThreadCtx.Module_State", new Proto.Str(spo, false));
    }

    void UIAUserCtx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.UIAUserCtx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.UIAUserCtx", new Value.Obj("ti.uia.events.UIAUserCtx", po));
        pkgV.bind("UIAUserCtx", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.events.UIAUserCtx$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.events.UIAUserCtx.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.events.UIAUserCtx$$Module_State", new Proto.Obj());
        om.bind("ti.uia.events.UIAUserCtx.Module_State", new Proto.Str(spo, false));
    }

    void IUIAMetaProvider$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.events.IUIAMetaProvider.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.events.IUIAMetaProvider", new Value.Obj("ti.uia.events.IUIAMetaProvider", po));
        pkgV.bind("IUIAMetaProvider", vo);
        // decls 
    }

    void DvtTypes$$CONSTS()
    {
        // module DvtTypes
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_START", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_START", 0));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_STOP", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_STOP", 1));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_STARTSTOP", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_STARTSTOP", 2));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_FUNCTIONENTRY", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_FUNCTIONENTRY", 3));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_FUNCTIONEXIT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_FUNCTIONEXIT", 4));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_SYNCPOINT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_SYNCPOINT", 5));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_CONTEXTCHANGE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_CONTEXTCHANGE", 6));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_STATECHANGE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_STATECHANGE", 7));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_MESSAGEMONITOR", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_MESSAGEMONITOR", 8));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_STATISTIC", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_STATISTIC", 9));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_SEMAPHORE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_SEMAPHORE", 10));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_LOCK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_LOCK", 11));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_EVENTMARKER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_EVENTMARKER", 12));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_MEMORYSNAPSHOT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_MEMORYSNAPSHOT", 13));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_STRINGSNAPSHOT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_STRINGSNAPSHOT", 14));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_NAMESNAPSHOT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_NAMESNAPSHOT", 15));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_STACKSNAPSHOT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_STACKSNAPSHOT", 16));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_REGISTERSNAPSHOT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_REGISTERSNAPSHOT", 17));
        om.bind("ti.uia.events.DvtTypes.DvtAnalysisType_CUSTOM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtAnalysisType_CUSTOM", 18));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP32", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP32", 0));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP64_MSW", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP64_MSW", 1));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP64_LSW", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP64_LSW", 2));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP_CTIME", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP_CTIME", 3));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_COOKIE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_COOKIE", 4));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_SIZE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_SIZE", 5));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_PROGRAMADRS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_PROGRAMADRS", 6));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS", 7));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_DATAADRS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_DATAADRS", 8));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_STRINGADRS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_STRINGADRS", 9));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE", 10));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_INITIATOR", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_INITIATOR", 11));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_PROCESSID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_PROCESSID", 12));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_THREADID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_THREADID", 13));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_TASKID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_TASKID", 14));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_CHANNELID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_CHANNELID", 15));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_PACKETID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_PACKETID", 16));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_FRAMEID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_FRAMEID", 17));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_APPID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_APPID", 18));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_HWIID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_HWIID", 19));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_SWIID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_SWIID", 20));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_STATEID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_STATEID", 21));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_STATESTRINGADRS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_STATESTRINGADRS", 22));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_COUNTER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_COUNTER", 23));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_VALUE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_VALUE", 24));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_FLAG", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_FLAG", 25));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_SEMAPHORE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_SEMAPHORE", 26));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_RETURNVALUE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_RETURNVALUE", 27));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_PARAMVALUE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_PARAMVALUE", 28));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_EXPECTEDVALUE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_EXPECTEDVALUE", 29));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_TESTNUMBER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_TESTNUMBER", 30));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_RESULT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_RESULT", 31));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_ERRORCODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_ERRORCODE", 32));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_EVENTCODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_EVENTCODE", 33));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR", 34));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_LINENUM", 35));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR", 36));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_GLOBALSYMBOLADRS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_GLOBALSYMBOLADRS", 37));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_ENUMVALUE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_ENUMVALUE", 38));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_LENGTHINMAUS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_LENGTHINMAUS", 39));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_DATAARRAY", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_DATAARRAY", 40));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_REFERENCEID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_REFERENCEID", 41));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_SNAPSHOTID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_SNAPSHOTID", 42));
        om.bind("ti.uia.events.DvtTypes.DvtDataDesc_CUSTOM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), "ti.uia.events.DvtTypes.DvtDataDesc_CUSTOM", 43));
    }

    void IUIAEvent$$CONSTS()
    {
        // interface IUIAEvent
    }

    void IUIACtx$$CONSTS()
    {
        // interface IUIACtx
    }

    void UIAAppCtx$$CONSTS()
    {
        // module UIAAppCtx
        om.bind("ti.uia.events.UIAAppCtx.getCtxId", new Extern("ti_uia_events_UIAAppCtx_getCtxId__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.uia.events.UIAAppCtx.getEnableOnValue", new Extern("ti_uia_events_UIAAppCtx_getEnableOnValue__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.uia.events.UIAAppCtx.setEnableOnValue", new Extern("ti_uia_events_UIAAppCtx_setEnableOnValue__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.uia.events.UIAAppCtx.isLoggingEnabled", new Extern("ti_uia_events_UIAAppCtx_isLoggingEnabled__E", "xdc_Bool(*)(xdc_UInt)", true, false));
        om.bind("ti.uia.events.UIAAppCtx.setOldValue", new Extern("ti_uia_events_UIAAppCtx_setOldValue__E", "xdc_UInt(*)(xdc_UInt)", true, false));
    }

    void UIABenchmark$$CONSTS()
    {
        // module UIABenchmark
    }

    void UIAChanCtx$$CONSTS()
    {
        // module UIAChanCtx
        om.bind("ti.uia.events.UIAChanCtx.getCtxId", new Extern("ti_uia_events_UIAChanCtx_getCtxId__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.uia.events.UIAChanCtx.getEnableOnValue", new Extern("ti_uia_events_UIAChanCtx_getEnableOnValue__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.uia.events.UIAChanCtx.setEnableOnValue", new Extern("ti_uia_events_UIAChanCtx_setEnableOnValue__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.uia.events.UIAChanCtx.isLoggingEnabled", new Extern("ti_uia_events_UIAChanCtx_isLoggingEnabled__E", "xdc_Bool(*)(xdc_UInt)", true, false));
        om.bind("ti.uia.events.UIAChanCtx.setOldValue", new Extern("ti_uia_events_UIAChanCtx_setOldValue__E", "xdc_UInt(*)(xdc_UInt)", true, false));
    }

    void UIAErr$$CONSTS()
    {
        // module UIAErr
    }

    void UIAEvt$$CONSTS()
    {
        // module UIAEvt
    }

    void UIAFrameCtx$$CONSTS()
    {
        // module UIAFrameCtx
        om.bind("ti.uia.events.UIAFrameCtx.getCtxId", new Extern("ti_uia_events_UIAFrameCtx_getCtxId__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.uia.events.UIAFrameCtx.getEnableOnValue", new Extern("ti_uia_events_UIAFrameCtx_getEnableOnValue__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.uia.events.UIAFrameCtx.setEnableOnValue", new Extern("ti_uia_events_UIAFrameCtx_setEnableOnValue__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.uia.events.UIAFrameCtx.isLoggingEnabled", new Extern("ti_uia_events_UIAFrameCtx_isLoggingEnabled__E", "xdc_Bool(*)(xdc_UInt)", true, false));
        om.bind("ti.uia.events.UIAFrameCtx.setOldValue", new Extern("ti_uia_events_UIAFrameCtx_setOldValue__E", "xdc_UInt(*)(xdc_UInt)", true, false));
    }

    void UIAMessage$$CONSTS()
    {
        // module UIAMessage
    }

    void UIAHWICtx$$CONSTS()
    {
        // module UIAHWICtx
    }

    void UIAProfile$$CONSTS()
    {
        // module UIAProfile
    }

    void UIARoundtrip$$CONSTS()
    {
        // module UIARoundtrip
    }

    void UIASWICtx$$CONSTS()
    {
        // module UIASWICtx
    }

    void UIASnapshot$$CONSTS()
    {
        // module UIASnapshot
    }

    void UIAStatistic$$CONSTS()
    {
        // module UIAStatistic
    }

    void UIASync$$CONSTS()
    {
        // module UIASync
    }

    void UIAThreadCtx$$CONSTS()
    {
        // module UIAThreadCtx
        om.bind("ti.uia.events.UIAThreadCtx.getCtxId", new Extern("ti_uia_events_UIAThreadCtx_getCtxId__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.uia.events.UIAThreadCtx.getEnableOnValue", new Extern("ti_uia_events_UIAThreadCtx_getEnableOnValue__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.uia.events.UIAThreadCtx.setEnableOnValue", new Extern("ti_uia_events_UIAThreadCtx_setEnableOnValue__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.uia.events.UIAThreadCtx.isLoggingEnabled", new Extern("ti_uia_events_UIAThreadCtx_isLoggingEnabled__E", "xdc_Bool(*)(xdc_UInt)", true, false));
        om.bind("ti.uia.events.UIAThreadCtx.setOldValue", new Extern("ti_uia_events_UIAThreadCtx_setOldValue__E", "xdc_UInt(*)(xdc_UInt)", true, false));
    }

    void UIAUserCtx$$CONSTS()
    {
        // module UIAUserCtx
        om.bind("ti.uia.events.UIAUserCtx.getCtxId", new Extern("ti_uia_events_UIAUserCtx_getCtxId__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.uia.events.UIAUserCtx.getEnableOnValue", new Extern("ti_uia_events_UIAUserCtx_getEnableOnValue__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.uia.events.UIAUserCtx.setEnableOnValue", new Extern("ti_uia_events_UIAUserCtx_setEnableOnValue__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.uia.events.UIAUserCtx.isLoggingEnabled", new Extern("ti_uia_events_UIAUserCtx_isLoggingEnabled__E", "xdc_Bool(*)(xdc_UInt)", true, false));
        om.bind("ti.uia.events.UIAUserCtx.setOldValue", new Extern("ti_uia_events_UIAUserCtx_setOldValue__E", "xdc_UInt(*)(xdc_UInt)", true, false));
    }

    void IUIAMetaProvider$$CONSTS()
    {
        // interface IUIAMetaProvider
    }

    void DvtTypes$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IUIAEvent$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IUIACtx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAAppCtx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIABenchmark$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAChanCtx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAErr$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAEvt$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAFrameCtx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAMessage$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAHWICtx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAProfile$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIARoundtrip$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIASWICtx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIASnapshot$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAStatistic$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIASync$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAThreadCtx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAUserCtx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IUIAMetaProvider$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void DvtTypes$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IUIAEvent$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IUIACtx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAAppCtx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIABenchmark$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAChanCtx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAErr$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAEvt$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAFrameCtx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAMessage$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAHWICtx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAProfile$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIARoundtrip$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIASWICtx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIASnapshot$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAStatistic$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIASync$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAThreadCtx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void UIAUserCtx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IUIAMetaProvider$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn IUIAMetaProvider.writeUIAMetaData
        fxn = (Proto.Fxn)om.bind("ti.uia.events.IUIAMetaProvider$$writeUIAMetaData", new Proto.Fxn(om.findStrict("ti.uia.events.IUIAMetaProvider.Module", "ti.uia.events"), null, 0, -1, false));
    }

    void DvtTypes$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void IUIAEvent$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void IUIACtx$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIAAppCtx$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.events.UIAAppCtx.Module_State", "ti.uia.events");
        sizes.clear();
        sizes.add(Global.newArray("mLastValue", "UInt"));
        sizes.add(Global.newArray("mEnableOnValue", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.events.UIAAppCtx.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.events.UIAAppCtx.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.events.UIAAppCtx.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void UIABenchmark$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIAChanCtx$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.events.UIAChanCtx.Module_State", "ti.uia.events");
        sizes.clear();
        sizes.add(Global.newArray("mLastValue", "UInt"));
        sizes.add(Global.newArray("mEnableOnValue", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.events.UIAChanCtx.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.events.UIAChanCtx.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.events.UIAChanCtx.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void UIAErr$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIAEvt$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIAFrameCtx$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.events.UIAFrameCtx.Module_State", "ti.uia.events");
        sizes.clear();
        sizes.add(Global.newArray("mLastValue", "UInt"));
        sizes.add(Global.newArray("mEnableOnValue", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.events.UIAFrameCtx.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.events.UIAFrameCtx.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.events.UIAFrameCtx.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void UIAMessage$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIAHWICtx$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIAProfile$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIARoundtrip$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIASWICtx$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIASnapshot$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIAStatistic$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIASync$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIAThreadCtx$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.events.UIAThreadCtx.Module_State", "ti.uia.events");
        sizes.clear();
        sizes.add(Global.newArray("mLastValue", "UInt"));
        sizes.add(Global.newArray("mEnableOnValue", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.events.UIAThreadCtx.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.events.UIAThreadCtx.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.events.UIAThreadCtx.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void UIAUserCtx$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.events.UIAUserCtx.Module_State", "ti.uia.events");
        sizes.clear();
        sizes.add(Global.newArray("mLastValue", "UInt"));
        sizes.add(Global.newArray("mEnableOnValue", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.events.UIAUserCtx.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.events.UIAUserCtx.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.events.UIAUserCtx.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void IUIAMetaProvider$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void DvtTypes$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.DvtTypes.Module", "ti.uia.events");
        po.init("ti.uia.events.DvtTypes.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        // struct DvtTypes.ParamDescriptor
        po = (Proto.Obj)om.findStrict("ti.uia.events.DvtTypes$$ParamDescriptor", "ti.uia.events");
        po.init("ti.uia.events.DvtTypes.ParamDescriptor", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("dataDesc", (Proto)om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"), $$UNDEF, "w");
                po.addFld("strCustomDataDesc", $$T_Str, $$UNDEF, "w");
                po.addFld("dataTypeName", $$T_Str, $$UNDEF, "w");
                po.addFld("units", $$T_Str, $$UNDEF, "w");
                po.addFld("isHidden", $$T_Bool, $$UNDEF, "w");
                po.addFld("lsb", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // struct DvtTypes.MetaEventDescriptor
        po = (Proto.Obj)om.findStrict("ti.uia.events.DvtTypes$$MetaEventDescriptor", "ti.uia.events");
        po.init("ti.uia.events.DvtTypes.MetaEventDescriptor", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("versionId", $$T_Str, $$UNDEF, "w");
                po.addFld("analysisType", (Proto)om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"), $$UNDEF, "w");
                po.addFld("strCustomAnalysisType", $$T_Str, $$UNDEF, "w");
                po.addFld("displayText", $$T_Str, $$UNDEF, "w");
                po.addFld("tooltipText", $$T_Str, $$UNDEF, "w");
                po.addFld("isMultiEventRecordItem", $$T_Bool, $$UNDEF, "w");
                po.addFld("multiEventRecordItemIndex", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("multiEventRecordLength", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numParameters", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("paramInfo", new Proto.Arr((Proto)om.findStrict("ti.uia.events.DvtTypes.ParamDescriptor", "ti.uia.events"), false), $$DEFAULT, "w");
    }

    void IUIAEvent$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events");
        po.init("ti.uia.events.IUIAEvent.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void IUIACtx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.IUIACtx.Module", "ti.uia.events");
        po.init("ti.uia.events.IUIACtx.Module", om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("isLoggingEnabledFxn", new Proto.Adr("xdc_Bool(*)(xdc_Int)", "PFb"), null, "w");
            po.addFld("ENABLEMASK", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
            po.addFld("SYNCID", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
        }//isCFG
        // typedef IUIACtx.IsLoggingEnabledFxn
        om.bind("ti.uia.events.IUIACtx.IsLoggingEnabledFxn", new Proto.Adr("xdc_Bool(*)(xdc_Int)", "PFb"));
    }

    void UIAAppCtx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/events/UIAAppCtx.xs");
        om.bind("ti.uia.events.UIAAppCtx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAAppCtx.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAAppCtx.Module", om.findStrict("ti.uia.events.IUIACtx.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.events"), $$UNDEF, "wh");
            po.addFld("ctxChg", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "AppID Ctx Change at %$F [Prev. AppID=0x%x] %$S"), "w");
            po.addFld("metaEventAppCtxChg", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_CONTEXTCHANGE"), "displayText", "AppID Ctx Change", "tooltipText", "Application ID Context Change", "numParameters", 5L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "__FILE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "__LINE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Prev. App ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_APPID"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "New App ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_APPID"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.events.UIAAppCtx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.events.UIAAppCtx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.events.UIAAppCtx$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.events.UIAAppCtx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct UIAAppCtx.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAAppCtx$$ModuleView", "ti.uia.events");
        po.init("ti.uia.events.UIAAppCtx.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("mLastValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mEnableOnValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct UIAAppCtx.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAAppCtx$$Module_State", "ti.uia.events");
        po.init("ti.uia.events.UIAAppCtx.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("mLastValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mEnableOnValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
    }

    void UIABenchmark$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.UIABenchmark.Module", "ti.uia.events");
        po.init("ti.uia.events.UIABenchmark.Module", om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("start", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Start: %$S "), "w");
            po.addFld("metaEventStart", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "Start", "tooltipText", "Marks the start of analysis", "numParameters", 1L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stop", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Stop: %$S "), "w");
            po.addFld("metaEventStop", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "Stop", "tooltipText", "Marks the end of analysis", "numParameters", 1L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false)})), "wh");
            po.addFld("startInstance", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "StartInstance: %$S "), "w");
            po.addFld("metaEventStartInstance", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "StartInstance", "tooltipText", "Marks the start of analysis for a module instance", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "InstanceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stopInstance", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "StopInstance: %$S "), "w");
            po.addFld("metaEventStopInstance", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "StopInstance", "tooltipText", "Marks the end of analysis for a module instance", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "InstanceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("startInstanceWithAdrs", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "StartInstanceWithAdrs: %$S"), "w");
            po.addFld("metaEventStartInstanceWithAdrs", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "StartInstanceWithAdrs", "tooltipText", "Marks the start of analysis for a module instance", "numParameters", 3L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "InstanceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "FunctionAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stopInstanceWithAdrs", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "StopInstanceWithAdrs: %$S"), "w");
            po.addFld("metaEventStopInstanceWithAdrs", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "StopInstanceWithAdrs", "tooltipText", "Marks the end of analysis for a module instance", "numParameters", 3L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "Handle", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "FunctionAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("startInstanceWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "StartInstanceWithStr: %$S"), "w");
            po.addFld("metaEventStartInstanceWithStr", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "StartInstanceWithStr", "tooltipText", "Marks the start of analysis for a module instance", "numParameters", 3L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "InstanceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "FunctionAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stopInstanceWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "StopInstanceWithStr: %$S"), "w");
            po.addFld("metaEventStopInstanceWithStr", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "StopInstanceWithStr", "tooltipText", "Marks the end of analysis for a module instance", "numParameters", 3L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "InstanceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "String", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_STRINGADRS"), "dataTypeName", "String", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
    }

    void UIAChanCtx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/events/UIAChanCtx.xs");
        om.bind("ti.uia.events.UIAChanCtx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAChanCtx.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAChanCtx.Module", om.findStrict("ti.uia.events.IUIACtx.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.events"), $$UNDEF, "wh");
            po.addFld("ctxChg", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Channel Ctx Change at %$F [Prev. chan ID=0x%x] %$S"), "w");
            po.addFld("metaEventChanCtxChg", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_CONTEXTCHANGE"), "displayText", "Channel Ctx Change", "tooltipText", "Channel ID Context Change", "numParameters", 5L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "__FILE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "__LINE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Prev. Chan ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_CHANNELID"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "New Chan ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_CHANNELID"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.events.UIAChanCtx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.events.UIAChanCtx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.events.UIAChanCtx$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.events.UIAChanCtx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct UIAChanCtx.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAChanCtx$$ModuleView", "ti.uia.events");
        po.init("ti.uia.events.UIAChanCtx.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("mLastValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mEnableOnValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct UIAChanCtx.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAChanCtx$$Module_State", "ti.uia.events");
        po.init("ti.uia.events.UIAChanCtx.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("mLastValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mEnableOnValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
    }

    void UIAErr$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAErr.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAErr.Module", om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("error", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: ErrorCode:0x%x"), "w");
            po.addFld("errorWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: ErrorCode:0x%x. %$S"), "w");
            po.addFld("hwError", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "HW ERROR: ErrorCode:0x%x"), "w");
            po.addFld("hwErrorWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "HW ERROR: ErrorCode:0x%x. %$S"), "w");
            po.addFld("fatal", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL1"), "msg", "FATAL ERROR: ErrorCode:0x%x"), "w");
            po.addFld("fatalWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL1"), "msg", "FATAL ERROR: ErrorCode:0x%x. %$S"), "w");
            po.addFld("critical", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL2"), "msg", "CRITICAL ERROR: ErrorCode:0x%x"), "w");
            po.addFld("criticalWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL2"), "msg", "CRITICAL ERROR: ErrorCode:0x%x. %$S"), "w");
            po.addFld("exception", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Exception at %$F."), "w");
            po.addFld("uncaughtException", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Uncaught Exception at %$F."), "w");
            po.addFld("nullPointerException", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Null Pointer Exception at %$F."), "w");
            po.addFld("unexpectedInterrupt", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Unexpected Interrupt at %$F."), "w");
            po.addFld("memoryAccessFault", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Memory Access Fault at %$F. [ADRS]0x%x"), "w");
            po.addFld("securityException", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Security Exception at %$F."), "w");
            po.addFld("divisionByZero", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Division by zero at %$F."), "w");
            po.addFld("overflowException", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Overflow exception at %$F."), "w");
            po.addFld("indexOutOfRange", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Index out of range at %$F. [INDEX]0x%x"), "w");
            po.addFld("notImplemented", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Attempt to access feature that is not implemented at %$F."), "w");
            po.addFld("stackOverflow", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL2"), "msg", "ERROR: Stack Overflow detected at %$F."), "w");
            po.addFld("illegalInstruction", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Illegal Instruction executed at %$F."), "w");
            po.addFld("entryPointNotFound", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Entry Point Not Found at %$F."), "w");
            po.addFld("moduleNotFound", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Module not found at %$F. [MODULE_ID]0x%x."), "w");
            po.addFld("floatingPointError", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Floating Point Error at %$F."), "w");
            po.addFld("invalidParameter", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL3"), "msg", "ERROR: Invalid Parameter at %$F. [ParamNum]%d [ParamValue]0x%x"), "w");
        }//isCFG
    }

    void UIAEvt$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAEvt.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAEvt.Module", om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("warning", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL4"), "msg", "WARNING: EventCode:0x%x"), "w");
            po.addFld("warningWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0080L, "level", om.find("xdc.runtime.Diags.LEVEL4"), "msg", "WARNING: EventCode:0x%x. %$S"), "w");
            po.addFld("info", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x4000L, "msg", "INFO: EventCode: 0x%x"), "w");
            po.addFld("infoWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x4000L, "msg", "INFO: EventCode:0x%x.  %$S"), "w");
            po.addFld("detail", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x4000L, "level", om.find("xdc.runtime.Diags.LEVEL4"), "msg", "DETAIL: EventCode:0x%x"), "w");
            po.addFld("detailWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x4000L, "level", om.find("xdc.runtime.Diags.LEVEL4"), "msg", "DETAIL: EventCode:0x%x.  %$S"), "w");
            po.addFld("intWithKey", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "VALUE=%d (AuxData=%d, %d) Key:%$S"), "w");
            po.addFld("metaEventIntWithKey", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STATISTIC"), "displayText", "intWithKey", "tooltipText", "Value with a key string", "numParameters", 4L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "value", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_VALUE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "aux1", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_VALUE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "aux2", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_VALUE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "key", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
    }

    void UIAFrameCtx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/events/UIAFrameCtx.xs");
        om.bind("ti.uia.events.UIAFrameCtx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAFrameCtx.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAFrameCtx.Module", om.findStrict("ti.uia.events.IUIACtx.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.events"), $$UNDEF, "wh");
            po.addFld("ctxChg", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Frame Ctx Change at %$F [prev. Frame ID=0x%x] %$S"), "w");
            po.addFld("metaEventFrameCtxChg", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_CONTEXTCHANGE"), "displayText", "Frame Ctx Change", "tooltipText", "Frame ID Context Change", "numParameters", 5L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "__FILE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "__LINE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Prev. Frame ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FRAMEID"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "New Frame ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FRAMEID"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.events.UIAFrameCtx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.events.UIAFrameCtx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.events.UIAFrameCtx$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.events.UIAFrameCtx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct UIAFrameCtx.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAFrameCtx$$ModuleView", "ti.uia.events");
        po.init("ti.uia.events.UIAFrameCtx.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("mLastValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mEnableOnValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct UIAFrameCtx.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAFrameCtx$$Module_State", "ti.uia.events");
        po.init("ti.uia.events.UIAFrameCtx.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("mLastValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mEnableOnValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
    }

    void UIAMessage$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAMessage.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAMessage.Module", om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("msgSent", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x4000L, "msg", "Msg Sent [NumBytes]0x%x [Flags]0x%x [MsgId]0x%x [Dest(ProcID)]0x%x [Reply(ProcID)]0x%x [Src(ProcID)]0x%x [HeapId]0x%x"), "w");
            po.addFld("msgReceived", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x4000L, "msg", "Msg Received [NumBytes]0x%x [Flags]0x%x [MsgId]0x%x [Dest(ProcID)]0x%x [Reply(ProcID)]0x%x [Src(ProcID)]0x%x [HeapId]0x%x"), "w");
            po.addFld("replySent", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x4000L, "msg", "Reply Sent [NumBytes]0x%x [Flags]0x%x [MsgId]0x%x [Dest(ProcID)]0x%x [Reply(ProcID)]0x%x [Src(ProcID)]0x%x [HeapId]0x%x"), "w");
            po.addFld("replyReceived", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x4000L, "msg", "Reply Recieved [MsgId]0x%x [ReplyId]0x%x [NumBytes]0x%x [Sender]0x%x [Dest]0x%x "), "w");
        }//isCFG
    }

    void UIAHWICtx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAHWICtx.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAHWICtx.Module", om.findStrict("ti.uia.events.IUIACtx.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("start", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "HWI start: %$S"), "w");
            po.addFld("metaEventHwiStart", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "HWI Start", "tooltipText", "HWI Start", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "HWI ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_HWIID"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stop", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "HWI stop: %$S"), "w");
            po.addFld("metaEventHwiStop", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "HWI Exit", "tooltipText", "HWI Exit", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "HWI ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_HWIID"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
    }

    void UIAProfile$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/events/UIAProfile.xs");
        om.bind("ti.uia.events.UIAProfile$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAProfile.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAProfile.Module", om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("enterFunctionAdrs", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0001L, "msg", "enterFunctionAdrs: taskHandle=0x%x, adrs=0x%x"), "w");
            po.addFld("metaEventEnterFunctionAdrs", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "enterFunctionAdrs", "tooltipText", "function entry", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "Qualifier", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "FunctionAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("exitFunctionAdrs", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0002L, "msg", "exitFunctionAdrs: taskHandle=0x%x, adrs=0x%x"), "w");
            po.addFld("metaEventExitFunctionAdrs", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "exitFunctionAdrs", "tooltipText", "Marks the end of analysis for a module instance", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "Qualifier", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "FunctionAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("enterFunctionName", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0001L, "msg", "enterFunctionName: taskHandle=0x%x, name=%s"), "w");
            po.addFld("metaEventEnterFunctionName", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "enterFunctionName", "tooltipText", "function entry", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "Qualifier", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "FunctionAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_STRINGADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("exitFunctionName", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x0002L, "msg", "exitFunctionName: taskHandle=0x%x, name=%s"), "w");
            po.addFld("metaEventExitFunctionName", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "exitFunctionName", "tooltipText", "Marks the end of analysis for a module instance", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "Qualifier", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "FunctionAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_STRINGADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("runtimeControl", $$T_Bool, false, "wh");
            po.addFld("isContextAwareProfilingEnabled", $$T_Bool, true, "wh");
            po.addFld("enable", $$T_Bool, true, "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.events.UIAProfile$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.events.UIAProfile$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.events.UIAProfile$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.events.UIAProfile$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void UIARoundtrip$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.UIARoundtrip.Module", "ti.uia.events");
        po.init("ti.uia.events.UIARoundtrip.Module", om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("start", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Roundtrip_Start: %$S"), "w");
            po.addFld("metaEventStart", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "Roundtrip_Start", "tooltipText", "Marks the start of analysis", "numParameters", 1L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stop", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Roundtrip_Stop: %$S"), "w");
            po.addFld("metaEventStop", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "Roundtrip_Stop", "tooltipText", "Marks the end of analysis", "numParameters", 1L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false)})), "wh");
            po.addFld("startInstance", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Roundtrip_StartInstance: %$S"), "w");
            po.addFld("metaEventStartInstance", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "Roundtrip_StartInstance", "tooltipText", "Marks the start of analysis for a module instance", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "InstanceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stopInstance", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Roundtrip_StopInstance: %$S"), "w");
            po.addFld("metaEventStopInstance", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "Roundtrip_StopInstance", "tooltipText", "Marks the end of analysis for a module instance", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "InstanceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("startInstanceWithAdrs", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Roundtrip_StartInstanceWithAdrs: %$S"), "w");
            po.addFld("metaEventStartInstanceWithAdrs", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "Roundtrip_StartInstanceWithAdrs", "tooltipText", "Marks the start of analysis for a module instance", "numParameters", 3L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "InstanceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "FunctionAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stopInstanceWithAdrs", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Roundtrip_StopInstanceWithAdrs: %$S"), "w");
            po.addFld("metaEventStopInstanceWithAdrs", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "Roundtrip_StopInstanceWithAdrs", "tooltipText", "Marks the end of analysis for a module instance", "numParameters", 3L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "Handle", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "FunctionAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("startInstanceWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Roundtrip_StartInstanceWithStr: %$S"), "w");
            po.addFld("metaEventStartInstanceWithStr", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "Roundtrip_StartInstanceWithStr", "tooltipText", "Marks the start of analysis for a module instance", "numParameters", 3L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "InstanceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "FunctionAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stopInstanceWithStr", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Roundtrip_StopInstanceWithStr: %$S"), "w");
            po.addFld("metaEventStopInstanceWithStr", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "Roundtrip_StopInstanceWithStr", "tooltipText", "Marks the end of analysis for a module instance", "numParameters", 3L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "InstanceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "String", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_STRINGADRS"), "dataTypeName", "String", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
    }

    void UIASWICtx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.UIASWICtx.Module", "ti.uia.events");
        po.init("ti.uia.events.UIASWICtx.Module", om.findStrict("ti.uia.events.IUIACtx.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("start", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "SWI start: %$S"), "w");
            po.addFld("metaEventSwiStart", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_START"), "displayText", "SWI Start", "tooltipText", "SWI Start", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "SWI ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_SWIID"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stop", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "SWI stop: %$S"), "w");
            po.addFld("metaEventSwiStop", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STOP"), "displayText", "SWI Exit", "tooltipText", "SWI Exit", "numParameters", 2L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "SWI ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_SWIID"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
    }

    void UIASnapshot$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.UIASnapshot.Module", "ti.uia.events");
        po.init("ti.uia.events.UIASnapshot.Module", om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("memoryRange", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Memory Snapshot at %$F% [snapshotID=%d,adrs=0x%x,numMAUsDataInEvent=%hd,numMAUsDataInRecord=%hd] %$S"), "w");
            po.addFld("metaEventMemoryRange", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_MEMORYSNAPSHOT"), "displayText", "Memory Snapshot", "tooltipText", "Memory Snapshot", "numParameters", 8L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "filename", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "linenum", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "snapshotID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_SNAPSHOTID"), "dataTypeName", "UInt32", "units", "none", "isHidden", false), Global.newObject("name", "startAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_DATAADRS"), "dataTypeName", "Ptr", "units", "none", "isHidden", false), Global.newObject("name", "numMAUsDataInEvent", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LENGTHINMAUS"), "dataTypeName", "Int16", "units", "none", "isHidden", false, "lsb", 16L), Global.newObject("name", "numMAUsDataInRecord", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LENGTHINMAUS"), "dataTypeName", "Int16", "units", "none", "isHidden", false, "lsb", 0L), Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "data", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_DATAARRAY"), "dataTypeName", "Int32", "units", "none", "isHidden", false)})), "wh");
            po.addFld("stringOnHeap", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "String Snapshot at %$F [snapshotID=%d,adrs=0x%x,numMAUsDataInEvent=%hd,numMAUsDataInRecord=%hd] %$S"), "w");
            po.addFld("metaEventStringOnHeap", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STRINGSNAPSHOT"), "displayText", "String Snapshot", "tooltipText", "String Snapshot", "numParameters", 8L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "filename", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "linenum", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "snapshotID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_SNAPSHOTID"), "dataTypeName", "UInt32", "units", "none", "isHidden", false), Global.newObject("name", "startAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_DATAADRS"), "dataTypeName", "Ptr", "units", "none", "isHidden", false), Global.newObject("name", "numMAUsDataInEvent", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LENGTHINMAUS"), "dataTypeName", "Int16", "units", "none", "isHidden", false, "lsb", 16L), Global.newObject("name", "numMAUsDataInRecord", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LENGTHINMAUS"), "dataTypeName", "Int16", "units", "none", "isHidden", false, "lsb", 0L), Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "data", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_DATAARRAY"), "dataTypeName", "Int32", "units", "none", "isHidden", false)})), "wh");
            po.addFld("nameOfReference", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "nameOfReference at %$F [refID=0x%x,adrs=0x%x,numMAUsDataInEvent=%hd numMAUsDataInRecord=%hd] %$S"), "w");
            po.addFld("metaEventNameOfReference", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_NAMESNAPSHOT"), "displayText", "Name Of Reference ID", "tooltipText", "Name Of Reference ID", "numParameters", 8L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "filename", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "linenum", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "referenceID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_REFERENCEID"), "dataTypeName", "UInt32", "units", "none", "isHidden", false), Global.newObject("name", "startAdrs", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_DATAADRS"), "dataTypeName", "Ptr", "units", "none", "isHidden", false), Global.newObject("name", "numMAUsDataInEvent", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LENGTHINMAUS"), "dataTypeName", "Int16", "units", "none", "isHidden", false, "lsb", 16L), Global.newObject("name", "numMAUsDataInRecord", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LENGTHINMAUS"), "dataTypeName", "Int16", "units", "none", "isHidden", false, "lsb", 0L), Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "data", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_DATAARRAY"), "dataTypeName", "Int32", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
    }

    void UIAStatistic$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAStatistic.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAStatistic.Module", om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("cpuLoad", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "CPU Load: NumCycles=%d"), "w");
            po.addFld("cpuLoadByInstance", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "CPU Load for %s (instanceId = 0x%x): NumCycles=%d"), "w");
            po.addFld("bytesProcessed", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Bytes Processed by %s: NumBytes=0x%x"), "w");
            po.addFld("bytesProcessedByInstance", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Bytes Processed at %$F (InstanceId 0x%x): Num Bytes=%d"), "w");
            po.addFld("metaEventBytesProcessedByInstance", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STATISTIC"), "displayText", "Bytes Processed", "tooltipText", "Bytes Processed", "numParameters", 4L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "__FILE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "__LINE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Instance ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Words Processed", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_VALUE"), "dataTypeName", "Int", "units", "bytes", "isHidden", false)})), "wh");
            po.addFld("wordsProcessed", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Words Processed by %s: NumWords=0x%x"), "w");
            po.addFld("wordsProcessedByInstance", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Words Processed at %$F (InstanceId 0x%x): Num Words=%d"), "w");
            po.addFld("metaEventWordsProcessedByInstance", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STATISTIC"), "displayText", "Words Processed", "tooltipText", "Words Processed", "numParameters", 4L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "__FILE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "__LINE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Instance ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Words Processed", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_VALUE"), "dataTypeName", "Int", "units", "words", "isHidden", false)})), "wh");
            po.addFld("freeBytes", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Heap at %$F (HeapId 0x%x): Free Bytes=%d"), "w");
            po.addFld("metaEventFreeBytes", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_STATISTIC"), "displayText", "Free Bytes", "tooltipText", "Free Bytes in Heap", "numParameters", 4L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "__FILE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "__LINE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Heap ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Free Bytes", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_VALUE"), "dataTypeName", "Int", "units", "bytes", "isHidden", false)})), "wh");
        }//isCFG
    }

    void UIASync$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.UIASync.Module", "ti.uia.events");
        po.init("ti.uia.events.UIASync.Module", om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("syncPoint", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Sync Point:  SerialNumber=0x%x, CpuTStamp [LSW=0x%x, MSW=0x%x], GlobalTStamp [LSW=0x%x, MSW=0x%x], CpuCyclesPerCpuTimerTick=%d, CpuFreq [LSW=0x%x, MSW=0x%x]"), "w");
            po.addFld("syncPointAfterHalt", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Sync Point After Halt:  SerialNumber=0x%x, CpuTStamp [LSW=0x%x, MSW=0x%x], GlobalTStamp [LSW=0x%x, MSW=0x%x], CpuCyclesPerCpuTimerTick=%d, CpuFreq [LSW=0x%x, MSW=0x%x]"), "w");
            po.addFld("globalTimerFreq", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Sync Point Global Timer Freq:  SerialNumber=0x%x, CpuCyclesPerGlobalTimerTick=%d, GlobalTimerFreq [LSW=0x%x, MSW=0x%x]"), "w");
            po.addFld("syncPointUserProvidedData", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Sync Point User Data: SerialNumber=0x%x, %$S"), "w");
        }//isCFG
    }

    void UIAThreadCtx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/events/UIAThreadCtx.xs");
        om.bind("ti.uia.events.UIAThreadCtx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAThreadCtx.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAThreadCtx.Module", om.findStrict("ti.uia.events.IUIACtx.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.events"), $$UNDEF, "wh");
            po.addFld("ctxChg", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Thread Ctx Change at %$F [Prev. thread ID = 0x%x] %$S"), "w");
            po.addFld("metaEventThreadCtxChg", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_CONTEXTCHANGE"), "displayText", "Thread Ctx Change", "tooltipText", "Thread Context Change", "numParameters", 5L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "__FILE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "__LINE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Prev. Thread ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_THREADID"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "New Thread ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_THREADID"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
            po.addFld("ctxChgWithFunc", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "Thread CtxChgWithFunc at %$F [Prev. threadId = 0x%x] %$S"), "w");
            po.addFld("metaEventThreadCtxChgWithFunc", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_CONTEXTCHANGE"), "displayText", "Thread Ctx Change with function addresses", "tooltipText", "Thread Context Change with function addresses", "numParameters", 7L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "__FILE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "__LINE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Prev. Thread ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_THREADID"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "New Frame ID", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_THREADID"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Preempted Function Address", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Resumed Function Address", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.events.UIAThreadCtx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.events.UIAThreadCtx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.events.UIAThreadCtx$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.events.UIAThreadCtx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct UIAThreadCtx.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAThreadCtx$$ModuleView", "ti.uia.events");
        po.init("ti.uia.events.UIAThreadCtx.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("mLastValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mEnableOnValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct UIAThreadCtx.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAThreadCtx$$Module_State", "ti.uia.events");
        po.init("ti.uia.events.UIAThreadCtx.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("mLastValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mEnableOnValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
    }

    void UIAUserCtx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/events/UIAUserCtx.xs");
        om.bind("ti.uia.events.UIAUserCtx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAUserCtx.Module", "ti.uia.events");
        po.init("ti.uia.events.UIAUserCtx.Module", om.findStrict("ti.uia.events.IUIACtx.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.events"), $$UNDEF, "wh");
            po.addFld("ctxChg", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.events"), Global.newObject("mask", 0x8000L, "msg", "User Ctx Change at %$F [Prev. ctx = 0x%x] %$S"), "w");
            po.addFld("metaEventUserCtxChg", (Proto)om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"), Global.newObject("versionId", "2.0", "analysisType", om.find("ti.uia.events.DvtTypes.DvtAnalysisType_CONTEXTCHANGE"), "displayText", "User Ctx Change", "tooltipText", "User Context Change", "numParameters", 5L, "paramInfo", Global.newArray(new Object[]{Global.newObject("name", "__FILE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "__LINE__", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "Prev. User Context", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_STATEID"), "dataTypeName", "Int", "units", "none", "isHidden", false), Global.newObject("name", "fmt", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR"), "dataTypeName", "String", "units", "none", "isHidden", false), Global.newObject("name", "New User Context", "dataDesc", om.find("ti.uia.events.DvtTypes.DvtDataDesc_STATEID"), "dataTypeName", "Int", "units", "none", "isHidden", false)})), "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.events.UIAUserCtx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.events.UIAUserCtx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.events.UIAUserCtx$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.events.UIAUserCtx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct UIAUserCtx.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAUserCtx$$ModuleView", "ti.uia.events");
        po.init("ti.uia.events.UIAUserCtx.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("mLastValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mEnableOnValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct UIAUserCtx.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAUserCtx$$Module_State", "ti.uia.events");
        po.init("ti.uia.events.UIAUserCtx.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("mLastValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mEnableOnValue", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
    }

    void IUIAMetaProvider$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.events.IUIAMetaProvider.Module", "ti.uia.events");
        po.init("ti.uia.events.IUIAMetaProvider.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.events"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("hasMetaData", $$T_Bool, false, "wh");
        }//isCFG
    }

    void DvtTypes$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.DvtTypes", "ti.uia.events");
    }

    void IUIAEvent$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.IUIAEvent", "ti.uia.events");
    }

    void IUIACtx$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.IUIACtx", "ti.uia.events");
    }

    void UIAAppCtx$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAAppCtx", "ti.uia.events");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.events.UIAAppCtx.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAAppCtx$$Module_State", "ti.uia.events");
    }

    void UIABenchmark$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIABenchmark", "ti.uia.events");
    }

    void UIAChanCtx$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAChanCtx", "ti.uia.events");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.events.UIAChanCtx.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAChanCtx$$Module_State", "ti.uia.events");
    }

    void UIAErr$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAErr", "ti.uia.events");
    }

    void UIAEvt$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAEvt", "ti.uia.events");
    }

    void UIAFrameCtx$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAFrameCtx", "ti.uia.events");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.events.UIAFrameCtx.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAFrameCtx$$Module_State", "ti.uia.events");
    }

    void UIAMessage$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAMessage", "ti.uia.events");
    }

    void UIAHWICtx$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAHWICtx", "ti.uia.events");
    }

    void UIAProfile$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAProfile", "ti.uia.events");
    }

    void UIARoundtrip$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIARoundtrip", "ti.uia.events");
    }

    void UIASWICtx$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIASWICtx", "ti.uia.events");
    }

    void UIASnapshot$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIASnapshot", "ti.uia.events");
    }

    void UIAStatistic$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAStatistic", "ti.uia.events");
    }

    void UIASync$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIASync", "ti.uia.events");
    }

    void UIAThreadCtx$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAThreadCtx", "ti.uia.events");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.events.UIAThreadCtx.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAThreadCtx$$Module_State", "ti.uia.events");
    }

    void UIAUserCtx$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAUserCtx", "ti.uia.events");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.events.UIAUserCtx.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAUserCtx$$Module_State", "ti.uia.events");
    }

    void IUIAMetaProvider$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.IUIAMetaProvider", "ti.uia.events");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.uia.events.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.uia.events"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/events/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.uia.events"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.uia.events"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.uia.events"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.uia.events"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.uia.events"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.uia.events"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.uia.events", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.uia.events");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.uia.events.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 2));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.uia.events'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.uia.events$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.uia.events$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.uia.events$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/release/ti.uia.events.aem4',\n");
            sb.append("'lib/release/ti.uia.events.am4',\n");
            sb.append("'lib/release/ti.uia.events.am4g',\n");
            sb.append("'lib/release/ti.uia.events.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/release/ti.uia.events.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/release/ti.uia.events.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/release/ti.uia.events.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/release/ti.uia.events.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void DvtTypes$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.DvtTypes", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.DvtTypes.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.DvtTypes", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("DvtAnalysisType", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType", "ti.uia.events"));
        vo.bind("DvtDataDesc", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc", "ti.uia.events"));
        vo.bind("ParamDescriptor", om.findStrict("ti.uia.events.DvtTypes.ParamDescriptor", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.DvtTypes.ParamDescriptor", "ti.uia.events"));
        vo.bind("MetaEventDescriptor", om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.DvtTypes.MetaEventDescriptor", "ti.uia.events"));
        vo.bind("DvtAnalysisType_START", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_START", "ti.uia.events"));
        vo.bind("DvtAnalysisType_STOP", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_STOP", "ti.uia.events"));
        vo.bind("DvtAnalysisType_STARTSTOP", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_STARTSTOP", "ti.uia.events"));
        vo.bind("DvtAnalysisType_FUNCTIONENTRY", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_FUNCTIONENTRY", "ti.uia.events"));
        vo.bind("DvtAnalysisType_FUNCTIONEXIT", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_FUNCTIONEXIT", "ti.uia.events"));
        vo.bind("DvtAnalysisType_SYNCPOINT", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_SYNCPOINT", "ti.uia.events"));
        vo.bind("DvtAnalysisType_CONTEXTCHANGE", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_CONTEXTCHANGE", "ti.uia.events"));
        vo.bind("DvtAnalysisType_STATECHANGE", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_STATECHANGE", "ti.uia.events"));
        vo.bind("DvtAnalysisType_MESSAGEMONITOR", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_MESSAGEMONITOR", "ti.uia.events"));
        vo.bind("DvtAnalysisType_STATISTIC", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_STATISTIC", "ti.uia.events"));
        vo.bind("DvtAnalysisType_SEMAPHORE", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_SEMAPHORE", "ti.uia.events"));
        vo.bind("DvtAnalysisType_LOCK", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_LOCK", "ti.uia.events"));
        vo.bind("DvtAnalysisType_EVENTMARKER", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_EVENTMARKER", "ti.uia.events"));
        vo.bind("DvtAnalysisType_MEMORYSNAPSHOT", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_MEMORYSNAPSHOT", "ti.uia.events"));
        vo.bind("DvtAnalysisType_STRINGSNAPSHOT", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_STRINGSNAPSHOT", "ti.uia.events"));
        vo.bind("DvtAnalysisType_NAMESNAPSHOT", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_NAMESNAPSHOT", "ti.uia.events"));
        vo.bind("DvtAnalysisType_STACKSNAPSHOT", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_STACKSNAPSHOT", "ti.uia.events"));
        vo.bind("DvtAnalysisType_REGISTERSNAPSHOT", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_REGISTERSNAPSHOT", "ti.uia.events"));
        vo.bind("DvtAnalysisType_CUSTOM", om.findStrict("ti.uia.events.DvtTypes.DvtAnalysisType_CUSTOM", "ti.uia.events"));
        vo.bind("DvtDataDesc_TIMESTAMP32", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP32", "ti.uia.events"));
        vo.bind("DvtDataDesc_TIMESTAMP64_MSW", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP64_MSW", "ti.uia.events"));
        vo.bind("DvtDataDesc_TIMESTAMP64_LSW", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP64_LSW", "ti.uia.events"));
        vo.bind("DvtDataDesc_TIMESTAMP_CTIME", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_TIMESTAMP_CTIME", "ti.uia.events"));
        vo.bind("DvtDataDesc_COOKIE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_COOKIE", "ti.uia.events"));
        vo.bind("DvtDataDesc_SIZE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_SIZE", "ti.uia.events"));
        vo.bind("DvtDataDesc_PROGRAMADRS", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_PROGRAMADRS", "ti.uia.events"));
        vo.bind("DvtDataDesc_FUNCTIONADRS", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_FUNCTIONADRS", "ti.uia.events"));
        vo.bind("DvtDataDesc_DATAADRS", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_DATAADRS", "ti.uia.events"));
        vo.bind("DvtDataDesc_STRINGADRS", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_STRINGADRS", "ti.uia.events"));
        vo.bind("DvtDataDesc_INSTANCE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_INSTANCE", "ti.uia.events"));
        vo.bind("DvtDataDesc_INITIATOR", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_INITIATOR", "ti.uia.events"));
        vo.bind("DvtDataDesc_PROCESSID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_PROCESSID", "ti.uia.events"));
        vo.bind("DvtDataDesc_THREADID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_THREADID", "ti.uia.events"));
        vo.bind("DvtDataDesc_TASKID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_TASKID", "ti.uia.events"));
        vo.bind("DvtDataDesc_CHANNELID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_CHANNELID", "ti.uia.events"));
        vo.bind("DvtDataDesc_PACKETID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_PACKETID", "ti.uia.events"));
        vo.bind("DvtDataDesc_FRAMEID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_FRAMEID", "ti.uia.events"));
        vo.bind("DvtDataDesc_APPID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_APPID", "ti.uia.events"));
        vo.bind("DvtDataDesc_HWIID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_HWIID", "ti.uia.events"));
        vo.bind("DvtDataDesc_SWIID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_SWIID", "ti.uia.events"));
        vo.bind("DvtDataDesc_STATEID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_STATEID", "ti.uia.events"));
        vo.bind("DvtDataDesc_STATESTRINGADRS", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_STATESTRINGADRS", "ti.uia.events"));
        vo.bind("DvtDataDesc_COUNTER", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_COUNTER", "ti.uia.events"));
        vo.bind("DvtDataDesc_VALUE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_VALUE", "ti.uia.events"));
        vo.bind("DvtDataDesc_FLAG", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_FLAG", "ti.uia.events"));
        vo.bind("DvtDataDesc_SEMAPHORE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_SEMAPHORE", "ti.uia.events"));
        vo.bind("DvtDataDesc_RETURNVALUE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_RETURNVALUE", "ti.uia.events"));
        vo.bind("DvtDataDesc_PARAMVALUE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_PARAMVALUE", "ti.uia.events"));
        vo.bind("DvtDataDesc_EXPECTEDVALUE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_EXPECTEDVALUE", "ti.uia.events"));
        vo.bind("DvtDataDesc_TESTNUMBER", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_TESTNUMBER", "ti.uia.events"));
        vo.bind("DvtDataDesc_RESULT", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_RESULT", "ti.uia.events"));
        vo.bind("DvtDataDesc_ERRORCODE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_ERRORCODE", "ti.uia.events"));
        vo.bind("DvtDataDesc_EVENTCODE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_EVENTCODE", "ti.uia.events"));
        vo.bind("DvtDataDesc_FILENAMESTR", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_FILENAMESTR", "ti.uia.events"));
        vo.bind("DvtDataDesc_LINENUM", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_LINENUM", "ti.uia.events"));
        vo.bind("DvtDataDesc_FMTSTR", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_FMTSTR", "ti.uia.events"));
        vo.bind("DvtDataDesc_GLOBALSYMBOLADRS", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_GLOBALSYMBOLADRS", "ti.uia.events"));
        vo.bind("DvtDataDesc_ENUMVALUE", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_ENUMVALUE", "ti.uia.events"));
        vo.bind("DvtDataDesc_LENGTHINMAUS", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_LENGTHINMAUS", "ti.uia.events"));
        vo.bind("DvtDataDesc_DATAARRAY", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_DATAARRAY", "ti.uia.events"));
        vo.bind("DvtDataDesc_REFERENCEID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_REFERENCEID", "ti.uia.events"));
        vo.bind("DvtDataDesc_SNAPSHOTID", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_SNAPSHOTID", "ti.uia.events"));
        vo.bind("DvtDataDesc_CUSTOM", om.findStrict("ti.uia.events.DvtTypes.DvtDataDesc_CUSTOM", "ti.uia.events"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_DvtTypes_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("DvtTypes", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("DvtTypes");
    }

    void IUIAEvent$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.IUIAEvent", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.IUIAEvent.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.IUIAEvent", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IUIAEvent", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IUIAEvent");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IUIACtx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.IUIACtx", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.IUIACtx.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.IUIACtx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("IsLoggingEnabledFxn", om.findStrict("ti.uia.events.IUIACtx.IsLoggingEnabledFxn", "ti.uia.events"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IUIACtx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IUIACtx");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void UIAAppCtx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAAppCtx", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAAppCtx.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAAppCtx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.events.UIAAppCtx$$capsule", "ti.uia.events"));
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("IsLoggingEnabledFxn", om.findStrict("ti.uia.events.IUIACtx.IsLoggingEnabledFxn", "ti.uia.events"));
        mcfgs.add("isLoggingEnabledFxn");
        mcfgs.add("ENABLEMASK");
        mcfgs.add("SYNCID");
        vo.bind("ModuleView", om.findStrict("ti.uia.events.UIAAppCtx.ModuleView", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.UIAAppCtx.ModuleView", "ti.uia.events"));
        mcfgs.add("ctxChg");
        vo.bind("Module_State", om.findStrict("ti.uia.events.UIAAppCtx.Module_State", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.UIAAppCtx.Module_State", "ti.uia.events"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("getCtxId", om.findStrict("ti.uia.events.UIAAppCtx.getCtxId", "ti.uia.events"));
        vo.bind("getEnableOnValue", om.findStrict("ti.uia.events.UIAAppCtx.getEnableOnValue", "ti.uia.events"));
        vo.bind("setEnableOnValue", om.findStrict("ti.uia.events.UIAAppCtx.setEnableOnValue", "ti.uia.events"));
        vo.bind("isLoggingEnabled", om.findStrict("ti.uia.events.UIAAppCtx.isLoggingEnabled", "ti.uia.events"));
        vo.bind("setOldValue", om.findStrict("ti.uia.events.UIAAppCtx.setOldValue", "ti.uia.events"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAAppCtx_Module__startupDone__E", "ti_uia_events_UIAAppCtx_getCtxId__E", "ti_uia_events_UIAAppCtx_getEnableOnValue__E", "ti_uia_events_UIAAppCtx_setEnableOnValue__E", "ti_uia_events_UIAAppCtx_isLoggingEnabled__E", "ti_uia_events_UIAAppCtx_setOldValue__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("ctxChg"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAAppCtx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAAppCtx");
    }

    void UIABenchmark$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIABenchmark", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIABenchmark.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIABenchmark", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("start");
        mcfgs.add("stop");
        mcfgs.add("startInstance");
        mcfgs.add("stopInstance");
        mcfgs.add("startInstanceWithAdrs");
        mcfgs.add("stopInstanceWithAdrs");
        mcfgs.add("startInstanceWithStr");
        mcfgs.add("stopInstanceWithStr");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIABenchmark_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("start", "stop", "startInstance", "stopInstance", "startInstanceWithAdrs", "stopInstanceWithAdrs", "startInstanceWithStr", "stopInstanceWithStr"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIABenchmark", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIABenchmark");
    }

    void UIAChanCtx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAChanCtx", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAChanCtx.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAChanCtx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.events.UIAChanCtx$$capsule", "ti.uia.events"));
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("IsLoggingEnabledFxn", om.findStrict("ti.uia.events.IUIACtx.IsLoggingEnabledFxn", "ti.uia.events"));
        mcfgs.add("isLoggingEnabledFxn");
        mcfgs.add("ENABLEMASK");
        mcfgs.add("SYNCID");
        vo.bind("ModuleView", om.findStrict("ti.uia.events.UIAChanCtx.ModuleView", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.UIAChanCtx.ModuleView", "ti.uia.events"));
        mcfgs.add("ctxChg");
        vo.bind("Module_State", om.findStrict("ti.uia.events.UIAChanCtx.Module_State", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.UIAChanCtx.Module_State", "ti.uia.events"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("getCtxId", om.findStrict("ti.uia.events.UIAChanCtx.getCtxId", "ti.uia.events"));
        vo.bind("getEnableOnValue", om.findStrict("ti.uia.events.UIAChanCtx.getEnableOnValue", "ti.uia.events"));
        vo.bind("setEnableOnValue", om.findStrict("ti.uia.events.UIAChanCtx.setEnableOnValue", "ti.uia.events"));
        vo.bind("isLoggingEnabled", om.findStrict("ti.uia.events.UIAChanCtx.isLoggingEnabled", "ti.uia.events"));
        vo.bind("setOldValue", om.findStrict("ti.uia.events.UIAChanCtx.setOldValue", "ti.uia.events"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAChanCtx_Module__startupDone__E", "ti_uia_events_UIAChanCtx_getCtxId__E", "ti_uia_events_UIAChanCtx_getEnableOnValue__E", "ti_uia_events_UIAChanCtx_setEnableOnValue__E", "ti_uia_events_UIAChanCtx_isLoggingEnabled__E", "ti_uia_events_UIAChanCtx_setOldValue__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("ctxChg"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAChanCtx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAChanCtx");
    }

    void UIAErr$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAErr", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAErr.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAErr", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("error");
        mcfgs.add("errorWithStr");
        mcfgs.add("hwError");
        mcfgs.add("hwErrorWithStr");
        mcfgs.add("fatal");
        mcfgs.add("fatalWithStr");
        mcfgs.add("critical");
        mcfgs.add("criticalWithStr");
        mcfgs.add("exception");
        mcfgs.add("uncaughtException");
        mcfgs.add("nullPointerException");
        mcfgs.add("unexpectedInterrupt");
        mcfgs.add("memoryAccessFault");
        mcfgs.add("securityException");
        mcfgs.add("divisionByZero");
        mcfgs.add("overflowException");
        mcfgs.add("indexOutOfRange");
        mcfgs.add("notImplemented");
        mcfgs.add("stackOverflow");
        mcfgs.add("illegalInstruction");
        mcfgs.add("entryPointNotFound");
        mcfgs.add("moduleNotFound");
        mcfgs.add("floatingPointError");
        mcfgs.add("invalidParameter");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAErr_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("error", "errorWithStr", "hwError", "hwErrorWithStr", "fatal", "fatalWithStr", "critical", "criticalWithStr", "exception", "uncaughtException", "nullPointerException", "unexpectedInterrupt", "memoryAccessFault", "securityException", "divisionByZero", "overflowException", "indexOutOfRange", "notImplemented", "stackOverflow", "illegalInstruction", "entryPointNotFound", "moduleNotFound", "floatingPointError", "invalidParameter"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAErr", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAErr");
    }

    void UIAEvt$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAEvt", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAEvt.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAEvt", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("warning");
        mcfgs.add("warningWithStr");
        mcfgs.add("info");
        mcfgs.add("infoWithStr");
        mcfgs.add("detail");
        mcfgs.add("detailWithStr");
        mcfgs.add("intWithKey");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAEvt_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("warning", "warningWithStr", "info", "infoWithStr", "detail", "detailWithStr", "intWithKey"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAEvt", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAEvt");
    }

    void UIAFrameCtx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAFrameCtx", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAFrameCtx.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAFrameCtx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.events.UIAFrameCtx$$capsule", "ti.uia.events"));
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("IsLoggingEnabledFxn", om.findStrict("ti.uia.events.IUIACtx.IsLoggingEnabledFxn", "ti.uia.events"));
        mcfgs.add("isLoggingEnabledFxn");
        mcfgs.add("ENABLEMASK");
        mcfgs.add("SYNCID");
        vo.bind("ModuleView", om.findStrict("ti.uia.events.UIAFrameCtx.ModuleView", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.UIAFrameCtx.ModuleView", "ti.uia.events"));
        mcfgs.add("ctxChg");
        vo.bind("Module_State", om.findStrict("ti.uia.events.UIAFrameCtx.Module_State", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.UIAFrameCtx.Module_State", "ti.uia.events"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("getCtxId", om.findStrict("ti.uia.events.UIAFrameCtx.getCtxId", "ti.uia.events"));
        vo.bind("getEnableOnValue", om.findStrict("ti.uia.events.UIAFrameCtx.getEnableOnValue", "ti.uia.events"));
        vo.bind("setEnableOnValue", om.findStrict("ti.uia.events.UIAFrameCtx.setEnableOnValue", "ti.uia.events"));
        vo.bind("isLoggingEnabled", om.findStrict("ti.uia.events.UIAFrameCtx.isLoggingEnabled", "ti.uia.events"));
        vo.bind("setOldValue", om.findStrict("ti.uia.events.UIAFrameCtx.setOldValue", "ti.uia.events"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAFrameCtx_Module__startupDone__E", "ti_uia_events_UIAFrameCtx_getCtxId__E", "ti_uia_events_UIAFrameCtx_getEnableOnValue__E", "ti_uia_events_UIAFrameCtx_setEnableOnValue__E", "ti_uia_events_UIAFrameCtx_isLoggingEnabled__E", "ti_uia_events_UIAFrameCtx_setOldValue__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("ctxChg"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAFrameCtx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAFrameCtx");
    }

    void UIAMessage$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAMessage", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAMessage.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAMessage", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("msgSent");
        mcfgs.add("msgReceived");
        mcfgs.add("replySent");
        mcfgs.add("replyReceived");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAMessage_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("msgSent", "msgReceived", "replySent", "replyReceived"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAMessage", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAMessage");
    }

    void UIAHWICtx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAHWICtx", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAHWICtx.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAHWICtx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("IsLoggingEnabledFxn", om.findStrict("ti.uia.events.IUIACtx.IsLoggingEnabledFxn", "ti.uia.events"));
        mcfgs.add("isLoggingEnabledFxn");
        mcfgs.add("ENABLEMASK");
        mcfgs.add("SYNCID");
        mcfgs.add("start");
        mcfgs.add("stop");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAHWICtx_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("start", "stop"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAHWICtx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAHWICtx");
    }

    void UIAProfile$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAProfile", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAProfile.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAProfile", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.events.UIAProfile$$capsule", "ti.uia.events"));
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("enterFunctionAdrs");
        mcfgs.add("exitFunctionAdrs");
        mcfgs.add("enterFunctionName");
        mcfgs.add("exitFunctionName");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAProfile_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("enterFunctionAdrs", "exitFunctionAdrs", "enterFunctionName", "exitFunctionName"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./UIAProfile.xdt");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./UIAProfile.xdt");
        pkgV.bind("UIAProfile", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAProfile");
    }

    void UIARoundtrip$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIARoundtrip", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIARoundtrip.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIARoundtrip", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("start");
        mcfgs.add("stop");
        mcfgs.add("startInstance");
        mcfgs.add("stopInstance");
        mcfgs.add("startInstanceWithAdrs");
        mcfgs.add("stopInstanceWithAdrs");
        mcfgs.add("startInstanceWithStr");
        mcfgs.add("stopInstanceWithStr");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIARoundtrip_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("start", "stop", "startInstance", "stopInstance", "startInstanceWithAdrs", "stopInstanceWithAdrs", "startInstanceWithStr", "stopInstanceWithStr"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIARoundtrip", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIARoundtrip");
    }

    void UIASWICtx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIASWICtx", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIASWICtx.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIASWICtx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("IsLoggingEnabledFxn", om.findStrict("ti.uia.events.IUIACtx.IsLoggingEnabledFxn", "ti.uia.events"));
        mcfgs.add("isLoggingEnabledFxn");
        mcfgs.add("ENABLEMASK");
        mcfgs.add("SYNCID");
        mcfgs.add("start");
        mcfgs.add("stop");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIASWICtx_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("start", "stop"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIASWICtx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIASWICtx");
    }

    void UIASnapshot$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIASnapshot", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIASnapshot.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIASnapshot", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("memoryRange");
        mcfgs.add("stringOnHeap");
        mcfgs.add("nameOfReference");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIASnapshot_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("memoryRange", "stringOnHeap", "nameOfReference"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIASnapshot", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIASnapshot");
    }

    void UIAStatistic$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAStatistic", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAStatistic.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAStatistic", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("cpuLoad");
        mcfgs.add("cpuLoadByInstance");
        mcfgs.add("bytesProcessed");
        mcfgs.add("bytesProcessedByInstance");
        mcfgs.add("wordsProcessed");
        mcfgs.add("wordsProcessedByInstance");
        mcfgs.add("freeBytes");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAStatistic_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("cpuLoad", "cpuLoadByInstance", "bytesProcessed", "bytesProcessedByInstance", "wordsProcessed", "wordsProcessedByInstance", "freeBytes"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAStatistic", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAStatistic");
    }

    void UIASync$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIASync", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIASync.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIASync", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        mcfgs.add("syncPoint");
        mcfgs.add("syncPointAfterHalt");
        mcfgs.add("globalTimerFreq");
        mcfgs.add("syncPointUserProvidedData");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIASync_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("syncPoint", "syncPointAfterHalt", "globalTimerFreq", "syncPointUserProvidedData"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIASync", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIASync");
    }

    void UIAThreadCtx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAThreadCtx", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAThreadCtx.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAThreadCtx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.events.UIAThreadCtx$$capsule", "ti.uia.events"));
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("IsLoggingEnabledFxn", om.findStrict("ti.uia.events.IUIACtx.IsLoggingEnabledFxn", "ti.uia.events"));
        mcfgs.add("isLoggingEnabledFxn");
        mcfgs.add("ENABLEMASK");
        mcfgs.add("SYNCID");
        vo.bind("ModuleView", om.findStrict("ti.uia.events.UIAThreadCtx.ModuleView", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.UIAThreadCtx.ModuleView", "ti.uia.events"));
        mcfgs.add("ctxChg");
        mcfgs.add("ctxChgWithFunc");
        vo.bind("Module_State", om.findStrict("ti.uia.events.UIAThreadCtx.Module_State", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.UIAThreadCtx.Module_State", "ti.uia.events"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("getCtxId", om.findStrict("ti.uia.events.UIAThreadCtx.getCtxId", "ti.uia.events"));
        vo.bind("getEnableOnValue", om.findStrict("ti.uia.events.UIAThreadCtx.getEnableOnValue", "ti.uia.events"));
        vo.bind("setEnableOnValue", om.findStrict("ti.uia.events.UIAThreadCtx.setEnableOnValue", "ti.uia.events"));
        vo.bind("isLoggingEnabled", om.findStrict("ti.uia.events.UIAThreadCtx.isLoggingEnabled", "ti.uia.events"));
        vo.bind("setOldValue", om.findStrict("ti.uia.events.UIAThreadCtx.setOldValue", "ti.uia.events"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAThreadCtx_Module__startupDone__E", "ti_uia_events_UIAThreadCtx_getCtxId__E", "ti_uia_events_UIAThreadCtx_getEnableOnValue__E", "ti_uia_events_UIAThreadCtx_setEnableOnValue__E", "ti_uia_events_UIAThreadCtx_isLoggingEnabled__E", "ti_uia_events_UIAThreadCtx_setOldValue__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("ctxChg", "ctxChgWithFunc"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAThreadCtx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAThreadCtx");
    }

    void UIAUserCtx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.UIAUserCtx", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.UIAUserCtx.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.UIAUserCtx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.events.UIAUserCtx$$capsule", "ti.uia.events"));
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("IsLoggingEnabledFxn", om.findStrict("ti.uia.events.IUIACtx.IsLoggingEnabledFxn", "ti.uia.events"));
        mcfgs.add("isLoggingEnabledFxn");
        mcfgs.add("ENABLEMASK");
        mcfgs.add("SYNCID");
        vo.bind("ModuleView", om.findStrict("ti.uia.events.UIAUserCtx.ModuleView", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.UIAUserCtx.ModuleView", "ti.uia.events"));
        mcfgs.add("ctxChg");
        vo.bind("Module_State", om.findStrict("ti.uia.events.UIAUserCtx.Module_State", "ti.uia.events"));
        tdefs.add(om.findStrict("ti.uia.events.UIAUserCtx.Module_State", "ti.uia.events"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.events")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("getCtxId", om.findStrict("ti.uia.events.UIAUserCtx.getCtxId", "ti.uia.events"));
        vo.bind("getEnableOnValue", om.findStrict("ti.uia.events.UIAUserCtx.getEnableOnValue", "ti.uia.events"));
        vo.bind("setEnableOnValue", om.findStrict("ti.uia.events.UIAUserCtx.setEnableOnValue", "ti.uia.events"));
        vo.bind("isLoggingEnabled", om.findStrict("ti.uia.events.UIAUserCtx.isLoggingEnabled", "ti.uia.events"));
        vo.bind("setOldValue", om.findStrict("ti.uia.events.UIAUserCtx.setOldValue", "ti.uia.events"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_events_UIAUserCtx_Module__startupDone__E", "ti_uia_events_UIAUserCtx_getCtxId__E", "ti_uia_events_UIAUserCtx_getEnableOnValue__E", "ti_uia_events_UIAUserCtx_setEnableOnValue__E", "ti_uia_events_UIAUserCtx_isLoggingEnabled__E", "ti_uia_events_UIAUserCtx_setOldValue__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("ctxChg"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAUserCtx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAUserCtx");
    }

    void IUIAMetaProvider$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.events.IUIAMetaProvider", "ti.uia.events");
        po = (Proto.Obj)om.findStrict("ti.uia.events.IUIAMetaProvider.Module", "ti.uia.events");
        vo.init2(po, "ti.uia.events.IUIAMetaProvider", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.events", "ti.uia.events"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IUIAMetaProvider", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IUIAMetaProvider");
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
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.DvtTypes", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAAppCtx", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIABenchmark", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAChanCtx", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAErr", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAEvt", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAFrameCtx", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAMessage", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAHWICtx", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAProfile", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIARoundtrip", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIASWICtx", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIASnapshot", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAStatistic", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIASync", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAThreadCtx", "ti.uia.events"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.events.UIAUserCtx", "ti.uia.events"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.events.UIAAppCtx", "ti.uia.events");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.events.UIAChanCtx", "ti.uia.events");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.events.UIAFrameCtx", "ti.uia.events");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.events.UIAThreadCtx", "ti.uia.events");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.events.UIAUserCtx", "ti.uia.events");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.uia.events.DvtTypes")).bless();
        ((Value.Obj)om.getv("ti.uia.events.IUIAEvent")).bless();
        ((Value.Obj)om.getv("ti.uia.events.IUIACtx")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAAppCtx")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIABenchmark")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAChanCtx")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAErr")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAEvt")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAFrameCtx")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAMessage")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAHWICtx")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAProfile")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIARoundtrip")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIASWICtx")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIASnapshot")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAStatistic")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIASync")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAThreadCtx")).bless();
        ((Value.Obj)om.getv("ti.uia.events.UIAUserCtx")).bless();
        ((Value.Obj)om.getv("ti.uia.events.IUIAMetaProvider")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.uia.events")).add(pkgV);
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
        DvtTypes$$OBJECTS();
        IUIAEvent$$OBJECTS();
        IUIACtx$$OBJECTS();
        UIAAppCtx$$OBJECTS();
        UIABenchmark$$OBJECTS();
        UIAChanCtx$$OBJECTS();
        UIAErr$$OBJECTS();
        UIAEvt$$OBJECTS();
        UIAFrameCtx$$OBJECTS();
        UIAMessage$$OBJECTS();
        UIAHWICtx$$OBJECTS();
        UIAProfile$$OBJECTS();
        UIARoundtrip$$OBJECTS();
        UIASWICtx$$OBJECTS();
        UIASnapshot$$OBJECTS();
        UIAStatistic$$OBJECTS();
        UIASync$$OBJECTS();
        UIAThreadCtx$$OBJECTS();
        UIAUserCtx$$OBJECTS();
        IUIAMetaProvider$$OBJECTS();
        DvtTypes$$CONSTS();
        IUIAEvent$$CONSTS();
        IUIACtx$$CONSTS();
        UIAAppCtx$$CONSTS();
        UIABenchmark$$CONSTS();
        UIAChanCtx$$CONSTS();
        UIAErr$$CONSTS();
        UIAEvt$$CONSTS();
        UIAFrameCtx$$CONSTS();
        UIAMessage$$CONSTS();
        UIAHWICtx$$CONSTS();
        UIAProfile$$CONSTS();
        UIARoundtrip$$CONSTS();
        UIASWICtx$$CONSTS();
        UIASnapshot$$CONSTS();
        UIAStatistic$$CONSTS();
        UIASync$$CONSTS();
        UIAThreadCtx$$CONSTS();
        UIAUserCtx$$CONSTS();
        IUIAMetaProvider$$CONSTS();
        DvtTypes$$CREATES();
        IUIAEvent$$CREATES();
        IUIACtx$$CREATES();
        UIAAppCtx$$CREATES();
        UIABenchmark$$CREATES();
        UIAChanCtx$$CREATES();
        UIAErr$$CREATES();
        UIAEvt$$CREATES();
        UIAFrameCtx$$CREATES();
        UIAMessage$$CREATES();
        UIAHWICtx$$CREATES();
        UIAProfile$$CREATES();
        UIARoundtrip$$CREATES();
        UIASWICtx$$CREATES();
        UIASnapshot$$CREATES();
        UIAStatistic$$CREATES();
        UIASync$$CREATES();
        UIAThreadCtx$$CREATES();
        UIAUserCtx$$CREATES();
        IUIAMetaProvider$$CREATES();
        DvtTypes$$FUNCTIONS();
        IUIAEvent$$FUNCTIONS();
        IUIACtx$$FUNCTIONS();
        UIAAppCtx$$FUNCTIONS();
        UIABenchmark$$FUNCTIONS();
        UIAChanCtx$$FUNCTIONS();
        UIAErr$$FUNCTIONS();
        UIAEvt$$FUNCTIONS();
        UIAFrameCtx$$FUNCTIONS();
        UIAMessage$$FUNCTIONS();
        UIAHWICtx$$FUNCTIONS();
        UIAProfile$$FUNCTIONS();
        UIARoundtrip$$FUNCTIONS();
        UIASWICtx$$FUNCTIONS();
        UIASnapshot$$FUNCTIONS();
        UIAStatistic$$FUNCTIONS();
        UIASync$$FUNCTIONS();
        UIAThreadCtx$$FUNCTIONS();
        UIAUserCtx$$FUNCTIONS();
        IUIAMetaProvider$$FUNCTIONS();
        DvtTypes$$SIZES();
        IUIAEvent$$SIZES();
        IUIACtx$$SIZES();
        UIAAppCtx$$SIZES();
        UIABenchmark$$SIZES();
        UIAChanCtx$$SIZES();
        UIAErr$$SIZES();
        UIAEvt$$SIZES();
        UIAFrameCtx$$SIZES();
        UIAMessage$$SIZES();
        UIAHWICtx$$SIZES();
        UIAProfile$$SIZES();
        UIARoundtrip$$SIZES();
        UIASWICtx$$SIZES();
        UIASnapshot$$SIZES();
        UIAStatistic$$SIZES();
        UIASync$$SIZES();
        UIAThreadCtx$$SIZES();
        UIAUserCtx$$SIZES();
        IUIAMetaProvider$$SIZES();
        DvtTypes$$TYPES();
        IUIAEvent$$TYPES();
        IUIACtx$$TYPES();
        UIAAppCtx$$TYPES();
        UIABenchmark$$TYPES();
        UIAChanCtx$$TYPES();
        UIAErr$$TYPES();
        UIAEvt$$TYPES();
        UIAFrameCtx$$TYPES();
        UIAMessage$$TYPES();
        UIAHWICtx$$TYPES();
        UIAProfile$$TYPES();
        UIARoundtrip$$TYPES();
        UIASWICtx$$TYPES();
        UIASnapshot$$TYPES();
        UIAStatistic$$TYPES();
        UIASync$$TYPES();
        UIAThreadCtx$$TYPES();
        UIAUserCtx$$TYPES();
        IUIAMetaProvider$$TYPES();
        if (isROV) {
            DvtTypes$$ROV();
            IUIAEvent$$ROV();
            IUIACtx$$ROV();
            UIAAppCtx$$ROV();
            UIABenchmark$$ROV();
            UIAChanCtx$$ROV();
            UIAErr$$ROV();
            UIAEvt$$ROV();
            UIAFrameCtx$$ROV();
            UIAMessage$$ROV();
            UIAHWICtx$$ROV();
            UIAProfile$$ROV();
            UIARoundtrip$$ROV();
            UIASWICtx$$ROV();
            UIASnapshot$$ROV();
            UIAStatistic$$ROV();
            UIASync$$ROV();
            UIAThreadCtx$$ROV();
            UIAUserCtx$$ROV();
            IUIAMetaProvider$$ROV();
        }//isROV
        $$SINGLETONS();
        DvtTypes$$SINGLETONS();
        IUIAEvent$$SINGLETONS();
        IUIACtx$$SINGLETONS();
        UIAAppCtx$$SINGLETONS();
        UIABenchmark$$SINGLETONS();
        UIAChanCtx$$SINGLETONS();
        UIAErr$$SINGLETONS();
        UIAEvt$$SINGLETONS();
        UIAFrameCtx$$SINGLETONS();
        UIAMessage$$SINGLETONS();
        UIAHWICtx$$SINGLETONS();
        UIAProfile$$SINGLETONS();
        UIARoundtrip$$SINGLETONS();
        UIASWICtx$$SINGLETONS();
        UIASnapshot$$SINGLETONS();
        UIAStatistic$$SINGLETONS();
        UIASync$$SINGLETONS();
        UIAThreadCtx$$SINGLETONS();
        UIAUserCtx$$SINGLETONS();
        IUIAMetaProvider$$SINGLETONS();
        $$INITIALIZATION();
    }
}
