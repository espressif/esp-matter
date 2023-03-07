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

public class ti_sysbios_knl
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
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.family");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.knl.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.knl", new Value.Obj("ti.sysbios.knl", pkgP));
    }

    void Clock$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Clock.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Clock", new Value.Obj("ti.sysbios.knl.Clock", po));
        pkgV.bind("Clock", vo);
        // decls 
        om.bind("ti.sysbios.knl.Clock.TickSource", new Proto.Enm("ti.sysbios.knl.Clock.TickSource"));
        om.bind("ti.sysbios.knl.Clock.TickMode", new Proto.Enm("ti.sysbios.knl.Clock.TickMode"));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Clock$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Clock.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Clock$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Clock.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Clock$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Clock.Instance_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Clock$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Clock.Module_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.knl.Clock.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Clock$$Object", new Proto.Obj());
        om.bind("ti.sysbios.knl.Clock.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Clock$$Params", new Proto.Obj());
        om.bind("ti.sysbios.knl.Clock.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.knl.Clock.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.knl.Clock.Object", om.findStrict("ti.sysbios.knl.Clock.Instance_State", "ti.sysbios.knl"));
        }//isROV
    }

    void Idle$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Idle.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Idle", new Value.Obj("ti.sysbios.knl.Idle", po));
        pkgV.bind("Idle", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Idle$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Idle.ModuleView", new Proto.Str(spo, false));
    }

    void Intrinsics$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Intrinsics.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Intrinsics", new Value.Obj("ti.sysbios.knl.Intrinsics", po));
        pkgV.bind("Intrinsics", vo);
        // decls 
    }

    void Event$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Event.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Event", new Value.Obj("ti.sysbios.knl.Event", po));
        pkgV.bind("Event", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Event$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Event.BasicView", new Proto.Str(spo, false));
        om.bind("ti.sysbios.knl.Event.PendState", new Proto.Enm("ti.sysbios.knl.Event.PendState"));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Event$$PendElem", new Proto.Obj());
        om.bind("ti.sysbios.knl.Event.PendElem", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Event$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Event.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.knl.Event.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Event$$Object", new Proto.Obj());
        om.bind("ti.sysbios.knl.Event.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Event$$Params", new Proto.Obj());
        om.bind("ti.sysbios.knl.Event.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.knl.Event.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.knl.Event.Object", om.findStrict("ti.sysbios.knl.Event.Instance_State", "ti.sysbios.knl"));
        }//isROV
    }

    void Mailbox$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Mailbox.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Mailbox", new Value.Obj("ti.sysbios.knl.Mailbox", po));
        pkgV.bind("Mailbox", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Mailbox$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Mailbox.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Mailbox$$DetailedView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Mailbox.DetailedView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Mailbox$$MbxElem", new Proto.Obj());
        om.bind("ti.sysbios.knl.Mailbox.MbxElem", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Mailbox$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Mailbox.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.knl.Mailbox.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Mailbox$$Object", new Proto.Obj());
        om.bind("ti.sysbios.knl.Mailbox.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Mailbox$$Params", new Proto.Obj());
        om.bind("ti.sysbios.knl.Mailbox.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.knl.Mailbox.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.knl.Mailbox.Object", om.findStrict("ti.sysbios.knl.Mailbox.Instance_State", "ti.sysbios.knl"));
        }//isROV
    }

    void Queue$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Queue.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Queue", new Value.Obj("ti.sysbios.knl.Queue", po));
        pkgV.bind("Queue", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Queue$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Queue.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Queue$$Elem", new Proto.Obj());
        om.bind("ti.sysbios.knl.Queue.Elem", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Queue$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Queue.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.knl.Queue.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Queue$$Object", new Proto.Obj());
        om.bind("ti.sysbios.knl.Queue.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Queue$$Params", new Proto.Obj());
        om.bind("ti.sysbios.knl.Queue.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.knl.Queue.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.knl.Queue.Object", om.findStrict("ti.sysbios.knl.Queue.Instance_State", "ti.sysbios.knl"));
        }//isROV
    }

    void Semaphore$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Semaphore.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Semaphore", new Value.Obj("ti.sysbios.knl.Semaphore", po));
        pkgV.bind("Semaphore", vo);
        // decls 
        om.bind("ti.sysbios.knl.Semaphore.Mode", new Proto.Enm("ti.sysbios.knl.Semaphore.Mode"));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Semaphore$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Semaphore.BasicView", new Proto.Str(spo, false));
        om.bind("ti.sysbios.knl.Semaphore.PendState", new Proto.Enm("ti.sysbios.knl.Semaphore.PendState"));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Semaphore$$PendElem", new Proto.Obj());
        om.bind("ti.sysbios.knl.Semaphore.PendElem", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Semaphore$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Semaphore.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.knl.Semaphore.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Semaphore$$Object", new Proto.Obj());
        om.bind("ti.sysbios.knl.Semaphore.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Semaphore$$Params", new Proto.Obj());
        om.bind("ti.sysbios.knl.Semaphore.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.knl.Semaphore.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.knl.Semaphore.Object", om.findStrict("ti.sysbios.knl.Semaphore.Instance_State", "ti.sysbios.knl"));
        }//isROV
    }

    void Swi$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Swi.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Swi", new Value.Obj("ti.sysbios.knl.Swi", po));
        pkgV.bind("Swi", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Swi$$HookSet", new Proto.Obj());
        om.bind("ti.sysbios.knl.Swi.HookSet", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Swi$$Struct2__", new Proto.Obj());
        om.bind("ti.sysbios.knl.Swi.Struct2__", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Swi$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Swi.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Swi$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Swi.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Swi$$ReadyQView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Swi.ReadyQView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Swi$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Swi.Instance_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Swi$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Swi.Module_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.knl.Swi.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Swi$$Object", new Proto.Obj());
        om.bind("ti.sysbios.knl.Swi.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Swi$$Params", new Proto.Obj());
        om.bind("ti.sysbios.knl.Swi.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.knl.Swi.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.knl.Swi.Object", om.findStrict("ti.sysbios.knl.Swi.Instance_State", "ti.sysbios.knl"));
        }//isROV
    }

    void Task$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Task.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Task", new Value.Obj("ti.sysbios.knl.Task", po));
        pkgV.bind("Task", vo);
        // decls 
        om.bind("ti.sysbios.knl.Task.Mode", new Proto.Enm("ti.sysbios.knl.Task.Mode"));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$Stat", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.Stat", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$HookSet", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.HookSet", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$DetailedView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.DetailedView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$CallStackView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.CallStackView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$ReadyQView", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.ReadyQView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$PendElem", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.PendElem", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.Instance_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.Module_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$RunQEntry", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.RunQEntry", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$Module_StateSmp", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.Module_StateSmp", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.knl.Task.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$Object", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Task$$Params", new Proto.Obj());
        om.bind("ti.sysbios.knl.Task.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.knl.Task.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.knl.Task.Object", om.findStrict("ti.sysbios.knl.Task.Instance_State", "ti.sysbios.knl"));
        }//isROV
    }

    void Clock_TimerProxy$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Clock_TimerProxy.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Clock_TimerProxy", new Value.Obj("ti.sysbios.knl.Clock_TimerProxy", po));
        pkgV.bind("Clock_TimerProxy", vo);
        // decls 
        om.bind("ti.sysbios.knl.Clock_TimerProxy.StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.knl"));
        om.bind("ti.sysbios.knl.Clock_TimerProxy.RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.knl"));
        om.bind("ti.sysbios.knl.Clock_TimerProxy.Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.knl"));
        om.bind("ti.sysbios.knl.Clock_TimerProxy.PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.knl"));
        // insts 
        Object insP = om.bind("ti.sysbios.knl.Clock_TimerProxy.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Clock_TimerProxy$$Object", new Proto.Obj());
        om.bind("ti.sysbios.knl.Clock_TimerProxy.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Clock_TimerProxy$$Params", new Proto.Obj());
        om.bind("ti.sysbios.knl.Clock_TimerProxy.Params", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.knl.Clock_TimerProxy$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.knl.Clock_TimerProxy.Instance_State", new Proto.Str(po, false));
        om.bind("ti.sysbios.knl.Clock_TimerProxy.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.knl.Clock_TimerProxy.Object", om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Instance_State", "ti.sysbios.knl"));
        }//isROV
    }

    void Intrinsics_SupportProxy$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Intrinsics_SupportProxy.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Intrinsics_SupportProxy", new Value.Obj("ti.sysbios.knl.Intrinsics_SupportProxy", po));
        pkgV.bind("Intrinsics_SupportProxy", vo);
        // decls 
    }

    void Task_SupportProxy$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.knl.Task_SupportProxy.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.knl.Task_SupportProxy", new Value.Obj("ti.sysbios.knl.Task_SupportProxy", po));
        pkgV.bind("Task_SupportProxy", vo);
        // decls 
    }

    void Clock$$CONSTS()
    {
        // module Clock
        om.bind("ti.sysbios.knl.Clock.TickSource_TIMER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Clock.TickSource", "ti.sysbios.knl"), "ti.sysbios.knl.Clock.TickSource_TIMER", 0));
        om.bind("ti.sysbios.knl.Clock.TickSource_USER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Clock.TickSource", "ti.sysbios.knl"), "ti.sysbios.knl.Clock.TickSource_USER", 1));
        om.bind("ti.sysbios.knl.Clock.TickSource_NULL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Clock.TickSource", "ti.sysbios.knl"), "ti.sysbios.knl.Clock.TickSource_NULL", 2));
        om.bind("ti.sysbios.knl.Clock.TickMode_PERIODIC", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Clock.TickMode", "ti.sysbios.knl"), "ti.sysbios.knl.Clock.TickMode_PERIODIC", 0));
        om.bind("ti.sysbios.knl.Clock.TickMode_DYNAMIC", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Clock.TickMode", "ti.sysbios.knl"), "ti.sysbios.knl.Clock.TickMode_DYNAMIC", 1));
        om.bind("ti.sysbios.knl.Clock.getTicks", new Extern("ti_sysbios_knl_Clock_getTicks__E", "xdc_UInt32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.getTimerHandle", new Extern("ti_sysbios_knl_Clock_getTimerHandle__E", "ti_sysbios_knl_Clock_TimerProxy_Handle(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.setTicks", new Extern("ti_sysbios_knl_Clock_setTicks__E", "xdc_Void(*)(xdc_UInt32)", true, false));
        om.bind("ti.sysbios.knl.Clock.tickStop", new Extern("ti_sysbios_knl_Clock_tickStop__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.tickReconfig", new Extern("ti_sysbios_knl_Clock_tickReconfig__E", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.tickStart", new Extern("ti_sysbios_knl_Clock_tickStart__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.tick", new Extern("ti_sysbios_knl_Clock_tick__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.workFunc", new Extern("ti_sysbios_knl_Clock_workFunc__E", "xdc_Void(*)(xdc_UArg,xdc_UArg)", true, false));
        om.bind("ti.sysbios.knl.Clock.workFuncDynamic", new Extern("ti_sysbios_knl_Clock_workFuncDynamic__E", "xdc_Void(*)(xdc_UArg,xdc_UArg)", true, false));
        om.bind("ti.sysbios.knl.Clock.logTick", new Extern("ti_sysbios_knl_Clock_logTick__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.getCompletedTicks", new Extern("ti_sysbios_knl_Clock_getCompletedTicks__E", "xdc_UInt32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.getTickPeriod", new Extern("ti_sysbios_knl_Clock_getTickPeriod__E", "xdc_UInt32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.getTicksUntilInterrupt", new Extern("ti_sysbios_knl_Clock_getTicksUntilInterrupt__E", "xdc_UInt32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.getTicksUntilTimeout", new Extern("ti_sysbios_knl_Clock_getTicksUntilTimeout__E", "xdc_UInt32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.walkQueueDynamic", new Extern("ti_sysbios_knl_Clock_walkQueueDynamic__E", "xdc_UInt32(*)(xdc_Bool,xdc_UInt32)", true, false));
        om.bind("ti.sysbios.knl.Clock.walkQueuePeriodic", new Extern("ti_sysbios_knl_Clock_walkQueuePeriodic__E", "xdc_UInt32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock.scheduleNextTick", new Extern("ti_sysbios_knl_Clock_scheduleNextTick__E", "xdc_Void(*)(xdc_UInt32,xdc_UInt32)", true, false));
        om.bind("ti.sysbios.knl.Clock.doTick", new Extern("ti_sysbios_knl_Clock_doTick__I", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.knl.Clock.triggerFunc", new Extern("ti_sysbios_knl_Clock_triggerFunc__I", "xdc_Void(*)(xdc_UArg)", true, false));
    }

    void Idle$$CONSTS()
    {
        // module Idle
        om.bind("ti.sysbios.knl.Idle.loop", new Extern("ti_sysbios_knl_Idle_loop__E", "xdc_Void(*)(xdc_UArg,xdc_UArg)", true, false));
        om.bind("ti.sysbios.knl.Idle.run", new Extern("ti_sysbios_knl_Idle_run__E", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void Intrinsics$$CONSTS()
    {
        // module Intrinsics
    }

    void Event$$CONSTS()
    {
        // module Event
        om.bind("ti.sysbios.knl.Event.Id_00", 0x1L);
        om.bind("ti.sysbios.knl.Event.Id_01", 0x2L);
        om.bind("ti.sysbios.knl.Event.Id_02", 0x4L);
        om.bind("ti.sysbios.knl.Event.Id_03", 0x8L);
        om.bind("ti.sysbios.knl.Event.Id_04", 0x10L);
        om.bind("ti.sysbios.knl.Event.Id_05", 0x20L);
        om.bind("ti.sysbios.knl.Event.Id_06", 0x40L);
        om.bind("ti.sysbios.knl.Event.Id_07", 0x80L);
        om.bind("ti.sysbios.knl.Event.Id_08", 0x100L);
        om.bind("ti.sysbios.knl.Event.Id_09", 0x200L);
        om.bind("ti.sysbios.knl.Event.Id_10", 0x400L);
        om.bind("ti.sysbios.knl.Event.Id_11", 0x800L);
        om.bind("ti.sysbios.knl.Event.Id_12", 0x1000L);
        om.bind("ti.sysbios.knl.Event.Id_13", 0x2000L);
        om.bind("ti.sysbios.knl.Event.Id_14", 0x4000L);
        om.bind("ti.sysbios.knl.Event.Id_15", 0x8000L);
        om.bind("ti.sysbios.knl.Event.Id_16", 0x10000L);
        om.bind("ti.sysbios.knl.Event.Id_17", 0x20000L);
        om.bind("ti.sysbios.knl.Event.Id_18", 0x40000L);
        om.bind("ti.sysbios.knl.Event.Id_19", 0x80000L);
        om.bind("ti.sysbios.knl.Event.Id_20", 0x100000L);
        om.bind("ti.sysbios.knl.Event.Id_21", 0x200000L);
        om.bind("ti.sysbios.knl.Event.Id_22", 0x400000L);
        om.bind("ti.sysbios.knl.Event.Id_23", 0x800000L);
        om.bind("ti.sysbios.knl.Event.Id_24", 0x1000000L);
        om.bind("ti.sysbios.knl.Event.Id_25", 0x2000000L);
        om.bind("ti.sysbios.knl.Event.Id_26", 0x4000000L);
        om.bind("ti.sysbios.knl.Event.Id_27", 0x8000000L);
        om.bind("ti.sysbios.knl.Event.Id_28", 0x10000000L);
        om.bind("ti.sysbios.knl.Event.Id_29", 0x20000000L);
        om.bind("ti.sysbios.knl.Event.Id_30", 0x40000000L);
        om.bind("ti.sysbios.knl.Event.Id_31", 0x80000000L);
        om.bind("ti.sysbios.knl.Event.Id_NONE", 0L);
        om.bind("ti.sysbios.knl.Event.PendState_TIMEOUT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Event.PendState", "ti.sysbios.knl"), "ti.sysbios.knl.Event.PendState_TIMEOUT", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.sysbios.knl.Event.PendState_POSTED", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Event.PendState", "ti.sysbios.knl"), "ti.sysbios.knl.Event.PendState_POSTED", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.sysbios.knl.Event.PendState_CLOCK_WAIT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Event.PendState", "ti.sysbios.knl"), "ti.sysbios.knl.Event.PendState_CLOCK_WAIT", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.sysbios.knl.Event.PendState_WAIT_FOREVER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Event.PendState", "ti.sysbios.knl"), "ti.sysbios.knl.Event.PendState_WAIT_FOREVER", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.sysbios.knl.Event.pendTimeout", new Extern("ti_sysbios_knl_Event_pendTimeout__I", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.knl.Event.checkEvents", new Extern("ti_sysbios_knl_Event_checkEvents__I", "xdc_UInt(*)(ti_sysbios_knl_Event_Object*,xdc_UInt,xdc_UInt)", true, false));
    }

    void Mailbox$$CONSTS()
    {
        // module Mailbox
        om.bind("ti.sysbios.knl.Mailbox.cleanQue", new Extern("ti_sysbios_knl_Mailbox_cleanQue__I", "xdc_Void(*)(ti_sysbios_knl_Queue_Handle)", true, false));
        om.bind("ti.sysbios.knl.Mailbox.postInit", new Extern("ti_sysbios_knl_Mailbox_postInit__I", "xdc_Int(*)(ti_sysbios_knl_Mailbox_Object*,xdc_SizeT)", true, false));
    }

    void Queue$$CONSTS()
    {
        // module Queue
        om.bind("ti.sysbios.knl.Queue.elemClear", new Extern("ti_sysbios_knl_Queue_elemClear__E", "xdc_Void(*)(ti_sysbios_knl_Queue_Elem*)", true, false));
        om.bind("ti.sysbios.knl.Queue.insert", new Extern("ti_sysbios_knl_Queue_insert__E", "xdc_Void(*)(ti_sysbios_knl_Queue_Elem*,ti_sysbios_knl_Queue_Elem*)", true, false));
        om.bind("ti.sysbios.knl.Queue.next", new Extern("ti_sysbios_knl_Queue_next__E", "xdc_Ptr(*)(ti_sysbios_knl_Queue_Elem*)", true, false));
        om.bind("ti.sysbios.knl.Queue.prev", new Extern("ti_sysbios_knl_Queue_prev__E", "xdc_Ptr(*)(ti_sysbios_knl_Queue_Elem*)", true, false));
        om.bind("ti.sysbios.knl.Queue.remove", new Extern("ti_sysbios_knl_Queue_remove__E", "xdc_Void(*)(ti_sysbios_knl_Queue_Elem*)", true, false));
        om.bind("ti.sysbios.knl.Queue.isQueued", new Extern("ti_sysbios_knl_Queue_isQueued__E", "xdc_Bool(*)(ti_sysbios_knl_Queue_Elem*)", true, false));
    }

    void Semaphore$$CONSTS()
    {
        // module Semaphore
        om.bind("ti.sysbios.knl.Semaphore.Mode_COUNTING", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Semaphore.Mode", "ti.sysbios.knl"), "ti.sysbios.knl.Semaphore.Mode_COUNTING", xdc.services.intern.xsr.Enum.intValue(0x0L)+0));
        om.bind("ti.sysbios.knl.Semaphore.Mode_BINARY", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Semaphore.Mode", "ti.sysbios.knl"), "ti.sysbios.knl.Semaphore.Mode_BINARY", xdc.services.intern.xsr.Enum.intValue(0x1L)+0));
        om.bind("ti.sysbios.knl.Semaphore.Mode_COUNTING_PRIORITY", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Semaphore.Mode", "ti.sysbios.knl"), "ti.sysbios.knl.Semaphore.Mode_COUNTING_PRIORITY", xdc.services.intern.xsr.Enum.intValue(0x2L)+0));
        om.bind("ti.sysbios.knl.Semaphore.Mode_BINARY_PRIORITY", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Semaphore.Mode", "ti.sysbios.knl"), "ti.sysbios.knl.Semaphore.Mode_BINARY_PRIORITY", xdc.services.intern.xsr.Enum.intValue(0x3L)+0));
        om.bind("ti.sysbios.knl.Semaphore.PendState_TIMEOUT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Semaphore.PendState", "ti.sysbios.knl"), "ti.sysbios.knl.Semaphore.PendState_TIMEOUT", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.sysbios.knl.Semaphore.PendState_POSTED", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Semaphore.PendState", "ti.sysbios.knl"), "ti.sysbios.knl.Semaphore.PendState_POSTED", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.sysbios.knl.Semaphore.PendState_CLOCK_WAIT", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Semaphore.PendState", "ti.sysbios.knl"), "ti.sysbios.knl.Semaphore.PendState_CLOCK_WAIT", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.sysbios.knl.Semaphore.PendState_WAIT_FOREVER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Semaphore.PendState", "ti.sysbios.knl"), "ti.sysbios.knl.Semaphore.PendState_WAIT_FOREVER", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.sysbios.knl.Semaphore.pendTimeout", new Extern("ti_sysbios_knl_Semaphore_pendTimeout__I", "xdc_Void(*)(xdc_UArg)", true, false));
    }

    void Swi$$CONSTS()
    {
        // module Swi
        om.bind("ti.sysbios.knl.Swi.construct2", new Extern("ti_sysbios_knl_Swi_construct2__E", "ti_sysbios_knl_Swi_Handle(*)(ti_sysbios_knl_Swi_Struct2__*,xdc_Void(*)(xdc_UArg,xdc_UArg),ti_sysbios_knl_Swi_Params*)", true, false));
        om.bind("ti.sysbios.knl.Swi.startup", new Extern("ti_sysbios_knl_Swi_startup__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Swi.enabled", new Extern("ti_sysbios_knl_Swi_enabled__E", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Swi.unlockSched", new Extern("ti_sysbios_knl_Swi_unlockSched__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Swi.disable", new Extern("ti_sysbios_knl_Swi_disable__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Swi.enable", new Extern("ti_sysbios_knl_Swi_enable__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Swi.restore", new Extern("ti_sysbios_knl_Swi_restore__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.knl.Swi.restoreHwi", new Extern("ti_sysbios_knl_Swi_restoreHwi__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.knl.Swi.self", new Extern("ti_sysbios_knl_Swi_self__E", "ti_sysbios_knl_Swi_Handle(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Swi.getTrigger", new Extern("ti_sysbios_knl_Swi_getTrigger__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Swi.raisePri", new Extern("ti_sysbios_knl_Swi_raisePri__E", "xdc_UInt(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.knl.Swi.restorePri", new Extern("ti_sysbios_knl_Swi_restorePri__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.knl.Swi.schedule", new Extern("ti_sysbios_knl_Swi_schedule__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Swi.runLoop", new Extern("ti_sysbios_knl_Swi_runLoop__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Swi.run", new Extern("ti_sysbios_knl_Swi_run__I", "xdc_Void(*)(ti_sysbios_knl_Swi_Object*)", true, false));
        om.bind("ti.sysbios.knl.Swi.postInit", new Extern("ti_sysbios_knl_Swi_postInit__I", "xdc_Int(*)(ti_sysbios_knl_Swi_Object*,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.knl.Swi.restoreSMP", new Extern("ti_sysbios_knl_Swi_restoreSMP__I", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void Task$$CONSTS()
    {
        // module Task
        om.bind("ti.sysbios.knl.Task.Mode_RUNNING", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Task.Mode", "ti.sysbios.knl"), "ti.sysbios.knl.Task.Mode_RUNNING", 0));
        om.bind("ti.sysbios.knl.Task.Mode_READY", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Task.Mode", "ti.sysbios.knl"), "ti.sysbios.knl.Task.Mode_READY", 1));
        om.bind("ti.sysbios.knl.Task.Mode_BLOCKED", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Task.Mode", "ti.sysbios.knl"), "ti.sysbios.knl.Task.Mode_BLOCKED", 2));
        om.bind("ti.sysbios.knl.Task.Mode_TERMINATED", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Task.Mode", "ti.sysbios.knl"), "ti.sysbios.knl.Task.Mode_TERMINATED", 3));
        om.bind("ti.sysbios.knl.Task.Mode_INACTIVE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.knl.Task.Mode", "ti.sysbios.knl"), "ti.sysbios.knl.Task.Mode_INACTIVE", 4));
        om.bind("ti.sysbios.knl.Task.AFFINITY_NONE", Global.eval("~(0)"));
        om.bind("ti.sysbios.knl.Task.startup", new Extern("ti_sysbios_knl_Task_startup__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.enabled", new Extern("ti_sysbios_knl_Task_enabled__E", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.unlockSched", new Extern("ti_sysbios_knl_Task_unlockSched__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.disable", new Extern("ti_sysbios_knl_Task_disable__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.enable", new Extern("ti_sysbios_knl_Task_enable__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.restore", new Extern("ti_sysbios_knl_Task_restore__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.knl.Task.restoreHwi", new Extern("ti_sysbios_knl_Task_restoreHwi__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.knl.Task.self", new Extern("ti_sysbios_knl_Task_self__E", "ti_sysbios_knl_Task_Handle(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.checkStacks", new Extern("ti_sysbios_knl_Task_checkStacks__E", "xdc_Void(*)(ti_sysbios_knl_Task_Handle,ti_sysbios_knl_Task_Handle)", true, false));
        om.bind("ti.sysbios.knl.Task.exit", new Extern("ti_sysbios_knl_Task_exit__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.sleep", new Extern("ti_sysbios_knl_Task_sleep__E", "xdc_Void(*)(xdc_UInt32)", true, false));
        om.bind("ti.sysbios.knl.Task.yield", new Extern("ti_sysbios_knl_Task_yield__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.getIdleTask", new Extern("ti_sysbios_knl_Task_getIdleTask__E", "ti_sysbios_knl_Task_Handle(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.getIdleTaskHandle", new Extern("ti_sysbios_knl_Task_getIdleTaskHandle__E", "ti_sysbios_knl_Task_Handle(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.knl.Task.startCore", new Extern("ti_sysbios_knl_Task_startCore__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.knl.Task.schedule", new Extern("ti_sysbios_knl_Task_schedule__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.enter", new Extern("ti_sysbios_knl_Task_enter__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.sleepTimeout", new Extern("ti_sysbios_knl_Task_sleepTimeout__I", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.knl.Task.postInit", new Extern("ti_sysbios_knl_Task_postInit__I", "xdc_Int(*)(ti_sysbios_knl_Task_Object*,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.knl.Task.allBlockedFunction", new Extern("ti_sysbios_knl_Task_allBlockedFunction__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.deleteTerminatedTasksFunc", new Extern("ti_sysbios_knl_Task_deleteTerminatedTasksFunc__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task.processVitalTaskFlag", new Extern("ti_sysbios_knl_Task_processVitalTaskFlag__I", "xdc_Void(*)(ti_sysbios_knl_Task_Object*)", true, false));
        om.bind("ti.sysbios.knl.Task.moduleStateCheck", new Extern("ti_sysbios_knl_Task_moduleStateCheck__I", "xdc_Int(*)(ti_sysbios_knl_Task_Module_State*,xdc_UInt32)", true, false));
        om.bind("ti.sysbios.knl.Task.getModuleStateCheckValue", new Extern("ti_sysbios_knl_Task_getModuleStateCheckValue__I", "xdc_UInt32(*)(ti_sysbios_knl_Task_Module_State*)", true, false));
        om.bind("ti.sysbios.knl.Task.objectCheck", new Extern("ti_sysbios_knl_Task_objectCheck__I", "xdc_Int(*)(ti_sysbios_knl_Task_Handle,xdc_UInt32)", true, false));
        om.bind("ti.sysbios.knl.Task.getObjectCheckValue", new Extern("ti_sysbios_knl_Task_getObjectCheckValue__I", "xdc_UInt32(*)(ti_sysbios_knl_Task_Handle)", true, false));
    }

    void Clock_TimerProxy$$CONSTS()
    {
        // module Clock_TimerProxy
        om.bind("ti.sysbios.knl.Clock_TimerProxy.getNumTimers", new Extern("ti_sysbios_knl_Clock_TimerProxy_getNumTimers__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Clock_TimerProxy.getStatus", new Extern("ti_sysbios_knl_Clock_TimerProxy_getStatus__E", "ti_sysbios_interfaces_ITimer_Status(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.knl.Clock_TimerProxy.startup", new Extern("ti_sysbios_knl_Clock_TimerProxy_startup__E", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void Intrinsics_SupportProxy$$CONSTS()
    {
        // module Intrinsics_SupportProxy
        om.bind("ti.sysbios.knl.Intrinsics_SupportProxy.maxbit", new Extern("ti_sysbios_knl_Intrinsics_SupportProxy_maxbit__E", "xdc_UInt(*)(xdc_UInt)", true, false));
    }

    void Task_SupportProxy$$CONSTS()
    {
        // module Task_SupportProxy
        om.bind("ti.sysbios.knl.Task_SupportProxy.start", new Extern("ti_sysbios_knl_Task_SupportProxy_start__E", "xdc_Ptr(*)(xdc_Ptr,xdc_Void(*)(xdc_Void),xdc_Void(*)(xdc_Void),xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.knl.Task_SupportProxy.swap", new Extern("ti_sysbios_knl_Task_SupportProxy_swap__E", "xdc_Void(*)(xdc_Ptr*,xdc_Ptr*)", true, false));
        om.bind("ti.sysbios.knl.Task_SupportProxy.checkStack", new Extern("ti_sysbios_knl_Task_SupportProxy_checkStack__E", "xdc_Bool(*)(xdc_Char*,xdc_SizeT)", true, false));
        om.bind("ti.sysbios.knl.Task_SupportProxy.stackUsed", new Extern("ti_sysbios_knl_Task_SupportProxy_stackUsed__E", "xdc_SizeT(*)(xdc_Char*,xdc_SizeT)", true, false));
        om.bind("ti.sysbios.knl.Task_SupportProxy.getStackAlignment", new Extern("ti_sysbios_knl_Task_SupportProxy_getStackAlignment__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task_SupportProxy.getDefaultStackSize", new Extern("ti_sysbios_knl_Task_SupportProxy_getDefaultStackSize__E", "xdc_SizeT(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.knl.Task_SupportProxy.getCheckValueAddr", new Extern("ti_sysbios_knl_Task_SupportProxy_getCheckValueAddr__E", "xdc_Ptr(*)(xdc_Ptr)", true, false));
    }

    void Clock$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Clock$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.knl.Clock.create() called before xdc.useModule('ti.sysbios.knl.Clock')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.clockFxn, inst.$args.timeout, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Clock$$create", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Clock.Module", "ti.sysbios.knl"), om.findStrict("ti.sysbios.knl.Clock.Instance", "ti.sysbios.knl"), 3, 2, false));
                        fxn.addArg(0, "clockFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(1, "timeout", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.knl.Clock.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Clock$$create = function( clockFxn, timeout, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Clock'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.knl.Clock.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.knl']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {clockFxn:clockFxn, timeout:timeout});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.knl.Clock'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [clockFxn, timeout]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Clock'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Clock$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Clock.Module", "ti.sysbios.knl"), null, 4, 2, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.knl.Clock$$Object", "ti.sysbios.knl"), null);
                        fxn.addArg(1, "clockFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "timeout", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF);
                        fxn.addArg(3, "__params", (Proto)om.findStrict("ti.sysbios.knl.Clock.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Clock$$construct = function( __obj, clockFxn, timeout, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Clock'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {clockFxn:clockFxn, timeout:timeout});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.knl.Clock'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Clock'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Idle$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Intrinsics$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Event$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Event$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.knl.Event.create() called before xdc.useModule('ti.sysbios.knl.Event')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Event$$create", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Event.Module", "ti.sysbios.knl"), om.findStrict("ti.sysbios.knl.Event.Instance", "ti.sysbios.knl"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.knl.Event.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Event$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Event'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.knl.Event.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.knl']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.knl.Event'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Event'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Event$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Event.Module", "ti.sysbios.knl"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.knl.Event$$Object", "ti.sysbios.knl"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.knl.Event.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Event$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Event'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.knl.Event'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Event'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Mailbox$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Mailbox$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.knl.Mailbox.create() called before xdc.useModule('ti.sysbios.knl.Mailbox')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.msgSize, inst.$args.numMsgs, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Mailbox$$create", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Mailbox.Module", "ti.sysbios.knl"), om.findStrict("ti.sysbios.knl.Mailbox.Instance", "ti.sysbios.knl"), 3, 2, false));
                        fxn.addArg(0, "msgSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF);
                        fxn.addArg(1, "numMsgs", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.knl.Mailbox.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Mailbox$$create = function( msgSize, numMsgs, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Mailbox'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.knl.Mailbox.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.knl']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {msgSize:msgSize, numMsgs:numMsgs});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.knl.Mailbox'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [msgSize, numMsgs]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Mailbox'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Mailbox$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Mailbox.Module", "ti.sysbios.knl"), null, 4, 2, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.knl.Mailbox$$Object", "ti.sysbios.knl"), null);
                        fxn.addArg(1, "msgSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF);
                        fxn.addArg(2, "numMsgs", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF);
                        fxn.addArg(3, "__params", (Proto)om.findStrict("ti.sysbios.knl.Mailbox.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Mailbox$$construct = function( __obj, msgSize, numMsgs, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Mailbox'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {msgSize:msgSize, numMsgs:numMsgs});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.knl.Mailbox'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Mailbox'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Queue$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Queue$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.knl.Queue.create() called before xdc.useModule('ti.sysbios.knl.Queue')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Queue$$create", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Queue.Module", "ti.sysbios.knl"), om.findStrict("ti.sysbios.knl.Queue.Instance", "ti.sysbios.knl"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.sysbios.knl.Queue.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Queue$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Queue'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.knl.Queue.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.knl']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.knl.Queue'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Queue'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Queue$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Queue.Module", "ti.sysbios.knl"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.knl.Queue$$Object", "ti.sysbios.knl"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.knl.Queue.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Queue$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Queue'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.knl.Queue'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Queue'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Semaphore$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Semaphore$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.knl.Semaphore.create() called before xdc.useModule('ti.sysbios.knl.Semaphore')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.count, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Semaphore$$create", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Semaphore.Module", "ti.sysbios.knl"), om.findStrict("ti.sysbios.knl.Semaphore.Instance", "ti.sysbios.knl"), 2, 1, false));
                        fxn.addArg(0, "count", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Semaphore$$create = function( count, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Semaphore'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.knl.Semaphore.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.knl']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {count:count});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.knl.Semaphore'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [count]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Semaphore'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Semaphore$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Semaphore.Module", "ti.sysbios.knl"), null, 3, 1, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.knl.Semaphore$$Object", "ti.sysbios.knl"), null);
                        fxn.addArg(1, "count", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Semaphore$$construct = function( __obj, count, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Semaphore'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {count:count});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.knl.Semaphore'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Semaphore'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Swi$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Swi$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.knl.Swi.create() called before xdc.useModule('ti.sysbios.knl.Swi')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.swiFxn, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Swi$$create", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Swi.Module", "ti.sysbios.knl"), om.findStrict("ti.sysbios.knl.Swi.Instance", "ti.sysbios.knl"), 2, 1, false));
                        fxn.addArg(0, "swiFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.knl.Swi.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Swi$$create = function( swiFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Swi'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.knl.Swi.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.knl']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {swiFxn:swiFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.knl.Swi'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [swiFxn]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Swi'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Swi$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Swi.Module", "ti.sysbios.knl"), null, 3, 1, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.knl.Swi$$Object", "ti.sysbios.knl"), null);
                        fxn.addArg(1, "swiFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.knl.Swi.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Swi$$construct = function( __obj, swiFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Swi'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {swiFxn:swiFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.knl.Swi'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Swi'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Task$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Task$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.knl.Task.create() called before xdc.useModule('ti.sysbios.knl.Task')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.fxn, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Task$$create", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Task.Module", "ti.sysbios.knl"), om.findStrict("ti.sysbios.knl.Task.Instance", "ti.sysbios.knl"), 2, 1, false));
                        fxn.addArg(0, "fxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.sysbios.knl.Task.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Task$$create = function( fxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Task'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.knl.Task.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.knl']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {fxn:fxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.knl.Task'].Instance_State);\n");
                sb.append("__inst.stackSection = __mod.PARAMS.stackSection;\n");
                sb.append("__inst.affinity = __mod.PARAMS.affinity;\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [fxn]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Task'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Task$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Task.Module", "ti.sysbios.knl"), null, 3, 1, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.knl.Task$$Object", "ti.sysbios.knl"), null);
                        fxn.addArg(1, "fxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.knl.Task.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Task$$construct = function( __obj, fxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Task'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {fxn:fxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.knl.Task'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("__inst.stackSection = __mod.PARAMS.stackSection;\n");
                sb.append("__inst.affinity = __mod.PARAMS.affinity;\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Task'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Clock_TimerProxy$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Clock_TimerProxy$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.knl.Clock_TimerProxy.create() called before xdc.useModule('ti.sysbios.knl.Clock_TimerProxy')\");\n");
                sb.append("}\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Clock_TimerProxy$$create", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Module", "ti.sysbios.knl"), om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Instance", "ti.sysbios.knl"), 3, 2, false));
                        fxn.addArg(0, "id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(1, "tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Params", "ti.sysbios.knl"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$knl$Clock_TimerProxy$$create = function( id, tickFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.knl.Clock_TimerProxy'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.knl.Clock_TimerProxy.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.knl']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {id:id, tickFxn:tickFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.knl.Clock_TimerProxy'].Instance_State);\n");
                sb.append("if (!__mod.delegate$) {\n");
                    sb.append("throw new Error(\"Unbound proxy module: ti.sysbios.knl.Clock_TimerProxy\");\n");
                sb.append("}\n");
                sb.append("var __dmod = __mod.delegate$.$orig;\n");
                sb.append("var __dinst = __dmod.create(id, tickFxn, __params);\n");
                sb.append("__inst.$$bind('delegate$', __dinst);\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [id, tickFxn]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.knl.Clock_TimerProxy'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Intrinsics_SupportProxy$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Task_SupportProxy$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Clock$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Idle$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Idle.addFunc
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Idle$$addFunc", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Idle.Module", "ti.sysbios.knl"), null, 1, 1, false));
                fxn.addArg(0, "func", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF);
        // fxn Idle.addCoreFunc
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Idle$$addCoreFunc", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Idle.Module", "ti.sysbios.knl"), null, 2, 2, false));
                fxn.addArg(0, "func", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF);
                fxn.addArg(1, "coreId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF);
    }

    void Intrinsics$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Event$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Event.syncMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Event$$syncMeta", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Event.Instance", "ti.sysbios.knl"), null, 2, 2, false));
                fxn.addArg(0, "eventId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF);
                fxn.addArg(1, "count", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF);
    }

    void Mailbox$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Queue$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Queue.elemClearMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Queue$$elemClearMeta", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Queue.Module", "ti.sysbios.knl"), null, 1, 1, false));
                fxn.addArg(0, "qelem", new Proto.Adr("ti_sysbios_knl_Queue_Elem*", "PS"), $$UNDEF);
        // fxn Queue.insertMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Queue$$insertMeta", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Queue.Module", "ti.sysbios.knl"), null, 2, 2, false));
                fxn.addArg(0, "qelem", new Proto.Adr("ti_sysbios_knl_Queue_Elem*", "PS"), $$UNDEF);
                fxn.addArg(1, "elem", new Proto.Adr("ti_sysbios_knl_Queue_Elem*", "PS"), $$UNDEF);
        // fxn Queue.headMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Queue$$headMeta", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Queue.Instance", "ti.sysbios.knl"), new Proto.Adr("xdc_Ptr", "Pv"), 0, 0, false));
        // fxn Queue.putMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Queue$$putMeta", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Queue.Instance", "ti.sysbios.knl"), null, 1, 1, false));
                fxn.addArg(0, "elem", new Proto.Adr("ti_sysbios_knl_Queue_Elem*", "PS"), $$UNDEF);
        // fxn Queue.nextMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Queue$$nextMeta", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Queue.Instance", "ti.sysbios.knl"), new Proto.Adr("xdc_Ptr", "Pv"), 1, 1, false));
                fxn.addArg(0, "qelem", new Proto.Adr("ti_sysbios_knl_Queue_Elem*", "PS"), $$UNDEF);
    }

    void Semaphore$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Swi$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Swi.addHookSet
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Swi$$addHookSet", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Swi.Module", "ti.sysbios.knl"), null, 1, 1, false));
                fxn.addArg(0, "hookSet", (Proto)om.findStrict("ti.sysbios.knl.Swi.HookSet", "ti.sysbios.knl"), $$DEFAULT);
    }

    void Task$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Task.addHookSet
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Task$$addHookSet", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Task.Module", "ti.sysbios.knl"), null, 1, 1, false));
                fxn.addArg(0, "hook", (Proto)om.findStrict("ti.sysbios.knl.Task.HookSet", "ti.sysbios.knl"), $$DEFAULT);
        // fxn Task.getNickName
        fxn = (Proto.Fxn)om.bind("ti.sysbios.knl.Task$$getNickName", new Proto.Fxn(om.findStrict("ti.sysbios.knl.Task.Module", "ti.sysbios.knl"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "tskView", $$T_Obj, $$UNDEF);
    }

    void Clock_TimerProxy$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Intrinsics_SupportProxy$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Task_SupportProxy$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Clock$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Clock.Instance_State", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("elem", "Sti.sysbios.knl.Queue;Elem"));
        sizes.add(Global.newArray("timeout", "UInt32"));
        sizes.add(Global.newArray("currTimeout", "UInt32"));
        sizes.add(Global.newArray("period", "UInt32"));
        sizes.add(Global.newArray("active", "UShort"));
        sizes.add(Global.newArray("fxn", "UFxn"));
        sizes.add(Global.newArray("arg", "UIArg"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Clock.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Clock.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Clock.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Clock.Module_State", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("ticks", "UInt32"));
        sizes.add(Global.newArray("swiCount", "UInt"));
        sizes.add(Global.newArray("timer", "UPtr"));
        sizes.add(Global.newArray("swi", "UPtr"));
        sizes.add(Global.newArray("numTickSkip", "UInt"));
        sizes.add(Global.newArray("nextScheduledTick", "UInt32"));
        sizes.add(Global.newArray("maxSkippable", "UInt32"));
        sizes.add(Global.newArray("inWorkFunc", "UShort"));
        sizes.add(Global.newArray("startDuringWorkFunc", "UShort"));
        sizes.add(Global.newArray("ticking", "UShort"));
        sizes.add(Global.newArray("clockQ", "Sti.sysbios.knl.Queue;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Clock.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Clock.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Clock.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Idle$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Intrinsics$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Event$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Event.PendElem", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("tpElem", "Sti.sysbios.knl.Task;PendElem"));
        sizes.add(Global.newArray("pendState", "Nti.sysbios.knl.Event.PendState;;0;1;2;3"));
        sizes.add(Global.newArray("matchingEvents", "UInt"));
        sizes.add(Global.newArray("andMask", "UInt"));
        sizes.add(Global.newArray("orMask", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Event.PendElem']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Event.PendElem']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Event.PendElem'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Event.Instance_State", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("postedEvents", "UInt"));
        sizes.add(Global.newArray("pendQ", "Sti.sysbios.knl.Queue;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Event.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Event.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Event.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Mailbox$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Mailbox.MbxElem", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("elem", "Sti.sysbios.knl.Queue;Elem"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Mailbox.MbxElem']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Mailbox.MbxElem']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Mailbox.MbxElem'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Mailbox.Instance_State", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("heap", "UPtr"));
        sizes.add(Global.newArray("msgSize", "USize"));
        sizes.add(Global.newArray("numMsgs", "UInt"));
        sizes.add(Global.newArray("buf", "UPtr"));
        sizes.add(Global.newArray("numFreeMsgs", "UInt"));
        sizes.add(Global.newArray("allocBuf", "UPtr"));
        sizes.add(Global.newArray("dataQue", "Sti.sysbios.knl.Queue;Instance_State"));
        sizes.add(Global.newArray("freeQue", "Sti.sysbios.knl.Queue;Instance_State"));
        sizes.add(Global.newArray("dataSem", "Sti.sysbios.knl.Semaphore;Instance_State"));
        sizes.add(Global.newArray("freeSem", "Sti.sysbios.knl.Semaphore;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Mailbox.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Mailbox.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Mailbox.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Queue$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("next", "UPtr"));
        sizes.add(Global.newArray("prev", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Queue.Elem']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Queue.Elem']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Queue.Elem'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Queue.Instance_State", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("elem", "Sti.sysbios.knl.Queue;Elem"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Queue.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Queue.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Queue.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Semaphore$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Semaphore.PendElem", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("tpElem", "Sti.sysbios.knl.Task;PendElem"));
        sizes.add(Global.newArray("pendState", "Nti.sysbios.knl.Semaphore.PendState;;0;1;2;3"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Semaphore.PendElem']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Semaphore.PendElem']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Semaphore.PendElem'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Semaphore.Instance_State", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("event", "UPtr"));
        sizes.add(Global.newArray("eventId", "UInt"));
        sizes.add(Global.newArray("mode", "Nti.sysbios.knl.Semaphore.Mode;;0x0;0x1;0x2;0x3"));
        sizes.add(Global.newArray("count", "UInt16"));
        sizes.add(Global.newArray("pendQ", "Sti.sysbios.knl.Queue;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Semaphore.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Semaphore.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Semaphore.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Swi$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Swi.HookSet", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("registerFxn", "UFxn"));
        sizes.add(Global.newArray("createFxn", "UFxn"));
        sizes.add(Global.newArray("readyFxn", "UFxn"));
        sizes.add(Global.newArray("beginFxn", "UFxn"));
        sizes.add(Global.newArray("endFxn", "UFxn"));
        sizes.add(Global.newArray("deleteFxn", "UFxn"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Swi.HookSet']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Swi.HookSet']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Swi.HookSet'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Swi.Struct2__", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("qElem", "Sti.sysbios.knl.Queue;Elem"));
        sizes.add(Global.newArray("fxn", "UFxn"));
        sizes.add(Global.newArray("arg0", "UIArg"));
        sizes.add(Global.newArray("arg1", "UIArg"));
        sizes.add(Global.newArray("priority", "UInt"));
        sizes.add(Global.newArray("mask", "UInt"));
        sizes.add(Global.newArray("posted", "UShort"));
        sizes.add(Global.newArray("initTrigger", "UInt"));
        sizes.add(Global.newArray("trigger", "UInt"));
        sizes.add(Global.newArray("readyQ", "UPtr"));
        sizes.add(Global.newArray("hookEnv", "UPtr"));
        sizes.add(Global.newArray("name", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Swi.Struct2__']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Swi.Struct2__']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Swi.Struct2__'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Swi.Instance_State", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("qElem", "Sti.sysbios.knl.Queue;Elem"));
        sizes.add(Global.newArray("fxn", "UFxn"));
        sizes.add(Global.newArray("arg0", "UIArg"));
        sizes.add(Global.newArray("arg1", "UIArg"));
        sizes.add(Global.newArray("priority", "UInt"));
        sizes.add(Global.newArray("mask", "UInt"));
        sizes.add(Global.newArray("posted", "UShort"));
        sizes.add(Global.newArray("initTrigger", "UInt"));
        sizes.add(Global.newArray("trigger", "UInt"));
        sizes.add(Global.newArray("readyQ", "UPtr"));
        sizes.add(Global.newArray("hookEnv", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Swi.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Swi.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Swi.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Swi.Module_State", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("locked", "UShort"));
        sizes.add(Global.newArray("curSet", "UInt"));
        sizes.add(Global.newArray("curTrigger", "UInt"));
        sizes.add(Global.newArray("curSwi", "UPtr"));
        sizes.add(Global.newArray("curQ", "UPtr"));
        sizes.add(Global.newArray("readyQ", "UPtr"));
        sizes.add(Global.newArray("constructedSwis", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Swi.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Swi.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Swi.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Task$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Task.Stat", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("priority", "TInt"));
        sizes.add(Global.newArray("stack", "UPtr"));
        sizes.add(Global.newArray("stackSize", "USize"));
        sizes.add(Global.newArray("stackHeap", "UPtr"));
        sizes.add(Global.newArray("env", "UPtr"));
        sizes.add(Global.newArray("mode", "Nti.sysbios.knl.Task.Mode;;;;;;"));
        sizes.add(Global.newArray("sp", "UPtr"));
        sizes.add(Global.newArray("used", "USize"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Task.Stat']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Task.Stat']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Task.Stat'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Task.HookSet", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("registerFxn", "UFxn"));
        sizes.add(Global.newArray("createFxn", "UFxn"));
        sizes.add(Global.newArray("readyFxn", "UFxn"));
        sizes.add(Global.newArray("switchFxn", "UFxn"));
        sizes.add(Global.newArray("exitFxn", "UFxn"));
        sizes.add(Global.newArray("deleteFxn", "UFxn"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Task.HookSet']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Task.HookSet']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Task.HookSet'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Task.PendElem", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("qElem", "Sti.sysbios.knl.Queue;Elem"));
        sizes.add(Global.newArray("task", "UPtr"));
        sizes.add(Global.newArray("clock", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Task.PendElem']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Task.PendElem']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Task.PendElem'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Task.Instance_State", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("qElem", "Sti.sysbios.knl.Queue;Elem"));
        sizes.add(Global.newArray("priority", "TInt"));
        sizes.add(Global.newArray("mask", "UInt"));
        sizes.add(Global.newArray("context", "UPtr"));
        sizes.add(Global.newArray("mode", "Nti.sysbios.knl.Task.Mode;;;;;;"));
        sizes.add(Global.newArray("pendElem", "UPtr"));
        sizes.add(Global.newArray("stackSize", "USize"));
        sizes.add(Global.newArray("stack", "UPtr"));
        sizes.add(Global.newArray("stackHeap", "UPtr"));
        sizes.add(Global.newArray("fxn", "UFxn"));
        sizes.add(Global.newArray("arg0", "UIArg"));
        sizes.add(Global.newArray("arg1", "UIArg"));
        sizes.add(Global.newArray("env", "UPtr"));
        sizes.add(Global.newArray("hookEnv", "UPtr"));
        sizes.add(Global.newArray("vitalTaskFlag", "UShort"));
        sizes.add(Global.newArray("readyQ", "UPtr"));
        sizes.add(Global.newArray("curCoreId", "UInt"));
        sizes.add(Global.newArray("affinity", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Task.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Task.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Task.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Task.Module_State", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("locked", "UShort"));
        sizes.add(Global.newArray("curSet", "UInt"));
        sizes.add(Global.newArray("workFlag", "UShort"));
        sizes.add(Global.newArray("vitalTasks", "UInt"));
        sizes.add(Global.newArray("curTask", "UPtr"));
        sizes.add(Global.newArray("curQ", "UPtr"));
        sizes.add(Global.newArray("readyQ", "UPtr"));
        sizes.add(Global.newArray("smpCurSet", "UPtr"));
        sizes.add(Global.newArray("smpCurMask", "UPtr"));
        sizes.add(Global.newArray("smpCurTask", "UPtr"));
        sizes.add(Global.newArray("smpReadyQ", "UPtr"));
        sizes.add(Global.newArray("idleTask", "UPtr"));
        sizes.add(Global.newArray("constructedTasks", "UPtr"));
        sizes.add(Global.newArray("inactiveQ", "Sti.sysbios.knl.Queue;Instance_State"));
        sizes.add(Global.newArray("terminatedQ", "Sti.sysbios.knl.Queue;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Task.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Task.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Task.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Task.RunQEntry", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("elem", "Sti.sysbios.knl.Queue;Elem"));
        sizes.add(Global.newArray("coreId", "UInt"));
        sizes.add(Global.newArray("priority", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Task.RunQEntry']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Task.RunQEntry']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Task.RunQEntry'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.knl.Task.Module_StateSmp", "ti.sysbios.knl");
        sizes.clear();
        sizes.add(Global.newArray("sortedRunQ", "UPtr"));
        sizes.add(Global.newArray("smpRunQ", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.knl.Task.Module_StateSmp']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.knl.Task.Module_StateSmp']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.knl.Task.Module_StateSmp'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Clock_TimerProxy$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Intrinsics_SupportProxy$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Task_SupportProxy$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Clock$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/knl/Clock.xs");
        om.bind("ti.sysbios.knl.Clock$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.knl"), $$UNDEF, "wh");
            po.addFldV("TimerProxy", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.knl"), null, "wh", $$delegGet, $$delegSet);
            po.addFld("LW_delayed", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", 0x0400L, "msg", "LW_delayed: delay: %d"), "w");
            po.addFld("LM_tick", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_tick: tick: %d"), "w");
            po.addFld("LM_begin", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_begin: clk: 0x%x, func: 0x%x"), "w");
            po.addFld("A_clockDisabled", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_clockDisabled: Cannot create a clock instance when BIOS.clockEnabled is false."), "w");
            po.addFld("A_badThreadType", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_badThreadType: Cannot create/delete a Clock from Hwi or Swi thread."), "w");
            po.addFld("serviceMargin", Proto.Elm.newCNum("(xdc_UInt32)"), 0L, "w");
            po.addFld("tickSource", (Proto)om.findStrict("ti.sysbios.knl.Clock.TickSource", "ti.sysbios.knl"), om.find("ti.sysbios.knl.Clock.TickSource_TIMER"), "w");
            po.addFld("tickMode", (Proto)om.findStrict("ti.sysbios.knl.Clock.TickMode", "ti.sysbios.knl"), $$UNDEF, "w");
            po.addFld("timerId", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "w");
            po.addFld("swiPriority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
            po.addFld("tickPeriod", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
            po.addFld("stopCheckNext", $$T_Bool, $$UNDEF, "wh");
            po.addFld("timerSupportsDynamic", $$T_Bool, false, "wh");
            po.addFld("doTickFunc", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF, "w");
            po.addFld("triggerClock", (Proto)om.findStrict("ti.sysbios.knl.Clock.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Clock$$create", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Clock$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Clock$$construct", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Clock$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.knl.Clock$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Clock$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Clock$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Clock$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.knl.Clock$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Clock$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock.Instance", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("startFlag", $$T_Bool, false, "w");
            po.addFld("period", Proto.Elm.newCNum("(xdc_UInt32)"), 0L, "w");
            po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), null, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock$$Params", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("startFlag", $$T_Bool, false, "w");
            po.addFld("period", Proto.Elm.newCNum("(xdc_UInt32)"), 0L, "w");
            po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), null, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock$$Object", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock.Object", om.findStrict("ti.sysbios.knl.Clock.Instance", "ti.sysbios.knl"));
        // struct Clock.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock$$BasicView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("timeout", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("fxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("started", $$T_Bool, $$UNDEF, "w");
                po.addFld("tRemaining", $$T_Str, $$UNDEF, "w");
                po.addFld("periodic", $$T_Bool, $$UNDEF, "w");
        // struct Clock.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock$$ModuleView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("ticks", $$T_Str, $$UNDEF, "w");
                po.addFld("tickSource", $$T_Str, $$UNDEF, "w");
                po.addFld("tickMode", $$T_Str, $$UNDEF, "w");
                po.addFld("timerHandle", $$T_Str, $$UNDEF, "w");
                po.addFld("timerId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("swiPriority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("tickPeriod", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("nSkip", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // typedef Clock.FuncPtr
        om.bind("ti.sysbios.knl.Clock.FuncPtr", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"));
        // struct Clock.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock$$Instance_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("elem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFld("timeout", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("currTimeout", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("active", $$T_Bool, $$UNDEF, "w");
                po.addFld("fxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF, "w");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
        // struct Clock.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock$$Module_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ticks", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("swiCount", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("timer", (Proto)om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("swi", (Proto)om.findStrict("ti.sysbios.knl.Swi.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("numTickSkip", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("nextScheduledTick", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("maxSkippable", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("inWorkFunc", $$T_Bool, $$UNDEF, "w");
                po.addFld("startDuringWorkFunc", $$T_Bool, $$UNDEF, "w");
                po.addFld("ticking", $$T_Bool, $$UNDEF, "w");
                po.addFldV("clockQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_clockQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "w");
    }

    void Idle$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/knl/Idle.xs");
        om.bind("ti.sysbios.knl.Idle$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Idle.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Idle.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.knl"), $$UNDEF, "wh");
            po.addFld("funcList", new Proto.Arr(new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), true), Global.newArray(new Object[]{}), "w");
            po.addFld("coreList", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt)"), true), Global.newArray(new Object[]{}), "w");
            po.addFld("idleFxns", new Proto.Arr(new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), true), Global.newArray(new Object[]{null, null, null, null, null, null, null, null}), "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.knl.Idle$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Idle$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Idle$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.knl.Idle$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("addFunc", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Idle$$addFunc", "ti.sysbios.knl"), Global.get(cap, "addFunc"));
                po.addFxn("addCoreFunc", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Idle$$addCoreFunc", "ti.sysbios.knl"), Global.get(cap, "addCoreFunc"));
        // typedef Idle.FuncPtr
        om.bind("ti.sysbios.knl.Idle.FuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"));
        // struct Idle.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Idle$$ModuleView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Idle.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("index", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("coreId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("fxn", $$T_Str, $$UNDEF, "w");
    }

    void Intrinsics$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/knl/Intrinsics.xs");
        om.bind("ti.sysbios.knl.Intrinsics$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Intrinsics.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Intrinsics.Module", om.findStrict("ti.sysbios.interfaces.IIntrinsicsSupport.Module", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFldV("SupportProxy", (Proto)om.findStrict("ti.sysbios.interfaces.IIntrinsicsSupport.Module", "ti.sysbios.knl"), null, "wh", $$delegGet, $$delegSet);
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.knl.Intrinsics$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Intrinsics$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Intrinsics$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.knl.Intrinsics$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void Event$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/knl/Event.xs");
        om.bind("ti.sysbios.knl.Event$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Event.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Event.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Id_00", Proto.Elm.newCNum("(xdc_UInt)"), 0x1L, "rh");
                po.addFld("Id_01", Proto.Elm.newCNum("(xdc_UInt)"), 0x2L, "rh");
                po.addFld("Id_02", Proto.Elm.newCNum("(xdc_UInt)"), 0x4L, "rh");
                po.addFld("Id_03", Proto.Elm.newCNum("(xdc_UInt)"), 0x8L, "rh");
                po.addFld("Id_04", Proto.Elm.newCNum("(xdc_UInt)"), 0x10L, "rh");
                po.addFld("Id_05", Proto.Elm.newCNum("(xdc_UInt)"), 0x20L, "rh");
                po.addFld("Id_06", Proto.Elm.newCNum("(xdc_UInt)"), 0x40L, "rh");
                po.addFld("Id_07", Proto.Elm.newCNum("(xdc_UInt)"), 0x80L, "rh");
                po.addFld("Id_08", Proto.Elm.newCNum("(xdc_UInt)"), 0x100L, "rh");
                po.addFld("Id_09", Proto.Elm.newCNum("(xdc_UInt)"), 0x200L, "rh");
                po.addFld("Id_10", Proto.Elm.newCNum("(xdc_UInt)"), 0x400L, "rh");
                po.addFld("Id_11", Proto.Elm.newCNum("(xdc_UInt)"), 0x800L, "rh");
                po.addFld("Id_12", Proto.Elm.newCNum("(xdc_UInt)"), 0x1000L, "rh");
                po.addFld("Id_13", Proto.Elm.newCNum("(xdc_UInt)"), 0x2000L, "rh");
                po.addFld("Id_14", Proto.Elm.newCNum("(xdc_UInt)"), 0x4000L, "rh");
                po.addFld("Id_15", Proto.Elm.newCNum("(xdc_UInt)"), 0x8000L, "rh");
                po.addFld("Id_16", Proto.Elm.newCNum("(xdc_UInt)"), 0x10000L, "rh");
                po.addFld("Id_17", Proto.Elm.newCNum("(xdc_UInt)"), 0x20000L, "rh");
                po.addFld("Id_18", Proto.Elm.newCNum("(xdc_UInt)"), 0x40000L, "rh");
                po.addFld("Id_19", Proto.Elm.newCNum("(xdc_UInt)"), 0x80000L, "rh");
                po.addFld("Id_20", Proto.Elm.newCNum("(xdc_UInt)"), 0x100000L, "rh");
                po.addFld("Id_21", Proto.Elm.newCNum("(xdc_UInt)"), 0x200000L, "rh");
                po.addFld("Id_22", Proto.Elm.newCNum("(xdc_UInt)"), 0x400000L, "rh");
                po.addFld("Id_23", Proto.Elm.newCNum("(xdc_UInt)"), 0x800000L, "rh");
                po.addFld("Id_24", Proto.Elm.newCNum("(xdc_UInt)"), 0x1000000L, "rh");
                po.addFld("Id_25", Proto.Elm.newCNum("(xdc_UInt)"), 0x2000000L, "rh");
                po.addFld("Id_26", Proto.Elm.newCNum("(xdc_UInt)"), 0x4000000L, "rh");
                po.addFld("Id_27", Proto.Elm.newCNum("(xdc_UInt)"), 0x8000000L, "rh");
                po.addFld("Id_28", Proto.Elm.newCNum("(xdc_UInt)"), 0x10000000L, "rh");
                po.addFld("Id_29", Proto.Elm.newCNum("(xdc_UInt)"), 0x20000000L, "rh");
                po.addFld("Id_30", Proto.Elm.newCNum("(xdc_UInt)"), 0x40000000L, "rh");
                po.addFld("Id_31", Proto.Elm.newCNum("(xdc_UInt)"), 0x80000000L, "rh");
                po.addFld("Id_NONE", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.knl"), $$UNDEF, "wh");
            po.addFld("LM_post", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_post: event: 0x%x, currEvents: 0x%x, eventId: 0x%x"), "w");
            po.addFld("LM_pend", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_pend: event: 0x%x, currEvents: 0x%x, andMask: 0x%x, orMask: 0x%x, timeout: %d"), "w");
            po.addFld("A_nullEventMasks", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_nullEventMasks: orMask and andMask are null."), "w");
            po.addFld("A_nullEventId", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_nullEventId: posted eventId is null."), "w");
            po.addFld("A_eventInUse", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_eventInUse: Event object already in use."), "w");
            po.addFld("A_badContext", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_badContext: bad calling context. Must be called from a Task."), "w");
            po.addFld("A_pendTaskDisabled", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_pendTaskDisabled: Cannot call Event_pend() while the Task or Swi scheduler is disabled."), "w");
            po.addFld("eventInstances", new Proto.Arr($$T_Obj, false), $$DEFAULT, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Event$$create", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Event$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Event$$construct", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Event$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.knl.Event$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Event$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Event$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Event$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.knl.Event$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Event$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Event.Instance", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Event.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Id_00", Proto.Elm.newCNum("(xdc_UInt)"), 0x1L, "rh");
                po.addFld("Id_01", Proto.Elm.newCNum("(xdc_UInt)"), 0x2L, "rh");
                po.addFld("Id_02", Proto.Elm.newCNum("(xdc_UInt)"), 0x4L, "rh");
                po.addFld("Id_03", Proto.Elm.newCNum("(xdc_UInt)"), 0x8L, "rh");
                po.addFld("Id_04", Proto.Elm.newCNum("(xdc_UInt)"), 0x10L, "rh");
                po.addFld("Id_05", Proto.Elm.newCNum("(xdc_UInt)"), 0x20L, "rh");
                po.addFld("Id_06", Proto.Elm.newCNum("(xdc_UInt)"), 0x40L, "rh");
                po.addFld("Id_07", Proto.Elm.newCNum("(xdc_UInt)"), 0x80L, "rh");
                po.addFld("Id_08", Proto.Elm.newCNum("(xdc_UInt)"), 0x100L, "rh");
                po.addFld("Id_09", Proto.Elm.newCNum("(xdc_UInt)"), 0x200L, "rh");
                po.addFld("Id_10", Proto.Elm.newCNum("(xdc_UInt)"), 0x400L, "rh");
                po.addFld("Id_11", Proto.Elm.newCNum("(xdc_UInt)"), 0x800L, "rh");
                po.addFld("Id_12", Proto.Elm.newCNum("(xdc_UInt)"), 0x1000L, "rh");
                po.addFld("Id_13", Proto.Elm.newCNum("(xdc_UInt)"), 0x2000L, "rh");
                po.addFld("Id_14", Proto.Elm.newCNum("(xdc_UInt)"), 0x4000L, "rh");
                po.addFld("Id_15", Proto.Elm.newCNum("(xdc_UInt)"), 0x8000L, "rh");
                po.addFld("Id_16", Proto.Elm.newCNum("(xdc_UInt)"), 0x10000L, "rh");
                po.addFld("Id_17", Proto.Elm.newCNum("(xdc_UInt)"), 0x20000L, "rh");
                po.addFld("Id_18", Proto.Elm.newCNum("(xdc_UInt)"), 0x40000L, "rh");
                po.addFld("Id_19", Proto.Elm.newCNum("(xdc_UInt)"), 0x80000L, "rh");
                po.addFld("Id_20", Proto.Elm.newCNum("(xdc_UInt)"), 0x100000L, "rh");
                po.addFld("Id_21", Proto.Elm.newCNum("(xdc_UInt)"), 0x200000L, "rh");
                po.addFld("Id_22", Proto.Elm.newCNum("(xdc_UInt)"), 0x400000L, "rh");
                po.addFld("Id_23", Proto.Elm.newCNum("(xdc_UInt)"), 0x800000L, "rh");
                po.addFld("Id_24", Proto.Elm.newCNum("(xdc_UInt)"), 0x1000000L, "rh");
                po.addFld("Id_25", Proto.Elm.newCNum("(xdc_UInt)"), 0x2000000L, "rh");
                po.addFld("Id_26", Proto.Elm.newCNum("(xdc_UInt)"), 0x4000000L, "rh");
                po.addFld("Id_27", Proto.Elm.newCNum("(xdc_UInt)"), 0x8000000L, "rh");
                po.addFld("Id_28", Proto.Elm.newCNum("(xdc_UInt)"), 0x10000000L, "rh");
                po.addFld("Id_29", Proto.Elm.newCNum("(xdc_UInt)"), 0x20000000L, "rh");
                po.addFld("Id_30", Proto.Elm.newCNum("(xdc_UInt)"), 0x40000000L, "rh");
                po.addFld("Id_31", Proto.Elm.newCNum("(xdc_UInt)"), 0x80000000L, "rh");
                po.addFld("Id_NONE", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
                po.addFxn("syncMeta", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Event$$syncMeta", "ti.sysbios.knl"), Global.get(cap, "syncMeta"));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Event$$Params", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Event.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Id_00", Proto.Elm.newCNum("(xdc_UInt)"), 0x1L, "rh");
                po.addFld("Id_01", Proto.Elm.newCNum("(xdc_UInt)"), 0x2L, "rh");
                po.addFld("Id_02", Proto.Elm.newCNum("(xdc_UInt)"), 0x4L, "rh");
                po.addFld("Id_03", Proto.Elm.newCNum("(xdc_UInt)"), 0x8L, "rh");
                po.addFld("Id_04", Proto.Elm.newCNum("(xdc_UInt)"), 0x10L, "rh");
                po.addFld("Id_05", Proto.Elm.newCNum("(xdc_UInt)"), 0x20L, "rh");
                po.addFld("Id_06", Proto.Elm.newCNum("(xdc_UInt)"), 0x40L, "rh");
                po.addFld("Id_07", Proto.Elm.newCNum("(xdc_UInt)"), 0x80L, "rh");
                po.addFld("Id_08", Proto.Elm.newCNum("(xdc_UInt)"), 0x100L, "rh");
                po.addFld("Id_09", Proto.Elm.newCNum("(xdc_UInt)"), 0x200L, "rh");
                po.addFld("Id_10", Proto.Elm.newCNum("(xdc_UInt)"), 0x400L, "rh");
                po.addFld("Id_11", Proto.Elm.newCNum("(xdc_UInt)"), 0x800L, "rh");
                po.addFld("Id_12", Proto.Elm.newCNum("(xdc_UInt)"), 0x1000L, "rh");
                po.addFld("Id_13", Proto.Elm.newCNum("(xdc_UInt)"), 0x2000L, "rh");
                po.addFld("Id_14", Proto.Elm.newCNum("(xdc_UInt)"), 0x4000L, "rh");
                po.addFld("Id_15", Proto.Elm.newCNum("(xdc_UInt)"), 0x8000L, "rh");
                po.addFld("Id_16", Proto.Elm.newCNum("(xdc_UInt)"), 0x10000L, "rh");
                po.addFld("Id_17", Proto.Elm.newCNum("(xdc_UInt)"), 0x20000L, "rh");
                po.addFld("Id_18", Proto.Elm.newCNum("(xdc_UInt)"), 0x40000L, "rh");
                po.addFld("Id_19", Proto.Elm.newCNum("(xdc_UInt)"), 0x80000L, "rh");
                po.addFld("Id_20", Proto.Elm.newCNum("(xdc_UInt)"), 0x100000L, "rh");
                po.addFld("Id_21", Proto.Elm.newCNum("(xdc_UInt)"), 0x200000L, "rh");
                po.addFld("Id_22", Proto.Elm.newCNum("(xdc_UInt)"), 0x400000L, "rh");
                po.addFld("Id_23", Proto.Elm.newCNum("(xdc_UInt)"), 0x800000L, "rh");
                po.addFld("Id_24", Proto.Elm.newCNum("(xdc_UInt)"), 0x1000000L, "rh");
                po.addFld("Id_25", Proto.Elm.newCNum("(xdc_UInt)"), 0x2000000L, "rh");
                po.addFld("Id_26", Proto.Elm.newCNum("(xdc_UInt)"), 0x4000000L, "rh");
                po.addFld("Id_27", Proto.Elm.newCNum("(xdc_UInt)"), 0x8000000L, "rh");
                po.addFld("Id_28", Proto.Elm.newCNum("(xdc_UInt)"), 0x10000000L, "rh");
                po.addFld("Id_29", Proto.Elm.newCNum("(xdc_UInt)"), 0x20000000L, "rh");
                po.addFld("Id_30", Proto.Elm.newCNum("(xdc_UInt)"), 0x40000000L, "rh");
                po.addFld("Id_31", Proto.Elm.newCNum("(xdc_UInt)"), 0x80000000L, "rh");
                po.addFld("Id_NONE", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Event$$Object", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Event.Object", om.findStrict("ti.sysbios.knl.Event.Instance", "ti.sysbios.knl"));
                po.addFxn("syncMeta", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Event$$syncMeta", "ti.sysbios.knl"), Global.get(cap, "syncMeta"));
        // struct Event.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Event$$BasicView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Event.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("postedEvents", $$T_Str, $$UNDEF, "w");
                po.addFld("pendedTask", $$T_Str, $$UNDEF, "w");
                po.addFld("andMask", $$T_Str, $$UNDEF, "w");
                po.addFld("orMask", $$T_Str, $$UNDEF, "w");
                po.addFld("timeout", $$T_Str, $$UNDEF, "w");
        // struct Event.PendElem
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Event$$PendElem", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Event.PendElem", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("tpElem", (Proto)om.findStrict("ti.sysbios.knl.Task.PendElem", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFld("pendState", (Proto)om.findStrict("ti.sysbios.knl.Event.PendState", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("matchingEvents", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("andMask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("orMask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct Event.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Event$$Instance_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Event.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("postedEvents", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFldV("pendQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_pendQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "w");
    }

    void Mailbox$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/knl/Mailbox.xs");
        om.bind("ti.sysbios.knl.Mailbox$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Mailbox.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.knl"), $$UNDEF, "wh");
            po.addFld("A_invalidBufSize", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "Mailbox_create's bufSize parameter is invalid (too small)"), "w");
            po.addFld("maxTypeAlign", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Mailbox$$create", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Mailbox$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Mailbox$$construct", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Mailbox$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.knl.Mailbox$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Mailbox$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Mailbox$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Mailbox$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.knl.Mailbox$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Mailbox$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox.Instance", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Mailbox.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("heap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.knl"), null, "w");
            po.addFld("sectionName", $$T_Str, null, "wh");
            po.addFld("readerEvent", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.knl"), null, "w");
            po.addFld("readerEventId", Proto.Elm.newCNum("(xdc_UInt)"), 1L, "w");
            po.addFld("writerEvent", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.knl"), null, "w");
            po.addFld("writerEventId", Proto.Elm.newCNum("(xdc_UInt)"), 1L, "w");
            po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), null, "w");
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox$$Params", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Mailbox.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("heap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.knl"), null, "w");
            po.addFld("sectionName", $$T_Str, null, "wh");
            po.addFld("readerEvent", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.knl"), null, "w");
            po.addFld("readerEventId", Proto.Elm.newCNum("(xdc_UInt)"), 1L, "w");
            po.addFld("writerEvent", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.knl"), null, "w");
            po.addFld("writerEventId", Proto.Elm.newCNum("(xdc_UInt)"), 1L, "w");
            po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), null, "w");
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox$$Object", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Mailbox.Object", om.findStrict("ti.sysbios.knl.Mailbox.Instance", "ti.sysbios.knl"));
        // struct Mailbox.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox$$BasicView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Mailbox.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("msgSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("numMsgs", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct Mailbox.DetailedView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox$$DetailedView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Mailbox.DetailedView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("msgSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("numMsgs", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("curNumMsgs", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("freeSlots", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("pendQueue", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("postQueue", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
        // struct Mailbox.MbxElem
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox$$MbxElem", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Mailbox.MbxElem", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("elem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl"), $$DEFAULT, "w");
        // struct Mailbox.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox$$Instance_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Mailbox.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("heap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("msgSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("numMsgs", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("buf", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("numFreeMsgs", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("allocBuf", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFldV("dataQue", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_dataQue", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFldV("freeQue", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_freeQue", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFldV("dataSem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.knl"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_dataSem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFldV("freeSem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.knl"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_freeSem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.knl"), $$DEFAULT, "w");
    }

    void Queue$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/knl/Queue.xs");
        om.bind("ti.sysbios.knl.Queue$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Queue.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Queue.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.knl"), $$UNDEF, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Queue$$create", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Queue$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Queue$$construct", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Queue$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.knl.Queue$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Queue$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Queue$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Queue$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.knl.Queue$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Queue$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                po.addFxn("elemClearMeta", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Queue$$elemClearMeta", "ti.sysbios.knl"), Global.get(cap, "elemClearMeta"));
                po.addFxn("insertMeta", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Queue$$insertMeta", "ti.sysbios.knl"), Global.get(cap, "insertMeta"));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Queue.Instance", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Queue.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("dummy", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
                po.addFxn("headMeta", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Queue$$headMeta", "ti.sysbios.knl"), Global.get(cap, "headMeta"));
                po.addFxn("putMeta", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Queue$$putMeta", "ti.sysbios.knl"), Global.get(cap, "putMeta"));
                po.addFxn("nextMeta", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Queue$$nextMeta", "ti.sysbios.knl"), Global.get(cap, "nextMeta"));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Queue$$Params", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Queue.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("dummy", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Queue$$Object", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Queue.Object", om.findStrict("ti.sysbios.knl.Queue.Instance", "ti.sysbios.knl"));
                po.addFxn("headMeta", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Queue$$headMeta", "ti.sysbios.knl"), Global.get(cap, "headMeta"));
                po.addFxn("putMeta", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Queue$$putMeta", "ti.sysbios.knl"), Global.get(cap, "putMeta"));
                po.addFxn("nextMeta", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Queue$$nextMeta", "ti.sysbios.knl"), Global.get(cap, "nextMeta"));
        // struct Queue.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Queue$$BasicView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Queue.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("elems", new Proto.Arr(new Proto.Adr("xdc_Ptr", "Pv"), false), $$DEFAULT, "w");
        // struct Queue.Elem
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Queue$$Elem", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Queue.Elem", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("next", new Proto.Adr("ti_sysbios_knl_Queue_Elem*", "PS"), $$UNDEF, "w");
                po.addFld("prev", new Proto.Adr("ti_sysbios_knl_Queue_Elem*", "PS"), $$UNDEF, "w");
        // struct Queue.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Queue$$Instance_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Queue.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("elem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl"), $$DEFAULT, "w");
    }

    void Semaphore$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/knl/Semaphore.xs");
        om.bind("ti.sysbios.knl.Semaphore$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Semaphore.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Semaphore.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.knl"), $$UNDEF, "wh");
            po.addFld("LM_post", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_post: sem: 0x%x, count: %d"), "w");
            po.addFld("LM_pend", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_pend: sem: 0x%x, count: %d, timeout: %d"), "w");
            po.addFld("A_noEvents", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_noEvents: The Event.supportsEvents flag is disabled."), "w");
            po.addFld("A_invTimeout", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_invTimeout: Can't use BIOS_EVENT_ACQUIRED with this Semaphore."), "w");
            po.addFld("A_badContext", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_badContext: bad calling context. Must be called from a Task."), "w");
            po.addFld("A_overflow", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_overflow: Count has exceeded 65535 and rolled over."), "w");
            po.addFld("A_pendTaskDisabled", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_pendTaskDisabled: Cannot call Semaphore_pend() while the Task or Swi scheduler is disabled."), "w");
            po.addFld("supportsEvents", $$T_Bool, false, "w");
            po.addFld("supportsPriority", $$T_Bool, true, "w");
            po.addFld("eventPost", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Event_Handle,xdc_UInt)", "PFv"), $$UNDEF, "w");
            po.addFld("eventSync", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Event_Handle,xdc_UInt,xdc_UInt)", "PFv"), $$UNDEF, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Semaphore$$create", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Semaphore$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Semaphore$$construct", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Semaphore$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.knl.Semaphore$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Semaphore$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Semaphore$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Semaphore$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.knl.Semaphore$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Semaphore$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Semaphore.Instance", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Semaphore.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("event", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.knl"), null, "w");
            po.addFld("eventId", Proto.Elm.newCNum("(xdc_UInt)"), 1L, "w");
            po.addFld("mode", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Mode", "ti.sysbios.knl"), om.find("ti.sysbios.knl.Semaphore.Mode_COUNTING"), "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Semaphore$$Params", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Semaphore.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("event", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.knl"), null, "w");
            po.addFld("eventId", Proto.Elm.newCNum("(xdc_UInt)"), 1L, "w");
            po.addFld("mode", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Mode", "ti.sysbios.knl"), om.find("ti.sysbios.knl.Semaphore.Mode_COUNTING"), "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Semaphore$$Object", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Semaphore.Object", om.findStrict("ti.sysbios.knl.Semaphore.Instance", "ti.sysbios.knl"));
        // struct Semaphore.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Semaphore$$BasicView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Semaphore.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("event", $$T_Str, $$UNDEF, "w");
                po.addFld("eventId", $$T_Str, $$UNDEF, "w");
                po.addFld("mode", $$T_Str, $$UNDEF, "w");
                po.addFld("count", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("pendedTasks", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
        // struct Semaphore.PendElem
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Semaphore$$PendElem", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Semaphore.PendElem", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("tpElem", (Proto)om.findStrict("ti.sysbios.knl.Task.PendElem", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFld("pendState", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.PendState", "ti.sysbios.knl"), $$UNDEF, "w");
        // struct Semaphore.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Semaphore$$Instance_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Semaphore.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("event", (Proto)om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("eventId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mode", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Mode", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("count", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "w");
                po.addFldV("pendQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_pendQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "w");
    }

    void Swi$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/knl/Swi.xs");
        om.bind("ti.sysbios.knl.Swi$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.knl"), $$UNDEF, "wh");
            po.addFld("LM_begin", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_begin: swi: 0x%x, func: 0x%x, preThread: %d"), "w");
            po.addFld("LD_end", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", 0x0200L, "msg", "LD_end: swi: 0x%x"), "w");
            po.addFld("LM_post", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_post: swi: 0x%x, func: 0x%x, pri: %d"), "w");
            po.addFld("A_swiDisabled", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_swiDisabled: Cannot create a Swi when Swi is disabled."), "w");
            po.addFld("A_badPriority", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_badPriority: An invalid Swi priority was used."), "w");
            po.addFld("numPriorities", Proto.Elm.newCNum("(xdc_UInt)"), 16L, "w");
            po.addFld("hooks", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Swi.HookSet", "ti.sysbios.knl"), true), Global.newArray(new Object[]{}), "w");
            po.addFld("taskDisable", new Proto.Adr("xdc_UInt(*)(xdc_Void)", "PFn"), $$UNDEF, "w");
            po.addFld("taskRestore", new Proto.Adr("xdc_Void(*)(xdc_UInt)", "PFv"), $$UNDEF, "w");
            po.addFld("numConstructedSwis", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Swi$$create", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Swi$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Swi$$construct", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Swi$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.knl.Swi$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Swi$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Swi$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Swi$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.knl.Swi$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Swi$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                po.addFxn("addHookSet", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Swi$$addHookSet", "ti.sysbios.knl"), Global.get(cap, "addHookSet"));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi.Instance", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("priority", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "w");
            po.addFld("trigger", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$Params", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("priority", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "w");
            po.addFld("trigger", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$Object", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.Object", om.findStrict("ti.sysbios.knl.Swi.Instance", "ti.sysbios.knl"));
        // typedef Swi.FuncPtr
        om.bind("ti.sysbios.knl.Swi.FuncPtr", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_UArg)", "PFv"));
        // struct Swi.HookSet
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$HookSet", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.HookSet", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("registerFxn", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"), $$UNDEF, "w");
                po.addFld("createFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Swi_Handle,xdc_runtime_Error_Block*)", "PFv"), $$UNDEF, "w");
                po.addFld("readyFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Swi_Handle)", "PFv"), $$UNDEF, "w");
                po.addFld("beginFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Swi_Handle)", "PFv"), $$UNDEF, "w");
                po.addFld("endFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Swi_Handle)", "PFv"), $$UNDEF, "w");
                po.addFld("deleteFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Swi_Handle)", "PFv"), $$UNDEF, "w");
        // struct Swi.Struct2__
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$Struct2__", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.Struct2__", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("qElem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFld("fxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_UArg)", "PFv"), $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("posted", $$T_Bool, $$UNDEF, "w");
                po.addFld("initTrigger", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("trigger", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("readyQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("hookEnv", new Proto.Arr(new Proto.Adr("xdc_Ptr", "Pv"), false), $$DEFAULT, "w");
                po.addFld("name", new Proto.Adr("xdc_runtime_Types_CordAddr__*", "PE"), $$UNDEF, "w");
        // typedef Swi.Struct2
        om.bind("ti.sysbios.knl.Swi.Struct2", (Proto)om.findStrict("ti.sysbios.knl.Swi.Struct2__", "ti.sysbios.knl"));
        // struct Swi.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$BasicView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("state", $$T_Str, $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("fxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("initTrigger", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("curTrigger", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct Swi.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$ModuleView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("schedulerState", $$T_Str, $$UNDEF, "w");
                po.addFld("readyQMask", $$T_Str, $$UNDEF, "w");
                po.addFld("currentSwi", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("currentFxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
        // struct Swi.ReadyQView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$ReadyQView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.ReadyQView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("swi", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("next", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("prev", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("readyQ", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("state", $$T_Str, $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("fxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
        // struct Swi.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$Instance_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("qElem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFld("fxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_UArg)", "PFv"), $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("posted", $$T_Bool, $$UNDEF, "w");
                po.addFld("initTrigger", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("trigger", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("readyQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("hookEnv", new Proto.Arr(new Proto.Adr("xdc_Ptr", "Pv"), false), $$DEFAULT, "w");
        // struct Swi.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$Module_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Swi.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("locked", $$T_Bool, $$UNDEF, "w");
                po.addFld("curSet", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("curTrigger", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("curSwi", (Proto)om.findStrict("ti.sysbios.knl.Swi.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("curQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("readyQ", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), false), $$DEFAULT, "w");
                po.addFld("constructedSwis", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Swi.Handle", "ti.sysbios.knl"), false), $$DEFAULT, "w");
    }

    void Task$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/knl/Task.xs");
        om.bind("ti.sysbios.knl.Task$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("AFFINITY_NONE", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
        if (isCFG) {
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.knl"), $$UNDEF, "wh");
            po.addFld("LM_switch", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_switch: oldtsk: 0x%x, oldfunc: 0x%x, newtsk: 0x%x, newfunc: 0x%x"), "w");
            po.addFld("LM_sleep", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_sleep: tsk: 0x%x, func: 0x%x, timeout: %d"), "w");
            po.addFld("LD_ready", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", 0x0200L, "msg", "LD_ready: tsk: 0x%x, func: 0x%x, pri: %d"), "w");
            po.addFld("LD_block", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", 0x0200L, "msg", "LD_block: tsk: 0x%x, func: 0x%x"), "w");
            po.addFld("LM_yield", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_yield: tsk: 0x%x, func: 0x%x, currThread: %d"), "w");
            po.addFld("LM_setPri", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_setPri: tsk: 0x%x, func: 0x%x, oldPri: %d, newPri %d"), "w");
            po.addFld("LD_exit", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", 0x0200L, "msg", "LD_exit: tsk: 0x%x, func: 0x%x"), "w");
            po.addFld("LM_setAffinity", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_setAffinity: tsk: 0x%x, func: 0x%x, oldCore: %d, oldAffinity %d, newAffinity %d"), "w");
            po.addFld("LM_schedule", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", 0x0400L, "msg", "LD_schedule: coreId: %d, workFlag: %d, curSetLocal: %d, curSetX: %d, curMaskLocal: %d"), "w");
            po.addFld("LM_noWork", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.knl"), Global.newObject("mask", 0x0400L, "msg", "LD_noWork: coreId: %d, curSetLocal: %d, curSetX: %d, curMaskLocal: %d"), "w");
            po.addFld("E_stackOverflow", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.knl"), Global.newObject("msg", "E_stackOverflow: Task 0x%x stack overflow."), "w");
            po.addFld("E_spOutOfBounds", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.knl"), Global.newObject("msg", "E_spOutOfBounds: Task 0x%x stack error, SP = 0x%x."), "w");
            po.addFld("E_deleteNotAllowed", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.knl"), Global.newObject("msg", "E_deleteNotAllowed: Task 0x%x."), "w");
            po.addFld("E_moduleStateCheckFailed", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.knl"), Global.newObject("msg", "E_moduleStateCheckFailed: Task module state data integrity check failed."), "w");
            po.addFld("E_objectCheckFailed", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.knl"), Global.newObject("msg", "E_objectCheckFailed: Task 0x%x object data integrity check failed."), "w");
            po.addFld("A_badThreadType", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_badThreadType: Cannot create/delete a task from Hwi or Swi thread."), "w");
            po.addFld("A_badTaskState", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_badTaskState: Can't delete a task in RUNNING state."), "w");
            po.addFld("A_noPendElem", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_noPendElem: Not enough info to delete BLOCKED task."), "w");
            po.addFld("A_taskDisabled", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_taskDisabled: Cannot create a task when tasking is disabled."), "w");
            po.addFld("A_badPriority", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_badPriority: An invalid task priority was used."), "w");
            po.addFld("A_badTimeout", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_badTimeout: Can't sleep FOREVER."), "w");
            po.addFld("A_badAffinity", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_badAffinity: Invalid affinity."), "w");
            po.addFld("A_sleepTaskDisabled", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_sleepTaskDisabled: Cannot call Task_sleep() while the Task scheduler is disabled."), "w");
            po.addFld("A_invalidCoreId", (Proto)om.findStrict("xdc.runtime.Assert$$Id", "ti.sysbios.knl"), Global.newObject("msg", "A_invalidCoreId: Cannot pass a non-zero CoreId in a non-SMP application."), "w");
            po.addFld("numPriorities", Proto.Elm.newCNum("(xdc_UInt)"), 16L, "w");
            po.addFld("defaultStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
            po.addFld("defaultStackSection", $$T_Str, $$UNDEF, "wh");
            po.addFld("defaultStackHeap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
            po.addFld("defaultAffinity", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "wh");
            po.addFld("enableIdleTask", $$T_Bool, true, "wh");
            po.addFld("minimizeLatency", $$T_Bool, false, "wh");
            po.addFld("idleTaskStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "wh");
            po.addFld("idleTaskStackSection", $$T_Str, $$UNDEF, "wh");
            po.addFld("idleTaskVitalTaskFlag", $$T_Bool, true, "wh");
            po.addFld("allBlockedFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), null, "w");
            po.addFld("initStackFlag", $$T_Bool, true, "w");
            po.addFld("checkStackFlag", $$T_Bool, true, "w");
            po.addFld("deleteTerminatedTasks", $$T_Bool, false, "w");
            po.addFld("hooks", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Task.HookSet", "ti.sysbios.knl"), true), Global.newArray(new Object[]{}), "w");
            po.addFld("moduleStateCheckFxn", new Proto.Adr("xdc_Int(*)(ti_sysbios_knl_Task_Module_State*,xdc_UInt32)", "PFn"), om.find("ti.sysbios.knl.Task.moduleStateCheck"), "w");
            po.addFld("moduleStateCheckValueFxn", new Proto.Adr("xdc_UInt32(*)(ti_sysbios_knl_Task_Module_State*)", "PFn"), om.find("ti.sysbios.knl.Task.getModuleStateCheckValue"), "w");
            po.addFld("moduleStateCheckFlag", $$T_Bool, false, "w");
            po.addFld("objectCheckFxn", new Proto.Adr("xdc_Int(*)(ti_sysbios_knl_Task_Handle,xdc_UInt32)", "PFn"), om.find("ti.sysbios.knl.Task.objectCheck"), "w");
            po.addFld("objectCheckValueFxn", new Proto.Adr("xdc_UInt32(*)(ti_sysbios_knl_Task_Handle)", "PFn"), om.find("ti.sysbios.knl.Task.getObjectCheckValue"), "w");
            po.addFld("objectCheckFlag", $$T_Bool, false, "w");
            po.addFldV("SupportProxy", (Proto)om.findStrict("ti.sysbios.interfaces.ITaskSupport.Module", "ti.sysbios.knl"), null, "wh", $$delegGet, $$delegSet);
            po.addFld("numConstructedTasks", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
            po.addFld("startupHookFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), null, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Task$$create", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Task$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Task$$construct", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Task$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.knl.Task$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Task$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Task$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Task$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.knl.Task$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.knl.Task$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                po.addFxn("addHookSet", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Task$$addHookSet", "ti.sysbios.knl"), Global.get(cap, "addHookSet"));
                po.addFxn("getNickName", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Task$$getNickName", "ti.sysbios.knl"), Global.get(cap, "getNickName"));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task.Instance", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("AFFINITY_NONE", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
        if (isCFG) {
            po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), 1L, "w");
            po.addFld("stack", new Proto.Adr("xdc_Ptr", "Pv"), null, "w");
            po.addFld("stackSize", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("stackSection", $$T_Str, $$UNDEF, "wh");
            po.addFld("stackHeap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.knl"), null, "w");
            po.addFld("env", new Proto.Adr("xdc_Ptr", "Pv"), null, "w");
            po.addFld("vitalTaskFlag", $$T_Bool, true, "w");
            po.addFld("affinity", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$Params", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("AFFINITY_NONE", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~(0)"), "rh");
        if (isCFG) {
            po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "w");
            po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), 1L, "w");
            po.addFld("stack", new Proto.Adr("xdc_Ptr", "Pv"), null, "w");
            po.addFld("stackSize", Proto.Elm.newCNum("(xdc_SizeT)"), 0L, "w");
            po.addFld("stackSection", $$T_Str, $$UNDEF, "wh");
            po.addFld("stackHeap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.knl"), null, "w");
            po.addFld("env", new Proto.Adr("xdc_Ptr", "Pv"), null, "w");
            po.addFld("vitalTaskFlag", $$T_Bool, true, "w");
            po.addFld("affinity", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$Object", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.Object", om.findStrict("ti.sysbios.knl.Task.Instance", "ti.sysbios.knl"));
        // typedef Task.FuncPtr
        om.bind("ti.sysbios.knl.Task.FuncPtr", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_UArg)", "PFv"));
        // typedef Task.AllBlockedFuncPtr
        om.bind("ti.sysbios.knl.Task.AllBlockedFuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"));
        // typedef Task.ModStateCheckValueFuncPtr
        om.bind("ti.sysbios.knl.Task.ModStateCheckValueFuncPtr", new Proto.Adr("xdc_UInt32(*)(ti_sysbios_knl_Task_Module_State*)", "PFn"));
        // typedef Task.ModStateCheckFuncPtr
        om.bind("ti.sysbios.knl.Task.ModStateCheckFuncPtr", new Proto.Adr("xdc_Int(*)(ti_sysbios_knl_Task_Module_State*,xdc_UInt32)", "PFn"));
        // typedef Task.ObjectCheckValueFuncPtr
        om.bind("ti.sysbios.knl.Task.ObjectCheckValueFuncPtr", new Proto.Adr("xdc_UInt32(*)(ti_sysbios_knl_Task_Handle)", "PFn"));
        // typedef Task.ObjectCheckFuncPtr
        om.bind("ti.sysbios.knl.Task.ObjectCheckFuncPtr", new Proto.Adr("xdc_Int(*)(ti_sysbios_knl_Task_Handle,xdc_UInt32)", "PFn"));
        // struct Task.Stat
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$Stat", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.Stat", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("stack", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("stackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("stackHeap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("env", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("mode", (Proto)om.findStrict("ti.sysbios.knl.Task.Mode", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("sp", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("used", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
        // struct Task.HookSet
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$HookSet", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.HookSet", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("registerFxn", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"), $$UNDEF, "w");
                po.addFld("createFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Task_Handle,xdc_runtime_Error_Block*)", "PFv"), $$UNDEF, "w");
                po.addFld("readyFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Task_Handle)", "PFv"), $$UNDEF, "w");
                po.addFld("switchFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Task_Handle,ti_sysbios_knl_Task_Handle)", "PFv"), $$UNDEF, "w");
                po.addFld("exitFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Task_Handle)", "PFv"), $$UNDEF, "w");
                po.addFld("deleteFxn", new Proto.Adr("xdc_Void(*)(ti_sysbios_knl_Task_Handle)", "PFv"), $$UNDEF, "w");
        // struct Task.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$BasicView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("mode", $$T_Str, $$UNDEF, "w");
                po.addFld("fxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("stackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("stackBase", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("curCoreId", $$T_Str, $$UNDEF, "w");
                po.addFld("affinity", $$T_Str, $$UNDEF, "w");
        // struct Task.DetailedView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$DetailedView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.DetailedView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("mode", $$T_Str, $$UNDEF, "w");
                po.addFld("fxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("stackPeak", $$T_Str, $$UNDEF, "w");
                po.addFld("stackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("stackBase", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("curCoreId", $$T_Str, $$UNDEF, "w");
                po.addFld("affinity", $$T_Str, $$UNDEF, "w");
                po.addFld("blockedOn", $$T_Str, $$UNDEF, "w");
        // struct Task.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$ModuleView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("schedulerState", $$T_Str, $$UNDEF, "w");
                po.addFld("readyQMask", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("workPending", $$T_Bool, $$UNDEF, "w");
                po.addFld("numVitalTasks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("currentTask", new Proto.Arr(new Proto.Adr("xdc_Ptr", "Pv"), false), $$DEFAULT, "w");
                po.addFld("hwiStackPeak", $$T_Str, $$UNDEF, "w");
                po.addFld("hwiStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("hwiStackBase", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        // struct Task.CallStackView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$CallStackView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.CallStackView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("depth", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("decode", $$T_Str, $$UNDEF, "w");
        // struct Task.ReadyQView
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$ReadyQView", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.ReadyQView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("task", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("next", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("prev", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("readyQ", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("mode", $$T_Str, $$UNDEF, "w");
                po.addFld("fxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("curCoreId", $$T_Str, $$UNDEF, "w");
                po.addFld("affinity", $$T_Str, $$UNDEF, "w");
        // struct Task.PendElem
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$PendElem", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.PendElem", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("qElem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFld("task", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("clock", (Proto)om.findStrict("ti.sysbios.knl.Clock.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
        // struct Task.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$Instance_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("qElem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("mask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("context", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("mode", (Proto)om.findStrict("ti.sysbios.knl.Task.Mode", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("pendElem", new Proto.Adr("ti_sysbios_knl_Task_PendElem*", "PS"), $$UNDEF, "w");
                po.addFld("stackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("stack", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("stackHeap", (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("fxn", new Proto.Adr("xdc_Void(*)(xdc_UArg,xdc_UArg)", "PFv"), $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("env", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("hookEnv", new Proto.Arr(new Proto.Adr("xdc_Ptr", "Pv"), false), $$DEFAULT, "w");
                po.addFld("vitalTaskFlag", $$T_Bool, $$UNDEF, "w");
                po.addFld("readyQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("curCoreId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("affinity", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct Task.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$Module_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("locked", $$T_Bool, $$UNDEF, "w");
                po.addFld("curSet", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("workFlag", $$T_Bool, $$UNDEF, "w");
                po.addFld("vitalTasks", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("curTask", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("curQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.sysbios.knl"), $$UNDEF, "w");
                po.addFld("readyQ", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), false), $$DEFAULT, "w");
                po.addFld("smpCurSet", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt)"), false), $$DEFAULT, "w");
                po.addFld("smpCurMask", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt)"), false), $$DEFAULT, "w");
                po.addFld("smpCurTask", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.knl"), false), $$DEFAULT, "w");
                po.addFld("smpReadyQ", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.sysbios.knl"), false), $$DEFAULT, "w");
                po.addFld("idleTask", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.knl"), false), $$DEFAULT, "w");
                po.addFld("constructedTasks", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.knl"), false), $$DEFAULT, "w");
                po.addFldV("inactiveQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_inactiveQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFldV("terminatedQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_terminatedQ", (Proto)om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"), $$DEFAULT, "w");
        // struct Task.RunQEntry
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$RunQEntry", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.RunQEntry", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("elem", (Proto)om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl"), $$DEFAULT, "w");
                po.addFld("coreId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // struct Task.Module_StateSmp
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$Module_StateSmp", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task.Module_StateSmp", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("sortedRunQ", new Proto.Adr("ti_sysbios_knl_Queue_Object*", "PO"), $$UNDEF, "w");
                po.addFld("smpRunQ", new Proto.Arr((Proto)om.findStrict("ti.sysbios.knl.Task.RunQEntry", "ti.sysbios.knl"), false), $$DEFAULT, "w");
    }

    void Clock_TimerProxy$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock_TimerProxy.Module", om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.knl"));
                po.addFld("delegate$", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.knl"), null, "wh");
                po.addFld("abstractInstances$", $$T_Bool, false, "wh");
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
        if (isCFG) {
            po.addFldV("supportsDynamic", $$T_Bool, false, "wh", $$proxyGet, $$proxySet);
            po.addFldV("defaultDynamic", $$T_Bool, false, "wh", $$proxyGet, $$proxySet);
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.knl.Clock_TimerProxy$$create", "ti.sysbios.knl"), Global.get("ti$sysbios$knl$Clock_TimerProxy$$create"));
        }//isCFG
                po.addFxn("viewGetCurrentClockTick", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimer$$viewGetCurrentClockTick", "ti.sysbios.knl"), $$UNDEF);
                po.addFxn("getFreqMeta", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimer$$getFreqMeta", "ti.sysbios.knl"), $$UNDEF);
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Instance", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock_TimerProxy.Instance", om.findStrict("ti.sysbios.interfaces.ITimer.Instance", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
        if (isCFG) {
            po.addFldV("runMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.knl"), om.find("ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS"), "w", $$proxyGet, $$proxySet);
            po.addFldV("startMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.knl"), om.find("ti.sysbios.interfaces.ITimer.StartMode_AUTO"), "w", $$proxyGet, $$proxySet);
            po.addFldV("arg", new Proto.Adr("xdc_UArg", "Pv"), null, "w", $$proxyGet, $$proxySet);
            po.addFldV("period", Proto.Elm.newCNum("(xdc_UInt32)"), 0L, "w", $$proxyGet, $$proxySet);
            po.addFldV("periodType", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.knl"), om.find("ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS"), "w", $$proxyGet, $$proxySet);
            po.addFldV("extFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios.knl"), Global.newObject("lo", 0L, "hi", 0L), "w", $$proxyGet, $$proxySet);
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock_TimerProxy$$Params", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock_TimerProxy.Params", om.findStrict("ti.sysbios.interfaces.ITimer$$Params", "ti.sysbios.knl"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
        if (isCFG) {
            po.addFld("runMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.knl"), om.find("ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS"), "w");
            po.addFld("startMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.knl"), om.find("ti.sysbios.interfaces.ITimer.StartMode_AUTO"), "w");
            po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), null, "w");
            po.addFld("period", Proto.Elm.newCNum("(xdc_UInt32)"), 0L, "w");
            po.addFld("periodType", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.knl"), om.find("ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS"), "w");
            po.addFld("extFreq", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios.knl"), Global.newObject("lo", 0L, "hi", 0L), "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.knl"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock_TimerProxy$$Object", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock_TimerProxy.Object", om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Instance", "ti.sysbios.knl"));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock_TimerProxy$$Instance_State", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Clock_TimerProxy.Instance_State", null);
        po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void Intrinsics_SupportProxy$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Intrinsics_SupportProxy.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Intrinsics_SupportProxy.Module", om.findStrict("ti.sysbios.interfaces.IIntrinsicsSupport.Module", "ti.sysbios.knl"));
                po.addFld("delegate$", (Proto)om.findStrict("ti.sysbios.interfaces.IIntrinsicsSupport.Module", "ti.sysbios.knl"), null, "wh");
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void Task_SupportProxy$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task_SupportProxy.Module", "ti.sysbios.knl");
        po.init("ti.sysbios.knl.Task_SupportProxy.Module", om.findStrict("ti.sysbios.interfaces.ITaskSupport.Module", "ti.sysbios.knl"));
                po.addFld("delegate$", (Proto)om.findStrict("ti.sysbios.interfaces.ITaskSupport.Module", "ti.sysbios.knl"), null, "wh");
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFldV("defaultStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "r", $$proxyGet, $$proxySet);
            po.addFldV("stackAlignment", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "r", $$proxyGet, $$proxySet);
        }//isCFG
                po.addFxn("stackUsed$view", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITaskSupport$$stackUsed$view", "ti.sysbios.knl"), $$UNDEF);
                po.addFxn("getCallStack$view", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITaskSupport$$getCallStack$view", "ti.sysbios.knl"), $$UNDEF);
    }

    void Clock$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Clock", "ti.sysbios.knl");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Clock.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock$$Instance_State", "ti.sysbios.knl");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Clock.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock$$Module_State", "ti.sysbios.knl");
    }

    void Idle$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Idle", "ti.sysbios.knl");
    }

    void Intrinsics$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Intrinsics", "ti.sysbios.knl");
    }

    void Event$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Event", "ti.sysbios.knl");
        vo.bind("PendElem$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Event.PendElem", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Event$$PendElem", "ti.sysbios.knl");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Event.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Event$$Instance_State", "ti.sysbios.knl");
    }

    void Mailbox$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Mailbox", "ti.sysbios.knl");
        vo.bind("MbxElem$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Mailbox.MbxElem", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox$$MbxElem", "ti.sysbios.knl");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Mailbox.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox$$Instance_State", "ti.sysbios.knl");
        po.bind("allocBuf$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
    }

    void Queue$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Queue", "ti.sysbios.knl");
        vo.bind("Elem$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Queue.Elem", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Queue$$Elem", "ti.sysbios.knl");
        po.bind("next$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Queue.Elem", "isScalar", false));
        po.bind("prev$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Queue.Elem", "isScalar", false));
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Queue.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Queue$$Instance_State", "ti.sysbios.knl");
    }

    void Semaphore$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Semaphore", "ti.sysbios.knl");
        vo.bind("PendElem$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Semaphore.PendElem", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Semaphore$$PendElem", "ti.sysbios.knl");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Semaphore.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Semaphore$$Instance_State", "ti.sysbios.knl");
    }

    void Swi$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Swi", "ti.sysbios.knl");
        vo.bind("HookSet$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Swi.HookSet", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$HookSet", "ti.sysbios.knl");
        vo.bind("Struct2__$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Swi.Struct2__", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$Struct2__", "ti.sysbios.knl");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Swi.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$Instance_State", "ti.sysbios.knl");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Swi.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi$$Module_State", "ti.sysbios.knl");
        po.bind("readyQ$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Queue.Object", "isScalar", false));
        po.bind("constructedSwis$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
    }

    void Task$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Task", "ti.sysbios.knl");
        vo.bind("Stat$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Task.Stat", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$Stat", "ti.sysbios.knl");
        vo.bind("HookSet$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Task.HookSet", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$HookSet", "ti.sysbios.knl");
        vo.bind("PendElem$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Task.PendElem", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$PendElem", "ti.sysbios.knl");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Task.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$Instance_State", "ti.sysbios.knl");
        po.bind("pendElem$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Task.PendElem", "isScalar", false));
        po.bind("stack$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Task.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$Module_State", "ti.sysbios.knl");
        po.bind("readyQ$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Queue.Object", "isScalar", false));
        po.bind("smpCurSet$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_UInt", "isScalar", true));
        po.bind("smpCurMask$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_UInt", "isScalar", true));
        po.bind("smpCurTask$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
        po.bind("smpReadyQ$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
        po.bind("idleTask$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
        po.bind("constructedTasks$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
        vo.bind("RunQEntry$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Task.RunQEntry", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$RunQEntry", "ti.sysbios.knl");
        vo.bind("Module_StateSmp$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Task.Module_StateSmp", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task$$Module_StateSmp", "ti.sysbios.knl");
        po.bind("sortedRunQ$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Queue.Object", "isScalar", false));
        po.bind("smpRunQ$fetchDesc", Global.newObject("type", "ti.sysbios.knl.Task.RunQEntry", "isScalar", false));
    }

    void Clock_TimerProxy$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Clock_TimerProxy", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock_TimerProxy$$Instance_State", "ti.sysbios.knl");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
    }

    void Intrinsics_SupportProxy$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Intrinsics_SupportProxy", "ti.sysbios.knl");
    }

    void Task_SupportProxy$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Task_SupportProxy", "ti.sysbios.knl");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.knl.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.knl"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/knl/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.knl"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.knl"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.knl"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.knl"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.knl"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.knl"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.knl", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.knl");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.knl.");
        pkgV.bind("$vers", Global.newArray(2, 0, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        imports.add(Global.newArray("ti.sysbios.family", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.knl'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.sysbios.knl$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.sysbios.knl$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.sysbios.knl$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/ti.sysbios.knl.aem4',\n");
            sb.append("'lib/debug/ti.sysbios.knl.am4',\n");
            sb.append("'lib/debug/ti.sysbios.knl.am4g',\n");
            sb.append("'lib/debug/ti.sysbios.knl.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/ti.sysbios.knl.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/debug/ti.sysbios.knl.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/debug/ti.sysbios.knl.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/debug/ti.sysbios.knl.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Clock$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Clock", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Clock", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.knl.Clock$$capsule", "ti.sysbios.knl"));
        vo.bind("Instance", om.findStrict("ti.sysbios.knl.Clock.Instance", "ti.sysbios.knl"));
        vo.bind("Params", om.findStrict("ti.sysbios.knl.Clock.Params", "ti.sysbios.knl"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.knl.Clock.Params", "ti.sysbios.knl")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.knl.Clock.Handle", "ti.sysbios.knl"));
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
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
        vo.bind("TickSource", om.findStrict("ti.sysbios.knl.Clock.TickSource", "ti.sysbios.knl"));
        vo.bind("TickMode", om.findStrict("ti.sysbios.knl.Clock.TickMode", "ti.sysbios.knl"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.knl.Clock.BasicView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Clock.BasicView", "ti.sysbios.knl"));
        vo.bind("ModuleView", om.findStrict("ti.sysbios.knl.Clock.ModuleView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Clock.ModuleView", "ti.sysbios.knl"));
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.knl.Clock.FuncPtr", "ti.sysbios.knl"));
        vo.bind("TimerProxy$proxy", om.findStrict("ti.sysbios.knl.Clock_TimerProxy", "ti.sysbios.knl"));
        proxies.add("TimerProxy");
        mcfgs.add("LW_delayed");
        mcfgs.add("LM_tick");
        mcfgs.add("LM_begin");
        mcfgs.add("A_clockDisabled");
        mcfgs.add("A_badThreadType");
        mcfgs.add("serviceMargin");
        mcfgs.add("tickSource");
        mcfgs.add("tickMode");
        mcfgs.add("timerId");
        mcfgs.add("tickPeriod");
        icfgs.add("timerSupportsDynamic");
        mcfgs.add("doTickFunc");
        icfgs.add("doTickFunc");
        mcfgs.add("triggerClock");
        icfgs.add("triggerClock");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.knl.Clock.Instance_State", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Clock.Instance_State", "ti.sysbios.knl"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.knl.Clock.Module_State", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Clock.Module_State", "ti.sysbios.knl"));
        vo.bind("TickSource_TIMER", om.findStrict("ti.sysbios.knl.Clock.TickSource_TIMER", "ti.sysbios.knl"));
        vo.bind("TickSource_USER", om.findStrict("ti.sysbios.knl.Clock.TickSource_USER", "ti.sysbios.knl"));
        vo.bind("TickSource_NULL", om.findStrict("ti.sysbios.knl.Clock.TickSource_NULL", "ti.sysbios.knl"));
        vo.bind("TickMode_PERIODIC", om.findStrict("ti.sysbios.knl.Clock.TickMode_PERIODIC", "ti.sysbios.knl"));
        vo.bind("TickMode_DYNAMIC", om.findStrict("ti.sysbios.knl.Clock.TickMode_DYNAMIC", "ti.sysbios.knl"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.sysbios.knl.Clock$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$knl$Clock$$__initObject"));
        }//isCFG
        vo.bind("getTicks", om.findStrict("ti.sysbios.knl.Clock.getTicks", "ti.sysbios.knl"));
        vo.bind("getTimerHandle", om.findStrict("ti.sysbios.knl.Clock.getTimerHandle", "ti.sysbios.knl"));
        vo.bind("setTicks", om.findStrict("ti.sysbios.knl.Clock.setTicks", "ti.sysbios.knl"));
        vo.bind("tickStop", om.findStrict("ti.sysbios.knl.Clock.tickStop", "ti.sysbios.knl"));
        vo.bind("tickReconfig", om.findStrict("ti.sysbios.knl.Clock.tickReconfig", "ti.sysbios.knl"));
        vo.bind("tickStart", om.findStrict("ti.sysbios.knl.Clock.tickStart", "ti.sysbios.knl"));
        vo.bind("tick", om.findStrict("ti.sysbios.knl.Clock.tick", "ti.sysbios.knl"));
        vo.bind("workFunc", om.findStrict("ti.sysbios.knl.Clock.workFunc", "ti.sysbios.knl"));
        vo.bind("workFuncDynamic", om.findStrict("ti.sysbios.knl.Clock.workFuncDynamic", "ti.sysbios.knl"));
        vo.bind("logTick", om.findStrict("ti.sysbios.knl.Clock.logTick", "ti.sysbios.knl"));
        vo.bind("getCompletedTicks", om.findStrict("ti.sysbios.knl.Clock.getCompletedTicks", "ti.sysbios.knl"));
        vo.bind("getTickPeriod", om.findStrict("ti.sysbios.knl.Clock.getTickPeriod", "ti.sysbios.knl"));
        vo.bind("getTicksUntilInterrupt", om.findStrict("ti.sysbios.knl.Clock.getTicksUntilInterrupt", "ti.sysbios.knl"));
        vo.bind("getTicksUntilTimeout", om.findStrict("ti.sysbios.knl.Clock.getTicksUntilTimeout", "ti.sysbios.knl"));
        vo.bind("walkQueueDynamic", om.findStrict("ti.sysbios.knl.Clock.walkQueueDynamic", "ti.sysbios.knl"));
        vo.bind("walkQueuePeriodic", om.findStrict("ti.sysbios.knl.Clock.walkQueuePeriodic", "ti.sysbios.knl"));
        vo.bind("scheduleNextTick", om.findStrict("ti.sysbios.knl.Clock.scheduleNextTick", "ti.sysbios.knl"));
        vo.bind("doTick", om.findStrict("ti.sysbios.knl.Clock.doTick", "ti.sysbios.knl"));
        vo.bind("triggerFunc", om.findStrict("ti.sysbios.knl.Clock.triggerFunc", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Clock_Handle__label__E", "ti_sysbios_knl_Clock_Module__startupDone__E", "ti_sysbios_knl_Clock_Object__create__E", "ti_sysbios_knl_Clock_Object__delete__E", "ti_sysbios_knl_Clock_Object__get__E", "ti_sysbios_knl_Clock_Object__first__E", "ti_sysbios_knl_Clock_Object__next__E", "ti_sysbios_knl_Clock_Params__init__E", "ti_sysbios_knl_Clock_getTicks__E", "ti_sysbios_knl_Clock_getTimerHandle__E", "ti_sysbios_knl_Clock_setTicks__E", "ti_sysbios_knl_Clock_tickStop__E", "ti_sysbios_knl_Clock_tickReconfig__E", "ti_sysbios_knl_Clock_tickStart__E", "ti_sysbios_knl_Clock_tick__E", "ti_sysbios_knl_Clock_workFunc__E", "ti_sysbios_knl_Clock_workFuncDynamic__E", "ti_sysbios_knl_Clock_logTick__E", "ti_sysbios_knl_Clock_getCompletedTicks__E", "ti_sysbios_knl_Clock_getTickPeriod__E", "ti_sysbios_knl_Clock_getTicksUntilInterrupt__E", "ti_sysbios_knl_Clock_getTicksUntilTimeout__E", "ti_sysbios_knl_Clock_walkQueueDynamic__E", "ti_sysbios_knl_Clock_walkQueuePeriodic__E", "ti_sysbios_knl_Clock_scheduleNextTick__E", "ti_sysbios_knl_Clock_addI__E", "ti_sysbios_knl_Clock_removeI__E", "ti_sysbios_knl_Clock_start__E", "ti_sysbios_knl_Clock_startI__E", "ti_sysbios_knl_Clock_stop__E", "ti_sysbios_knl_Clock_setPeriod__E", "ti_sysbios_knl_Clock_setTimeout__E", "ti_sysbios_knl_Clock_setFunc__E", "ti_sysbios_knl_Clock_getPeriod__E", "ti_sysbios_knl_Clock_getTimeout__E", "ti_sysbios_knl_Clock_isActive__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("LW_delayed", "LM_tick", "LM_begin"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_clockDisabled", "A_badThreadType"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", "./Clock.xdt");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.knl.Clock.Object", "ti.sysbios.knl"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./Clock.xdt");
        pkgV.bind("Clock", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Clock");
    }

    void Idle$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Idle", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Idle.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Idle", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.knl.Idle$$capsule", "ti.sysbios.knl"));
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
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
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.knl.Idle.FuncPtr", "ti.sysbios.knl"));
        vo.bind("ModuleView", om.findStrict("ti.sysbios.knl.Idle.ModuleView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Idle.ModuleView", "ti.sysbios.knl"));
        mcfgs.add("funcList");
        mcfgs.add("coreList");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
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
        vo.bind("loop", om.findStrict("ti.sysbios.knl.Idle.loop", "ti.sysbios.knl"));
        vo.bind("run", om.findStrict("ti.sysbios.knl.Idle.run", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Idle_Module__startupDone__E", "ti_sysbios_knl_Idle_loop__E", "ti_sysbios_knl_Idle_run__E"));
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
        pkgV.bind("Idle", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Idle");
    }

    void Intrinsics$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Intrinsics", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Intrinsics.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Intrinsics", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.knl.Intrinsics$$capsule", "ti.sysbios.knl"));
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
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
        vo.bind("SupportProxy$proxy", om.findStrict("ti.sysbios.knl.Intrinsics_SupportProxy", "ti.sysbios.knl"));
        proxies.add("SupportProxy");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
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
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Intrinsics_Module__startupDone__E", "ti_sysbios_knl_Intrinsics_maxbit__E"));
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
        pkgV.bind("Intrinsics", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Intrinsics");
    }

    void Event$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Event", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Event.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Event", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.knl.Event$$capsule", "ti.sysbios.knl"));
        vo.bind("Instance", om.findStrict("ti.sysbios.knl.Event.Instance", "ti.sysbios.knl"));
        vo.bind("Params", om.findStrict("ti.sysbios.knl.Event.Params", "ti.sysbios.knl"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.knl.Event.Params", "ti.sysbios.knl")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.knl.Event.Handle", "ti.sysbios.knl"));
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.knl.Event.BasicView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Event.BasicView", "ti.sysbios.knl"));
        mcfgs.add("LM_post");
        mcfgs.add("LM_pend");
        mcfgs.add("A_nullEventMasks");
        mcfgs.add("A_nullEventId");
        mcfgs.add("A_eventInUse");
        mcfgs.add("A_badContext");
        mcfgs.add("A_pendTaskDisabled");
        icfgs.add("eventInstances");
        vo.bind("PendState", om.findStrict("ti.sysbios.knl.Event.PendState", "ti.sysbios.knl"));
        vo.bind("PendElem", om.findStrict("ti.sysbios.knl.Event.PendElem", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Event.PendElem", "ti.sysbios.knl"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.knl.Event.Instance_State", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Event.Instance_State", "ti.sysbios.knl"));
        vo.bind("PendState_TIMEOUT", om.findStrict("ti.sysbios.knl.Event.PendState_TIMEOUT", "ti.sysbios.knl"));
        vo.bind("PendState_POSTED", om.findStrict("ti.sysbios.knl.Event.PendState_POSTED", "ti.sysbios.knl"));
        vo.bind("PendState_CLOCK_WAIT", om.findStrict("ti.sysbios.knl.Event.PendState_CLOCK_WAIT", "ti.sysbios.knl"));
        vo.bind("PendState_WAIT_FOREVER", om.findStrict("ti.sysbios.knl.Event.PendState_WAIT_FOREVER", "ti.sysbios.knl"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.sysbios.knl.Event$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$knl$Event$$__initObject"));
        }//isCFG
        vo.bind("pendTimeout", om.findStrict("ti.sysbios.knl.Event.pendTimeout", "ti.sysbios.knl"));
        vo.bind("checkEvents", om.findStrict("ti.sysbios.knl.Event.checkEvents", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Event_Handle__label__E", "ti_sysbios_knl_Event_Module__startupDone__E", "ti_sysbios_knl_Event_Object__create__E", "ti_sysbios_knl_Event_Object__delete__E", "ti_sysbios_knl_Event_Object__get__E", "ti_sysbios_knl_Event_Object__first__E", "ti_sysbios_knl_Event_Object__next__E", "ti_sysbios_knl_Event_Params__init__E", "ti_sysbios_knl_Event_pend__E", "ti_sysbios_knl_Event_post__E", "ti_sysbios_knl_Event_getPostedEvents__E", "ti_sysbios_knl_Event_sync__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("LM_post", "LM_pend"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_nullEventMasks", "A_nullEventId", "A_eventInUse", "A_badContext", "A_pendTaskDisabled"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.knl.Event.Object", "ti.sysbios.knl"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Event", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Event");
    }

    void Mailbox$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Mailbox", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Mailbox.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Mailbox", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.knl.Mailbox$$capsule", "ti.sysbios.knl"));
        vo.bind("Instance", om.findStrict("ti.sysbios.knl.Mailbox.Instance", "ti.sysbios.knl"));
        vo.bind("Params", om.findStrict("ti.sysbios.knl.Mailbox.Params", "ti.sysbios.knl"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.knl.Mailbox.Params", "ti.sysbios.knl")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.knl.Mailbox.Handle", "ti.sysbios.knl"));
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.knl.Mailbox.BasicView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Mailbox.BasicView", "ti.sysbios.knl"));
        vo.bind("DetailedView", om.findStrict("ti.sysbios.knl.Mailbox.DetailedView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Mailbox.DetailedView", "ti.sysbios.knl"));
        vo.bind("MbxElem", om.findStrict("ti.sysbios.knl.Mailbox.MbxElem", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Mailbox.MbxElem", "ti.sysbios.knl"));
        mcfgs.add("A_invalidBufSize");
        mcfgs.add("maxTypeAlign");
        icfgs.add("maxTypeAlign");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.knl.Mailbox.Instance_State", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Mailbox.Instance_State", "ti.sysbios.knl"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.sysbios.knl.Mailbox$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$knl$Mailbox$$__initObject"));
        }//isCFG
        vo.bind("cleanQue", om.findStrict("ti.sysbios.knl.Mailbox.cleanQue", "ti.sysbios.knl"));
        vo.bind("postInit", om.findStrict("ti.sysbios.knl.Mailbox.postInit", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Mailbox_Handle__label__E", "ti_sysbios_knl_Mailbox_Module__startupDone__E", "ti_sysbios_knl_Mailbox_Object__create__E", "ti_sysbios_knl_Mailbox_Object__delete__E", "ti_sysbios_knl_Mailbox_Object__get__E", "ti_sysbios_knl_Mailbox_Object__first__E", "ti_sysbios_knl_Mailbox_Object__next__E", "ti_sysbios_knl_Mailbox_Params__init__E", "ti_sysbios_knl_Mailbox_getMsgSize__E", "ti_sysbios_knl_Mailbox_getNumFreeMsgs__E", "ti_sysbios_knl_Mailbox_getNumPendingMsgs__E", "ti_sysbios_knl_Mailbox_pend__E", "ti_sysbios_knl_Mailbox_post__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_invalidBufSize"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.knl.Mailbox.Object", "ti.sysbios.knl"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Mailbox", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Mailbox");
    }

    void Queue$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Queue", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Queue.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Queue", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.knl.Queue$$capsule", "ti.sysbios.knl"));
        vo.bind("Instance", om.findStrict("ti.sysbios.knl.Queue.Instance", "ti.sysbios.knl"));
        vo.bind("Params", om.findStrict("ti.sysbios.knl.Queue.Params", "ti.sysbios.knl"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.knl.Queue.Params", "ti.sysbios.knl")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.knl.Queue.Handle", "ti.sysbios.knl"));
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
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
        vo.bind("BasicView", om.findStrict("ti.sysbios.knl.Queue.BasicView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Queue.BasicView", "ti.sysbios.knl"));
        vo.bind("Elem", om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Queue.Elem", "ti.sysbios.knl"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.knl.Queue.Instance_State", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Queue.Instance_State", "ti.sysbios.knl"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.sysbios.knl.Queue$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$knl$Queue$$__initObject"));
        }//isCFG
        vo.bind("elemClear", om.findStrict("ti.sysbios.knl.Queue.elemClear", "ti.sysbios.knl"));
        vo.bind("insert", om.findStrict("ti.sysbios.knl.Queue.insert", "ti.sysbios.knl"));
        vo.bind("next", om.findStrict("ti.sysbios.knl.Queue.next", "ti.sysbios.knl"));
        vo.bind("prev", om.findStrict("ti.sysbios.knl.Queue.prev", "ti.sysbios.knl"));
        vo.bind("remove", om.findStrict("ti.sysbios.knl.Queue.remove", "ti.sysbios.knl"));
        vo.bind("isQueued", om.findStrict("ti.sysbios.knl.Queue.isQueued", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Queue_Handle__label__E", "ti_sysbios_knl_Queue_Module__startupDone__E", "ti_sysbios_knl_Queue_Object__create__E", "ti_sysbios_knl_Queue_Object__delete__E", "ti_sysbios_knl_Queue_Object__get__E", "ti_sysbios_knl_Queue_Object__first__E", "ti_sysbios_knl_Queue_Object__next__E", "ti_sysbios_knl_Queue_Params__init__E", "ti_sysbios_knl_Queue_elemClear__E", "ti_sysbios_knl_Queue_insert__E", "ti_sysbios_knl_Queue_next__E", "ti_sysbios_knl_Queue_prev__E", "ti_sysbios_knl_Queue_remove__E", "ti_sysbios_knl_Queue_isQueued__E", "ti_sysbios_knl_Queue_dequeue__E", "ti_sysbios_knl_Queue_empty__E", "ti_sysbios_knl_Queue_enqueue__E", "ti_sysbios_knl_Queue_get__E", "ti_sysbios_knl_Queue_getTail__E", "ti_sysbios_knl_Queue_head__E", "ti_sysbios_knl_Queue_put__E", "ti_sysbios_knl_Queue_putHead__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.knl.Queue.Object", "ti.sysbios.knl"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Queue", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Queue");
    }

    void Semaphore$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Semaphore", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Semaphore.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Semaphore", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.knl.Semaphore$$capsule", "ti.sysbios.knl"));
        vo.bind("Instance", om.findStrict("ti.sysbios.knl.Semaphore.Instance", "ti.sysbios.knl"));
        vo.bind("Params", om.findStrict("ti.sysbios.knl.Semaphore.Params", "ti.sysbios.knl"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.knl.Semaphore.Params", "ti.sysbios.knl")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.knl.Semaphore.Handle", "ti.sysbios.knl"));
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
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
        vo.bind("Mode", om.findStrict("ti.sysbios.knl.Semaphore.Mode", "ti.sysbios.knl"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.knl.Semaphore.BasicView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Semaphore.BasicView", "ti.sysbios.knl"));
        mcfgs.add("LM_post");
        mcfgs.add("LM_pend");
        mcfgs.add("A_noEvents");
        mcfgs.add("A_invTimeout");
        mcfgs.add("A_badContext");
        mcfgs.add("A_overflow");
        mcfgs.add("A_pendTaskDisabled");
        mcfgs.add("supportsEvents");
        mcfgs.add("supportsPriority");
        mcfgs.add("eventPost");
        icfgs.add("eventPost");
        mcfgs.add("eventSync");
        icfgs.add("eventSync");
        vo.bind("PendState", om.findStrict("ti.sysbios.knl.Semaphore.PendState", "ti.sysbios.knl"));
        vo.bind("PendElem", om.findStrict("ti.sysbios.knl.Semaphore.PendElem", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Semaphore.PendElem", "ti.sysbios.knl"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.knl.Semaphore.Instance_State", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Semaphore.Instance_State", "ti.sysbios.knl"));
        vo.bind("Mode_COUNTING", om.findStrict("ti.sysbios.knl.Semaphore.Mode_COUNTING", "ti.sysbios.knl"));
        vo.bind("Mode_BINARY", om.findStrict("ti.sysbios.knl.Semaphore.Mode_BINARY", "ti.sysbios.knl"));
        vo.bind("Mode_COUNTING_PRIORITY", om.findStrict("ti.sysbios.knl.Semaphore.Mode_COUNTING_PRIORITY", "ti.sysbios.knl"));
        vo.bind("Mode_BINARY_PRIORITY", om.findStrict("ti.sysbios.knl.Semaphore.Mode_BINARY_PRIORITY", "ti.sysbios.knl"));
        vo.bind("PendState_TIMEOUT", om.findStrict("ti.sysbios.knl.Semaphore.PendState_TIMEOUT", "ti.sysbios.knl"));
        vo.bind("PendState_POSTED", om.findStrict("ti.sysbios.knl.Semaphore.PendState_POSTED", "ti.sysbios.knl"));
        vo.bind("PendState_CLOCK_WAIT", om.findStrict("ti.sysbios.knl.Semaphore.PendState_CLOCK_WAIT", "ti.sysbios.knl"));
        vo.bind("PendState_WAIT_FOREVER", om.findStrict("ti.sysbios.knl.Semaphore.PendState_WAIT_FOREVER", "ti.sysbios.knl"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.sysbios.knl.Semaphore$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$knl$Semaphore$$__initObject"));
        }//isCFG
        vo.bind("pendTimeout", om.findStrict("ti.sysbios.knl.Semaphore.pendTimeout", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Semaphore_Handle__label__E", "ti_sysbios_knl_Semaphore_Module__startupDone__E", "ti_sysbios_knl_Semaphore_Object__create__E", "ti_sysbios_knl_Semaphore_Object__delete__E", "ti_sysbios_knl_Semaphore_Object__get__E", "ti_sysbios_knl_Semaphore_Object__first__E", "ti_sysbios_knl_Semaphore_Object__next__E", "ti_sysbios_knl_Semaphore_Params__init__E", "ti_sysbios_knl_Semaphore_getCount__E", "ti_sysbios_knl_Semaphore_pend__E", "ti_sysbios_knl_Semaphore_post__E", "ti_sysbios_knl_Semaphore_registerEvent__E", "ti_sysbios_knl_Semaphore_reset__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("LM_post", "LM_pend"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_noEvents", "A_invTimeout", "A_badContext", "A_overflow", "A_pendTaskDisabled"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.sysbios.knl"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Semaphore", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Semaphore");
    }

    void Swi$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Swi", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Swi.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Swi", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.knl.Swi$$capsule", "ti.sysbios.knl"));
        vo.bind("Instance", om.findStrict("ti.sysbios.knl.Swi.Instance", "ti.sysbios.knl"));
        vo.bind("Params", om.findStrict("ti.sysbios.knl.Swi.Params", "ti.sysbios.knl"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.knl.Swi.Params", "ti.sysbios.knl")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.knl.Swi.Handle", "ti.sysbios.knl"));
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
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
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.knl.Swi.FuncPtr", "ti.sysbios.knl"));
        vo.bind("HookSet", om.findStrict("ti.sysbios.knl.Swi.HookSet", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Swi.HookSet", "ti.sysbios.knl"));
        vo.bind("Struct2__", om.findStrict("ti.sysbios.knl.Swi.Struct2__", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Swi.Struct2__", "ti.sysbios.knl"));
        vo.bind("Struct2", om.findStrict("ti.sysbios.knl.Swi.Struct2", "ti.sysbios.knl"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.knl.Swi.BasicView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Swi.BasicView", "ti.sysbios.knl"));
        vo.bind("ModuleView", om.findStrict("ti.sysbios.knl.Swi.ModuleView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Swi.ModuleView", "ti.sysbios.knl"));
        vo.bind("ReadyQView", om.findStrict("ti.sysbios.knl.Swi.ReadyQView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Swi.ReadyQView", "ti.sysbios.knl"));
        mcfgs.add("LM_begin");
        mcfgs.add("LD_end");
        mcfgs.add("LM_post");
        mcfgs.add("A_swiDisabled");
        mcfgs.add("A_badPriority");
        mcfgs.add("numPriorities");
        mcfgs.add("hooks");
        mcfgs.add("taskDisable");
        icfgs.add("taskDisable");
        mcfgs.add("taskRestore");
        icfgs.add("taskRestore");
        mcfgs.add("numConstructedSwis");
        icfgs.add("numConstructedSwis");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.knl.Swi.Instance_State", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Swi.Instance_State", "ti.sysbios.knl"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.knl.Swi.Module_State", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Swi.Module_State", "ti.sysbios.knl"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.sysbios.knl.Swi$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$knl$Swi$$__initObject"));
        }//isCFG
        vo.bind("construct2", om.findStrict("ti.sysbios.knl.Swi.construct2", "ti.sysbios.knl"));
        vo.bind("startup", om.findStrict("ti.sysbios.knl.Swi.startup", "ti.sysbios.knl"));
        vo.bind("enabled", om.findStrict("ti.sysbios.knl.Swi.enabled", "ti.sysbios.knl"));
        vo.bind("unlockSched", om.findStrict("ti.sysbios.knl.Swi.unlockSched", "ti.sysbios.knl"));
        vo.bind("disable", om.findStrict("ti.sysbios.knl.Swi.disable", "ti.sysbios.knl"));
        vo.bind("enable", om.findStrict("ti.sysbios.knl.Swi.enable", "ti.sysbios.knl"));
        vo.bind("restore", om.findStrict("ti.sysbios.knl.Swi.restore", "ti.sysbios.knl"));
        vo.bind("restoreHwi", om.findStrict("ti.sysbios.knl.Swi.restoreHwi", "ti.sysbios.knl"));
        vo.bind("self", om.findStrict("ti.sysbios.knl.Swi.self", "ti.sysbios.knl"));
        vo.bind("getTrigger", om.findStrict("ti.sysbios.knl.Swi.getTrigger", "ti.sysbios.knl"));
        vo.bind("raisePri", om.findStrict("ti.sysbios.knl.Swi.raisePri", "ti.sysbios.knl"));
        vo.bind("restorePri", om.findStrict("ti.sysbios.knl.Swi.restorePri", "ti.sysbios.knl"));
        vo.bind("schedule", om.findStrict("ti.sysbios.knl.Swi.schedule", "ti.sysbios.knl"));
        vo.bind("runLoop", om.findStrict("ti.sysbios.knl.Swi.runLoop", "ti.sysbios.knl"));
        vo.bind("run", om.findStrict("ti.sysbios.knl.Swi.run", "ti.sysbios.knl"));
        vo.bind("postInit", om.findStrict("ti.sysbios.knl.Swi.postInit", "ti.sysbios.knl"));
        vo.bind("restoreSMP", om.findStrict("ti.sysbios.knl.Swi.restoreSMP", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Swi_Handle__label__E", "ti_sysbios_knl_Swi_Module__startupDone__E", "ti_sysbios_knl_Swi_Object__create__E", "ti_sysbios_knl_Swi_Object__delete__E", "ti_sysbios_knl_Swi_Object__get__E", "ti_sysbios_knl_Swi_Object__first__E", "ti_sysbios_knl_Swi_Object__next__E", "ti_sysbios_knl_Swi_Params__init__E", "ti_sysbios_knl_Swi_construct2__E", "ti_sysbios_knl_Swi_startup__E", "ti_sysbios_knl_Swi_enabled__E", "ti_sysbios_knl_Swi_unlockSched__E", "ti_sysbios_knl_Swi_disable__E", "ti_sysbios_knl_Swi_enable__E", "ti_sysbios_knl_Swi_restore__E", "ti_sysbios_knl_Swi_restoreHwi__E", "ti_sysbios_knl_Swi_self__E", "ti_sysbios_knl_Swi_getTrigger__E", "ti_sysbios_knl_Swi_raisePri__E", "ti_sysbios_knl_Swi_restorePri__E", "ti_sysbios_knl_Swi_andn__E", "ti_sysbios_knl_Swi_dec__E", "ti_sysbios_knl_Swi_getHookContext__E", "ti_sysbios_knl_Swi_setHookContext__E", "ti_sysbios_knl_Swi_getPri__E", "ti_sysbios_knl_Swi_getFunc__E", "ti_sysbios_knl_Swi_getAttrs__E", "ti_sysbios_knl_Swi_setAttrs__E", "ti_sysbios_knl_Swi_setPri__E", "ti_sysbios_knl_Swi_inc__E", "ti_sysbios_knl_Swi_or__E", "ti_sysbios_knl_Swi_post__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("LM_begin", "LD_end", "LM_post"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray("A_swiDisabled", "A_badPriority"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.knl.Swi.Object", "ti.sysbios.knl"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Swi", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Swi");
    }

    void Task$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Task", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Task", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.knl.Task$$capsule", "ti.sysbios.knl"));
        vo.bind("Instance", om.findStrict("ti.sysbios.knl.Task.Instance", "ti.sysbios.knl"));
        vo.bind("Params", om.findStrict("ti.sysbios.knl.Task.Params", "ti.sysbios.knl"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.knl.Task.Params", "ti.sysbios.knl")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.knl.Task.Handle", "ti.sysbios.knl"));
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
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
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.knl.Task.FuncPtr", "ti.sysbios.knl"));
        vo.bind("AllBlockedFuncPtr", om.findStrict("ti.sysbios.knl.Task.AllBlockedFuncPtr", "ti.sysbios.knl"));
        vo.bind("ModStateCheckValueFuncPtr", om.findStrict("ti.sysbios.knl.Task.ModStateCheckValueFuncPtr", "ti.sysbios.knl"));
        vo.bind("ModStateCheckFuncPtr", om.findStrict("ti.sysbios.knl.Task.ModStateCheckFuncPtr", "ti.sysbios.knl"));
        vo.bind("ObjectCheckValueFuncPtr", om.findStrict("ti.sysbios.knl.Task.ObjectCheckValueFuncPtr", "ti.sysbios.knl"));
        vo.bind("ObjectCheckFuncPtr", om.findStrict("ti.sysbios.knl.Task.ObjectCheckFuncPtr", "ti.sysbios.knl"));
        vo.bind("Mode", om.findStrict("ti.sysbios.knl.Task.Mode", "ti.sysbios.knl"));
        vo.bind("Stat", om.findStrict("ti.sysbios.knl.Task.Stat", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.Stat", "ti.sysbios.knl"));
        vo.bind("HookSet", om.findStrict("ti.sysbios.knl.Task.HookSet", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.HookSet", "ti.sysbios.knl"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.knl.Task.BasicView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.BasicView", "ti.sysbios.knl"));
        vo.bind("DetailedView", om.findStrict("ti.sysbios.knl.Task.DetailedView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.DetailedView", "ti.sysbios.knl"));
        vo.bind("ModuleView", om.findStrict("ti.sysbios.knl.Task.ModuleView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.ModuleView", "ti.sysbios.knl"));
        vo.bind("CallStackView", om.findStrict("ti.sysbios.knl.Task.CallStackView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.CallStackView", "ti.sysbios.knl"));
        vo.bind("ReadyQView", om.findStrict("ti.sysbios.knl.Task.ReadyQView", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.ReadyQView", "ti.sysbios.knl"));
        mcfgs.add("LM_switch");
        mcfgs.add("LM_sleep");
        mcfgs.add("LD_ready");
        mcfgs.add("LD_block");
        mcfgs.add("LM_yield");
        mcfgs.add("LM_setPri");
        mcfgs.add("LD_exit");
        mcfgs.add("LM_setAffinity");
        mcfgs.add("LM_schedule");
        mcfgs.add("LM_noWork");
        mcfgs.add("E_stackOverflow");
        mcfgs.add("E_spOutOfBounds");
        mcfgs.add("E_deleteNotAllowed");
        mcfgs.add("E_moduleStateCheckFailed");
        mcfgs.add("E_objectCheckFailed");
        mcfgs.add("A_badThreadType");
        mcfgs.add("A_badTaskState");
        mcfgs.add("A_noPendElem");
        mcfgs.add("A_taskDisabled");
        mcfgs.add("A_badPriority");
        mcfgs.add("A_badTimeout");
        mcfgs.add("A_badAffinity");
        mcfgs.add("A_sleepTaskDisabled");
        mcfgs.add("A_invalidCoreId");
        mcfgs.add("numPriorities");
        mcfgs.add("defaultStackSize");
        mcfgs.add("defaultStackHeap");
        mcfgs.add("allBlockedFunc");
        mcfgs.add("initStackFlag");
        mcfgs.add("checkStackFlag");
        mcfgs.add("deleteTerminatedTasks");
        mcfgs.add("hooks");
        mcfgs.add("moduleStateCheckFxn");
        mcfgs.add("moduleStateCheckValueFxn");
        mcfgs.add("moduleStateCheckFlag");
        mcfgs.add("objectCheckFxn");
        mcfgs.add("objectCheckValueFxn");
        mcfgs.add("objectCheckFlag");
        vo.bind("SupportProxy$proxy", om.findStrict("ti.sysbios.knl.Task_SupportProxy", "ti.sysbios.knl"));
        proxies.add("SupportProxy");
        mcfgs.add("numConstructedTasks");
        icfgs.add("numConstructedTasks");
        mcfgs.add("startupHookFunc");
        icfgs.add("startupHookFunc");
        vo.bind("PendElem", om.findStrict("ti.sysbios.knl.Task.PendElem", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.PendElem", "ti.sysbios.knl"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.knl.Task.Instance_State", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.Instance_State", "ti.sysbios.knl"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.knl.Task.Module_State", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.Module_State", "ti.sysbios.knl"));
        vo.bind("RunQEntry", om.findStrict("ti.sysbios.knl.Task.RunQEntry", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.RunQEntry", "ti.sysbios.knl"));
        vo.bind("Module_StateSmp", om.findStrict("ti.sysbios.knl.Task.Module_StateSmp", "ti.sysbios.knl"));
        tdefs.add(om.findStrict("ti.sysbios.knl.Task.Module_StateSmp", "ti.sysbios.knl"));
        vo.bind("Mode_RUNNING", om.findStrict("ti.sysbios.knl.Task.Mode_RUNNING", "ti.sysbios.knl"));
        vo.bind("Mode_READY", om.findStrict("ti.sysbios.knl.Task.Mode_READY", "ti.sysbios.knl"));
        vo.bind("Mode_BLOCKED", om.findStrict("ti.sysbios.knl.Task.Mode_BLOCKED", "ti.sysbios.knl"));
        vo.bind("Mode_TERMINATED", om.findStrict("ti.sysbios.knl.Task.Mode_TERMINATED", "ti.sysbios.knl"));
        vo.bind("Mode_INACTIVE", om.findStrict("ti.sysbios.knl.Task.Mode_INACTIVE", "ti.sysbios.knl"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.sysbios.knl.Task$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$knl$Task$$__initObject"));
        }//isCFG
        vo.bind("startup", om.findStrict("ti.sysbios.knl.Task.startup", "ti.sysbios.knl"));
        vo.bind("enabled", om.findStrict("ti.sysbios.knl.Task.enabled", "ti.sysbios.knl"));
        vo.bind("unlockSched", om.findStrict("ti.sysbios.knl.Task.unlockSched", "ti.sysbios.knl"));
        vo.bind("disable", om.findStrict("ti.sysbios.knl.Task.disable", "ti.sysbios.knl"));
        vo.bind("enable", om.findStrict("ti.sysbios.knl.Task.enable", "ti.sysbios.knl"));
        vo.bind("restore", om.findStrict("ti.sysbios.knl.Task.restore", "ti.sysbios.knl"));
        vo.bind("restoreHwi", om.findStrict("ti.sysbios.knl.Task.restoreHwi", "ti.sysbios.knl"));
        vo.bind("self", om.findStrict("ti.sysbios.knl.Task.self", "ti.sysbios.knl"));
        vo.bind("checkStacks", om.findStrict("ti.sysbios.knl.Task.checkStacks", "ti.sysbios.knl"));
        vo.bind("exit", om.findStrict("ti.sysbios.knl.Task.exit", "ti.sysbios.knl"));
        vo.bind("sleep", om.findStrict("ti.sysbios.knl.Task.sleep", "ti.sysbios.knl"));
        vo.bind("yield", om.findStrict("ti.sysbios.knl.Task.yield", "ti.sysbios.knl"));
        vo.bind("getIdleTask", om.findStrict("ti.sysbios.knl.Task.getIdleTask", "ti.sysbios.knl"));
        vo.bind("getIdleTaskHandle", om.findStrict("ti.sysbios.knl.Task.getIdleTaskHandle", "ti.sysbios.knl"));
        vo.bind("startCore", om.findStrict("ti.sysbios.knl.Task.startCore", "ti.sysbios.knl"));
        vo.bind("schedule", om.findStrict("ti.sysbios.knl.Task.schedule", "ti.sysbios.knl"));
        vo.bind("enter", om.findStrict("ti.sysbios.knl.Task.enter", "ti.sysbios.knl"));
        vo.bind("sleepTimeout", om.findStrict("ti.sysbios.knl.Task.sleepTimeout", "ti.sysbios.knl"));
        vo.bind("postInit", om.findStrict("ti.sysbios.knl.Task.postInit", "ti.sysbios.knl"));
        vo.bind("allBlockedFunction", om.findStrict("ti.sysbios.knl.Task.allBlockedFunction", "ti.sysbios.knl"));
        vo.bind("deleteTerminatedTasksFunc", om.findStrict("ti.sysbios.knl.Task.deleteTerminatedTasksFunc", "ti.sysbios.knl"));
        vo.bind("processVitalTaskFlag", om.findStrict("ti.sysbios.knl.Task.processVitalTaskFlag", "ti.sysbios.knl"));
        vo.bind("moduleStateCheck", om.findStrict("ti.sysbios.knl.Task.moduleStateCheck", "ti.sysbios.knl"));
        vo.bind("getModuleStateCheckValue", om.findStrict("ti.sysbios.knl.Task.getModuleStateCheckValue", "ti.sysbios.knl"));
        vo.bind("objectCheck", om.findStrict("ti.sysbios.knl.Task.objectCheck", "ti.sysbios.knl"));
        vo.bind("getObjectCheckValue", om.findStrict("ti.sysbios.knl.Task.getObjectCheckValue", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Task_Handle__label__E", "ti_sysbios_knl_Task_Module__startupDone__E", "ti_sysbios_knl_Task_Object__create__E", "ti_sysbios_knl_Task_Object__delete__E", "ti_sysbios_knl_Task_Object__get__E", "ti_sysbios_knl_Task_Object__first__E", "ti_sysbios_knl_Task_Object__next__E", "ti_sysbios_knl_Task_Params__init__E", "ti_sysbios_knl_Task_startup__E", "ti_sysbios_knl_Task_enabled__E", "ti_sysbios_knl_Task_unlockSched__E", "ti_sysbios_knl_Task_disable__E", "ti_sysbios_knl_Task_enable__E", "ti_sysbios_knl_Task_restore__E", "ti_sysbios_knl_Task_restoreHwi__E", "ti_sysbios_knl_Task_self__E", "ti_sysbios_knl_Task_selfMacro__E", "ti_sysbios_knl_Task_checkStacks__E", "ti_sysbios_knl_Task_exit__E", "ti_sysbios_knl_Task_sleep__E", "ti_sysbios_knl_Task_yield__E", "ti_sysbios_knl_Task_getIdleTask__E", "ti_sysbios_knl_Task_getIdleTaskHandle__E", "ti_sysbios_knl_Task_startCore__E", "ti_sysbios_knl_Task_getArg0__E", "ti_sysbios_knl_Task_getArg1__E", "ti_sysbios_knl_Task_getEnv__E", "ti_sysbios_knl_Task_getFunc__E", "ti_sysbios_knl_Task_getHookContext__E", "ti_sysbios_knl_Task_getPri__E", "ti_sysbios_knl_Task_setArg0__E", "ti_sysbios_knl_Task_setArg1__E", "ti_sysbios_knl_Task_setEnv__E", "ti_sysbios_knl_Task_setHookContext__E", "ti_sysbios_knl_Task_setPri__E", "ti_sysbios_knl_Task_stat__E", "ti_sysbios_knl_Task_getMode__E", "ti_sysbios_knl_Task_setAffinity__E", "ti_sysbios_knl_Task_getAffinity__E", "ti_sysbios_knl_Task_block__E", "ti_sysbios_knl_Task_unblock__E", "ti_sysbios_knl_Task_blockI__E", "ti_sysbios_knl_Task_unblockI__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("LM_switch", "LM_sleep", "LD_ready", "LD_block", "LM_yield", "LM_setPri", "LD_exit", "LM_setAffinity", "LM_schedule", "LM_noWork"));
        vo.bind("$$errorDescCfgs", Global.newArray("E_stackOverflow", "E_spOutOfBounds", "E_deleteNotAllowed", "E_moduleStateCheckFailed", "E_objectCheckFailed"));
        vo.bind("$$assertDescCfgs", Global.newArray("A_badThreadType", "A_badTaskState", "A_noPendElem", "A_taskDisabled", "A_badPriority", "A_badTimeout", "A_badAffinity", "A_sleepTaskDisabled", "A_invalidCoreId"));
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", "./Task.xdt");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.knl.Task.Object", "ti.sysbios.knl"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./Task.xdt");
        pkgV.bind("Task", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Task");
    }

    void Clock_TimerProxy$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Clock_TimerProxy", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Clock_TimerProxy", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("Instance", om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Instance", "ti.sysbios.knl"));
        vo.bind("Params", om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Params", "ti.sysbios.knl"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Params", "ti.sysbios.knl")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Handle", "ti.sysbios.knl"));
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
        tdefs.clear();
        proxies.clear();
        proxies.add("delegate$");
        inherits.clear();
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.interfaces.ITimer.FuncPtr", "ti.sysbios.knl"));
        vo.bind("StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.knl"));
        vo.bind("RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.knl"));
        vo.bind("Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.knl"));
        vo.bind("PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.knl"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
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
            vo.bind("__initObject", Global.get("ti$sysbios$knl$Clock_TimerProxy$$__initObject"));
        }//isCFG
        vo.bind("getNumTimers", om.findStrict("ti.sysbios.knl.Clock_TimerProxy.getNumTimers", "ti.sysbios.knl"));
        vo.bind("getStatus", om.findStrict("ti.sysbios.knl.Clock_TimerProxy.getStatus", "ti.sysbios.knl"));
        vo.bind("startup", om.findStrict("ti.sysbios.knl.Clock_TimerProxy.startup", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Clock_TimerProxy_DELEGATE__Handle__label", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__Module__startupDone", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__Object__create", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__Object__delete", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__Object__get", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__Object__first", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__Object__next", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__Params__init", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__Proxy__abstract", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__Proxy__delegate", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__viewGetCurrentClockTick", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getNumTimers", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getStatus", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__startup", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getFreqMeta", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getMaxTicks", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__setNextTick", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__start", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__stop", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__setPeriod", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__setPeriodMicroSecs", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getPeriod", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getCount", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getFreq", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getFunc", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__setFunc", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__trigger", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getExpiredCounts", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getExpiredTicks", "ti_sysbios_knl_Clock_TimerProxy_DELEGATE__getCurrentTick"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Object", "ti.sysbios.knl"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.knl.Clock_TimerProxy.Instance_State", "ti.sysbios.knl"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 1);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Clock_TimerProxy", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Clock_TimerProxy");
    }

    void Intrinsics_SupportProxy$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Intrinsics_SupportProxy", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Intrinsics_SupportProxy.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Intrinsics_SupportProxy", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
        tdefs.clear();
        proxies.clear();
        proxies.add("delegate$");
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
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
        vo.bind("maxbit", om.findStrict("ti.sysbios.knl.Intrinsics_SupportProxy.maxbit", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__Handle__label", "ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__Module__startupDone", "ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__Object__create", "ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__Object__delete", "ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__Object__get", "ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__Object__first", "ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__Object__next", "ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__Params__init", "ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__Proxy__abstract", "ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__Proxy__delegate", "ti_sysbios_knl_Intrinsics_SupportProxy_DELEGATE__maxbit"));
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
        pkgV.bind("Intrinsics_SupportProxy", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Intrinsics_SupportProxy");
    }

    void Task_SupportProxy$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Task_SupportProxy", "ti.sysbios.knl");
        po = (Proto.Obj)om.findStrict("ti.sysbios.knl.Task_SupportProxy.Module", "ti.sysbios.knl");
        vo.init2(po, "ti.sysbios.knl.Task_SupportProxy", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.knl", "ti.sysbios.knl"));
        tdefs.clear();
        proxies.clear();
        proxies.add("delegate$");
        inherits.clear();
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.interfaces.ITaskSupport.FuncPtr", "ti.sysbios.knl"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.knl")).add(vo);
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
        vo.bind("start", om.findStrict("ti.sysbios.knl.Task_SupportProxy.start", "ti.sysbios.knl"));
        vo.bind("swap", om.findStrict("ti.sysbios.knl.Task_SupportProxy.swap", "ti.sysbios.knl"));
        vo.bind("checkStack", om.findStrict("ti.sysbios.knl.Task_SupportProxy.checkStack", "ti.sysbios.knl"));
        vo.bind("stackUsed", om.findStrict("ti.sysbios.knl.Task_SupportProxy.stackUsed", "ti.sysbios.knl"));
        vo.bind("getStackAlignment", om.findStrict("ti.sysbios.knl.Task_SupportProxy.getStackAlignment", "ti.sysbios.knl"));
        vo.bind("getDefaultStackSize", om.findStrict("ti.sysbios.knl.Task_SupportProxy.getDefaultStackSize", "ti.sysbios.knl"));
        vo.bind("getCheckValueAddr", om.findStrict("ti.sysbios.knl.Task_SupportProxy.getCheckValueAddr", "ti.sysbios.knl"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_knl_Task_SupportProxy_DELEGATE__Handle__label", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__Module__startupDone", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__Object__create", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__Object__delete", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__Object__get", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__Object__first", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__Object__next", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__Params__init", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__Proxy__abstract", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__Proxy__delegate", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__start", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__swap", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__checkStack", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__stackUsed", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__getStackAlignment", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__getDefaultStackSize", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__getCheckValueAddr", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__stackUsed$view", "ti_sysbios_knl_Task_SupportProxy_DELEGATE__getCallStack$view"));
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
        pkgV.bind("Task_SupportProxy", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Task_SupportProxy");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.sysbios.knl")).findStrict("PARAMS", "ti.sysbios.knl");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.knl.Clock", "ti.sysbios.knl")).findStrict("PARAMS", "ti.sysbios.knl");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.knl.Event", "ti.sysbios.knl")).findStrict("PARAMS", "ti.sysbios.knl");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.knl.Mailbox", "ti.sysbios.knl")).findStrict("PARAMS", "ti.sysbios.knl");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.knl.Queue", "ti.sysbios.knl")).findStrict("PARAMS", "ti.sysbios.knl");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.knl.Semaphore", "ti.sysbios.knl")).findStrict("PARAMS", "ti.sysbios.knl");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.knl.Swi", "ti.sysbios.knl")).findStrict("PARAMS", "ti.sysbios.knl");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.knl.Task", "ti.sysbios.knl")).findStrict("PARAMS", "ti.sysbios.knl");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.knl.Clock_TimerProxy", "ti.sysbios.knl")).findStrict("PARAMS", "ti.sysbios.knl");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Clock", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Idle", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Intrinsics", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Event", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Mailbox", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Queue", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Semaphore", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Swi", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Task", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Clock_TimerProxy", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Intrinsics_SupportProxy", "ti.sysbios.knl"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.knl.Task_SupportProxy", "ti.sysbios.knl"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Clock", "ti.sysbios.knl");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Idle", "ti.sysbios.knl");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Idle.funcList", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Event", "ti.sysbios.knl");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Mailbox", "ti.sysbios.knl");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Detailed", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitDetailed", "structName", "DetailedView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Queue", "ti.sysbios.knl");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitInstance", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Semaphore", "ti.sysbios.knl");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Swi", "ti.sysbios.knl");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")}), Global.newArray(new Object[]{"ReadyQs", Global.newObject("type", om.find("xdc.rov.ViewInfo.TREE_TABLE"), "viewInitFxn", "viewInitReadyQs", "structName", "ReadyQView")})}))));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.knl.Task", "ti.sysbios.knl");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Detailed", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitDetailed", "structName", "DetailedView")}), Global.newArray(new Object[]{"CallStacks", Global.newObject("type", om.find("xdc.rov.ViewInfo.TREE"), "viewInitFxn", "viewInitCallStack", "structName", "CallStackView")}), Global.newArray(new Object[]{"ReadyQs", Global.newObject("type", om.find("xdc.rov.ViewInfo.TREE_TABLE"), "viewInitFxn", "viewInitReadyQs", "structName", "ReadyQView")}), Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.knl.Clock")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Idle")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Intrinsics")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Event")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Mailbox")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Queue")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Semaphore")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Swi")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Task")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Clock_TimerProxy")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Intrinsics_SupportProxy")).bless();
        ((Value.Obj)om.getv("ti.sysbios.knl.Task_SupportProxy")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.knl")).add(pkgV);
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
        Clock$$OBJECTS();
        Idle$$OBJECTS();
        Intrinsics$$OBJECTS();
        Event$$OBJECTS();
        Mailbox$$OBJECTS();
        Queue$$OBJECTS();
        Semaphore$$OBJECTS();
        Swi$$OBJECTS();
        Task$$OBJECTS();
        Clock_TimerProxy$$OBJECTS();
        Intrinsics_SupportProxy$$OBJECTS();
        Task_SupportProxy$$OBJECTS();
        Clock$$CONSTS();
        Idle$$CONSTS();
        Intrinsics$$CONSTS();
        Event$$CONSTS();
        Mailbox$$CONSTS();
        Queue$$CONSTS();
        Semaphore$$CONSTS();
        Swi$$CONSTS();
        Task$$CONSTS();
        Clock_TimerProxy$$CONSTS();
        Intrinsics_SupportProxy$$CONSTS();
        Task_SupportProxy$$CONSTS();
        Clock$$CREATES();
        Idle$$CREATES();
        Intrinsics$$CREATES();
        Event$$CREATES();
        Mailbox$$CREATES();
        Queue$$CREATES();
        Semaphore$$CREATES();
        Swi$$CREATES();
        Task$$CREATES();
        Clock_TimerProxy$$CREATES();
        Intrinsics_SupportProxy$$CREATES();
        Task_SupportProxy$$CREATES();
        Clock$$FUNCTIONS();
        Idle$$FUNCTIONS();
        Intrinsics$$FUNCTIONS();
        Event$$FUNCTIONS();
        Mailbox$$FUNCTIONS();
        Queue$$FUNCTIONS();
        Semaphore$$FUNCTIONS();
        Swi$$FUNCTIONS();
        Task$$FUNCTIONS();
        Clock_TimerProxy$$FUNCTIONS();
        Intrinsics_SupportProxy$$FUNCTIONS();
        Task_SupportProxy$$FUNCTIONS();
        Clock$$SIZES();
        Idle$$SIZES();
        Intrinsics$$SIZES();
        Event$$SIZES();
        Mailbox$$SIZES();
        Queue$$SIZES();
        Semaphore$$SIZES();
        Swi$$SIZES();
        Task$$SIZES();
        Clock_TimerProxy$$SIZES();
        Intrinsics_SupportProxy$$SIZES();
        Task_SupportProxy$$SIZES();
        Clock$$TYPES();
        Idle$$TYPES();
        Intrinsics$$TYPES();
        Event$$TYPES();
        Mailbox$$TYPES();
        Queue$$TYPES();
        Semaphore$$TYPES();
        Swi$$TYPES();
        Task$$TYPES();
        Clock_TimerProxy$$TYPES();
        Intrinsics_SupportProxy$$TYPES();
        Task_SupportProxy$$TYPES();
        if (isROV) {
            Clock$$ROV();
            Idle$$ROV();
            Intrinsics$$ROV();
            Event$$ROV();
            Mailbox$$ROV();
            Queue$$ROV();
            Semaphore$$ROV();
            Swi$$ROV();
            Task$$ROV();
            Clock_TimerProxy$$ROV();
            Intrinsics_SupportProxy$$ROV();
            Task_SupportProxy$$ROV();
        }//isROV
        $$SINGLETONS();
        Clock$$SINGLETONS();
        Idle$$SINGLETONS();
        Intrinsics$$SINGLETONS();
        Event$$SINGLETONS();
        Mailbox$$SINGLETONS();
        Queue$$SINGLETONS();
        Semaphore$$SINGLETONS();
        Swi$$SINGLETONS();
        Task$$SINGLETONS();
        Clock_TimerProxy$$SINGLETONS();
        Intrinsics_SupportProxy$$SINGLETONS();
        Task_SupportProxy$$SINGLETONS();
        $$INITIALIZATION();
    }
}
