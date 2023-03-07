/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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
 *  ======== MPU.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>

#if (defined ti_targets_arm_elf_R5F) || (defined ti_targets_arm_elf_R5Ft)
#include <ti/sysbios/family/arm/v7r/Cache.h>
#else
#include <ti/sysbios/hal/Cache.h>
/*
 *  Cache_getEnabled() is not part of ICache. Creating a dummy macro
 *  so compiler does not complain.
 */
#define Cache_getEnabled()  0
#endif

#include "package/internal/MPU.xdc.h"

#define MPU_RBAR_VALID      0x00000010UL

/*
 *  ======== MPU_startup ========
 */
Void MPU_startup()
{
    UInt i;

    if (MPU_isEnabled()) {
        MPU_disable();
    }

    MPU_disableBR();

    /*
     * Initialize all MPU regions
     */
    for (i = 0; i < MPU_numRegions; i++) {
        if (!MPU_isMemoryMapped) {
            MPU_setRegionAsm(i, MPU_regionEntry[i].baseAddress,
                MPU_regionEntry[i].sizeAndEnable,
                MPU_regionEntry[i].regionAttrs);
        }
        else {
            MPU_deviceRegs.RBAR = MPU_regionEntry[i].baseAddress |
                                  MPU_RBAR_VALID | i;
            MPU_deviceRegs.RASR = (MPU_regionEntry[i].regionAttrs << 16) |
                                  (MPU_regionEntry[i].sizeAndEnable & 0xFFFF);
        }
    }

    if (MPU_enableBackgroundRegion) {
        MPU_enableBR();
    }

    if (MPU_enableMPU) {
        MPU_enable();
    }
}

/*
 *  ======== MPU_disable ========
 *  Function to disable the MPU.
 */
Void MPU_disable()
{
    UInt16 type;
    UInt   key;

    if (!(MPU_isEnabled())) {
        return;
    }

    key = Hwi_disable();

    /* get the current enabled bits */
    type = Cache_getEnabled();

    /* disable all enabled caches */
    Cache_disable(type);

    /* Ensure all memory transactions have completed */
#if (defined(__ti__) || defined(__IAR_SYSTEMS_ICC__))
    asm (" dsb");
#else
    __asm__ (" dsb");
#endif

    /* disables the MPU */
    if (!MPU_isMemoryMapped) {
        MPU_disableAsm();
    }
    else {
        MPU_deviceRegs.CTRL = MPU_deviceRegs.CTRL & (~0x1);
    }

    /* set cache back to initial settings */
    Cache_enable(type);

    Hwi_restore(key);
}

/*
 *  ======== MPU_enable ========
 *  Function to enable the MPU.
 */
Void MPU_enable()
{
    UInt16 type;
    UInt   key;

    /* if MPU is already enabled then just return */
    if (MPU_isEnabled()) {
        return;
    }

    key = Hwi_disable();

    /* get the current enabled bits */
    type = Cache_getEnabled();

    if (type & Cache_Type_ALLP) {
        Cache_disable(Cache_Type_ALLP);
    }

    if (!MPU_isMemoryMapped) {
        MPU_enableAsm();
    }
    else {
        MPU_deviceRegs.CTRL = MPU_deviceRegs.CTRL | 0x1;
    }

    /* set cache back to initial settings */
    Cache_enable(type);

#if (defined(__ti__) || defined(__IAR_SYSTEMS_ICC__))
    asm (" dsb");
    asm (" isb");
#else
    __asm__ (" dsb");
    __asm__ (" isb");
#endif

    Hwi_restore(key);
}

/*
 *  ======== MPU_disableBR ========
 */
Void MPU_disableBR()
{
    if (!MPU_isMemoryMapped) {
        MPU_disableBRAsm();
    }
    else {
        MPU_deviceRegs.CTRL = MPU_deviceRegs.CTRL & (~0x4);
    }
}

/*
 *  ======== MPU_enableBR ========
 */
Void MPU_enableBR()
{
    if (!MPU_isMemoryMapped) {
        MPU_enableBRAsm();
    }
    else {
        MPU_deviceRegs.CTRL = MPU_deviceRegs.CTRL | 0x4;
    }
}

/*
 *  ======== isEnabled ========
 */
Bool MPU_isEnabled()
{
    if (!MPU_isMemoryMapped) {
        return (MPU_isEnabledAsm());
    }
    else {
        return ((MPU_deviceRegs.CTRL & 0x1) != 0);
    }
}

/*
 *  ======== MPU_initRegionAttrs ========
 */
Void MPU_initRegionAttrs(MPU_RegionAttrs *attrs)
{
    Assert_isTrue(attrs != NULL, MPU_A_nullPointer);

    attrs->enable = MPU_defaultAttrs.enable;
    attrs->bufferable = MPU_defaultAttrs.bufferable;
    attrs->cacheable = MPU_defaultAttrs.cacheable;
    attrs->shareable = MPU_defaultAttrs.shareable;
    attrs->noExecute = MPU_defaultAttrs.noExecute;
    attrs->accPerm = MPU_defaultAttrs.accPerm;
    attrs->tex = MPU_defaultAttrs.tex;
    attrs->subregionDisableMask = MPU_defaultAttrs.subregionDisableMask;
}

/*
 *  ======== MPU_setRegion ========
 */
Void MPU_setRegion(UInt8 regionId, Ptr regionBaseAddr,
    MPU_RegionSize regionSize, MPU_RegionAttrs *attrs)
{
    UInt32 key;
    Bool   enabled;
    UInt32 sizeAndEnableReg, regionAttrsReg;

    Assert_isTrue(attrs != NULL, MPU_A_nullPointer);
    Assert_isTrue(regionId < MPU_numRegions, MPU_A_invalidRegionId);
    Assert_isTrue(regionBaseAddr ==
        (Ptr)((UInt32)regionBaseAddr & (~0 << ((regionSize >> 1) + 1))),
        MPU_A_unalignedBaseAddr);
    Assert_isTrue(!(
        (attrs->tex == 1 &&
         attrs->cacheable == FALSE && attrs->bufferable == TRUE) ||
        (attrs->tex == 1 &&
         attrs->cacheable == TRUE && attrs->bufferable == FALSE)),
        MPU_A_reservedAttrs);

    enabled = MPU_isEnabled();

    /* disable the MPU (if already disabled, does nothing) */
    MPU_disable();

    sizeAndEnableReg = (attrs->subregionDisableMask << 8) | regionSize |
                       (attrs->enable);
    regionAttrsReg   = (attrs->noExecute << 12) | (attrs->accPerm << 8) |
        (attrs->tex << 3) | (attrs->shareable << 2) | (attrs->cacheable << 1) |
        (attrs->bufferable);

    key = Hwi_disable();

    if (!MPU_isMemoryMapped) {
        MPU_setRegionAsm(regionId, (UInt32)regionBaseAddr, sizeAndEnableReg,
            regionAttrsReg);
    }
    else {
        MPU_deviceRegs.RBAR = (UInt32)regionBaseAddr | MPU_RBAR_VALID | regionId;
        MPU_deviceRegs.RASR = (regionAttrsReg << 16) |
                              (sizeAndEnableReg & 0xFFFF);
    }

    /* Copy register values to module state for use by ROV */
    (MPU_module->regionEntry[regionId]).baseAddress = (UInt32)regionBaseAddr;
    (MPU_module->regionEntry[regionId]).sizeAndEnable = sizeAndEnableReg;
    (MPU_module->regionEntry[regionId]).regionAttrs = regionAttrsReg;

    Hwi_restore(key);

    if (enabled) {
        MPU_enable();
    }
}

/*
 *  ======== MPU_setRegionRaw ========
 *  Cortex-M specific implementation
 */
Void MPU_setRegionRaw(UInt32 rbar, UInt32 rasr)
{
    UInt32 key;
    Bool   enabled;

    if (MPU_isMemoryMapped) {
        key = Hwi_disable();

        enabled = MPU_isEnabled();

        /* disable the MPU (if already disabled, does nothing) */
        MPU_disable();

        MPU_deviceRegs.RBAR = rbar;
        MPU_deviceRegs.RASR = rasr;

        if (enabled) {
            MPU_enable();
        }

        Hwi_restore(key);
    }
}
