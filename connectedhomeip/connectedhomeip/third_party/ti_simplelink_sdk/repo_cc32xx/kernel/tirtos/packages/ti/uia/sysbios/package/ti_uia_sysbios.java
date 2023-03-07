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

public class ti_uia_sysbios
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
        Global.callFxn("loadPackage", xdcO, "ti.uia.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.syncs");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.uia.sysbios.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.uia.sysbios", new Value.Obj("ti.uia.sysbios", pkgP));
    }

    void IpcMP$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.sysbios.IpcMP.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.sysbios.IpcMP", new Value.Obj("ti.uia.sysbios.IpcMP", po));
        pkgV.bind("IpcMP", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.IpcMP$$Module_State", new Proto.Obj());
        om.bind("ti.uia.sysbios.IpcMP.Module_State", new Proto.Str(spo, false));
    }

    void Adaptor$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.sysbios.Adaptor.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.sysbios.Adaptor", new Value.Obj("ti.uia.sysbios.Adaptor", po));
        pkgV.bind("Adaptor", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.Adaptor$$Entry", new Proto.Obj());
        om.bind("ti.uia.sysbios.Adaptor.Entry", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.Adaptor$$Module_State", new Proto.Obj());
        om.bind("ti.uia.sysbios.Adaptor.Module_State", new Proto.Str(spo, false));
    }

    void LoggingSetup$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.sysbios.LoggingSetup.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.sysbios.LoggingSetup", new Value.Obj("ti.uia.sysbios.LoggingSetup", po));
        pkgV.bind("LoggingSetup", vo);
        // decls 
        om.bind("ti.uia.sysbios.LoggingSetup.TimestampSize", new Proto.Enm("ti.uia.sysbios.LoggingSetup.TimestampSize"));
        om.bind("ti.uia.sysbios.LoggingSetup.UploadMode", new Proto.Enm("ti.uia.sysbios.LoggingSetup.UploadMode"));
        om.bind("ti.uia.sysbios.LoggingSetup.LoggerType", new Proto.Enm("ti.uia.sysbios.LoggingSetup.LoggerType"));
    }

    void LoggerStreamer$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.sysbios.LoggerStreamer", new Value.Obj("ti.uia.sysbios.LoggerStreamer", po));
        pkgV.bind("LoggerStreamer", vo);
        // decls 
        om.bind("ti.uia.sysbios.LoggerStreamer.TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.sysbios"));
        om.bind("ti.uia.sysbios.LoggerStreamer.Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.sysbios"));
        om.bind("ti.uia.sysbios.LoggerStreamer.MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.sysbios"));
        om.bind("ti.uia.sysbios.LoggerStreamer.TransportType", new Proto.Enm("ti.uia.sysbios.LoggerStreamer.TransportType"));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer$$RecordView", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer.RecordView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer$$Module_State", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer.Module_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.uia.sysbios.LoggerStreamer.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer$$Object", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer$$Params", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer.Params", new Proto.Str(po, false));
        om.bind("ti.uia.sysbios.LoggerStreamer.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.sysbios.LoggerStreamer.Object", om.findStrict("ti.uia.sysbios.LoggerStreamer.Instance_State", "ti.uia.sysbios"));
        }//isROV
    }

    void LoggerStreamer2$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer2.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.sysbios.LoggerStreamer2", new Value.Obj("ti.uia.sysbios.LoggerStreamer2", po));
        pkgV.bind("LoggerStreamer2", vo);
        // decls 
        om.bind("ti.uia.sysbios.LoggerStreamer2.TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.sysbios"));
        om.bind("ti.uia.sysbios.LoggerStreamer2.Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.sysbios"));
        om.bind("ti.uia.sysbios.LoggerStreamer2.MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.sysbios"));
        om.bind("ti.uia.sysbios.LoggerStreamer2.TransportType", new Proto.Enm("ti.uia.sysbios.LoggerStreamer2.TransportType"));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer2$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer2.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer2$$InstanceView", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer2.InstanceView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer2$$RecordView", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer2.RecordView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer2$$RtaData", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer2.RtaData", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer2$$Module_State", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer2.Module_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer2$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer2.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.uia.sysbios.LoggerStreamer2.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer2$$Object", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer2.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerStreamer2$$Params", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerStreamer2.Params", new Proto.Str(po, false));
        om.bind("ti.uia.sysbios.LoggerStreamer2.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.sysbios.LoggerStreamer2.Object", om.findStrict("ti.uia.sysbios.LoggerStreamer2.Instance_State", "ti.uia.sysbios"));
        }//isROV
    }

    void LoggerIdle$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerIdle.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.sysbios.LoggerIdle", new Value.Obj("ti.uia.sysbios.LoggerIdle", po));
        pkgV.bind("LoggerIdle", vo);
        // decls 
        om.bind("ti.uia.sysbios.LoggerIdle.TransportType", new Proto.Enm("ti.uia.sysbios.LoggerIdle.TransportType"));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerIdle$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerIdle.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerIdle$$RecordView", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerIdle.RecordView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerIdle$$RtaData", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerIdle.RtaData", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerIdle$$Module_State", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerIdle.Module_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerIdle$$Instance_State", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerIdle.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.uia.sysbios.LoggerIdle.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerIdle$$Object", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerIdle.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.uia.sysbios.LoggerIdle$$Params", new Proto.Obj());
        om.bind("ti.uia.sysbios.LoggerIdle.Params", new Proto.Str(po, false));
        om.bind("ti.uia.sysbios.LoggerIdle.Handle", insP);
        if (isROV) {
            om.bind("ti.uia.sysbios.LoggerIdle.Object", om.findStrict("ti.uia.sysbios.LoggerIdle.Instance_State", "ti.uia.sysbios"));
        }//isROV
    }

    void IpcMP$$CONSTS()
    {
        // module IpcMP
        om.bind("ti.uia.sysbios.IpcMP.freePacket", new Extern("ti_uia_sysbios_IpcMP_freePacket__E", "xdc_Void(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.getFreePacket", new Extern("ti_uia_sysbios_IpcMP_getFreePacket__E", "ti_uia_runtime_UIAPacket_Hdr*(*)(ti_uia_runtime_UIAPacket_HdrType,xdc_UInt)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.requestEnergy", new Extern("ti_uia_sysbios_IpcMP_requestEnergy__E", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.sendPacket", new Extern("ti_uia_sysbios_IpcMP_sendPacket__E", "xdc_Bool(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.setPeriod", new Extern("ti_uia_sysbios_IpcMP_setPeriod__E", "xdc_Void(*)(xdc_Int,xdc_UInt32)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.rxTaskFxn", new Extern("ti_uia_sysbios_IpcMP_rxTaskFxn__E", "xdc_Void(*)(xdc_UArg,xdc_UArg)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.transferAgentFxn", new Extern("ti_uia_sysbios_IpcMP_transferAgentFxn__E", "xdc_Void(*)(xdc_UArg,xdc_UArg)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.clockFxn", new Extern("ti_uia_sysbios_IpcMP_clockFxn__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.start", new Extern("ti_uia_sysbios_IpcMP_start__E", "xdc_Int(*)(xdc_UArg,xdc_UInt16)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.startOrig", new Extern("ti_uia_sysbios_IpcMP_startOrig__I", "xdc_Int(*)(xdc_Ptr*,xdc_UInt16)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.giveEnergy", new Extern("ti_uia_sysbios_IpcMP_giveEnergy__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.handleMsg", new Extern("ti_uia_sysbios_IpcMP_handleMsg__I", "xdc_Void(*)(xdc_Ptr)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.prime", new Extern("ti_uia_sysbios_IpcMP_prime__I", "xdc_Void(*)(xdc_Ptr,xdc_Int,xdc_Int)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.registerWithMaster", new Extern("ti_uia_sysbios_IpcMP_registerWithMaster__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.sysbios.IpcMP.requestEvents", new Extern("ti_uia_sysbios_IpcMP_requestEvents__I", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void Adaptor$$CONSTS()
    {
        // module Adaptor
        om.bind("ti.uia.sysbios.Adaptor.freePacket", new Extern("ti_uia_sysbios_Adaptor_freePacket__E", "xdc_Void(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.getFreePacket", new Extern("ti_uia_sysbios_Adaptor_getFreePacket__E", "ti_uia_runtime_UIAPacket_Hdr*(*)(ti_uia_runtime_UIAPacket_HdrType,xdc_UInt)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.requestEnergy", new Extern("ti_uia_sysbios_Adaptor_requestEnergy__E", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.sendPacket", new Extern("ti_uia_sysbios_Adaptor_sendPacket__E", "xdc_Bool(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.setPeriod", new Extern("ti_uia_sysbios_Adaptor_setPeriod__E", "xdc_Void(*)(xdc_Int,xdc_UInt32)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.rxTaskFxn", new Extern("ti_uia_sysbios_Adaptor_rxTaskFxn__E", "xdc_Void(*)(xdc_UArg,xdc_UArg)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.transferAgentTaskFxn", new Extern("ti_uia_sysbios_Adaptor_transferAgentTaskFxn__E", "xdc_Void(*)(xdc_UArg,xdc_UArg)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.clockFxn", new Extern("ti_uia_sysbios_Adaptor_clockFxn__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.giveEnergy", new Extern("ti_uia_sysbios_Adaptor_giveEnergy__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.sendToHost", new Extern("ti_uia_sysbios_Adaptor_sendToHost__I", "xdc_Bool(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.sendToService", new Extern("ti_uia_sysbios_Adaptor_sendToService__I", "xdc_Void(*)(ti_uia_sysbios_Adaptor_Entry*)", true, false));
        om.bind("ti.uia.sysbios.Adaptor.runScheduledServices", new Extern("ti_uia_sysbios_Adaptor_runScheduledServices__I", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void LoggingSetup$$CONSTS()
    {
        // module LoggingSetup
        om.bind("ti.uia.sysbios.LoggingSetup.TimestampSize_AUTO", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.TimestampSize", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.TimestampSize_AUTO", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.TimestampSize_NONE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.TimestampSize", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.TimestampSize_NONE", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.TimestampSize_32b", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.TimestampSize", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.TimestampSize_32b", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.TimestampSize_64b", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.TimestampSize", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.TimestampSize_64b", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.UploadMode_SIMULATOR", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.UploadMode_SIMULATOR", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.UploadMode_PROBEPOINT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.UploadMode_PROBEPOINT", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.UploadMode_JTAGSTOPMODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.UploadMode_JTAGSTOPMODE", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.UploadMode_JTAGRUNMODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.UploadMode_JTAGRUNMODE", xdc.services.intern.xsr.Enum.intValue(4L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.UploadMode_NONJTAGTRANSPORT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.UploadMode_NONJTAGTRANSPORT", xdc.services.intern.xsr.Enum.intValue(5L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.UploadMode_NONJTAG_AND_JTAGSTOPMODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.UploadMode_NONJTAG_AND_JTAGSTOPMODE", xdc.services.intern.xsr.Enum.intValue(6L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.UploadMode_STREAMER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.UploadMode_STREAMER", xdc.services.intern.xsr.Enum.intValue(7L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.UploadMode_IDLE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.UploadMode_IDLE", xdc.services.intern.xsr.Enum.intValue(8L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.UploadMode_STREAMER2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.UploadMode_STREAMER2", xdc.services.intern.xsr.Enum.intValue(9L)+0));
        om.bind("ti.uia.sysbios.LoggingSetup.LoggerType_MIN", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.LoggerType_MIN", 0));
        om.bind("ti.uia.sysbios.LoggingSetup.LoggerType_STOPMODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.LoggerType_STOPMODE", 1));
        om.bind("ti.uia.sysbios.LoggingSetup.LoggerType_JTAGRUNMODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.LoggerType_JTAGRUNMODE", 2));
        om.bind("ti.uia.sysbios.LoggingSetup.LoggerType_RUNMODE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.LoggerType_RUNMODE", 3));
        om.bind("ti.uia.sysbios.LoggingSetup.LoggerType_IDLE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.LoggerType_IDLE", 4));
        om.bind("ti.uia.sysbios.LoggingSetup.LoggerType_STREAMER2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggingSetup.LoggerType_STREAMER2", 5));
    }

    void LoggerStreamer$$CONSTS()
    {
        // module LoggerStreamer
        om.bind("ti.uia.sysbios.LoggerStreamer.TransportType_UART", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerStreamer.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerStreamer.TransportType_UART", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.sysbios.LoggerStreamer.TransportType_USB", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerStreamer.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerStreamer.TransportType_USB", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.sysbios.LoggerStreamer.TransportType_ETHERNET", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerStreamer.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerStreamer.TransportType_ETHERNET", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.sysbios.LoggerStreamer.TransportType_CUSTOM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerStreamer.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerStreamer.TransportType_CUSTOM", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.sysbios.LoggerStreamer.WRITE0_SIZE_IN_BYTES", 8L);
        om.bind("ti.uia.sysbios.LoggerStreamer.WRITE1_SIZE_IN_BYTES", 12L);
        om.bind("ti.uia.sysbios.LoggerStreamer.WRITE2_SIZE_IN_BYTES", 16L);
        om.bind("ti.uia.sysbios.LoggerStreamer.WRITE4_SIZE_IN_BYTES", 24L);
        om.bind("ti.uia.sysbios.LoggerStreamer.WRITE8_SIZE_IN_BYTES", 40L);
        om.bind("ti.uia.sysbios.LoggerStreamer.TIMESTAMP", 8L);
        om.bind("ti.uia.sysbios.LoggerStreamer.NO_TIMESTAMP", 0L);
        om.bind("ti.uia.sysbios.LoggerStreamer.flush", new Extern("ti_uia_sysbios_LoggerStreamer_flush__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.sysbios.LoggerStreamer.prime", new Extern("ti_uia_sysbios_LoggerStreamer_prime__E", "xdc_Bool(*)(xdc_Ptr)", true, false));
        om.bind("ti.uia.sysbios.LoggerStreamer.setModuleIdToRouteToStatusLogger", new Extern("ti_uia_sysbios_LoggerStreamer_setModuleIdToRouteToStatusLogger__E", "xdc_Void(*)(xdc_Bits16)", true, false));
        om.bind("ti.uia.sysbios.LoggerStreamer.validatePacket", new Extern("ti_uia_sysbios_LoggerStreamer_validatePacket__E", "xdc_Char*(*)(xdc_UInt32*,xdc_UInt32)", true, false));
        om.bind("ti.uia.sysbios.LoggerStreamer.filterOutEvent", new Extern("ti_uia_sysbios_LoggerStreamer_filterOutEvent__I", "xdc_Bool(*)(xdc_Bits16)", true, false));
    }

    void LoggerStreamer2$$CONSTS()
    {
        // module LoggerStreamer2
        om.bind("ti.uia.sysbios.LoggerStreamer2.TransportType_UART", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerStreamer2.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerStreamer2.TransportType_UART", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.sysbios.LoggerStreamer2.TransportType_USB", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerStreamer2.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerStreamer2.TransportType_USB", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.sysbios.LoggerStreamer2.TransportType_ETHERNET", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerStreamer2.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerStreamer2.TransportType_ETHERNET", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.sysbios.LoggerStreamer2.TransportType_CUSTOM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerStreamer2.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerStreamer2.TransportType_CUSTOM", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.sysbios.LoggerStreamer2.WRITE0_SIZE_IN_BYTES", 8L);
        om.bind("ti.uia.sysbios.LoggerStreamer2.WRITE1_SIZE_IN_BYTES", 12L);
        om.bind("ti.uia.sysbios.LoggerStreamer2.WRITE2_SIZE_IN_BYTES", 16L);
        om.bind("ti.uia.sysbios.LoggerStreamer2.WRITE4_SIZE_IN_BYTES", 24L);
        om.bind("ti.uia.sysbios.LoggerStreamer2.WRITE8_SIZE_IN_BYTES", 40L);
        om.bind("ti.uia.sysbios.LoggerStreamer2.TIMESTAMP", 8L);
        om.bind("ti.uia.sysbios.LoggerStreamer2.NO_TIMESTAMP", 0L);
        om.bind("ti.uia.sysbios.LoggerStreamer2.setModuleIdToRouteToStatusLogger", new Extern("ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger__E", "xdc_Void(*)(xdc_Bits16)", true, false));
        om.bind("ti.uia.sysbios.LoggerStreamer2.filterOutEvent", new Extern("ti_uia_sysbios_LoggerStreamer2_filterOutEvent__I", "xdc_Bool(*)(xdc_Bits16)", true, false));
    }

    void LoggerIdle$$CONSTS()
    {
        // module LoggerIdle
        om.bind("ti.uia.sysbios.LoggerIdle.TransportType_UART", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerIdle.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerIdle.TransportType_UART", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.sysbios.LoggerIdle.TransportType_USB", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerIdle.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerIdle.TransportType_USB", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.sysbios.LoggerIdle.TransportType_ETHERNET", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerIdle.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerIdle.TransportType_ETHERNET", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.sysbios.LoggerIdle.TransportType_CUSTOM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.sysbios.LoggerIdle.TransportType", "ti.uia.sysbios"), "ti.uia.sysbios.LoggerIdle.TransportType_CUSTOM", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.sysbios.LoggerIdle.flush", new Extern("ti_uia_sysbios_LoggerIdle_flush__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.sysbios.LoggerIdle.idleWrite", new Extern("ti_uia_sysbios_LoggerIdle_idleWrite__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.sysbios.LoggerIdle.idleWriteEvent", new Extern("ti_uia_sysbios_LoggerIdle_idleWriteEvent__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.sysbios.LoggerIdle.write", new Extern("ti_uia_sysbios_LoggerIdle_write__I", "xdc_Void(*)(xdc_runtime_Log_Event,xdc_Bits16,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg,xdc_IArg)", true, false));
    }

    void IpcMP$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Adaptor$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LoggingSetup$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LoggerStreamer$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$uia$sysbios$LoggerStreamer$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.uia.sysbios.LoggerStreamer.create() called before xdc.useModule('ti.uia.sysbios.LoggerStreamer')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.uia.sysbios.LoggerStreamer$$create", new Proto.Fxn(om.findStrict("ti.uia.sysbios.LoggerStreamer.Module", "ti.uia.sysbios"), om.findStrict("ti.uia.sysbios.LoggerStreamer.Instance", "ti.uia.sysbios"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.uia.sysbios.LoggerStreamer.Params", "ti.uia.sysbios"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$sysbios$LoggerStreamer$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.sysbios.LoggerStreamer'];\n");
                sb.append("var __inst = xdc.om['ti.uia.sysbios.LoggerStreamer.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.uia.sysbios']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.uia.sysbios.LoggerStreamer'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.sysbios.LoggerStreamer'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.uia.sysbios.LoggerStreamer$$construct", new Proto.Fxn(om.findStrict("ti.uia.sysbios.LoggerStreamer.Module", "ti.uia.sysbios"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.uia.sysbios.LoggerStreamer$$Object", "ti.uia.sysbios"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.uia.sysbios.LoggerStreamer.Params", "ti.uia.sysbios"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$sysbios$LoggerStreamer$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.sysbios.LoggerStreamer'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.uia.sysbios.LoggerStreamer'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.sysbios.LoggerStreamer'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void LoggerStreamer2$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$uia$sysbios$LoggerStreamer2$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.uia.sysbios.LoggerStreamer2.create() called before xdc.useModule('ti.uia.sysbios.LoggerStreamer2')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.uia.sysbios.LoggerStreamer2$$create", new Proto.Fxn(om.findStrict("ti.uia.sysbios.LoggerStreamer2.Module", "ti.uia.sysbios"), om.findStrict("ti.uia.sysbios.LoggerStreamer2.Instance", "ti.uia.sysbios"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.uia.sysbios.LoggerStreamer2.Params", "ti.uia.sysbios"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$sysbios$LoggerStreamer2$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.sysbios.LoggerStreamer2'];\n");
                sb.append("var __inst = xdc.om['ti.uia.sysbios.LoggerStreamer2.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.uia.sysbios']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.uia.sysbios.LoggerStreamer2'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.sysbios.LoggerStreamer2'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.uia.sysbios.LoggerStreamer2$$construct", new Proto.Fxn(om.findStrict("ti.uia.sysbios.LoggerStreamer2.Module", "ti.uia.sysbios"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$Object", "ti.uia.sysbios"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.uia.sysbios.LoggerStreamer2.Params", "ti.uia.sysbios"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$sysbios$LoggerStreamer2$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.sysbios.LoggerStreamer2'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.uia.sysbios.LoggerStreamer2'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.sysbios.LoggerStreamer2'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void LoggerIdle$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$uia$sysbios$LoggerIdle$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.uia.sysbios.LoggerIdle.create() called before xdc.useModule('ti.uia.sysbios.LoggerIdle')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.uia.sysbios.LoggerIdle$$create", new Proto.Fxn(om.findStrict("ti.uia.sysbios.LoggerIdle.Module", "ti.uia.sysbios"), om.findStrict("ti.uia.sysbios.LoggerIdle.Instance", "ti.uia.sysbios"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.uia.sysbios.LoggerIdle.Params", "ti.uia.sysbios"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$sysbios$LoggerIdle$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.sysbios.LoggerIdle'];\n");
                sb.append("var __inst = xdc.om['ti.uia.sysbios.LoggerIdle.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.uia.sysbios']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.uia.sysbios.LoggerIdle'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.sysbios.LoggerIdle'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.uia.sysbios.LoggerIdle$$construct", new Proto.Fxn(om.findStrict("ti.uia.sysbios.LoggerIdle.Module", "ti.uia.sysbios"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.uia.sysbios.LoggerIdle$$Object", "ti.uia.sysbios"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.uia.sysbios.LoggerIdle.Params", "ti.uia.sysbios"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$uia$sysbios$LoggerIdle$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.uia.sysbios.LoggerIdle'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.uia.sysbios.LoggerIdle'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.uia.sysbios.LoggerIdle'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void IpcMP$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn IpcMP.doNotPlugIpc
        fxn = (Proto.Fxn)om.bind("ti.uia.sysbios.IpcMP$$doNotPlugIpc", new Proto.Fxn(om.findStrict("ti.uia.sysbios.IpcMP.Module", "ti.uia.sysbios"), null, 0, 0, false));
    }

    void Adaptor$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LoggingSetup$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn LoggingSetup.createLogger
        fxn = (Proto.Fxn)om.bind("ti.uia.sysbios.LoggingSetup$$createLogger", new Proto.Fxn(om.findStrict("ti.uia.sysbios.LoggingSetup.Module", "ti.uia.sysbios"), null, 0, -1, false));
        // fxn LoggingSetup.writeUIAMetaData
        fxn = (Proto.Fxn)om.bind("ti.uia.sysbios.LoggingSetup$$writeUIAMetaData", new Proto.Fxn(om.findStrict("ti.uia.sysbios.LoggingSetup.Module", "ti.uia.sysbios"), null, 0, -1, false));
    }

    void LoggerStreamer$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LoggerStreamer2$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LoggerIdle$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn LoggerIdle.initDecoder
        fxn = (Proto.Fxn)om.bind("ti.uia.sysbios.LoggerIdle$$initDecoder", new Proto.Fxn(om.findStrict("ti.uia.sysbios.LoggerIdle.Module", "ti.uia.sysbios"), null, 0, -1, false));
    }

    void IpcMP$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.sysbios.IpcMP.Module_State", "ti.uia.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("event", "UPtr"));
        sizes.add(Global.newArray("clock", "UPtr"));
        sizes.add(Global.newArray("freeEventMQ", "UPtr"));
        sizes.add(Global.newArray("freeMsgMQ", "UPtr"));
        sizes.add(Global.newArray("routerMQ", "UPtr"));
        sizes.add(Global.newArray("startedMQ", "UPtr"));
        sizes.add(Global.newArray("masterMQ", "UInt32"));
        sizes.add(Global.newArray("incomingMsgMQ", "UPtr"));
        sizes.add(Global.newArray("replyMQ", "UPtr"));
        sizes.add(Global.newArray("syncEvent01", "UPtr"));
        sizes.add(Global.newArray("transferAgentHandle", "UPtr"));
        sizes.add(Global.newArray("transportMsgHandle", "UPtr"));
        sizes.add(Global.newArray("transportEventHandle", "UPtr"));
        sizes.add(Global.newArray("releaseTasksSem", "UPtr"));
        sizes.add(Global.newArray("numMSGPacketsSent", "TInt"));
        sizes.add(Global.newArray("numEventPacketsSent", "TInt"));
        sizes.add(Global.newArray("master", "UShort"));
        sizes.add(Global.newArray("masterRunning", "UShort"));
        sizes.add(Global.newArray("period", "UPtr"));
        sizes.add(Global.newArray("scheduled", "UPtr"));
        sizes.add(Global.newArray("reqEnergy", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.sysbios.IpcMP.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.sysbios.IpcMP.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.sysbios.IpcMP.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Adaptor$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.sysbios.Adaptor.Entry", "ti.uia.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("elem", "Sti.sysbios.knl.Queue;Elem"));
        sizes.add(Global.newArray("packet", "Sti.uia.runtime.UIAPacket;Hdr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.sysbios.Adaptor.Entry']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.sysbios.Adaptor.Entry']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.sysbios.Adaptor.Entry'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.sysbios.Adaptor.Module_State", "ti.uia.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("event", "UPtr"));
        sizes.add(Global.newArray("clock", "UPtr"));
        sizes.add(Global.newArray("freeEventSem", "UPtr"));
        sizes.add(Global.newArray("freeMsgSem", "UPtr"));
        sizes.add(Global.newArray("incomingSem", "UPtr"));
        sizes.add(Global.newArray("freeEventQ", "UPtr"));
        sizes.add(Global.newArray("freeMsgQ", "UPtr"));
        sizes.add(Global.newArray("incomingQ", "UPtr"));
        sizes.add(Global.newArray("outgoingQ", "UPtr"));
        sizes.add(Global.newArray("syncEvent01", "UPtr"));
        sizes.add(Global.newArray("transferAgentHandle", "UPtr"));
        sizes.add(Global.newArray("transportMsgHandle", "UPtr"));
        sizes.add(Global.newArray("transportEventHandle", "UPtr"));
        sizes.add(Global.newArray("numMsgPacketsSent", "TInt"));
        sizes.add(Global.newArray("numMsgPacketsFailed", "TInt"));
        sizes.add(Global.newArray("numEventPacketsSent", "TInt"));
        sizes.add(Global.newArray("numEventPacketsFailed", "TInt"));
        sizes.add(Global.newArray("period", "UPtr"));
        sizes.add(Global.newArray("scheduled", "UPtr"));
        sizes.add(Global.newArray("reqEnergy", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.sysbios.Adaptor.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.sysbios.Adaptor.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.sysbios.Adaptor.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LoggingSetup$$SIZES()
    {
    }

    void LoggerStreamer$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.sysbios.LoggerStreamer.Module_State", "ti.uia.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("enabled", "UShort"));
        sizes.add(Global.newArray("buffer", "UPtr"));
        sizes.add(Global.newArray("write", "UPtr"));
        sizes.add(Global.newArray("end", "UPtr"));
        sizes.add(Global.newArray("level1", "UInt16"));
        sizes.add(Global.newArray("level2", "UInt16"));
        sizes.add(Global.newArray("level3", "UInt16"));
        sizes.add(Global.newArray("moduleIdToRouteToStatusLogger", "UInt16"));
        sizes.add(Global.newArray("maxEventSizeInBits32", "USize"));
        sizes.add(Global.newArray("droppedEvents", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.sysbios.LoggerStreamer.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.sysbios.LoggerStreamer.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.sysbios.LoggerStreamer.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.sysbios.LoggerStreamer.Instance_State", "ti.uia.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.sysbios.LoggerStreamer.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.sysbios.LoggerStreamer.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.sysbios.LoggerStreamer.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LoggerStreamer2$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.sysbios.LoggerStreamer2.Module_State", "ti.uia.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("enabled", "UShort"));
        sizes.add(Global.newArray("level1", "UInt16"));
        sizes.add(Global.newArray("level2", "UInt16"));
        sizes.add(Global.newArray("level3", "UInt16"));
        sizes.add(Global.newArray("moduleIdToRouteToStatusLogger", "UInt16"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.sysbios.LoggerStreamer2.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.sysbios.LoggerStreamer2.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.sysbios.LoggerStreamer2.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.sysbios.LoggerStreamer2.Instance_State", "ti.uia.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("enabled", "UShort"));
        sizes.add(Global.newArray("instanceId", "TInt16"));
        sizes.add(Global.newArray("primeFxn", "UFxn"));
        sizes.add(Global.newArray("exchangeFxn", "UFxn"));
        sizes.add(Global.newArray("context", "UIArg"));
        sizes.add(Global.newArray("primeStatus", "UShort"));
        sizes.add(Global.newArray("bufSize", "UInt32"));
        sizes.add(Global.newArray("buffer", "UPtr"));
        sizes.add(Global.newArray("write", "UPtr"));
        sizes.add(Global.newArray("end", "UPtr"));
        sizes.add(Global.newArray("maxEventSizeUArg", "USize"));
        sizes.add(Global.newArray("maxEventSize", "USize"));
        sizes.add(Global.newArray("droppedEvents", "TInt"));
        sizes.add(Global.newArray("seqNumber", "UInt16"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.sysbios.LoggerStreamer2.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.sysbios.LoggerStreamer2.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.sysbios.LoggerStreamer2.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LoggerIdle$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.sysbios.LoggerIdle.Module_State", "ti.uia.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("loggerFxn", "UFxn"));
        sizes.add(Global.newArray("enabled", "UShort"));
        sizes.add(Global.newArray("empty", "UShort"));
        sizes.add(Global.newArray("bufferSize", "UInt"));
        sizes.add(Global.newArray("idleBuffer", "UPtr"));
        sizes.add(Global.newArray("tempBuffer", "UPtr"));
        sizes.add(Global.newArray("bufferRead", "UPtr"));
        sizes.add(Global.newArray("bufferWrite", "UPtr"));
        sizes.add(Global.newArray("bufferPad", "UPtr"));
        sizes.add(Global.newArray("bufferEnd", "UPtr"));
        sizes.add(Global.newArray("eventSequenceNum", "UInt16"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.sysbios.LoggerIdle.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.sysbios.LoggerIdle.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.sysbios.LoggerIdle.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.sysbios.LoggerIdle.Instance_State", "ti.uia.sysbios");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.sysbios.LoggerIdle.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.sysbios.LoggerIdle.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.sysbios.LoggerIdle.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void IpcMP$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/sysbios/IpcMP.xs");
        om.bind("ti.uia.sysbios.IpcMP$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.IpcMP.Module", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.IpcMP.Module", om.findStrict("ti.uia.runtime.IServiceMgrSupport.Module", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("A_IpcMPFailure", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.uia.sysbios"), Global.newObject("msg", "A_IpcMPFailure: Unexpected failure with the IpcMP"), "w");
            po.addFld("sharedRegionId", Proto.Elm.newCNum("(xdc_Int)"), 0L, "w");
            po.addFld("createRxTask", $$T_Bool, false, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.sysbios.IpcMP$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.sysbios.IpcMP$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.sysbios.IpcMP$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.sysbios.IpcMP$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "newService");
                if (fxn != null) po.addFxn("newService", (Proto.Fxn)om.findStrict("ti.uia.runtime.IServiceMgrSupport$$newService", "ti.uia.sysbios"), fxn);
                po.addFxn("doNotPlugIpc", (Proto.Fxn)om.findStrict("ti.uia.sysbios.IpcMP$$doNotPlugIpc", "ti.uia.sysbios"), Global.get(cap, "doNotPlugIpc"));
        // struct IpcMP.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.IpcMP$$Module_State", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.IpcMP.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("event", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("clock", (Proto)om.findStrict("ti.sysbios.knl.Clock.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("freeEventMQ", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("freeMsgMQ", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("routerMQ", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("startedMQ", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("masterMQ", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("incomingMsgMQ", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("replyMQ", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false), $$DEFAULT, "w");
                po.addFld("syncEvent01", (Proto)om.findStrict("ti.sysbios.syncs.SyncEvent.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("transferAgentHandle", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("transportMsgHandle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("transportEventHandle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("releaseTasksSem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("numMSGPacketsSent", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numEventPacketsSent", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("master", $$T_Bool, $$UNDEF, "w");
                po.addFld("masterRunning", $$T_Bool, $$UNDEF, "w");
                po.addFld("period", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false), $$DEFAULT, "w");
                po.addFld("scheduled", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false), $$DEFAULT, "w");
                po.addFld("reqEnergy", new Proto.Arr($$T_Bool, false), $$DEFAULT, "w");
    }

    void Adaptor$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/sysbios/Adaptor.xs");
        om.bind("ti.uia.sysbios.Adaptor$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.Adaptor.Module", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.Adaptor.Module", om.findStrict("ti.uia.runtime.IServiceMgrSupport.Module", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("packetSection", $$T_Str, $$UNDEF, "wh");
            po.addFld("eventBuf", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
            po.addFld("msgBuf", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.sysbios.Adaptor$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.sysbios.Adaptor$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.sysbios.Adaptor$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.sysbios.Adaptor$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "newService");
                if (fxn != null) po.addFxn("newService", (Proto.Fxn)om.findStrict("ti.uia.runtime.IServiceMgrSupport$$newService", "ti.uia.sysbios"), fxn);
        // struct Adaptor.Entry
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.Adaptor$$Entry", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.Adaptor.Entry", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("elem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.uia.sysbios"), $$DEFAULT, "w");
                po.addFld("packet", (Proto)om.findStrict("ti.uia.runtime.UIAPacket.Hdr", "ti.uia.sysbios"), $$DEFAULT, "w");
        // struct Adaptor.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.Adaptor$$Module_State", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.Adaptor.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("event", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("clock", (Proto)om.findStrict("ti.sysbios.knl.Clock.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("freeEventSem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("freeMsgSem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("incomingSem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("freeEventQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("freeMsgQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("incomingQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("outgoingQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("syncEvent01", (Proto)om.findStrict("ti.sysbios.syncs.SyncEvent.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("transferAgentHandle", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.uia.sysbios"), $$UNDEF, "w");
                po.addFld("transportMsgHandle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("transportEventHandle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("numMsgPacketsSent", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numMsgPacketsFailed", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numEventPacketsSent", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numEventPacketsFailed", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("period", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false), $$DEFAULT, "w");
                po.addFld("scheduled", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false), $$DEFAULT, "w");
                po.addFld("reqEnergy", new Proto.Arr($$T_Bool, false), $$DEFAULT, "w");
    }

    void LoggingSetup$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/sysbios/LoggingSetup.xs");
        om.bind("ti.uia.sysbios.LoggingSetup$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggingSetup.Module", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggingSetup.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("enableTaskProfiler", $$T_Bool, false, "wh");
        po.addFld("enableContextAwareFunctionProfiler", $$T_Bool, false, "wh");
        po.addFld("memorySectionName", $$T_Str, null, "wh");
        po.addFld("numCores", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("cpuId", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        po.addFld("timestampSize", (Proto)om.findStrict("ti.uia.sysbios.LoggingSetup.TimestampSize", "ti.uia.sysbios"), om.find("ti.uia.sysbios.LoggingSetup.TimestampSize_AUTO"), "wh");
        po.addFld("loadLogging", $$T_Bool, true, "wh");
        po.addFld("loadTaskLogging", $$T_Bool, false, "wh");
        po.addFld("loadSwiLogging", $$T_Bool, false, "wh");
        po.addFld("loadHwiLogging", $$T_Bool, false, "wh");
        po.addFld("loadLogger", (Proto)om.findStrict("xdc.runtime.ILogger.Handle", "ti.uia.sysbios"), null, "wh");
        po.addFld("loadLoggerSize", Proto.Elm.newCNum("(xdc_SizeT)"), 512L, "wh");
        po.addFld("loadLoggingRuntimeControl", $$T_Bool, false, "wh");
        po.addFld("mainLogging", $$T_Bool, true, "wh");
        po.addFld("mainLogger", (Proto)om.findStrict("xdc.runtime.ILogger.Handle", "ti.uia.sysbios"), null, "wh");
        po.addFld("mainLoggerSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1024L, "wh");
        po.addFld("mainLoggingRuntimeControl", $$T_Bool, true, "wh");
        po.addFld("sysbiosLogger", (Proto)om.findStrict("xdc.runtime.ILogger.Handle", "ti.uia.sysbios"), null, "wh");
        po.addFld("sysbiosLoggerSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1024L, "wh");
        po.addFld("sysbiosTaskLogging", $$T_Bool, true, "wh");
        po.addFld("sysbiosTaskLoggingRuntimeControl", $$T_Bool, true, "wh");
        po.addFld("sysbiosSwiLogging", $$T_Bool, false, "wh");
        po.addFld("sysbiosSwiLoggingRuntimeControl", $$T_Bool, false, "wh");
        po.addFld("sysbiosHwiLogging", $$T_Bool, false, "wh");
        po.addFld("sysbiosHwiLoggingRuntimeControl", $$T_Bool, false, "wh");
        po.addFld("sysbiosSemaphoreLogging", $$T_Bool, false, "wh");
        po.addFld("sysbiosSemaphoreLoggingRuntimeControl", $$T_Bool, false, "wh");
        po.addFld("profileLogging", $$T_Bool, false, "wh");
        po.addFld("countingAndGraphingLogging", $$T_Bool, false, "wh");
        po.addFld("benchmarkLogging", $$T_Bool, false, "wh");
        po.addFld("snapshotLogging", $$T_Bool, false, "wh");
        po.addFld("overflowLoggerSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1024L, "wh");
        po.addFld("eventUploadMode", (Proto)om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"), om.find("ti.uia.sysbios.LoggingSetup.UploadMode_JTAGSTOPMODE"), "wh");
        po.addFld("loggerType", (Proto)om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType", "ti.uia.sysbios"), om.find("ti.uia.sysbios.LoggingSetup.LoggerType_STOPMODE"), "wh");
        po.addFld("multicoreEventCorrelation", $$T_Bool, false, "wh");
        po.addFld("disableMulticoreEventCorrelation", $$T_Bool, $$UNDEF, "wh");
        po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), 128L, "wh");
        po.addFld("syncLoggerSize", Proto.Elm.newCNum("(xdc_SizeT)"), 256L, "wh");
        po.addFld("showMulticoreEventCorrelationOption", $$T_Bool, false, "wh");
        po.addFld("isSysbiosLoggerAutoCreated", $$T_Bool, true, "wh");
        po.addFld("isLoadLoggerAutoCreated", $$T_Bool, true, "wh");
        po.addFld("isMainLoggerAutoCreated", $$T_Bool, true, "wh");
        po.addFld("showNumCoresOption", $$T_Bool, false, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggingSetup$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggingSetup$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggingSetup$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("createLogger", (Proto.Fxn)om.findStrict("ti.uia.sysbios.LoggingSetup$$createLogger", "ti.uia.sysbios"), Global.get(cap, "createLogger"));
                po.addFxn("writeUIAMetaData", (Proto.Fxn)om.findStrict("ti.uia.sysbios.LoggingSetup$$writeUIAMetaData", "ti.uia.sysbios"), Global.get(cap, "writeUIAMetaData"));
    }

    void LoggerStreamer$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/sysbios/LoggerStreamer.xs");
        om.bind("ti.uia.sysbios.LoggerStreamer$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer.Module", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer.Module", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Module", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("WRITE0_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("WRITE1_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 12L, "rh");
                po.addFld("WRITE2_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
                po.addFld("WRITE4_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 24L, "rh");
                po.addFld("WRITE8_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 40L, "rh");
                po.addFld("TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("NO_TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 0L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.sysbios"), $$UNDEF, "wh");
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1400L, "w");
            po.addFld("transportType", (Proto)om.findStrict("ti.uia.sysbios.LoggerStreamer.TransportType", "ti.uia.sysbios"), om.find("ti.uia.sysbios.LoggerStreamer.TransportType_ETHERNET"), "wh");
            po.addFld("customTransportType", $$T_Str, null, "w");
            po.addFld("isTimestampEnabled", $$T_Bool, false, "w");
            po.addFld("isBadPacketDetectionEnabled", $$T_Bool, false, "w");
            po.addFld("supportLoggerDisable", $$T_Bool, false, "w");
            po.addFld("testForNullWrPtr", $$T_Bool, true, "w");
            po.addFld("primeFxn", new Proto.Adr("xdc_Ptr(*)(xdc_Void)", "PFPv"), null, "w");
            po.addFld("exchangeFxn", new Proto.Adr("xdc_Ptr(*)(xdc_Ptr)", "PFPv"), null, "w");
            po.addFld("statusLogger", (Proto)om.findStrict("xdc.runtime.IFilterLogger.Handle", "ti.uia.sysbios"), null, "w");
            po.addFld("level1Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level2Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level3Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level4Mask", Proto.Elm.newCNum("(xdc_Bits16)"), Global.eval("0xFF9F & (~0x0010) & (~0x0008)"), "w");
            po.addFld("moduleToRouteToStatusLogger", $$T_Str, $$UNDEF, "wh");
            po.addFld("L_test", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.sysbios"), Global.newObject("mask", 0x0100L, "msg", "Test"), "w");
            po.addFld("E_badLevel", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.uia.sysbios"), Global.newObject("msg", "E_badLevel: Bad filter level value: %d"), "w");
            po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), 512L, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.uia.sysbios.LoggerStreamer$$create", "ti.uia.sysbios"), Global.get("ti$uia$sysbios$LoggerStreamer$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.uia.sysbios.LoggerStreamer$$construct", "ti.uia.sysbios"), Global.get("ti$uia$sysbios$LoggerStreamer$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getMetaArgs");
                if (fxn != null) po.addFxn("getMetaArgs", (Proto.Fxn)om.findStrict("xdc.runtime.ILogger$$getMetaArgs", "ti.uia.sysbios"), fxn);
                fxn = Global.get(cap, "getPtrToQueueDescriptorMeta");
                if (fxn != null) po.addFxn("getPtrToQueueDescriptorMeta", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getPtrToQueueDescriptorMeta", "ti.uia.sysbios"), fxn);
                fxn = Global.get(cap, "setPtrToQueueDescriptorMeta");
                if (fxn != null) po.addFxn("setPtrToQueueDescriptorMeta", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$setPtrToQueueDescriptorMeta", "ti.uia.sysbios"), fxn);
                fxn = Global.get(cap, "getLoggerInstanceId");
                if (fxn != null) po.addFxn("getLoggerInstanceId", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getLoggerInstanceId", "ti.uia.sysbios"), fxn);
                fxn = Global.get(cap, "getLoggerPriority");
                if (fxn != null) po.addFxn("getLoggerPriority", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getLoggerPriority", "ti.uia.sysbios"), fxn);
                fxn = Global.get(cap, "setLoggerPriority");
                if (fxn != null) po.addFxn("setLoggerPriority", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$setLoggerPriority", "ti.uia.sysbios"), fxn);
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer.Instance", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer.Instance", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Instance", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("WRITE0_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("WRITE1_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 12L, "rh");
                po.addFld("WRITE2_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
                po.addFld("WRITE4_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 24L, "rh");
                po.addFld("WRITE8_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 40L, "rh");
                po.addFld("TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("NO_TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 0L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.sysbios"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer$$Params", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer.Params", om.findStrict("ti.uia.runtime.ILoggerSnapshot$$Params", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("WRITE0_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("WRITE1_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 12L, "rh");
                po.addFld("WRITE2_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
                po.addFld("WRITE4_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 24L, "rh");
                po.addFld("WRITE8_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 40L, "rh");
                po.addFld("TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("NO_TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 0L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.sysbios"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer$$Object", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer.Object", om.findStrict("ti.uia.sysbios.LoggerStreamer.Instance", "ti.uia.sysbios"));
        // struct LoggerStreamer.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer$$ModuleView", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("isEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("isTimestampEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("bufferSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("primeFunc", $$T_Str, $$UNDEF, "w");
                po.addFld("exchangeFunc", $$T_Str, $$UNDEF, "w");
                po.addFld("transportType", $$T_Str, $$UNDEF, "w");
                po.addFld("customTransport", $$T_Str, $$UNDEF, "w");
        // struct LoggerStreamer.RecordView
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer$$RecordView", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer.RecordView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("sequence", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("timestampRaw", Proto.Elm.newCNum("(xdc_Long)"), $$UNDEF, "w");
                po.addFld("modName", $$T_Str, $$UNDEF, "w");
                po.addFld("text", $$T_Str, $$UNDEF, "w");
                po.addFld("eventId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("eventName", $$T_Str, $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg2", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg3", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg4", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg5", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg6", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg7", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
        // typedef LoggerStreamer.ExchangeFxnType
        om.bind("ti.uia.sysbios.LoggerStreamer.ExchangeFxnType", new Proto.Adr("xdc_Ptr(*)(xdc_Ptr)", "PFPv"));
        // typedef LoggerStreamer.PrimeFxnType
        om.bind("ti.uia.sysbios.LoggerStreamer.PrimeFxnType", new Proto.Adr("xdc_Ptr(*)(xdc_Void)", "PFPv"));
        // struct LoggerStreamer.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer$$Module_State", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("buffer", new Proto.Adr("xdc_UInt32*", "Pn"), $$UNDEF, "w");
                po.addFld("write", new Proto.Adr("xdc_UInt32*", "Pn"), $$UNDEF, "w");
                po.addFld("end", new Proto.Adr("xdc_UInt32*", "Pn"), $$UNDEF, "w");
                po.addFld("level1", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("level2", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("level3", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("moduleIdToRouteToStatusLogger", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("maxEventSizeInBits32", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("droppedEvents", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // struct LoggerStreamer.Instance_State
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer$$Instance_State", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void LoggerStreamer2$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/sysbios/LoggerStreamer2.xs");
        om.bind("ti.uia.sysbios.LoggerStreamer2$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2.Module", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer2.Module", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Module", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("WRITE0_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("WRITE1_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 12L, "rh");
                po.addFld("WRITE2_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
                po.addFld("WRITE4_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 24L, "rh");
                po.addFld("WRITE8_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 40L, "rh");
                po.addFld("TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("NO_TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 0L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.sysbios"), $$UNDEF, "wh");
            po.addFld("transportType", (Proto)om.findStrict("ti.uia.sysbios.LoggerStreamer2.TransportType", "ti.uia.sysbios"), om.find("ti.uia.sysbios.LoggerStreamer2.TransportType_ETHERNET"), "wh");
            po.addFld("customTransportType", $$T_Str, null, "w");
            po.addFld("isTimestampEnabled", $$T_Bool, false, "w");
            po.addFld("isBadPacketDetectionEnabled", $$T_Bool, false, "w");
            po.addFld("supportLoggerDisable", $$T_Bool, false, "w");
            po.addFld("testForNullWrPtr", $$T_Bool, true, "w");
            po.addFld("statusLogger", (Proto)om.findStrict("xdc.runtime.IFilterLogger.Handle", "ti.uia.sysbios"), null, "w");
            po.addFld("level1Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level2Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level3Mask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("level4Mask", Proto.Elm.newCNum("(xdc_Bits16)"), Global.eval("0xFF9F & (~0x0010) & (~0x0008)"), "w");
            po.addFld("moduleToRouteToStatusLogger", $$T_Str, $$UNDEF, "wh");
            po.addFld("L_test", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.sysbios"), Global.newObject("mask", 0x0100L, "msg", "Test"), "w");
            po.addFld("E_badLevel", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.uia.sysbios"), Global.newObject("msg", "E_badLevel: Bad filter level value: %d"), "w");
            po.addFld("A_invalidBuffer", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.uia.sysbios"), Global.newObject("msg", "LoggerStreamer2_create's buffer returned by primeFxn is NULL"), "w");
            po.addFld("maxId", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$create", "ti.uia.sysbios"), Global.get("ti$uia$sysbios$LoggerStreamer2$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$construct", "ti.uia.sysbios"), Global.get("ti$uia$sysbios$LoggerStreamer2$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer2$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer2$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer2$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer2$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer2$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerStreamer2$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getMetaArgs");
                if (fxn != null) po.addFxn("getMetaArgs", (Proto.Fxn)om.findStrict("xdc.runtime.ILogger$$getMetaArgs", "ti.uia.sysbios"), fxn);
                fxn = Global.get(cap, "getPtrToQueueDescriptorMeta");
                if (fxn != null) po.addFxn("getPtrToQueueDescriptorMeta", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getPtrToQueueDescriptorMeta", "ti.uia.sysbios"), fxn);
                fxn = Global.get(cap, "setPtrToQueueDescriptorMeta");
                if (fxn != null) po.addFxn("setPtrToQueueDescriptorMeta", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$setPtrToQueueDescriptorMeta", "ti.uia.sysbios"), fxn);
                fxn = Global.get(cap, "getLoggerInstanceId");
                if (fxn != null) po.addFxn("getLoggerInstanceId", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getLoggerInstanceId", "ti.uia.sysbios"), fxn);
                fxn = Global.get(cap, "getLoggerPriority");
                if (fxn != null) po.addFxn("getLoggerPriority", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$getLoggerPriority", "ti.uia.sysbios"), fxn);
                fxn = Global.get(cap, "setLoggerPriority");
                if (fxn != null) po.addFxn("setLoggerPriority", (Proto.Fxn)om.findStrict("ti.uia.runtime.IUIATransfer$$setLoggerPriority", "ti.uia.sysbios"), fxn);
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2.Instance", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer2.Instance", om.findStrict("ti.uia.runtime.ILoggerSnapshot.Instance", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("WRITE0_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("WRITE1_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 12L, "rh");
                po.addFld("WRITE2_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
                po.addFld("WRITE4_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 24L, "rh");
                po.addFld("WRITE8_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 40L, "rh");
                po.addFld("TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("NO_TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 0L, "rh");
        if (isCFG) {
            po.addFld("context", new Proto.Adr("xdc_UArg", "Pv"), null, "w");
            po.addFld("primeFxn", new Proto.Adr("xdc_Ptr(*)(ti_uia_sysbios_LoggerStreamer2_Object*)", "PFPv"), null, "w");
            po.addFld("exchangeFxn", new Proto.Adr("xdc_Ptr(*)(ti_uia_sysbios_LoggerStreamer2_Object*,xdc_Ptr)", "PFPv"), null, "w");
            po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int16)"), 1L, "w");
            po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), 512L, "w");
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1400L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.sysbios"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$Params", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer2.Params", om.findStrict("ti.uia.runtime.ILoggerSnapshot$$Params", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("WRITE0_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("WRITE1_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 12L, "rh");
                po.addFld("WRITE2_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
                po.addFld("WRITE4_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 24L, "rh");
                po.addFld("WRITE8_SIZE_IN_BYTES", Proto.Elm.newCNum("(xdc_Int)"), 40L, "rh");
                po.addFld("TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 8L, "rh");
                po.addFld("NO_TIMESTAMP", Proto.Elm.newCNum("(xdc_Int)"), 0L, "rh");
        if (isCFG) {
            po.addFld("context", new Proto.Adr("xdc_UArg", "Pv"), null, "w");
            po.addFld("primeFxn", new Proto.Adr("xdc_Ptr(*)(ti_uia_sysbios_LoggerStreamer2_Object*)", "PFPv"), null, "w");
            po.addFld("exchangeFxn", new Proto.Adr("xdc_Ptr(*)(ti_uia_sysbios_LoggerStreamer2_Object*,xdc_Ptr)", "PFPv"), null, "w");
            po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int16)"), 1L, "w");
            po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), 512L, "w");
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1400L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.sysbios"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$Object", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer2.Object", om.findStrict("ti.uia.sysbios.LoggerStreamer2.Instance", "ti.uia.sysbios"));
        // struct LoggerStreamer2.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$ModuleView", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer2.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("isEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("isTimestampEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("transportType", $$T_Str, $$UNDEF, "w");
                po.addFld("customTransport", $$T_Str, $$UNDEF, "w");
        // struct LoggerStreamer2.InstanceView
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$InstanceView", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer2.InstanceView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("bufferSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("primeFunc", $$T_Str, $$UNDEF, "w");
                po.addFld("exchangeFunc", $$T_Str, $$UNDEF, "w");
                po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("context", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
        // struct LoggerStreamer2.RecordView
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$RecordView", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer2.RecordView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("sequence", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("timestampRaw", Proto.Elm.newCNum("(xdc_Long)"), $$UNDEF, "w");
                po.addFld("modName", $$T_Str, $$UNDEF, "w");
                po.addFld("text", $$T_Str, $$UNDEF, "w");
                po.addFld("eventId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("eventName", $$T_Str, $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg2", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg3", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg4", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg5", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg6", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg7", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
        // struct LoggerStreamer2.RtaData
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$RtaData", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer2.RtaData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // typedef LoggerStreamer2.ExchangeFxnType
        om.bind("ti.uia.sysbios.LoggerStreamer2.ExchangeFxnType", new Proto.Adr("xdc_Ptr(*)(ti_uia_sysbios_LoggerStreamer2_Object*,xdc_Ptr)", "PFPv"));
        // typedef LoggerStreamer2.PrimeFxnType
        om.bind("ti.uia.sysbios.LoggerStreamer2.PrimeFxnType", new Proto.Adr("xdc_Ptr(*)(ti_uia_sysbios_LoggerStreamer2_Object*)", "PFPv"));
        // struct LoggerStreamer2.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$Module_State", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer2.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("level1", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("level2", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("level3", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("moduleIdToRouteToStatusLogger", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
        // struct LoggerStreamer2.Instance_State
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$Instance_State", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerStreamer2.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int16)"), $$UNDEF, "w");
                po.addFld("primeFxn", new Proto.Adr("xdc_Ptr(*)(ti_uia_sysbios_LoggerStreamer2_Object*)", "PFPv"), $$UNDEF, "w");
                po.addFld("exchangeFxn", new Proto.Adr("xdc_Ptr(*)(ti_uia_sysbios_LoggerStreamer2_Object*,xdc_Ptr)", "PFPv"), $$UNDEF, "w");
                po.addFld("context", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("primeStatus", $$T_Bool, $$UNDEF, "w");
                po.addFld("bufSize", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("buffer", new Proto.Adr("xdc_UArg*", "PPv"), $$UNDEF, "w");
                po.addFld("write", new Proto.Adr("xdc_UArg*", "PPv"), $$UNDEF, "w");
                po.addFld("end", new Proto.Adr("xdc_UArg*", "PPv"), $$UNDEF, "w");
                po.addFld("maxEventSizeUArg", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("maxEventSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("droppedEvents", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("seqNumber", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
    }

    void LoggerIdle$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/sysbios/LoggerIdle.xs");
        om.bind("ti.uia.sysbios.LoggerIdle$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle.Module", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerIdle.Module", om.findStrict("xdc.runtime.ILogger.Module", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.sysbios"), $$UNDEF, "wh");
            po.addFld("bufferSize", Proto.Elm.newCNum("(xdc_SizeT)"), 256L, "w");
            po.addFld("isTimestampEnabled", $$T_Bool, true, "w");
            po.addFld("transportType", (Proto)om.findStrict("ti.uia.sysbios.LoggerIdle.TransportType", "ti.uia.sysbios"), om.find("ti.uia.sysbios.LoggerIdle.TransportType_UART"), "wh");
            po.addFld("customTransportType", $$T_Str, null, "w");
            po.addFld("transportFxn", new Proto.Adr("xdc_Int(*)(xdc_UChar*,xdc_Int)", "PFn"), null, "w");
            po.addFld("writeWhenFull", $$T_Bool, false, "w");
            po.addFld("L_test", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.sysbios"), Global.newObject("mask", 0x0100L, "msg", "Test Event"), "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.uia.sysbios.LoggerIdle$$create", "ti.uia.sysbios"), Global.get("ti$uia$sysbios$LoggerIdle$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.uia.sysbios.LoggerIdle$$construct", "ti.uia.sysbios"), Global.get("ti$uia$sysbios$LoggerIdle$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerIdle$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerIdle$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerIdle$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerIdle$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerIdle$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.uia.sysbios.LoggerIdle$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getMetaArgs");
                if (fxn != null) po.addFxn("getMetaArgs", (Proto.Fxn)om.findStrict("xdc.runtime.ILogger$$getMetaArgs", "ti.uia.sysbios"), fxn);
                po.addFxn("initDecoder", (Proto.Fxn)om.findStrict("ti.uia.sysbios.LoggerIdle$$initDecoder", "ti.uia.sysbios"), Global.get(cap, "initDecoder"));
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle.Instance", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerIdle.Instance", om.findStrict("xdc.runtime.ILogger.Instance", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.sysbios"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle$$Params", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerIdle.Params", om.findStrict("xdc.runtime.ILogger$$Params", "ti.uia.sysbios"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.uia.sysbios"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle$$Object", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerIdle.Object", om.findStrict("ti.uia.sysbios.LoggerIdle.Instance", "ti.uia.sysbios"));
        // struct LoggerIdle.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle$$ModuleView", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerIdle.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("isEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("isTimestampEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("bufferSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("sequenceNumber", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("transportType", $$T_Str, $$UNDEF, "w");
                po.addFld("customTransport", $$T_Str, $$UNDEF, "w");
        // struct LoggerIdle.RecordView
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle$$RecordView", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerIdle.RecordView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("sequence", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("timestampRaw", Proto.Elm.newCNum("(xdc_Long)"), $$UNDEF, "w");
                po.addFld("modName", $$T_Str, $$UNDEF, "w");
                po.addFld("text", $$T_Str, $$UNDEF, "w");
                po.addFld("eventId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("eventName", $$T_Str, $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg2", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg3", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg4", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg5", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg6", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg7", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
        // struct LoggerIdle.RtaData
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle$$RtaData", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerIdle.RtaData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("instanceId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // typedef LoggerIdle.LoggerFxn
        om.bind("ti.uia.sysbios.LoggerIdle.LoggerFxn", new Proto.Adr("xdc_Int(*)(xdc_UChar*,xdc_Int)", "PFn"));
        // struct LoggerIdle.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle$$Module_State", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerIdle.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("loggerFxn", new Proto.Adr("xdc_Int(*)(xdc_UChar*,xdc_Int)", "PFn"), $$UNDEF, "w");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("empty", $$T_Bool, $$UNDEF, "w");
                po.addFld("bufferSize", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("idleBuffer", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false), $$DEFAULT, "w");
                po.addFld("tempBuffer", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false), $$DEFAULT, "w");
                po.addFld("bufferRead", new Proto.Adr("xdc_UInt32*", "Pn"), $$UNDEF, "w");
                po.addFld("bufferWrite", new Proto.Adr("xdc_UInt32*", "Pn"), $$UNDEF, "w");
                po.addFld("bufferPad", new Proto.Adr("xdc_UInt32*", "Pn"), $$UNDEF, "w");
                po.addFld("bufferEnd", new Proto.Adr("xdc_UInt32*", "Pn"), $$UNDEF, "w");
                po.addFld("eventSequenceNum", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
        // struct LoggerIdle.Instance_State
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle$$Instance_State", "ti.uia.sysbios");
        po.init("ti.uia.sysbios.LoggerIdle.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void IpcMP$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.IpcMP", "ti.uia.sysbios");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.sysbios.IpcMP.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.IpcMP$$Module_State", "ti.uia.sysbios");
        po.bind("replyMQ$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_UInt32", "isScalar", true));
        po.bind("period$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_UInt32", "isScalar", true));
        po.bind("scheduled$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_UInt32", "isScalar", true));
        po.bind("reqEnergy$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Bool", "isScalar", true));
    }

    void Adaptor$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.Adaptor", "ti.uia.sysbios");
        vo.bind("Entry$fetchDesc", Global.newObject("type", "ti.uia.sysbios.Adaptor.Entry", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.Adaptor$$Entry", "ti.uia.sysbios");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.sysbios.Adaptor.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.Adaptor$$Module_State", "ti.uia.sysbios");
        po.bind("period$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_UInt32", "isScalar", true));
        po.bind("scheduled$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_UInt32", "isScalar", true));
        po.bind("reqEnergy$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Bool", "isScalar", true));
    }

    void LoggingSetup$$ROV()
    {
    }

    void LoggerStreamer$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer", "ti.uia.sysbios");
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer$$Instance_State", "ti.uia.sysbios");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.sysbios.LoggerStreamer.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer$$Module_State", "ti.uia.sysbios");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.uia.sysbios.LoggerStreamer.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer$$Instance_State", "ti.uia.sysbios");
    }

    void LoggerStreamer2$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2", "ti.uia.sysbios");
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$Instance_State", "ti.uia.sysbios");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.sysbios.LoggerStreamer2.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$Module_State", "ti.uia.sysbios");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.uia.sysbios.LoggerStreamer2.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2$$Instance_State", "ti.uia.sysbios");
    }

    void LoggerIdle$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle", "ti.uia.sysbios");
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle$$Instance_State", "ti.uia.sysbios");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.sysbios.LoggerIdle.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle$$Module_State", "ti.uia.sysbios");
        po.bind("idleBuffer$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_UInt32", "isScalar", true));
        po.bind("tempBuffer$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_UInt32", "isScalar", true));
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.uia.sysbios.LoggerIdle.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle$$Instance_State", "ti.uia.sysbios");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.uia.sysbios.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.uia.sysbios"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.uia.sysbios", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.uia.sysbios");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.uia.sysbios.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.rov", Global.newArray()));
        imports.add(Global.newArray("xdc.rta", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.uia.sysbios'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.uia.sysbios$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.uia.sysbios$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.uia.sysbios$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/release/ti.uia.sysbios.aem4',\n");
            sb.append("'lib/release/ti.uia.sysbios.am4',\n");
            sb.append("'lib/release/ti.uia.sysbios.am4g',\n");
            sb.append("'lib/release/ti.uia.sysbios.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/release/ti.uia.sysbios.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/release/ti.uia.sysbios.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/release/ti.uia.sysbios.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/release/ti.uia.sysbios.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void IpcMP$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.IpcMP", "ti.uia.sysbios");
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.IpcMP.Module", "ti.uia.sysbios");
        vo.init2(po, "ti.uia.sysbios.IpcMP", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.sysbios.IpcMP$$capsule", "ti.uia.sysbios"));
        vo.bind("$package", om.findStrict("ti.uia.sysbios", "ti.uia.sysbios"));
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
        mcfgs.add("A_IpcMPFailure");
        mcfgs.add("sharedRegionId");
        mcfgs.add("createRxTask");
        icfgs.add("createRxTask");
        vo.bind("Module_State", om.findStrict("ti.uia.sysbios.IpcMP.Module_State", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.IpcMP.Module_State", "ti.uia.sysbios"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.sysbios")).add(vo);
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
        vo.bind("freePacket", om.findStrict("ti.uia.sysbios.IpcMP.freePacket", "ti.uia.sysbios"));
        vo.bind("getFreePacket", om.findStrict("ti.uia.sysbios.IpcMP.getFreePacket", "ti.uia.sysbios"));
        vo.bind("requestEnergy", om.findStrict("ti.uia.sysbios.IpcMP.requestEnergy", "ti.uia.sysbios"));
        vo.bind("sendPacket", om.findStrict("ti.uia.sysbios.IpcMP.sendPacket", "ti.uia.sysbios"));
        vo.bind("setPeriod", om.findStrict("ti.uia.sysbios.IpcMP.setPeriod", "ti.uia.sysbios"));
        vo.bind("rxTaskFxn", om.findStrict("ti.uia.sysbios.IpcMP.rxTaskFxn", "ti.uia.sysbios"));
        vo.bind("transferAgentFxn", om.findStrict("ti.uia.sysbios.IpcMP.transferAgentFxn", "ti.uia.sysbios"));
        vo.bind("clockFxn", om.findStrict("ti.uia.sysbios.IpcMP.clockFxn", "ti.uia.sysbios"));
        vo.bind("start", om.findStrict("ti.uia.sysbios.IpcMP.start", "ti.uia.sysbios"));
        vo.bind("startOrig", om.findStrict("ti.uia.sysbios.IpcMP.startOrig", "ti.uia.sysbios"));
        vo.bind("giveEnergy", om.findStrict("ti.uia.sysbios.IpcMP.giveEnergy", "ti.uia.sysbios"));
        vo.bind("handleMsg", om.findStrict("ti.uia.sysbios.IpcMP.handleMsg", "ti.uia.sysbios"));
        vo.bind("prime", om.findStrict("ti.uia.sysbios.IpcMP.prime", "ti.uia.sysbios"));
        vo.bind("registerWithMaster", om.findStrict("ti.uia.sysbios.IpcMP.registerWithMaster", "ti.uia.sysbios"));
        vo.bind("requestEvents", om.findStrict("ti.uia.sysbios.IpcMP.requestEvents", "ti.uia.sysbios"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_sysbios_IpcMP_Module__startupDone__E", "ti_uia_sysbios_IpcMP_freePacket__E", "ti_uia_sysbios_IpcMP_getFreePacket__E", "ti_uia_sysbios_IpcMP_requestEnergy__E", "ti_uia_sysbios_IpcMP_sendPacket__E", "ti_uia_sysbios_IpcMP_setPeriod__E", "ti_uia_sysbios_IpcMP_rxTaskFxn__E", "ti_uia_sysbios_IpcMP_transferAgentFxn__E", "ti_uia_sysbios_IpcMP_clockFxn__E", "ti_uia_sysbios_IpcMP_start__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_IpcMPFailure"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("IpcMP", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IpcMP");
    }

    void Adaptor$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.Adaptor", "ti.uia.sysbios");
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.Adaptor.Module", "ti.uia.sysbios");
        vo.init2(po, "ti.uia.sysbios.Adaptor", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.sysbios.Adaptor$$capsule", "ti.uia.sysbios"));
        vo.bind("$package", om.findStrict("ti.uia.sysbios", "ti.uia.sysbios"));
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
        vo.bind("Entry", om.findStrict("ti.uia.sysbios.Adaptor.Entry", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.Adaptor.Entry", "ti.uia.sysbios"));
        mcfgs.add("eventBuf");
        icfgs.add("eventBuf");
        mcfgs.add("msgBuf");
        icfgs.add("msgBuf");
        vo.bind("Module_State", om.findStrict("ti.uia.sysbios.Adaptor.Module_State", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.Adaptor.Module_State", "ti.uia.sysbios"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.sysbios")).add(vo);
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
        vo.bind("freePacket", om.findStrict("ti.uia.sysbios.Adaptor.freePacket", "ti.uia.sysbios"));
        vo.bind("getFreePacket", om.findStrict("ti.uia.sysbios.Adaptor.getFreePacket", "ti.uia.sysbios"));
        vo.bind("requestEnergy", om.findStrict("ti.uia.sysbios.Adaptor.requestEnergy", "ti.uia.sysbios"));
        vo.bind("sendPacket", om.findStrict("ti.uia.sysbios.Adaptor.sendPacket", "ti.uia.sysbios"));
        vo.bind("setPeriod", om.findStrict("ti.uia.sysbios.Adaptor.setPeriod", "ti.uia.sysbios"));
        vo.bind("rxTaskFxn", om.findStrict("ti.uia.sysbios.Adaptor.rxTaskFxn", "ti.uia.sysbios"));
        vo.bind("transferAgentTaskFxn", om.findStrict("ti.uia.sysbios.Adaptor.transferAgentTaskFxn", "ti.uia.sysbios"));
        vo.bind("clockFxn", om.findStrict("ti.uia.sysbios.Adaptor.clockFxn", "ti.uia.sysbios"));
        vo.bind("giveEnergy", om.findStrict("ti.uia.sysbios.Adaptor.giveEnergy", "ti.uia.sysbios"));
        vo.bind("sendToHost", om.findStrict("ti.uia.sysbios.Adaptor.sendToHost", "ti.uia.sysbios"));
        vo.bind("sendToService", om.findStrict("ti.uia.sysbios.Adaptor.sendToService", "ti.uia.sysbios"));
        vo.bind("runScheduledServices", om.findStrict("ti.uia.sysbios.Adaptor.runScheduledServices", "ti.uia.sysbios"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_sysbios_Adaptor_Module__startupDone__E", "ti_uia_sysbios_Adaptor_freePacket__E", "ti_uia_sysbios_Adaptor_getFreePacket__E", "ti_uia_sysbios_Adaptor_requestEnergy__E", "ti_uia_sysbios_Adaptor_sendPacket__E", "ti_uia_sysbios_Adaptor_setPeriod__E", "ti_uia_sysbios_Adaptor_rxTaskFxn__E", "ti_uia_sysbios_Adaptor_transferAgentTaskFxn__E", "ti_uia_sysbios_Adaptor_clockFxn__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Adaptor", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Adaptor");
    }

    void LoggingSetup$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.LoggingSetup", "ti.uia.sysbios");
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggingSetup.Module", "ti.uia.sysbios");
        vo.init2(po, "ti.uia.sysbios.LoggingSetup", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.sysbios.LoggingSetup$$capsule", "ti.uia.sysbios"));
        vo.bind("$package", om.findStrict("ti.uia.sysbios", "ti.uia.sysbios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("TimestampSize", om.findStrict("ti.uia.sysbios.LoggingSetup.TimestampSize", "ti.uia.sysbios"));
        vo.bind("UploadMode", om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode", "ti.uia.sysbios"));
        vo.bind("LoggerType", om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType", "ti.uia.sysbios"));
        vo.bind("TimestampSize_AUTO", om.findStrict("ti.uia.sysbios.LoggingSetup.TimestampSize_AUTO", "ti.uia.sysbios"));
        vo.bind("TimestampSize_NONE", om.findStrict("ti.uia.sysbios.LoggingSetup.TimestampSize_NONE", "ti.uia.sysbios"));
        vo.bind("TimestampSize_32b", om.findStrict("ti.uia.sysbios.LoggingSetup.TimestampSize_32b", "ti.uia.sysbios"));
        vo.bind("TimestampSize_64b", om.findStrict("ti.uia.sysbios.LoggingSetup.TimestampSize_64b", "ti.uia.sysbios"));
        vo.bind("UploadMode_SIMULATOR", om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode_SIMULATOR", "ti.uia.sysbios"));
        vo.bind("UploadMode_PROBEPOINT", om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode_PROBEPOINT", "ti.uia.sysbios"));
        vo.bind("UploadMode_JTAGSTOPMODE", om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode_JTAGSTOPMODE", "ti.uia.sysbios"));
        vo.bind("UploadMode_JTAGRUNMODE", om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode_JTAGRUNMODE", "ti.uia.sysbios"));
        vo.bind("UploadMode_NONJTAGTRANSPORT", om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode_NONJTAGTRANSPORT", "ti.uia.sysbios"));
        vo.bind("UploadMode_NONJTAG_AND_JTAGSTOPMODE", om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode_NONJTAG_AND_JTAGSTOPMODE", "ti.uia.sysbios"));
        vo.bind("UploadMode_STREAMER", om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode_STREAMER", "ti.uia.sysbios"));
        vo.bind("UploadMode_IDLE", om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode_IDLE", "ti.uia.sysbios"));
        vo.bind("UploadMode_STREAMER2", om.findStrict("ti.uia.sysbios.LoggingSetup.UploadMode_STREAMER2", "ti.uia.sysbios"));
        vo.bind("LoggerType_MIN", om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType_MIN", "ti.uia.sysbios"));
        vo.bind("LoggerType_STOPMODE", om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType_STOPMODE", "ti.uia.sysbios"));
        vo.bind("LoggerType_JTAGRUNMODE", om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType_JTAGRUNMODE", "ti.uia.sysbios"));
        vo.bind("LoggerType_RUNMODE", om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType_RUNMODE", "ti.uia.sysbios"));
        vo.bind("LoggerType_IDLE", om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType_IDLE", "ti.uia.sysbios"));
        vo.bind("LoggerType_STREAMER2", om.findStrict("ti.uia.sysbios.LoggingSetup.LoggerType_STREAMER2", "ti.uia.sysbios"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.sysbios")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.uia.sysbios.LoggingSetup$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("LoggingSetup", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggingSetup");
    }

    void LoggerStreamer$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer", "ti.uia.sysbios");
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer.Module", "ti.uia.sysbios");
        vo.init2(po, "ti.uia.sysbios.LoggerStreamer", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.sysbios.LoggerStreamer$$capsule", "ti.uia.sysbios"));
        vo.bind("Instance", om.findStrict("ti.uia.sysbios.LoggerStreamer.Instance", "ti.uia.sysbios"));
        vo.bind("Params", om.findStrict("ti.uia.sysbios.LoggerStreamer.Params", "ti.uia.sysbios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.sysbios.LoggerStreamer.Params", "ti.uia.sysbios")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.sysbios.LoggerStreamer.Handle", "ti.uia.sysbios"));
        vo.bind("$package", om.findStrict("ti.uia.sysbios", "ti.uia.sysbios"));
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
        vo.bind("TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.sysbios"));
        vo.bind("Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.sysbios"));
        vo.bind("MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.sysbios"));
        vo.bind("TransportType", om.findStrict("ti.uia.sysbios.LoggerStreamer.TransportType", "ti.uia.sysbios"));
        vo.bind("ModuleView", om.findStrict("ti.uia.sysbios.LoggerStreamer.ModuleView", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerStreamer.ModuleView", "ti.uia.sysbios"));
        vo.bind("RecordView", om.findStrict("ti.uia.sysbios.LoggerStreamer.RecordView", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerStreamer.RecordView", "ti.uia.sysbios"));
        vo.bind("ExchangeFxnType", om.findStrict("ti.uia.sysbios.LoggerStreamer.ExchangeFxnType", "ti.uia.sysbios"));
        vo.bind("PrimeFxnType", om.findStrict("ti.uia.sysbios.LoggerStreamer.PrimeFxnType", "ti.uia.sysbios"));
        mcfgs.add("bufSize");
        mcfgs.add("customTransportType");
        mcfgs.add("isTimestampEnabled");
        mcfgs.add("isBadPacketDetectionEnabled");
        mcfgs.add("supportLoggerDisable");
        mcfgs.add("testForNullWrPtr");
        mcfgs.add("primeFxn");
        mcfgs.add("exchangeFxn");
        mcfgs.add("statusLogger");
        mcfgs.add("level1Mask");
        mcfgs.add("level2Mask");
        mcfgs.add("level3Mask");
        mcfgs.add("level4Mask");
        mcfgs.add("L_test");
        mcfgs.add("E_badLevel");
        mcfgs.add("maxEventSize");
        vo.bind("Module_State", om.findStrict("ti.uia.sysbios.LoggerStreamer.Module_State", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerStreamer.Module_State", "ti.uia.sysbios"));
        vo.bind("Instance_State", om.findStrict("ti.uia.sysbios.LoggerStreamer.Instance_State", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerStreamer.Instance_State", "ti.uia.sysbios"));
        vo.bind("TransferType_RELIABLE", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_RELIABLE", "ti.uia.sysbios"));
        vo.bind("TransferType_LOSSY", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_LOSSY", "ti.uia.sysbios"));
        vo.bind("Priority_LOW", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_LOW", "ti.uia.sysbios"));
        vo.bind("Priority_STANDARD", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_STANDARD", "ti.uia.sysbios"));
        vo.bind("Priority_HIGH", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_HIGH", "ti.uia.sysbios"));
        vo.bind("Priority_SYNC", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_SYNC", "ti.uia.sysbios"));
        vo.bind("TransportType_UART", om.findStrict("ti.uia.sysbios.LoggerStreamer.TransportType_UART", "ti.uia.sysbios"));
        vo.bind("TransportType_USB", om.findStrict("ti.uia.sysbios.LoggerStreamer.TransportType_USB", "ti.uia.sysbios"));
        vo.bind("TransportType_ETHERNET", om.findStrict("ti.uia.sysbios.LoggerStreamer.TransportType_ETHERNET", "ti.uia.sysbios"));
        vo.bind("TransportType_CUSTOM", om.findStrict("ti.uia.sysbios.LoggerStreamer.TransportType_CUSTOM", "ti.uia.sysbios"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.runtime");
        inherits.add("ti.uia.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.sysbios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.uia.sysbios.LoggerStreamer$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$uia$sysbios$LoggerStreamer$$__initObject"));
        }//isCFG
        vo.bind("flush", om.findStrict("ti.uia.sysbios.LoggerStreamer.flush", "ti.uia.sysbios"));
        vo.bind("prime", om.findStrict("ti.uia.sysbios.LoggerStreamer.prime", "ti.uia.sysbios"));
        vo.bind("setModuleIdToRouteToStatusLogger", om.findStrict("ti.uia.sysbios.LoggerStreamer.setModuleIdToRouteToStatusLogger", "ti.uia.sysbios"));
        vo.bind("validatePacket", om.findStrict("ti.uia.sysbios.LoggerStreamer.validatePacket", "ti.uia.sysbios"));
        vo.bind("filterOutEvent", om.findStrict("ti.uia.sysbios.LoggerStreamer.filterOutEvent", "ti.uia.sysbios"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_sysbios_LoggerStreamer_Handle__label__E", "ti_uia_sysbios_LoggerStreamer_Module__startupDone__E", "ti_uia_sysbios_LoggerStreamer_Object__create__E", "ti_uia_sysbios_LoggerStreamer_Object__delete__E", "ti_uia_sysbios_LoggerStreamer_Object__get__E", "ti_uia_sysbios_LoggerStreamer_Object__first__E", "ti_uia_sysbios_LoggerStreamer_Object__next__E", "ti_uia_sysbios_LoggerStreamer_Params__init__E", "ti_uia_sysbios_LoggerStreamer_enable__E", "ti_uia_sysbios_LoggerStreamer_disable__E", "ti_uia_sysbios_LoggerStreamer_getTransferType__E", "ti_uia_sysbios_LoggerStreamer_getContents__E", "ti_uia_sysbios_LoggerStreamer_isEmpty__E", "ti_uia_sysbios_LoggerStreamer_getMaxLength__E", "ti_uia_sysbios_LoggerStreamer_getInstanceId__E", "ti_uia_sysbios_LoggerStreamer_getPriority__E", "ti_uia_sysbios_LoggerStreamer_setPriority__E", "ti_uia_sysbios_LoggerStreamer_reset__E", "ti_uia_sysbios_LoggerStreamer_writeMemoryRange__E", "ti_uia_sysbios_LoggerStreamer_initBuffer__E", "ti_uia_sysbios_LoggerStreamer_flush__E", "ti_uia_sysbios_LoggerStreamer_prime__E", "ti_uia_sysbios_LoggerStreamer_setModuleIdToRouteToStatusLogger__E", "ti_uia_sysbios_LoggerStreamer_validatePacket__E", "ti_uia_sysbios_LoggerStreamer_write0__E", "ti_uia_sysbios_LoggerStreamer_write1__E", "ti_uia_sysbios_LoggerStreamer_write2__E", "ti_uia_sysbios_LoggerStreamer_write4__E", "ti_uia_sysbios_LoggerStreamer_write8__E", "ti_uia_sysbios_LoggerStreamer_setFilterLevel__E", "ti_uia_sysbios_LoggerStreamer_getFilterLevel__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("L_test"));
        vo.bind("$$errorDescCfgs", Global.newArray("E_badLevel"));
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", "./LoggerStreamer.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.uia.sysbios.LoggerStreamer.Object", "ti.uia.sysbios"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./LoggerStreamer.xdt");
        pkgV.bind("LoggerStreamer", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerStreamer");
    }

    void LoggerStreamer2$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2", "ti.uia.sysbios");
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2.Module", "ti.uia.sysbios");
        vo.init2(po, "ti.uia.sysbios.LoggerStreamer2", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.sysbios.LoggerStreamer2$$capsule", "ti.uia.sysbios"));
        vo.bind("Instance", om.findStrict("ti.uia.sysbios.LoggerStreamer2.Instance", "ti.uia.sysbios"));
        vo.bind("Params", om.findStrict("ti.uia.sysbios.LoggerStreamer2.Params", "ti.uia.sysbios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.sysbios.LoggerStreamer2.Params", "ti.uia.sysbios")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.sysbios.LoggerStreamer2.Handle", "ti.uia.sysbios"));
        vo.bind("$package", om.findStrict("ti.uia.sysbios", "ti.uia.sysbios"));
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
        vo.bind("TransferType", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType", "ti.uia.sysbios"));
        vo.bind("Priority", om.findStrict("ti.uia.runtime.IUIATransfer.Priority", "ti.uia.sysbios"));
        vo.bind("MetaData", om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.runtime.IUIATransfer.MetaData", "ti.uia.sysbios"));
        vo.bind("TransportType", om.findStrict("ti.uia.sysbios.LoggerStreamer2.TransportType", "ti.uia.sysbios"));
        vo.bind("ModuleView", om.findStrict("ti.uia.sysbios.LoggerStreamer2.ModuleView", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerStreamer2.ModuleView", "ti.uia.sysbios"));
        vo.bind("InstanceView", om.findStrict("ti.uia.sysbios.LoggerStreamer2.InstanceView", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerStreamer2.InstanceView", "ti.uia.sysbios"));
        vo.bind("RecordView", om.findStrict("ti.uia.sysbios.LoggerStreamer2.RecordView", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerStreamer2.RecordView", "ti.uia.sysbios"));
        vo.bind("RtaData", om.findStrict("ti.uia.sysbios.LoggerStreamer2.RtaData", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerStreamer2.RtaData", "ti.uia.sysbios"));
        vo.bind("ExchangeFxnType", om.findStrict("ti.uia.sysbios.LoggerStreamer2.ExchangeFxnType", "ti.uia.sysbios"));
        vo.bind("PrimeFxnType", om.findStrict("ti.uia.sysbios.LoggerStreamer2.PrimeFxnType", "ti.uia.sysbios"));
        mcfgs.add("customTransportType");
        mcfgs.add("isTimestampEnabled");
        mcfgs.add("isBadPacketDetectionEnabled");
        mcfgs.add("supportLoggerDisable");
        mcfgs.add("testForNullWrPtr");
        mcfgs.add("statusLogger");
        mcfgs.add("level1Mask");
        mcfgs.add("level2Mask");
        mcfgs.add("level3Mask");
        mcfgs.add("level4Mask");
        mcfgs.add("L_test");
        mcfgs.add("E_badLevel");
        mcfgs.add("A_invalidBuffer");
        icfgs.add("maxId");
        vo.bind("Module_State", om.findStrict("ti.uia.sysbios.LoggerStreamer2.Module_State", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerStreamer2.Module_State", "ti.uia.sysbios"));
        vo.bind("Instance_State", om.findStrict("ti.uia.sysbios.LoggerStreamer2.Instance_State", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerStreamer2.Instance_State", "ti.uia.sysbios"));
        vo.bind("TransferType_RELIABLE", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_RELIABLE", "ti.uia.sysbios"));
        vo.bind("TransferType_LOSSY", om.findStrict("ti.uia.runtime.IUIATransfer.TransferType_LOSSY", "ti.uia.sysbios"));
        vo.bind("Priority_LOW", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_LOW", "ti.uia.sysbios"));
        vo.bind("Priority_STANDARD", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_STANDARD", "ti.uia.sysbios"));
        vo.bind("Priority_HIGH", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_HIGH", "ti.uia.sysbios"));
        vo.bind("Priority_SYNC", om.findStrict("ti.uia.runtime.IUIATransfer.Priority_SYNC", "ti.uia.sysbios"));
        vo.bind("TransportType_UART", om.findStrict("ti.uia.sysbios.LoggerStreamer2.TransportType_UART", "ti.uia.sysbios"));
        vo.bind("TransportType_USB", om.findStrict("ti.uia.sysbios.LoggerStreamer2.TransportType_USB", "ti.uia.sysbios"));
        vo.bind("TransportType_ETHERNET", om.findStrict("ti.uia.sysbios.LoggerStreamer2.TransportType_ETHERNET", "ti.uia.sysbios"));
        vo.bind("TransportType_CUSTOM", om.findStrict("ti.uia.sysbios.LoggerStreamer2.TransportType_CUSTOM", "ti.uia.sysbios"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.uia.runtime");
        inherits.add("ti.uia.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.sysbios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.uia.sysbios.LoggerStreamer2$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$uia$sysbios$LoggerStreamer2$$__initObject"));
        }//isCFG
        vo.bind("setModuleIdToRouteToStatusLogger", om.findStrict("ti.uia.sysbios.LoggerStreamer2.setModuleIdToRouteToStatusLogger", "ti.uia.sysbios"));
        vo.bind("filterOutEvent", om.findStrict("ti.uia.sysbios.LoggerStreamer2.filterOutEvent", "ti.uia.sysbios"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_sysbios_LoggerStreamer2_Handle__label__E", "ti_uia_sysbios_LoggerStreamer2_Module__startupDone__E", "ti_uia_sysbios_LoggerStreamer2_Object__create__E", "ti_uia_sysbios_LoggerStreamer2_Object__delete__E", "ti_uia_sysbios_LoggerStreamer2_Object__get__E", "ti_uia_sysbios_LoggerStreamer2_Object__first__E", "ti_uia_sysbios_LoggerStreamer2_Object__next__E", "ti_uia_sysbios_LoggerStreamer2_Params__init__E", "ti_uia_sysbios_LoggerStreamer2_enable__E", "ti_uia_sysbios_LoggerStreamer2_disable__E", "ti_uia_sysbios_LoggerStreamer2_getTransferType__E", "ti_uia_sysbios_LoggerStreamer2_getContents__E", "ti_uia_sysbios_LoggerStreamer2_isEmpty__E", "ti_uia_sysbios_LoggerStreamer2_getMaxLength__E", "ti_uia_sysbios_LoggerStreamer2_getInstanceId__E", "ti_uia_sysbios_LoggerStreamer2_getPriority__E", "ti_uia_sysbios_LoggerStreamer2_setPriority__E", "ti_uia_sysbios_LoggerStreamer2_reset__E", "ti_uia_sysbios_LoggerStreamer2_writeMemoryRange__E", "ti_uia_sysbios_LoggerStreamer2_setModuleIdToRouteToStatusLogger__E", "ti_uia_sysbios_LoggerStreamer2_initBuffer__E", "ti_uia_sysbios_LoggerStreamer2_flush__E", "ti_uia_sysbios_LoggerStreamer2_prime__E", "ti_uia_sysbios_LoggerStreamer2_validatePacket__E", "ti_uia_sysbios_LoggerStreamer2_write0__E", "ti_uia_sysbios_LoggerStreamer2_write1__E", "ti_uia_sysbios_LoggerStreamer2_write2__E", "ti_uia_sysbios_LoggerStreamer2_write4__E", "ti_uia_sysbios_LoggerStreamer2_write8__E", "ti_uia_sysbios_LoggerStreamer2_setFilterLevel__E", "ti_uia_sysbios_LoggerStreamer2_getFilterLevel__E", "ti_uia_sysbios_LoggerStreamer2_getBufSize__E", "ti_uia_sysbios_LoggerStreamer2_getContext__E", "ti_uia_sysbios_LoggerStreamer2_setContext__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("L_test"));
        vo.bind("$$errorDescCfgs", Global.newArray("E_badLevel"));
        vo.bind("$$assertDescCfgs", Global.newArray("A_invalidBuffer"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", "./LoggerStreamer2.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.uia.sysbios.LoggerStreamer2.Object", "ti.uia.sysbios"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./LoggerStreamer2.xdt");
        pkgV.bind("LoggerStreamer2", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerStreamer2");
    }

    void LoggerIdle$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle", "ti.uia.sysbios");
        po = (Proto.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle.Module", "ti.uia.sysbios");
        vo.init2(po, "ti.uia.sysbios.LoggerIdle", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.sysbios.LoggerIdle$$capsule", "ti.uia.sysbios"));
        vo.bind("Instance", om.findStrict("ti.uia.sysbios.LoggerIdle.Instance", "ti.uia.sysbios"));
        vo.bind("Params", om.findStrict("ti.uia.sysbios.LoggerIdle.Params", "ti.uia.sysbios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.uia.sysbios.LoggerIdle.Params", "ti.uia.sysbios")).newInstance());
        vo.bind("Handle", om.findStrict("ti.uia.sysbios.LoggerIdle.Handle", "ti.uia.sysbios"));
        vo.bind("$package", om.findStrict("ti.uia.sysbios", "ti.uia.sysbios"));
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
        vo.bind("TransportType", om.findStrict("ti.uia.sysbios.LoggerIdle.TransportType", "ti.uia.sysbios"));
        vo.bind("ModuleView", om.findStrict("ti.uia.sysbios.LoggerIdle.ModuleView", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerIdle.ModuleView", "ti.uia.sysbios"));
        vo.bind("RecordView", om.findStrict("ti.uia.sysbios.LoggerIdle.RecordView", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerIdle.RecordView", "ti.uia.sysbios"));
        vo.bind("RtaData", om.findStrict("ti.uia.sysbios.LoggerIdle.RtaData", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerIdle.RtaData", "ti.uia.sysbios"));
        vo.bind("LoggerFxn", om.findStrict("ti.uia.sysbios.LoggerIdle.LoggerFxn", "ti.uia.sysbios"));
        mcfgs.add("bufferSize");
        mcfgs.add("isTimestampEnabled");
        mcfgs.add("customTransportType");
        mcfgs.add("transportFxn");
        mcfgs.add("writeWhenFull");
        mcfgs.add("L_test");
        vo.bind("Module_State", om.findStrict("ti.uia.sysbios.LoggerIdle.Module_State", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerIdle.Module_State", "ti.uia.sysbios"));
        vo.bind("Instance_State", om.findStrict("ti.uia.sysbios.LoggerIdle.Instance_State", "ti.uia.sysbios"));
        tdefs.add(om.findStrict("ti.uia.sysbios.LoggerIdle.Instance_State", "ti.uia.sysbios"));
        vo.bind("TransportType_UART", om.findStrict("ti.uia.sysbios.LoggerIdle.TransportType_UART", "ti.uia.sysbios"));
        vo.bind("TransportType_USB", om.findStrict("ti.uia.sysbios.LoggerIdle.TransportType_USB", "ti.uia.sysbios"));
        vo.bind("TransportType_ETHERNET", om.findStrict("ti.uia.sysbios.LoggerIdle.TransportType_ETHERNET", "ti.uia.sysbios"));
        vo.bind("TransportType_CUSTOM", om.findStrict("ti.uia.sysbios.LoggerIdle.TransportType_CUSTOM", "ti.uia.sysbios"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.sysbios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.uia.sysbios.LoggerIdle$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$uia$sysbios$LoggerIdle$$__initObject"));
        }//isCFG
        vo.bind("flush", om.findStrict("ti.uia.sysbios.LoggerIdle.flush", "ti.uia.sysbios"));
        vo.bind("idleWrite", om.findStrict("ti.uia.sysbios.LoggerIdle.idleWrite", "ti.uia.sysbios"));
        vo.bind("idleWriteEvent", om.findStrict("ti.uia.sysbios.LoggerIdle.idleWriteEvent", "ti.uia.sysbios"));
        vo.bind("write", om.findStrict("ti.uia.sysbios.LoggerIdle.write", "ti.uia.sysbios"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_sysbios_LoggerIdle_Handle__label__E", "ti_uia_sysbios_LoggerIdle_Module__startupDone__E", "ti_uia_sysbios_LoggerIdle_Object__create__E", "ti_uia_sysbios_LoggerIdle_Object__delete__E", "ti_uia_sysbios_LoggerIdle_Object__get__E", "ti_uia_sysbios_LoggerIdle_Object__first__E", "ti_uia_sysbios_LoggerIdle_Object__next__E", "ti_uia_sysbios_LoggerIdle_Params__init__E", "ti_uia_sysbios_LoggerIdle_enable__E", "ti_uia_sysbios_LoggerIdle_disable__E", "ti_uia_sysbios_LoggerIdle_flush__E", "ti_uia_sysbios_LoggerIdle_write0__E", "ti_uia_sysbios_LoggerIdle_write1__E", "ti_uia_sysbios_LoggerIdle_write2__E", "ti_uia_sysbios_LoggerIdle_write4__E", "ti_uia_sysbios_LoggerIdle_write8__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("L_test"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", "./LoggerIdle.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.uia.sysbios.LoggerIdle.Object", "ti.uia.sysbios"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./LoggerIdle.xdt");
        pkgV.bind("LoggerIdle", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerIdle");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.uia.sysbios")).findStrict("PARAMS", "ti.uia.sysbios");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.uia.sysbios.LoggerStreamer", "ti.uia.sysbios")).findStrict("PARAMS", "ti.uia.sysbios");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.uia.sysbios.LoggerStreamer2", "ti.uia.sysbios")).findStrict("PARAMS", "ti.uia.sysbios");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.uia.sysbios.LoggerIdle", "ti.uia.sysbios")).findStrict("PARAMS", "ti.uia.sysbios");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.sysbios.IpcMP", "ti.uia.sysbios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.sysbios.Adaptor", "ti.uia.sysbios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.sysbios.LoggingSetup", "ti.uia.sysbios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.sysbios.LoggerStreamer", "ti.uia.sysbios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.sysbios.LoggerStreamer2", "ti.uia.sysbios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.sysbios.LoggerIdle", "ti.uia.sysbios"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer", "ti.uia.sysbios");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")}), Global.newArray(new Object[]{"Records", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitRecords", "structName", "RecordView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.sysbios.LoggerStreamer2", "ti.uia.sysbios");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")}), Global.newArray(new Object[]{"Instances", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitInstances", "structName", "InstanceView")}), Global.newArray(new Object[]{"Records", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitRecords", "structName", "RecordView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.sysbios.LoggerIdle", "ti.uia.sysbios");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")}), Global.newArray(new Object[]{"Records", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitRecords", "structName", "RecordView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.uia.sysbios.IpcMP")).bless();
        ((Value.Obj)om.getv("ti.uia.sysbios.Adaptor")).bless();
        ((Value.Obj)om.getv("ti.uia.sysbios.LoggingSetup")).bless();
        ((Value.Obj)om.getv("ti.uia.sysbios.LoggerStreamer")).bless();
        ((Value.Obj)om.getv("ti.uia.sysbios.LoggerStreamer2")).bless();
        ((Value.Obj)om.getv("ti.uia.sysbios.LoggerIdle")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.uia.sysbios")).add(pkgV);
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
        IpcMP$$OBJECTS();
        Adaptor$$OBJECTS();
        LoggingSetup$$OBJECTS();
        LoggerStreamer$$OBJECTS();
        LoggerStreamer2$$OBJECTS();
        LoggerIdle$$OBJECTS();
        IpcMP$$CONSTS();
        Adaptor$$CONSTS();
        LoggingSetup$$CONSTS();
        LoggerStreamer$$CONSTS();
        LoggerStreamer2$$CONSTS();
        LoggerIdle$$CONSTS();
        IpcMP$$CREATES();
        Adaptor$$CREATES();
        LoggingSetup$$CREATES();
        LoggerStreamer$$CREATES();
        LoggerStreamer2$$CREATES();
        LoggerIdle$$CREATES();
        IpcMP$$FUNCTIONS();
        Adaptor$$FUNCTIONS();
        LoggingSetup$$FUNCTIONS();
        LoggerStreamer$$FUNCTIONS();
        LoggerStreamer2$$FUNCTIONS();
        LoggerIdle$$FUNCTIONS();
        IpcMP$$SIZES();
        Adaptor$$SIZES();
        LoggingSetup$$SIZES();
        LoggerStreamer$$SIZES();
        LoggerStreamer2$$SIZES();
        LoggerIdle$$SIZES();
        IpcMP$$TYPES();
        Adaptor$$TYPES();
        LoggingSetup$$TYPES();
        LoggerStreamer$$TYPES();
        LoggerStreamer2$$TYPES();
        LoggerIdle$$TYPES();
        if (isROV) {
            IpcMP$$ROV();
            Adaptor$$ROV();
            LoggingSetup$$ROV();
            LoggerStreamer$$ROV();
            LoggerStreamer2$$ROV();
            LoggerIdle$$ROV();
        }//isROV
        $$SINGLETONS();
        IpcMP$$SINGLETONS();
        Adaptor$$SINGLETONS();
        LoggingSetup$$SINGLETONS();
        LoggerStreamer$$SINGLETONS();
        LoggerStreamer2$$SINGLETONS();
        LoggerIdle$$SINGLETONS();
        $$INITIALIZATION();
    }
}
