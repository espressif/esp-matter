/* 
 *  Copyright (c) 2008-2018 Texas Instruments Incorporated
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
 *  ======== IArm.asmName ========
 */
function asmName(CName)
{
    return (CName);
}

/*
 *  ======== IArm.link ========
 */
function _hide_link(goal)
{
    var target = this;

    var cap = xdc.loadCapsule("ti/targets/ITarget.xs");
    var _bldUtils = xdc.useModule('xdc.bld.Utils');

    cap._initTabs();

    var compString = this.getVersion().split('{')[1];
    var compVersion = compString.split(',');

    var result = null;
    var tool2cmd = cap._targetCmds[target.suffix];

    if (tool2cmd != null) {
        result = new xdc.om['xdc.bld.ITarget'].CommandSet;
        result.msg = "lnk" + target.suffix + " $@ ...";
        cap._setEnv(target, result);

        var cmd = tool2cmd["link"];
        var pre = target.lnkOpts == null ? "" :
            (goal.dllMode ? "" : target.lnkOpts.prefix);

        result.cmds = _bldUtils.expandString(cmd, {
            LOPTS_P: pre,
            LOPTS_S: target.lnkOpts == null ? "" :
                (goal.dllMode ? "-r -m $(XDCCFGDIR)/$@.map" :
                                target.lnkOpts.suffix),
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
 *  ======== IArm.getVersion ========
 */
function getVersion()
{
    /* SYS/BIOS and TI-RTOS versions that use this tree require an Arm compiler
     * 5.2.2 or newer.
     */
    var File = xdc.module("xdc.services.io.File");
    if (!(File.exists(this.rootDir + "/bin/armcl")
        || File.exists(this.rootDir + "/bin/armcl.exe")) &&
        !(File.exists(this.rootDir + "/bin/tiarmclang")
        || File.exists(this.rootDir + "/bin/tiarmclang.exe"))) {
        if (!(File.exists(this.rootDir + "/bin/cl470")
            || File.exists(this.rootDir + "/bin/cl470.exe"))) {
            throw new Error("Cannot find compiler in " + this.rootDir
                + ". Please check compiler path.");
        }
        else {
            throw new Error("The compiler in " + this.rootDir + " is not compatible"
                + " with this version of TI-RTOS or SYS/BIOS. Please use Arm"
                + " compiler 5.2.2 or newer.");
        }
    }

    return (xdc.module("ti.targets.ITarget").getVersion.$fxn.call(this));
}
/*
 *  @(#) ti.targets.arm.elf; 1, 0, 0,; 7-28-2021 06:57:41; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

