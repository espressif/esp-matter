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

public class ti_uia_runtime
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
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "xdc.rta");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.uia.events");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.uia.runtime.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.uia.runtime", new Value.Obj("ti.uia.runtime", pkgP));
    }

    void CtxFilter$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.CtxFilter.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.CtxFilter", new Value.Obj("ti.uia.runtime.CtxFilter", po));
        pkgV.bind("CtxFilter", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.runtime.CtxFilter$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.runtime.CtxFilter.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.CtxFilter$$Module_State", new Proto.Obj());
        om.bind("ti.uia.runtime.CtxFilter.Module_State", new Proto.Str(spo, false));
    }

    void EventHdr$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.EventHdr.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.EventHdr", new Value.Obj("ti.uia.runtime.EventHdr", po));
        pkgV.bind("EventHdr", vo);
        // decls 
        om.bind("ti.uia.runtime.EventHdr.HdrType", new Proto.Enm("ti.uia.runtime.EventHdr.HdrType"));
    }

    void LogCtxChg$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.LogCtxChg.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.LogCtxChg", new Value.Obj("ti.uia.runtime.LogCtxChg", po));
        pkgV.bind("LogCtxChg", vo);
        // decls 
    }

    void IUIATraceSyncClient$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.IUIATraceSyncClient.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.IUIATraceSyncClient", new Value.Obj("ti.uia.runtime.IUIATraceSyncClient", po));
        pkgV.bind("IUIATraceSyncClient", vo);
        // decls 
    }

    void LogSnapshot$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.LogSnapshot.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.LogSnapshot", new Value.Obj("ti.uia.runtime.LogSnapshot", po));
        pkgV.bind("LogSnapshot", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.runtime.LogSnapshot$$EventRec", new Proto.Obj());
        om.bind("ti.uia.runtime.LogSnapshot.EventRec", new Proto.Str(spo, false));
    }

    void LogSync$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.LogSync.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.LogSync", new Value.Obj("ti.uia.runtime.LogSync", po));
        pkgV.bind("LogSync", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.runtime.LogSync$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.runtime.LogSync.ModuleView", new Proto.Str(spo, false));
        om.bind("ti.uia.runtime.LogSync.LoggerType", new Proto.Enm("ti.uia.runtime.LogSync.LoggerType"));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.LogSync$$Module_State", new Proto.Obj());
        om.bind("ti.uia.runtime.LogSync.Module_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.LogSync$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.runtime.LogSync.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.uia.runtime.LogSync.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.runtime.LogSync$$Object", new Proto.Obj());
        om.bind("ti.uia.runtime.LogSync.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.runtime.LogSync$$Params", new Proto.Obj());
        om.bind("ti.uia.runtime.LogSync.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.runtime.LogSync$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.runtime.LogSync.Instance_State", new Proto.Str(po, false));
        om.bind("ti.uia.runtime.LogSync.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.runtime.LogSync.Object", om.findStrict("ti.uia.runtime.LogSync.Instance_State", "ti.uia.runtime"));
        }//isROV
    }

    void LoggerSM$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.LoggerSM.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.LoggerSM", new Value.Obj("ti.uia.runtime.LoggerSM", po));
        pkgV.bind("LoggerSM", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.runtime.LoggerSM$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.runtime.LoggerSM.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.LoggerSM$$InstanceView", new Proto.Obj());
        om.bind("ti.uia.runtime.LoggerSM.InstanceView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.LoggerSM$$MetaData", new Proto.Obj());
        om.bind("ti.uia.runtime.LoggerSM.MetaData", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.LoggerSM$$Module_State", new Proto.Obj());
        om.bind("ti.uia.runtime.LoggerSM.Module_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.LoggerSM$$SharedObj", new Proto.Obj());
        om.bind("ti.uia.runtime.LoggerSM.SharedObj", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.LoggerSM$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.runtime.LoggerSM.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.uia.runtime.LoggerSM.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.runtime.LoggerSM$$Object", new Proto.Obj());
        om.bind("ti.uia.runtime.LoggerSM.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.runtime.LoggerSM$$Params", new Proto.Obj());
        om.bind("ti.uia.runtime.LoggerSM.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.runtime.LoggerSM$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.runtime.LoggerSM.Instance_State", new Proto.Str(po, false));
        om.bind("ti.uia.runtime.LoggerSM.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.runtime.LoggerSM.Object", om.findStrict("ti.uia.runtime.LoggerSM.Instance_State", "ti.uia.runtime"));
        }//isROV
    }

    void LoggerTypes$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.LoggerTypes.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.LoggerTypes", new Value.Obj("ti.uia.runtime.LoggerTypes", po));
        pkgV.bind("LoggerTypes", vo);
        // decls 
    }

    void MultiCoreTypes$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.MultiCoreTypes.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.MultiCoreTypes", new Value.Obj("ti.uia.runtime.MultiCoreTypes", po));
        pkgV.bind("MultiCoreTypes", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.runtime.MultiCoreTypes$$ServiceHdr", new Proto.Obj());
        om.bind("ti.uia.runtime.MultiCoreTypes.ServiceHdr", new Proto.Str(spo, false));
        om.bind("ti.uia.runtime.MultiCoreTypes.Action", new Proto.Enm("ti.uia.runtime.MultiCoreTypes.Action"));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.MultiCoreTypes$$RegisterMsg", new Proto.Obj());
        om.bind("ti.uia.runtime.MultiCoreTypes.RegisterMsg", new Proto.Str(spo, false));
    }

    void QueueDescriptor$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.QueueDescriptor.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.QueueDescriptor", new Value.Obj("ti.uia.runtime.QueueDescriptor", po));
        pkgV.bind("QueueDescriptor", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.runtime.QueueDescriptor$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.runtime.QueueDescriptor.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.QueueDescriptor$$Header", new Proto.Obj());
        om.bind("ti.uia.runtime.QueueDescriptor.Header", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.QueueDescriptor$$Module_State", new Proto.Obj());
        om.bind("ti.uia.runtime.QueueDescriptor.Module_State", new Proto.Str(spo, false));
    }

    void UIAMetaData$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.UIAMetaData.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.UIAMetaData", new Value.Obj("ti.uia.runtime.UIAMetaData", po));
        pkgV.bind("UIAMetaData", vo);
        // decls 
        om.bind("ti.uia.runtime.UIAMetaData.UploadMode", new Proto.Enm("ti.uia.runtime.UIAMetaData.UploadMode"));
    }

    void UIAPacket$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.UIAPacket.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.UIAPacket", new Value.Obj("ti.uia.runtime.UIAPacket", po));
        pkgV.bind("UIAPacket", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.runtime.UIAPacket$$Hdr", new Proto.Obj());
        om.bind("ti.uia.runtime.UIAPacket.Hdr", new Proto.Str(spo, false));
        om.bind("ti.uia.runtime.UIAPacket.HdrType", new Proto.Enm("ti.uia.runtime.UIAPacket.HdrType"));
        om.bind("ti.uia.runtime.UIAPacket.PayloadEndian", new Proto.Enm("ti.uia.runtime.UIAPacket.PayloadEndian"));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.UIAPacket$$Footer", new Proto.Obj());
        om.bind("ti.uia.runtime.UIAPacket.Footer", new Proto.Str(spo, false));
        om.bind("ti.uia.runtime.UIAPacket.MsgType", new Proto.Enm("ti.uia.runtime.UIAPacket.MsgType"));
        om.bind("ti.uia.runtime.UIAPacket.NACKErrorCode", new Proto.Enm("ti.uia.runtime.UIAPacket.NACKErrorCode"));
    }

    void ServiceMgr$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.ServiceMgr.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.ServiceMgr", new Value.Obj("ti.uia.runtime.ServiceMgr", po));
        pkgV.bind("ServiceMgr", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.runtime.ServiceMgr$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.runtime.ServiceMgr.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.ServiceMgr$$PacketView", new Proto.Obj());
        om.bind("ti.uia.runtime.ServiceMgr.PacketView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.ServiceMgr$$TransportView", new Proto.Obj());
        om.bind("ti.uia.runtime.ServiceMgr.TransportView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.ServiceMgr$$StatisticsView", new Proto.Obj());
        om.bind("ti.uia.runtime.ServiceMgr.StatisticsView", new Proto.Str(spo, false));
        om.bind("ti.uia.runtime.ServiceMgr.Reason", new Proto.Enm("ti.uia.runtime.ServiceMgr.Reason"));
        om.bind("ti.uia.runtime.ServiceMgr.Topology", new Proto.Enm("ti.uia.runtime.ServiceMgr.Topology"));
        om.bind("ti.uia.runtime.ServiceMgr.TransportType", new Proto.Enm("ti.uia.runtime.ServiceMgr.TransportType"));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.ServiceMgr$$ServiceIdDesc", new Proto.Obj());
        om.bind("ti.uia.runtime.ServiceMgr.ServiceIdDesc", new Proto.Str(spo, false));
        om.bind("ti.uia.runtime.ServiceMgr$$ServiceId", new Proto.Tel());
        spo = (Proto.Obj)om.bind("ti.uia.runtime.ServiceMgr$$Module_State", new Proto.Obj());
        om.bind("ti.uia.runtime.ServiceMgr.Module_State", new Proto.Str(spo, false));
    }

    void Transport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.Transport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.Transport", new Value.Obj("ti.uia.runtime.Transport", po));
        pkgV.bind("Transport", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.runtime.Transport$$FxnSet", new Proto.Obj());
        om.bind("ti.uia.runtime.Transport.FxnSet", new Proto.Str(spo, false));
    }

    void ICtxFilterCallback$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.ICtxFilterCallback.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.ICtxFilterCallback", new Value.Obj("ti.uia.runtime.ICtxFilterCallback", po));
        pkgV.bind("ICtxFilterCallback", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.uia.runtime.ICtxFilterCallback.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.runtime.ICtxFilterCallback$$Object", new Proto.Obj());
        om.bind("ti.uia.runtime.ICtxFilterCallback.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.runtime.ICtxFilterCallback$$Params", new Proto.Obj());
        om.bind("ti.uia.runtime.ICtxFilterCallback.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.runtime.ICtxFilterCallback$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.runtime.ICtxFilterCallback.Instance_State", new Proto.Str(po, false));
        om.bind("ti.uia.runtime.ICtxFilterCallback.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.runtime.ICtxFilterCallback.Object", om.findStrict("ti.uia.runtime.ICtxFilterCallback.Instance_State", "ti.uia.runtime"));
        }//isROV
    }

    void IUIATransfer$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.IUIATransfer.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.IUIATransfer", new Value.Obj("ti.uia.runtime.IUIATransfer", po));
        pkgV.bind("IUIATransfer", vo);
        // decls 
        om.bind("ti.uia.runtime.IUIATransfer.TransferType", new Proto.Enm("ti.uia.runtime.IUIATransfer.TransferType"));
        om.bind("ti.uia.runtime.IUIATransfer.Priority", new Proto.Enm("ti.uia.runtime.IUIATransfer.Priority"));
        spo = (Proto.Obj)om.bind("ti.uia.runtime.IUIATransfer$$MetaData", new Proto.Obj());
        om.bind("ti.uia.runtime.IUIATransfer.MetaData", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.uia.runtime.IUIATransfer.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.runtime.IUIATransfer$$Object", new Proto.Obj());
        om.bind("ti.uia.runtime.IUIATransfer.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.runtime.IUIATransfer$$Params", new Proto.Obj());
        om.bind("ti.uia.runtime.IUIATransfer.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.runtime.IUIATransfer$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.runtime.IUIATransfer.Instance_State", new Proto.Str(po, false));
        om.bind("ti.uia.runtime.IUIATransfer.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.runtime.IUIATransfer.Object", om.findStrict("ti.uia.runtime.IUIATransfer.Instance_State", "ti.uia.runtime"));
        }//isROV
    }

    void ILoggerSnapshot$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.ILoggerSnapshot.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.ILoggerSnapshot", new Value.Obj("ti.uia.runtime.ILoggerSnapshot", po));
        pkgV.bind("ILoggerSnapshot", vo);
        // decls 
        om.bind("ti.uia.runtime.ILoggerSnapshot.TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.runtime"));
        om.bind("ti.uia.runtime.ILoggerSnapshot.Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.runtime"));
        om.bind("ti.uia.runtime.ILoggerSnapshot.MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.runtime"));
        // insts 
        Object insP = om.bind("ti.uia.runtime.ILoggerSnapshot.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.runtime.ILoggerSnapshot$$Object", new Proto.Obj());
        om.bind("ti.uia.runtime.ILoggerSnapshot.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.runtime.ILoggerSnapshot$$Params", new Proto.Obj());
        om.bind("ti.uia.runtime.ILoggerSnapshot.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.runtime.ILoggerSnapshot$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.runtime.ILoggerSnapshot.Instance_State", new Proto.Str(po, false));
        om.bind("ti.uia.runtime.ILoggerSnapshot.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.runtime.ILoggerSnapshot.Object", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Instance_State", "ti.uia.runtime"));
        }//isROV
    }

    void IUIATimestampProvider$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.IUIATimestampProvider.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.IUIATimestampProvider", new Value.Obj("ti.uia.runtime.IUIATimestampProvider", po));
        pkgV.bind("IUIATimestampProvider", vo);
        // decls 
    }

    void IUIATraceSyncProvider$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.IUIATraceSyncProvider.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.IUIATraceSyncProvider", new Value.Obj("ti.uia.runtime.IUIATraceSyncProvider", po));
        pkgV.bind("IUIATraceSyncProvider", vo);
        // decls 
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType", new Proto.Enm("ti.uia.runtime.IUIATraceSyncProvider.ContextType"));
    }

    void IServiceMgrSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.IServiceMgrSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.IServiceMgrSupport", new Value.Obj("ti.uia.runtime.IServiceMgrSupport", po));
        pkgV.bind("IServiceMgrSupport", vo);
        // decls 
    }

    void LogSync_CpuTimestampProxy$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.LogSync_CpuTimestampProxy.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.LogSync_CpuTimestampProxy", new Value.Obj("ti.uia.runtime.LogSync_CpuTimestampProxy", po));
        pkgV.bind("LogSync_CpuTimestampProxy", vo);
        // decls 
    }

    void LogSync_GlobalTimestampProxy$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.LogSync_GlobalTimestampProxy.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.LogSync_GlobalTimestampProxy", new Value.Obj("ti.uia.runtime.LogSync_GlobalTimestampProxy", po));
        pkgV.bind("LogSync_GlobalTimestampProxy", vo);
        // decls 
    }

    void ServiceMgr_SupportProxy$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.runtime.ServiceMgr_SupportProxy.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.runtime.ServiceMgr_SupportProxy", new Value.Obj("ti.uia.runtime.ServiceMgr_SupportProxy", po));
        pkgV.bind("ServiceMgr_SupportProxy", vo);
        // decls 
    }

    void CtxFilter$$CONSTS()
    {
        // module CtxFilter
        om.bind("ti.uia.runtime.CtxFilter.ALWAYS_ENABLED", 0x0000L);
        om.bind("ti.uia.runtime.CtxFilter.CONTEXT_ENABLED", true);
        om.bind("ti.uia.runtime.CtxFilter.CONTEXT_DISABLED", false);
        om.bind("ti.uia.runtime.CtxFilter.isCtxEnabled", new Extern("ti_uia_runtime_CtxFilter_isCtxEnabled__E", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.uia.runtime.CtxFilter.setCtxEnabled", new Extern("ti_uia_runtime_CtxFilter_setCtxEnabled__E", "xdc_Bool(*)(xdc_Bool)", true, false));
        om.bind("ti.uia.runtime.CtxFilter.setContextFilterFlags", new Extern("ti_uia_runtime_CtxFilter_setContextFilterFlags__E", "xdc_Void(*)(xdc_Bits16)", true, false));
        om.bind("ti.uia.runtime.CtxFilter.isLoggingEnabledForAppCtx", new Extern("ti_uia_runtime_CtxFilter_isLoggingEnabledForAppCtx__E", "xdc_Bool(*)(xdc_Int)", true, false));
        om.bind("ti.uia.runtime.CtxFilter.isLoggingEnabledForChanCtx", new Extern("ti_uia_runtime_CtxFilter_isLoggingEnabledForChanCtx__E", "xdc_Bool(*)(xdc_Int)", true, false));
        om.bind("ti.uia.runtime.CtxFilter.isLoggingEnabledForFrameCtx", new Extern("ti_uia_runtime_CtxFilter_isLoggingEnabledForFrameCtx__E", "xdc_Bool(*)(xdc_Int)", true, false));
        om.bind("ti.uia.runtime.CtxFilter.isLoggingEnabledForThreadCtx", new Extern("ti_uia_runtime_CtxFilter_isLoggingEnabledForThreadCtx__E", "xdc_Bool(*)(xdc_Int)", true, false));
        om.bind("ti.uia.runtime.CtxFilter.isLoggingEnabledForUserCtx", new Extern("ti_uia_runtime_CtxFilter_isLoggingEnabledForUserCtx__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void EventHdr$$CONSTS()
    {
        // module EventHdr
        om.bind("ti.uia.runtime.EventHdr.HdrType_Event", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Event", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_EventWithTimestamp", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_EventWithTimestamp", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotId", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotId", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotIdAndTimestamp", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotIdAndTimestamp", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_EventWith32bTimestamp", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_EventWith32bTimestamp", xdc.services.intern.xsr.Enum.intValue(4L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_EventWithTimestampAndEndpointId", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_EventWithTimestampAndEndpointId", xdc.services.intern.xsr.Enum.intValue(5L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotIdAnd32bTimestamp", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotIdAnd32bTimestamp", xdc.services.intern.xsr.Enum.intValue(6L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotIdAndTimestampAndEndpointId", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotIdAndTimestampAndEndpointId", xdc.services.intern.xsr.Enum.intValue(7L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_EventWithPrevLenWithNoTimestamp", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_EventWithPrevLenWithNoTimestamp", xdc.services.intern.xsr.Enum.intValue(8L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_EventWithPrevLenWith32bTimestamp", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_EventWithPrevLenWith32bTimestamp", xdc.services.intern.xsr.Enum.intValue(9L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_EventWithPrevLenWith64bTimestamp", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_EventWithPrevLenWith64bTimestamp", xdc.services.intern.xsr.Enum.intValue(10L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_SnapshotEventWithPrevLen", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_SnapshotEventWithPrevLen", xdc.services.intern.xsr.Enum.intValue(11L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_SnapshotEventWithPrevLenAnd64bTimestamp", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_SnapshotEventWithPrevLenAnd64bTimestamp", xdc.services.intern.xsr.Enum.intValue(12L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved13", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved13", xdc.services.intern.xsr.Enum.intValue(13L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved14", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved14", xdc.services.intern.xsr.Enum.intValue(14L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved15", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved15", xdc.services.intern.xsr.Enum.intValue(15L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved16", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved16", xdc.services.intern.xsr.Enum.intValue(16L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved17", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved17", xdc.services.intern.xsr.Enum.intValue(17L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved18", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved18", xdc.services.intern.xsr.Enum.intValue(18L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved19", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved19", xdc.services.intern.xsr.Enum.intValue(19L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved20", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved20", xdc.services.intern.xsr.Enum.intValue(20L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved21", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved21", xdc.services.intern.xsr.Enum.intValue(21L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved22", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved22", xdc.services.intern.xsr.Enum.intValue(22L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved23", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved23", xdc.services.intern.xsr.Enum.intValue(23L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved24", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved24", xdc.services.intern.xsr.Enum.intValue(24L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved25", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved25", xdc.services.intern.xsr.Enum.intValue(25L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved26", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved26", xdc.services.intern.xsr.Enum.intValue(26L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved27", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved27", xdc.services.intern.xsr.Enum.intValue(27L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved28", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved28", xdc.services.intern.xsr.Enum.intValue(28L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved29", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved29", xdc.services.intern.xsr.Enum.intValue(29L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved30", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved30", xdc.services.intern.xsr.Enum.intValue(30L)+0));
        om.bind("ti.uia.runtime.EventHdr.HdrType_Reserved31", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"), "ti.uia.runtime.EventHdr.HdrType_Reserved31", xdc.services.intern.xsr.Enum.intValue(31L)+0));
    }

    void LogCtxChg$$CONSTS()
    {
        // module LogCtxChg
    }

    void IUIATraceSyncClient$$CONSTS()
    {
        // interface IUIATraceSyncClient
    }

    void LogSnapshot$$CONSTS()
    {
        // module LogSnapshot
        om.bind("ti.uia.runtime.LogSnapshot.getSnapshotId", new Extern("ti_uia_runtime_LogSnapshot_getSnapshotId__E", "xdc_UArg(*)(xdc_Void)", true, false));
        om.bind("ti.uia.runtime.LogSnapshot.doPrint", new Extern("ti_uia_runtime_LogSnapshot_doPrint__E", "xdc_Void(*)(ti_uia_runtime_LogSnapshot_EventRec*)", true, false));
    }

    void LogSync$$CONSTS()
    {
        // module LogSync
        om.bind("ti.uia.runtime.LogSync.LoggerType_NONE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.LogSync.LoggerType", "ti.uia.runtime"), "ti.uia.runtime.LogSync.LoggerType_NONE", 0));
        om.bind("ti.uia.runtime.LogSync.LoggerType_MIN", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.LogSync.LoggerType", "ti.uia.runtime"), "ti.uia.runtime.LogSync.LoggerType_MIN", 1));
        om.bind("ti.uia.runtime.LogSync.LoggerType_STOPMODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.LogSync.LoggerType", "ti.uia.runtime"), "ti.uia.runtime.LogSync.LoggerType_STOPMODE", 2));
        om.bind("ti.uia.runtime.LogSync.LoggerType_RUNMODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.LogSync.LoggerType", "ti.uia.runtime"), "ti.uia.runtime.LogSync.LoggerType_RUNMODE", 3));
        om.bind("ti.uia.runtime.LogSync.LoggerType_IDLE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.LogSync.LoggerType", "ti.uia.runtime"), "ti.uia.runtime.LogSync.LoggerType_IDLE", 4));
        om.bind("ti.uia.runtime.LogSync.LoggerType_STREAMER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.LogSync.LoggerType", "ti.uia.runtime"), "ti.uia.runtime.LogSync.LoggerType_STREAMER", 5));
        om.bind("ti.uia.runtime.LogSync.LoggerType_STREAMER2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.LogSync.LoggerType", "ti.uia.runtime"), "ti.uia.runtime.LogSync.LoggerType_STREAMER2", 6));
        om.bind("ti.uia.runtime.LogSync.enable", new Extern("ti_uia_runtime_LogSync_enable__E", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.uia.runtime.LogSync.disable", new Extern("ti_uia_runtime_LogSync_disable__E", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.uia.runtime.LogSync.idleHook", new Extern("ti_uia_runtime_LogSync_idleHook__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.runtime.LogSync.timerHook", new Extern("ti_uia_runtime_LogSync_timerHook__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.uia.runtime.LogSync.putSyncPoint", new Extern("ti_uia_runtime_LogSync_putSyncPoint__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.runtime.LogSync.writeSyncPointRaw", new Extern("ti_uia_runtime_LogSync_writeSyncPointRaw__E", "xdc_Void(*)(xdc_runtime_Types_Timestamp64*,xdc_runtime_Types_Timestamp64*,xdc_runtime_Types_FreqHz*)", true, false));
        om.bind("ti.uia.runtime.LogSync.isSyncEventRequired", new Extern("ti_uia_runtime_LogSync_isSyncEventRequired__E", "xdc_Bool(*)(xdc_Void)", true, false));
    }

    void LoggerSM$$CONSTS()
    {
        // module LoggerSM
        om.bind("ti.uia.runtime.LoggerSM.VERSION", 1L);
        om.bind("ti.uia.runtime.LoggerSM.setPartitionId", new Extern("ti_uia_runtime_LoggerSM_setPartitionId__E", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.uia.runtime.LoggerSM.setSharedMemory", new Extern("ti_uia_runtime_LoggerSM_setSharedMemory__E", "xdc_Bool(*)(xdc_Ptr,xdc_Bits32)", true, false));
        om.bind("ti.uia.runtime.LoggerSM.filterOutEvent", new Extern("ti_uia_runtime_LoggerSM_filterOutEvent__I", "xdc_Bool(*)(xdc_Bits16)", true, false));
    }

    void LoggerTypes$$CONSTS()
    {
        // module LoggerTypes
    }

    void MultiCoreTypes$$CONSTS()
    {
        // module MultiCoreTypes
        om.bind("ti.uia.runtime.MultiCoreTypes.Action_TOHOST", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.MultiCoreTypes.Action", "ti.uia.runtime"), "ti.uia.runtime.MultiCoreTypes.Action_TOHOST", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.runtime.MultiCoreTypes.Action_FROMHOST", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.MultiCoreTypes.Action", "ti.uia.runtime"), "ti.uia.runtime.MultiCoreTypes.Action_FROMHOST", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.MultiCoreTypes.Action_TOHOST", "ti.uia.runtime"))+1));
        om.bind("ti.uia.runtime.MultiCoreTypes.Action_REGISTER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.MultiCoreTypes.Action", "ti.uia.runtime"), "ti.uia.runtime.MultiCoreTypes.Action_REGISTER", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.MultiCoreTypes.Action_TOHOST", "ti.uia.runtime"))+2));
        om.bind("ti.uia.runtime.MultiCoreTypes.Action_STOP", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.MultiCoreTypes.Action", "ti.uia.runtime"), "ti.uia.runtime.MultiCoreTypes.Action_STOP", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.MultiCoreTypes.Action_TOHOST", "ti.uia.runtime"))+3));
        om.bind("ti.uia.runtime.MultiCoreTypes.Action_STOPACK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.MultiCoreTypes.Action", "ti.uia.runtime"), "ti.uia.runtime.MultiCoreTypes.Action_STOPACK", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.MultiCoreTypes.Action_TOHOST", "ti.uia.runtime"))+4));
        om.bind("ti.uia.runtime.MultiCoreTypes.SLAVENAME", "uiaSlave");
        om.bind("ti.uia.runtime.MultiCoreTypes.MASTERNAME", "uiaMaster");
        om.bind("ti.uia.runtime.MultiCoreTypes.MASTERSTARTED", "uiaStarted");
    }

    void QueueDescriptor$$CONSTS()
    {
        // module QueueDescriptor
        om.bind("ti.uia.runtime.QueueDescriptor.QueueType_NONE", 0L);
        om.bind("ti.uia.runtime.QueueDescriptor.QueueType_TOHOST_CMD_CIRCULAR_BUFFER", 1L);
        om.bind("ti.uia.runtime.QueueDescriptor.QueueType_FROMHOST_CMD_CIRCULAR_BUFFER", 2L);
        om.bind("ti.uia.runtime.QueueDescriptor.QueueType_TOHOST_EVENT_CIRCULAR_BUFFER", 3L);
        om.bind("ti.uia.runtime.QueueDescriptor.QueueType_TOHOST_EVENT_OVERFLOW_BUFFER", 4L);
        om.bind("ti.uia.runtime.QueueDescriptor.QueueType_TOHOST_DATA_CIRCULAR_BUFFER", 5L);
        om.bind("ti.uia.runtime.QueueDescriptor.QueueType_FROMHOST_DATA_CIRCULAR_BUFFER", 6L);
        om.bind("ti.uia.runtime.QueueDescriptor.QueueType_TOHOST_EVENT_UIAPACKET_ARRAY", 7L);
        om.bind("ti.uia.runtime.QueueDescriptor.QueueType_TOHOST_EVENT_UIAPACKET_STOPMODE", 8L);
        om.bind("ti.uia.runtime.QueueDescriptor.addToList", new Extern("ti_uia_runtime_QueueDescriptor_addToList__E", "xdc_Void(*)(ti_uia_runtime_QueueDescriptor_Header*)", true, false));
        om.bind("ti.uia.runtime.QueueDescriptor.initHeader", new Extern("ti_uia_runtime_QueueDescriptor_initHeader__E", "xdc_Void(*)(ti_uia_runtime_QueueDescriptor_Header*,xdc_Ptr,xdc_SizeT,xdc_UInt,xdc_UInt,xdc_UInt,xdc_UInt,xdc_Ptr)", true, false));
        om.bind("ti.uia.runtime.QueueDescriptor.removeFromList", new Extern("ti_uia_runtime_QueueDescriptor_removeFromList__E", "xdc_Void(*)(ti_uia_runtime_QueueDescriptor_Header*)", true, false));
    }

    void UIAMetaData$$CONSTS()
    {
        // module UIAMetaData
        om.bind("ti.uia.runtime.UIAMetaData.UploadMode_SIMULATOR", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode", "ti.uia.runtime"), "ti.uia.runtime.UIAMetaData.UploadMode_SIMULATOR", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.runtime.UIAMetaData.UploadMode_PROBEPOINT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode", "ti.uia.runtime"), "ti.uia.runtime.UIAMetaData.UploadMode_PROBEPOINT", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.runtime.UIAMetaData.UploadMode_JTAGSTOPMODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode", "ti.uia.runtime"), "ti.uia.runtime.UIAMetaData.UploadMode_JTAGSTOPMODE", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.runtime.UIAMetaData.UploadMode_JTAGRUNMODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode", "ti.uia.runtime"), "ti.uia.runtime.UIAMetaData.UploadMode_JTAGRUNMODE", xdc.services.intern.xsr.Enum.intValue(4L)+0));
        om.bind("ti.uia.runtime.UIAMetaData.UploadMode_NONJTAGTRANSPORT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode", "ti.uia.runtime"), "ti.uia.runtime.UIAMetaData.UploadMode_NONJTAGTRANSPORT", xdc.services.intern.xsr.Enum.intValue(5L)+0));
        om.bind("ti.uia.runtime.UIAMetaData.UploadMode_CUSTOM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode", "ti.uia.runtime"), "ti.uia.runtime.UIAMetaData.UploadMode_CUSTOM", xdc.services.intern.xsr.Enum.intValue(6L)+0));
    }

    void UIAPacket$$CONSTS()
    {
        // module UIAPacket
        om.bind("ti.uia.runtime.UIAPacket.HdrType_InvalidData", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_InvalidData", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_MsgWithPID", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_MsgWithPID", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_EventPktWithCRC", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_EventPktWithCRC", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_MinEventPkt", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_MinEventPkt", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_Reserved4", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_Reserved4", xdc.services.intern.xsr.Enum.intValue(4L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_Reserved5", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_Reserved5", xdc.services.intern.xsr.Enum.intValue(5L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_Reserved6", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_Reserved6", xdc.services.intern.xsr.Enum.intValue(6L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_Reserved7", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_Reserved7", xdc.services.intern.xsr.Enum.intValue(7L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_ChannelizedData", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_ChannelizedData", xdc.services.intern.xsr.Enum.intValue(8L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_Msg", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_Msg", xdc.services.intern.xsr.Enum.intValue(9L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_EventPkt", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_EventPkt", xdc.services.intern.xsr.Enum.intValue(10L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_CPUTrace", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_CPUTrace", xdc.services.intern.xsr.Enum.intValue(11L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_STMTrace", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_STMTrace", xdc.services.intern.xsr.Enum.intValue(12L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_MemoryBuffer", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_MemoryBuffer", xdc.services.intern.xsr.Enum.intValue(13L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_USER2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_USER2", xdc.services.intern.xsr.Enum.intValue(14L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HdrType_USER3", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.HdrType_USER3", xdc.services.intern.xsr.Enum.intValue(15L)+0));
        om.bind("ti.uia.runtime.UIAPacket.PayloadEndian_LITTLE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.PayloadEndian", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.PayloadEndian_LITTLE", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.runtime.UIAPacket.PayloadEndian_BIG", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.PayloadEndian", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.PayloadEndian_BIG", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_ACK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_ACK", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_CMD", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_CMD", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_RESULT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_RESULT", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_PARTIALRESULT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_PARTIALRESULT", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_NOTIFY", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_NOTIFY", xdc.services.intern.xsr.Enum.intValue(4L)+0));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_FLOWCTRL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_FLOWCTRL", xdc.services.intern.xsr.Enum.intValue(5L)+0));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_DATA", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_DATA", xdc.services.intern.xsr.Enum.intValue(6L)+0));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_RESERVED7", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_RESERVED7", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.UIAPacket.MsgType_DATA", "ti.uia.runtime"))+1));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_RESERVED8", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_RESERVED8", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.UIAPacket.MsgType_DATA", "ti.uia.runtime"))+2));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_RESERVED9", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_RESERVED9", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.UIAPacket.MsgType_DATA", "ti.uia.runtime"))+3));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_RESERVED10", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_RESERVED10", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.UIAPacket.MsgType_DATA", "ti.uia.runtime"))+4));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_RESERVED11", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_RESERVED11", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.UIAPacket.MsgType_DATA", "ti.uia.runtime"))+5));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_RESERVED12", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_RESERVED12", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.UIAPacket.MsgType_DATA", "ti.uia.runtime"))+6));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_RESERVED13", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_RESERVED13", xdc.services.intern.xsr.Enum.intValue(om.findStrict("ti.uia.runtime.UIAPacket.MsgType_DATA", "ti.uia.runtime"))+7));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_NACK_BAD_DATA", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_NACK_BAD_DATA", xdc.services.intern.xsr.Enum.intValue(14L)+0));
        om.bind("ti.uia.runtime.UIAPacket.MsgType_NACK_WITH_ERROR_CODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.MsgType_NACK_WITH_ERROR_CODE", xdc.services.intern.xsr.Enum.intValue(15L)+0));
        om.bind("ti.uia.runtime.UIAPacket.NACKErrorCode_NO_REASON_SPECIFIED", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.NACKErrorCode_NO_REASON_SPECIFIED", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.runtime.UIAPacket.NACKErrorCode_SERVICE_NOT_SUPPORTED", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.NACKErrorCode_SERVICE_NOT_SUPPORTED", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.runtime.UIAPacket.NACKErrorCode_CMD_NOT_SUPPORTED", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.NACKErrorCode_CMD_NOT_SUPPORTED", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.runtime.UIAPacket.NACKErrorCode_QUEUE_FULL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.NACKErrorCode_QUEUE_FULL", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.runtime.UIAPacket.NACKErrorCode_BAD_ENDPOINT_ADDRESS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.NACKErrorCode_BAD_ENDPOINT_ADDRESS", xdc.services.intern.xsr.Enum.intValue(4L)+0));
        om.bind("ti.uia.runtime.UIAPacket.NACKErrorCode_BAD_MESSAGE_LENGTH", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode", "ti.uia.runtime"), "ti.uia.runtime.UIAPacket.NACKErrorCode_BAD_MESSAGE_LENGTH", xdc.services.intern.xsr.Enum.intValue(5L)+0));
        om.bind("ti.uia.runtime.UIAPacket.HOST", 0xFFFFL);
        om.bind("ti.uia.runtime.UIAPacket.BROADCAST", 0xFFFEL);
        om.bind("ti.uia.runtime.UIAPacket.getLength", new Extern("ti_uia_runtime_UIAPacket_getLength__E", "xdc_Int32(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
    }

    void ServiceMgr$$CONSTS()
    {
        // module ServiceMgr
        om.bind("ti.uia.runtime.ServiceMgr.Reason_PERIODEXPIRED", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.ServiceMgr.Reason", "ti.uia.runtime"), "ti.uia.runtime.ServiceMgr.Reason_PERIODEXPIRED", 0));
        om.bind("ti.uia.runtime.ServiceMgr.Reason_REQUESTENERGY", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.ServiceMgr.Reason", "ti.uia.runtime"), "ti.uia.runtime.ServiceMgr.Reason_REQUESTENERGY", 1));
        om.bind("ti.uia.runtime.ServiceMgr.Reason_INCOMINGMSG", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.ServiceMgr.Reason", "ti.uia.runtime"), "ti.uia.runtime.ServiceMgr.Reason_INCOMINGMSG", 2));
        om.bind("ti.uia.runtime.ServiceMgr.Topology_SINGLECORE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.ServiceMgr.Topology", "ti.uia.runtime"), "ti.uia.runtime.ServiceMgr.Topology_SINGLECORE", 0));
        om.bind("ti.uia.runtime.ServiceMgr.Topology_MULTICORE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.ServiceMgr.Topology", "ti.uia.runtime"), "ti.uia.runtime.ServiceMgr.Topology_MULTICORE", 1));
        om.bind("ti.uia.runtime.ServiceMgr.TransportType_ETHERNET", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.ServiceMgr.TransportType", "ti.uia.runtime"), "ti.uia.runtime.ServiceMgr.TransportType_ETHERNET", 0));
        om.bind("ti.uia.runtime.ServiceMgr.TransportType_FILE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.ServiceMgr.TransportType", "ti.uia.runtime"), "ti.uia.runtime.ServiceMgr.TransportType_FILE", 1));
        om.bind("ti.uia.runtime.ServiceMgr.TransportType_USER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.ServiceMgr.TransportType", "ti.uia.runtime"), "ti.uia.runtime.ServiceMgr.TransportType_USER", 2));
        om.bind("ti.uia.runtime.ServiceMgr.TransportType_NULL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.ServiceMgr.TransportType", "ti.uia.runtime"), "ti.uia.runtime.ServiceMgr.TransportType_NULL", 3));
        om.bind("ti.uia.runtime.ServiceMgr.WAIT_FOREVER", Global.eval("~(0)"));
        om.bind("ti.uia.runtime.ServiceMgr.freePacket", new Extern("ti_uia_runtime_ServiceMgr_freePacket__E", "xdc_Void(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.runtime.ServiceMgr.getFreePacket", new Extern("ti_uia_runtime_ServiceMgr_getFreePacket__E", "ti_uia_runtime_UIAPacket_Hdr*(*)(ti_uia_runtime_UIAPacket_HdrType,xdc_UInt)", true, false));
        om.bind("ti.uia.runtime.ServiceMgr.getNumServices", new Extern("ti_uia_runtime_ServiceMgr_getNumServices__E", "xdc_Int(*)(xdc_Void)", true, false));
        om.bind("ti.uia.runtime.ServiceMgr.processCallback", new Extern("ti_uia_runtime_ServiceMgr_processCallback__E", "xdc_Void(*)(ti_uia_runtime_ServiceMgr_ServiceId,ti_uia_runtime_ServiceMgr_Reason,ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.runtime.ServiceMgr.requestEnergy", new Extern("ti_uia_runtime_ServiceMgr_requestEnergy__E", "xdc_Void(*)(ti_uia_runtime_ServiceMgr_ServiceId)", true, false));
        om.bind("ti.uia.runtime.ServiceMgr.sendPacket", new Extern("ti_uia_runtime_ServiceMgr_sendPacket__E", "xdc_Bool(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.runtime.ServiceMgr.setPeriod", new Extern("ti_uia_runtime_ServiceMgr_setPeriod__E", "xdc_Void(*)(ti_uia_runtime_ServiceMgr_ServiceId,xdc_UInt32)", true, false));
    }

    void Transport$$CONSTS()
    {
        // module Transport
    }

    void ICtxFilterCallback$$CONSTS()
    {
        // interface ICtxFilterCallback
    }

    void IUIATransfer$$CONSTS()
    {
        // interface IUIATransfer
        om.bind("ti.uia.runtime.IUIATransfer.TransferType_RELIABLE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.runtime"), "ti.uia.runtime.IUIATransfer.TransferType_RELIABLE", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.runtime.IUIATransfer.TransferType_LOSSY", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.runtime"), "ti.uia.runtime.IUIATransfer.TransferType_LOSSY", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.runtime.IUIATransfer.Priority_LOW", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.runtime"), "ti.uia.runtime.IUIATransfer.Priority_LOW", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.runtime.IUIATransfer.Priority_STANDARD", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.runtime"), "ti.uia.runtime.IUIATransfer.Priority_STANDARD", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.runtime.IUIATransfer.Priority_HIGH", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.runtime"), "ti.uia.runtime.IUIATransfer.Priority_HIGH", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.runtime.IUIATransfer.Priority_SYNC", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.runtime"), "ti.uia.runtime.IUIATransfer.Priority_SYNC", xdc.services.intern.xsr.Enum.intValue(3L)+0));
    }

    void ILoggerSnapshot$$CONSTS()
    {
        // interface ILoggerSnapshot
    }

    void IUIATimestampProvider$$CONSTS()
    {
        // interface IUIATimestampProvider
    }

    void IUIATraceSyncProvider$$CONSTS()
    {
        // interface IUIATraceSyncProvider
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved0", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved0", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_SyncPoint", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_SyncPoint", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_ContextChange", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_ContextChange", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Snapshot", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Snapshot", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved4", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved4", xdc.services.intern.xsr.Enum.intValue(4L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved5", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved5", xdc.services.intern.xsr.Enum.intValue(5L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved6", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved6", xdc.services.intern.xsr.Enum.intValue(6L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved7", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved7", xdc.services.intern.xsr.Enum.intValue(7L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved8", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved8", xdc.services.intern.xsr.Enum.intValue(8L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved9", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved9", xdc.services.intern.xsr.Enum.intValue(9L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved10", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved10", xdc.services.intern.xsr.Enum.intValue(10L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved11", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved11", xdc.services.intern.xsr.Enum.intValue(11L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved12", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved12", xdc.services.intern.xsr.Enum.intValue(12L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved13", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved13", xdc.services.intern.xsr.Enum.intValue(13L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Global32bTimestamp", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_Global32bTimestamp", xdc.services.intern.xsr.Enum.intValue(14L)+0));
        om.bind("ti.uia.runtime.IUIATraceSyncProvider.ContextType_User", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"), "ti.uia.runtime.IUIATraceSyncProvider.ContextType_User", xdc.services.intern.xsr.Enum.intValue(15L)+0));
    }

    void IServiceMgrSupport$$CONSTS()
    {
        // interface IServiceMgrSupport
    }

    void LogSync_CpuTimestampProxy$$CONSTS()
    {
        // module LogSync_CpuTimestampProxy
        om.bind("ti.uia.runtime.LogSync_CpuTimestampProxy.get32", new Extern("ti_uia_runtime_LogSync_CpuTimestampProxy_get32__E", "xdc_Bits32(*)(xdc_Void)", true, false));
        om.bind("ti.uia.runtime.LogSync_CpuTimestampProxy.get64", new Extern("ti_uia_runtime_LogSync_CpuTimestampProxy_get64__E", "xdc_Void(*)(xdc_runtime_Types_Timestamp64*)", true, false));
        om.bind("ti.uia.runtime.LogSync_CpuTimestampProxy.getFreq", new Extern("ti_uia_runtime_LogSync_CpuTimestampProxy_getFreq__E", "xdc_Void(*)(xdc_runtime_Types_FreqHz*)", true, false));
    }

    void LogSync_GlobalTimestampProxy$$CONSTS()
    {
        // module LogSync_GlobalTimestampProxy
        om.bind("ti.uia.runtime.LogSync_GlobalTimestampProxy.get32", new Extern("ti_uia_runtime_LogSync_GlobalTimestampProxy_get32__E", "xdc_Bits32(*)(xdc_Void)", true, false));
        om.bind("ti.uia.runtime.LogSync_GlobalTimestampProxy.get64", new Extern("ti_uia_runtime_LogSync_GlobalTimestampProxy_get64__E", "xdc_Void(*)(xdc_runtime_Types_Timestamp64*)", true, false));
        om.bind("ti.uia.runtime.LogSync_GlobalTimestampProxy.getFreq", new Extern("ti_uia_runtime_LogSync_GlobalTimestampProxy_getFreq__E", "xdc_Void(*)(xdc_runtime_Types_FreqHz*)", true, false));
    }

    void ServiceMgr_SupportProxy$$CONSTS()
    {
        // module ServiceMgr_SupportProxy
        om.bind("ti.uia.runtime.ServiceMgr_SupportProxy.freePacket", new Extern("ti_uia_runtime_ServiceMgr_SupportProxy_freePacket__E", "xdc_Void(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.runtime.ServiceMgr_SupportProxy.getFreePacket", new Extern("ti_uia_runtime_ServiceMgr_SupportProxy_getFreePacket__E", "ti_uia_runtime_UIAPacket_Hdr*(*)(ti_uia_runtime_UIAPacket_HdrType,xdc_UInt)", true, false));
        om.bind("ti.uia.runtime.ServiceMgr_SupportProxy.requestEnergy", new Extern("ti_uia_runtime_ServiceMgr_SupportProxy_requestEnergy__E", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.uia.runtime.ServiceMgr_SupportProxy.sendPacket", new Extern("ti_uia_runtime_ServiceMgr_SupportProxy_sendPacket__E", "xdc_Bool(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.runtime.ServiceMgr_SupportProxy.setPeriod", new Extern("ti_uia_runtime_ServiceMgr_SupportProxy_setPeriod__E", "xdc_Void(*)(xdc_Int,xdc_UInt32)", true, false));
    }

    void CtxFilter$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void EventHdr$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LogCtxChg$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IUIATraceSyncClient$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LogSnapshot$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LogSync$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$uia$runtime$LogSync$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.uia.runtime.LogSync.create() called before xdc.useModule('ti.uia.runtime.LogSync')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.uia.runtime.LogSync$$create", new Proto.Fxn(om.findStrict("ti.uia.runtime.LogSync.Module", "ti.uia.runtime"), om.findStrict("ti.uia.runtime.LogSync.Instance", "ti.uia.runtime"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.uia.runtime.LogSync.Params", "ti.uia.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$runtime$LogSync$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.runtime.LogSync'];\n");
                sb.append("var __inst = xdc.om['ti.uia.runtime.LogSync.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.uia.runtime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.uia.runtime.LogSync'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.runtime.LogSync'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.uia.runtime.LogSync$$construct", new Proto.Fxn(om.findStrict("ti.uia.runtime.LogSync.Module", "ti.uia.runtime"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.uia.runtime.LogSync$$Object", "ti.uia.runtime"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.uia.runtime.LogSync.Params", "ti.uia.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$runtime$LogSync$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.runtime.LogSync'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.uia.runtime.LogSync'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.runtime.LogSync'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void LoggerSM$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$uia$runtime$LoggerSM$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.uia.runtime.LoggerSM.create() called before xdc.useModule('ti.uia.runtime.LoggerSM')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.uia.runtime.LoggerSM$$create", new Proto.Fxn(om.findStrict("ti.uia.runtime.LoggerSM.Module", "ti.uia.runtime"), om.findStrict("ti.uia.runtime.LoggerSM.Instance", "ti.uia.runtime"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.uia.runtime.LoggerSM.Params", "ti.uia.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$runtime$LoggerSM$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.runtime.LoggerSM'];\n");
                sb.append("var __inst = xdc.om['ti.uia.runtime.LoggerSM.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.uia.runtime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.uia.runtime.LoggerSM'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.runtime.LoggerSM'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.uia.runtime.LoggerSM$$construct", new Proto.Fxn(om.findStrict("ti.uia.runtime.LoggerSM.Module", "ti.uia.runtime"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.uia.runtime.LoggerSM$$Object", "ti.uia.runtime"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.uia.runtime.LoggerSM.Params", "ti.uia.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$runtime$LoggerSM$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.runtime.LoggerSM'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.uia.runtime.LoggerSM'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.runtime.LoggerSM'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void LoggerTypes$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void MultiCoreTypes$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void QueueDescriptor$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAMetaData$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void UIAPacket$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ServiceMgr$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Transport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ICtxFilterCallback$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IUIATransfer$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ILoggerSnapshot$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IUIATimestampProvider$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IUIATraceSyncProvider$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IServiceMgrSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LogSync_CpuTimestampProxy$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LogSync_GlobalTimestampProxy$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ServiceMgr_SupportProxy$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void CtxFilter$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void EventHdr$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LogCtxChg$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IUIATraceSyncClient$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LogSnapshot$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LogSync$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn LogSync.finalize
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.LogSync$$finalize", new Proto.Fxn(om.findStrict("ti.uia.runtime.LogSync.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn LogSync.isUsedByRta
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.LogSync$$isUsedByRta", new Proto.Fxn(om.findStrict("ti.uia.runtime.LogSync.Module", "ti.uia.runtime"), null, 0, -1, false));
    }

    void LoggerSM$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LoggerTypes$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void MultiCoreTypes$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void QueueDescriptor$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn QueueDescriptor.generateInstanceId
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.QueueDescriptor$$generateInstanceId", new Proto.Fxn(om.findStrict("ti.uia.runtime.QueueDescriptor.Module", "ti.uia.runtime"), Proto.Elm.newCNum("(xdc_UInt16)"), 0, 0, false));
    }

    void UIAMetaData$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn UIAMetaData.generateXML
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$generateXML", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn UIAMetaData.genXmlEntry
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$genXmlEntry", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn UIAMetaData.genXmlEntryOpen
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$genXmlEntryOpen", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn UIAMetaData.genXmlEntryOpenWithKey
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$genXmlEntryOpenWithKey", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn UIAMetaData.genXmlEntryWithKey
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$genXmlEntryWithKey", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn UIAMetaData.genXmlComment
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$genXmlComment", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn UIAMetaData.genXmlEntryClose
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$genXmlEntryClose", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn UIAMetaData.setTransportFields
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$setTransportFields", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn UIAMetaData.setLoggingSetupConfigured
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$setLoggingSetupConfigured", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn UIAMetaData.isLoggingSetupConfigured
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$isLoggingSetupConfigured", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn UIAMetaData.setLogSyncInfo
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.UIAMetaData$$setLogSyncInfo", new Proto.Fxn(om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime"), null, 0, -1, false));
    }

    void UIAPacket$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ServiceMgr$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ServiceMgr.register
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.ServiceMgr$$register", new Proto.Fxn(om.findStrict("ti.uia.runtime.ServiceMgr.Module", "ti.uia.runtime"), Proto.Elm.newCNum("(xdc_Int)"), 3, 3, false));
                fxn.addArg(0, "id", (Proto)om.findStrict("ti.uia.runtime.ServiceMgr$$ServiceId", "ti.uia.runtime"), $$DEFAULT);
                fxn.addArg(1, "processCallbackFxn", new Proto.Adr("xdc_Void(*)(ti_uia_runtime_ServiceMgr_Reason,ti_uia_runtime_UIAPacket_Hdr*)", "PFv"), $$UNDEF);
                fxn.addArg(2, "periodInMs", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF);
    }

    void Transport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ICtxFilterCallback$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IUIATransfer$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn IUIATransfer.getPtrToQueueDescriptorMeta
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.IUIATransfer$$getPtrToQueueDescriptorMeta", new Proto.Fxn(om.findStrict("ti.uia.runtime.IUIATransfer.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn IUIATransfer.setPtrToQueueDescriptorMeta
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.IUIATransfer$$setPtrToQueueDescriptorMeta", new Proto.Fxn(om.findStrict("ti.uia.runtime.IUIATransfer.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn IUIATransfer.getLoggerInstanceId
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.IUIATransfer$$getLoggerInstanceId", new Proto.Fxn(om.findStrict("ti.uia.runtime.IUIATransfer.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn IUIATransfer.getLoggerPriority
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.IUIATransfer$$getLoggerPriority", new Proto.Fxn(om.findStrict("ti.uia.runtime.IUIATransfer.Module", "ti.uia.runtime"), null, 0, -1, false));
        // fxn IUIATransfer.setLoggerPriority
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.IUIATransfer$$setLoggerPriority", new Proto.Fxn(om.findStrict("ti.uia.runtime.IUIATransfer.Module", "ti.uia.runtime"), null, 0, -1, false));
    }

    void ILoggerSnapshot$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IUIATimestampProvider$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IUIATraceSyncProvider$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IServiceMgrSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn IServiceMgrSupport.newService
        fxn = (Proto.Fxn)om.bind("ti.uia.runtime.IServiceMgrSupport$$newService", new Proto.Fxn(om.findStrict("ti.uia.runtime.IServiceMgrSupport.Module", "ti.uia.runtime"), null, 2, 2, false));
                fxn.addArg(0, "id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                fxn.addArg(1, "periodInMs", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF);
    }

    void LogSync_CpuTimestampProxy$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LogSync_GlobalTimestampProxy$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ServiceMgr_SupportProxy$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CtxFilter$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.runtime.CtxFilter.Module_State", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("mFlags", "UInt16"));
        sizes.add(Global.newArray("mEnableMask", "UInt16"));
        sizes.add(Global.newArray("mIsLoggingEnabled", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.CtxFilter.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.CtxFilter.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.CtxFilter.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void EventHdr$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void LogCtxChg$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void IUIATraceSyncClient$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void LogSnapshot$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.runtime.LogSnapshot.EventRec", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("tstamp", "Sxdc.runtime.Types;Timestamp64"));
        sizes.add(Global.newArray("serial", "UInt32"));
        sizes.add(Global.newArray("evt", "UInt32"));
        sizes.add(Global.newArray("snapshotId", "UIArg"));
        sizes.add(Global.newArray("fmt", "TIArg"));
        sizes.add(Global.newArray("pData", "UPtr"));
        sizes.add(Global.newArray("lengthInMAUs", "UInt16"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.LogSnapshot.EventRec']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.LogSnapshot.EventRec']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.LogSnapshot.EventRec'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LogSync$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.runtime.LogSync.Module_State", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("numTimesHalted", "UInt32"));
        sizes.add(Global.newArray("serialNumber", "UInt32"));
        sizes.add(Global.newArray("isEnabled", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.LogSync.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.LogSync.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.LogSync.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.runtime.LogSync.Instance_State", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.LogSync.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.LogSync.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.LogSync.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LoggerSM$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.runtime.LoggerSM.Module_State", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("partitionId", "TInt"));
        sizes.add(Global.newArray("level1", "UInt16"));
        sizes.add(Global.newArray("level2", "UInt16"));
        sizes.add(Global.newArray("level3", "UInt16"));
        sizes.add(Global.newArray("sharedObj", "UPtr"));
        sizes.add(Global.newArray("sharedBuffer", "UPtr"));
        sizes.add(Global.newArray("serial", "UInt16"));
        sizes.add(Global.newArray("enabled", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.LoggerSM.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.LoggerSM.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.LoggerSM.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.runtime.LoggerSM.SharedObj", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("headerTag", "UInt32"));
        sizes.add(Global.newArray("version", "UInt32"));
        sizes.add(Global.newArray("numPartitions", "UInt32"));
        sizes.add(Global.newArray("endPtr", "UPtr"));
        sizes.add(Global.newArray("readPtr", "UPtr"));
        sizes.add(Global.newArray("writePtr", "UPtr"));
        sizes.add(Global.newArray("buffer", "UPtr"));
        sizes.add(Global.newArray("bufferSizeMAU", "UInt32"));
        sizes.add(Global.newArray("droppedEvents", "UInt32"));
        sizes.add(Global.newArray("moduleId", "UInt16"));
        sizes.add(Global.newArray("instanceId", "UInt16"));
        sizes.add(Global.newArray("decode", "UInt16"));
        sizes.add(Global.newArray("overwrite", "UInt16"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.LoggerSM.SharedObj']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.LoggerSM.SharedObj']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.LoggerSM.SharedObj'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.runtime.LoggerSM.Instance_State", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.LoggerSM.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.LoggerSM.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.LoggerSM.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LoggerTypes$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void MultiCoreTypes$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.runtime.MultiCoreTypes.ServiceHdr", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("msgHdr", "A32;TChar"));
        sizes.add(Global.newArray("packet", "Sti.uia.runtime.UIAPacket;Hdr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.MultiCoreTypes.ServiceHdr']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.MultiCoreTypes.ServiceHdr']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.MultiCoreTypes.ServiceHdr'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.runtime.MultiCoreTypes.RegisterMsg", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("msgHdr", "A32;TChar"));
        sizes.add(Global.newArray("remoteMQ", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.MultiCoreTypes.RegisterMsg']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.MultiCoreTypes.RegisterMsg']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.MultiCoreTypes.RegisterMsg'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void QueueDescriptor$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.runtime.QueueDescriptor.Header", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("structSize", "TInt"));
        sizes.add(Global.newArray("next", "UPtr"));
        sizes.add(Global.newArray("queueType", "UInt"));
        sizes.add(Global.newArray("readPtr", "UPtr"));
        sizes.add(Global.newArray("writePtr", "UPtr"));
        sizes.add(Global.newArray("queueStartAdrs", "UPtr"));
        sizes.add(Global.newArray("queueSizeInMAUs", "USize"));
        sizes.add(Global.newArray("instanceId", "UInt"));
        sizes.add(Global.newArray("ownerModuleId", "UInt"));
        sizes.add(Global.newArray("priority", "UInt"));
        sizes.add(Global.newArray("numDroppedCtrAdrs", "UPtr"));
        sizes.add(Global.newArray("partialPacketWritePtr", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.QueueDescriptor.Header']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.QueueDescriptor.Header']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.QueueDescriptor.Header'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.runtime.QueueDescriptor.Module_State", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("mPtrToFirstDescriptor", "UPtr"));
        sizes.add(Global.newArray("mUpdateCount", "UInt"));
        sizes.add(Global.newArray("is5555ifInitialized", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.QueueDescriptor.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.QueueDescriptor.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.QueueDescriptor.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void UIAMetaData$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void UIAPacket$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.runtime.UIAPacket.Hdr", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("word1", "UInt32"));
        sizes.add(Global.newArray("word2", "UInt32"));
        sizes.add(Global.newArray("word3", "UInt32"));
        sizes.add(Global.newArray("word4", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.UIAPacket.Hdr']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.UIAPacket.Hdr']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.UIAPacket.Hdr'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.runtime.UIAPacket.Footer", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("word1", "TInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.UIAPacket.Footer']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.UIAPacket.Footer']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.UIAPacket.Footer'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void ServiceMgr$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.runtime.ServiceMgr.Module_State", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("runCount", "TInt"));
        sizes.add(Global.newArray("numServices", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.ServiceMgr.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.ServiceMgr.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.ServiceMgr.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Transport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.runtime.Transport.FxnSet", "ti.uia.runtime");
        sizes.clear();
        sizes.add(Global.newArray("initFxn", "UFxn"));
        sizes.add(Global.newArray("startFxn", "UFxn"));
        sizes.add(Global.newArray("recvFxn", "UFxn"));
        sizes.add(Global.newArray("sendFxn", "UFxn"));
        sizes.add(Global.newArray("stopFxn", "UFxn"));
        sizes.add(Global.newArray("exitFxn", "UFxn"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.runtime.Transport.FxnSet']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.runtime.Transport.FxnSet']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.runtime.Transport.FxnSet'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void ICtxFilterCallback$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void IUIATransfer$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void ILoggerSnapshot$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void IUIATimestampProvider$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void IUIATraceSyncProvider$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void IServiceMgrSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void LogSync_CpuTimestampProxy$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void LogSync_GlobalTimestampProxy$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void ServiceMgr_SupportProxy$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void CtxFilter$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/runtime/CtxFilter.xs");
        om.bind("ti.uia.runtime.CtxFilter$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.CtxFilter.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.CtxFilter.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ALWAYS_ENABLED", Proto.Elm.newCNum("(xdc_Bits16)"), 0x0000L, "rh");
                po.addFld("CONTEXT_ENABLED", Proto.Elm.newCNum("(xdc_Bits16)"), true, "rh");
                po.addFld("CONTEXT_DISABLED", Proto.Elm.newCNum("(xdc_Bits16)"), false, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.runtime"), $$UNDEF, "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.runtime.CtxFilter$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.CtxFilter$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.CtxFilter$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.runtime.CtxFilter$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct CtxFilter.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.CtxFilter$$ModuleView", "ti.uia.runtime");
        po.init("ti.uia.runtime.CtxFilter.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("mFlags", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("mEnableMask", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("mIsLoggingEnabled", $$T_Bool, $$UNDEF, "w");
        // typedef CtxFilter.Mask
        om.bind("ti.uia.runtime.CtxFilter.Mask", Proto.Elm.newCNum("(xdc_Bits16)"));
        // struct CtxFilter.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.CtxFilter$$Module_State", "ti.uia.runtime");
        po.init("ti.uia.runtime.CtxFilter.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("mFlags", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("mEnableMask", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("mIsLoggingEnabled", $$T_Bool, $$UNDEF, "w");
    }

    void EventHdr$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.EventHdr.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.EventHdr.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void LogCtxChg$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/runtime/LogCtxChg.xs");
        om.bind("ti.uia.runtime.LogCtxChg$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogCtxChg.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogCtxChg.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("isTimestampEnabled", $$T_Bool, true, "w");
            po.addFld("loggerDefined", $$T_Bool, false, "w");
            po.addFld("ctxFilterEnabled", $$T_Bool, false, "w");
            po.addFld("loggerObj", new Proto.Adr("xdc_Ptr", "Pv"), null, "w");
            po.addFld("loggerFxn2", new Proto.Adr("xdc_Void(*)(xdc_Ptr,xdc_Bits32,xdc_Bits16,xdc_IArg,xdc_IArg)", "PFv"), null, "w");
            po.addFld("loggerFxn8", new Proto.Adr("xdc_Void(*)(xdc_Ptr,xdc_Bits32,xdc_Bits16,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg)", "PFv"), null, "w");
            po.addFld("idToInfo", new Proto.Map($$T_Str), Global.newArray(new Object[]{}), "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.runtime.LogCtxChg$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.LogCtxChg$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.LogCtxChg$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.runtime.LogCtxChg$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void IUIATraceSyncClient$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATraceSyncClient.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.IUIATraceSyncClient.Module", om.findStrict("ti.uia.events.IUIAMetaProvider.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("injectIntoTraceFxn", new Proto.Adr("xdc_Void(*)(xdc_UInt32,ti_uia_runtime_IUIATraceSyncProvider_ContextType)", "PFv"), null, "w");
            po.addFld("isInjectIntoTraceEnabled", $$T_Bool, true, "wh");
        }//isCFG
    }

    void LogSnapshot$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/runtime/LogSnapshot.xs");
        om.bind("ti.uia.runtime.LogSnapshot$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSnapshot.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSnapshot.Module", om.findStrict("ti.uia.runtime.IUIATraceSyncClient.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("maxLengthInMAUs", Proto.Elm.newCNum("(xdc_Int)"), 512L, "w");
            po.addFld("isTimestampEnabled", $$T_Bool, true, "w");
            po.addFld("loggerDefined", $$T_Bool, false, "w");
            po.addFld("loggerObj", new Proto.Adr("xdc_Ptr", "Pv"), null, "w");
            po.addFld("loggerMemoryRangeFxn", new Proto.Adr("xdc_Void(*)(xdc_Ptr,xdc_runtime_Log_Event,xdc_UInt32,xdc_UInt32,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg)", "PFv"), null, "w");
            po.addFld("idToInfo", new Proto.Map($$T_Str), Global.newArray(new Object[]{}), "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.runtime.LogSnapshot$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.LogSnapshot$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.LogSnapshot$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.runtime.LogSnapshot$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "writeUIAMetaData");
                if (fxn != null) po.addFxn("writeUIAMetaData", (Proto.Fxn)om.findStrict("ti.uia.events.IUIAMetaProvider$$writeUIAMetaData", "ti.uia.runtime"), fxn);
        // struct LogSnapshot.EventRec
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSnapshot$$EventRec", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSnapshot.EventRec", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("tstamp", (Proto)om.findStrict("xdc.runtime.Types.Timestamp64", "ti.uia.runtime"), $$DEFAULT, "w");
                po.addFld("serial", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("evt", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("snapshotId", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("fmt", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("pData", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("lengthInMAUs", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
    }

    void LogSync$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/runtime/LogSync.xs");
        om.bind("ti.uia.runtime.LogSync$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSync.Module", om.findStrict("ti.uia.runtime.IUIATraceSyncClient.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.runtime"), $$UNDEF, "wh");
            po.addFld("loggerType", (Proto)om.findStrict("ti.uia.runtime.LogSync.LoggerType", "ti.uia.runtime"), om.find("ti.uia.runtime.LogSync.LoggerType_NONE"), "wh");
            po.addFld("syncLogger", (Proto)om.findStrict("xdc.runtime.ILogger.Handle", "ti.uia.runtime"), $$UNDEF, "wh");
            po.addFld("defaultSyncLoggerSize", Proto.Elm.newCNum("(xdc_SizeT)"), 256L, "wh");
            po.addFld("isEnabled", $$T_Bool, true, "wh");
            po.addFldV("CpuTimestampProxy", (Proto)om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.uia.runtime"), null, "wh", $$delegGet, $$delegSet);
            po.addFld("cpuTimestampCyclesPerTick", Proto.Elm.newCNum("(xdc_UInt32)"), 1L, "w");
            po.addFld("maxCpuClockFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.uia.runtime"), $$DEFAULT, "wh");
            po.addFld("canCpuFrequencyBeChanged", $$T_Bool, false, "wh");
            po.addFld("canCpuCyclesPerTickBeChanged", $$T_Bool, false, "wh");
            po.addFldV("GlobalTimestampProxy", (Proto)om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.uia.runtime"), null, "wh", $$delegGet, $$delegSet);
            po.addFld("globalTimestampCpuCyclesPerTick", Proto.Elm.newCNum("(xdc_UInt32)"), 0L, "w");
            po.addFld("maxGlobalClockFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.uia.runtime"), $$DEFAULT, "w");
            po.addFld("enableEventCorrelationForJTAG", $$T_Bool, true, "wh");
            po.addFld("hasMetaData", $$T_Bool, true, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.uia.runtime.LogSync$$create", "ti.uia.runtime"), Global.get("ti$uia$runtime$LogSync$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.uia.runtime.LogSync$$construct", "ti.uia.runtime"), Global.get("ti$uia$runtime$LogSync$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.runtime.LogSync$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.LogSync$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.LogSync$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.LogSync$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.runtime.LogSync$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.LogSync$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "writeUIAMetaData");
                if (fxn != null) po.addFxn("writeUIAMetaData", (Proto.Fxn)om.findStrict("ti.uia.events.IUIAMetaProvider$$writeUIAMetaData", "ti.uia.runtime"), fxn);
                po.addFxn("finalize", (Proto.Fxn)om.findStrict("ti.uia.runtime.LogSync$$finalize", "ti.uia.runtime"), Global.get(cap, "finalize"));
                po.addFxn("isUsedByRta", (Proto.Fxn)om.findStrict("ti.uia.runtime.LogSync$$isUsedByRta", "ti.uia.runtime"), Global.get(cap, "isUsedByRta"));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync.Instance", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSync.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync$$Params", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSync.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync$$Object", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSync.Object", om.findStrict("ti.uia.runtime.LogSync.Instance", "ti.uia.runtime"));
        // struct LogSync.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync$$ModuleView", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSync.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("numTimesHalted", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("serialNumber", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("isEnabled", $$T_Bool, $$UNDEF, "w");
        // struct LogSync.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync$$Module_State", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSync.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("numTimesHalted", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("serialNumber", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("isEnabled", $$T_Bool, $$UNDEF, "w");
        // struct LogSync.Instance_State
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync$$Instance_State", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSync.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync$$Instance_State", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSync.Instance_State", null);
        po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void LoggerSM$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/runtime/LoggerSM.xs");
        om.bind("ti.uia.runtime.LoggerSM$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.Module", om.findStrict("xdc.runtime.IFilterLogger.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("VERSION", Proto.Elm.newCNum("(xdc_UInt16)"), 1L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.runtime"), $$UNDEF, "wh");
            po.addFld("E_badLevel", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.uia.runtime"), Global.newObject("msg", "E_badLevel: Bad filter level value: %d"), "w");
            po.addFld("isTimestampEnabled", $$T_Bool, true, "w");
            po.addFld("decode", $$T_Bool, true, "w");
            po.addFld("overwrite", $$T_Bool, false, "w");
            po.addFld("level1Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level2Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level3Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level4Mask", Proto.Elm.newCNum("(xdc_Bits16)"), Global.eval("0xFF9F & (~0x0010) & (~0x0008)"), "w");
            po.addFld("partitionId", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
            po.addFld("numPartitions", Proto.Elm.newCNum("(xdc_Int)"), 3L, "w");
            po.addFld("sharedMemorySize", Proto.Elm.newCNum("(xdc_SizeT)"), 0x20000L, "w");
            po.addFld("userTimestamp", $$T_Bool, false, "wh");
            po.addFld("bufSection", $$T_Str, null, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.uia.runtime.LoggerSM$$create", "ti.uia.runtime"), Global.get("ti$uia$runtime$LoggerSM$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.uia.runtime.LoggerSM$$construct", "ti.uia.runtime"), Global.get("ti$uia$runtime$LoggerSM$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.runtime.LoggerSM$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.LoggerSM$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.LoggerSM$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.LoggerSM$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.runtime.LoggerSM$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.LoggerSM$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getMetaArgs");
                if (fxn != null) po.addFxn("getMetaArgs", (Proto.Fxn)om.findStrict("xdc.runtime.ILogger$$getMetaArgs", "ti.uia.runtime"), fxn);
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM.Instance", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.Instance", om.findStrict("xdc.runtime.IFilterLogger.Instance", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("VERSION", Proto.Elm.newCNum("(xdc_UInt16)"), 1L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$Params", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.Params", om.findStrict("xdc.runtime.IFilterLogger$$Params", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("VERSION", Proto.Elm.newCNum("(xdc_UInt16)"), 1L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$Object", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.Object", om.findStrict("ti.uia.runtime.LoggerSM.Instance", "ti.uia.runtime"));
        // struct LoggerSM.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$ModuleView", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("isTimestampEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("decode", $$T_Bool, $$UNDEF, "w");
                po.addFld("overwrite", $$T_Bool, $$UNDEF, "w");
        // struct LoggerSM.InstanceView
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$InstanceView", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.InstanceView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
        // struct LoggerSM.MetaData
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$MetaData", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.MetaData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // struct LoggerSM.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$Module_State", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("partitionId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("level1", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("level2", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("level3", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("sharedObj", new Proto.Adr("ti_uia_runtime_LoggerSM_SharedObj*", "PS"), $$UNDEF, "w");
                po.addFld("sharedBuffer", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("serial", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
        // struct LoggerSM.SharedObj
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$SharedObj", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.SharedObj", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("headerTag", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("version", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("numPartitions", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("endPtr", new Proto.Adr("xdc_Char*", "Pn"), $$UNDEF, "w");
                po.addFld("readPtr", new Proto.Adr("xdc_Char*", "Pn"), $$UNDEF, "w");
                po.addFld("writePtr", new Proto.Adr("xdc_Char*", "Pn"), $$UNDEF, "w");
                po.addFld("buffer", new Proto.Adr("xdc_Char*", "Pn"), $$UNDEF, "w");
                po.addFld("bufferSizeMAU", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("droppedEvents", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("moduleId", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("decode", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("overwrite", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
        // struct LoggerSM.Instance_State
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$Instance_State", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$Instance_State", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerSM.Instance_State", null);
        po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void LoggerTypes$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerTypes.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.LoggerTypes.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        // typedef LoggerTypes.LogMemoryRangeFxn
        om.bind("ti.uia.runtime.LoggerTypes.LogMemoryRangeFxn", new Proto.Adr("xdc_Void(*)(xdc_Ptr,xdc_runtime_Log_Event,xdc_UInt32,xdc_UInt32,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg)", "PFv"));
        // typedef LoggerTypes.InjectIntoTraceFxn
        om.bind("ti.uia.runtime.LoggerTypes.InjectIntoTraceFxn", new Proto.Adr("xdc_Void(*)(xdc_UInt32,ti_uia_runtime_IUIATraceSyncProvider_ContextType)", "PFv"));
    }

    void MultiCoreTypes$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.MultiCoreTypes.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.MultiCoreTypes.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("SLAVENAME", $$T_Str, "uiaSlave", "rh");
                po.addFld("MASTERNAME", $$T_Str, "uiaMaster", "rh");
                po.addFld("MASTERSTARTED", $$T_Str, "uiaStarted", "rh");
        if (isCFG) {
            po.addFld("A_ipcFailed", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.uia.runtime"), Global.newObject("msg", "A_ipcFailed: Unexpected MessageQ failed"), "w");
            po.addFld("A_invalidHdrType", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.uia.runtime"), Global.newObject("msg", "A_invalidHdrType: Invalid HdrType specified"), "w");
        }//isCFG
        // struct MultiCoreTypes.ServiceHdr
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.MultiCoreTypes$$ServiceHdr", "ti.uia.runtime");
        po.init("ti.uia.runtime.MultiCoreTypes.ServiceHdr", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("msgHdr", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false, xdc.services.intern.xsr.Enum.intValue(32L)), $$DEFAULT, "w");
                po.addFld("packet", (Proto)om.findStrict("ti.uia.runtime.UIAPacket.Hdr", "ti.uia.runtime"), $$DEFAULT, "w");
        // struct MultiCoreTypes.RegisterMsg
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.MultiCoreTypes$$RegisterMsg", "ti.uia.runtime");
        po.init("ti.uia.runtime.MultiCoreTypes.RegisterMsg", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("msgHdr", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false, xdc.services.intern.xsr.Enum.intValue(32L)), $$DEFAULT, "w");
                po.addFld("remoteMQ", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
    }

    void QueueDescriptor$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/runtime/QueueDescriptor.xs");
        om.bind("ti.uia.runtime.QueueDescriptor$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.QueueDescriptor.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.QueueDescriptor.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("QueueType_NONE", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
                po.addFld("QueueType_TOHOST_CMD_CIRCULAR_BUFFER", Proto.Elm.newCNum("(xdc_UInt)"), 1L, "rh");
                po.addFld("QueueType_FROMHOST_CMD_CIRCULAR_BUFFER", Proto.Elm.newCNum("(xdc_UInt)"), 2L, "rh");
                po.addFld("QueueType_TOHOST_EVENT_CIRCULAR_BUFFER", Proto.Elm.newCNum("(xdc_UInt)"), 3L, "rh");
                po.addFld("QueueType_TOHOST_EVENT_OVERFLOW_BUFFER", Proto.Elm.newCNum("(xdc_UInt)"), 4L, "rh");
                po.addFld("QueueType_TOHOST_DATA_CIRCULAR_BUFFER", Proto.Elm.newCNum("(xdc_UInt)"), 5L, "rh");
                po.addFld("QueueType_FROMHOST_DATA_CIRCULAR_BUFFER", Proto.Elm.newCNum("(xdc_UInt)"), 6L, "rh");
                po.addFld("QueueType_TOHOST_EVENT_UIAPACKET_ARRAY", Proto.Elm.newCNum("(xdc_UInt)"), 7L, "rh");
                po.addFld("QueueType_TOHOST_EVENT_UIAPACKET_STOPMODE", Proto.Elm.newCNum("(xdc_UInt)"), 8L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.runtime"), $$UNDEF, "wh");
            po.addFld("maxId", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.runtime.QueueDescriptor$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.QueueDescriptor$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.QueueDescriptor$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.runtime.QueueDescriptor$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("generateInstanceId", (Proto.Fxn)om.findStrict("ti.uia.runtime.QueueDescriptor$$generateInstanceId", "ti.uia.runtime"), Global.get(cap, "generateInstanceId"));
        // struct QueueDescriptor.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.QueueDescriptor$$ModuleView", "ti.uia.runtime");
        po.init("ti.uia.runtime.QueueDescriptor.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("mPtrToFirstDescriptor", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("mUpdateCount", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("is5555ifInitialized", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
        // struct QueueDescriptor.Header
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.QueueDescriptor$$Header", "ti.uia.runtime");
        po.init("ti.uia.runtime.QueueDescriptor.Header", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("structSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("next", new Proto.Adr("ti_uia_runtime_QueueDescriptor_Header*", "PS"), $$UNDEF, "w");
                po.addFld("queueType", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("readPtr", new Proto.Adr("xdc_Bits32*", "Pn"), $$UNDEF, "w");
                po.addFld("writePtr", new Proto.Adr("xdc_Bits32*", "Pn"), $$UNDEF, "w");
                po.addFld("queueStartAdrs", new Proto.Adr("xdc_Bits32*", "Pn"), $$UNDEF, "w");
                po.addFld("queueSizeInMAUs", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("ownerModuleId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("numDroppedCtrAdrs", new Proto.Adr("xdc_Bits32*", "Pn"), $$UNDEF, "w");
                po.addFld("partialPacketWritePtr", new Proto.Adr("xdc_Bits32*", "Pn"), $$UNDEF, "w");
        // struct QueueDescriptor.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.QueueDescriptor$$Module_State", "ti.uia.runtime");
        po.init("ti.uia.runtime.QueueDescriptor.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("mPtrToFirstDescriptor", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("mUpdateCount", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("is5555ifInitialized", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
    }

    void UIAMetaData$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/runtime/UIAMetaData.xs");
        om.bind("ti.uia.runtime.UIAMetaData$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.UIAMetaData.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("cpuFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.uia.runtime"), $$DEFAULT, "wh");
            po.addFld("timestampFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.uia.runtime"), $$DEFAULT, "wh");
            po.addFld("overrideCpuFreq", $$T_Bool, false, "wh");
            po.addFld("overrideTimestampFreq", $$T_Bool, false, "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.runtime.UIAMetaData$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.UIAMetaData$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.UIAMetaData$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.runtime.UIAMetaData$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("generateXML", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$generateXML", "ti.uia.runtime"), Global.get(cap, "generateXML"));
                po.addFxn("genXmlEntry", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$genXmlEntry", "ti.uia.runtime"), Global.get(cap, "genXmlEntry"));
                po.addFxn("genXmlEntryOpen", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$genXmlEntryOpen", "ti.uia.runtime"), Global.get(cap, "genXmlEntryOpen"));
                po.addFxn("genXmlEntryOpenWithKey", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$genXmlEntryOpenWithKey", "ti.uia.runtime"), Global.get(cap, "genXmlEntryOpenWithKey"));
                po.addFxn("genXmlEntryWithKey", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$genXmlEntryWithKey", "ti.uia.runtime"), Global.get(cap, "genXmlEntryWithKey"));
                po.addFxn("genXmlComment", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$genXmlComment", "ti.uia.runtime"), Global.get(cap, "genXmlComment"));
                po.addFxn("genXmlEntryClose", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$genXmlEntryClose", "ti.uia.runtime"), Global.get(cap, "genXmlEntryClose"));
                po.addFxn("setTransportFields", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$setTransportFields", "ti.uia.runtime"), Global.get(cap, "setTransportFields"));
                po.addFxn("setLoggingSetupConfigured", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$setLoggingSetupConfigured", "ti.uia.runtime"), Global.get(cap, "setLoggingSetupConfigured"));
                po.addFxn("isLoggingSetupConfigured", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$isLoggingSetupConfigured", "ti.uia.runtime"), Global.get(cap, "isLoggingSetupConfigured"));
                po.addFxn("setLogSyncInfo", (Proto.Fxn)om.findStrict("ti.uia.runtime.UIAMetaData$$setLogSyncInfo", "ti.uia.runtime"), Global.get(cap, "setLogSyncInfo"));
    }

    void UIAPacket$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/runtime/UIAPacket.xs");
        om.bind("ti.uia.runtime.UIAPacket$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.UIAPacket.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.UIAPacket.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("HOST", Proto.Elm.newCNum("(xdc_UInt16)"), 0xFFFFL, "rh");
                po.addFld("BROADCAST", Proto.Elm.newCNum("(xdc_UInt16)"), 0xFFFEL, "rh");
        if (isCFG) {
            po.addFld("maxPktLengthInBytes", Proto.Elm.newCNum("(xdc_Int)"), 128L, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.runtime.UIAPacket$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.UIAPacket$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.UIAPacket$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.runtime.UIAPacket$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct UIAPacket.Hdr
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.UIAPacket$$Hdr", "ti.uia.runtime");
        po.init("ti.uia.runtime.UIAPacket.Hdr", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("word1", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("word2", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("word3", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("word4", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
        // struct UIAPacket.Footer
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.UIAPacket$$Footer", "ti.uia.runtime");
        po.init("ti.uia.runtime.UIAPacket.Footer", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("word1", Proto.Elm.newCNum("(xdc_Int32)"), $$UNDEF, "w");
    }

    void ServiceMgr$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/runtime/ServiceMgr.xs");
        om.bind("ti.uia.runtime.ServiceMgr$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.ServiceMgr.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("WAIT_FOREVER", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.runtime"), $$UNDEF, "wh");
            po.addFld("A_invalidServiceId", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.uia.runtime"), Global.newObject("msg", "A_invalidServiceId: ServiceId out of range"), "w");
            po.addFld("A_invalidProcessCallbackFxn", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.uia.runtime"), Global.newObject("msg", "A_invalidProcessCallbackFxn: Callback cannot be NULL"), "w");
            po.addFld("customTransportType", $$T_Str, null, "wh");
            po.addFld("transportFxns", (Proto)om.findStrict("ti.uia.runtime.Transport.FxnSet", "ti.uia.runtime"), $$DEFAULT, "w");
            po.addFld("topology", (Proto)om.findStrict("ti.uia.runtime.ServiceMgr.Topology", "ti.uia.runtime"), om.find("ti.uia.runtime.ServiceMgr.Topology_SINGLECORE"), "w");
            po.addFld("transportType", (Proto)om.findStrict("ti.uia.runtime.ServiceMgr.TransportType", "ti.uia.runtime"), $$UNDEF, "wh");
            po.addFld("periodInMs", Proto.Elm.newCNum("(xdc_Int)"), 100L, "w");
            po.addFld("maxEventPacketSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
            po.addFld("numEventPacketBufs", Proto.Elm.newCNum("(xdc_Int)"), 2L, "w");
            po.addFld("maxCtrlPacketSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
            po.addFld("numOutgoingCtrlPacketBufs", Proto.Elm.newCNum("(xdc_Int)"), 2L, "w");
            po.addFld("numIncomingCtrlPacketBufs", Proto.Elm.newCNum("(xdc_Int)"), 2L, "w");
            po.addFld("supportControl", $$T_Bool, $$UNDEF, "w");
            po.addFld("transferAgentPriority", Proto.Elm.newCNum("(xdc_Int)"), 1L, "w");
            po.addFld("transferAgentStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), 2048L, "w");
            po.addFld("transferAgentStackSection", $$T_Str, null, "wh");
            po.addFld("rxTaskPriority", Proto.Elm.newCNum("(xdc_Int)"), 1L, "w");
            po.addFld("rxTaskStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), 2048L, "w");
            po.addFld("rxTaskStackSection", $$T_Str, null, "wh");
            po.addFldV("SupportProxy", (Proto)om.findStrict("ti.uia.runtime.IServiceMgrSupport.Module", "ti.uia.runtime"), null, "wh", $$delegGet, $$delegSet);
            po.addFld("masterProcId", Proto.Elm.newCNum("(xdc_UInt16)"), 0L, "w");
            po.addFld("processCallbackFxn", new Proto.Arr(new Proto.Adr("xdc_Void(*)(ti_uia_runtime_ServiceMgr_Reason,ti_uia_runtime_UIAPacket_Hdr*)", "PFv"), false), $$DEFAULT, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.runtime.ServiceMgr$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.runtime.ServiceMgr$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.runtime.ServiceMgr$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.runtime.ServiceMgr$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("register", (Proto.Fxn)om.findStrict("ti.uia.runtime.ServiceMgr$$register", "ti.uia.runtime"), Global.get(cap, "register"));
        // struct ServiceMgr.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr$$ModuleView", "ti.uia.runtime");
        po.init("ti.uia.runtime.ServiceMgr.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("periodInMs", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("supportControl", $$T_Bool, $$UNDEF, "w");
                po.addFld("topology", $$T_Str, $$UNDEF, "w");
                po.addFld("numServices", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("masterProcId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("runCount", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // struct ServiceMgr.PacketView
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr$$PacketView", "ti.uia.runtime");
        po.init("ti.uia.runtime.ServiceMgr.PacketView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("maxEventPacketSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("numEventPacketBufs", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("maxCtrlPacketSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("numOutgoingCtrlPacketBufs", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numIncomingCtrlPacketBufs", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // struct ServiceMgr.TransportView
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr$$TransportView", "ti.uia.runtime");
        po.init("ti.uia.runtime.ServiceMgr.TransportView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("initFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("startFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("recvFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("sendFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("stopFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("exitFxn", $$T_Str, $$UNDEF, "w");
        // struct ServiceMgr.StatisticsView
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr$$StatisticsView", "ti.uia.runtime");
        po.init("ti.uia.runtime.ServiceMgr.StatisticsView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("numEventPacketsSent", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numEventPacketsFailed", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numMsgPacketsSent", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numMsgPacketsFailed", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // typedef ServiceMgr.ProcessCallback
        om.bind("ti.uia.runtime.ServiceMgr.ProcessCallback", new Proto.Adr("xdc_Void(*)(ti_uia_runtime_ServiceMgr_Reason,ti_uia_runtime_UIAPacket_Hdr*)", "PFv"));
        // struct ServiceMgr.ServiceIdDesc
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr$$ServiceIdDesc", "ti.uia.runtime");
        po.init("ti.uia.runtime.ServiceMgr.ServiceIdDesc", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("val", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
        // typedef ServiceMgr.ServiceId
        pt = (Proto.Typedef)om.findStrict("ti.uia.runtime.ServiceMgr$$ServiceId", "ti.uia.runtime");
        pt.init("ti.uia.runtime.ServiceMgr.ServiceId", (Proto)om.findStrict("ti.uia.runtime.ServiceMgr.ServiceIdDesc", "ti.uia.runtime"), Global.get(cap, "ServiceId$encode"));
        om.bind("ti.uia.runtime.ServiceMgr.ServiceId", pt);
        // struct ServiceMgr.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr$$Module_State", "ti.uia.runtime");
        po.init("ti.uia.runtime.ServiceMgr.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("runCount", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numServices", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
    }

    void Transport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.Transport.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.Transport.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        // struct Transport.FxnSet
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.Transport$$FxnSet", "ti.uia.runtime");
        po.init("ti.uia.runtime.Transport.FxnSet", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("initFxn", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "w");
                po.addFld("startFxn", new Proto.Adr("xdc_Ptr(*)(ti_uia_runtime_UIAPacket_HdrType)", "PFPv"), $$UNDEF, "w");
                po.addFld("recvFxn", new Proto.Adr("xdc_SizeT(*)(xdc_Ptr,ti_uia_runtime_UIAPacket_Hdr**,xdc_SizeT)", "PFn"), $$UNDEF, "w");
                po.addFld("sendFxn", new Proto.Adr("xdc_Bool(*)(xdc_Ptr,ti_uia_runtime_UIAPacket_Hdr**)", "PFb"), $$UNDEF, "w");
                po.addFld("stopFxn", new Proto.Adr("xdc_Void(*)(xdc_Ptr)", "PFv"), $$UNDEF, "w");
                po.addFld("exitFxn", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "w");
    }

    void ICtxFilterCallback$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ICtxFilterCallback.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.ICtxFilterCallback.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ICtxFilterCallback.Instance", "ti.uia.runtime");
        po.init("ti.uia.runtime.ICtxFilterCallback.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ICtxFilterCallback$$Params", "ti.uia.runtime");
        po.init("ti.uia.runtime.ICtxFilterCallback.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void IUIATransfer$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATransfer.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.IUIATransfer.Module", om.findStrict("xdc.runtime.IFilterLogger.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATransfer.Instance", "ti.uia.runtime");
        po.init("ti.uia.runtime.IUIATransfer.Instance", om.findStrict("xdc.runtime.IFilterLogger.Instance", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("transferType", (Proto)om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.runtime"), om.find("ti.uia.runtime.IUIATransfer.TransferType_LOSSY"), "w");
            po.addFld("priority", (Proto)om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.runtime"), om.find("ti.uia.runtime.IUIATransfer.Priority_STANDARD"), "w");
            po.addFld("ptrToQueueDescriptorMeta", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATransfer$$Params", "ti.uia.runtime");
        po.init("ti.uia.runtime.IUIATransfer.Params", om.findStrict("xdc.runtime.IFilterLogger$$Params", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("transferType", (Proto)om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.runtime"), om.find("ti.uia.runtime.IUIATransfer.TransferType_LOSSY"), "w");
            po.addFld("priority", (Proto)om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.runtime"), om.find("ti.uia.runtime.IUIATransfer.Priority_STANDARD"), "w");
            po.addFld("ptrToQueueDescriptorMeta", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        }//isCFG
        // struct IUIATransfer.MetaData
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATransfer$$MetaData", "ti.uia.runtime");
        po.init("ti.uia.runtime.IUIATransfer.MetaData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
    }

    void ILoggerSnapshot$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ILoggerSnapshot.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.ILoggerSnapshot.Module", om.findStrict("ti.uia.runtime.IUIATransfer.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ILoggerSnapshot.Instance", "ti.uia.runtime");
        po.init("ti.uia.runtime.ILoggerSnapshot.Instance", om.findStrict("ti.uia.runtime.IUIATransfer.Instance", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ILoggerSnapshot$$Params", "ti.uia.runtime");
        po.init("ti.uia.runtime.ILoggerSnapshot.Params", om.findStrict("ti.uia.runtime.IUIATransfer$$Params", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void IUIATimestampProvider$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATimestampProvider.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.IUIATimestampProvider.Module", om.findStrict("xdc.runtime.ITimestampProvider.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("maxTimerClockFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.uia.runtime"), $$DEFAULT, "w");
            po.addFld("maxBusClockFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.uia.runtime"), $$DEFAULT, "w");
            po.addFld("canFrequencyBeChanged", $$T_Bool, false, "wh");
            po.addFld("cpuCyclesPerTick", Proto.Elm.newCNum("(xdc_UInt32)"), 0L, "wh");
            po.addFld("canCpuCyclesPerTickBeChanged", $$T_Bool, false, "wh");
        }//isCFG
    }

    void IUIATraceSyncProvider$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.IUIATraceSyncProvider.Module", om.findStrict("ti.uia.events.IUIAMetaProvider.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void IServiceMgrSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IServiceMgrSupport.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.IServiceMgrSupport.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void LogSync_CpuTimestampProxy$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync_CpuTimestampProxy.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSync_CpuTimestampProxy.Module", om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.uia.runtime"));
                po.addFld("delegate$", (Proto)om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.uia.runtime"), null, "wh");
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void LogSync_GlobalTimestampProxy$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync_GlobalTimestampProxy.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.LogSync_GlobalTimestampProxy.Module", om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.uia.runtime"));
                po.addFld("delegate$", (Proto)om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.uia.runtime"), null, "wh");
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void ServiceMgr_SupportProxy$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy.Module", "ti.uia.runtime");
        po.init("ti.uia.runtime.ServiceMgr_SupportProxy.Module", om.findStrict("ti.uia.runtime.IServiceMgrSupport.Module", "ti.uia.runtime"));
                po.addFld("delegate$", (Proto)om.findStrict("ti.uia.runtime.IServiceMgrSupport.Module", "ti.uia.runtime"), null, "wh");
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
                po.addFxn("newService", (Proto.Fxn)om.findStrict("ti.uia.runtime.IServiceMgrSupport$$newService", "ti.uia.runtime"), $$UNDEF);
    }

    void CtxFilter$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.CtxFilter", "ti.uia.runtime");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.runtime.CtxFilter.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.CtxFilter$$Module_State", "ti.uia.runtime");
    }

    void EventHdr$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.EventHdr", "ti.uia.runtime");
    }

    void LogCtxChg$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogCtxChg", "ti.uia.runtime");
    }

    void IUIATraceSyncClient$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.IUIATraceSyncClient", "ti.uia.runtime");
    }

    void LogSnapshot$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogSnapshot", "ti.uia.runtime");
        vo.bind("EventRec$fetchDesc", Global.newObject("type", "ti.uia.runtime.LogSnapshot.EventRec", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSnapshot$$EventRec", "ti.uia.runtime");
    }

    void LogSync$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogSync", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync$$Instance_State", "ti.uia.runtime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.runtime.LogSync.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync$$Module_State", "ti.uia.runtime");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.uia.runtime.LogSync.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync$$Instance_State", "ti.uia.runtime");
    }

    void LoggerSM$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LoggerSM", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$Instance_State", "ti.uia.runtime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.runtime.LoggerSM.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$Module_State", "ti.uia.runtime");
        po.bind("sharedObj$fetchDesc", Global.newObject("type", "ti.uia.runtime.LoggerSM.SharedObj", "isScalar", false));
        po.bind("sharedBuffer$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
        vo.bind("SharedObj$fetchDesc", Global.newObject("type", "ti.uia.runtime.LoggerSM.SharedObj", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$SharedObj", "ti.uia.runtime");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.uia.runtime.LoggerSM.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM$$Instance_State", "ti.uia.runtime");
    }

    void LoggerTypes$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LoggerTypes", "ti.uia.runtime");
    }

    void MultiCoreTypes$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.MultiCoreTypes", "ti.uia.runtime");
        vo.bind("ServiceHdr$fetchDesc", Global.newObject("type", "ti.uia.runtime.MultiCoreTypes.ServiceHdr", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.MultiCoreTypes$$ServiceHdr", "ti.uia.runtime");
        vo.bind("RegisterMsg$fetchDesc", Global.newObject("type", "ti.uia.runtime.MultiCoreTypes.RegisterMsg", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.MultiCoreTypes$$RegisterMsg", "ti.uia.runtime");
    }

    void QueueDescriptor$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.QueueDescriptor", "ti.uia.runtime");
        vo.bind("Header$fetchDesc", Global.newObject("type", "ti.uia.runtime.QueueDescriptor.Header", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.QueueDescriptor$$Header", "ti.uia.runtime");
        po.bind("next$fetchDesc", Global.newObject("type", "ti.uia.runtime.QueueDescriptor.Header", "isScalar", false));
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.runtime.QueueDescriptor.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.QueueDescriptor$$Module_State", "ti.uia.runtime");
    }

    void UIAMetaData$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.UIAMetaData", "ti.uia.runtime");
    }

    void UIAPacket$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.UIAPacket", "ti.uia.runtime");
        vo.bind("Hdr$fetchDesc", Global.newObject("type", "ti.uia.runtime.UIAPacket.Hdr", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.UIAPacket$$Hdr", "ti.uia.runtime");
        vo.bind("Footer$fetchDesc", Global.newObject("type", "ti.uia.runtime.UIAPacket.Footer", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.UIAPacket$$Footer", "ti.uia.runtime");
    }

    void ServiceMgr$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.ServiceMgr", "ti.uia.runtime");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.runtime.ServiceMgr.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr$$Module_State", "ti.uia.runtime");
    }

    void Transport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.Transport", "ti.uia.runtime");
        vo.bind("FxnSet$fetchDesc", Global.newObject("type", "ti.uia.runtime.Transport.FxnSet", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.Transport$$FxnSet", "ti.uia.runtime");
    }

    void ICtxFilterCallback$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.ICtxFilterCallback", "ti.uia.runtime");
    }

    void IUIATransfer$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.IUIATransfer", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATransfer$$Instance_State", "ti.uia.runtime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
    }

    void ILoggerSnapshot$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.ILoggerSnapshot", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ILoggerSnapshot$$Instance_State", "ti.uia.runtime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
    }

    void IUIATimestampProvider$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.IUIATimestampProvider", "ti.uia.runtime");
    }

    void IUIATraceSyncProvider$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider", "ti.uia.runtime");
    }

    void IServiceMgrSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.IServiceMgrSupport", "ti.uia.runtime");
    }

    void LogSync_CpuTimestampProxy$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogSync_CpuTimestampProxy", "ti.uia.runtime");
    }

    void LogSync_GlobalTimestampProxy$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogSync_GlobalTimestampProxy", "ti.uia.runtime");
    }

    void ServiceMgr_SupportProxy$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy", "ti.uia.runtime");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.uia.runtime.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.uia.runtime"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/runtime/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.uia.runtime"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.uia.runtime"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.uia.runtime"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.uia.runtime"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.uia.runtime"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.uia.runtime"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.uia.runtime", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.uia.runtime");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.uia.runtime.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 2));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.rov", Global.newArray()));
        imports.add(Global.newArray("xdc.rta", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.uia.runtime'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.uia.runtime$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.uia.runtime$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.uia.runtime$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/release/ti.uia.runtime.aem4',\n");
            sb.append("'lib/release/ti.uia.runtime.am4',\n");
            sb.append("'lib/release/ti.uia.runtime.am4g',\n");
            sb.append("'lib/release/ti.uia.runtime.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/release/ti.uia.runtime.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/release/ti.uia.runtime.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/release/ti.uia.runtime.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/release/ti.uia.runtime.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void CtxFilter$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.CtxFilter", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.CtxFilter.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.CtxFilter", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.runtime.CtxFilter$$capsule", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        vo.bind("ModuleView", om.findStrict("ti.uia.runtime.CtxFilter.ModuleView", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.CtxFilter.ModuleView", "ti.uia.runtime"));
        vo.bind("Mask", om.findStrict("ti.uia.runtime.CtxFilter.Mask", "ti.uia.runtime"));
        vo.bind("Module_State", om.findStrict("ti.uia.runtime.CtxFilter.Module_State", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.CtxFilter.Module_State", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("isCtxEnabled", om.findStrict("ti.uia.runtime.CtxFilter.isCtxEnabled", "ti.uia.runtime"));
        vo.bind("setCtxEnabled", om.findStrict("ti.uia.runtime.CtxFilter.setCtxEnabled", "ti.uia.runtime"));
        vo.bind("setContextFilterFlags", om.findStrict("ti.uia.runtime.CtxFilter.setContextFilterFlags", "ti.uia.runtime"));
        vo.bind("isLoggingEnabledForAppCtx", om.findStrict("ti.uia.runtime.CtxFilter.isLoggingEnabledForAppCtx", "ti.uia.runtime"));
        vo.bind("isLoggingEnabledForChanCtx", om.findStrict("ti.uia.runtime.CtxFilter.isLoggingEnabledForChanCtx", "ti.uia.runtime"));
        vo.bind("isLoggingEnabledForFrameCtx", om.findStrict("ti.uia.runtime.CtxFilter.isLoggingEnabledForFrameCtx", "ti.uia.runtime"));
        vo.bind("isLoggingEnabledForThreadCtx", om.findStrict("ti.uia.runtime.CtxFilter.isLoggingEnabledForThreadCtx", "ti.uia.runtime"));
        vo.bind("isLoggingEnabledForUserCtx", om.findStrict("ti.uia.runtime.CtxFilter.isLoggingEnabledForUserCtx", "ti.uia.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_CtxFilter_Module__startupDone__E", "ti_uia_runtime_CtxFilter_isCtxEnabled__E", "ti_uia_runtime_CtxFilter_setCtxEnabled__E", "ti_uia_runtime_CtxFilter_setContextFilterFlags__E", "ti_uia_runtime_CtxFilter_isLoggingEnabledForAppCtx__E", "ti_uia_runtime_CtxFilter_isLoggingEnabledForChanCtx__E", "ti_uia_runtime_CtxFilter_isLoggingEnabledForFrameCtx__E", "ti_uia_runtime_CtxFilter_isLoggingEnabledForThreadCtx__E", "ti_uia_runtime_CtxFilter_isLoggingEnabledForUserCtx__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("CtxFilter", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CtxFilter");
    }

    void EventHdr$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.EventHdr", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.EventHdr.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.EventHdr", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        vo.bind("HdrType", om.findStrict("ti.uia.runtime.EventHdr.HdrType", "ti.uia.runtime"));
        vo.bind("HdrType_Event", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Event", "ti.uia.runtime"));
        vo.bind("HdrType_EventWithTimestamp", om.findStrict("ti.uia.runtime.EventHdr.HdrType_EventWithTimestamp", "ti.uia.runtime"));
        vo.bind("HdrType_EventWithSnapshotId", om.findStrict("ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotId", "ti.uia.runtime"));
        vo.bind("HdrType_EventWithSnapshotIdAndTimestamp", om.findStrict("ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotIdAndTimestamp", "ti.uia.runtime"));
        vo.bind("HdrType_EventWith32bTimestamp", om.findStrict("ti.uia.runtime.EventHdr.HdrType_EventWith32bTimestamp", "ti.uia.runtime"));
        vo.bind("HdrType_EventWithTimestampAndEndpointId", om.findStrict("ti.uia.runtime.EventHdr.HdrType_EventWithTimestampAndEndpointId", "ti.uia.runtime"));
        vo.bind("HdrType_EventWithSnapshotIdAnd32bTimestamp", om.findStrict("ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotIdAnd32bTimestamp", "ti.uia.runtime"));
        vo.bind("HdrType_EventWithSnapshotIdAndTimestampAndEndpointId", om.findStrict("ti.uia.runtime.EventHdr.HdrType_EventWithSnapshotIdAndTimestampAndEndpointId", "ti.uia.runtime"));
        vo.bind("HdrType_EventWithPrevLenWithNoTimestamp", om.findStrict("ti.uia.runtime.EventHdr.HdrType_EventWithPrevLenWithNoTimestamp", "ti.uia.runtime"));
        vo.bind("HdrType_EventWithPrevLenWith32bTimestamp", om.findStrict("ti.uia.runtime.EventHdr.HdrType_EventWithPrevLenWith32bTimestamp", "ti.uia.runtime"));
        vo.bind("HdrType_EventWithPrevLenWith64bTimestamp", om.findStrict("ti.uia.runtime.EventHdr.HdrType_EventWithPrevLenWith64bTimestamp", "ti.uia.runtime"));
        vo.bind("HdrType_SnapshotEventWithPrevLen", om.findStrict("ti.uia.runtime.EventHdr.HdrType_SnapshotEventWithPrevLen", "ti.uia.runtime"));
        vo.bind("HdrType_SnapshotEventWithPrevLenAnd64bTimestamp", om.findStrict("ti.uia.runtime.EventHdr.HdrType_SnapshotEventWithPrevLenAnd64bTimestamp", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved13", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved13", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved14", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved14", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved15", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved15", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved16", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved16", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved17", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved17", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved18", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved18", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved19", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved19", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved20", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved20", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved21", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved21", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved22", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved22", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved23", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved23", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved24", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved24", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved25", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved25", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved26", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved26", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved27", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved27", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved28", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved28", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved29", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved29", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved30", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved30", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved31", om.findStrict("ti.uia.runtime.EventHdr.HdrType_Reserved31", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_EventHdr_Module__startupDone__E", "ti_uia_runtime_EventHdr_getHdrType__E", "ti_uia_runtime_EventHdr_setHdrType__E", "ti_uia_runtime_EventHdr_getLength__E", "ti_uia_runtime_EventHdr_setLength__E", "ti_uia_runtime_EventHdr_getSeqCount__E", "ti_uia_runtime_EventHdr_setSeqCount__E", "ti_uia_runtime_EventHdr_genEventHdrWord1__E", "ti_uia_runtime_EventHdr_genEventWithPrevLenHdrWord1__E", "ti_uia_runtime_EventHdr_getEventHdrWithPrevLenSeqCount__E", "ti_uia_runtime_EventHdr_getPrevLength__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("EventHdr", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("EventHdr");
    }

    void LogCtxChg$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogCtxChg", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogCtxChg.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.LogCtxChg", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.runtime.LogCtxChg$$capsule", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        mcfgs.add("isTimestampEnabled");
        mcfgs.add("loggerDefined");
        mcfgs.add("ctxFilterEnabled");
        mcfgs.add("loggerObj");
        mcfgs.add("loggerFxn2");
        mcfgs.add("loggerFxn8");
        icfgs.add("idToInfo");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_LogCtxChg_Module__startupDone__E", "ti_uia_runtime_LogCtxChg_putCtxChg1__E", "ti_uia_runtime_LogCtxChg_putCtxChg2__E", "ti_uia_runtime_LogCtxChg_putCtxChg8__E", "ti_uia_runtime_LogCtxChg_app__E", "ti_uia_runtime_LogCtxChg_channel__E", "ti_uia_runtime_LogCtxChg_thread__E", "ti_uia_runtime_LogCtxChg_threadAndFunc__E", "ti_uia_runtime_LogCtxChg_frame__E", "ti_uia_runtime_LogCtxChg_hwiStart__E", "ti_uia_runtime_LogCtxChg_hwiStop__E", "ti_uia_runtime_LogCtxChg_swiStart__E", "ti_uia_runtime_LogCtxChg_swiStop__E", "ti_uia_runtime_LogCtxChg_user__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LogCtxChg", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LogCtxChg");
    }

    void IUIATraceSyncClient$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.IUIATraceSyncClient", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATraceSyncClient.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.IUIATraceSyncClient", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IUIATraceSyncClient", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IUIATraceSyncClient");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void LogSnapshot$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogSnapshot", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSnapshot.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.LogSnapshot", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.runtime.LogSnapshot$$capsule", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        mcfgs.add("injectIntoTraceFxn");
        vo.bind("EventRec", om.findStrict("ti.uia.runtime.LogSnapshot.EventRec", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.LogSnapshot.EventRec", "ti.uia.runtime"));
        mcfgs.add("maxLengthInMAUs");
        mcfgs.add("isTimestampEnabled");
        mcfgs.add("loggerDefined");
        mcfgs.add("loggerObj");
        mcfgs.add("loggerMemoryRangeFxn");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.runtime");
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("getSnapshotId", om.findStrict("ti.uia.runtime.LogSnapshot.getSnapshotId", "ti.uia.runtime"));
        vo.bind("doPrint", om.findStrict("ti.uia.runtime.LogSnapshot.doPrint", "ti.uia.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_LogSnapshot_Module__startupDone__E", "ti_uia_runtime_LogSnapshot_putMemoryRange__E", "ti_uia_runtime_LogSnapshot_writeMemoryBlockWithIdTag__E", "ti_uia_runtime_LogSnapshot_writeMemoryBlock__E", "ti_uia_runtime_LogSnapshot_writeStringWithIdTag__E", "ti_uia_runtime_LogSnapshot_writeString__E", "ti_uia_runtime_LogSnapshot_writeNameOfReference__E", "ti_uia_runtime_LogSnapshot_getSnapshotId__E", "ti_uia_runtime_LogSnapshot_doPrint__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "doPrint", "entry", "%p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LogSnapshot", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LogSnapshot");
    }

    void LogSync$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogSync", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.LogSync", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.runtime.LogSync$$capsule", "ti.uia.runtime"));
        vo.bind("Instance", om.findStrict("ti.uia.runtime.LogSync.Instance", "ti.uia.runtime"));
        vo.bind("Params", om.findStrict("ti.uia.runtime.LogSync.Params", "ti.uia.runtime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.runtime.LogSync.Params", "ti.uia.runtime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.runtime.LogSync.Handle", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        mcfgs.add("injectIntoTraceFxn");
        vo.bind("ModuleView", om.findStrict("ti.uia.runtime.LogSync.ModuleView", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.LogSync.ModuleView", "ti.uia.runtime"));
        vo.bind("LoggerType", om.findStrict("ti.uia.runtime.LogSync.LoggerType", "ti.uia.runtime"));
        vo.bind("CpuTimestampProxy$proxy", om.findStrict("ti.uia.runtime.LogSync_CpuTimestampProxy", "ti.uia.runtime"));
        proxies.add("CpuTimestampProxy");
        mcfgs.add("cpuTimestampCyclesPerTick");
        vo.bind("GlobalTimestampProxy$proxy", om.findStrict("ti.uia.runtime.LogSync_GlobalTimestampProxy", "ti.uia.runtime"));
        proxies.add("GlobalTimestampProxy");
        mcfgs.add("globalTimestampCpuCyclesPerTick");
        mcfgs.add("maxGlobalClockFreq");
        vo.bind("Module_State", om.findStrict("ti.uia.runtime.LogSync.Module_State", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.LogSync.Module_State", "ti.uia.runtime"));
        vo.bind("Instance_State", om.findStrict("ti.uia.runtime.LogSync.Instance_State", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.LogSync.Instance_State", "ti.uia.runtime"));
        vo.bind("LoggerType_NONE", om.findStrict("ti.uia.runtime.LogSync.LoggerType_NONE", "ti.uia.runtime"));
        vo.bind("LoggerType_MIN", om.findStrict("ti.uia.runtime.LogSync.LoggerType_MIN", "ti.uia.runtime"));
        vo.bind("LoggerType_STOPMODE", om.findStrict("ti.uia.runtime.LogSync.LoggerType_STOPMODE", "ti.uia.runtime"));
        vo.bind("LoggerType_RUNMODE", om.findStrict("ti.uia.runtime.LogSync.LoggerType_RUNMODE", "ti.uia.runtime"));
        vo.bind("LoggerType_IDLE", om.findStrict("ti.uia.runtime.LogSync.LoggerType_IDLE", "ti.uia.runtime"));
        vo.bind("LoggerType_STREAMER", om.findStrict("ti.uia.runtime.LogSync.LoggerType_STREAMER", "ti.uia.runtime"));
        vo.bind("LoggerType_STREAMER2", om.findStrict("ti.uia.runtime.LogSync.LoggerType_STREAMER2", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.runtime");
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
        vo.bind("$$instflag", 1);
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
            vo.bind("__initObject", Global.get("ti$uia$runtime$LogSync$$__initObject"));
        }//isCFG
        vo.bind("enable", om.findStrict("ti.uia.runtime.LogSync.enable", "ti.uia.runtime"));
        vo.bind("disable", om.findStrict("ti.uia.runtime.LogSync.disable", "ti.uia.runtime"));
        vo.bind("idleHook", om.findStrict("ti.uia.runtime.LogSync.idleHook", "ti.uia.runtime"));
        vo.bind("timerHook", om.findStrict("ti.uia.runtime.LogSync.timerHook", "ti.uia.runtime"));
        vo.bind("putSyncPoint", om.findStrict("ti.uia.runtime.LogSync.putSyncPoint", "ti.uia.runtime"));
        vo.bind("writeSyncPointRaw", om.findStrict("ti.uia.runtime.LogSync.writeSyncPointRaw", "ti.uia.runtime"));
        vo.bind("isSyncEventRequired", om.findStrict("ti.uia.runtime.LogSync.isSyncEventRequired", "ti.uia.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_LogSync_Handle__label__E", "ti_uia_runtime_LogSync_Module__startupDone__E", "ti_uia_runtime_LogSync_Object__create__E", "ti_uia_runtime_LogSync_Object__delete__E", "ti_uia_runtime_LogSync_Object__get__E", "ti_uia_runtime_LogSync_Object__first__E", "ti_uia_runtime_LogSync_Object__next__E", "ti_uia_runtime_LogSync_Params__init__E", "ti_uia_runtime_LogSync_enable__E", "ti_uia_runtime_LogSync_disable__E", "ti_uia_runtime_LogSync_idleHook__E", "ti_uia_runtime_LogSync_timerHook__E", "ti_uia_runtime_LogSync_putSyncPoint__E", "ti_uia_runtime_LogSync_writeSyncPoint__E", "ti_uia_runtime_LogSync_writeSyncPointRaw__E", "ti_uia_runtime_LogSync_isSyncEventRequired__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.uia.runtime.LogSync.Object", "ti.uia.runtime"));
        vo.bind("Instance_State", om.findStrict("ti.uia.runtime.LogSync.Instance_State", "ti.uia.runtime"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "writeSyncPointRaw", "entry", "%p, %p, %p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LogSync", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LogSync");
    }

    void LoggerSM$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LoggerSM", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerSM.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.LoggerSM", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.runtime.LoggerSM$$capsule", "ti.uia.runtime"));
        vo.bind("Instance", om.findStrict("ti.uia.runtime.LoggerSM.Instance", "ti.uia.runtime"));
        vo.bind("Params", om.findStrict("ti.uia.runtime.LoggerSM.Params", "ti.uia.runtime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.runtime.LoggerSM.Params", "ti.uia.runtime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.runtime.LoggerSM.Handle", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        mcfgs.add("filterByLevel");
        vo.bind("ModuleView", om.findStrict("ti.uia.runtime.LoggerSM.ModuleView", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.LoggerSM.ModuleView", "ti.uia.runtime"));
        vo.bind("InstanceView", om.findStrict("ti.uia.runtime.LoggerSM.InstanceView", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.LoggerSM.InstanceView", "ti.uia.runtime"));
        mcfgs.add("E_badLevel");
        mcfgs.add("isTimestampEnabled");
        mcfgs.add("decode");
        mcfgs.add("overwrite");
        mcfgs.add("level1Mask");
        mcfgs.add("level2Mask");
        mcfgs.add("level3Mask");
        mcfgs.add("level4Mask");
        mcfgs.add("numPartitions");
        mcfgs.add("sharedMemorySize");
        vo.bind("MetaData", om.findStrict("ti.uia.runtime.LoggerSM.MetaData", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.LoggerSM.MetaData", "ti.uia.runtime"));
        vo.bind("Module_State", om.findStrict("ti.uia.runtime.LoggerSM.Module_State", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.LoggerSM.Module_State", "ti.uia.runtime"));
        vo.bind("SharedObj", om.findStrict("ti.uia.runtime.LoggerSM.SharedObj", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.LoggerSM.SharedObj", "ti.uia.runtime"));
        vo.bind("Instance_State", om.findStrict("ti.uia.runtime.LoggerSM.Instance_State", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.LoggerSM.Instance_State", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
        vo.bind("$$instflag", 1);
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
            vo.bind("__initObject", Global.get("ti$uia$runtime$LoggerSM$$__initObject"));
        }//isCFG
        vo.bind("setPartitionId", om.findStrict("ti.uia.runtime.LoggerSM.setPartitionId", "ti.uia.runtime"));
        vo.bind("setSharedMemory", om.findStrict("ti.uia.runtime.LoggerSM.setSharedMemory", "ti.uia.runtime"));
        vo.bind("filterOutEvent", om.findStrict("ti.uia.runtime.LoggerSM.filterOutEvent", "ti.uia.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_LoggerSM_Handle__label__E", "ti_uia_runtime_LoggerSM_Module__startupDone__E", "ti_uia_runtime_LoggerSM_Object__create__E", "ti_uia_runtime_LoggerSM_Object__delete__E", "ti_uia_runtime_LoggerSM_Object__get__E", "ti_uia_runtime_LoggerSM_Object__first__E", "ti_uia_runtime_LoggerSM_Object__next__E", "ti_uia_runtime_LoggerSM_Params__init__E", "ti_uia_runtime_LoggerSM_setPartitionId__E", "ti_uia_runtime_LoggerSM_setSharedMemory__E", "ti_uia_runtime_LoggerSM_enable__E", "ti_uia_runtime_LoggerSM_disable__E", "ti_uia_runtime_LoggerSM_write0__E", "ti_uia_runtime_LoggerSM_write1__E", "ti_uia_runtime_LoggerSM_write2__E", "ti_uia_runtime_LoggerSM_write4__E", "ti_uia_runtime_LoggerSM_write8__E", "ti_uia_runtime_LoggerSM_setFilterLevel__E", "ti_uia_runtime_LoggerSM_getFilterLevel__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray("E_badLevel"));
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", "./LoggerSM.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.uia.runtime.LoggerSM.Object", "ti.uia.runtime"));
        vo.bind("Instance_State", om.findStrict("ti.uia.runtime.LoggerSM.Instance_State", "ti.uia.runtime"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./LoggerSM.xdt");
        pkgV.bind("LoggerSM", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerSM");
    }

    void LoggerTypes$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LoggerTypes", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LoggerTypes.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.LoggerTypes", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        vo.bind("LogMemoryRangeFxn", om.findStrict("ti.uia.runtime.LoggerTypes.LogMemoryRangeFxn", "ti.uia.runtime"));
        vo.bind("InjectIntoTraceFxn", om.findStrict("ti.uia.runtime.LoggerTypes.InjectIntoTraceFxn", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_LoggerTypes_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LoggerTypes", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerTypes");
    }

    void MultiCoreTypes$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.MultiCoreTypes", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.MultiCoreTypes.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.MultiCoreTypes", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        vo.bind("ServiceHdr", om.findStrict("ti.uia.runtime.MultiCoreTypes.ServiceHdr", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.MultiCoreTypes.ServiceHdr", "ti.uia.runtime"));
        mcfgs.add("A_ipcFailed");
        mcfgs.add("A_invalidHdrType");
        vo.bind("Action", om.findStrict("ti.uia.runtime.MultiCoreTypes.Action", "ti.uia.runtime"));
        vo.bind("RegisterMsg", om.findStrict("ti.uia.runtime.MultiCoreTypes.RegisterMsg", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.MultiCoreTypes.RegisterMsg", "ti.uia.runtime"));
        vo.bind("Action_TOHOST", om.findStrict("ti.uia.runtime.MultiCoreTypes.Action_TOHOST", "ti.uia.runtime"));
        vo.bind("Action_FROMHOST", om.findStrict("ti.uia.runtime.MultiCoreTypes.Action_FROMHOST", "ti.uia.runtime"));
        vo.bind("Action_REGISTER", om.findStrict("ti.uia.runtime.MultiCoreTypes.Action_REGISTER", "ti.uia.runtime"));
        vo.bind("Action_STOP", om.findStrict("ti.uia.runtime.MultiCoreTypes.Action_STOP", "ti.uia.runtime"));
        vo.bind("Action_STOPACK", om.findStrict("ti.uia.runtime.MultiCoreTypes.Action_STOPACK", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_MultiCoreTypes_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_ipcFailed", "A_invalidHdrType"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("MultiCoreTypes", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("MultiCoreTypes");
    }

    void QueueDescriptor$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.QueueDescriptor", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.QueueDescriptor.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.QueueDescriptor", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.runtime.QueueDescriptor$$capsule", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        vo.bind("ModuleView", om.findStrict("ti.uia.runtime.QueueDescriptor.ModuleView", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.QueueDescriptor.ModuleView", "ti.uia.runtime"));
        vo.bind("Header", om.findStrict("ti.uia.runtime.QueueDescriptor.Header", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.QueueDescriptor.Header", "ti.uia.runtime"));
        icfgs.add("maxId");
        vo.bind("Module_State", om.findStrict("ti.uia.runtime.QueueDescriptor.Module_State", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.QueueDescriptor.Module_State", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("addToList", om.findStrict("ti.uia.runtime.QueueDescriptor.addToList", "ti.uia.runtime"));
        vo.bind("initHeader", om.findStrict("ti.uia.runtime.QueueDescriptor.initHeader", "ti.uia.runtime"));
        vo.bind("removeFromList", om.findStrict("ti.uia.runtime.QueueDescriptor.removeFromList", "ti.uia.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_QueueDescriptor_Module__startupDone__E", "ti_uia_runtime_QueueDescriptor_addToList__E", "ti_uia_runtime_QueueDescriptor_initHeader__E", "ti_uia_runtime_QueueDescriptor_removeFromList__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("QueueDescriptor", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("QueueDescriptor");
    }

    void UIAMetaData$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.UIAMetaData", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.UIAMetaData.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.UIAMetaData", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.runtime.UIAMetaData$$capsule", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        vo.bind("UploadMode", om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode", "ti.uia.runtime"));
        vo.bind("UploadMode_SIMULATOR", om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode_SIMULATOR", "ti.uia.runtime"));
        vo.bind("UploadMode_PROBEPOINT", om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode_PROBEPOINT", "ti.uia.runtime"));
        vo.bind("UploadMode_JTAGSTOPMODE", om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode_JTAGSTOPMODE", "ti.uia.runtime"));
        vo.bind("UploadMode_JTAGRUNMODE", om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode_JTAGRUNMODE", "ti.uia.runtime"));
        vo.bind("UploadMode_NONJTAGTRANSPORT", om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode_NONJTAGTRANSPORT", "ti.uia.runtime"));
        vo.bind("UploadMode_CUSTOM", om.findStrict("ti.uia.runtime.UIAMetaData.UploadMode_CUSTOM", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_UIAMetaData_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAMetaData", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAMetaData");
    }

    void UIAPacket$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.UIAPacket", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.UIAPacket.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.UIAPacket", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.runtime.UIAPacket$$capsule", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        vo.bind("Hdr", om.findStrict("ti.uia.runtime.UIAPacket.Hdr", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.UIAPacket.Hdr", "ti.uia.runtime"));
        vo.bind("HdrType", om.findStrict("ti.uia.runtime.UIAPacket.HdrType", "ti.uia.runtime"));
        vo.bind("PayloadEndian", om.findStrict("ti.uia.runtime.UIAPacket.PayloadEndian", "ti.uia.runtime"));
        vo.bind("Footer", om.findStrict("ti.uia.runtime.UIAPacket.Footer", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.UIAPacket.Footer", "ti.uia.runtime"));
        vo.bind("MsgType", om.findStrict("ti.uia.runtime.UIAPacket.MsgType", "ti.uia.runtime"));
        vo.bind("NACKErrorCode", om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode", "ti.uia.runtime"));
        mcfgs.add("maxPktLengthInBytes");
        vo.bind("HdrType_InvalidData", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_InvalidData", "ti.uia.runtime"));
        vo.bind("HdrType_MsgWithPID", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_MsgWithPID", "ti.uia.runtime"));
        vo.bind("HdrType_EventPktWithCRC", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_EventPktWithCRC", "ti.uia.runtime"));
        vo.bind("HdrType_MinEventPkt", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_MinEventPkt", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved4", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_Reserved4", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved5", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_Reserved5", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved6", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_Reserved6", "ti.uia.runtime"));
        vo.bind("HdrType_Reserved7", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_Reserved7", "ti.uia.runtime"));
        vo.bind("HdrType_ChannelizedData", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_ChannelizedData", "ti.uia.runtime"));
        vo.bind("HdrType_Msg", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_Msg", "ti.uia.runtime"));
        vo.bind("HdrType_EventPkt", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_EventPkt", "ti.uia.runtime"));
        vo.bind("HdrType_CPUTrace", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_CPUTrace", "ti.uia.runtime"));
        vo.bind("HdrType_STMTrace", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_STMTrace", "ti.uia.runtime"));
        vo.bind("HdrType_MemoryBuffer", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_MemoryBuffer", "ti.uia.runtime"));
        vo.bind("HdrType_USER2", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_USER2", "ti.uia.runtime"));
        vo.bind("HdrType_USER3", om.findStrict("ti.uia.runtime.UIAPacket.HdrType_USER3", "ti.uia.runtime"));
        vo.bind("PayloadEndian_LITTLE", om.findStrict("ti.uia.runtime.UIAPacket.PayloadEndian_LITTLE", "ti.uia.runtime"));
        vo.bind("PayloadEndian_BIG", om.findStrict("ti.uia.runtime.UIAPacket.PayloadEndian_BIG", "ti.uia.runtime"));
        vo.bind("MsgType_ACK", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_ACK", "ti.uia.runtime"));
        vo.bind("MsgType_CMD", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_CMD", "ti.uia.runtime"));
        vo.bind("MsgType_RESULT", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_RESULT", "ti.uia.runtime"));
        vo.bind("MsgType_PARTIALRESULT", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_PARTIALRESULT", "ti.uia.runtime"));
        vo.bind("MsgType_NOTIFY", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_NOTIFY", "ti.uia.runtime"));
        vo.bind("MsgType_FLOWCTRL", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_FLOWCTRL", "ti.uia.runtime"));
        vo.bind("MsgType_DATA", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_DATA", "ti.uia.runtime"));
        vo.bind("MsgType_RESERVED7", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_RESERVED7", "ti.uia.runtime"));
        vo.bind("MsgType_RESERVED8", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_RESERVED8", "ti.uia.runtime"));
        vo.bind("MsgType_RESERVED9", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_RESERVED9", "ti.uia.runtime"));
        vo.bind("MsgType_RESERVED10", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_RESERVED10", "ti.uia.runtime"));
        vo.bind("MsgType_RESERVED11", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_RESERVED11", "ti.uia.runtime"));
        vo.bind("MsgType_RESERVED12", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_RESERVED12", "ti.uia.runtime"));
        vo.bind("MsgType_RESERVED13", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_RESERVED13", "ti.uia.runtime"));
        vo.bind("MsgType_NACK_BAD_DATA", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_NACK_BAD_DATA", "ti.uia.runtime"));
        vo.bind("MsgType_NACK_WITH_ERROR_CODE", om.findStrict("ti.uia.runtime.UIAPacket.MsgType_NACK_WITH_ERROR_CODE", "ti.uia.runtime"));
        vo.bind("NACKErrorCode_NO_REASON_SPECIFIED", om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode_NO_REASON_SPECIFIED", "ti.uia.runtime"));
        vo.bind("NACKErrorCode_SERVICE_NOT_SUPPORTED", om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode_SERVICE_NOT_SUPPORTED", "ti.uia.runtime"));
        vo.bind("NACKErrorCode_CMD_NOT_SUPPORTED", om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode_CMD_NOT_SUPPORTED", "ti.uia.runtime"));
        vo.bind("NACKErrorCode_QUEUE_FULL", om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode_QUEUE_FULL", "ti.uia.runtime"));
        vo.bind("NACKErrorCode_BAD_ENDPOINT_ADDRESS", om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode_BAD_ENDPOINT_ADDRESS", "ti.uia.runtime"));
        vo.bind("NACKErrorCode_BAD_MESSAGE_LENGTH", om.findStrict("ti.uia.runtime.UIAPacket.NACKErrorCode_BAD_MESSAGE_LENGTH", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("getLength", om.findStrict("ti.uia.runtime.UIAPacket.getLength", "ti.uia.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_UIAPacket_Module__startupDone__E", "ti_uia_runtime_UIAPacket_swizzle__E", "ti_uia_runtime_UIAPacket_swizzle16__E", "ti_uia_runtime_UIAPacket_getHdrType__E", "ti_uia_runtime_UIAPacket_setHdrType__E", "ti_uia_runtime_UIAPacket_getPayloadEndianness__E", "ti_uia_runtime_UIAPacket_setPayloadEndianness__E", "ti_uia_runtime_UIAPacket_getMsgLength__E", "ti_uia_runtime_UIAPacket_setMsgLength__E", "ti_uia_runtime_UIAPacket_getEventLength__E", "ti_uia_runtime_UIAPacket_getMinEventLength__E", "ti_uia_runtime_UIAPacket_setEventLength__E", "ti_uia_runtime_UIAPacket_setEventLengthFast__E", "ti_uia_runtime_UIAPacket_setMinEventPacketLength__E", "ti_uia_runtime_UIAPacket_getLength__E", "ti_uia_runtime_UIAPacket_getSequenceCount__E", "ti_uia_runtime_UIAPacket_setSequenceCount__E", "ti_uia_runtime_UIAPacket_setSequenceCountFast__E", "ti_uia_runtime_UIAPacket_setSequenceCounts__E", "ti_uia_runtime_UIAPacket_setMinEventPacketSequenceCount__E", "ti_uia_runtime_UIAPacket_getLoggerPriority__E", "ti_uia_runtime_UIAPacket_setLoggerPriority__E", "ti_uia_runtime_UIAPacket_getLoggerModuleId__E", "ti_uia_runtime_UIAPacket_setLoggerModuleId__E", "ti_uia_runtime_UIAPacket_getLoggerInstanceId__E", "ti_uia_runtime_UIAPacket_setLoggerInstanceId__E", "ti_uia_runtime_UIAPacket_getMsgType__E", "ti_uia_runtime_UIAPacket_setMsgType__E", "ti_uia_runtime_UIAPacket_getCmdId__E", "ti_uia_runtime_UIAPacket_setCmdId__E", "ti_uia_runtime_UIAPacket_getServiceId__E", "ti_uia_runtime_UIAPacket_setServiceId__E", "ti_uia_runtime_UIAPacket_getTag__E", "ti_uia_runtime_UIAPacket_setTag__E", "ti_uia_runtime_UIAPacket_getDestAdrs__E", "ti_uia_runtime_UIAPacket_setDestAdrs__E", "ti_uia_runtime_UIAPacket_getSenderAdrs__E", "ti_uia_runtime_UIAPacket_setSenderAdrs__E", "ti_uia_runtime_UIAPacket_initMsgHdr__E", "ti_uia_runtime_UIAPacket_initEventRecHdr__E", "ti_uia_runtime_UIAPacket_initMinEventRecHdr__E", "ti_uia_runtime_UIAPacket_getFooter__E", "ti_uia_runtime_UIAPacket_setInvalidHdr__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "getLength", "entry", "%p", "exit", "%d"));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("UIAPacket", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UIAPacket");
    }

    void ServiceMgr$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.ServiceMgr", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.ServiceMgr", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.runtime.ServiceMgr$$capsule", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        vo.bind("ModuleView", om.findStrict("ti.uia.runtime.ServiceMgr.ModuleView", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.ServiceMgr.ModuleView", "ti.uia.runtime"));
        vo.bind("PacketView", om.findStrict("ti.uia.runtime.ServiceMgr.PacketView", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.ServiceMgr.PacketView", "ti.uia.runtime"));
        vo.bind("TransportView", om.findStrict("ti.uia.runtime.ServiceMgr.TransportView", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.ServiceMgr.TransportView", "ti.uia.runtime"));
        vo.bind("StatisticsView", om.findStrict("ti.uia.runtime.ServiceMgr.StatisticsView", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.ServiceMgr.StatisticsView", "ti.uia.runtime"));
        vo.bind("Reason", om.findStrict("ti.uia.runtime.ServiceMgr.Reason", "ti.uia.runtime"));
        vo.bind("Topology", om.findStrict("ti.uia.runtime.ServiceMgr.Topology", "ti.uia.runtime"));
        vo.bind("TransportType", om.findStrict("ti.uia.runtime.ServiceMgr.TransportType", "ti.uia.runtime"));
        vo.bind("ProcessCallback", om.findStrict("ti.uia.runtime.ServiceMgr.ProcessCallback", "ti.uia.runtime"));
        vo.bind("ServiceIdDesc", om.findStrict("ti.uia.runtime.ServiceMgr.ServiceIdDesc", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.ServiceMgr.ServiceIdDesc", "ti.uia.runtime"));
        vo.bind("ServiceId", om.findStrict("ti.uia.runtime.ServiceMgr.ServiceId", "ti.uia.runtime"));
        mcfgs.add("A_invalidServiceId");
        mcfgs.add("A_invalidProcessCallbackFxn");
        mcfgs.add("transportFxns");
        mcfgs.add("topology");
        mcfgs.add("periodInMs");
        mcfgs.add("maxEventPacketSize");
        mcfgs.add("numEventPacketBufs");
        mcfgs.add("maxCtrlPacketSize");
        mcfgs.add("numOutgoingCtrlPacketBufs");
        mcfgs.add("numIncomingCtrlPacketBufs");
        mcfgs.add("supportControl");
        mcfgs.add("transferAgentPriority");
        mcfgs.add("transferAgentStackSize");
        mcfgs.add("rxTaskPriority");
        mcfgs.add("rxTaskStackSize");
        vo.bind("SupportProxy$proxy", om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy", "ti.uia.runtime"));
        proxies.add("SupportProxy");
        mcfgs.add("masterProcId");
        mcfgs.add("processCallbackFxn");
        icfgs.add("processCallbackFxn");
        vo.bind("Module_State", om.findStrict("ti.uia.runtime.ServiceMgr.Module_State", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.ServiceMgr.Module_State", "ti.uia.runtime"));
        vo.bind("Reason_PERIODEXPIRED", om.findStrict("ti.uia.runtime.ServiceMgr.Reason_PERIODEXPIRED", "ti.uia.runtime"));
        vo.bind("Reason_REQUESTENERGY", om.findStrict("ti.uia.runtime.ServiceMgr.Reason_REQUESTENERGY", "ti.uia.runtime"));
        vo.bind("Reason_INCOMINGMSG", om.findStrict("ti.uia.runtime.ServiceMgr.Reason_INCOMINGMSG", "ti.uia.runtime"));
        vo.bind("Topology_SINGLECORE", om.findStrict("ti.uia.runtime.ServiceMgr.Topology_SINGLECORE", "ti.uia.runtime"));
        vo.bind("Topology_MULTICORE", om.findStrict("ti.uia.runtime.ServiceMgr.Topology_MULTICORE", "ti.uia.runtime"));
        vo.bind("TransportType_ETHERNET", om.findStrict("ti.uia.runtime.ServiceMgr.TransportType_ETHERNET", "ti.uia.runtime"));
        vo.bind("TransportType_FILE", om.findStrict("ti.uia.runtime.ServiceMgr.TransportType_FILE", "ti.uia.runtime"));
        vo.bind("TransportType_USER", om.findStrict("ti.uia.runtime.ServiceMgr.TransportType_USER", "ti.uia.runtime"));
        vo.bind("TransportType_NULL", om.findStrict("ti.uia.runtime.ServiceMgr.TransportType_NULL", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("freePacket", om.findStrict("ti.uia.runtime.ServiceMgr.freePacket", "ti.uia.runtime"));
        vo.bind("getFreePacket", om.findStrict("ti.uia.runtime.ServiceMgr.getFreePacket", "ti.uia.runtime"));
        vo.bind("getNumServices", om.findStrict("ti.uia.runtime.ServiceMgr.getNumServices", "ti.uia.runtime"));
        vo.bind("processCallback", om.findStrict("ti.uia.runtime.ServiceMgr.processCallback", "ti.uia.runtime"));
        vo.bind("requestEnergy", om.findStrict("ti.uia.runtime.ServiceMgr.requestEnergy", "ti.uia.runtime"));
        vo.bind("sendPacket", om.findStrict("ti.uia.runtime.ServiceMgr.sendPacket", "ti.uia.runtime"));
        vo.bind("setPeriod", om.findStrict("ti.uia.runtime.ServiceMgr.setPeriod", "ti.uia.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_ServiceMgr_Module__startupDone__E", "ti_uia_runtime_ServiceMgr_freePacket__E", "ti_uia_runtime_ServiceMgr_getFreePacket__E", "ti_uia_runtime_ServiceMgr_getNumServices__E", "ti_uia_runtime_ServiceMgr_processCallback__E", "ti_uia_runtime_ServiceMgr_requestEnergy__E", "ti_uia_runtime_ServiceMgr_sendPacket__E", "ti_uia_runtime_ServiceMgr_setPeriod__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_invalidServiceId", "A_invalidProcessCallbackFxn"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "freePacket", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "getFreePacket", "entry", "%d, 0x%x", "exit", "%p"));
        loggables.add(Global.newObject("name", "getNumServices", "entry", "", "exit", "%d"));
        loggables.add(Global.newObject("name", "processCallback", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "requestEnergy", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "sendPacket", "entry", "%p", "exit", "%d"));
        loggables.add(Global.newObject("name", "setPeriod", "entry", "", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("ServiceMgr", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ServiceMgr");
    }

    void Transport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.Transport", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.Transport.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.Transport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        vo.bind("FxnSet", om.findStrict("ti.uia.runtime.Transport.FxnSet", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.Transport.FxnSet", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
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
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_Transport_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Transport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Transport");
    }

    void ICtxFilterCallback$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.ICtxFilterCallback", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ICtxFilterCallback.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.ICtxFilterCallback", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.uia.runtime.ICtxFilterCallback.Instance", "ti.uia.runtime"));
        vo.bind("Params", om.findStrict("ti.uia.runtime.ICtxFilterCallback.Params", "ti.uia.runtime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.runtime.ICtxFilterCallback.Params", "ti.uia.runtime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.runtime.ICtxFilterCallback.Handle", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ICtxFilterCallback", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ICtxFilterCallback");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IUIATransfer$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.IUIATransfer", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATransfer.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.IUIATransfer", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.uia.runtime.IUIATransfer.Instance", "ti.uia.runtime"));
        vo.bind("Params", om.findStrict("ti.uia.runtime.IUIATransfer.Params", "ti.uia.runtime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.runtime.IUIATransfer.Params", "ti.uia.runtime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.runtime.IUIATransfer.Handle", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.runtime"));
        vo.bind("Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.runtime"));
        vo.bind("MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.runtime"));
        vo.bind("TransferType_RELIABLE", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_RELIABLE", "ti.uia.runtime"));
        vo.bind("TransferType_LOSSY", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_LOSSY", "ti.uia.runtime"));
        vo.bind("Priority_LOW", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_LOW", "ti.uia.runtime"));
        vo.bind("Priority_STANDARD", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_STANDARD", "ti.uia.runtime"));
        vo.bind("Priority_HIGH", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_HIGH", "ti.uia.runtime"));
        vo.bind("Priority_SYNC", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_SYNC", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IUIATransfer", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IUIATransfer");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void ILoggerSnapshot$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.ILoggerSnapshot", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ILoggerSnapshot.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.ILoggerSnapshot", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Instance", "ti.uia.runtime"));
        vo.bind("Params", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Params", "ti.uia.runtime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.runtime.ILoggerSnapshot.Params", "ti.uia.runtime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Handle", "ti.uia.runtime"));
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.runtime"));
        vo.bind("Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.runtime"));
        vo.bind("MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.runtime"));
        tdefs.add(om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.runtime"));
        vo.bind("TransferType_RELIABLE", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_RELIABLE", "ti.uia.runtime"));
        vo.bind("TransferType_LOSSY", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_LOSSY", "ti.uia.runtime"));
        vo.bind("Priority_LOW", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_LOW", "ti.uia.runtime"));
        vo.bind("Priority_STANDARD", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_STANDARD", "ti.uia.runtime"));
        vo.bind("Priority_HIGH", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_HIGH", "ti.uia.runtime"));
        vo.bind("Priority_SYNC", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_SYNC", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.uia.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ILoggerSnapshot", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ILoggerSnapshot");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IUIATimestampProvider$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.IUIATimestampProvider", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATimestampProvider.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.IUIATimestampProvider", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
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
        pkgV.bind("IUIATimestampProvider", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IUIATimestampProvider");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IUIATraceSyncProvider$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.IUIATraceSyncProvider", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("ContextType", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved0", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved0", "ti.uia.runtime"));
        vo.bind("ContextType_SyncPoint", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_SyncPoint", "ti.uia.runtime"));
        vo.bind("ContextType_ContextChange", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_ContextChange", "ti.uia.runtime"));
        vo.bind("ContextType_Snapshot", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Snapshot", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved4", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved4", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved5", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved5", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved6", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved6", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved7", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved7", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved8", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved8", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved9", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved9", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved10", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved10", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved11", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved11", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved12", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved12", "ti.uia.runtime"));
        vo.bind("ContextType_Reserved13", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Reserved13", "ti.uia.runtime"));
        vo.bind("ContextType_Global32bTimestamp", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_Global32bTimestamp", "ti.uia.runtime"));
        vo.bind("ContextType_User", om.findStrict("ti.uia.runtime.IUIATraceSyncProvider.ContextType_User", "ti.uia.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.uia.events");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IUIATraceSyncProvider", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IUIATraceSyncProvider");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IServiceMgrSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.IServiceMgrSupport", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.IServiceMgrSupport.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.IServiceMgrSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IServiceMgrSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IServiceMgrSupport");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void LogSync_CpuTimestampProxy$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogSync_CpuTimestampProxy", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync_CpuTimestampProxy.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.LogSync_CpuTimestampProxy", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
        tdefs.clear();
        proxies.clear();
        proxies.add("delegate$");
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 0);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("get32", om.findStrict("ti.uia.runtime.LogSync_CpuTimestampProxy.get32", "ti.uia.runtime"));
        vo.bind("get64", om.findStrict("ti.uia.runtime.LogSync_CpuTimestampProxy.get64", "ti.uia.runtime"));
        vo.bind("getFreq", om.findStrict("ti.uia.runtime.LogSync_CpuTimestampProxy.getFreq", "ti.uia.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__Handle__label", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__Module__startupDone", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__Object__create", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__Object__delete", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__Object__get", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__Object__first", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__Object__next", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__Params__init", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__Proxy__abstract", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__Proxy__delegate", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__get32", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__get64", "ti_uia_runtime_LogSync_CpuTimestampProxy_DELEGATE__getFreq"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 1);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LogSync_CpuTimestampProxy", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LogSync_CpuTimestampProxy");
    }

    void LogSync_GlobalTimestampProxy$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogSync_GlobalTimestampProxy", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.LogSync_GlobalTimestampProxy.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.LogSync_GlobalTimestampProxy", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
        tdefs.clear();
        proxies.clear();
        proxies.add("delegate$");
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 0);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("get32", om.findStrict("ti.uia.runtime.LogSync_GlobalTimestampProxy.get32", "ti.uia.runtime"));
        vo.bind("get64", om.findStrict("ti.uia.runtime.LogSync_GlobalTimestampProxy.get64", "ti.uia.runtime"));
        vo.bind("getFreq", om.findStrict("ti.uia.runtime.LogSync_GlobalTimestampProxy.getFreq", "ti.uia.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__Handle__label", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__Module__startupDone", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__Object__create", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__Object__delete", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__Object__get", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__Object__first", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__Object__next", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__Params__init", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__Proxy__abstract", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__Proxy__delegate", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__get32", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__get64", "ti_uia_runtime_LogSync_GlobalTimestampProxy_DELEGATE__getFreq"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 1);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LogSync_GlobalTimestampProxy", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LogSync_GlobalTimestampProxy");
    }

    void ServiceMgr_SupportProxy$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy", "ti.uia.runtime");
        po = (Proto.Obj)om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy.Module", "ti.uia.runtime");
        vo.init2(po, "ti.uia.runtime.ServiceMgr_SupportProxy", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.uia.runtime", "ti.uia.runtime"));
        tdefs.clear();
        proxies.clear();
        proxies.add("delegate$");
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.uia.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.runtime")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 0);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("freePacket", om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy.freePacket", "ti.uia.runtime"));
        vo.bind("getFreePacket", om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy.getFreePacket", "ti.uia.runtime"));
        vo.bind("requestEnergy", om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy.requestEnergy", "ti.uia.runtime"));
        vo.bind("sendPacket", om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy.sendPacket", "ti.uia.runtime"));
        vo.bind("setPeriod", om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy.setPeriod", "ti.uia.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__Handle__label", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__Module__startupDone", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__Object__create", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__Object__delete", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__Object__get", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__Object__first", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__Object__next", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__Params__init", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__Proxy__abstract", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__Proxy__delegate", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__newService", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__freePacket", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__getFreePacket", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__requestEnergy", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__sendPacket", "ti_uia_runtime_ServiceMgr_SupportProxy_DELEGATE__setPeriod"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 1);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("ServiceMgr_SupportProxy", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ServiceMgr_SupportProxy");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.uia.runtime")).findStrict("PARAMS", "ti.uia.runtime");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.uia.runtime.LogSync", "ti.uia.runtime")).findStrict("PARAMS", "ti.uia.runtime");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.uia.runtime.LoggerSM", "ti.uia.runtime")).findStrict("PARAMS", "ti.uia.runtime");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.CtxFilter", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.EventHdr", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.LogCtxChg", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.LogSnapshot", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.LogSync", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.LoggerSM", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.LoggerTypes", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.MultiCoreTypes", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.QueueDescriptor", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.UIAMetaData", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.UIAPacket", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.ServiceMgr", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.Transport", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.LogSync_CpuTimestampProxy", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.LogSync_GlobalTimestampProxy", "ti.uia.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.runtime.ServiceMgr_SupportProxy", "ti.uia.runtime"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.runtime.CtxFilter", "ti.uia.runtime");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.runtime.LogSync", "ti.uia.runtime");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.runtime.LoggerSM", "ti.uia.runtime");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")}), Global.newArray(new Object[]{"Instances", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitInstances", "structName", "InstanceView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.runtime.QueueDescriptor", "ti.uia.runtime");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.runtime.ServiceMgr", "ti.uia.runtime");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")}), Global.newArray(new Object[]{"Transport", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitTransport", "structName", "TransportView")}), Global.newArray(new Object[]{"Packet", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitPacket", "structName", "PacketView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.uia.runtime.CtxFilter")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.EventHdr")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.LogCtxChg")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.IUIATraceSyncClient")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.LogSnapshot")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.LogSync")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.LoggerSM")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.LoggerTypes")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.MultiCoreTypes")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.QueueDescriptor")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.UIAMetaData")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.UIAPacket")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.ServiceMgr")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.Transport")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.ICtxFilterCallback")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.IUIATransfer")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.ILoggerSnapshot")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.IUIATimestampProvider")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.IUIATraceSyncProvider")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.IServiceMgrSupport")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.LogSync_CpuTimestampProxy")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.LogSync_GlobalTimestampProxy")).bless();
        ((Value.Obj)om.getv("ti.uia.runtime.ServiceMgr_SupportProxy")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.uia.runtime")).add(pkgV);
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
        CtxFilter$$OBJECTS();
        EventHdr$$OBJECTS();
        LogCtxChg$$OBJECTS();
        IUIATraceSyncClient$$OBJECTS();
        LogSnapshot$$OBJECTS();
        LogSync$$OBJECTS();
        LoggerSM$$OBJECTS();
        LoggerTypes$$OBJECTS();
        MultiCoreTypes$$OBJECTS();
        QueueDescriptor$$OBJECTS();
        UIAMetaData$$OBJECTS();
        UIAPacket$$OBJECTS();
        ServiceMgr$$OBJECTS();
        Transport$$OBJECTS();
        ICtxFilterCallback$$OBJECTS();
        IUIATransfer$$OBJECTS();
        ILoggerSnapshot$$OBJECTS();
        IUIATimestampProvider$$OBJECTS();
        IUIATraceSyncProvider$$OBJECTS();
        IServiceMgrSupport$$OBJECTS();
        LogSync_CpuTimestampProxy$$OBJECTS();
        LogSync_GlobalTimestampProxy$$OBJECTS();
        ServiceMgr_SupportProxy$$OBJECTS();
        CtxFilter$$CONSTS();
        EventHdr$$CONSTS();
        LogCtxChg$$CONSTS();
        IUIATraceSyncClient$$CONSTS();
        LogSnapshot$$CONSTS();
        LogSync$$CONSTS();
        LoggerSM$$CONSTS();
        LoggerTypes$$CONSTS();
        MultiCoreTypes$$CONSTS();
        QueueDescriptor$$CONSTS();
        UIAMetaData$$CONSTS();
        UIAPacket$$CONSTS();
        ServiceMgr$$CONSTS();
        Transport$$CONSTS();
        ICtxFilterCallback$$CONSTS();
        IUIATransfer$$CONSTS();
        ILoggerSnapshot$$CONSTS();
        IUIATimestampProvider$$CONSTS();
        IUIATraceSyncProvider$$CONSTS();
        IServiceMgrSupport$$CONSTS();
        LogSync_CpuTimestampProxy$$CONSTS();
        LogSync_GlobalTimestampProxy$$CONSTS();
        ServiceMgr_SupportProxy$$CONSTS();
        CtxFilter$$CREATES();
        EventHdr$$CREATES();
        LogCtxChg$$CREATES();
        IUIATraceSyncClient$$CREATES();
        LogSnapshot$$CREATES();
        LogSync$$CREATES();
        LoggerSM$$CREATES();
        LoggerTypes$$CREATES();
        MultiCoreTypes$$CREATES();
        QueueDescriptor$$CREATES();
        UIAMetaData$$CREATES();
        UIAPacket$$CREATES();
        ServiceMgr$$CREATES();
        Transport$$CREATES();
        ICtxFilterCallback$$CREATES();
        IUIATransfer$$CREATES();
        ILoggerSnapshot$$CREATES();
        IUIATimestampProvider$$CREATES();
        IUIATraceSyncProvider$$CREATES();
        IServiceMgrSupport$$CREATES();
        LogSync_CpuTimestampProxy$$CREATES();
        LogSync_GlobalTimestampProxy$$CREATES();
        ServiceMgr_SupportProxy$$CREATES();
        CtxFilter$$FUNCTIONS();
        EventHdr$$FUNCTIONS();
        LogCtxChg$$FUNCTIONS();
        IUIATraceSyncClient$$FUNCTIONS();
        LogSnapshot$$FUNCTIONS();
        LogSync$$FUNCTIONS();
        LoggerSM$$FUNCTIONS();
        LoggerTypes$$FUNCTIONS();
        MultiCoreTypes$$FUNCTIONS();
        QueueDescriptor$$FUNCTIONS();
        UIAMetaData$$FUNCTIONS();
        UIAPacket$$FUNCTIONS();
        ServiceMgr$$FUNCTIONS();
        Transport$$FUNCTIONS();
        ICtxFilterCallback$$FUNCTIONS();
        IUIATransfer$$FUNCTIONS();
        ILoggerSnapshot$$FUNCTIONS();
        IUIATimestampProvider$$FUNCTIONS();
        IUIATraceSyncProvider$$FUNCTIONS();
        IServiceMgrSupport$$FUNCTIONS();
        LogSync_CpuTimestampProxy$$FUNCTIONS();
        LogSync_GlobalTimestampProxy$$FUNCTIONS();
        ServiceMgr_SupportProxy$$FUNCTIONS();
        CtxFilter$$SIZES();
        EventHdr$$SIZES();
        LogCtxChg$$SIZES();
        IUIATraceSyncClient$$SIZES();
        LogSnapshot$$SIZES();
        LogSync$$SIZES();
        LoggerSM$$SIZES();
        LoggerTypes$$SIZES();
        MultiCoreTypes$$SIZES();
        QueueDescriptor$$SIZES();
        UIAMetaData$$SIZES();
        UIAPacket$$SIZES();
        ServiceMgr$$SIZES();
        Transport$$SIZES();
        ICtxFilterCallback$$SIZES();
        IUIATransfer$$SIZES();
        ILoggerSnapshot$$SIZES();
        IUIATimestampProvider$$SIZES();
        IUIATraceSyncProvider$$SIZES();
        IServiceMgrSupport$$SIZES();
        LogSync_CpuTimestampProxy$$SIZES();
        LogSync_GlobalTimestampProxy$$SIZES();
        ServiceMgr_SupportProxy$$SIZES();
        CtxFilter$$TYPES();
        EventHdr$$TYPES();
        LogCtxChg$$TYPES();
        IUIATraceSyncClient$$TYPES();
        LogSnapshot$$TYPES();
        LogSync$$TYPES();
        LoggerSM$$TYPES();
        LoggerTypes$$TYPES();
        MultiCoreTypes$$TYPES();
        QueueDescriptor$$TYPES();
        UIAMetaData$$TYPES();
        UIAPacket$$TYPES();
        ServiceMgr$$TYPES();
        Transport$$TYPES();
        ICtxFilterCallback$$TYPES();
        IUIATransfer$$TYPES();
        ILoggerSnapshot$$TYPES();
        IUIATimestampProvider$$TYPES();
        IUIATraceSyncProvider$$TYPES();
        IServiceMgrSupport$$TYPES();
        LogSync_CpuTimestampProxy$$TYPES();
        LogSync_GlobalTimestampProxy$$TYPES();
        ServiceMgr_SupportProxy$$TYPES();
        if (isROV) {
            CtxFilter$$ROV();
            EventHdr$$ROV();
            LogCtxChg$$ROV();
            IUIATraceSyncClient$$ROV();
            LogSnapshot$$ROV();
            LogSync$$ROV();
            LoggerSM$$ROV();
            LoggerTypes$$ROV();
            MultiCoreTypes$$ROV();
            QueueDescriptor$$ROV();
            UIAMetaData$$ROV();
            UIAPacket$$ROV();
            ServiceMgr$$ROV();
            Transport$$ROV();
            ICtxFilterCallback$$ROV();
            IUIATransfer$$ROV();
            ILoggerSnapshot$$ROV();
            IUIATimestampProvider$$ROV();
            IUIATraceSyncProvider$$ROV();
            IServiceMgrSupport$$ROV();
            LogSync_CpuTimestampProxy$$ROV();
            LogSync_GlobalTimestampProxy$$ROV();
            ServiceMgr_SupportProxy$$ROV();
        }//isROV
        $$SINGLETONS();
        CtxFilter$$SINGLETONS();
        EventHdr$$SINGLETONS();
        LogCtxChg$$SINGLETONS();
        IUIATraceSyncClient$$SINGLETONS();
        LogSnapshot$$SINGLETONS();
        LogSync$$SINGLETONS();
        LoggerSM$$SINGLETONS();
        LoggerTypes$$SINGLETONS();
        MultiCoreTypes$$SINGLETONS();
        QueueDescriptor$$SINGLETONS();
        UIAMetaData$$SINGLETONS();
        UIAPacket$$SINGLETONS();
        ServiceMgr$$SINGLETONS();
        Transport$$SINGLETONS();
        ICtxFilterCallback$$SINGLETONS();
        IUIATransfer$$SINGLETONS();
        ILoggerSnapshot$$SINGLETONS();
        IUIATimestampProvider$$SINGLETONS();
        IUIATraceSyncProvider$$SINGLETONS();
        IServiceMgrSupport$$SINGLETONS();
        LogSync_CpuTimestampProxy$$SINGLETONS();
        LogSync_GlobalTimestampProxy$$SINGLETONS();
        ServiceMgr_SupportProxy$$SINGLETONS();
        $$INITIALIZATION();
    }
}
