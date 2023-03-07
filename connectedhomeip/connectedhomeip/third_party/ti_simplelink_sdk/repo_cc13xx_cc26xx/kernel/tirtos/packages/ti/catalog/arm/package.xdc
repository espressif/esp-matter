/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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

requires xdc.platform;
requires ti.catalog.arm.peripherals.timers;

/*!
 *  ======== ti.catalog.arm ========
 *  Package of devices for the arm family of Arm processors.
 *
 *  Each module in this package implements the xdc.platform.ICpuDataSheet
 *  interface. This interface is used by platforms (modules that implement
 *  xdc.platform.IPlatform) to obtain the memory map supported by each CPU.
 */
package ti.catalog.arm [1,0,1] {
    module TMS470R10;
    module TMS320C5912;
    module TMS320C1030;
    module TMS320CDM6446, TMS320CDM6467;
    module TMS320DM355, TMS320DM357, TMS320DM365;
    module TMS320CDM510;
    interface IOMAP2x3x;
    module OMAP2430, OMAP2431, OMAP2530, OMAP2531;
    interface IOMAP3xxx;
    module OMAP3403, OMAP3405, OMAP3503, OMAP3505, OMAP3515, OMAP3517;
    module OMAP3425, OMAP3525, TMS320C3430, OMAP3530;
    interface ITMS320DA8xx;
    module TMS320DA828, TMS320DA830;
    module OMAPL137;
    module CortexM3;
    module TMS570PSF762, TMS570LS20216;
    module TMS320DA840, TMS320DA850;
    module AM1705, AM1707;
    module AM1806, AM1808;
    module OMAPL108, OMAPL118, OMAPL138;
    module TMS320CDM730, TMS320CDM740;
    module TNETV107X;
}
