/*
 * Copyright (c) 2017-2019, Texas Instruments Incorporated
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

MEMORY
{
    MAIN            (RX)  : origin = 0x00000000, length = 0x00180000
    APP_0_STARTUP   (RWX) : origin = 0x2C800000, length = 0x00000400
    APP_1_STARTUP   (RWX) : origin = 0x2C400000, length = 0x00000400
    APP_0_SRAM      (RWX) : origin = 0x2C400400, length = 0x0003FC00
    APP_1_SRAM      (RWX) : origin = 0x2C800400, length = 0x0003FC00
    SHARED          (RW)  : origin = 0x401C3800, length = 0x00000800
}

SECTIONS
{
    .intvecs:   > 0x2C400400
    .text   :   > APP_0_SRAM
    .const  :   > APP_0_SRAM
    .rodata :   > APP_0_SRAM
    .cinit  :   > APP_0_SRAM
    .pinit  :   > APP_0_SRAM
    .init_array   :     > APP_0_SRAM
    .binit        : {}  > APP_0_SRAM
    .app_0_startup : > APP_0_STARTUP
    .app_1_startup : > APP_1_STARTUP

    .vtable :   > APP_0_SRAM
    .data*  :   > APP_0_SRAM
    .bss    :   > APP_0_SRAM
    .sysmem :   > APP_0_SRAM
    .args   :   > APP_0_SRAM

    .ARM.exidx : > APP_0_SRAM
    .ARM.extab : > APP_0_SRAM

    .stack  :   > APP_0_SRAM (HIGH)
}
