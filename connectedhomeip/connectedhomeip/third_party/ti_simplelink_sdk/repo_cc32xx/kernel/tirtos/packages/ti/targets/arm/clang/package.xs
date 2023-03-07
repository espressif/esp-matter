/*
 *  Copyright 2021 by Texas Instruments Incorporated.
 *
 */

/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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
 *  ======== init ========
 *  Initialize this package.
 *
 *  This is called after all modules in this package are defined and
 *  initialized.  It is called before config.bld.
 */
function init()
{
    /* Remove target checks */
    environment["xdc.cfg.check.exclude"] = ".*";

    /* initialize module configuration params that are computed */
    var ma = this.$modules;
    var tiTargets = xdc.loadPackage('ti.targets');
    for (var i = 0; i < ma.length; i++) {
        if (ma[i] instanceof tiTargets.ITarget.Module) {
            var targ = tiTargets.ITarget.Module(ma[i]);
            var isas = targ.getISAChain(null);

            if (isas.length > 0) {
                /* generate a default platform for each target, if necessary */
                if (targ.platform == null) {
                    targ.platform = "ti.platforms.sim" + isas[0] + "xx";
                }

                /* add target-specific assembly-language extensions */
                for (var idx = 0; idx < isas.length; idx++) {
                    var ext = ".s" + isas[idx];
                    targ.extensions.$putHead(ext,
                        new xdc.om['xdc.bld.ITarget.Extension'](
                            {suf: ext, typ: "asm"}
                        )
                    );
                }
            }
            else {
                print("Warning: ISAChain is empty for target " + targ);
            }

            /* add .s<suffix> to list of recognized extensions */
            var ext = ".s" + targ.suffix;
            targ.extensions.$putHead(ext,new xdc.om['xdc.bld.ITarget.Extension']({
                suf: ext,   typ: "asm"
            }));
            /* Big endian and little endian targets can use the same assembly
             * files, so they have to have each other's extension in the list
             */
            if (targ.model.endian == "little") {
                ext = ext + "e";
                targ.extensions.$putHead(
                    ext, new xdc.om['xdc.bld.ITarget.Extension']({
                    suf: ext, typ: "asm"}));

            }
            else if (targ.model.endian == "big") {
                ext = ext.substring(0, ext.length - 1);
                targ.extensions.$putHead(
                    ext, new xdc.om['xdc.bld.ITarget.Extension']({
                    suf: ext, typ: "asm"}));

            }
        }
    }
}

/*
 *  ======== Package.getLibs ========
 *  This package's implementation of xdc.IPackage.getLibs.
 */
function getLibs()
{
    return (null);    /* no library required for this package */
}
/*
 *  @(#) ti.targets.arm.clang; 1, 0, 0,; 7-28-2021 06:57:50; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

