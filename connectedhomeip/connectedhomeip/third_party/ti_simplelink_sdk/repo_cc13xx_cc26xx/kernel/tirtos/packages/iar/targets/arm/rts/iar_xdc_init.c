/* 
 *  Copyright (c) 2013 Texas Instruments and others.
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
 *  ======== iar_xdc_init ========
 *  The required XDC Startup hooks are called
 */

#include <xdc/runtime/Startup.h>
extern int xdc_runtime_Startup__RESETFXN__C;
extern int xdc_runtime_Startup__EXECFXN__C;

void iar_xdc_startup_reset(void)
{
    /*------------------------------------------------------------------------*/
    /* Call hook configured into Startup_resetFxn                             */
    /*------------------------------------------------------------------------*/
    if (&xdc_runtime_Startup__RESETFXN__C == (int*)1) {
        xdc_runtime_Startup_reset__I();
    }
}

void iar_xdc_startup_exec(void)
{
    /*------------------------------------------------------------------------*/
    /* Process XDC Startup                                                    */
    /*------------------------------------------------------------------------*/
    if (&xdc_runtime_Startup__EXECFXN__C == (int*)1) {
        xdc_runtime_Startup_exec__E();
    }
}
/*
 *  @(#) iar.targets.arm.rts; 1, 0, 0,; 7-28-2021 07:05:45; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

