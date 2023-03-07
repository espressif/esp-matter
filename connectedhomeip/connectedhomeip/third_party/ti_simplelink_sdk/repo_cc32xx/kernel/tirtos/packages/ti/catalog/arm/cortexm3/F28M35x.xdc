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

/*
 *  ======== F28M35x.xdc ========
 *
 */

/*!
 *  ======== F28M35x.xdc ========
 *  The F28M35x device data sheet module.
 *
 *  This module implements the xdc.platform.ICpuDataSheet interface and is
 *  used by platforms to obtain "data sheet" information about this device.
 *
 */

metaonly module F28M35x inherits ti.catalog.ICpuDataSheet
{

instance:
    override config string cpuCore           = "CM3";
    override config string isa               = "v7M";
    override config string cpuCoreRevision   = "1.0";
    override config int    minProgUnitSize   = 1;
    override config int    minDataUnitSize   = 1;
    override config int    dataWordSize      = 4;

    config ti.catalog.arm.peripherals.timers.Timer.Instance timer0;
    config ti.catalog.arm.peripherals.timers.Timer.Instance timer1;
    config ti.catalog.arm.peripherals.timers.Timer.Instance timer2;
    config ti.catalog.arm.peripherals.timers.Timer.Instance timer3;

    /*!
     *  ======== memMap ========
     *  The memory map returned by getMemoryMap().
     */
    config xdc.platform.IPlatform.Memory memMap[string] = [
        ["BOOTROM", {
            comment: "On-Chip Boot ROM",
            name: "BOOTROM",
            base: 0x0,
            len:  0x10000,
            space: "code",
            access: "RX"
        }],

        ["FLASH_BOOT", {
            comment: "Branch for boot from Flash",
            name: "FLASH_BOOT",
            base: 0x200030,
            len:  0x4,
            space: "code/data",
            access: "RWX"
        }],

        ["FLASH", {
            comment: "512KB Flash memory",
            name: "FLASH",
            base: 0x200034,
            len:  0x7FFC8,
            space: "code/data",
            access: "RWX"
        }],

        ["OTP", {
            comment: "4KB OTP memory",
            name: "OTP",
            base: 0x680000,
            len:  0x1000,
            space: "code",
            access: "RWX"
        }],

        ["C03SRAM", {
            comment: "32KB On-Chip RAM Memory",
            name: "C03SRAM",
            base: 0x20000000,
            len:  0x8000,
            space: "code/data",
            access: "RWX"
        }],

        ["S07SHRAM", {
            comment: "64KB On-Chip Shared RAM Memory",
            name: "S07SHRAM",
            base: 0x20008000,
            len:  0x10000,
            space: "code/data",
            access: "RWX"
        }],

        ["CTOMMSGRAM", {
            comment: "C28 to M3 MSG RAM Memory",
            name: "CTOMMSGRAM",
            base: 0x2007F000,
            len:  0x800,
            space: "data",
            access: "R"
        }],

        ["MTOCMSGRAM", {
            comment: "M3 to C28 MSG RAM Memory",
            name: "MTOCMSGRAM",
            base: 0x2007F800,
            len:  0x800,
            space: "data",
            access: "RW"
        }],
    ];
};
