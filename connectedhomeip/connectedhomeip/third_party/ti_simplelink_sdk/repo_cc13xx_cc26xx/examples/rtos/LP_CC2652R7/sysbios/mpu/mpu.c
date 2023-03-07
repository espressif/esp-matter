/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 *  ======== mpu.c ========
 */

#include <stdbool.h>

/* XDC Module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/family/arm/MPU.h>

#include <ti/drivers/Board.h>

#define FLASH_BASE_ADDRESS 0x00000000
#define FLASH_SIZE         0x58000    /* 352KB */

#define SRAM_BASE_ADDRESS  0x20000000
#define SRAM_SIZE          0x14000    /* 80KB */

#define GPRAM_BASE_ADDRESS 0x11000000
#define GPRAM_SIZE         0x2000     /* 8KB */

#define OFFSET_256K        0x40000

typedef void (*funcptr_t)();

void MPU_init();

int16_t dummy[2] = {0xBF00 /* "nop" opcode */, 0x4770 /* "bx lr" opcode */};

/*
 *  ======== main ========
 */
int main()
{
    funcptr_t foo;

    /* Call driver init functions */
    Board_init();

    MPU_init();

    System_printf("hello world\n");

    /*
     *  Attempt to execute function in SRAM. This should result in a Memory
     *  fault as MPU programs SRAM as non-executable.
     *
     *  See decoded exception in Hwi module's ROV view under the Exception tab.
     */
    foo = (funcptr_t)((int)&dummy[0] | (int)0x1);
    foo();

    /*
     *  normal BIOS programs, would call BIOS_start() to enable interrupts
     *  and start the scheduler and kick BIOS into gear.  But, this program
     *  is a simple sanity test and calls BIOS_exit() instead.
     */
    BIOS_exit(0);  /* terminates program and dumps SysMin output */
    return(0);
}

/*
 *  ======== MPU_init ========
 */
void MPU_init()
{
    MPU_RegionAttrs attrs;

    MPU_initRegionAttrs(&attrs);

    /* Program MPU entry for FLASH */
    attrs.enable = true;
    attrs.bufferable = false;
    attrs.cacheable = true;
    attrs.shareable = false;
    attrs.noExecute = false;
    attrs.accPerm = 5; /* Privileged Read-only */
    attrs.tex = 0;
    attrs.subregionDisableMask = 0;
    MPU_setRegion(0, (Ptr)FLASH_BASE_ADDRESS, MPU_RegionSize_256K, &attrs); /* Map first 256KB */

    attrs.subregionDisableMask = 0xC0; /* Mask last 32KB so this entry covers 96KB of FLASH */
    MPU_setRegion(1, (Ptr)(FLASH_BASE_ADDRESS + OFFSET_256K), MPU_RegionSize_128K, &attrs); /* Map last 96KB */

    /* Program MPU entry for GPRAM */
    attrs.enable = true;
    attrs.bufferable = false;
    attrs.cacheable = true;
    attrs.shareable = true;
    attrs.noExecute = true;
    attrs.accPerm = 1; /* Privileged Read-Write */
    attrs.tex = 0;
    attrs.subregionDisableMask = 0x0;
    MPU_setRegion(2, (Ptr)GPRAM_BASE_ADDRESS, MPU_RegionSize_8K, &attrs);

    /* Program MPU entry for SRAM */
    attrs.enable = true;
    attrs.bufferable = false;
    attrs.cacheable = true;
    attrs.shareable = true;
    attrs.noExecute = true;
    attrs.accPerm = 1; /* Privileged Read-Write */
    attrs.tex = 0;
    attrs.subregionDisableMask = 0xE0; /* Mask last 48KB so this entry covers 80KB of SRAM */
    MPU_setRegion(3, (Ptr)SRAM_BASE_ADDRESS, MPU_RegionSize_128K, &attrs); /* Map first 80KB */

    MPU_enable();
}
