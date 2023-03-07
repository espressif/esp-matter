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
/*!
 *  ======== VectorTable ========
 *  IAR's vector table module
 */
metaonly module VectorTable
{
    /*!
     *  ======== getVectorTableLib  ========
     *  Configurable vector table library option
     *
     *  By default, the IAR's vector table library in this package is pulled
     *  in during link phase. RTOS and other products which define their own 
     *  vector table can disable using the vector table library provided by  
     *  this package by setting `getVectorTableLib` config parameter to false.
     */
    config Bool getVectorTableLib = true;

}
/*
 *  @(#) iar.targets.arm.rts; 1, 0, 0,; 7-28-2021 07:05:45; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

