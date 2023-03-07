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

public class ti_sysbios_utils
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
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.interfaces");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.utils.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.utils", new Value.Obj("ti.sysbios.utils", pkgP));
    }

    void Load$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.utils.Load.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.utils.Load", new Value.Obj("ti.sysbios.utils.Load", po));
        pkgV.bind("Load", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.utils.Load$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.utils.Load.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.utils.Load$$Stat", new Proto.Obj());
        om.bind("ti.sysbios.utils.Load.Stat", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.utils.Load$$HookContext", new Proto.Obj());
        om.bind("ti.sysbios.utils.Load.HookContext", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.utils.Load$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.utils.Load.Module_State", new Proto.Str(spo, false));
    }

    void Load$$CONSTS()
    {
        // module Load
        om.bind("ti.sysbios.utils.Load.getTaskLoad", new Extern("ti_sysbios_utils_Load_getTaskLoad__E", "xdc_Bool(*)(ti_sysbios_knl_Task_Handle,ti_sysbios_utils_Load_Stat*)", true, false));
        om.bind("ti.sysbios.utils.Load.update", new Extern("ti_sysbios_utils_Load_update__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.updateCPULoad", new Extern("ti_sysbios_utils_Load_updateCPULoad__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.updateLoads", new Extern("ti_sysbios_utils_Load_updateLoads__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.updateContextsAndPost", new Extern("ti_sysbios_utils_Load_updateContextsAndPost__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.updateCurrentThreadTime", new Extern("ti_sysbios_utils_Load_updateCurrentThreadTime__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.updateThreadContexts", new Extern("ti_sysbios_utils_Load_updateThreadContexts__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.reset", new Extern("ti_sysbios_utils_Load_reset__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.getGlobalSwiLoad", new Extern("ti_sysbios_utils_Load_getGlobalSwiLoad__E", "xdc_Bool(*)(ti_sysbios_utils_Load_Stat*)", true, false));
        om.bind("ti.sysbios.utils.Load.getGlobalHwiLoad", new Extern("ti_sysbios_utils_Load_getGlobalHwiLoad__E", "xdc_Bool(*)(ti_sysbios_utils_Load_Stat*)", true, false));
        om.bind("ti.sysbios.utils.Load.getCPULoad", new Extern("ti_sysbios_utils_Load_getCPULoad__E", "xdc_UInt32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.calculateLoad", new Extern("ti_sysbios_utils_Load_calculateLoad__E", "xdc_UInt32(*)(ti_sysbios_utils_Load_Stat*)", true, false));
        om.bind("ti.sysbios.utils.Load.setMinIdle", new Extern("ti_sysbios_utils_Load_setMinIdle__E", "xdc_UInt32(*)(xdc_UInt32)", true, false));
        om.bind("ti.sysbios.utils.Load.addTask", new Extern("ti_sysbios_utils_Load_addTask__E", "xdc_Void(*)(ti_sysbios_knl_Task_Handle,ti_sysbios_utils_Load_HookContext*)", true, false));
        om.bind("ti.sysbios.utils.Load.removeTask", new Extern("ti_sysbios_utils_Load_removeTask__E", "xdc_Bool(*)(ti_sysbios_knl_Task_Handle)", true, false));
        om.bind("ti.sysbios.utils.Load.idleFxn", new Extern("ti_sysbios_utils_Load_idleFxn__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.idleFxnPwr", new Extern("ti_sysbios_utils_Load_idleFxnPwr__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.startup", new Extern("ti_sysbios_utils_Load_startup__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.taskCreateHook", new Extern("ti_sysbios_utils_Load_taskCreateHook__E", "xdc_Void(*)(ti_sysbios_knl_Task_Handle,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.utils.Load.taskDeleteHook", new Extern("ti_sysbios_utils_Load_taskDeleteHook__E", "xdc_Void(*)(ti_sysbios_knl_Task_Handle)", true, false));
        om.bind("ti.sysbios.utils.Load.taskSwitchHook", new Extern("ti_sysbios_utils_Load_taskSwitchHook__E", "xdc_Void(*)(ti_sysbios_knl_Task_Handle,ti_sysbios_knl_Task_Handle)", true, false));
        om.bind("ti.sysbios.utils.Load.swiBeginHook", new Extern("ti_sysbios_utils_Load_swiBeginHook__E", "xdc_Void(*)(ti_sysbios_knl_Swi_Handle)", true, false));
        om.bind("ti.sysbios.utils.Load.swiEndHook", new Extern("ti_sysbios_utils_Load_swiEndHook__E", "xdc_Void(*)(ti_sysbios_knl_Swi_Handle)", true, false));
        om.bind("ti.sysbios.utils.Load.hwiBeginHook", new Extern("ti_sysbios_utils_Load_hwiBeginHook__E", "xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle)", true, false));
        om.bind("ti.sysbios.utils.Load.hwiEndHook", new Extern("ti_sysbios_utils_Load_hwiEndHook__E", "xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle)", true, false));
        om.bind("ti.sysbios.utils.Load.taskRegHook", new Extern("ti_sysbios_utils_Load_taskRegHook__E", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.utils.Load.logLoads", new Extern("ti_sysbios_utils_Load_logLoads__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.utils.Load.logCPULoad", new Extern("ti_sysbios_utils_Load_logCPULoad__I", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void Load$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Load$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Load$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.utils.Load.Stat", "ti.sysbios.utils");
        sizes.clear();
        sizes.add(Global.newArray("threadTime", "UInt32"));
        sizes.add(Global.newArray("totalTime", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.utils.Load.Stat']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.utils.Load.Stat']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.utils.Load.Stat'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.utils.Load.HookContext", "ti.sysbios.utils");
        sizes.clear();
        sizes.add(Global.newArray("qElem", "Sti.sysbios.knl.Queue;Elem"));
        sizes.add(Global.newArray("totalTimeElapsed", "UInt32"));
        sizes.add(Global.newArray("totalTime", "UInt32"));
        sizes.add(Global.newArray("nextTotalTime", "UInt32"));
        sizes.add(Global.newArray("timeOfLastUpdate", "UInt32"));
        sizes.add(Global.newArray("threadHandle", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.utils.Load.HookContext']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.utils.Load.HookContext']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.utils.Load.HookContext'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.utils.Load.Module_State", "ti.sysbios.utils");
        sizes.clear();
        sizes.add(Global.newArray("taskHId", "TInt"));
        sizes.add(Global.newArray("taskStartTime", "UPtr"));
        sizes.add(Global.newArray("timeElapsed", "UInt32"));
        sizes.add(Global.newArray("runningTask", "UPtr"));
        sizes.add(Global.newArray("firstSwitchDone", "UShort"));
        sizes.add(Global.newArray("swiStartTime", "UInt32"));
        sizes.add(Global.newArray("swiEnv", "Sti.sysbios.utils.Load;HookContext"));
        sizes.add(Global.newArray("taskEnv", "UPtr"));
        sizes.add(Global.newArray("swiCnt", "UInt32"));
        sizes.add(Global.newArray("hwiStartTime", "UInt32"));
        sizes.add(Global.newArray("hwiEnv", "Sti.sysbios.utils.Load;HookContext"));
        sizes.add(Global.newArray("hwiCnt", "UInt32"));
        sizes.add(Global.newArray("timeSlotCnt", "UInt32"));
        sizes.add(Global.newArray("minLoop", "UInt32"));
        sizes.add(Global.newArray("minIdle", "UInt32"));
        sizes.add(Global.newArray("t0", "UInt32"));
        sizes.add(Global.newArray("idleCnt", "UInt32"));
        sizes.add(Global.newArray("cpuLoad", "UInt32"));
        sizes.add(Global.newArray("taskEnvLen", "UInt32"));
        sizes.add(Global.newArray("taskNum", "UInt32"));
        sizes.add(Global.newArray("powerEnabled", "UShort"));
        sizes.add(Global.newArray("idleStartTime", "UInt32"));
        sizes.add(Global.newArray("busyStartTime", "UInt32"));
        sizes.add(Global.newArray("busyTime", "UInt32"));
        sizes.add(Global.newArray("taskList", "Sti.sysbios.knl.Queue;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.utils.Load.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.utils.Load.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.utils.Load.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Load$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/utils/Load.xs");
        om.bind("ti.sysbios.utils.Load$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.utils.Load.Module", "ti.sysbios.utils");
        po.init("ti.sysbios.utils.Load.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.utils"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.utils"), $$UNDEF, "wh");
            po.addFld("LS_cpuLoad", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.utils"), Global.newObject("mask", 0x0800L, "msg", "LS_cpuLoad: %d%%"), "w");
            po.addFld("LS_hwiLoad", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.utils"), Global.newObject("mask", 0x0800L, "msg", "LS_hwiLoad: %d,%d"), "w");
            po.addFld("LS_swiLoad", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.utils"), Global.newObject("mask", 0x0800L, "msg", "LS_swiLoad: %d,%d"), "w");
            po.addFld("LS_taskLoad", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.utils"), Global.newObject("mask", 0x0800L, "msg", "LS_taskLoad: 0x%x,%d,%d,0x%x"), "w");
            po.addFld("postUpdate", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), null, "w");
            po.addFld("updateInIdle", $$T_Bool, true, "w");
            po.addFld("enableCPULoadCalc", $$T_Bool, true, "wh");
            po.addFld("minIdle", Proto.Elm.newCNum("(xdc_UInt32)"), 0L, "wh");
            po.addFld("windowInMs", Proto.Elm.newCNum("(xdc_UInt)"), 500L, "w");
            po.addFld("hwiEnabled", $$T_Bool, false, "w");
            po.addFld("swiEnabled", $$T_Bool, false, "w");
            po.addFld("taskEnabled", $$T_Bool, true, "w");
            po.addFld("autoAddTasks", $$T_Bool, true, "w");
            po.addFld("powerEnabled", $$T_Bool, $$UNDEF, "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.utils.Load$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.utils.Load$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.utils.Load$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.utils.Load$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct Load.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.utils.Load$$ModuleView", "ti.sysbios.utils");
        po.init("ti.sysbios.utils.Load.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("cpuLoad", $$T_Str, $$UNDEF, "w");
                po.addFld("swiLoad", $$T_Str, $$UNDEF, "w");
                po.addFld("hwiLoad", $$T_Str, $$UNDEF, "w");
                po.addFld("idleError", $$T_Str, $$UNDEF, "w");
        // typedef Load.FuncPtr
        om.bind("ti.sysbios.utils.Load.FuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"));
        // struct Load.Stat
        po = (Proto.Obj)om.findStrict("ti.sysbios.utils.Load$$Stat", "ti.sysbios.utils");
        po.init("ti.sysbios.utils.Load.Stat", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("threadTime", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("totalTime", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
        // struct Load.HookContext
        po = (Proto.Obj)om.findStrict("ti.sysbios.utils.Load$$HookContext", "ti.sysbios.utils");
        po.init("ti.sysbios.utils.Load.HookContext", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("qElem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.utils"), $$DEFAULT, "w");
                po.addFld("totalTimeElapsed", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("totalTime", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("nextTotalTime", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("timeOfLastUpdate", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("threadHandle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        // struct Load.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.utils.Load$$Module_State", "ti.sysbios.utils");
        po.init("ti.sysbios.utils.Load.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("taskHId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("taskStartTime", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false), $$DEFAULT, "w");
                po.addFld("timeElapsed", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("runningTask", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.utils"), false), $$DEFAULT, "w");
                po.addFld("firstSwitchDone", $$T_Bool, $$UNDEF, "w");
                po.addFld("swiStartTime", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("swiEnv", (Proto)om.findStrict("ti.sysbios.utils.Load.HookContext", "ti.sysbios.utils"), $$DEFAULT, "w");
                po.addFld("taskEnv", new Proto.Arr((Proto)om.findStrict("ti.sysbios.utils.Load.HookContext", "ti.sysbios.utils"), false), $$DEFAULT, "w");
                po.addFld("swiCnt", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("hwiStartTime", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("hwiEnv", (Proto)om.findStrict("ti.sysbios.utils.Load.HookContext", "ti.sysbios.utils"), $$DEFAULT, "w");
                po.addFld("hwiCnt", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("timeSlotCnt", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("minLoop", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("minIdle", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("t0", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("idleCnt", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("cpuLoad", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("taskEnvLen", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("taskNum", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("powerEnabled", $$T_Bool, $$UNDEF, "w");
                po.addFld("idleStartTime", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("busyStartTime", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("busyTime", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFldV("taskList", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.utils"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_taskList", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.utils"), $$DEFAULT, "w");
    }

    void Load$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.utils.Load", "ti.sysbios.utils");
        vo.bind("Stat$fetchDesc", Global.newObject("type", "ti.sysbios.utils.Load.Stat", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.utils.Load$$Stat", "ti.sysbios.utils");
        vo.bind("HookContext$fetchDesc", Global.newObject("type", "ti.sysbios.utils.Load.HookContext", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.utils.Load$$HookContext", "ti.sysbios.utils");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.utils.Load.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.utils.Load$$Module_State", "ti.sysbios.utils");
        po.bind("taskStartTime$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_UInt32", "isScalar", true));
        po.bind("runningTask$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
        po.bind("taskEnv$fetchDesc", Global.newObject("type", "ti.sysbios.utils.Load.HookContext", "isScalar", false));
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.utils.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.utils"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/utils/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.utils"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.utils"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.utils"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.utils"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.utils"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.utils"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.utils", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.utils");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.utils.");
        pkgV.bind("$vers", Global.newArray(2, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.utils'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.utils$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.utils$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.utils$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.utils.aem4',\n");
            sb.append("'lib/debug/ti.sysbios.utils.am4',\n");
            sb.append("'lib/debug/ti.sysbios.utils.am4g',\n");
            sb.append("'lib/debug/ti.sysbios.utils.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.utils.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/debug/ti.sysbios.utils.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/debug/ti.sysbios.utils.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/debug/ti.sysbios.utils.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Load$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.utils.Load", "ti.sysbios.utils");
        po = (Proto.Obj)om.findStrict("ti.sysbios.utils.Load.Module", "ti.sysbios.utils");
        vo.init2(po, "ti.sysbios.utils.Load", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.utils.Load$$capsule", "ti.sysbios.utils"));
        vo.bind("$package", om.findStrict("ti.sysbios.utils", "ti.sysbios.utils"));
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
        vo.bind("ModuleView", om.findStrict("ti.sysbios.utils.Load.ModuleView", "ti.sysbios.utils"));
        tdefs.add(om.findStrict("ti.sysbios.utils.Load.ModuleView", "ti.sysbios.utils"));
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.utils.Load.FuncPtr", "ti.sysbios.utils"));
        vo.bind("Stat", om.findStrict("ti.sysbios.utils.Load.Stat", "ti.sysbios.utils"));
        tdefs.add(om.findStrict("ti.sysbios.utils.Load.Stat", "ti.sysbios.utils"));
        mcfgs.add("LS_cpuLoad");
        mcfgs.add("LS_hwiLoad");
        mcfgs.add("LS_swiLoad");
        mcfgs.add("LS_taskLoad");
        mcfgs.add("postUpdate");
        mcfgs.add("updateInIdle");
        mcfgs.add("windowInMs");
        mcfgs.add("hwiEnabled");
        mcfgs.add("swiEnabled");
        mcfgs.add("taskEnabled");
        vo.bind("HookContext", om.findStrict("ti.sysbios.utils.Load.HookContext", "ti.sysbios.utils"));
        tdefs.add(om.findStrict("ti.sysbios.utils.Load.HookContext", "ti.sysbios.utils"));
        mcfgs.add("autoAddTasks");
        icfgs.add("autoAddTasks");
        icfgs.add("powerEnabled");
        vo.bind("Module_State", om.findStrict("ti.sysbios.utils.Load.Module_State", "ti.sysbios.utils"));
        tdefs.add(om.findStrict("ti.sysbios.utils.Load.Module_State", "ti.sysbios.utils"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.utils")).add(vo);
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
        vo.bind("getTaskLoad", om.findStrict("ti.sysbios.utils.Load.getTaskLoad", "ti.sysbios.utils"));
        vo.bind("update", om.findStrict("ti.sysbios.utils.Load.update", "ti.sysbios.utils"));
        vo.bind("updateCPULoad", om.findStrict("ti.sysbios.utils.Load.updateCPULoad", "ti.sysbios.utils"));
        vo.bind("updateLoads", om.findStrict("ti.sysbios.utils.Load.updateLoads", "ti.sysbios.utils"));
        vo.bind("updateContextsAndPost", om.findStrict("ti.sysbios.utils.Load.updateContextsAndPost", "ti.sysbios.utils"));
        vo.bind("updateCurrentThreadTime", om.findStrict("ti.sysbios.utils.Load.updateCurrentThreadTime", "ti.sysbios.utils"));
        vo.bind("updateThreadContexts", om.findStrict("ti.sysbios.utils.Load.updateThreadContexts", "ti.sysbios.utils"));
        vo.bind("reset", om.findStrict("ti.sysbios.utils.Load.reset", "ti.sysbios.utils"));
        vo.bind("getGlobalSwiLoad", om.findStrict("ti.sysbios.utils.Load.getGlobalSwiLoad", "ti.sysbios.utils"));
        vo.bind("getGlobalHwiLoad", om.findStrict("ti.sysbios.utils.Load.getGlobalHwiLoad", "ti.sysbios.utils"));
        vo.bind("getCPULoad", om.findStrict("ti.sysbios.utils.Load.getCPULoad", "ti.sysbios.utils"));
        vo.bind("calculateLoad", om.findStrict("ti.sysbios.utils.Load.calculateLoad", "ti.sysbios.utils"));
        vo.bind("setMinIdle", om.findStrict("ti.sysbios.utils.Load.setMinIdle", "ti.sysbios.utils"));
        vo.bind("addTask", om.findStrict("ti.sysbios.utils.Load.addTask", "ti.sysbios.utils"));
        vo.bind("removeTask", om.findStrict("ti.sysbios.utils.Load.removeTask", "ti.sysbios.utils"));
        vo.bind("idleFxn", om.findStrict("ti.sysbios.utils.Load.idleFxn", "ti.sysbios.utils"));
        vo.bind("idleFxnPwr", om.findStrict("ti.sysbios.utils.Load.idleFxnPwr", "ti.sysbios.utils"));
        vo.bind("startup", om.findStrict("ti.sysbios.utils.Load.startup", "ti.sysbios.utils"));
        vo.bind("taskCreateHook", om.findStrict("ti.sysbios.utils.Load.taskCreateHook", "ti.sysbios.utils"));
        vo.bind("taskDeleteHook", om.findStrict("ti.sysbios.utils.Load.taskDeleteHook", "ti.sysbios.utils"));
        vo.bind("taskSwitchHook", om.findStrict("ti.sysbios.utils.Load.taskSwitchHook", "ti.sysbios.utils"));
        vo.bind("swiBeginHook", om.findStrict("ti.sysbios.utils.Load.swiBeginHook", "ti.sysbios.utils"));
        vo.bind("swiEndHook", om.findStrict("ti.sysbios.utils.Load.swiEndHook", "ti.sysbios.utils"));
        vo.bind("hwiBeginHook", om.findStrict("ti.sysbios.utils.Load.hwiBeginHook", "ti.sysbios.utils"));
        vo.bind("hwiEndHook", om.findStrict("ti.sysbios.utils.Load.hwiEndHook", "ti.sysbios.utils"));
        vo.bind("taskRegHook", om.findStrict("ti.sysbios.utils.Load.taskRegHook", "ti.sysbios.utils"));
        vo.bind("logLoads", om.findStrict("ti.sysbios.utils.Load.logLoads", "ti.sysbios.utils"));
        vo.bind("logCPULoad", om.findStrict("ti.sysbios.utils.Load.logCPULoad", "ti.sysbios.utils"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_utils_Load_Module__startupDone__E", "ti_sysbios_utils_Load_getTaskLoad__E", "ti_sysbios_utils_Load_update__E", "ti_sysbios_utils_Load_updateCPULoad__E", "ti_sysbios_utils_Load_updateLoads__E", "ti_sysbios_utils_Load_updateContextsAndPost__E", "ti_sysbios_utils_Load_updateCurrentThreadTime__E", "ti_sysbios_utils_Load_updateThreadContexts__E", "ti_sysbios_utils_Load_reset__E", "ti_sysbios_utils_Load_getGlobalSwiLoad__E", "ti_sysbios_utils_Load_getGlobalHwiLoad__E", "ti_sysbios_utils_Load_getCPULoad__E", "ti_sysbios_utils_Load_calculateLoad__E", "ti_sysbios_utils_Load_setMinIdle__E", "ti_sysbios_utils_Load_addTask__E", "ti_sysbios_utils_Load_removeTask__E", "ti_sysbios_utils_Load_idleFxn__E", "ti_sysbios_utils_Load_idleFxnPwr__E", "ti_sysbios_utils_Load_startup__E", "ti_sysbios_utils_Load_taskCreateHook__E", "ti_sysbios_utils_Load_taskDeleteHook__E", "ti_sysbios_utils_Load_taskSwitchHook__E", "ti_sysbios_utils_Load_swiBeginHook__E", "ti_sysbios_utils_Load_swiEndHook__E", "ti_sysbios_utils_Load_hwiBeginHook__E", "ti_sysbios_utils_Load_hwiEndHook__E", "ti_sysbios_utils_Load_taskRegHook__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("LS_cpuLoad", "LS_hwiLoad", "LS_swiLoad", "LS_taskLoad"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./Load.xdt");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./Load.xdt");
        pkgV.bind("Load", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Load");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.utils.Load", "ti.sysbios.utils"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.utils.Load", "ti.sysbios.utils");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.utils.Load")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.utils")).add(pkgV);
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
        Load$$OBJECTS();
        Load$$CONSTS();
        Load$$CREATES();
        Load$$FUNCTIONS();
        Load$$SIZES();
        Load$$TYPES();
        if (isROV) {
            Load$$ROV();
        }//isROV
        $$SINGLETONS();
        Load$$SINGLETONS();
        $$INITIALIZATION();
    }
}
