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

public class ti_uia_services
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
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.uia.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.uia.services.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.uia.services", new Value.Obj("ti.uia.services", pkgP));
    }

    void Rta$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.uia.services.Rta.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.uia.services.Rta", new Value.Obj("ti.uia.services.Rta", po));
        pkgV.bind("Rta", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.uia.services.Rta$$ModuleView", new Proto.Obj());
        om.bind("ti.uia.services.Rta.ModuleView", new Proto.Str(spo, false));
        om.bind("ti.uia.services.Rta.Command", new Proto.Enm("ti.uia.services.Rta.Command"));
        om.bind("ti.uia.services.Rta.ErrorCode", new Proto.Enm("ti.uia.services.Rta.ErrorCode"));
        spo = (Proto.Obj)om.bind("ti.uia.services.Rta$$Packet", new Proto.Obj());
        om.bind("ti.uia.services.Rta.Packet", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.uia.services.Rta$$Module_State", new Proto.Obj());
        om.bind("ti.uia.services.Rta.Module_State", new Proto.Str(spo, false));
    }

    void Rta$$CONSTS()
    {
        // module Rta
        om.bind("ti.uia.services.Rta.Command_READ_MASK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_READ_MASK", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.services.Rta.Command_WRITE_MASK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_WRITE_MASK", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.uia.services.Rta.Command_LOGGER_OFF", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_LOGGER_OFF", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.uia.services.Rta.Command_LOGGER_ON", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_LOGGER_ON", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.uia.services.Rta.Command_GET_CPU_SPEED", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_GET_CPU_SPEED", xdc.services.intern.xsr.Enum.intValue(4L)+0));
        om.bind("ti.uia.services.Rta.Command_RESET_LOGGER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_RESET_LOGGER", xdc.services.intern.xsr.Enum.intValue(5L)+0));
        om.bind("ti.uia.services.Rta.Command_CHANGE_PERIOD", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_CHANGE_PERIOD", xdc.services.intern.xsr.Enum.intValue(6L)+0));
        om.bind("ti.uia.services.Rta.Command_START_TX", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_START_TX", xdc.services.intern.xsr.Enum.intValue(7L)+0));
        om.bind("ti.uia.services.Rta.Command_STOP_TX", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_STOP_TX", xdc.services.intern.xsr.Enum.intValue(8L)+0));
        om.bind("ti.uia.services.Rta.Command_LOGGER_OFF_ALL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_LOGGER_OFF_ALL", xdc.services.intern.xsr.Enum.intValue(9L)+0));
        om.bind("ti.uia.services.Rta.Command_LOGGER_ON_ALL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_LOGGER_ON_ALL", xdc.services.intern.xsr.Enum.intValue(10L)+0));
        om.bind("ti.uia.services.Rta.Command_RESET_LOGGER_ALL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_RESET_LOGGER_ALL", xdc.services.intern.xsr.Enum.intValue(11L)+0));
        om.bind("ti.uia.services.Rta.Command_SNAPSHOT_ALL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"), "ti.uia.services.Rta.Command_SNAPSHOT_ALL", xdc.services.intern.xsr.Enum.intValue(12L)+0));
        om.bind("ti.uia.services.Rta.ErrorCode_NULLPOINTER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.uia.services.Rta.ErrorCode", "ti.uia.services"), "ti.uia.services.Rta.ErrorCode_NULLPOINTER", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.uia.services.Rta.processCallback", new Extern("ti_uia_services_Rta_processCallback__E", "xdc_Void(*)(ti_uia_runtime_ServiceMgr_Reason,ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.services.Rta.disableAllLogs", new Extern("ti_uia_services_Rta_disableAllLogs__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.services.Rta.enableAllLogs", new Extern("ti_uia_services_Rta_enableAllLogs__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.services.Rta.snapshotAllLogs", new Extern("ti_uia_services_Rta_snapshotAllLogs__E", "xdc_Void(*)(xdc_UArg,xdc_UArg)", true, false));
        om.bind("ti.uia.services.Rta.resetAllLogs", new Extern("ti_uia_services_Rta_resetAllLogs__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.services.Rta.startDataTx", new Extern("ti_uia_services_Rta_startDataTx__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.services.Rta.stopDataTx", new Extern("ti_uia_services_Rta_stopDataTx__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.services.Rta.sendEvents", new Extern("ti_uia_services_Rta_sendEvents__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.uia.services.Rta.processMsg", new Extern("ti_uia_services_Rta_processMsg__I", "xdc_Void(*)(ti_uia_runtime_UIAPacket_Hdr*)", true, false));
        om.bind("ti.uia.services.Rta.flushLogger", new Extern("ti_uia_services_Rta_flushLogger__I", "xdc_Void(*)(ti_uia_runtime_IUIATransfer_Handle,xdc_UInt)", true, false));
        om.bind("ti.uia.services.Rta.acknowledgeCmd", new Extern("ti_uia_services_Rta_acknowledgeCmd__I", "xdc_Void(*)(ti_uia_services_Rta_Packet*)", true, false));
        om.bind("ti.uia.services.Rta.readMask", new Extern("ti_uia_services_Rta_readMask__I", "ti_uia_runtime_UIAPacket_MsgType(*)(ti_uia_services_Rta_Packet*,xdc_UArg)", true, false));
        om.bind("ti.uia.services.Rta.writeMask", new Extern("ti_uia_services_Rta_writeMask__I", "ti_uia_runtime_UIAPacket_MsgType(*)(ti_uia_services_Rta_Packet*,xdc_UArg,xdc_UArg)", true, false));
        om.bind("ti.uia.services.Rta.enableLog", new Extern("ti_uia_services_Rta_enableLog__I", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.uia.services.Rta.disableLog", new Extern("ti_uia_services_Rta_disableLog__I", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.uia.services.Rta.getCpuSpeed", new Extern("ti_uia_services_Rta_getCpuSpeed__I", "xdc_Void(*)(ti_uia_services_Rta_Packet*)", true, false));
        om.bind("ti.uia.services.Rta.resetLog", new Extern("ti_uia_services_Rta_resetLog__I", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.uia.services.Rta.changePeriod", new Extern("ti_uia_services_Rta_changePeriod__I", "xdc_Void(*)(xdc_UArg)", true, false));
    }

    void Rta$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Rta$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Rta.registerLoggers
        fxn = (Proto.Fxn)om.bind("ti.uia.services.Rta$$registerLoggers", new Proto.Fxn(om.findStrict("ti.uia.services.Rta.Module", "ti.uia.services"), null, 0, -1, false));
    }

    void Rta$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.uia.services.Rta.Packet", "ti.uia.services");
        sizes.clear();
        sizes.add(Global.newArray("hdr", "Sti.uia.runtime.UIAPacket;Hdr"));
        sizes.add(Global.newArray("arg0", "UInt32"));
        sizes.add(Global.newArray("arg1", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.services.Rta.Packet']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.services.Rta.Packet']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.services.Rta.Packet'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.uia.services.Rta.Module_State", "ti.uia.services");
        sizes.clear();
        sizes.add(Global.newArray("loggers", "UPtr"));
        sizes.add(Global.newArray("numLoggers", "UInt"));
        sizes.add(Global.newArray("totalPacketsSent", "UInt"));
        sizes.add(Global.newArray("period", "TInt"));
        sizes.add(Global.newArray("seq", "UInt16"));
        sizes.add(Global.newArray("txData", "UShort"));
        sizes.add(Global.newArray("snapshot", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.uia.services.Rta.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.uia.services.Rta.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.uia.services.Rta.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Rta$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/services/Rta.xs");
        om.bind("ti.uia.services.Rta$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.uia.services.Rta.Module", "ti.uia.services");
        po.init("ti.uia.services.Rta.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.uia.services"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.uia.services"), $$UNDEF, "wh");
            po.addFld("LD_recordsSent", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.services"), Global.newObject("mask", 0x0200L, "msg", "LD_recordsSent: Sent %d bytes from logger [%d] 0x%x"), "w");
            po.addFld("LD_cmdRcvd", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.services"), Global.newObject("mask", 0x0200L, "msg", "LD_cmdRcvd: Received command: %d, arg0: 0x%x, arg1: 0x%x"), "w");
            po.addFld("LD_writeMask", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.uia.services"), Global.newObject("mask", 0x0200L, "msg", "LD_writeMask: Mask addres: 0x%x, New mask value: 0x%x"), "w");
            po.addFld("periodInMs", Proto.Elm.newCNum("(xdc_Int)"), 100L, "w");
            po.addFld("SERVICEID", (Proto)om.findStrict("ti.uia.runtime.ServiceMgr$$ServiceId", "ti.uia.services"), $$DEFAULT, "r");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.uia.services.Rta$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.uia.services.Rta$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.uia.services.Rta$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.uia.services.Rta$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("registerLoggers", (Proto.Fxn)om.findStrict("ti.uia.services.Rta$$registerLoggers", "ti.uia.services"), Global.get(cap, "registerLoggers"));
        // struct Rta.ModuleView
        po = (Proto.Obj)om.findStrict("ti.uia.services.Rta$$ModuleView", "ti.uia.services");
        po.init("ti.uia.services.Rta.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("serviceId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("snapshotMode", $$T_Bool, $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numLoggers", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("loggers", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("sequence", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("totalPacketsSent", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct Rta.Packet
        po = (Proto.Obj)om.findStrict("ti.uia.services.Rta$$Packet", "ti.uia.services");
        po.init("ti.uia.services.Rta.Packet", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("hdr", (Proto)om.findStrict("ti.uia.runtime.UIAPacket.Hdr", "ti.uia.services"), $$DEFAULT, "w");
                po.addFld("arg0", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("arg1", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
        // struct Rta.Module_State
        po = (Proto.Obj)om.findStrict("ti.uia.services.Rta$$Module_State", "ti.uia.services");
        po.init("ti.uia.services.Rta.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("loggers", new Proto.Arr((Proto)om.findStrict("ti.uia.runtime.IUIATransfer.Handle", "ti.uia.services"), false), $$DEFAULT, "w");
                po.addFld("numLoggers", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("totalPacketsSent", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("seq", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("txData", $$T_Bool, $$UNDEF, "w");
                po.addFld("snapshot", $$T_Bool, $$UNDEF, "w");
    }

    void Rta$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.services.Rta", "ti.uia.services");
        vo.bind("Packet$fetchDesc", Global.newObject("type", "ti.uia.services.Rta.Packet", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.services.Rta$$Packet", "ti.uia.services");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.uia.services.Rta.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.uia.services.Rta$$Module_State", "ti.uia.services");
        po.bind("loggers$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.uia.services.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.uia.services"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/uia/services/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.uia.services"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.uia.services"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.uia.services"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.uia.services"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.uia.services"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.uia.services"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.uia.services", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.uia.services");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.uia.services.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.rov", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.uia.services'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.uia.services$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.uia.services$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.uia.services$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/release/ti.uia.services.aem4',\n");
            sb.append("'lib/release/ti.uia.services.am4',\n");
            sb.append("'lib/release/ti.uia.services.am4g',\n");
            sb.append("'lib/release/ti.uia.services.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/release/ti.uia.services.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/release/ti.uia.services.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/release/ti.uia.services.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/release/ti.uia.services.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Rta$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.uia.services.Rta", "ti.uia.services");
        po = (Proto.Obj)om.findStrict("ti.uia.services.Rta.Module", "ti.uia.services");
        vo.init2(po, "ti.uia.services.Rta", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.uia.services.Rta$$capsule", "ti.uia.services"));
        vo.bind("$package", om.findStrict("ti.uia.services", "ti.uia.services"));
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
        vo.bind("ModuleView", om.findStrict("ti.uia.services.Rta.ModuleView", "ti.uia.services"));
        tdefs.add(om.findStrict("ti.uia.services.Rta.ModuleView", "ti.uia.services"));
        mcfgs.add("LD_recordsSent");
        mcfgs.add("LD_cmdRcvd");
        mcfgs.add("LD_writeMask");
        mcfgs.add("periodInMs");
        mcfgs.add("SERVICEID");
        icfgs.add("SERVICEID");
        vo.bind("Command", om.findStrict("ti.uia.services.Rta.Command", "ti.uia.services"));
        vo.bind("ErrorCode", om.findStrict("ti.uia.services.Rta.ErrorCode", "ti.uia.services"));
        vo.bind("Packet", om.findStrict("ti.uia.services.Rta.Packet", "ti.uia.services"));
        tdefs.add(om.findStrict("ti.uia.services.Rta.Packet", "ti.uia.services"));
        vo.bind("Module_State", om.findStrict("ti.uia.services.Rta.Module_State", "ti.uia.services"));
        tdefs.add(om.findStrict("ti.uia.services.Rta.Module_State", "ti.uia.services"));
        vo.bind("Command_READ_MASK", om.findStrict("ti.uia.services.Rta.Command_READ_MASK", "ti.uia.services"));
        vo.bind("Command_WRITE_MASK", om.findStrict("ti.uia.services.Rta.Command_WRITE_MASK", "ti.uia.services"));
        vo.bind("Command_LOGGER_OFF", om.findStrict("ti.uia.services.Rta.Command_LOGGER_OFF", "ti.uia.services"));
        vo.bind("Command_LOGGER_ON", om.findStrict("ti.uia.services.Rta.Command_LOGGER_ON", "ti.uia.services"));
        vo.bind("Command_GET_CPU_SPEED", om.findStrict("ti.uia.services.Rta.Command_GET_CPU_SPEED", "ti.uia.services"));
        vo.bind("Command_RESET_LOGGER", om.findStrict("ti.uia.services.Rta.Command_RESET_LOGGER", "ti.uia.services"));
        vo.bind("Command_CHANGE_PERIOD", om.findStrict("ti.uia.services.Rta.Command_CHANGE_PERIOD", "ti.uia.services"));
        vo.bind("Command_START_TX", om.findStrict("ti.uia.services.Rta.Command_START_TX", "ti.uia.services"));
        vo.bind("Command_STOP_TX", om.findStrict("ti.uia.services.Rta.Command_STOP_TX", "ti.uia.services"));
        vo.bind("Command_LOGGER_OFF_ALL", om.findStrict("ti.uia.services.Rta.Command_LOGGER_OFF_ALL", "ti.uia.services"));
        vo.bind("Command_LOGGER_ON_ALL", om.findStrict("ti.uia.services.Rta.Command_LOGGER_ON_ALL", "ti.uia.services"));
        vo.bind("Command_RESET_LOGGER_ALL", om.findStrict("ti.uia.services.Rta.Command_RESET_LOGGER_ALL", "ti.uia.services"));
        vo.bind("Command_SNAPSHOT_ALL", om.findStrict("ti.uia.services.Rta.Command_SNAPSHOT_ALL", "ti.uia.services"));
        vo.bind("ErrorCode_NULLPOINTER", om.findStrict("ti.uia.services.Rta.ErrorCode_NULLPOINTER", "ti.uia.services"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.uia.services")).add(vo);
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
        vo.bind("processCallback", om.findStrict("ti.uia.services.Rta.processCallback", "ti.uia.services"));
        vo.bind("disableAllLogs", om.findStrict("ti.uia.services.Rta.disableAllLogs", "ti.uia.services"));
        vo.bind("enableAllLogs", om.findStrict("ti.uia.services.Rta.enableAllLogs", "ti.uia.services"));
        vo.bind("snapshotAllLogs", om.findStrict("ti.uia.services.Rta.snapshotAllLogs", "ti.uia.services"));
        vo.bind("resetAllLogs", om.findStrict("ti.uia.services.Rta.resetAllLogs", "ti.uia.services"));
        vo.bind("startDataTx", om.findStrict("ti.uia.services.Rta.startDataTx", "ti.uia.services"));
        vo.bind("stopDataTx", om.findStrict("ti.uia.services.Rta.stopDataTx", "ti.uia.services"));
        vo.bind("sendEvents", om.findStrict("ti.uia.services.Rta.sendEvents", "ti.uia.services"));
        vo.bind("processMsg", om.findStrict("ti.uia.services.Rta.processMsg", "ti.uia.services"));
        vo.bind("flushLogger", om.findStrict("ti.uia.services.Rta.flushLogger", "ti.uia.services"));
        vo.bind("acknowledgeCmd", om.findStrict("ti.uia.services.Rta.acknowledgeCmd", "ti.uia.services"));
        vo.bind("readMask", om.findStrict("ti.uia.services.Rta.readMask", "ti.uia.services"));
        vo.bind("writeMask", om.findStrict("ti.uia.services.Rta.writeMask", "ti.uia.services"));
        vo.bind("enableLog", om.findStrict("ti.uia.services.Rta.enableLog", "ti.uia.services"));
        vo.bind("disableLog", om.findStrict("ti.uia.services.Rta.disableLog", "ti.uia.services"));
        vo.bind("getCpuSpeed", om.findStrict("ti.uia.services.Rta.getCpuSpeed", "ti.uia.services"));
        vo.bind("resetLog", om.findStrict("ti.uia.services.Rta.resetLog", "ti.uia.services"));
        vo.bind("changePeriod", om.findStrict("ti.uia.services.Rta.changePeriod", "ti.uia.services"));
        vo.bind("$$fxntab", Global.newArray("ti_uia_services_Rta_Module__startupDone__E", "ti_uia_services_Rta_processCallback__E", "ti_uia_services_Rta_disableAllLogs__E", "ti_uia_services_Rta_enableAllLogs__E", "ti_uia_services_Rta_snapshotAllLogs__E", "ti_uia_services_Rta_resetAllLogs__E", "ti_uia_services_Rta_startDataTx__E", "ti_uia_services_Rta_stopDataTx__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("LD_recordsSent", "LD_cmdRcvd", "LD_writeMask"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "processCallback", "entry", "%d, %p", "exit", ""));
        loggables.add(Global.newObject("name", "disableAllLogs", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "enableAllLogs", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "snapshotAllLogs", "entry", "%p, %p", "exit", ""));
        loggables.add(Global.newObject("name", "resetAllLogs", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "startDataTx", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "stopDataTx", "entry", "", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Rta", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Rta");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.uia.services.Rta", "ti.uia.services"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.uia.services.Rta", "ti.uia.services");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.uia.services.Rta")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.uia.services")).add(pkgV);
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
        Rta$$OBJECTS();
        Rta$$CONSTS();
        Rta$$CREATES();
        Rta$$FUNCTIONS();
        Rta$$SIZES();
        Rta$$TYPES();
        if (isROV) {
            Rta$$ROV();
        }//isROV
        $$SINGLETONS();
        Rta$$SINGLETONS();
        $$INITIALIZATION();
    }
}
