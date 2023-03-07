/* 
 *  Copyright (c) 2008-2018 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * 
 */
/*
 *  ======== package.xdc ========
 *
 */

requires xdc.bld;

/*!
 *  ======== ti.targets ========
 *  Package of targets for TI code generation tools
 *
 *  The modules in this package implement the xdc.bld.ITarget interface; this
 *  interface "wraps" the compiler tool-chain with an abstract interface
 *  that enables these tools to to be used by the XDC Build Engine.
 */
package ti.targets [1,0,3] {
    module C28, C28_large, C28_float;
    module C64P, C674;
    module C66;

    interface ITarget;
}
/*
 *  @(#) ti.targets; 1, 0, 3,; 7-28-2021 06:57:38; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

