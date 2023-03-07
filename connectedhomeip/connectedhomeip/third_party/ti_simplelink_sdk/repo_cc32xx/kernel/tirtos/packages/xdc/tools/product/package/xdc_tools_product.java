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

public class xdc_tools_product
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
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("xdc.tools.product.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("xdc.tools.product", new Value.Obj("xdc.tools.product", pkgP));
    }

    void IProductView$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("xdc.tools.product.IProductView.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("xdc.tools.product.IProductView", new Value.Obj("xdc.tools.product.IProductView", po));
        pkgV.bind("IProductView", vo);
        // decls 
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProductView$$ProductElemDesc", new Proto.Obj());
        om.bind("xdc.tools.product.IProductView.ProductElemDesc", new Proto.Str(spo, true));
    }

    void IProduct$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("xdc.tools.product.IProduct.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("xdc.tools.product.IProduct", new Value.Obj("xdc.tools.product.IProduct", po));
        pkgV.bind("IProduct", vo);
        // decls 
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProduct$$HelpToc", new Proto.Obj());
        om.bind("xdc.tools.product.IProduct.HelpToc", new Proto.Str(spo, true));
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProduct$$Target", new Proto.Obj());
        om.bind("xdc.tools.product.IProduct.Target", new Proto.Str(spo, true));
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProduct$$UrlDescriptor", new Proto.Obj());
        om.bind("xdc.tools.product.IProduct.UrlDescriptor", new Proto.Str(spo, true));
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProduct$$PluginDescriptor", new Proto.Obj());
        om.bind("xdc.tools.product.IProduct.PluginDescriptor", new Proto.Str(spo, true));
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProduct$$MacroDescriptor", new Proto.Obj());
        om.bind("xdc.tools.product.IProduct.MacroDescriptor", new Proto.Str(spo, true));
    }

    void IProductTemplate$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("xdc.tools.product.IProductTemplate.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("xdc.tools.product.IProductTemplate", new Value.Obj("xdc.tools.product.IProductTemplate", po));
        pkgV.bind("IProductTemplate", vo);
        // decls 
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProductTemplate$$FileDesc", new Proto.Obj());
        om.bind("xdc.tools.product.IProductTemplate.FileDesc", new Proto.Str(spo, true));
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProductTemplate$$Board", new Proto.Obj());
        om.bind("xdc.tools.product.IProductTemplate.Board", new Proto.Str(spo, true));
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProductTemplate$$Filter", new Proto.Obj());
        om.bind("xdc.tools.product.IProductTemplate.Filter", new Proto.Str(spo, true));
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProductTemplate$$TemplateInfo", new Proto.Obj());
        om.bind("xdc.tools.product.IProductTemplate.TemplateInfo", new Proto.Str(spo, true));
        spo = (Proto.Obj)om.bind("xdc.tools.product.IProductTemplate$$TemplateGroup", new Proto.Obj());
        om.bind("xdc.tools.product.IProductTemplate.TemplateGroup", new Proto.Str(spo, true));
    }

    void IProductView$$CONSTS()
    {
        // interface IProductView
    }

    void IProduct$$CONSTS()
    {
        // interface IProduct
    }

    void IProductTemplate$$CONSTS()
    {
        // interface IProductTemplate
    }

    void IProductView$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IProduct$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IProductTemplate$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void IProductView$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn IProductView.getProductDescriptor
        fxn = (Proto.Fxn)om.bind("xdc.tools.product.IProductView$$getProductDescriptor", new Proto.Fxn(om.findStrict("xdc.tools.product.IProductView.Module", "xdc.tools.product"), (Proto)om.findStrict("xdc.tools.product.IProductView.ProductElemDesc", "xdc.tools.product"), 0, 0, false));
    }

    void IProduct$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IProductTemplate$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void IProductView$$SIZES()
    {
    }

    void IProduct$$SIZES()
    {
    }

    void IProductTemplate$$SIZES()
    {
    }

    void IProductView$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProductView.Module", "xdc.tools.product");
        po.init("xdc.tools.product.IProductView.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("homeModule", $$T_Str, $$UNDEF, "wh");
        po.addFld("linksToArray", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        // struct IProductView.ProductElemDesc
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProductView$$ProductElemDesc", "xdc.tools.product");
        po.init("xdc.tools.product.IProductView.ProductElemDesc", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("elemName", $$T_Str, $$UNDEF, "w");
                po.addFld("moduleName", $$T_Str, $$UNDEF, "w");
                po.addFld("iconFileName", $$T_Str, $$UNDEF, "w");
                po.addFld("elemArray", new Proto.Arr((Proto)om.findStrict("xdc.tools.product.IProductView.ProductElemDesc", "xdc.tools.product"), false), $$DEFAULT, "w");
    }

    void IProduct$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProduct.Module", "xdc.tools.product");
        po.init("xdc.tools.product.IProduct.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, $$UNDEF, "wh");
        po.addFld("id", $$T_Str, $$UNDEF, "wh");
        po.addFld("version", $$T_Str, $$UNDEF, "wh");
        po.addFld("featureId", $$T_Str, $$UNDEF, "wh");
        po.addFld("updateSite", (Proto)om.findStrict("xdc.tools.product.IProduct.UrlDescriptor", "xdc.tools.product"), $$DEFAULT, "wh");
        po.addFld("companyName", $$T_Str, $$UNDEF, "wh");
        po.addFld("productDescriptor", (Proto)om.findStrict("xdc.tools.product.IProduct.UrlDescriptor", "xdc.tools.product"), $$DEFAULT, "wh");
        po.addFld("licenseDescriptor", (Proto)om.findStrict("xdc.tools.product.IProduct.UrlDescriptor", "xdc.tools.product"), $$DEFAULT, "wh");
        po.addFld("copyRightNotice", $$T_Str, $$UNDEF, "wh");
        po.addFld("repositoryArr", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("docsLocArr", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("templateModule", $$T_Str, $$UNDEF, "wh");
        po.addFld("productViewModule", $$T_Str, $$UNDEF, "wh");
        po.addFld("bundleName", $$T_Str, $$UNDEF, "wh");
        po.addFld("targetFile", (Proto)om.findStrict("xdc.tools.product.IProduct.Target", "xdc.tools.product"), $$DEFAULT, "wh");
        po.addFld("platformSourceFile", $$T_Str, $$UNDEF, "wh");
        po.addFld("helpTocArr", new Proto.Arr((Proto)om.findStrict("xdc.tools.product.IProduct.HelpToc", "xdc.tools.product"), false), $$DEFAULT, "wh");
        po.addFld("tocIndexFile", $$T_Str, $$UNDEF, "wh");
        po.addFld("icon", $$T_Str, $$UNDEF, "wh");
        po.addFld("exclusive", $$T_Bool, false, "wh");
        po.addFld("otherFiles", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("macro", (Proto)om.findStrict("xdc.tools.product.IProduct.MacroDescriptor", "xdc.tools.product"), $$DEFAULT, "wh");
        po.addFld("externalPlugins", new Proto.Arr((Proto)om.findStrict("xdc.tools.product.IProduct.PluginDescriptor", "xdc.tools.product"), false), $$DEFAULT, "wh");
        po.addFld("externalRequirements", new Proto.Arr($$T_Str, false), $$DEFAULT, "wh");
        po.addFld("projectSpecPath", $$T_Str, "resources", "wh");
        // struct IProduct.HelpToc
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProduct$$HelpToc", "xdc.tools.product");
        po.init("xdc.tools.product.IProduct.HelpToc", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("filePath", $$T_Str, $$UNDEF, "w");
                po.addFld("tocFile", $$T_Bool, $$UNDEF, "w");
        // struct IProduct.Target
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProduct$$Target", "xdc.tools.product");
        po.init("xdc.tools.product.IProduct.Target", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("sourceFile", $$T_Str, $$UNDEF, "w");
        // struct IProduct.UrlDescriptor
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProduct$$UrlDescriptor", "xdc.tools.product");
        po.init("xdc.tools.product.IProduct.UrlDescriptor", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("url", $$T_Str, $$UNDEF, "w");
                po.addFld("text", $$T_Str, $$UNDEF, "w");
                po.addFld("targetText", $$T_Str, $$UNDEF, "w");
        // struct IProduct.PluginDescriptor
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProduct$$PluginDescriptor", "xdc.tools.product");
        po.init("xdc.tools.product.IProduct.PluginDescriptor", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("id", $$T_Str, $$UNDEF, "w");
                po.addFld("version", $$T_Str, $$UNDEF, "w");
                po.addFld("path", $$T_Str, $$UNDEF, "w");
        // struct IProduct.MacroDescriptor
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProduct$$MacroDescriptor", "xdc.tools.product");
        po.init("xdc.tools.product.IProduct.MacroDescriptor", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("desc", $$T_Str, $$UNDEF, "w");
    }

    void IProductTemplate$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProductTemplate.Module", "xdc.tools.product");
        po.init("xdc.tools.product.IProductTemplate.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("templateArr", new Proto.Arr((Proto)om.findStrict("xdc.tools.product.IProductTemplate.TemplateInfo", "xdc.tools.product"), false), $$DEFAULT, "wh");
        po.addFld("templateGroupArr", new Proto.Arr((Proto)om.findStrict("xdc.tools.product.IProductTemplate.TemplateGroup", "xdc.tools.product"), false), Global.newArray(new Object[]{}), "wh");
        // struct IProductTemplate.FileDesc
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProductTemplate$$FileDesc", "xdc.tools.product");
        po.init("xdc.tools.product.IProductTemplate.FileDesc", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("path", $$T_Str, $$UNDEF, "w");
                po.addFld("excludeFromBuild", $$T_Bool, $$UNDEF, "w");
                po.addFld("openOnCreation", $$T_Bool, $$UNDEF, "w");
                po.addFld("copyIntoConfiguration", $$T_Bool, $$UNDEF, "w");
                po.addFld("action", $$T_Str, $$UNDEF, "w");
                po.addFld("targetDirectory", $$T_Str, $$UNDEF, "w");
        // struct IProductTemplate.Board
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProductTemplate$$Board", "xdc.tools.product");
        po.init("xdc.tools.product.IProductTemplate.Board", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("tiCloudName", $$T_Str, $$UNDEF, "w");
        // struct IProductTemplate.Filter
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProductTemplate$$Filter", "xdc.tools.product");
        po.init("xdc.tools.product.IProductTemplate.Filter", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("deviceFamily", $$T_Str, $$UNDEF, "w");
                po.addFld("deviceVariant", $$T_Str, $$UNDEF, "w");
                po.addFld("deviceId", $$T_Str, $$UNDEF, "w");
                po.addFld("endianness", $$T_Str, $$UNDEF, "w");
                po.addFld("toolChain", $$T_Str, $$UNDEF, "w");
                po.addFld("outputFormat", $$T_Str, $$UNDEF, "w");
        // struct IProductTemplate.TemplateInfo
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProductTemplate$$TemplateInfo", "xdc.tools.product");
        po.init("xdc.tools.product.IProductTemplate.TemplateInfo", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("title", $$T_Str, $$UNDEF, "w");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("fileList", new Proto.Arr((Proto)om.findStrict("xdc.tools.product.IProductTemplate.FileDesc", "xdc.tools.product"), false), $$DEFAULT, "w");
                po.addFld("description", $$T_Str, $$UNDEF, "w");
                po.addFld("target", $$T_Str, $$UNDEF, "w");
                po.addFld("platform", $$T_Str, $$UNDEF, "w");
                po.addFld("board", (Proto)om.findStrict("xdc.tools.product.IProductTemplate.Board", "xdc.tools.product"), $$DEFAULT, "w");
                po.addFld("buildProfile", $$T_Str, $$UNDEF, "w");
                po.addFld("linkerCommandFile", $$T_Str, $$UNDEF, "w");
                po.addFld("compilerBuildOptions", $$T_Str, $$UNDEF, "w");
                po.addFld("linkerBuildOptions", $$T_Str, $$UNDEF, "w");
                po.addFld("endianness", $$T_Str, $$UNDEF, "w");
                po.addFld("outputFormat", $$T_Str, $$UNDEF, "w");
                po.addFld("requiredProducts", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("xdcToolsVersion", $$T_Str, $$UNDEF, "w");
                po.addFld("groups", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("legacyTcf", $$T_Bool, $$UNDEF, "w");
                po.addFld("configuroOptions", $$T_Str, $$UNDEF, "w");
                po.addFld("configOnly", $$T_Bool, $$UNDEF, "w");
                po.addFld("isHybrid", $$T_Bool, $$UNDEF, "w");
                po.addFld("isFragment", $$T_Bool, $$UNDEF, "w");
                po.addFld("filterArr", new Proto.Arr((Proto)om.findStrict("xdc.tools.product.IProductTemplate.Filter", "xdc.tools.product"), false), $$DEFAULT, "w");
                po.addFld("ignoreDefaults", $$T_Bool, $$UNDEF, "w");
                po.addFld("options", $$T_Str, $$UNDEF, "w");
                po.addFld("references", $$T_Str, $$UNDEF, "w");
                po.addFld("buildCommandFlags", $$T_Str, $$UNDEF, "w");
                po.addFld("launchWizard", $$T_Bool, $$UNDEF, "w");
                po.addFld("preBuildStep", $$T_Str, $$UNDEF, "w");
                po.addFld("postBuildStep", $$T_Str, $$UNDEF, "w");
                po.addFld("projectType", $$T_Str, $$UNDEF, "w");
                po.addFld("outputType", $$T_Str, $$UNDEF, "w");
        // struct IProductTemplate.TemplateGroup
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProductTemplate$$TemplateGroup", "xdc.tools.product");
        po.init("xdc.tools.product.IProductTemplate.TemplateGroup", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("id", $$T_Str, $$UNDEF, "w");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("description", $$T_Str, $$UNDEF, "w");
                po.addFld("groups", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
    }

    void IProductView$$ROV()
    {
    }

    void IProduct$$ROV()
    {
    }

    void IProductTemplate$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("xdc.tools.product.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "xdc.tools.product"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "xdc.tools.product", Value.DEFAULT, false);
        pkgV.bind("$name", "xdc.tools.product");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "xdc.tools.product.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['xdc.tools.product'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('xdc.tools.product$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['xdc.tools.product$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['xdc.tools.product$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void IProductView$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("xdc.tools.product.IProductView", "xdc.tools.product");
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProductView.Module", "xdc.tools.product");
        vo.init2(po, "xdc.tools.product.IProductView", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("xdc.tools.product", "xdc.tools.product"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("ProductElemDesc", om.findStrict("xdc.tools.product.IProductView.ProductElemDesc", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProductView.ProductElemDesc", "xdc.tools.product"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IProductView", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IProductView");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IProduct$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("xdc.tools.product.IProduct", "xdc.tools.product");
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProduct.Module", "xdc.tools.product");
        vo.init2(po, "xdc.tools.product.IProduct", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("xdc.tools.product", "xdc.tools.product"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("HelpToc", om.findStrict("xdc.tools.product.IProduct.HelpToc", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProduct.HelpToc", "xdc.tools.product"));
        vo.bind("Target", om.findStrict("xdc.tools.product.IProduct.Target", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProduct.Target", "xdc.tools.product"));
        vo.bind("UrlDescriptor", om.findStrict("xdc.tools.product.IProduct.UrlDescriptor", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProduct.UrlDescriptor", "xdc.tools.product"));
        vo.bind("PluginDescriptor", om.findStrict("xdc.tools.product.IProduct.PluginDescriptor", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProduct.PluginDescriptor", "xdc.tools.product"));
        vo.bind("MacroDescriptor", om.findStrict("xdc.tools.product.IProduct.MacroDescriptor", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProduct.MacroDescriptor", "xdc.tools.product"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IProduct", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IProduct");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void IProductTemplate$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("xdc.tools.product.IProductTemplate", "xdc.tools.product");
        po = (Proto.Obj)om.findStrict("xdc.tools.product.IProductTemplate.Module", "xdc.tools.product");
        vo.init2(po, "xdc.tools.product.IProductTemplate", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("xdc.tools.product", "xdc.tools.product"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("FileDesc", om.findStrict("xdc.tools.product.IProductTemplate.FileDesc", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProductTemplate.FileDesc", "xdc.tools.product"));
        vo.bind("Board", om.findStrict("xdc.tools.product.IProductTemplate.Board", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProductTemplate.Board", "xdc.tools.product"));
        vo.bind("Filter", om.findStrict("xdc.tools.product.IProductTemplate.Filter", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProductTemplate.Filter", "xdc.tools.product"));
        vo.bind("TemplateInfo", om.findStrict("xdc.tools.product.IProductTemplate.TemplateInfo", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProductTemplate.TemplateInfo", "xdc.tools.product"));
        vo.bind("TemplateGroup", om.findStrict("xdc.tools.product.IProductTemplate.TemplateGroup", "xdc.tools.product"));
        tdefs.add(om.findStrict("xdc.tools.product.IProductTemplate.TemplateGroup", "xdc.tools.product"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("IProductTemplate", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IProductTemplate");
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
        ((Value.Obj)om.getv("xdc.tools.product.IProductView")).bless();
        ((Value.Obj)om.getv("xdc.tools.product.IProduct")).bless();
        ((Value.Obj)om.getv("xdc.tools.product.IProductTemplate")).bless();
        ((Value.Arr)om.findStrict("$packages", "xdc.tools.product")).add(pkgV);
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
        IProductView$$OBJECTS();
        IProduct$$OBJECTS();
        IProductTemplate$$OBJECTS();
        IProductView$$CONSTS();
        IProduct$$CONSTS();
        IProductTemplate$$CONSTS();
        IProductView$$CREATES();
        IProduct$$CREATES();
        IProductTemplate$$CREATES();
        IProductView$$FUNCTIONS();
        IProduct$$FUNCTIONS();
        IProductTemplate$$FUNCTIONS();
        IProductView$$SIZES();
        IProduct$$SIZES();
        IProductTemplate$$SIZES();
        IProductView$$TYPES();
        IProduct$$TYPES();
        IProductTemplate$$TYPES();
        if (isROV) {
            IProductView$$ROV();
            IProduct$$ROV();
            IProductTemplate$$ROV();
        }//isROV
        $$SINGLETONS();
        IProductView$$SINGLETONS();
        IProduct$$SINGLETONS();
        IProductTemplate$$SINGLETONS();
        $$INITIALIZATION();
    }
}
