/*
 *  Copyright 2021 by Texas Instruments Incorporated.
 *
 */

/*
 *  ======== _targetCmds ========
 *  Hash table of commands (indexed by target.suffix).  Commands are
 *  themselves hashtables (indexed by file type: "asm", "c", ...) that
 *  define the commands for cc, asm , etc.
 */
var _targetCmds = null;

/*
 *  ======== _mkCmds ========
 */
function _mkCmds(target)
{

    if (_targetCmds == null) {
        _targetCmds = {};
    }

    if (_targetCmds[target.suffix] != null) {
        return (_targetCmds[target.suffix]);
    }

    /* Is this needed here? It doesn't seem that anything calculated in
     * initVers() is used later.
     */
    target.initVers();

    var cmd, cmdOpts;
    var tool2cmd = {};

    var prefix = "";
    var ccCmd = target.cc.cmd;
    if (target.remoteHost != null) {
        prefix = "$(packageBase)/runh -h " + target.remoteHost + " ";

        /* it is not sufficient to simply overwrite the .dep file; network
         * filesystem latencies can result in file corruption
         */
        ccCmd = ccCmd.replace("-MD -MF $@.dep", "");
    }
    else if ("cmdPrefix" in target.$orig && target.$orig.cmdPrefix != undefined) {
        prefix = target.$orig.cmdPrefix;

        /* it is not sufficient to simply overwrite the .dep file; network
         * filesystem latencies can result in file corruption
         */
        ccCmd = ccCmd.replace("-MD -MF $@.dep", "");
    }

    var mkdep = null;
    if (target.remoteHost != null) {
        /* make dependencies command; we compute this rather than adding
         * "-MD -MF $@.dep" to the command line to ensure that dependency file
         * names are local.  If the names are computed remotely we run the
         * risk that non-existent files will constantly trigger unnecessary
         * rebuilds.  We also need to do this for cygwin environments because
         * we are using a natively built GNU make which does not understand
         * cygwin file names.
         */
        mkdep = "\n$(MKDEP) -o $@.dep -p $(dstDir) -s o"
            + target.suffix + " $< -C ";
    }

    /* define assemble command template */
    cmdOpts = " $(AOPTS_P) "
        + target.asm.opts
        + " $(AOPTS_S) $(defs) $(aopts) $(incs) $(XDCINCS) "
        + target.includeOpts;
    cmd  = prefix + target.asm.cmd + cmdOpts + " -o $@ $<";
    cmd = mkdep == null ? cmd : cmd.concat(mkdep + cmdOpts);
    tool2cmd["asm"] = cmd;

    /* define the C compile command template */
    cmdOpts = "$(COPTS_P) " + target.cc.opts
        + " $(COPTS_S) $(defs) $(copts) $(incs) $(XDCINCS) "
        + target.includeOpts;
    cmd = prefix + ccCmd + " -x c $(ASMONLY) " + cmdOpts + " -o $@ $<";
    tool2cmd["c"] = mkdep == null ? cmd : cmd.concat(mkdep + cmdOpts);

    /* define the C++ compile command template */
    cmd = prefix + ccCmd + " -x c++ $(ASMONLY) " + cmdOpts + " -o $@ $<";
    tool2cmd["cpp"] = mkdep == null ? cmd : cmd.concat(mkdep + cmdOpts);

    /* define the link command template */
    cmd = "$(RM) $(XDCCFGDIR)$@.map\n" + prefix + target.lnk.cmd
        + " $(LOPTS_P) $(lopts) -o $@ $(files) " + target.lnk.opts
        + " $(LOPTS_S)\n";
    tool2cmd["link"] = cmd;

    /* define the ar command template */
    cmd  = prefix + target.ar.cmd + " $(AROPTS_P) " + target.ar.opts
        + " $@ $(AROPTS_S) $(aropts) $(files)";
    tool2cmd["ar"] = cmd;

    /* add command template set to _targetCmds */
    _targetCmds[target.suffix] = tool2cmd;
    return(tool2cmd);
}
/*
 *  @(#) gnu.targets.arm; 1, 0, 0,; 7-28-2021 06:57:32; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

