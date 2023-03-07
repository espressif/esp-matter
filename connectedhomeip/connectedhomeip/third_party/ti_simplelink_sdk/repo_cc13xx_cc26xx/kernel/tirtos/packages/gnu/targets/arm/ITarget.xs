/*
 *  Copyright 2021 by Texas Instruments Incorporated.
 *
 */

/*
 * Copyright (c) 2017-2019 Texas Instruments Incorporated - http://www.ti.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

var debug;
if (debug == null) {
    debug = function (msg) {};
}

var _bldUtils = xdc.useModule('xdc.bld.Utils');
var _utils = xdc.loadCapsule("gnu/targets/arm/_utils.xs");

/*
 *  ======== _targetEnvs ========
 *  Environment variable settings required for the compile/link/archive
 *  commands.
 */
var _targetEnvs = "LD_LIBRARY_PATH=";

/*
 *  ======== ITarget.archive ========
 */
function archive(goal)
{
    var target = this;
    var result = null;
    var tool2cmd = _utils._mkCmds(target);

    if (tool2cmd != null) {
        result = new xdc.om['xdc.bld'].ITarget.CommandSet;
        result.msg = "archiving " + goal.files + " into $@ ...";
        result.envs = _targetEnvs.split("\n");
        result.cmds = _bldUtils.expandString(tool2cmd["ar"], {
            AROPTS_P:   target.arOpts.prefix,
            AROPTS_S:   target.arOpts.suffix,
            GCCVERS:    target.GCCVERS,
            GCCTARG:    target.GCCTARG,
            LONGNAME:   target.LONGNAME,
            aropts:     goal.opts,
            files:      goal.files
        });
    }

    return (result);
}

/*
 *  ======== ITarget.asmName ========
 */
function asmName(cname)
{
    return (cname);
}

/*
 *  ======== ITarget.compile ========
 */
function compile(goal) {
    return (_compile(this, goal, false));
}

/*
 *  ======== ITarget.link ========
 */
function link(goal)
{
    var target = this;
    var result = null;
    var tool2cmd = _utils._mkCmds(target);
    var fileList = goal.files;

    if (target.BINVERS >= "2.19") {
        fileList = goal.files.replace(/(^|\s+)(\S+\.xdl)($|\s+)/,
            " -Wl,-T,$2 ");
    }

    var lib = "";
    /*
     * bspLib is a config param that can be added to the target. If it is
     * present, it is expected to contain the name of the BSP library.
     * The specified library is included on the link line.
     *
     * Acceptable values for this field are "nosys" and "rdimon".
     */
    if (("bspLib" in target) && target.bspLib != null) {
        lib = " --specs=" + target.bspLib + ".specs";
    }

    if (tool2cmd != null) {
        result = new xdc.om['xdc.bld'].ITarget.CommandSet;
        result.msg = "lnk" + target.suffix + " $@ ...";
        result.envs = _targetEnvs.split("\n");
        result.cmds = _bldUtils.expandString(tool2cmd["link"], {
            LOPTS_P:    target.lnkOpts == null ? "" : target.lnkOpts.prefix,
            LOPTS_S:    target.lnkOpts == null ? "" : (target.lnkOpts.suffix + lib),
            GCCVERS:    target.GCCVERS,
            GCCTARG:    target.GCCTARG,
            LONGNAME:   target.LONGNAME,
            lopts:      goal.opts,
            files:      fileList
        });
    }

    return (result);
}

/*
 *  ======== ITarget.genVisibleData ========
 *
 *  This function is invoked at the config time, therefore we can check the
 *  build profile.
 */
function genVisibleData(quals, types, names)
{
    if (xdc.om.$name != 'cfg') {
        return (null);
    }

    var sb = new java.lang.StringBuilder();
    for (var i = 0; i < names.length; i++) {
        sb.append(quals[i] + " " + types[i] + " " + names[i]
            + " __attribute__ ((externally_visible));\n");
    }
    return (sb.toString() + "");
}

/*
 *  ======== genVisibleFxns ========
 */
function genVisibleFxns(types, names, args)
{
    if (xdc.om.$name != 'cfg') {
        return (null);
    }

    var sb = new java.lang.StringBuilder();
    for (var i = 0; i < names.length; i++) {
        sb.append(types[i] + " " + names[i] + "(" + args[i] + ")"
            + " __attribute__ ((externally_visible));\n");
    }
    return (sb.toString() + "");
}

/*
 *  ======== ITarget.getVersion ========
 */
function getVersion()
{
    var target = this;

    _utils._mkCmds(target);

    var result = target.$name + "{";
    var key;

    if ((key = target.versionMap["gcc" + target.GCCVERS]) == null) {
        /* map 2.3.4 to "1,0,2.3,4", 2.3 to "1,0,2.3", and 2 to "1,0,2" */
        key = "1,0";
        var va = target.GCCVERS.split('.');
        for (var i = 0; i < va.length; i++) {
            key += (i == 1 ? '.' : ',') + va[i];
        }
    }
    result += key;

    return (result);
}

/*
 *  ======== ITarget.getRawVersion ========
 */
function getRawVersion()
{
    var target = this;
    _utils._mkCmds(target);;
    return (target.GCCVERS);
}


/*
 *  ======== ITarget.scompile ========
 */
function scompile(goal) {
    return (_compile(this, goal, true));
}

/*
 *  ======== _compile ========
 */
function _compile(target, goal, asm)
{
    var result = null;
    var tool2cmd = _utils._mkCmds(target);    /* get tool to template map */

    if (tool2cmd != null) {
        var ext = target.extensions[goal.srcSuffix];
        if (ext != null) {
            if (asm == true && ext.typ == "asm") {
                return (null);
            }

            result = new xdc.om['xdc.bld'].ITarget.CommandSet;
            result.msg = (ext.typ == "c" ? "cl" : ext.typ)
                            + target.suffix
                            + (asm == true ? " -S $< ..." : " $< ...");

            var dstDir = goal.dstPrefix + goal.base;
            dstDir = dstDir.replace(/[^\/\\]*$/, "");

            var ccoptsPre = target.ccOpts.prefix;
            var ccoptsSuf = target.ccOpts.suffix;
            /* If the target defines ccConfigOpts, they should be used
             * instead of ccOpts. The string expansion has to be done here,
             * in the case ccConfigOpts contains COPTS. If the expansion was
             * left for the end of the function, there would be an infinite
             * recursion.
             */
            if ("ccConfigOpts" in target && goal.configOpts == true) {
                ccoptsPre = _bldUtils.expandString(target.ccConfigOpts.prefix,
                    {"ccOpts.prefix": target.ccOpts.prefix});
                ccoptsSuf = _bldUtils.expandString(target.ccConfigOpts.suffix,
                    {"ccOpts.suffix": target.ccOpts.suffix});
            }

            result.envs = _targetEnvs.split("\n");

            result.cmds = _bldUtils.expandString(tool2cmd[ext.typ], {
                COPTS_P:    ccoptsPre,
                COPTS_S:    ccoptsSuf,
                AOPTS_P:    target.asmOpts.prefix,
                AOPTS_S:    target.asmOpts.suffix,
                GCCVERS:    target.GCCVERS,
                GCCTARG:    target.GCCTARG,
                LONGNAME:   target.LONGNAME,
                ASMONLY:    asm ? "-S" : "",
                dstDir:     dstDir,
                srcExt:     goal.srcSuffix,
                copts:      goal.configOpts ? goal.opts.cfgcopts :
                                              goal.opts.copts,
                aopts:      goal.opts.aopts,
                defs:       goal.opts.defs,
                incs:       goal.opts.incs
            });
        }
    }

    return (result);
}

/*
 *  ======== _init ========
 *  Initialize this capsule at package init time.
 */
function _init(pkg)
{
    _thisPkg = pkg;
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /*  Expand 'packageBase' here because Utils.expandString() works
     *  only on properties of the module, not properties of the package.
     */
    var pkgBase = this.$package.packageBase;
    this.includeOpts = this.includeOpts.replace(/\$\(packageBase\)/g, pkgBase);
    this.lnkOpts.suffix =
            this.lnkOpts.suffix.replace(/\$\(packageBase\)/g, pkgBase);
}

/*
 *  ======== initVers ========
 *  Initialize target.GCCVERS, target.GCCTARG and target.BINVERS, if they are
 *  not already initialized by a previous call to initVers() or by the user
 *  via the config.bld script.
 */
function initVers()
{
    function execExists(path)
    {
        if (new java.io.File(path).exists() ||
            new java.io.File(path + ".exe").exists()) {
            return (true);
        }
        return (false);
    }

    var target = this;
    if (target.$private.vers == true || target.rootDir == null) {
        return;
    }

    var gccPath = target.rootDir + "/bin/" + target.GCCTARG + "-gcc";
    var gccMkString = "$(rootDir)/bin/$(GCCTARG)-gcc";

    /* If LONGNAME is specified, use it. Should we have a warning? */
    if (target.GCCTARG == null && target.LONGNAME != null) {
        /* The expression in parentheses is non-greedy to allow an optional '-'
         * character to be consumed by the matching '-' outside of the
         * parentheses. The only time there is no '-' character is when we are
         * dealing with the native compiler, and the GCC target string is empty.
         */
        var gccPrefix = (target.LONGNAME.match(/bin\/(.*?)-*?gcc/)[1]);
        if (gccPrefix == "") {
            gccPath = target.rootDir + "/bin/gcc";
            gccMkString = "$(rootDir)/bin/gcc";
        }
        else {
            /* adjusting for the new prefix */
            gccPath = target.rootDir + "/bin/" + gccPrefix + "-gcc";
            gccMkString = "$(rootDir)/bin/" + gccPrefix + "-gcc";
        }
        if (!execExists(gccPath)) {
            throw new Error("A deprecated parameter LONGNAME is set, but the "
                + "executable '" + gccPath + "' does not exist. Try commenting "
                + "out LONGNAME or setting it to null.");
        }
    }

    /* If GCCTARG is set, but the executable 'gccPath' does not exist, we don't
     * exit because there are compiler whose 'gcc' executable is not named
     * $GCCTARG-gcc. We look for the 'gcc' executable, and leave GCCTARG as it
     * is. If it's NULL, it will be set later, if it's not we are not going to
     * change the user's setting.
     */
    else if (target.GCCTARG == null || !execExists(gccPath)) {
        var fileDir = new java.io.File(target.rootDir + "/bin");
        if (!fileDir.exists()) {
            throw new Error("Directory '" + target.rootDir + "/bin' cannot be "
                + "found. Ensure that rootDir for the " + target.name
                + " target is set correctly in '"
                + java.lang.System.getenv("XDCBUILDCFG") + "'.");
        }
        var fileArray = fileDir.listFiles();
        for (var i = 0; i < fileArray.length; i++) {
            var file = fileArray[i];
            if (!file.isDirectory()
                && (file.getName() + "").match(/.+gcc(.exe)?$/)) {
                gccPath = target.rootDir + "/bin/" + file.getName();
                gccMkString = "$(rootDir)/bin/" + file.getName();
                break;
             }
        }
        if (i == fileArray.length) {
            throw new Error("Can't find a gcc executable in '"
                + fileDir.getAbsolutePath() + "'.");
        }
    }

    var currCmd = target.cc.cmd.match(/^(\S+).*/);
    if (currCmd != gccMkString) {
        target.cc.$unseal("cmd");
        target.asm.$unseal("cmd");
        target.lnk.$unseal("cmd");
        target.ar.$unseal("cmd");
        /* The assumption is that there are no white spaces in rootDir, which is
         * something we generaly recommend because of make files.
         */
        target.cc.cmd = target.cc.cmd.replace(/^\S+/, gccMkString);
        target.asm.cmd = target.asm.cmd.replace(/^\S+/, gccMkString);
        target.lnk.cmd = target.lnk.cmd.replace(/^\S+/, gccMkString);
        target.ar.cmd = target.ar.cmd.replace(/^\S+/,
            gccMkString.replace(/gcc$/, "ar"));
        target.cc.$seal("cmd");
        target.asm.$seal("cmd");
        target.lnk.$seal("cmd");
        target.ar.$seal("cmd");
    }

    var prefix = "";
    if (target.remoteHost != null) {
        prefix = target.$package.packageBase + "/runh -h "
            + target.remoteHost + " ";
    }
    else if ("cmdPrefix" in target.$orig
             && target.$orig.cmdPrefix != undefined) {
        prefix = target.$orig.cmdPrefix;
    }

//    print("getting version for target " + target.$name);


    function askLd(target)
    {
        var ldCmd = gccPath.replace(/gcc$/, "ld");

        /* If the ld executable cannot be found, and the prefix is not used,
         * it's probably a misspelled directory path.
         */
        if (prefix == "" && !execExists(ldCmd)) {
            throw new Error("'" + ldCmd + "' cannot be found.");
        }
        var cmd = prefix + ldCmd + " -v";
        var status = {};
        if (xdc.exec(cmd, null, status) >= 0) {
            /* codesourcery loaders since 2010-q3 print an internal version
             * number inside parenthesis. We don't want to pick up that one.
             */
            var va = status.output.match(/(?:\(.*\))?\s(\d\.\d+)/);
            if (va != null) {
                return (va[1]);
            }
            else {
                print(target.$name + ": warning: failed to parse '-v' output "
                    + "(= '" + status.output + "'); check compiler options");
            }
        }
        else {
            print(target.$name + ": warning: exec of '" + cmd + "' failed:"
                + status.output + "; check tools installation");
        }

        return ("");
    }

    function askGcc(target, opt, filter)
    {
        var cmd = prefix + gccPath + " " + opt;
        var status = {};

        var attrs = null;

        /* If remote execution is run through a script, that script may need
         * PATH or other environment variables.
         */
        if  ("cmdPrefix" in target.$orig
            && target.$orig.cmdPrefix != undefined) {
            attrs = {useEnv: true};
        }

        if (xdc.exec(cmd, attrs, status) >= 0) {
            var va = status.output.match(filter);
            if (va != null) {
                return (va[0]);
            }
            else {
                print(target.$name + ": warning: failed to parse '"
                    + opt + "' output (= '" + status.output + "')"+
           "; check compiler options");
            }
        }
        else {
            print(target.$name + ": warning: exec of '" + cmd + "' failed:"
                + status.output + "; check tools installation");
        }

        return ("");
    }

//      print("setting " + target.$name + ".GCCVERS and .GCCTARG to ''");

    /* BUG: we should compare against target's tool-chain os */
    if (target.GCCTARG != null && target.GCCVERS != null
        && target.BINVERS != null) {
        return;
    }

    target.$unseal("GCCVERS");
    target.$unseal("GCCTARG");
    target.$unseal("BINVERS");

    /* call compiler to get its version number and set GCCVERS */
    var result;
    if (target.GCCTARG == null) {
        if ((result = askGcc(target, "-dumpmachine", /^[\w-\.]+/)) == "") {
            print(target.$name + ": warning: can't determine gcc target; ");
            print("Check if " + gccPath + " can run on ");
            if (target.remoteHost != null) {
                print(target.remoteHost + ". ");
            }
            else {
                print(xdc.om['xdc.bld'].BuildEnvironment.hostOSName + ". ");
            }
            print("Also, try explicitly setting the GCCTARG configuration "
                + "parameter.");
        }
//      print("setting " + target.$name + ".GCCTARG to " + result);
        target.GCCTARG = result;
    }

    if (target.GCCVERS == null) {
        if ((result = askGcc(target, "-dumpversion", /^[\d\.]+/)) == "") {
            print(target.$name + ": warning: can't determine gcc version; ");
            print("Check if " + gccPath + " can run on ");
            if (target.remoteHost != null) {
                print(target.remoteHost + ". ");
            }
            else {
                print(xdc.om['xdc.bld'].BuildEnvironment.hostOSName + ". ");
            }
            print("Also, try explicitly setting the GCCVERS configuration "
                + "parameter.");
        }
//      print("setting " + target.$name + ".GCCVERS to " + result);
        target.GCCVERS = result;
    }

    if (target.BINVERS == null) {
        if ((result = askLd(target)) == "") {
            print(target.$name + ": warning: can't determine binutils version; ");
            print("Check if " + gccPath.replace(/gcc$/, "ld") + " can run on ");
            if (target.remoteHost != null) {
                print(target.remoteHost + ". ");
            }
            else {
                print(xdc.om['xdc.bld'].BuildEnvironment.hostOSName + ". ");
            }
            print("Also, try explicitly setting the BINVERS configuration "
                + "parameter.");
        }
        target.BINVERS = result;
    }

    target.$seal("GCCVERS");
    target.$seal("GCCTARG");
    target.$seal("BINVERS");

    target.$private.vers = true;

    /*
     * Special case for a customer that wants to use
     * gcc-arm-none-eabi-4_7_2012q4.
     * We need to get the target's custom library off the compile and link lines.
     * We also remove the 'nano.specs' file so the these old complier users get
     * the traditional newlib library (as they were using previously).
     */
    if (target.name == "A15F" && target.GCCVERS.match(/^4.7/)) {
        target.includeOpts =
            target.includeOpts.replace(/\S+install-native\S+/g, "");
        target.lnkOpts.suffix =
            target.lnkOpts.suffix.replace(/\S+install-native\S+/g, "");
        target.lnkOpts.suffix =
            target.lnkOpts.suffix.replace(/--specs=nano.specs/, "");
    }
}
/*
 *  @(#) gnu.targets.arm; 1, 0, 0,; 7-28-2021 06:57:31; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

