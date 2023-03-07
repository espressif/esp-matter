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

public class ti_sysbios_productview
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
        Global.callFxn("loadPackage", xdcO, "xdc.tools.product");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.productview.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.productview", new Value.Obj("ti.sysbios.productview", pkgP));
    }

    void BiosProductView$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.productview.BiosProductView.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.productview.BiosProductView", new Value.Obj("ti.sysbios.productview.BiosProductView", po));
        pkgV.bind("BiosProductView", vo);
        // decls 
        om.bind("ti.sysbios.productview.BiosProductView.ProductElemDesc", om.findStrict("xdc.tools.product.IProductView.ProductElemDesc", "ti.sysbios.productview"));
    }

    void BiosProductView$$CONSTS()
    {
        // module BiosProductView
    }

    void BiosProductView$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void BiosProductView$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void BiosProductView$$SIZES()
    {
    }

    void BiosProductView$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/productview/BiosProductView.xs");
        om.bind("ti.sysbios.productview.BiosProductView$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.productview.BiosProductView.Module", "ti.sysbios.productview");
        po.init("ti.sysbios.productview.BiosProductView.Module", om.findStrict("xdc.tools.product.IProductView.Module", "ti.sysbios.productview"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("homeModule", $$T_Str, "ti.sysbios.BIOS", "wh");
        po.addFld("linksToArray", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"org.eclipse.rtsc.xdctools"}), "wh");
        po.addFld("ti_sysbios_BIOS", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_rom_ROM", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_cfg_Program", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_Defaults", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_System", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_Startup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_Main", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_SysMin", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_SysStd", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_Log", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_LoggerBuf", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_LoggerSys", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_Assert", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_Diags", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_Error", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_Timestamp", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_hal_Cache", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_Memory", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_HeapMin", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_heaps_HeapBuf", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_heaps_HeapCallback", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_heaps_HeapMem", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_heaps_HeapMultiBuf", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_heaps_HeapNull", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_heaps_HeapTrack", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_knl_Task", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_knl_Swi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_hal_Hwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_knl_Idle", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_knl_Clock", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_hal_Seconds", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_hal_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_knl_Semaphore", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_knl_Mailbox", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_knl_Queue", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_knl_Event", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_gates_GateAll", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_gates_GateHwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_gates_GateSwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_gates_GateTask", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_gates_GateMutex", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_gates_GateMutexPri", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_io_DEV", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_io_GIO", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("xdc_runtime_knl_SyncGeneric", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_syncs_SyncEvent", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_syncs_SyncSem", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_syncs_SyncSwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_utils_Load", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_hal_unicache_Cache", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_hal_ammu_AMMU", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_rts_gnu_SemiHostSupport", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_timers_dmtimer_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_timers_gptimer_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_timers_timer64_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_timers_timer64_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c28_Hwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c28_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c28_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c28_f28m35x_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c64p_Hwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c64p_Cache", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c64p_MemoryProtect", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c64p_EventCombiner", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c64p_Exception", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c64p_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c66_Cache", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c66_tci66xx_CpIntc", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_c674_Power", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_msp430_ClockFreqs", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_msp430_Hwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_msp430_Power", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_msp430_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_msp430_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_arm9_Cache", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_arm9_Mmu", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_dm6446_Hwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_da830_Hwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_da830_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_f28m35x_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_v7a_Pmu", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_a8_Cache", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_a8_Mmu", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_a8_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_a8_intcps_Hwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_a9_Cache", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_a9_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_a15_Cache", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_a15_Mmu", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_a15_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_a15_tci66xx_CpIntc", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_exc_Exception", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_gic_Hwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_systimer_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_m3_Hwi", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_m3_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_m3_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_lm3_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_lm3_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_lm4_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_lm4_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_ducati_Core", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_ducati_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_ducati_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_cc26xx_Alarm", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_cc26xx_Boot", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_cc26xx_Timer", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_cc26xx_Seconds", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ti_sysbios_family_arm_cc26xx_TimestampProvider", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("systemGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("diagnosticsGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("memoryGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("schedulingGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("synchronizationGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ioGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("targetGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("syncsGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("devDriversGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("gatesGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("heapsGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("loggersGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("rtaGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("systemProvidersGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("timersGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("c28Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("c64Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("c64pGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("c64tGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("c66Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("c674Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("msp430Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("armGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("arm9davinciGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("arm9da830Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("arm9Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("a8Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("a9Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("a15Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("m3Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("lm3Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("lm4Group", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("ducatiGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("cc26xxGroup", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        po.addFld("biosProduct", (Proto)om.findStrict("ti.sysbios.productview.BiosProductView.ProductElemDesc", "ti.sysbios.productview"), $$DEFAULT, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.productview.BiosProductView$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.productview.BiosProductView$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.productview.BiosProductView$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "getProductDescriptor");
                if (fxn != null) po.addFxn("getProductDescriptor", (Proto.Fxn)om.findStrict("xdc.tools.product.IProductView$$getProductDescriptor", "ti.sysbios.productview"), fxn);
    }

    void BiosProductView$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.productview.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.productview"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.sysbios.productview", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.productview");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.productview.");
        pkgV.bind("$vers", Global.newArray());
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.productview'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.productview$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.productview$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.productview$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void BiosProductView$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.productview.BiosProductView", "ti.sysbios.productview");
        po = (Proto.Obj)om.findStrict("ti.sysbios.productview.BiosProductView.Module", "ti.sysbios.productview");
        vo.init2(po, "ti.sysbios.productview.BiosProductView", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.productview.BiosProductView$$capsule", "ti.sysbios.productview"));
        vo.bind("$package", om.findStrict("ti.sysbios.productview", "ti.sysbios.productview"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("ProductElemDesc", om.findStrict("xdc.tools.product.IProductView.ProductElemDesc", "ti.sysbios.productview"));
        tdefs.add(om.findStrict("xdc.tools.product.IProductView.ProductElemDesc", "ti.sysbios.productview"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.tools.product");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.productview")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.sysbios.productview.BiosProductView$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("BiosProductView", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("BiosProductView");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.productview.BiosProductView", "ti.sysbios.productview"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.productview.BiosProductView")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.productview")).add(pkgV);
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
        BiosProductView$$OBJECTS();
        BiosProductView$$CONSTS();
        BiosProductView$$CREATES();
        BiosProductView$$FUNCTIONS();
        BiosProductView$$SIZES();
        BiosProductView$$TYPES();
        if (isROV) {
            BiosProductView$$ROV();
        }//isROV
        $$SINGLETONS();
        BiosProductView$$SINGLETONS();
        $$INITIALIZATION();
    }
}
