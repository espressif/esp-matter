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
 *  ======== ITI811X.xdc ========
 *
 */

/*!
 *  ======== ITI811X ========
 *  An interface implemented by all TI811X devices
 *
 *  This interface is defined to factor common data about all TI811X type
 *  devices into a single place; they all have the same internal memory.
 */
metaonly interface ITI811X inherits ti.catalog.ICpuDataSheet
{
instance:
    config ti.catalog.peripherals.hdvicp2.HDVICP2.Instance hdvicp0;

    override config string cpuCore           = "CM3";
    override config string isa               = "v7M";
    override config string cpuCoreRevision   = "1.0";
    override config int    minProgUnitSize   = 1;
    override config int    minDataUnitSize   = 1;
    override config int    dataWordSize      = 4;

    /*!
     *  ======== memMap ========
     *  The memory map returned be getMemoryMap().
     */
    config xdc.platform.IPlatform.Memory memMap[string] = [

        /*
         * AMMU mapped L2 BOOT virtual address
         * Physical address is 0x5502_0000, virt is 0x00000000
         * Reset vectors and other boot code is placed here.
         *
         * Note that actual L2 RAM is 256K starting at 0x5502_0000.
         * The first 16K is reserved for reset vectors (i.e. L2_BOOT).
         * The remaining 240K is placed in the L2_RAM definition.
         */
        ["L2_BOOT", {
            name: "L2_BOOT",
            base: 0x00000000,
            len:  0x4000
        }],

        /*
         * AMMU mapped L2 RAM virtual address
         * Physical address is 0x5502_4000, virt is 0x2000_4000
         *
         * Note that actual L2 RAM is 256K starting at 0x5502_0000.
         * The first 16K is reserved for reset vectors (i.e. L2_BOOT).
         * The remaining 240K is placed in the L2_RAM definition.
         */
        ["L2_RAM", {
            name: "L2_RAM",
            base: 0x20004000,
            len:  0x3C000
        }],

        /*
         * OCMC (On-chip RAM)
         * Physical address is 0x40300000
         * Size is 128K
         */
        ["OCMC", {
            name: "OCMC",
            base: 0x00300000,
            len:  0x20000
        }],
    ];
};
