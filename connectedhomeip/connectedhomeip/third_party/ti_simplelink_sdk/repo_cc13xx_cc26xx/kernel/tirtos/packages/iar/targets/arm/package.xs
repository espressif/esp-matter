/* 
 *  Copyright (c) 2013-2017 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * */
var cap = xdc.loadCapsule("ITarget.xs");

function init()
{
    cap._init(this);

    /* Remove target checks */
    environment["xdc.cfg.check.exclude"] = ".*";

    var ma = this.$modules;
    var iarTargets = xdc.loadPackage('iar.targets.arm');

    for (var i = 0; i < ma.length; i++) {
        if (ma[i] instanceof iarTargets.ITarget.Module) {
            var targ = iarTargets.ITarget.Module(ma[i]);

            /* add .s<suffix> to list of recognized extensions */
            var ext = ".s" + targ.suffix;
            targ.extensions.$putHead(ext,new xdc.om['xdc.bld.ITarget.Extension'](
                {
                    suf: ext,
                    typ: "asm"
                }
            ));
        }
    }
}
/*
 *  @(#) iar.targets.arm; 1, 0, 0,; 7-28-2021 06:57:36; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

