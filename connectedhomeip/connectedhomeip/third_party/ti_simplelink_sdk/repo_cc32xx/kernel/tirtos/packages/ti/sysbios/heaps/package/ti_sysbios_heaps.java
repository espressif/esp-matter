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

public class ti_sysbios_heaps
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
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.interfaces");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.heaps.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.heaps", new Value.Obj("ti.sysbios.heaps", pkgP));
    }

    void HeapBuf$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapBuf.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.heaps.HeapBuf", new Value.Obj("ti.sysbios.heaps.HeapBuf", po));
        pkgV.bind("HeapBuf", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapBuf$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapBuf.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapBuf$$DetailedView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapBuf.DetailedView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapBuf$$ExtendedStats", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapBuf.ExtendedStats", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapBuf$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapBuf.Instance_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapBuf$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapBuf.Module_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.heaps.HeapBuf.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapBuf$$Object", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapBuf.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapBuf$$Params", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapBuf.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.heaps.HeapBuf.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.heaps.HeapBuf.Object", om.findStrict("ti.sysbios.heaps.HeapBuf.Instance_State", "ti.sysbios.heaps"));
        }//isROV
    }

    void HeapMem$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.heaps.HeapMem", new Value.Obj("ti.sysbios.heaps.HeapMem", po));
        pkgV.bind("HeapMem", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem$$DetailedView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem.DetailedView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem$$FreeBlockView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem.FreeBlockView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem$$ExtendedStats", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem.ExtendedStats", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem$$Header", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem.Header", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.heaps.HeapMem.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem$$Object", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem$$Params", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.heaps.HeapMem.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.heaps.HeapMem.Object", om.findStrict("ti.sysbios.heaps.HeapMem.Instance_State", "ti.sysbios.heaps"));
        }//isROV
    }

    void HeapMultiBuf$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMultiBuf.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.heaps.HeapMultiBuf", new Value.Obj("ti.sysbios.heaps.HeapMultiBuf", po));
        pkgV.bind("HeapMultiBuf", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMultiBuf$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMultiBuf.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMultiBuf$$DetailedView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMultiBuf.DetailedView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMultiBuf$$AddrPair", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMultiBuf.AddrPair", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMultiBuf$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMultiBuf.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.heaps.HeapMultiBuf.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMultiBuf$$Object", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMultiBuf.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMultiBuf$$Params", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMultiBuf.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.heaps.HeapMultiBuf.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.heaps.HeapMultiBuf.Object", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Instance_State", "ti.sysbios.heaps"));
        }//isROV
    }

    void HeapNull$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapNull.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.heaps.HeapNull", new Value.Obj("ti.sysbios.heaps.HeapNull", po));
        pkgV.bind("HeapNull", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapNull$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapNull.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.heaps.HeapNull.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapNull$$Object", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapNull.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapNull$$Params", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapNull.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapNull$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapNull.Instance_State", new Proto.Str(po, false));
        om.bind("ti.sysbios.heaps.HeapNull.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.heaps.HeapNull.Object", om.findStrict("ti.sysbios.heaps.HeapNull.Instance_State", "ti.sysbios.heaps"));
        }//isROV
    }

    void HeapTrack$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapTrack.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.heaps.HeapTrack", new Value.Obj("ti.sysbios.heaps.HeapTrack", po));
        pkgV.bind("HeapTrack", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapTrack$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapTrack.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapTrack$$TaskView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapTrack.TaskView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapTrack$$HeapListView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapTrack.HeapListView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapTrack$$Tracker", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapTrack.Tracker", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapTrack$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapTrack.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.heaps.HeapTrack.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapTrack$$Object", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapTrack.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapTrack$$Params", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapTrack.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.heaps.HeapTrack.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.heaps.HeapTrack.Object", om.findStrict("ti.sysbios.heaps.HeapTrack.Instance_State", "ti.sysbios.heaps"));
        }//isROV
    }

    void HeapCallback$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapCallback.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.heaps.HeapCallback", new Value.Obj("ti.sysbios.heaps.HeapCallback", po));
        pkgV.bind("HeapCallback", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapCallback$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapCallback.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapCallback$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapCallback.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapCallback$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapCallback.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.heaps.HeapCallback.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapCallback$$Object", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapCallback.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapCallback$$Params", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapCallback.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.heaps.HeapCallback.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.heaps.HeapCallback.Object", om.findStrict("ti.sysbios.heaps.HeapCallback.Instance_State", "ti.sysbios.heaps"));
        }//isROV
    }

    void HeapMem_Module_GateProxy$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy", new Value.Obj("ti.sysbios.heaps.HeapMem_Module_GateProxy", po));
        pkgV.bind("HeapMem_Module_GateProxy", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy$$Object", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy$$Params", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance_State", new Proto.Str(po, false));
        om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy.Object", om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance_State", "ti.sysbios.heaps"));
        }//isROV
    }

    void HeapBuf$$CONSTS()
    {
        // module HeapBuf
        om.bind("ti.sysbios.heaps.HeapBuf.postInit", new Extern("ti_sysbios_heaps_HeapBuf_postInit__I", "xdc_Void(*)(ti_sysbios_heaps_HeapBuf_Object*)", true, false));
    }

    void HeapMem$$CONSTS()
    {
        // module HeapMem
        om.bind("ti.sysbios.heaps.HeapMem.enter", new Extern("ti_sysbios_heaps_HeapMem_enter__E", "xdc_IArg(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.heaps.HeapMem.leave", new Extern("ti_sysbios_heaps_HeapMem_leave__E", "xdc_Void(*)(xdc_IArg)", true, false));
        om.bind("ti.sysbios.heaps.HeapMem.init", new Extern("ti_sysbios_heaps_HeapMem_init__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.heaps.HeapMem.initPrimary", new Extern("ti_sysbios_heaps_HeapMem_initPrimary__I", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void HeapMultiBuf$$CONSTS()
    {
        // module HeapMultiBuf
        om.bind("ti.sysbios.heaps.HeapMultiBuf.addrPairCompare", new Extern("ti_sysbios_heaps_HeapMultiBuf_addrPairCompare__I", "xdc_Int(*)(xdc_Void*,xdc_Void*)", true, false));
        om.bind("ti.sysbios.heaps.HeapMultiBuf.sizeAlignCompare", new Extern("ti_sysbios_heaps_HeapMultiBuf_sizeAlignCompare__I", "xdc_Int(*)(xdc_Void*,xdc_Void*)", true, false));
        om.bind("ti.sysbios.heaps.HeapMultiBuf.borrowBlock", new Extern("ti_sysbios_heaps_HeapMultiBuf_borrowBlock__I", "xdc_Void*(*)(ti_sysbios_heaps_HeapMultiBuf_Object*,xdc_SizeT,xdc_SizeT,xdc_Int)", true, false));
    }

    void HeapNull$$CONSTS()
    {
        // module HeapNull
    }

    void HeapTrack$$CONSTS()
    {
        // module HeapTrack
        om.bind("ti.sysbios.heaps.HeapTrack.STARTSCRIBBLE", 0xa5a5a5a5L);
        om.bind("ti.sysbios.heaps.HeapTrack.NOSCRIBBLE", 0x05101920L);
        om.bind("ti.sysbios.heaps.HeapTrack.printTask", new Extern("ti_sysbios_heaps_HeapTrack_printTask__E", "xdc_Void(*)(ti_sysbios_knl_Task_Handle)", true, false));
        om.bind("ti.sysbios.heaps.HeapTrack.printTrack", new Extern("ti_sysbios_heaps_HeapTrack_printTrack__I", "xdc_Bool(*)(ti_sysbios_heaps_HeapTrack_Tracker*,ti_sysbios_heaps_HeapTrack_Handle)", true, false));
    }

    void HeapCallback$$CONSTS()
    {
        // module HeapCallback
    }

    void HeapMem_Module_GateProxy$$CONSTS()
    {
        // module HeapMem_Module_GateProxy
        om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy.query", new Extern("ti_sysbios_heaps_HeapMem_Module_GateProxy_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void HeapBuf$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapBuf$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.heaps.HeapBuf.create() called before xdc.useModule('ti.sysbios.heaps.HeapBuf')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapBuf$$create", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapBuf.Module", "ti.sysbios.heaps"), om.findStrict("ti.sysbios.heaps.HeapBuf.Instance", "ti.sysbios.heaps"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapBuf.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapBuf$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapBuf'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.heaps.HeapBuf.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.heaps']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.heaps.HeapBuf'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapBuf'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapBuf$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapBuf.Module", "ti.sysbios.heaps"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.heaps.HeapBuf$$Object", "ti.sysbios.heaps"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapBuf.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapBuf$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapBuf'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.heaps.HeapBuf'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapBuf'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void HeapMem$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapMem$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.heaps.HeapMem.create() called before xdc.useModule('ti.sysbios.heaps.HeapMem')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapMem$$create", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapMem.Module", "ti.sysbios.heaps"), om.findStrict("ti.sysbios.heaps.HeapMem.Instance", "ti.sysbios.heaps"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapMem.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapMem$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapMem'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.heaps.HeapMem.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.heaps']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.heaps.HeapMem'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapMem'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapMem$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapMem.Module", "ti.sysbios.heaps"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.heaps.HeapMem$$Object", "ti.sysbios.heaps"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapMem.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapMem$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapMem'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.heaps.HeapMem'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapMem'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void HeapMultiBuf$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapMultiBuf$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.heaps.HeapMultiBuf.create() called before xdc.useModule('ti.sysbios.heaps.HeapMultiBuf')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapMultiBuf$$create", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Module", "ti.sysbios.heaps"), om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Instance", "ti.sysbios.heaps"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapMultiBuf$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapMultiBuf'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.heaps.HeapMultiBuf.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.heaps']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.heaps.HeapMultiBuf'].Instance_State);\n");
                sb.append("__inst.bufParams = __mod.PARAMS.bufParams;\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapMultiBuf'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapMultiBuf$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Module", "ti.sysbios.heaps"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$Object", "ti.sysbios.heaps"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapMultiBuf$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapMultiBuf'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.heaps.HeapMultiBuf'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("__inst.bufParams = __mod.PARAMS.bufParams;\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapMultiBuf'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void HeapNull$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapNull$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.heaps.HeapNull.create() called before xdc.useModule('ti.sysbios.heaps.HeapNull')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapNull$$create", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapNull.Module", "ti.sysbios.heaps"), om.findStrict("ti.sysbios.heaps.HeapNull.Instance", "ti.sysbios.heaps"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapNull.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapNull$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapNull'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.heaps.HeapNull.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.heaps']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.heaps.HeapNull'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapNull'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapNull$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapNull.Module", "ti.sysbios.heaps"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.heaps.HeapNull$$Object", "ti.sysbios.heaps"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapNull.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapNull$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapNull'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.heaps.HeapNull'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapNull'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void HeapTrack$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapTrack$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.heaps.HeapTrack.create() called before xdc.useModule('ti.sysbios.heaps.HeapTrack')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapTrack$$create", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapTrack.Module", "ti.sysbios.heaps"), om.findStrict("ti.sysbios.heaps.HeapTrack.Instance", "ti.sysbios.heaps"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapTrack.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapTrack$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapTrack'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.heaps.HeapTrack.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.heaps']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.heaps.HeapTrack'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapTrack'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapTrack$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapTrack.Module", "ti.sysbios.heaps"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.heaps.HeapTrack$$Object", "ti.sysbios.heaps"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapTrack.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapTrack$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapTrack'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.heaps.HeapTrack'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapTrack'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void HeapCallback$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapCallback$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.heaps.HeapCallback.create() called before xdc.useModule('ti.sysbios.heaps.HeapCallback')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapCallback$$create", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapCallback.Module", "ti.sysbios.heaps"), om.findStrict("ti.sysbios.heaps.HeapCallback.Instance", "ti.sysbios.heaps"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapCallback.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapCallback$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapCallback'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.heaps.HeapCallback.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.heaps']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.heaps.HeapCallback'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapCallback'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapCallback$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapCallback.Module", "ti.sysbios.heaps"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.heaps.HeapCallback$$Object", "ti.sysbios.heaps"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapCallback.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapCallback$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapCallback'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.heaps.HeapCallback'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapCallback'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void HeapMem_Module_GateProxy$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapMem_Module_GateProxy$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.heaps.HeapMem_Module_GateProxy.create() called before xdc.useModule('ti.sysbios.heaps.HeapMem_Module_GateProxy')\");\n");
                sb.append("}\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.heaps.HeapMem_Module_GateProxy$$create", new Proto.Fxn(om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Module", "ti.sysbios.heaps"), om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance", "ti.sysbios.heaps"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Params", "ti.sysbios.heaps"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$heaps$HeapMem_Module_GateProxy$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.heaps.HeapMem_Module_GateProxy'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.heaps']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.heaps.HeapMem_Module_GateProxy'].Instance_State);\n");
                sb.append("if (!__mod.delegate$) {\n");
                    sb.append("throw new Error(\"Unbound proxy module: ti.sysbios.heaps.HeapMem_Module_GateProxy\");\n");
                sb.append("}\n");
                sb.append("var __dmod = __mod.delegate$.$orig;\n");
                sb.append("var __dinst = __dmod.create(__params);\n");
                sb.append("__inst.$$bind('delegate$', __dinst);\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.heaps.HeapMem_Module_GateProxy'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void HeapBuf$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void HeapMem$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void HeapMultiBuf$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void HeapNull$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void HeapTrack$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void HeapCallback$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void HeapMem_Module_GateProxy$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void HeapBuf$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapBuf.ExtendedStats", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("maxAllocatedBlocks", "UInt"));
        sizes.add(Global.newArray("numAllocatedBlocks", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapBuf.ExtendedStats']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapBuf.ExtendedStats']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapBuf.ExtendedStats'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapBuf.Instance_State", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("blockSize", "USize"));
        sizes.add(Global.newArray("align", "USize"));
        sizes.add(Global.newArray("numBlocks", "UInt"));
        sizes.add(Global.newArray("bufSize", "UIArg"));
        sizes.add(Global.newArray("buf", "UPtr"));
        sizes.add(Global.newArray("numFreeBlocks", "UInt"));
        sizes.add(Global.newArray("minFreeBlocks", "UInt"));
        sizes.add(Global.newArray("freeList", "Sti.sysbios.knl.Queue;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapBuf.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapBuf.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapBuf.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapBuf.Module_State", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("constructedHeaps", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapBuf.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapBuf.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapBuf.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void HeapMem$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapMem.ExtendedStats", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("buf", "UPtr"));
        sizes.add(Global.newArray("size", "USize"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapMem.ExtendedStats']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapMem.ExtendedStats']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapMem.ExtendedStats'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapMem.Header", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("next", "UPtr"));
        sizes.add(Global.newArray("size", "UIArg"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapMem.Header']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapMem.Header']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapMem.Header'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapMem.Instance_State", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("align", "UIArg"));
        sizes.add(Global.newArray("buf", "UPtr"));
        sizes.add(Global.newArray("head", "Sti.sysbios.heaps.HeapMem;Header"));
        sizes.add(Global.newArray("minBlockAlign", "USize"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapMem.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapMem.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapMem.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void HeapMultiBuf$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapMultiBuf.AddrPair", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("heapBuf", "UPtr"));
        sizes.add(Global.newArray("lastAddr", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapMultiBuf.AddrPair']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapMultiBuf.AddrPair']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapMultiBuf.AddrPair'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Instance_State", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("blockBorrow", "UShort"));
        sizes.add(Global.newArray("numHeapBufs", "TInt"));
        sizes.add(Global.newArray("bufsBySize", "UPtr"));
        sizes.add(Global.newArray("numBufs", "TInt"));
        sizes.add(Global.newArray("bufsByAddr", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapMultiBuf.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapMultiBuf.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapMultiBuf.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void HeapNull$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapNull.Instance_State", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapNull.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapNull.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapNull.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void HeapTrack$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapTrack.Tracker", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("scribble", "UIArg"));
        sizes.add(Global.newArray("queElem", "Sti.sysbios.knl.Queue;Elem"));
        sizes.add(Global.newArray("size", "USize"));
        sizes.add(Global.newArray("tick", "UInt32"));
        sizes.add(Global.newArray("taskHandle", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapTrack.Tracker']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapTrack.Tracker']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapTrack.Tracker'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapTrack.Instance_State", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("internalHeap", "UPtr"));
        sizes.add(Global.newArray("size", "USize"));
        sizes.add(Global.newArray("peak", "USize"));
        sizes.add(Global.newArray("sizeWithoutTracker", "USize"));
        sizes.add(Global.newArray("peakWithoutTracker", "USize"));
        sizes.add(Global.newArray("trackQueue", "Sti.sysbios.knl.Queue;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapTrack.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapTrack.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapTrack.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void HeapCallback$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.heaps.HeapCallback.Instance_State", "ti.sysbios.heaps");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("context", "UIArg"));
        sizes.add(Global.newArray("arg", "UIArg"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.heaps.HeapCallback.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.heaps.HeapCallback.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.heaps.HeapCallback.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void HeapMem_Module_GateProxy$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void HeapBuf$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/heaps/HeapBuf.xs");
        om.bind("ti.sysbios.heaps.HeapBuf$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf.Module", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapBuf.Module", om.findStrict("xdc.runtime.IHeap.Module", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.heaps"), $$UNDEF, "wh");
            po.addFld("A_nullBuf", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "buf parameter cannot be null"), "w");
            po.addFld("A_bufAlign", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "buf not properly aligned"), "w");
            po.addFld("A_invalidAlign", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "align parameter must be 0 or a power of 2 >= the value of Memory_getMaxDefaultTypeAlign()"), "w");
            po.addFld("A_invalidRequestedAlign", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "align parameter 1) must be 0 or a power of 2 and 2) not greater than the heaps alignment"), "w");
            po.addFld("A_invalidBlockSize", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "blockSize must be large enough to hold atleast two pointers"), "w");
            po.addFld("A_zeroBlocks", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "numBlocks cannot be zero"), "w");
            po.addFld("A_zeroBufSize", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "bufSize cannot be zero"), "w");
            po.addFld("A_invalidBufSize", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "HeapBuf_create's bufSize parameter is invalid (too small)"), "w");
            po.addFld("A_noBlocksToFree", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "Cannot call HeapBuf_free when no blocks have been allocated"), "w");
            po.addFld("A_invalidFree", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "A_invalidFree: Invalid free"), "w");
            po.addFld("E_size", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "requested size is too big: handle=0x%x, size=%u"), "w");
            po.addFld("trackMaxAllocs", $$T_Bool, false, "w");
            po.addFld("numConstructedHeaps", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapBuf$$create", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapBuf$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapBuf$$construct", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapBuf$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapBuf$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapBuf$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapBuf$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapBuf$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapBuf$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapBuf$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf.Instance", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapBuf.Instance", om.findStrict("xdc.runtime.IHeap.Instance", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("align", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("sectionName", $$T_Str, null, "wh");
            po.addFld("numBlocks", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
            po.addFld("blockSize", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("bufSize", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$Params", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapBuf.Params", om.findStrict("xdc.runtime.IHeap$$Params", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("align", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("sectionName", $$T_Str, null, "wh");
            po.addFld("numBlocks", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
            po.addFld("blockSize", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("bufSize", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$Object", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapBuf.Object", om.findStrict("ti.sysbios.heaps.HeapBuf.Instance", "ti.sysbios.heaps"));
        // struct HeapBuf.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$BasicView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapBuf.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("bufSize", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("blockSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("numBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("totalFreeSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numberAllocatedBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("numFreeBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("maxAllocatedBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("statStr", $$T_Str, $$UNDEF, "w");
        // struct HeapBuf.DetailedView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$DetailedView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapBuf.DetailedView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("bufSize", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("blockSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("numBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("totalFreeSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numberAllocatedBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("numFreeBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("maxAllocatedBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("freeList", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("statStr", $$T_Str, $$UNDEF, "w");
        // struct HeapBuf.ExtendedStats
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$ExtendedStats", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapBuf.ExtendedStats", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("maxAllocatedBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("numAllocatedBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct HeapBuf.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$Instance_State", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapBuf.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("blockSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("align", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("numBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("bufSize", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("buf", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("numFreeBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("minFreeBlocks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFldV("freeList", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.heaps"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_freeList", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.heaps"), $$DEFAULT, "w");
        // struct HeapBuf.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$Module_State", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapBuf.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("constructedHeaps", new Proto.Arr((Proto)om.findStrict("ti.sysbios.heaps.HeapBuf.Handle", "ti.sysbios.heaps"), false), $$DEFAULT, "w");
    }

    void HeapMem$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/heaps/HeapMem.xs");
        om.bind("ti.sysbios.heaps.HeapMem$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem.Module", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem.Module", om.findStrict("xdc.runtime.IHeap.Module", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.heaps"), $$UNDEF, "wh");
            po.addFld("A_zeroBlock", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "A_zeroBlock: Cannot allocate size 0"), "w");
            po.addFld("A_heapSize", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "A_heapSize: Requested heap size is too small"), "w");
            po.addFld("A_align", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "A_align: Requested align is not a power of 2"), "w");
            po.addFld("E_memory", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "out of memory: handle=0x%x, size=%u"), "w");
            po.addFld("A_invalidFree", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "A_invalidFree: Invalid free"), "w");
            po.addFld("primaryHeapBaseAddr", new Proto.Adr("xdc_Char*", "Pn"), null, "w");
            po.addFld("primaryHeapEndAddr", new Proto.Adr("xdc_Char*", "Pn"), null, "w");
            po.addFld("reqAlign", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
            po.addFldV("Module_GateProxy", (Proto)om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.heaps"), null, "wh", $$delegGet, $$delegSet);
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapMem$$create", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapMem$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapMem$$construct", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapMem$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMem$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMem$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMem$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMem$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMem$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMem$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem.Instance", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem.Instance", om.findStrict("xdc.runtime.IHeap.Instance", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("usePrimaryHeap", $$T_Bool, false, "wh");
            po.addFld("align", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "wh");
            po.addFld("minBlockAlign", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("sectionName", $$T_Str, null, "wh");
            po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), 0L, "w");
            po.addFld("size", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$Params", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem.Params", om.findStrict("xdc.runtime.IHeap$$Params", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("usePrimaryHeap", $$T_Bool, false, "wh");
            po.addFld("align", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "wh");
            po.addFld("minBlockAlign", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("sectionName", $$T_Str, null, "wh");
            po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), 0L, "w");
            po.addFld("size", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$Object", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem.Object", om.findStrict("ti.sysbios.heaps.HeapMem.Instance", "ti.sysbios.heaps"));
        // struct HeapMem.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$BasicView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("minBlockAlign", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("sectionName", $$T_Str, $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // struct HeapMem.DetailedView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$DetailedView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem.DetailedView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("minBlockAlign", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("sectionName", $$T_Str, $$UNDEF, "w");
                po.addFld("totalSize", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("totalFreeSize", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("largestFreeSize", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // struct HeapMem.FreeBlockView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$FreeBlockView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem.FreeBlockView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("Address", $$T_Str, $$UNDEF, "w");
                po.addFld("size", $$T_Str, $$UNDEF, "w");
                po.addFld("next", $$T_Str, $$UNDEF, "w");
                po.addFld("status", $$T_Str, $$UNDEF, "w");
        // struct HeapMem.ExtendedStats
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$ExtendedStats", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem.ExtendedStats", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("size", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
        // struct HeapMem.Header
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$Header", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem.Header", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("next", new Proto.Adr("ti_sysbios_heaps_HeapMem_Header*", "PS"), $$UNDEF, "w");
                po.addFld("size", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
        // struct HeapMem.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$Instance_State", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("align", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("buf", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("head", (Proto)om.findStrict("ti.sysbios.heaps.HeapMem.Header", "ti.sysbios.heaps"), $$DEFAULT, "w");
                po.addFld("minBlockAlign", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
    }

    void HeapMultiBuf$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/heaps/HeapMultiBuf.xs");
        om.bind("ti.sysbios.heaps.HeapMultiBuf$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Module", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMultiBuf.Module", om.findStrict("xdc.runtime.IHeap.Module", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.heaps"), $$UNDEF, "wh");
            po.addFld("A_blockNotFreed", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "Invalid block address on the free. Failed to free block back to heap."), "w");
            po.addFld("E_size", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "requested size is too big: handle=0x%x, size=%u"), "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$create", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapMultiBuf$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$construct", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapMultiBuf$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMultiBuf$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMultiBuf$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMultiBuf$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMultiBuf$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMultiBuf$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapMultiBuf$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Instance", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMultiBuf.Instance", om.findStrict("xdc.runtime.IHeap.Instance", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("numBufs", Proto.Elm.newCNum("(xdc_Int)"), 0L, "w");
            po.addFld("blockBorrow", $$T_Bool, false, "w");
            po.addFld("bufParams", new Proto.Arr((Proto)om.findStrict("ti.sysbios.heaps.HeapBuf.Params", "ti.sysbios.heaps"), false), $$DEFAULT, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$Params", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMultiBuf.Params", om.findStrict("xdc.runtime.IHeap$$Params", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("numBufs", Proto.Elm.newCNum("(xdc_Int)"), 0L, "w");
            po.addFld("blockBorrow", $$T_Bool, false, "w");
            po.addFld("bufParams", new Proto.Arr((Proto)om.findStrict("ti.sysbios.heaps.HeapBuf.Params", "ti.sysbios.heaps"), false), $$DEFAULT, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$Object", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMultiBuf.Object", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Instance", "ti.sysbios.heaps"));
        // struct HeapMultiBuf.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$BasicView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMultiBuf.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("blockBorrow", $$T_Bool, $$UNDEF, "w");
                po.addFld("numHeapBufs", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // struct HeapMultiBuf.DetailedView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$DetailedView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMultiBuf.DetailedView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("blockBorrow", $$T_Bool, $$UNDEF, "w");
                po.addFld("numHeapBufs", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("HeapBufHandles", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
        // struct HeapMultiBuf.AddrPair
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$AddrPair", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMultiBuf.AddrPair", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("heapBuf", (Proto)om.findStrict("ti.sysbios.heaps.HeapBuf.Handle", "ti.sysbios.heaps"), $$UNDEF, "w");
                po.addFld("lastAddr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        // struct HeapMultiBuf.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$Instance_State", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMultiBuf.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("blockBorrow", $$T_Bool, $$UNDEF, "w");
                po.addFld("numHeapBufs", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("bufsBySize", new Proto.Arr((Proto)om.findStrict("ti.sysbios.heaps.HeapBuf.Handle", "ti.sysbios.heaps"), false), $$DEFAULT, "w");
                po.addFld("numBufs", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("bufsByAddr", new Proto.Arr((Proto)om.findStrict("ti.sysbios.heaps.HeapMultiBuf.AddrPair", "ti.sysbios.heaps"), false), $$DEFAULT, "w");
    }

    void HeapNull$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/heaps/HeapNull.xs");
        om.bind("ti.sysbios.heaps.HeapNull$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapNull.Module", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapNull.Module", om.findStrict("xdc.runtime.IHeap.Module", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapNull$$create", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapNull$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapNull$$construct", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapNull$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapNull$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapNull$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapNull$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapNull$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapNull$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapNull$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapNull.Instance", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapNull.Instance", om.findStrict("xdc.runtime.IHeap.Instance", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapNull$$Params", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapNull.Params", om.findStrict("xdc.runtime.IHeap$$Params", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapNull$$Object", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapNull.Object", om.findStrict("ti.sysbios.heaps.HeapNull.Instance", "ti.sysbios.heaps"));
        // struct HeapNull.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapNull$$Instance_State", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapNull.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapNull$$Instance_State", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapNull.Instance_State", null);
        po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void HeapTrack$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/heaps/HeapTrack.xs");
        om.bind("ti.sysbios.heaps.HeapTrack$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack.Module", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapTrack.Module", om.findStrict("xdc.runtime.IHeap.Module", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("STARTSCRIBBLE", new Proto.Adr("xdc_UArg", "Pv"), 0xa5a5a5a5L, "rh");
                po.addFld("NOSCRIBBLE", new Proto.Adr("xdc_UArg", "Pv"), 0x05101920L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.heaps"), $$UNDEF, "wh");
            po.addFld("A_doubleFree", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "A_doubleFree: Buffer already free"), "w");
            po.addFld("A_bufOverflow", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "A_bufOverflow: Buffer overflow"), "w");
            po.addFld("A_notEmpty", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "A_notEmpty: Heap not empty"), "w");
            po.addFld("A_nullObject", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.heaps"), Global.newObject("msg", "A_nullObject: HeapTrack_printHeap called with null obj"), "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapTrack$$create", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapTrack$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapTrack$$construct", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapTrack$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapTrack$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapTrack$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapTrack$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapTrack$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapTrack$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapTrack$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack.Instance", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapTrack.Instance", om.findStrict("xdc.runtime.IHeap.Instance", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("STARTSCRIBBLE", new Proto.Adr("xdc_UArg", "Pv"), 0xa5a5a5a5L, "rh");
                po.addFld("NOSCRIBBLE", new Proto.Adr("xdc_UArg", "Pv"), 0x05101920L, "rh");
        if (isCFG) {
            po.addFld("heap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.heaps"), null, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack$$Params", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapTrack.Params", om.findStrict("xdc.runtime.IHeap$$Params", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("STARTSCRIBBLE", new Proto.Adr("xdc_UArg", "Pv"), 0xa5a5a5a5L, "rh");
                po.addFld("NOSCRIBBLE", new Proto.Adr("xdc_UArg", "Pv"), 0x05101920L, "rh");
        if (isCFG) {
            po.addFld("heap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.heaps"), null, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack$$Object", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapTrack.Object", om.findStrict("ti.sysbios.heaps.HeapTrack.Instance", "ti.sysbios.heaps"));
        // struct HeapTrack.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack$$BasicView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapTrack.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("heapHandle", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.heaps"), $$UNDEF, "w");
                po.addFld("inUse", $$T_Str, $$UNDEF, "w");
                po.addFld("inUsePeak", $$T_Str, $$UNDEF, "w");
                po.addFld("inUseWithoutTracker", $$T_Str, $$UNDEF, "w");
                po.addFld("inUsePeakWithoutTracker", $$T_Str, $$UNDEF, "w");
        // struct HeapTrack.TaskView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack$$TaskView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapTrack.TaskView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("block", $$T_Str, $$UNDEF, "w");
                po.addFld("heapHandle", $$T_Str, $$UNDEF, "w");
                po.addFld("blockAddr", $$T_Str, $$UNDEF, "w");
                po.addFld("requestedSize", $$T_Str, $$UNDEF, "w");
                po.addFld("clockTick", $$T_Str, $$UNDEF, "w");
                po.addFld("overflow", $$T_Str, $$UNDEF, "w");
        // struct HeapTrack.HeapListView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack$$HeapListView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapTrack.HeapListView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("block", $$T_Str, $$UNDEF, "w");
                po.addFld("taskHandle", $$T_Str, $$UNDEF, "w");
                po.addFld("heapHandle", $$T_Str, $$UNDEF, "w");
                po.addFld("blockAddr", $$T_Str, $$UNDEF, "w");
                po.addFld("requestedSize", $$T_Str, $$UNDEF, "w");
                po.addFld("clockTick", $$T_Str, $$UNDEF, "w");
                po.addFld("overflow", $$T_Str, $$UNDEF, "w");
        // struct HeapTrack.Tracker
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack$$Tracker", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapTrack.Tracker", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("scribble", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("queElem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.heaps"), $$DEFAULT, "w");
                po.addFld("size", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("tick", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("taskHandle", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.heaps"), $$UNDEF, "w");
        // struct HeapTrack.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack$$Instance_State", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapTrack.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("internalHeap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.heaps"), $$UNDEF, "w");
                po.addFld("size", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("peak", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("sizeWithoutTracker", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("peakWithoutTracker", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFldV("trackQueue", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.heaps"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_trackQueue", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.heaps"), $$DEFAULT, "w");
    }

    void HeapCallback$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/heaps/HeapCallback.xs");
        om.bind("ti.sysbios.heaps.HeapCallback$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback.Module", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapCallback.Module", om.findStrict("xdc.runtime.IHeap.Module", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.heaps"), $$UNDEF, "wh");
            po.addFld("allocInstFxn", new Proto.Adr("xdc_Ptr(*)(xdc_UArg,xdc_SizeT,xdc_SizeT)", "PFPv"), "&ti_sysbios_heaps_HeapCallback_defaultAlloc", "w");
            po.addFld("createInstFxn", new Proto.Adr("xdc_UArg(*)(xdc_UArg)", "PFPv"), "&ti_sysbios_heaps_HeapCallback_defaultCreate", "w");
            po.addFld("deleteInstFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), "&ti_sysbios_heaps_HeapCallback_defaultDelete", "w");
            po.addFld("freeInstFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_Ptr,xdc_SizeT)", "PFv"), "&ti_sysbios_heaps_HeapCallback_defaultFree", "w");
            po.addFld("getStatsInstFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_runtime_Memory_Stats*)", "PFv"), "&ti_sysbios_heaps_HeapCallback_defaultGetStats", "w");
            po.addFld("initInstFxn", new Proto.Adr("xdc_UArg(*)(xdc_UArg)", "PFPv"), "&ti_sysbios_heaps_HeapCallback_defaultInit", "w");
            po.addFld("isBlockingInstFxn", new Proto.Adr("xdc_Bool(*)(xdc_UArg)", "PFb"), "&ti_sysbios_heaps_HeapCallback_defaultIsBlocking", "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapCallback$$create", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapCallback$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapCallback$$construct", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapCallback$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapCallback$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapCallback$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapCallback$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapCallback$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapCallback$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.heaps.HeapCallback$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback.Instance", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapCallback.Instance", om.findStrict("xdc.runtime.IHeap.Instance", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback$$Params", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapCallback.Params", om.findStrict("xdc.runtime.IHeap$$Params", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback$$Object", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapCallback.Object", om.findStrict("ti.sysbios.heaps.HeapCallback.Instance", "ti.sysbios.heaps"));
        // struct HeapCallback.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback$$BasicView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapCallback.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("context", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
        // struct HeapCallback.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback$$ModuleView", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapCallback.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("initInstFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("createInstFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("deleteInstFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("allocInstFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("freeInstFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("getStatsInstFxn", $$T_Str, $$UNDEF, "w");
                po.addFld("isBlockingInstFxn", $$T_Str, $$UNDEF, "w");
        // typedef HeapCallback.AllocInstFxn
        om.bind("ti.sysbios.heaps.HeapCallback.AllocInstFxn", new Proto.Adr("xdc_Ptr(*)(xdc_UArg,xdc_SizeT,xdc_SizeT)", "PFPv"));
        // typedef HeapCallback.CreateInstFxn
        om.bind("ti.sysbios.heaps.HeapCallback.CreateInstFxn", new Proto.Adr("xdc_UArg(*)(xdc_UArg)", "PFPv"));
        // typedef HeapCallback.DeleteInstFxn
        om.bind("ti.sysbios.heaps.HeapCallback.DeleteInstFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"));
        // typedef HeapCallback.FreeInstFxn
        om.bind("ti.sysbios.heaps.HeapCallback.FreeInstFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_Ptr,xdc_SizeT)", "PFv"));
        // typedef HeapCallback.GetStatsInstFxn
        om.bind("ti.sysbios.heaps.HeapCallback.GetStatsInstFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_runtime_Memory_Stats*)", "PFv"));
        // typedef HeapCallback.InitInstFxn
        om.bind("ti.sysbios.heaps.HeapCallback.InitInstFxn", new Proto.Adr("xdc_UArg(*)(xdc_UArg)", "PFPv"));
        // typedef HeapCallback.IsBlockingInstFxn
        om.bind("ti.sysbios.heaps.HeapCallback.IsBlockingInstFxn", new Proto.Adr("xdc_Bool(*)(xdc_UArg)", "PFb"));
        // struct HeapCallback.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback$$Instance_State", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapCallback.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("context", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
    }

    void HeapMem_Module_GateProxy$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Module", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem_Module_GateProxy.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.heaps"));
                po.addFld("delegate$", (Proto)om.findStrict("xdc.runtime.IGateProvider.Module", "ti.sysbios.heaps"), null, "wh");
                po.addFld("abstractInstances$", $$T_Bool, false, "wh");
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy$$create", "ti.sysbios.heaps"), Global.get("ti$sysbios$heaps$HeapMem_Module_GateProxy$$create"));
        }//isCFG
                po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.sysbios.heaps"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy$$Params", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem_Module_GateProxy.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.sysbios.heaps"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.heaps"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy$$Object", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem_Module_GateProxy.Object", om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance", "ti.sysbios.heaps"));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy$$Instance_State", "ti.sysbios.heaps");
        po.init("ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance_State", null);
        po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void HeapBuf$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$Instance_State", "ti.sysbios.heaps");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("ExtendedStats$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapBuf.ExtendedStats", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$ExtendedStats", "ti.sysbios.heaps");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapBuf.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$Instance_State", "ti.sysbios.heaps");
        po.bind("buf$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapBuf.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf$$Module_State", "ti.sysbios.heaps");
        po.bind("constructedHeaps$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
    }

    void HeapMem$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapMem", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$Instance_State", "ti.sysbios.heaps");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("ExtendedStats$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapMem.ExtendedStats", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$ExtendedStats", "ti.sysbios.heaps");
        vo.bind("Header$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapMem.Header", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$Header", "ti.sysbios.heaps");
        po.bind("next$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapMem.Header", "isScalar", false));
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapMem.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem$$Instance_State", "ti.sysbios.heaps");
        po.bind("buf$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
    }

    void HeapMultiBuf$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$Instance_State", "ti.sysbios.heaps");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("AddrPair$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapMultiBuf.AddrPair", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$AddrPair", "ti.sysbios.heaps");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapMultiBuf.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$Instance_State", "ti.sysbios.heaps");
        po.bind("bufsBySize$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
        po.bind("bufsByAddr$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapMultiBuf.AddrPair", "isScalar", false));
    }

    void HeapNull$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapNull", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapNull$$Instance_State", "ti.sysbios.heaps");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapNull.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapNull$$Instance_State", "ti.sysbios.heaps");
    }

    void HeapTrack$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack$$Instance_State", "ti.sysbios.heaps");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Tracker$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapTrack.Tracker", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack$$Tracker", "ti.sysbios.heaps");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapTrack.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack$$Instance_State", "ti.sysbios.heaps");
    }

    void HeapCallback$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback$$Instance_State", "ti.sysbios.heaps");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.heaps.HeapCallback.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback$$Instance_State", "ti.sysbios.heaps");
    }

    void HeapMem_Module_GateProxy$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy$$Instance_State", "ti.sysbios.heaps");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.heaps.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.heaps"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/heaps/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.heaps"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.heaps"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.heaps"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.heaps"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.heaps"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.heaps"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.heaps", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.heaps");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.heaps.");
        pkgV.bind("$vers", Global.newArray(2, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.heaps'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.heaps$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.heaps$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.heaps$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.heaps.aem4',\n");
            sb.append("'lib/debug/ti.sysbios.heaps.am4',\n");
            sb.append("'lib/debug/ti.sysbios.heaps.am4g',\n");
            sb.append("'lib/debug/ti.sysbios.heaps.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.heaps.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/debug/ti.sysbios.heaps.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/debug/ti.sysbios.heaps.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/debug/ti.sysbios.heaps.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void HeapBuf$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf.Module", "ti.sysbios.heaps");
        vo.init2(po, "ti.sysbios.heaps.HeapBuf", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.heaps.HeapBuf$$capsule", "ti.sysbios.heaps"));
        vo.bind("Instance", om.findStrict("ti.sysbios.heaps.HeapBuf.Instance", "ti.sysbios.heaps"));
        vo.bind("Params", om.findStrict("ti.sysbios.heaps.HeapBuf.Params", "ti.sysbios.heaps"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.heaps.HeapBuf.Params", "ti.sysbios.heaps")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.heaps.HeapBuf.Handle", "ti.sysbios.heaps"));
        vo.bind("$package", om.findStrict("ti.sysbios.heaps", "ti.sysbios.heaps"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.heaps.HeapBuf.BasicView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapBuf.BasicView", "ti.sysbios.heaps"));
        vo.bind("DetailedView", om.findStrict("ti.sysbios.heaps.HeapBuf.DetailedView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapBuf.DetailedView", "ti.sysbios.heaps"));
        vo.bind("ExtendedStats", om.findStrict("ti.sysbios.heaps.HeapBuf.ExtendedStats", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapBuf.ExtendedStats", "ti.sysbios.heaps"));
        mcfgs.add("A_nullBuf");
        mcfgs.add("A_bufAlign");
        mcfgs.add("A_invalidAlign");
        mcfgs.add("A_invalidRequestedAlign");
        mcfgs.add("A_invalidBlockSize");
        mcfgs.add("A_zeroBlocks");
        mcfgs.add("A_zeroBufSize");
        mcfgs.add("A_invalidBufSize");
        mcfgs.add("A_noBlocksToFree");
        mcfgs.add("A_invalidFree");
        mcfgs.add("E_size");
        mcfgs.add("trackMaxAllocs");
        mcfgs.add("numConstructedHeaps");
        icfgs.add("numConstructedHeaps");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.heaps.HeapBuf.Instance_State", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapBuf.Instance_State", "ti.sysbios.heaps"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.heaps.HeapBuf.Module_State", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapBuf.Module_State", "ti.sysbios.heaps"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.heaps")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.heaps.HeapBuf$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$heaps$HeapBuf$$__initObject"));
        }//isCFG
        vo.bind("postInit", om.findStrict("ti.sysbios.heaps.HeapBuf.postInit", "ti.sysbios.heaps"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_heaps_HeapBuf_Handle__label__E", "ti_sysbios_heaps_HeapBuf_Module__startupDone__E", "ti_sysbios_heaps_HeapBuf_Object__create__E", "ti_sysbios_heaps_HeapBuf_Object__delete__E", "ti_sysbios_heaps_HeapBuf_Object__get__E", "ti_sysbios_heaps_HeapBuf_Object__first__E", "ti_sysbios_heaps_HeapBuf_Object__next__E", "ti_sysbios_heaps_HeapBuf_Params__init__E", "ti_sysbios_heaps_HeapBuf_free__E", "ti_sysbios_heaps_HeapBuf_getStats__E", "ti_sysbios_heaps_HeapBuf_alloc__E", "ti_sysbios_heaps_HeapBuf_isBlocking__E", "ti_sysbios_heaps_HeapBuf_getBlockSize__E", "ti_sysbios_heaps_HeapBuf_getAlign__E", "ti_sysbios_heaps_HeapBuf_getEndAddr__E", "ti_sysbios_heaps_HeapBuf_getExtendedStats__E", "ti_sysbios_heaps_HeapBuf_mergeHeapBufs__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray("E_size"));
        vo.bind("$$assertDescCfgs", Global.newArray("A_nullBuf", "A_bufAlign", "A_invalidAlign", "A_invalidRequestedAlign", "A_invalidBlockSize", "A_zeroBlocks", "A_zeroBufSize", "A_invalidBufSize", "A_noBlocksToFree", "A_invalidFree"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.heaps.HeapBuf.Object", "ti.sysbios.heaps"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("HeapBuf", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HeapBuf");
    }

    void HeapMem$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapMem", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem.Module", "ti.sysbios.heaps");
        vo.init2(po, "ti.sysbios.heaps.HeapMem", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.heaps.HeapMem$$capsule", "ti.sysbios.heaps"));
        vo.bind("Instance", om.findStrict("ti.sysbios.heaps.HeapMem.Instance", "ti.sysbios.heaps"));
        vo.bind("Params", om.findStrict("ti.sysbios.heaps.HeapMem.Params", "ti.sysbios.heaps"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.heaps.HeapMem.Params", "ti.sysbios.heaps")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.heaps.HeapMem.Handle", "ti.sysbios.heaps"));
        vo.bind("$package", om.findStrict("ti.sysbios.heaps", "ti.sysbios.heaps"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.heaps.HeapMem.BasicView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapMem.BasicView", "ti.sysbios.heaps"));
        vo.bind("DetailedView", om.findStrict("ti.sysbios.heaps.HeapMem.DetailedView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapMem.DetailedView", "ti.sysbios.heaps"));
        vo.bind("FreeBlockView", om.findStrict("ti.sysbios.heaps.HeapMem.FreeBlockView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapMem.FreeBlockView", "ti.sysbios.heaps"));
        vo.bind("ExtendedStats", om.findStrict("ti.sysbios.heaps.HeapMem.ExtendedStats", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapMem.ExtendedStats", "ti.sysbios.heaps"));
        mcfgs.add("A_zeroBlock");
        mcfgs.add("A_heapSize");
        mcfgs.add("A_align");
        mcfgs.add("E_memory");
        mcfgs.add("A_invalidFree");
        mcfgs.add("primaryHeapBaseAddr");
        mcfgs.add("primaryHeapEndAddr");
        mcfgs.add("reqAlign");
        icfgs.add("reqAlign");
        vo.bind("Header", om.findStrict("ti.sysbios.heaps.HeapMem.Header", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapMem.Header", "ti.sysbios.heaps"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.heaps.HeapMem.Instance_State", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapMem.Instance_State", "ti.sysbios.heaps"));
        vo.bind("Module_GateProxy$proxy", om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy", "ti.sysbios.heaps"));
        proxies.add("Module_GateProxy");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.heaps")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.heaps.HeapMem$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$heaps$HeapMem$$__initObject"));
        }//isCFG
        vo.bind("enter", om.findStrict("ti.sysbios.heaps.HeapMem.enter", "ti.sysbios.heaps"));
        vo.bind("leave", om.findStrict("ti.sysbios.heaps.HeapMem.leave", "ti.sysbios.heaps"));
        vo.bind("init", om.findStrict("ti.sysbios.heaps.HeapMem.init", "ti.sysbios.heaps"));
        vo.bind("initPrimary", om.findStrict("ti.sysbios.heaps.HeapMem.initPrimary", "ti.sysbios.heaps"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_heaps_HeapMem_Handle__label__E", "ti_sysbios_heaps_HeapMem_Module__startupDone__E", "ti_sysbios_heaps_HeapMem_Object__create__E", "ti_sysbios_heaps_HeapMem_Object__delete__E", "ti_sysbios_heaps_HeapMem_Object__get__E", "ti_sysbios_heaps_HeapMem_Object__first__E", "ti_sysbios_heaps_HeapMem_Object__next__E", "ti_sysbios_heaps_HeapMem_Params__init__E", "ti_sysbios_heaps_HeapMem_enter__E", "ti_sysbios_heaps_HeapMem_leave__E", "ti_sysbios_heaps_HeapMem_alloc__E", "ti_sysbios_heaps_HeapMem_allocUnprotected__E", "ti_sysbios_heaps_HeapMem_free__E", "ti_sysbios_heaps_HeapMem_freeUnprotected__E", "ti_sysbios_heaps_HeapMem_isBlocking__E", "ti_sysbios_heaps_HeapMem_getStats__E", "ti_sysbios_heaps_HeapMem_restore__E", "ti_sysbios_heaps_HeapMem_getExtendedStats__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray("E_memory"));
        vo.bind("$$assertDescCfgs", Global.newArray("A_zeroBlock", "A_heapSize", "A_align", "A_invalidFree"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.heaps.HeapMem.Object", "ti.sysbios.heaps"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("HeapMem", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HeapMem");
    }

    void HeapMultiBuf$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Module", "ti.sysbios.heaps");
        vo.init2(po, "ti.sysbios.heaps.HeapMultiBuf", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.heaps.HeapMultiBuf$$capsule", "ti.sysbios.heaps"));
        vo.bind("Instance", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Instance", "ti.sysbios.heaps"));
        vo.bind("Params", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Params", "ti.sysbios.heaps"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Params", "ti.sysbios.heaps")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Handle", "ti.sysbios.heaps"));
        vo.bind("$package", om.findStrict("ti.sysbios.heaps", "ti.sysbios.heaps"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.BasicView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapMultiBuf.BasicView", "ti.sysbios.heaps"));
        vo.bind("DetailedView", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.DetailedView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapMultiBuf.DetailedView", "ti.sysbios.heaps"));
        mcfgs.add("A_blockNotFreed");
        mcfgs.add("E_size");
        vo.bind("AddrPair", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.AddrPair", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapMultiBuf.AddrPair", "ti.sysbios.heaps"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Instance_State", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Instance_State", "ti.sysbios.heaps"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.heaps")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.heaps.HeapMultiBuf$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$heaps$HeapMultiBuf$$__initObject"));
        }//isCFG
        vo.bind("addrPairCompare", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.addrPairCompare", "ti.sysbios.heaps"));
        vo.bind("sizeAlignCompare", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.sizeAlignCompare", "ti.sysbios.heaps"));
        vo.bind("borrowBlock", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.borrowBlock", "ti.sysbios.heaps"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_heaps_HeapMultiBuf_Handle__label__E", "ti_sysbios_heaps_HeapMultiBuf_Module__startupDone__E", "ti_sysbios_heaps_HeapMultiBuf_Object__create__E", "ti_sysbios_heaps_HeapMultiBuf_Object__delete__E", "ti_sysbios_heaps_HeapMultiBuf_Object__get__E", "ti_sysbios_heaps_HeapMultiBuf_Object__first__E", "ti_sysbios_heaps_HeapMultiBuf_Object__next__E", "ti_sysbios_heaps_HeapMultiBuf_Params__init__E", "ti_sysbios_heaps_HeapMultiBuf_getStats__E", "ti_sysbios_heaps_HeapMultiBuf_alloc__E", "ti_sysbios_heaps_HeapMultiBuf_free__E", "ti_sysbios_heaps_HeapMultiBuf_isBlocking__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray("E_size"));
        vo.bind("$$assertDescCfgs", Global.newArray("A_blockNotFreed"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.heaps.HeapMultiBuf.Object", "ti.sysbios.heaps"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("HeapMultiBuf", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HeapMultiBuf");
    }

    void HeapNull$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapNull", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapNull.Module", "ti.sysbios.heaps");
        vo.init2(po, "ti.sysbios.heaps.HeapNull", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.heaps.HeapNull$$capsule", "ti.sysbios.heaps"));
        vo.bind("Instance", om.findStrict("ti.sysbios.heaps.HeapNull.Instance", "ti.sysbios.heaps"));
        vo.bind("Params", om.findStrict("ti.sysbios.heaps.HeapNull.Params", "ti.sysbios.heaps"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.heaps.HeapNull.Params", "ti.sysbios.heaps")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.heaps.HeapNull.Handle", "ti.sysbios.heaps"));
        vo.bind("$package", om.findStrict("ti.sysbios.heaps", "ti.sysbios.heaps"));
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
        vo.bind("Instance_State", om.findStrict("ti.sysbios.heaps.HeapNull.Instance_State", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapNull.Instance_State", "ti.sysbios.heaps"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.heaps")).add(vo);
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
            vo.bind("__initObject", Global.get("ti$sysbios$heaps$HeapNull$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_heaps_HeapNull_Handle__label__E", "ti_sysbios_heaps_HeapNull_Module__startupDone__E", "ti_sysbios_heaps_HeapNull_Object__create__E", "ti_sysbios_heaps_HeapNull_Object__delete__E", "ti_sysbios_heaps_HeapNull_Object__get__E", "ti_sysbios_heaps_HeapNull_Object__first__E", "ti_sysbios_heaps_HeapNull_Object__next__E", "ti_sysbios_heaps_HeapNull_Params__init__E", "ti_sysbios_heaps_HeapNull_alloc__E", "ti_sysbios_heaps_HeapNull_free__E", "ti_sysbios_heaps_HeapNull_isBlocking__E", "ti_sysbios_heaps_HeapNull_getStats__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.heaps.HeapNull.Object", "ti.sysbios.heaps"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.heaps.HeapNull.Instance_State", "ti.sysbios.heaps"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("HeapNull", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HeapNull");
    }

    void HeapTrack$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack.Module", "ti.sysbios.heaps");
        vo.init2(po, "ti.sysbios.heaps.HeapTrack", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.heaps.HeapTrack$$capsule", "ti.sysbios.heaps"));
        vo.bind("Instance", om.findStrict("ti.sysbios.heaps.HeapTrack.Instance", "ti.sysbios.heaps"));
        vo.bind("Params", om.findStrict("ti.sysbios.heaps.HeapTrack.Params", "ti.sysbios.heaps"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.heaps.HeapTrack.Params", "ti.sysbios.heaps")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.heaps.HeapTrack.Handle", "ti.sysbios.heaps"));
        vo.bind("$package", om.findStrict("ti.sysbios.heaps", "ti.sysbios.heaps"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.heaps.HeapTrack.BasicView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapTrack.BasicView", "ti.sysbios.heaps"));
        vo.bind("TaskView", om.findStrict("ti.sysbios.heaps.HeapTrack.TaskView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapTrack.TaskView", "ti.sysbios.heaps"));
        vo.bind("HeapListView", om.findStrict("ti.sysbios.heaps.HeapTrack.HeapListView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapTrack.HeapListView", "ti.sysbios.heaps"));
        vo.bind("Tracker", om.findStrict("ti.sysbios.heaps.HeapTrack.Tracker", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapTrack.Tracker", "ti.sysbios.heaps"));
        mcfgs.add("A_doubleFree");
        mcfgs.add("A_bufOverflow");
        mcfgs.add("A_notEmpty");
        mcfgs.add("A_nullObject");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.heaps.HeapTrack.Instance_State", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapTrack.Instance_State", "ti.sysbios.heaps"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.heaps")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.heaps.HeapTrack$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$heaps$HeapTrack$$__initObject"));
        }//isCFG
        vo.bind("printTask", om.findStrict("ti.sysbios.heaps.HeapTrack.printTask", "ti.sysbios.heaps"));
        vo.bind("printTrack", om.findStrict("ti.sysbios.heaps.HeapTrack.printTrack", "ti.sysbios.heaps"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_heaps_HeapTrack_Handle__label__E", "ti_sysbios_heaps_HeapTrack_Module__startupDone__E", "ti_sysbios_heaps_HeapTrack_Object__create__E", "ti_sysbios_heaps_HeapTrack_Object__delete__E", "ti_sysbios_heaps_HeapTrack_Object__get__E", "ti_sysbios_heaps_HeapTrack_Object__first__E", "ti_sysbios_heaps_HeapTrack_Object__next__E", "ti_sysbios_heaps_HeapTrack_Params__init__E", "ti_sysbios_heaps_HeapTrack_alloc__E", "ti_sysbios_heaps_HeapTrack_free__E", "ti_sysbios_heaps_HeapTrack_isBlocking__E", "ti_sysbios_heaps_HeapTrack_getStats__E", "ti_sysbios_heaps_HeapTrack_printTask__E", "ti_sysbios_heaps_HeapTrack_printHeap__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_doubleFree", "A_bufOverflow", "A_notEmpty", "A_nullObject"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.heaps.HeapTrack.Object", "ti.sysbios.heaps"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("HeapTrack", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HeapTrack");
    }

    void HeapCallback$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback.Module", "ti.sysbios.heaps");
        vo.init2(po, "ti.sysbios.heaps.HeapCallback", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.heaps.HeapCallback$$capsule", "ti.sysbios.heaps"));
        vo.bind("Instance", om.findStrict("ti.sysbios.heaps.HeapCallback.Instance", "ti.sysbios.heaps"));
        vo.bind("Params", om.findStrict("ti.sysbios.heaps.HeapCallback.Params", "ti.sysbios.heaps"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.heaps.HeapCallback.Params", "ti.sysbios.heaps")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.heaps.HeapCallback.Handle", "ti.sysbios.heaps"));
        vo.bind("$package", om.findStrict("ti.sysbios.heaps", "ti.sysbios.heaps"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.heaps.HeapCallback.BasicView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapCallback.BasicView", "ti.sysbios.heaps"));
        vo.bind("ModuleView", om.findStrict("ti.sysbios.heaps.HeapCallback.ModuleView", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapCallback.ModuleView", "ti.sysbios.heaps"));
        vo.bind("AllocInstFxn", om.findStrict("ti.sysbios.heaps.HeapCallback.AllocInstFxn", "ti.sysbios.heaps"));
        vo.bind("CreateInstFxn", om.findStrict("ti.sysbios.heaps.HeapCallback.CreateInstFxn", "ti.sysbios.heaps"));
        vo.bind("DeleteInstFxn", om.findStrict("ti.sysbios.heaps.HeapCallback.DeleteInstFxn", "ti.sysbios.heaps"));
        vo.bind("FreeInstFxn", om.findStrict("ti.sysbios.heaps.HeapCallback.FreeInstFxn", "ti.sysbios.heaps"));
        vo.bind("GetStatsInstFxn", om.findStrict("ti.sysbios.heaps.HeapCallback.GetStatsInstFxn", "ti.sysbios.heaps"));
        vo.bind("InitInstFxn", om.findStrict("ti.sysbios.heaps.HeapCallback.InitInstFxn", "ti.sysbios.heaps"));
        vo.bind("IsBlockingInstFxn", om.findStrict("ti.sysbios.heaps.HeapCallback.IsBlockingInstFxn", "ti.sysbios.heaps"));
        mcfgs.add("allocInstFxn");
        mcfgs.add("createInstFxn");
        mcfgs.add("deleteInstFxn");
        mcfgs.add("freeInstFxn");
        mcfgs.add("getStatsInstFxn");
        mcfgs.add("initInstFxn");
        mcfgs.add("isBlockingInstFxn");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.heaps.HeapCallback.Instance_State", "ti.sysbios.heaps"));
        tdefs.add(om.findStrict("ti.sysbios.heaps.HeapCallback.Instance_State", "ti.sysbios.heaps"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.heaps")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.heaps.HeapCallback$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$heaps$HeapCallback$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_heaps_HeapCallback_Handle__label__E", "ti_sysbios_heaps_HeapCallback_Module__startupDone__E", "ti_sysbios_heaps_HeapCallback_Object__create__E", "ti_sysbios_heaps_HeapCallback_Object__delete__E", "ti_sysbios_heaps_HeapCallback_Object__get__E", "ti_sysbios_heaps_HeapCallback_Object__first__E", "ti_sysbios_heaps_HeapCallback_Object__next__E", "ti_sysbios_heaps_HeapCallback_Params__init__E", "ti_sysbios_heaps_HeapCallback_alloc__E", "ti_sysbios_heaps_HeapCallback_free__E", "ti_sysbios_heaps_HeapCallback_isBlocking__E", "ti_sysbios_heaps_HeapCallback_getStats__E", "ti_sysbios_heaps_HeapCallback_getContext__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.heaps.HeapCallback.Object", "ti.sysbios.heaps"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("HeapCallback", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HeapCallback");
    }

    void HeapMem_Module_GateProxy$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy", "ti.sysbios.heaps");
        po = (Proto.Obj)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Module", "ti.sysbios.heaps");
        vo.init2(po, "ti.sysbios.heaps.HeapMem_Module_GateProxy", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance", "ti.sysbios.heaps"));
        vo.bind("Params", om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Params", "ti.sysbios.heaps"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Params", "ti.sysbios.heaps")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Handle", "ti.sysbios.heaps"));
        vo.bind("$package", om.findStrict("ti.sysbios.heaps", "ti.sysbios.heaps"));
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
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.heaps")).add(vo);
        vo.bind("$$instflag", 1);
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
            vo.bind("__initObject", Global.get("ti$sysbios$heaps$HeapMem_Module_GateProxy$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.query", "ti.sysbios.heaps"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__Handle__label", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__Module__startupDone", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__Object__create", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__Object__delete", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__Object__get", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__Object__first", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__Object__next", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__Params__init", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__Proxy__abstract", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__Proxy__delegate", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__queryMeta", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__query", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__enter", "ti_sysbios_heaps_HeapMem_Module_GateProxy_DELEGATE__leave"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Object", "ti.sysbios.heaps"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy.Instance_State", "ti.sysbios.heaps"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 1);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("HeapMem_Module_GateProxy", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HeapMem_Module_GateProxy");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.sysbios.heaps")).findStrict("PARAMS", "ti.sysbios.heaps");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.heaps.HeapBuf", "ti.sysbios.heaps")).findStrict("PARAMS", "ti.sysbios.heaps");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.heaps.HeapMem", "ti.sysbios.heaps")).findStrict("PARAMS", "ti.sysbios.heaps");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.heaps.HeapMultiBuf", "ti.sysbios.heaps")).findStrict("PARAMS", "ti.sysbios.heaps");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.heaps.HeapNull", "ti.sysbios.heaps")).findStrict("PARAMS", "ti.sysbios.heaps");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.heaps.HeapTrack", "ti.sysbios.heaps")).findStrict("PARAMS", "ti.sysbios.heaps");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.heaps.HeapCallback", "ti.sysbios.heaps")).findStrict("PARAMS", "ti.sysbios.heaps");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy", "ti.sysbios.heaps")).findStrict("PARAMS", "ti.sysbios.heaps");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.heaps.HeapBuf", "ti.sysbios.heaps"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.heaps.HeapMem", "ti.sysbios.heaps"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.heaps.HeapMultiBuf", "ti.sysbios.heaps"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.heaps.HeapNull", "ti.sysbios.heaps"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.heaps.HeapTrack", "ti.sysbios.heaps"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.heaps.HeapCallback", "ti.sysbios.heaps"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.heaps.HeapMem_Module_GateProxy", "ti.sysbios.heaps"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapBuf", "ti.sysbios.heaps");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitInstance", "structName", "BasicView")}), Global.newArray(new Object[]{"Detailed", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitDetailed", "structName", "DetailedView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapMem", "ti.sysbios.heaps");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Detailed", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitDetailed", "structName", "DetailedView")}), Global.newArray(new Object[]{"FreeList", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE_DATA"), "viewInitFxn", "viewInitData", "structName", "FreeBlockView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapMultiBuf", "ti.sysbios.heaps");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Detailed", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitDetailed", "structName", "DetailedView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapTrack", "ti.sysbios.heaps");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"HeapAllocList", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE_DATA"), "viewInitFxn", "viewInitHeapList", "structName", "HeapListView")}), Global.newArray(new Object[]{"TaskAllocList", Global.newObject("type", om.find("xdc.rov.ViewInfo.TREE_TABLE"), "viewInitFxn", "viewInitTask", "structName", "TaskView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.heaps.HeapCallback", "ti.sysbios.heaps");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.heaps.HeapBuf")).bless();
        ((Value.Obj)om.getv("ti.sysbios.heaps.HeapMem")).bless();
        ((Value.Obj)om.getv("ti.sysbios.heaps.HeapMultiBuf")).bless();
        ((Value.Obj)om.getv("ti.sysbios.heaps.HeapNull")).bless();
        ((Value.Obj)om.getv("ti.sysbios.heaps.HeapTrack")).bless();
        ((Value.Obj)om.getv("ti.sysbios.heaps.HeapCallback")).bless();
        ((Value.Obj)om.getv("ti.sysbios.heaps.HeapMem_Module_GateProxy")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.heaps")).add(pkgV);
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
        HeapBuf$$OBJECTS();
        HeapMem$$OBJECTS();
        HeapMultiBuf$$OBJECTS();
        HeapNull$$OBJECTS();
        HeapTrack$$OBJECTS();
        HeapCallback$$OBJECTS();
        HeapMem_Module_GateProxy$$OBJECTS();
        HeapBuf$$CONSTS();
        HeapMem$$CONSTS();
        HeapMultiBuf$$CONSTS();
        HeapNull$$CONSTS();
        HeapTrack$$CONSTS();
        HeapCallback$$CONSTS();
        HeapMem_Module_GateProxy$$CONSTS();
        HeapBuf$$CREATES();
        HeapMem$$CREATES();
        HeapMultiBuf$$CREATES();
        HeapNull$$CREATES();
        HeapTrack$$CREATES();
        HeapCallback$$CREATES();
        HeapMem_Module_GateProxy$$CREATES();
        HeapBuf$$FUNCTIONS();
        HeapMem$$FUNCTIONS();
        HeapMultiBuf$$FUNCTIONS();
        HeapNull$$FUNCTIONS();
        HeapTrack$$FUNCTIONS();
        HeapCallback$$FUNCTIONS();
        HeapMem_Module_GateProxy$$FUNCTIONS();
        HeapBuf$$SIZES();
        HeapMem$$SIZES();
        HeapMultiBuf$$SIZES();
        HeapNull$$SIZES();
        HeapTrack$$SIZES();
        HeapCallback$$SIZES();
        HeapMem_Module_GateProxy$$SIZES();
        HeapBuf$$TYPES();
        HeapMem$$TYPES();
        HeapMultiBuf$$TYPES();
        HeapNull$$TYPES();
        HeapTrack$$TYPES();
        HeapCallback$$TYPES();
        HeapMem_Module_GateProxy$$TYPES();
        if (isROV) {
            HeapBuf$$ROV();
            HeapMem$$ROV();
            HeapMultiBuf$$ROV();
            HeapNull$$ROV();
            HeapTrack$$ROV();
            HeapCallback$$ROV();
            HeapMem_Module_GateProxy$$ROV();
        }//isROV
        $$SINGLETONS();
        HeapBuf$$SINGLETONS();
        HeapMem$$SINGLETONS();
        HeapMultiBuf$$SINGLETONS();
        HeapNull$$SINGLETONS();
        HeapTrack$$SINGLETONS();
        HeapCallback$$SINGLETONS();
        HeapMem_Module_GateProxy$$SINGLETONS();
        $$INITIALIZATION();
    }
}
