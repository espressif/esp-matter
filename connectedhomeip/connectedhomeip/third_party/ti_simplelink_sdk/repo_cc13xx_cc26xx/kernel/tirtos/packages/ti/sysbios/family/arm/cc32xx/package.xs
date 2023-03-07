/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 *  ======== package.xs ========
 */

var Build;
var BIOS;
var Boot;

/*
 *  ======== Package.getLibs ========
 *  This function is called when a program's configuration files are
 *  being generated and it returns the name of a library appropriate
 *  for the program's configuration.
 */
function getLibs(prog)
{
    Build = xdc.module("ti.sysbios.Build");

    /* use shared getLibs() */
    return (Build.getLibs(this));
}

/*
 *  ======== Package.getSects ========
 */
function getSects()
{
    return "ti/sysbios/family/arm/cc32xx/linkcmd.xdt";
}

/*
 *  ======== Package.init ========
 *  If this package is used pull in the Boot module.
 */
function init()
{
    BIOS = xdc.module("ti.sysbios.BIOS");
    Build = xdc.module("ti.sysbios.Build");
}

/*
 *  ======== Package.close ========
 */
function close()
{
    var File = xdc.module("xdc.services.io.File");
    Build = xdc.module("ti.sysbios.Build");
    /* add driverlib include file dirs to search path for Boot's setup.c */
    //Build.ccArgs.$add("-I " + File.getDOSPath(this.packageBase) + "driverlib" + Boot.driverlibVersion + "/inc/");
    //Build.ccArgs.$add("-I " + File.getDOSPath(this.packageBase) + "driverlib" + Boot.driverlibVersion + "/source/");
}
