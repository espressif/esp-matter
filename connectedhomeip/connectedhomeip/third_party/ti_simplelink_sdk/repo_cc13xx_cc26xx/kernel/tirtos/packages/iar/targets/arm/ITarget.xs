/* 
 *  Copyright (c) 2013-2020 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * */
var _bldUtils = xdc.useModule('xdc.bld.Utils');

/*
 *  ======== _targetCmds ========
 *  Hash table of commands (indexed by target.suffix).  Commands are
 *  themselves hashtables (indexed by file type: "asm", "c", ...) that
 *  define the commands for cc, asm , etc.
 */
var _targetCmds = null;

/*
 *  ======== _thisPkg ========
 *  This variable is initialized by this package's init method to the
 *  package containing this file.  We use this in lieu of hard coding
 *  this package's name in this file.
 */
var _thisPkg = null;

/*
 *  ======== _versionCache ========
 *  A cache of version results to avoid calling the same compiler more
 *  than once to get a version string.
 */
var _versionCache = {};

/*
 *  ======== _isaChainMap ========
 *  This map defines an "is a " releationship between ISAs (Instruction
 *  Set Architectures).  
 */
var _isaChainMap = {
    isa_v6M  : ["v6M"],
    isa_v7M4 : ["v7M", "v7M4"],
    isa_v7M  : ["v7M"],
    isa_v8M  : ["v8M", "v7M"]
};

/*
 *  ======== ITarget.archive ========
 */
function archive(goal)
{
    var target = this;

    _initTabs();

    var result = null;
    var tool2cmd = _targetCmds[target.suffix];

    if (tool2cmd != null) {
        result = new xdc.om['xdc.bld.ITarget'].CommandSet;
        result.msg = "archiving " + goal.files + " into $@ ...";

        result.cmds = _bldUtils.expandString(tool2cmd["ar"], {
            AROPTS_P:   target.arOpts.prefix,
            AROPTS_S:   target.arOpts.suffix,
            aropts:     goal.opts,
            files:      goal.files
        });
    }

    return (result);
}

/*
 *  ======== ITarget.compile ========
 */
function compile(goal) {
    return (_compile(this, goal, false));
}

/*
 *  ======== ITarget.genConstCustom ========
 */
function genConstCustom(names, types)
{
    if (xdc.om.$name != 'cfg') {
        return (null);
    }

    var sb = new java.lang.StringBuilder();
    for (var i = 0; i < names.length; i++) {
        sb.append('#pragma location = \".const_' + names[i] + '\"\n');
    }
    return (sb.toString() + "");
}

/*
 *  ======== ITarget.getVersion ========
 */
function getVersion()
{
    var target = this;

    var raw = target.getRawVersion();

    /* parse compiler's raw version to create a compatibility key */
    var va = raw.match(/(\d+\.\d+)\.(\d+)/m);
    if (va == null || va.length < 3) {
        print(target.$name + ": warning: version match failed on '"
            + raw + "'");
        va = "0.0,0";
    }

    return (target.$name + "{1,0," + va[1] + "," + va[2]);
}

/*
 *  ======== ITarget.getRawVersion ========
 */
function getRawVersion()
{
    var target = this;
    _initTabs();

    var result = "";

    /* call compiler to get its version number and append to result */
    var tool2cmd = _targetCmds[target.suffix];
    if (tool2cmd != null) {
        var cmd = tool2cmd["vers"].replace("\$(rootDir)", target.rootDir);

        /* Check if rootDir is set correctly */
        var file = new java.io.File(target.rootDir);
        if (target.cmdPrefix == "" && !file.exists()) {
            throw new Error(target.rootDir + " cannot be found. Ensure that" +
                " rootDir for the " + target.name + " target is set " +
                "correctly in " + java.lang.System.getenv("XDCBUILDCFG"));
        }

        /* if we already know the result, return it now */
        var ver;
        if ((ver = _versionCache[cmd]) != null) {
            return (ver);
        }

        var status = {};
        /* LC_ALL=C env var has to be set for code gen to run on SUSE */
        if (xdc.exec(cmd, {envs: ["LC_ALL=C"], useEnv: true}, status) >= 0) {
            var tmp = status.output.match(/V((?:(\d+\.)*)\d+)/m);
            if (tmp != null) {
                result = tmp[0].substring(1);
            }
            else {
                print(target.$name + ": warning: exec of '" + cmd 
                    + "' returned an unexpected result: "
                    + status.output + "; check tools installation");
            }
        }
        else {
            print(target.$name + ": warning: exec of '" + cmd + "' failed:"
                + status.output + "; check tools installation");
        }

        /* cache the result so we don't have to run this command again */
        _versionCache[cmd] = result;        
    }

    return (result);
}
/*
 *  ======== ITarget.link ========
 */
function link(goal)
{
    var target = this;
    _initTabs();

    var result = null;
    var tool2cmd = _targetCmds[target.suffix];

    if (tool2cmd != null) {
        result = new xdc.om['xdc.bld.ITarget'].CommandSet;
        result.msg = "lnk" + target.suffix + " $@ ...";

        var cmd = tool2cmd["link"];
        goal.files = goal.files.replace(/(^|\s+)([\/\w\\\.]+\.xdl)($|\s+)/,
            " -f $2 ");

        /* if an 'xdl' file is in 'goal.files', we need to add '@' as its
         * prefix
         */
        result.cmds = _bldUtils.expandString(cmd, {
            LOPTS_P: target.lnkOpts == null ? "" : target.lnkOpts.prefix,
            LOPTS_S: target.lnkOpts == null ? "" : target.lnkOpts.suffix,
            lopts:   goal.opts,
            files:   goal.files,
        });

    }

    return (result);
}

/*
 *  ======== ITarget.scompile ========
 */
function scompile(goal) {
    return (_compile(this, goal, true));
}

/*
 *  ======== ITarget.getISAChain ========
 */
function getISAChain(isa) {
    var key = "isa_" + (isa == null ? this.isa : isa);
    var result = _isaChainMap[key];
    return (result == null ? [] : result);
}

/*
 *  ======== _langOpts ========
 */
var _langOpts = {asm: "", c: "", cpp: "--c++ --no_exceptions --no_rtti --guard_calls"};

/*
 *  ======== _compile ========
 */
function _compile(target, goal, asm)
{
    var result = null;

    _initTabs();

    var tool2cmd = _targetCmds[target.suffix];  /* get tool to template map */

    if (tool2cmd != null) {
        var ext = target.extensions[goal.srcSuffix];
        if (ext != null) {
            var ea = ext.typ.split(':');
            var cmdType = ea[0];
            var langOpt = _langOpts[cmdType];
            if (ea[1] != null) {
                langOpt = ea[1];
            }

            if (asm == true && cmdType == "asm") {
                return (null);  /* nothing to do to convert to asm code */
            }
            var result = new xdc.om['xdc.bld.ITarget'].CommandSet;
            result.msg = (cmdType == "c" ? "cl" : cmdType)
                + target.suffix + " $< ...";

            var dstDir = goal.dstPrefix + goal.base;
            dstDir = dstDir.replace(/[^\/\\]*$/, "");

            result.cmds = _bldUtils.expandString(tool2cmd[cmdType], {
                COPTS_P:    target.ccOpts.prefix,
                COPTS_S:    target.ccOpts.suffix,
                AOPTS_P:    target.asmOpts.prefix,
                AOPTS_S:    target.asmOpts.suffix,
                ASMONLY:    "",
                dstDir:     dstDir,
                srcExt:     goal.srcSuffix,
                copts:      goal.opts.copts,
                aopts:      goal.opts.aopts,
                defs:       goal.opts.defs,
                incs:       goal.opts.incs,
                langOpt:    langOpt,
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
 *  ======== _initTabs ========
 *  Initialize command tables *after* config.bld to allow config.bld to
 *  adjust the commands.
 */
function _initTabs()
{
    if (_targetCmds == null) {
        _targetCmds = {};

        var pa = _thisPkg.$modules;
        for (var j = 0; j < pa.length; j++) {
            var ma = pa[j];
            _mkCmds(ma);
        }
    }
}

/*
 *  ======== _mkCmds ========
 */
function _mkCmds(target)
{
    var cmdOpts, cmd;
    var tool2cmd = {};

    var prefix = target.cmdPrefix + target.binDir;

    /* define assembly options */
    cmdOpts = " $(AOPTS_P) " + target.asm.opts + " $(AOPTS_S) $(defs) "
        + "$(aopts) $(incs) $(XDCINCS) " + target.includeOpts;

    /* define assemble command template */
    cmd  = prefix + target.asm.cmd + cmdOpts + " -o $@ $(langOpt) $<\n";
    tool2cmd["asm"] = cmd;

    cmdOpts = " $(ASMONLY) $(COPTS_P) " + target.cc.opts + " $(COPTS_S) "
        + "$(defs) $(copts) $(incs) $(XDCINCS) " + target.includeOpts
        + " -o $@";

    /* define the C compile command template */
    cmd =  prefix + target.cc.cmd + cmdOpts + " $(langOpt) $<\n";
    tool2cmd["c"] = cmd;

    /* define the C++ compile command template */
    tool2cmd["cpp"] = cmd;  /* only diff between C and C++ is $(langOpt) */

    /* define the link command template */
    cmd = "$(RM) $(XDCCFGDIR)/$@.map\n" + prefix + target.lnk.cmd
        + " $(LOPTS_P) -o $@ $(files) $(lopts) " + target.lnk.opts
        + " $(LOPTS_S)\n";
    tool2cmd["link"] = cmd;

    /* define the ar command template */
    cmd = prefix + target.ar.cmd + " $(AROPTS_P) " + target.ar.opts
        + "-o $@ $(AROPTS_S) $(aropts) $(files)";
    tool2cmd["ar"] = cmd;

    /*
     *  define the vers command template 
     *  vers cmd is run by calling  xdc.exec, so setting an environment var  
     *  by adding a env variable as prefix (unix style) will not work. Instead,
     *  LC_ALL=C is added to args list when calling xdc.exec.
     */
    cmd = "";
    if (!target.cmdPrefix.match(/^LC_ALL/)) {
        cmd = target.cmdPrefix;
    }
    cmd += target.binDir + target.vers.cmd + " " + target.vers.opts;
    tool2cmd["vers"] = cmd;

    /* add set of command templates to _targetCmds */
    _targetCmds[target.suffix] = tool2cmd;
}
/*
 *  @(#) iar.targets.arm; 1, 0, 0,; 7-28-2021 06:57:36; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

