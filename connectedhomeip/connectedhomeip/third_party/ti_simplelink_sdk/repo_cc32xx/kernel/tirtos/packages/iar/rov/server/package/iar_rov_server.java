/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-F07
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class iar_rov_server
{
    static final String VERS = "@(#) xdc-F07\n";

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
        Global.callFxn("loadPackage", xdcO, "xdc.tools");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("iar.rov.server.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("iar.rov.server", new Value.Obj("iar.rov.server", pkgP));
    }

    void CallBack$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.rov.server.CallBack.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.rov.server.CallBack", new Value.Obj("iar.rov.server.CallBack", po));
        pkgV.bind("CallBack", vo);
        // decls 
        // insts 
        Object insP = om.bind("iar.rov.server.CallBack.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("iar.rov.server.CallBack$$Object", new Proto.Obj());
        om.bind("iar.rov.server.CallBack.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("iar.rov.server.CallBack$$Params", new Proto.Obj());
        om.bind("iar.rov.server.CallBack.Params", new Proto.Str(po, true));
    }

    void CallStack$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.rov.server.CallStack.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.rov.server.CallStack", new Value.Obj("iar.rov.server.CallStack", po));
        pkgV.bind("CallStack", vo);
        // decls 
        // insts 
        Object insP = om.bind("iar.rov.server.CallStack.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("iar.rov.server.CallStack$$Object", new Proto.Obj());
        om.bind("iar.rov.server.CallStack.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("iar.rov.server.CallStack$$Params", new Proto.Obj());
        om.bind("iar.rov.server.CallStack.Params", new Proto.Str(po, true));
    }

    void Main$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.rov.server.Main.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.rov.server.Main", new Value.Obj("iar.rov.server.Main", po));
        pkgV.bind("Main", vo);
        // decls 
        // insts 
        Object insP = om.bind("iar.rov.server.Main.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("iar.rov.server.Main$$Object", new Proto.Obj());
        om.bind("iar.rov.server.Main.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("iar.rov.server.Main$$Params", new Proto.Obj());
        om.bind("iar.rov.server.Main.Params", new Proto.Str(po, true));
    }

    void Server$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.rov.server.Server.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.rov.server.Server", new Value.Obj("iar.rov.server.Server", po));
        pkgV.bind("Server", vo);
        // decls 
    }

    void SymbolTable$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("iar.rov.server.SymbolTable.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("iar.rov.server.SymbolTable", new Value.Obj("iar.rov.server.SymbolTable", po));
        pkgV.bind("SymbolTable", vo);
        // decls 
        // insts 
        Object insP = om.bind("iar.rov.server.SymbolTable.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("iar.rov.server.SymbolTable$$Object", new Proto.Obj());
        om.bind("iar.rov.server.SymbolTable.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("iar.rov.server.SymbolTable$$Params", new Proto.Obj());
        om.bind("iar.rov.server.SymbolTable.Params", new Proto.Str(po, true));
    }

    void CallBack$$CONSTS()
    {
        // module CallBack
    }

    void CallStack$$CONSTS()
    {
        // module CallStack
    }

    void Main$$CONSTS()
    {
        // module Main
    }

    void Server$$CONSTS()
    {
        // module Server
    }

    void SymbolTable$$CONSTS()
    {
        // module SymbolTable
    }

    void CallBack$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("iar.rov.server.CallBack$$create", new Proto.Fxn(om.findStrict("iar.rov.server.CallBack.Module", "iar.rov.server"), om.findStrict("iar.rov.server.CallBack.Instance", "iar.rov.server"), 1, 0, false));
                fxn.addArg(0, "__params", (Proto)om.findStrict("iar.rov.server.CallBack.Params", "iar.rov.server"), Global.newObject());
        sb = new StringBuilder();
        sb.append("iar$rov$server$CallBack$$create = function( __params ) {\n");
            sb.append("var __mod = xdc.om['iar.rov.server.CallBack'];\n");
            sb.append("var __inst = xdc.om['iar.rov.server.CallBack.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['iar.rov.server']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("iar.rov.server.CallBack$$construct", new Proto.Fxn(om.findStrict("iar.rov.server.CallBack.Module", "iar.rov.server"), null, 2, 0, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("iar.rov.server.CallBack$$Object", "iar.rov.server"), null);
                fxn.addArg(1, "__params", (Proto)om.findStrict("iar.rov.server.CallBack.Params", "iar.rov.server"), Global.newObject());
        sb = new StringBuilder();
        sb.append("iar$rov$server$CallBack$$construct = function( __obj, __params ) {\n");
            sb.append("var __mod = xdc.om['iar.rov.server.CallBack'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void CallStack$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("iar.rov.server.CallStack$$create", new Proto.Fxn(om.findStrict("iar.rov.server.CallStack.Module", "iar.rov.server"), om.findStrict("iar.rov.server.CallStack.Instance", "iar.rov.server"), 2, 1, false));
                fxn.addArg(0, "memoryImage", $$T_Obj, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("iar.rov.server.CallStack.Params", "iar.rov.server"), Global.newObject());
        sb = new StringBuilder();
        sb.append("iar$rov$server$CallStack$$create = function( memoryImage, __params ) {\n");
            sb.append("var __mod = xdc.om['iar.rov.server.CallStack'];\n");
            sb.append("var __inst = xdc.om['iar.rov.server.CallStack.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['iar.rov.server']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {memoryImage:memoryImage});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [memoryImage]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("iar.rov.server.CallStack$$construct", new Proto.Fxn(om.findStrict("iar.rov.server.CallStack.Module", "iar.rov.server"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("iar.rov.server.CallStack$$Object", "iar.rov.server"), null);
                fxn.addArg(1, "memoryImage", $$T_Obj, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("iar.rov.server.CallStack.Params", "iar.rov.server"), Global.newObject());
        sb = new StringBuilder();
        sb.append("iar$rov$server$CallStack$$construct = function( __obj, memoryImage, __params ) {\n");
            sb.append("var __mod = xdc.om['iar.rov.server.CallStack'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {memoryImage:memoryImage});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void Main$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("iar.rov.server.Main$$create", new Proto.Fxn(om.findStrict("iar.rov.server.Main.Module", "iar.rov.server"), om.findStrict("iar.rov.server.Main.Instance", "iar.rov.server"), 1, 0, false));
                fxn.addArg(0, "__params", (Proto)om.findStrict("iar.rov.server.Main.Params", "iar.rov.server"), Global.newObject());
        sb = new StringBuilder();
        sb.append("iar$rov$server$Main$$create = function( __params ) {\n");
            sb.append("var __mod = xdc.om['iar.rov.server.Main'];\n");
            sb.append("var __inst = xdc.om['iar.rov.server.Main.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['iar.rov.server']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("iar.rov.server.Main$$construct", new Proto.Fxn(om.findStrict("iar.rov.server.Main.Module", "iar.rov.server"), null, 2, 0, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("iar.rov.server.Main$$Object", "iar.rov.server"), null);
                fxn.addArg(1, "__params", (Proto)om.findStrict("iar.rov.server.Main.Params", "iar.rov.server"), Global.newObject());
        sb = new StringBuilder();
        sb.append("iar$rov$server$Main$$construct = function( __obj, __params ) {\n");
            sb.append("var __mod = xdc.om['iar.rov.server.Main'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void Server$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void SymbolTable$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("iar.rov.server.SymbolTable$$create", new Proto.Fxn(om.findStrict("iar.rov.server.SymbolTable.Module", "iar.rov.server"), om.findStrict("iar.rov.server.SymbolTable.Instance", "iar.rov.server"), 2, 1, false));
                fxn.addArg(0, "ISymInst", $$T_Obj, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("iar.rov.server.SymbolTable.Params", "iar.rov.server"), Global.newObject());
        sb = new StringBuilder();
        sb.append("iar$rov$server$SymbolTable$$create = function( ISymInst, __params ) {\n");
            sb.append("var __mod = xdc.om['iar.rov.server.SymbolTable'];\n");
            sb.append("var __inst = xdc.om['iar.rov.server.SymbolTable.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['iar.rov.server']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {ISymInst:ISymInst});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [ISymInst]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("iar.rov.server.SymbolTable$$construct", new Proto.Fxn(om.findStrict("iar.rov.server.SymbolTable.Module", "iar.rov.server"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("iar.rov.server.SymbolTable$$Object", "iar.rov.server"), null);
                fxn.addArg(1, "ISymInst", $$T_Obj, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("iar.rov.server.SymbolTable.Params", "iar.rov.server"), Global.newObject());
        sb = new StringBuilder();
        sb.append("iar$rov$server$SymbolTable$$construct = function( __obj, ISymInst, __params ) {\n");
            sb.append("var __mod = xdc.om['iar.rov.server.SymbolTable'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {ISymInst:ISymInst});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void CallBack$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CallStack$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Main$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Server$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Server.startModel
        fxn = (Proto.Fxn)om.bind("iar.rov.server.Server$$startModel", new Proto.Fxn(om.findStrict("iar.rov.server.Server.Module", "iar.rov.server"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "executable", $$T_Str, $$UNDEF);
        // fxn Server.retrieveData
        fxn = (Proto.Fxn)om.bind("iar.rov.server.Server$$retrieveData", new Proto.Fxn(om.findStrict("iar.rov.server.Server.Module", "iar.rov.server"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "modTab", $$T_Str, $$UNDEF);
        // fxn Server.retrieveModuleList
        fxn = (Proto.Fxn)om.bind("iar.rov.server.Server$$retrieveModuleList", new Proto.Fxn(om.findStrict("iar.rov.server.Server.Module", "iar.rov.server"), $$T_Str, 0, 0, false));
        // fxn Server.retrieveLimitedList
        fxn = (Proto.Fxn)om.bind("iar.rov.server.Server$$retrieveLimitedList", new Proto.Fxn(om.findStrict("iar.rov.server.Server.Module", "iar.rov.server"), $$T_Str, 0, 0, false));
    }

    void SymbolTable$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void CallBack$$SIZES()
    {
    }

    void CallStack$$SIZES()
    {
    }

    void Main$$SIZES()
    {
    }

    void Server$$SIZES()
    {
    }

    void SymbolTable$$SIZES()
    {
    }

    void CallBack$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "iar/rov/server/CallBack.xs");
        om.bind("iar.rov.server.CallBack$$capsule", cap);
        po = (Proto.Obj)om.findStrict("iar.rov.server.CallBack.Module", "iar.rov.server");
        po.init("iar.rov.server.CallBack.Module", om.findStrict("xdc.rov.ICallBack.Module", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("iar.rov.server.CallBack$$create", "iar.rov.server"), Global.get("iar$rov$server$CallBack$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("iar.rov.server.CallBack$$construct", "iar.rov.server"), Global.get("iar$rov$server$CallBack$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("iar.rov.server.CallBack$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("iar.rov.server.CallBack$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("iar.rov.server.CallBack$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("iar.rov.server.CallBack$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("iar.rov.server.CallBack.Instance", "iar.rov.server");
        po.init("iar.rov.server.CallBack.Instance", om.findStrict("xdc.rov.ICallBack.Instance", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                fxn = Global.get(cap, "updateStartupProgress");
                if (fxn != null) po.addFxn("updateStartupProgress", (Proto.Fxn)om.findStrict("xdc.rov.ICallBack$$updateStartupProgress", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "getAbortFlag");
                if (fxn != null) po.addFxn("getAbortFlag", (Proto.Fxn)om.findStrict("xdc.rov.ICallBack$$getAbortFlag", "iar.rov.server"), fxn);
        po = (Proto.Obj)om.findStrict("iar.rov.server.CallBack$$Params", "iar.rov.server");
        po.init("iar.rov.server.CallBack.Params", om.findStrict("xdc.rov.ICallBack$$Params", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("iar.rov.server.CallBack$$Object", "iar.rov.server");
        po.init("iar.rov.server.CallBack.Object", om.findStrict("iar.rov.server.CallBack.Instance", "iar.rov.server"));
                fxn = Global.get(cap, "updateStartupProgress");
                if (fxn != null) po.addFxn("updateStartupProgress", (Proto.Fxn)om.findStrict("xdc.rov.ICallBack$$updateStartupProgress", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "getAbortFlag");
                if (fxn != null) po.addFxn("getAbortFlag", (Proto.Fxn)om.findStrict("xdc.rov.ICallBack$$getAbortFlag", "iar.rov.server"), fxn);
    }

    void CallStack$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "iar/rov/server/CallStack.xs");
        om.bind("iar.rov.server.CallStack$$capsule", cap);
        po = (Proto.Obj)om.findStrict("iar.rov.server.CallStack.Module", "iar.rov.server");
        po.init("iar.rov.server.CallStack.Module", om.findStrict("xdc.rov.ICallStack.Module", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("iar.rov.server.CallStack$$create", "iar.rov.server"), Global.get("iar$rov$server$CallStack$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("iar.rov.server.CallStack$$construct", "iar.rov.server"), Global.get("iar$rov$server$CallStack$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("iar.rov.server.CallStack$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("iar.rov.server.CallStack$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("iar.rov.server.CallStack$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("iar.rov.server.CallStack$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("iar.rov.server.CallStack.Instance", "iar.rov.server");
        po.init("iar.rov.server.CallStack.Instance", om.findStrict("xdc.rov.ICallStack.Instance", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                fxn = Global.get(cap, "clearRegisters");
                if (fxn != null) po.addFxn("clearRegisters", (Proto.Fxn)om.findStrict("xdc.rov.ICallStack$$clearRegisters", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "fetchRegisters");
                if (fxn != null) po.addFxn("fetchRegisters", (Proto.Fxn)om.findStrict("xdc.rov.ICallStack$$fetchRegisters", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "getRegister");
                if (fxn != null) po.addFxn("getRegister", (Proto.Fxn)om.findStrict("xdc.rov.ICallStack$$getRegister", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "setRegister");
                if (fxn != null) po.addFxn("setRegister", (Proto.Fxn)om.findStrict("xdc.rov.ICallStack$$setRegister", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "toText");
                if (fxn != null) po.addFxn("toText", (Proto.Fxn)om.findStrict("xdc.rov.ICallStack$$toText", "iar.rov.server"), fxn);
        po = (Proto.Obj)om.findStrict("iar.rov.server.CallStack$$Params", "iar.rov.server");
        po.init("iar.rov.server.CallStack.Params", om.findStrict("xdc.rov.ICallStack$$Params", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("iar.rov.server.CallStack$$Object", "iar.rov.server");
        po.init("iar.rov.server.CallStack.Object", om.findStrict("iar.rov.server.CallStack.Instance", "iar.rov.server"));
                fxn = Global.get(cap, "clearRegisters");
                if (fxn != null) po.addFxn("clearRegisters", (Proto.Fxn)om.findStrict("xdc.rov.ICallStack$$clearRegisters", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "fetchRegisters");
                if (fxn != null) po.addFxn("fetchRegisters", (Proto.Fxn)om.findStrict("xdc.rov.ICallStack$$fetchRegisters", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "getRegister");
                if (fxn != null) po.addFxn("getRegister", (Proto.Fxn)om.findStrict("xdc.rov.ICallStack$$getRegister", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "setRegister");
                if (fxn != null) po.addFxn("setRegister", (Proto.Fxn)om.findStrict("xdc.rov.ICallStack$$setRegister", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "toText");
                if (fxn != null) po.addFxn("toText", (Proto.Fxn)om.findStrict("xdc.rov.ICallStack$$toText", "iar.rov.server"), fxn);
    }

    void Main$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "iar/rov/server/Main.xs");
        om.bind("iar.rov.server.Main$$capsule", cap);
        po = (Proto.Obj)om.findStrict("iar.rov.server.Main.Module", "iar.rov.server");
        po.init("iar.rov.server.Main.Module", om.findStrict("xdc.tools.ICmd.Module", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("usage", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{" ", "Usage", "[-e executable]", "[--clientVersion <N>]", "[-m Module Tab]", "[-l]", "[--help]", " "}), "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("iar.rov.server.Main$$create", "iar.rov.server"), Global.get("iar$rov$server$Main$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("iar.rov.server.Main$$construct", "iar.rov.server"), Global.get("iar$rov$server$Main$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("iar.rov.server.Main$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("iar.rov.server.Main$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("iar.rov.server.Main$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("iar.rov.server.Main$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "main");
                if (fxn != null) po.addFxn("main", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$main", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "exec");
                if (fxn != null) po.addFxn("exec", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$exec", "iar.rov.server"), fxn);
        po = (Proto.Obj)om.findStrict("iar.rov.server.Main.Instance", "iar.rov.server");
        po.init("iar.rov.server.Main.Instance", om.findStrict("xdc.tools.ICmd.Instance", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("modTab", $$T_Str, "", "wh");
        po.addFld("executable", $$T_Str, "", "wh");
        po.addFld("listMod", $$T_Bool, false, "wh");
        po.addFld("listAll", $$T_Bool, false, "wh");
        po.addFld("clientVersion", Proto.Elm.newCNum("(xdc_UInt)"), 3L, "wh");
                fxn = Global.get(cap, "run");
                if (fxn != null) po.addFxn("run", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$run", "iar.rov.server"), fxn);
        po = (Proto.Obj)om.findStrict("iar.rov.server.Main$$Params", "iar.rov.server");
        po.init("iar.rov.server.Main.Params", om.findStrict("xdc.tools.ICmd$$Params", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("modTab", $$T_Str, "", "wh");
        po.addFld("executable", $$T_Str, "", "wh");
        po.addFld("listMod", $$T_Bool, false, "wh");
        po.addFld("listAll", $$T_Bool, false, "wh");
        po.addFld("clientVersion", Proto.Elm.newCNum("(xdc_UInt)"), 3L, "wh");
        po = (Proto.Obj)om.findStrict("iar.rov.server.Main$$Object", "iar.rov.server");
        po.init("iar.rov.server.Main.Object", om.findStrict("iar.rov.server.Main.Instance", "iar.rov.server"));
                fxn = Global.get(cap, "run");
                if (fxn != null) po.addFxn("run", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$run", "iar.rov.server"), fxn);
    }

    void Server$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "iar/rov/server/Server.xs");
        om.bind("iar.rov.server.Server$$capsule", cap);
        po = (Proto.Obj)om.findStrict("iar.rov.server.Server.Module", "iar.rov.server");
        po.init("iar.rov.server.Server.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("modelVers", Proto.Elm.newCNum("(xdc_Int)"), 5L, "wh");
        po.addFld("clientVers", Proto.Elm.newCNum("(xdc_UInt)"), 3L, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("iar.rov.server.Server$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("iar.rov.server.Server$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("iar.rov.server.Server$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("startModel", (Proto.Fxn)om.findStrict("iar.rov.server.Server$$startModel", "iar.rov.server"), Global.get(cap, "startModel"));
                po.addFxn("retrieveData", (Proto.Fxn)om.findStrict("iar.rov.server.Server$$retrieveData", "iar.rov.server"), Global.get(cap, "retrieveData"));
                po.addFxn("retrieveModuleList", (Proto.Fxn)om.findStrict("iar.rov.server.Server$$retrieveModuleList", "iar.rov.server"), Global.get(cap, "retrieveModuleList"));
                po.addFxn("retrieveLimitedList", (Proto.Fxn)om.findStrict("iar.rov.server.Server$$retrieveLimitedList", "iar.rov.server"), Global.get(cap, "retrieveLimitedList"));
    }

    void SymbolTable$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "iar/rov/server/SymbolTable.xs");
        om.bind("iar.rov.server.SymbolTable$$capsule", cap);
        po = (Proto.Obj)om.findStrict("iar.rov.server.SymbolTable.Module", "iar.rov.server");
        po.init("iar.rov.server.SymbolTable.Module", om.findStrict("xdc.rov.ISymbolTable.Module", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("iar.rov.server.SymbolTable$$create", "iar.rov.server"), Global.get("iar$rov$server$SymbolTable$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("iar.rov.server.SymbolTable$$construct", "iar.rov.server"), Global.get("iar$rov$server$SymbolTable$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("iar.rov.server.SymbolTable$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("iar.rov.server.SymbolTable$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("iar.rov.server.SymbolTable$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("iar.rov.server.SymbolTable$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("iar.rov.server.SymbolTable.Instance", "iar.rov.server");
        po.init("iar.rov.server.SymbolTable.Instance", om.findStrict("xdc.rov.ISymbolTable.Instance", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                fxn = Global.get(cap, "getSymbolValue");
                if (fxn != null) po.addFxn("getSymbolValue", (Proto.Fxn)om.findStrict("xdc.rov.ISymbolTable$$getSymbolValue", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "lookupDataSymbol");
                if (fxn != null) po.addFxn("lookupDataSymbol", (Proto.Fxn)om.findStrict("xdc.rov.ISymbolTable$$lookupDataSymbol", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "lookupFuncName");
                if (fxn != null) po.addFxn("lookupFuncName", (Proto.Fxn)om.findStrict("xdc.rov.ISymbolTable$$lookupFuncName", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "getISA");
                if (fxn != null) po.addFxn("getISA", (Proto.Fxn)om.findStrict("xdc.rov.ISymbolTable$$getISA", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "getJavaImp");
                if (fxn != null) po.addFxn("getJavaImp", (Proto.Fxn)om.findStrict("xdc.rov.ISymbolTable$$getJavaImp", "iar.rov.server"), fxn);
        po = (Proto.Obj)om.findStrict("iar.rov.server.SymbolTable$$Params", "iar.rov.server");
        po.init("iar.rov.server.SymbolTable.Params", om.findStrict("xdc.rov.ISymbolTable$$Params", "iar.rov.server"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("iar.rov.server.SymbolTable$$Object", "iar.rov.server");
        po.init("iar.rov.server.SymbolTable.Object", om.findStrict("iar.rov.server.SymbolTable.Instance", "iar.rov.server"));
                fxn = Global.get(cap, "getSymbolValue");
                if (fxn != null) po.addFxn("getSymbolValue", (Proto.Fxn)om.findStrict("xdc.rov.ISymbolTable$$getSymbolValue", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "lookupDataSymbol");
                if (fxn != null) po.addFxn("lookupDataSymbol", (Proto.Fxn)om.findStrict("xdc.rov.ISymbolTable$$lookupDataSymbol", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "lookupFuncName");
                if (fxn != null) po.addFxn("lookupFuncName", (Proto.Fxn)om.findStrict("xdc.rov.ISymbolTable$$lookupFuncName", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "getISA");
                if (fxn != null) po.addFxn("getISA", (Proto.Fxn)om.findStrict("xdc.rov.ISymbolTable$$getISA", "iar.rov.server"), fxn);
                fxn = Global.get(cap, "getJavaImp");
                if (fxn != null) po.addFxn("getJavaImp", (Proto.Fxn)om.findStrict("xdc.rov.ISymbolTable$$getJavaImp", "iar.rov.server"), fxn);
    }

    void CallBack$$ROV()
    {
    }

    void CallStack$$ROV()
    {
    }

    void Main$$ROV()
    {
    }

    void Server$$ROV()
    {
    }

    void SymbolTable$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("iar.rov.server.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "iar.rov.server"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "iar.rov.server", Value.DEFAULT, false);
        pkgV.bind("$name", "iar.rov.server");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "iar.rov.server.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.rov", Global.newArray()));
        imports.add(Global.newArray("xdc.rta", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['iar.rov.server'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('iar.rov.server$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['iar.rov.server$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['iar.rov.server$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void CallBack$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.rov.server.CallBack", "iar.rov.server");
        po = (Proto.Obj)om.findStrict("iar.rov.server.CallBack.Module", "iar.rov.server");
        vo.init2(po, "iar.rov.server.CallBack", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("iar.rov.server.CallBack$$capsule", "iar.rov.server"));
        vo.bind("Instance", om.findStrict("iar.rov.server.CallBack.Instance", "iar.rov.server"));
        vo.bind("Params", om.findStrict("iar.rov.server.CallBack.Params", "iar.rov.server"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("iar.rov.server.CallBack.Params", "iar.rov.server")).newInstance());
        vo.bind("$package", om.findStrict("iar.rov.server", "iar.rov.server"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.rov");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.rov.server")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.rov.server.CallBack$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("iar.rov.server.CallBack.Object", "iar.rov.server"));
        pkgV.bind("CallBack", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CallBack");
    }

    void CallStack$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.rov.server.CallStack", "iar.rov.server");
        po = (Proto.Obj)om.findStrict("iar.rov.server.CallStack.Module", "iar.rov.server");
        vo.init2(po, "iar.rov.server.CallStack", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("iar.rov.server.CallStack$$capsule", "iar.rov.server"));
        vo.bind("Instance", om.findStrict("iar.rov.server.CallStack.Instance", "iar.rov.server"));
        vo.bind("Params", om.findStrict("iar.rov.server.CallStack.Params", "iar.rov.server"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("iar.rov.server.CallStack.Params", "iar.rov.server")).newInstance());
        vo.bind("$package", om.findStrict("iar.rov.server", "iar.rov.server"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.rov");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.rov.server")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.rov.server.CallStack$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("iar.rov.server.CallStack.Object", "iar.rov.server"));
        pkgV.bind("CallStack", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CallStack");
    }

    void Main$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.rov.server.Main", "iar.rov.server");
        po = (Proto.Obj)om.findStrict("iar.rov.server.Main.Module", "iar.rov.server");
        vo.init2(po, "iar.rov.server.Main", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("iar.rov.server.Main$$capsule", "iar.rov.server"));
        vo.bind("Instance", om.findStrict("iar.rov.server.Main.Instance", "iar.rov.server"));
        vo.bind("Params", om.findStrict("iar.rov.server.Main.Params", "iar.rov.server"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("iar.rov.server.Main.Params", "iar.rov.server")).newInstance());
        vo.bind("$package", om.findStrict("iar.rov.server", "iar.rov.server"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.tools");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.rov.server")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.rov.server.Main$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("iar.rov.server.Main.Object", "iar.rov.server"));
        pkgV.bind("Main", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Main");
    }

    void Server$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.rov.server.Server", "iar.rov.server");
        po = (Proto.Obj)om.findStrict("iar.rov.server.Server.Module", "iar.rov.server");
        vo.init2(po, "iar.rov.server.Server", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("iar.rov.server.Server$$capsule", "iar.rov.server"));
        vo.bind("$package", om.findStrict("iar.rov.server", "iar.rov.server"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.rov.server")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.rov.server.Server$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("Server", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Server");
    }

    void SymbolTable$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("iar.rov.server.SymbolTable", "iar.rov.server");
        po = (Proto.Obj)om.findStrict("iar.rov.server.SymbolTable.Module", "iar.rov.server");
        vo.init2(po, "iar.rov.server.SymbolTable", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("iar.rov.server.SymbolTable$$capsule", "iar.rov.server"));
        vo.bind("Instance", om.findStrict("iar.rov.server.SymbolTable.Instance", "iar.rov.server"));
        vo.bind("Params", om.findStrict("iar.rov.server.SymbolTable.Params", "iar.rov.server"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("iar.rov.server.SymbolTable.Params", "iar.rov.server")).newInstance());
        vo.bind("$package", om.findStrict("iar.rov.server", "iar.rov.server"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.rov");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "iar.rov.server")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("iar.rov.server.SymbolTable$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("iar.rov.server.SymbolTable.Object", "iar.rov.server"));
        pkgV.bind("SymbolTable", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SymbolTable");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.rov.server.CallBack", "iar.rov.server"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.rov.server.CallStack", "iar.rov.server"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.rov.server.Main", "iar.rov.server"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.rov.server.Server", "iar.rov.server"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("iar.rov.server.SymbolTable", "iar.rov.server"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("iar.rov.server.CallBack")).bless();
        ((Value.Obj)om.getv("iar.rov.server.CallStack")).bless();
        ((Value.Obj)om.getv("iar.rov.server.Main")).bless();
        ((Value.Obj)om.getv("iar.rov.server.Server")).bless();
        ((Value.Obj)om.getv("iar.rov.server.SymbolTable")).bless();
        ((Value.Arr)om.findStrict("$packages", "iar.rov.server")).add(pkgV);
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
        CallBack$$OBJECTS();
        CallStack$$OBJECTS();
        Main$$OBJECTS();
        Server$$OBJECTS();
        SymbolTable$$OBJECTS();
        CallBack$$CONSTS();
        CallStack$$CONSTS();
        Main$$CONSTS();
        Server$$CONSTS();
        SymbolTable$$CONSTS();
        CallBack$$CREATES();
        CallStack$$CREATES();
        Main$$CREATES();
        Server$$CREATES();
        SymbolTable$$CREATES();
        CallBack$$FUNCTIONS();
        CallStack$$FUNCTIONS();
        Main$$FUNCTIONS();
        Server$$FUNCTIONS();
        SymbolTable$$FUNCTIONS();
        CallBack$$SIZES();
        CallStack$$SIZES();
        Main$$SIZES();
        Server$$SIZES();
        SymbolTable$$SIZES();
        CallBack$$TYPES();
        CallStack$$TYPES();
        Main$$TYPES();
        Server$$TYPES();
        SymbolTable$$TYPES();
        if (isROV) {
            CallBack$$ROV();
            CallStack$$ROV();
            Main$$ROV();
            Server$$ROV();
            SymbolTable$$ROV();
        }//isROV
        $$SINGLETONS();
        CallBack$$SINGLETONS();
        CallStack$$SINGLETONS();
        Main$$SINGLETONS();
        Server$$SINGLETONS();
        SymbolTable$$SINGLETONS();
        $$INITIALIZATION();
    }
}
