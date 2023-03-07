/*
 * Copyright (c) 2008-2020 Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== ITarget.xs ========
 *
 *! Revision History
 *! ================
 *! 21-Feb-2018 ashish  add TI LLVM support
 *! 01-Sep-2011 sg      added v7M4
 *! 26-Jul-2010 kw      added "--diag_suppress=23000" to avoid spurious type
 *!                     errors starting with 7.1 codegen (workaround from Anbu)
 *! 07-May-2008 sasha   rearrangement of whole_program profiles (21494, 25333)
 *! 08-Apr-2008 sasha   removed workaround for bugzilla 795 (old 54 tools)
 *! 13-Jun-2007 kw      use -op2 for Arm
 *! 14-Nov-2006 sasha   replaced -mv5e with -mv4 for TMS470 targets
 *! 01-Nov-2006 sasha   added "-o3" back to alchemy options for arm targets
 *! 04-Oct-2005 sasha   added workaround for bugzilla 795
 *! 18-Aug-2005 sasha   If an alchemy profile has linkOpts defined, then
 *!                     nothing is added to the link command line in link()
 *! 16-Aug-2005 sasha   Removed the check for the alchemy target from link()
 *!
 */

var debug;
if (debug == null) {
    debug = function (msg) {};
}

var _bldUtils = xdc.module('xdc.bld.Utils');

/*
 *  ======== _isaChainMap ========
 *  This map defines an "is a " releationship between ISAs (Instruction
 *  Set Architectures).  This table controls both the names of generated
 *  assembly language files (as part of the configuration model) and the
 *  searching of assembly language sources files.
 *
 *  Each chain is an array of other ISAs that are "source-code compatible"
 *  with the specified ISA.  The 64P ISA chain below is defined to as
 *  ["62", "64", "64P"] which indicates that .s62, .s64, and .s64P assembly
 *  language files may be recompiled to run on a 64P device.
 */
var _isaChainMap = {
    isa_62:  ["62"],
    isa_64:  ["62", "64"],
    isa_64P: ["62", "64", "64P"],
    isa_674: ["62", "64", "64P", "674"],
    isa_67:  ["62", "67"],
    isa_67P: ["62", "67", "67P"],
    isa_64T: ["64T"],
    isa_66:  ["62", "64", "64P", "674", "66"],
    isa_71:  ["71"],
    isa_28:  ["28"],
    isa_28FP:["28", "28FP"],
    isa_28FP64:["28", "28FP64"],
    isa_430: ["430"],
    isa_430X:["430", "430X"],
    isa_470: ["470", "v4T"],
    isa_v4T: ["470", "v4T"],
    isa_v5T: ["470", "v4T", "v5T"],
    isa_v6:  ["470", "v4T", "v5T", "v6"],
    isa_v7A: ["470", "v4T", "v5T", "v6", "v7A"],
    isa_v6M: ["v6M"],
    isa_v7M: ["v7M"],
    isa_v7M4:["v7M", "v7M4"],
    isa_v7R: ["470", "v4T", "v5T", "v6", "v7R"],
    isa_v8M: ["v8M", "v7M"],
    isa_arp32: ["arp32"]
};

/*
 *  ======== _targetCmds ========
 *  Hash table of commands (indexed by target.suffix).  Commands are themselves
 *  hashtables (indexed by file type: "asm", "c", ...) that define the commands
 *  for cc, asm , etc.
 */
var _targetCmds = null;

/*
 *  ======== _targetEnvs ========
 *  Hash table of path settings for each target (indexed by target.suffix).
 */
var _targetEnvs = {};

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
 *  ======== _rawVersionCache ========
 *  A cache of version results to avoid calling the same compiler more
 *  than once to get a version string.
 */
var _rawVersionCache = {};

/*
 *  ======== _noCompressTargets ========
 *  List of targets for which we use --no_compress option.
 */
var _noCompressTargets = {C64P: 1, C64P_big_endian: 1, C674: 1, C674_big_endian: 1, C64T_big_endian: 1, C64T: 1};

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
        _setEnv(target, result);

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
 *  ======== ITarget.asmName ========
 */
function asmName(cname)
{
    return ('_' + cname);
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

    var pragmaOnly = false;
    if (this.binaryParser != "ti.targets.omf.cof.Coff") {
        pragmaOnly = true;
    }

    var sb = new java.lang.StringBuilder();
    for (var i = 0; i < names.length; i++) {
        sb.append('#pragma DATA_SECTION(' + names[i] + ', \".const:' + names[i]
            + '\");\n');

        if (!pragmaOnly) {
            sb.append('asm(\"\t.sect \\\".const:' + names[i] + '\\\"\");\n');
            sb.append('asm(\"\t.clink \");\n');

            /* fix for the alchemy problem with renaming sections */
            sb.append('asm(\"\t.sect \\\"[0].const:' + names[i] + '\\\"\");\n');
            sb.append('asm(\"\t.clink \");\n');
            sb.append('asm(\"\t.sect \\\"[1].const:' + names[i] + '\\\"\");\n');
            sb.append('asm(\"\t.clink \");\n');
        }
    }
    return (sb.toString() + "");
}

/*
 *  ======== ITarget.getVersion ========
 */
function getVersion()
{
    var target = this;
    _initTabs();

    var result = target.$name + "{";

    /* call compiler to get its version number and append to result */
    var tool2cmd = _targetCmds[target.suffix];
    if (tool2cmd != null) {
        var cmd = tool2cmd["vers"].replace("\$(rootDir)", target.rootDir);

        /* Check if rootDir is set correctly */
        var file = new java.io.File(target.rootDir);
        if (!file.exists()) {
            throw new Error(target.rootDir + " cannot be found. Ensure that" +
                " rootDir for the " + target.name + " target is set " +
                "correctly in " + java.lang.System.getenv("XDCBUILDCFG"));
        }

        var envs = {};
        _setEnv(target, envs);

        /* if we already know the result, return it now */
        var tmp;
        var verKey = cmd + envs.path;
        if ((tmp = _versionCache[verKey]) != null) {
            return (result + tmp.split("{")[1]);
        }

        /* otherwise, run compiler to get version string output */
        var path = "PATH=" + envs.path.join(";");
        path = path.replace("$(" + target.$name + ".rootDir)",
            target.rootDir, "g");
        if (environment["xdc.hostOS"] != "Windows") {
            path = path.replace(/;/g, ":");
        }
        var attrs = {envs: [path]};

        var status = {};
        if (xdc.exec(cmd, attrs, status) >= 0) {
            var comp = status.output.match(/^\w+/);
            comp = comp == null ? "" : comp[0];

            /* extract compiler version number from compiler output */
            var cver;
            var va = status.output.match(/Version\s+[0-9\.]+/mg);
            if (va != null) {       /* old style version string */
                cver = va[0].replace(/[Version\s]/g, "");
            }
            else {
                va = status.output.match(/\s+v([0-9\.]+)([IPAB]([0-9]+)){0,1}/mg);
                if (va != null) {   /* new style version string */
                    cver = va[0].replace(/\s+v/, "");
                }
                else {
                    va = status.output.match(/^([0-9\.]+)([IPAB]([0-9]+)){0,1}/);
                    /* --compiler_revision style version string */
                    if (va != null) {
                        cver = va[0];
                    }
                    else {
                        va = status.output.match(/clang\sversion\s[0-9]+\.[0-9]+\.[0-9]+/g);
                        if (va != null) {
                            cver = va[0].replace(/[clang\sversion\s]/g, "");
                        }
                        else {
                            va = status.output.match(/Clang\sCompiler\s[0-9]+\.[0-9]+\.[0-9]+/g);
                            if (va != null) {
                                cver = va[0].replace(/[Clang\sCompiler\s]/g, "");
                            }
                            else {
                                print(target.$name
                                    + ": warning: version match failed on '"
                                    + status.output + "'");
                                cver = "0.0.0";
                            }
                        }
                    }
                }
            }

            /* get user-specified key mapping */
            var key = target.versionMap[comp + "_" + cver];

            /* if not specified, generate a default mapping */
            if (key == null) {
    //          print(target.$name +" can't find '" + comp + "_" + cver + "'");
                va = cver.split('.');
                var radius = va[0] + '.' + va[1];   /* Major.Minor */
                var patch = 0;                      /* Update[.W] */
                var suffix;                         /* [,date] */
                if (va.length == 3) {       /* new style (without W) */
                    va = va[2].match(/([0-9]+)([IPAB]([0-9]+)){0,1}/);
                    patch = va[1];
                    suffix = va[3];
                }
                else if (va.length == 4) {  /* new style (with W) */
                    var w = va[2];
                    va = va[3].match(/([0-9]+)([IPAB]([0-9]+)){0,1}/);
                    patch = va[1] + '.' + w;
                    suffix = va[3];
                }
                suffix = suffix == null ? "" : (',' + suffix);

                key = "1,0," + radius + "," + patch + suffix;
            }

            /* create target compatibility string */
            result += key;
        }
        else {
            print(target.$name + ": warning: exec of '" + cmd + "' failed:"
                + status.output + "; check tools installation");
        }

        /* cache the result so we don't have to run this command again */
        _versionCache[verKey] = result;
    }

    return (result);
}

/*
 *  ======== ITarget3.getRawVersion ========
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
        if (!file.exists()) {
            throw new Error(target.rootDir + " cannot be found. Ensure that" +
                " rootDir for the " + target.name + " target is set " +
                "correctly in " + java.lang.System.getenv("XDCBUILDCFG"));
        }

        var envs = {};
        _setEnv(target, envs);

        /* if we already know the result, return it now */
        var tmp;
        var verKey = cmd + envs.path;
        if ((tmp = _rawVersionCache[verKey]) != null) {
            return (result + tmp);
        }

        /* otherwise, run compiler to get version string output */
        var path = "PATH=" + envs.path.join(";");
        path = path.replace("$(" + target.$name + ".rootDir)",
            target.rootDir, "g");
        if (environment["xdc.hostOS"] != "Windows") {
            path = path.replace(/;/g, ":");
        }
        var attrs = {envs: [path]};

        var status = {};
        if (xdc.exec(cmd, attrs, status) >= 0) {
           result += status.output.replace("\n", "");
        }
        else {
            print(target.$name + ": warning: exec of '" + cmd + "' failed:"
                + status.output + "; check tools installation");
        }

        /* cache the result so we don't have to run this command again */
        _rawVersionCache[verKey] = result;
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
        _setEnv(target, result);

        var lib = "";
        if (("linkLib" in target) && target.linkLib != null) {
            if (target.$name.match(/clang/)) {
                lib = " -l $(rootDir)/lib/generic/" + target.linkLib;
            }
            else {
                lib = " -l $(rootDir)/lib/" + target.linkLib;
            }
        }

        var cmd = tool2cmd["link"];
        var pre;
        if (target.$name.match(/clang/)) {
            pre = target.lnkOpts == null ? "" :
                (goal.dllMode ? "-Wl,-q" : target.lnkOpts.prefix);
        }
        else {
            pre = target.lnkOpts == null ? "" :
                (goal.dllMode ? "-q" : target.lnkOpts.prefix);
        }

        var compString = this.getVersion().split('{')[1];
        var compVersion = compString.split(',');

        if (!target.$name.match(/clang/)) {
            var fsopt = "-fs $(XDCCFGDIR)$(dir $@)";
            if (Build.hostOSName == "Windows") {
                /* This is a workaround for a CodeGen bug when '/' is the
                 * last character in a directory name.
                 */
                fsopt += ".";
            }
            pre = fsopt + " " + pre;
        }

        var suf = "";
        if (target.$name.match(/clang/)) {
            suf = target.lnkOpts == null ? suf : suf + " " +
                (goal.dllMode ? "-Wl,-r -Wl,-m,$(XDCCFGDIR)/$@.map" :
                target.lnkOpts.suffix + lib);
            goal.files = goal.files.replace(/(^|\s+)(\S+\.xdl)($|\s+)/,
                " -Xlinker $2 ");
        }
        else {
            suf = target.lnkOpts == null ? suf : suf + " " +
                (goal.dllMode ? "-r -m $(XDCCFGDIR)/$@.map" :
                target.lnkOpts.suffix + lib);
        }

        result.cmds = _bldUtils.expandString(cmd, {
            LOPTS_P: pre,
            LOPTS_S: suf,
            lopts:   goal.opts,
            files:   goal.files
        });

        if (goal.dllMode) {
            result.cmds += "$(MV) $@ $(XDCCFGDIR)/$@.obj\n";
            result.cmds += "$(TOUCH) $@\n";
        }
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
var _langOpts = {asm: "-fa", c: "-fc", cpp: "-fp"};

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
            result = new xdc.om['xdc.bld.ITarget'].CommandSet;
            result.msg = (cmdType == "c" ? "cl" : cmdType)
                            + target.suffix
                            + (asm == true ? " -n $< ..." : " $< ...");

            /* BUG: normally we should retain the trailing "/" in
             * the dstDir directory but we are working around a cl6x
             * bug (SDSsq23670).
             */
            var dstDir = goal.dstPrefix + goal.base;
            dstDir = dstDir.replace(/[\/\\][^\/\\]*$/, "");

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

            _setEnv(target, result);
            if (target.$name.match(/clang/)) {
                result.cmds = _bldUtils.expandString(tool2cmd[cmdType], {
                    COPTS_P:    ccoptsPre,
                    COPTS_S:    ccoptsSuf,
                    AOPTS_P:    target.asmOpts.prefix,
                    AOPTS_S:    target.asmOpts.suffix,
                    ASMONLY:    "",
                    dstDir:     dstDir,
                    srcExt:     goal.srcSuffix,
                    copts:      (goal.configOpts && "cfgcopts" in goal.opts)
                                ? goal.opts.cfgcopts : goal.opts.copts,
                    aopts:      goal.opts.aopts,
                    defs:       goal.opts.defs,
                    incs:       goal.opts.incs,
                    langOpt:    ""
                });
            }
            else {
                result.cmds = _bldUtils.expandString(tool2cmd[cmdType], {
                    COPTS_P:    ccoptsPre,
                    COPTS_S:    ccoptsSuf,
                    AOPTS_P:    target.asmOpts.prefix,
                    AOPTS_S:    target.asmOpts.suffix,
                    ASMONLY:    asm ? "-n -s --symdebug:none" : "",
                    dstDir:     dstDir,
                    srcExt:     goal.srcSuffix,
                    copts:      (goal.configOpts && "cfgcopts" in goal.opts)
                                ? goal.opts.cfgcopts : goal.opts.copts,
                    aopts:      goal.opts.aopts,
                    defs:       goal.opts.defs,
                    incs:       goal.opts.incs,
                    langOpt:    langOpt
                });
            }
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

        /*
         * Loop over all packages in case another package has a module
         * that wants to extend this package's ITarget interface.
         */
        var pa = xdc.om.$packages;
        for (var j = 0; j < pa.length; j++) {
            var ma = pa[j].$modules;
            for (var i = 0; i < ma.length; i++) {
                if (ma[i] instanceof _thisPkg.ITarget.Module) {
                    var targ = _thisPkg.ITarget.Module(ma[i]);
                    _mkCmds(targ);
                }
            }
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

    /* cmd prefix:
     *      $(binDir) for optional codegen debug support
     */
    var cmdPrefix = target.binDir;

    /* default make dependencies command */
    var mkdep = "$(MKDEP) -a $@.dep -p $(dstDir) -s o"
            + target.suffix + " $< -C ";

    /* define assembly options */
    if (target.$name.match(/clang/)) {
        cmdOpts = " $(AOPTS_P) " + target.asm.opts 
          + " $(AOPTS_S) $(defs) $(aopts) $(incs) $(XDCINCS) "
          + target.includeOpts;
        /* define assemble command template */
        cmd = cmdPrefix + target.asm.cmd + cmdOpts
          + " $(langOpt) -o $@ $<\n";
    }
    else {
        cmdOpts = " $(AOPTS_P) " + target.asm.opts
          + " -eo.o" + target.suffix
          + " -ea.s" + target.suffix
          + " $(AOPTS_S) $(defs) $(aopts) $(incs) $(XDCINCS) "
          + target.includeOpts;
        /* define assemble command template */
        cmd = cmdPrefix + target.asm.cmd + cmdOpts
          + " -fr=./$(dstDir) $(langOpt) $<\n";
    }
    cmd = cmd.concat(mkdep + cmdOpts);
    tool2cmd["asm"] = cmd;

    /* define the C options */
    /* -eo and -ea options are added here rather than in target defintions,
     * so that configuro can ask for cc.opts and get only the necessary
     * options.
     *
     * The users who build with XDC still get the extension functionality that
     * allows us to build for multiple targets.
     */
    if (target.$name.match(/clang/)) {
        cmdOpts = " $(ASMONLY) $(COPTS_P) $(COPTS_S) $(defs) $(copts) "
            + "$(incs) $(XDCINCS) " + target.includeOpts;
        /* define the C compile command template */
        cmd =  cmdPrefix + target.cc.cmd + " " + target.cc.opts + cmdOpts +
            " $(langOpt) -o $@ $<\n";
    }
    else {
        cmdOpts = " $(ASMONLY) $(COPTS_P) " + target.cc.opts + " -eo.o"
          + target.suffix + " -ea.s" + target.suffix
          + " $(COPTS_S) $(defs) $(copts) $(incs) $(XDCINCS) "
          + target.includeOpts + " -fs=./$(dstDir) -fr=./$(dstDir)";
        /* define the C compile command template */
        cmd =  cmdPrefix + target.cc.cmd + cmdOpts + " $(langOpt) $<\n";
    }

    if (0) {
         /* redefine MKDEP command template to use compiler.  This gives us
          * precise dependencies but the TI compiler does not (yet) allow one
          * to specify the output file for dependiences, so we can't run a
          * parallel build!
          *
          * Once the compiler has the ability to support parallel build we
          * can incorporate the dependency generation with the compilation
          * and avoid a separate compile step (-ppa -ppd=$@.dep).
          */
         mkdep = cmdPrefix + target.cc.cmd + " -ppd " + cmdOpts + " $<\n"
            + "@$(MV) $(patsubst %$(srcExt),%.pp,$<) $@.dep\n#";
    }

    cmd = cmd.concat(mkdep + cmdOpts);
    tool2cmd["c"] = cmd;

    /* define the C++ compile command template */
    tool2cmd["cpp"] = cmd;  /* only diff between C and C++ is $(langOpt) */

    /* define the link command template */
    cmd = "$(RM) $(XDCCFGDIR)/$@.map\n" + cmdPrefix + target.lnk.cmd
              + " $(LOPTS_P) " + target.lnk.opts
              + " $(lopts) -o $@ $(files) $(LOPTS_S)\n";
    tool2cmd["link"] = cmd;

    /* define the ar command template */
    cmd = cmdPrefix + target.ar.cmd + " $(AROPTS_P) " + target.ar.opts
            + " $@ $(AROPTS_S) $(aropts) $(files)";
    tool2cmd["ar"] = cmd;

    /* define the vers command template */
    cmd = cmdPrefix + target.vers.cmd + " " + target.vers.opts;
    tool2cmd["vers"] = cmd;

    /* add set of command templates to _targetCmds */
    _targetCmds[target.suffix] = tool2cmd;
}

/*
 *  ======== _setEnv ========
 */
function _setEnv(target, result)
{
    var path = _targetEnvs[target.suffix];

    if (path == null) {
        var pathTemplate = "$(pathPrefix);$(binDir);$(PATH)";

        /* expand path template (because binDir and pathPrefix might
         * embed additional $(...) elements
         */
        var tmp = _bldUtils.expandString(pathTemplate, {
            binDir:     target.binDir,
            pathPrefix: target.pathPrefix
        });
        path = tmp.split(";");

        /* save the result to avoid recomputing this for each compile line */
        _targetEnvs[target.suffix] = path;
    }

    result.path = path;
    result.envs = ["C_DIR="];
}
