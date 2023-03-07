/* 
 *  Copyright (c) 2008-2015 Texas Instruments Incorporated
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * */
/*
 *  ======== ITarget.asmName ========
 */
function asmName(CName)
{
    return (CName);
}

/*
 *  ======== ITarget.link ========
 */
function _hide_link(goal)
{
    var target = this;

    var cap = xdc.loadCapsule("ti/targets/ITarget.xs");
    var _bldUtils = xdc.useModule('xdc.bld.Utils');

    cap._initTabs();

    var compString = this.getVersion().split('{')[1];
    var compVersion = compString.split(',');

    /* Partial link can be used only in compilers with the fix for SDOCM31285 */
    if (!target.isa[0] == "6" || compVersion[2] < "7.1") {
         return (cap.link.call(this, goal));
    }

    var result = null;
    var tool2cmd = cap._targetCmds[target.suffix];

    if (tool2cmd != null) {
        result = new xdc.om['xdc.bld.ITarget'].CommandSet;
        result.msg = "lnk" + target.suffix + " $@ ...";
        cap._setEnv(target, result);

        var lib = "";
        if (("linkLib" in target) && target.linkLib != null) {
            lib = " -l $(rootDir)/lib/" + target.linkLib;
        }

        var cmd = tool2cmd["link"];
        var pre = target.lnkOpts == null ? "" :
            (goal.dllMode ? "" : target.lnkOpts.prefix);

        result.cmds = _bldUtils.expandString(cmd, {
            LOPTS_P: pre,
            LOPTS_S: target.lnkOpts == null ? "" :
                (goal.dllMode ? "-r -m $(XDCCFGDIR)/$@.map" :
                                target.lnkOpts.suffix + lib),
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
 *  @(#) ti.targets.elf; 1, 0, 0,; 7-28-2021 06:57:45; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

