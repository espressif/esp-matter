/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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
   /* Flash sectors */
   CMBANK0_RESETISR : origin = 0x00200000, length = 0x00000008 /* Boot Entry */
   CMBANK0_SECTOR0  : origin = 0x00200008, length = 0x00003FF7
   CMBANK0_SECTOR1  : origin = 0x00204000, length = 0x00004000
   CMBANK0_SECTOR2  : origin = 0x00208000, length = 0x00004000
   CMBANK0_SECTOR3  : origin = 0x0020C000, length = 0x00004000
   CMBANK0_SECTOR4  : origin = 0x00210000, length = 0x00010000
   CMBANK0_SECTOR5  : origin = 0x00220000, length = 0x00010000
   CMBANK0_SECTOR6  : origin = 0x00230000, length = 0x00010000
   CMBANK0_SECTOR7  : origin = 0x00240000, length = 0x00010000
   CMBANK0_SECTOR8  : origin = 0x00250000, length = 0x00010000
   CMBANK0_SECTOR9  : origin = 0x00260000, length = 0x00010000
   CMBANK0_SECTOR10 : origin = 0x00270000, length = 0x00004000
   CMBANK0_SECTOR11 : origin = 0x00274000, length = 0x00004000
   CMBANK0_SECTOR12 : origin = 0x00278000, length = 0x00004000
   CMBANK0_SECTOR13 : origin = 0x0027C000, length = 0x00004000

   C1RAM            : origin = 0x1FFFC000, length = 0x00001FFF
   C0RAM            : origin = 0x1FFFE000, length = 0x00001FFF

   /* Part of S0, BOOT rom will use this for stack */
   BOOT_RSVD        : origin = 0x20000000, length = 0x00000800
   S0RAM            : origin = 0x20000800, length = 0x000037FF
   S1RAM            : origin = 0x20004000, length = 0x00003FFF
   S2RAM            : origin = 0x20008000, length = 0x00003FFF
   S3RAM            : origin = 0x2000C000, length = 0x00003FFF
   E0RAM            : origin = 0x20010000, length = 0x00003FFF

   CPU1TOCMMSGRAM0  : origin = 0x20080000, length = 0x00000800
   CPU1TOCMMSGRAM1  : origin = 0x20080800, length = 0x00000800
   CMTOCPU1MSGRAM0  : origin = 0x20082000, length = 0x00000800
   CMTOCPU1MSGRAM1  : origin = 0x20082800, length = 0x00000800
   CPU2TOCMMSGRAM0  : origin = 0x20084000, length = 0x00000800
   CPU2TOCMMSGRAM1  : origin = 0x20084800, length = 0x00000800
   CMTOCPU2MSGRAM0  : origin = 0x20086000, length = 0x00000800
   CMTOCPU2MSGRAM1  : origin = 0x20086800, length = 0x00000800
}

SECTIONS
{
   .resetisr        : > CMBANK0_RESETISR
   .vftable         : > CMBANK0_SECTOR0    /* App vector table Flash */
   .vtable          : > S0RAM              /* App vector table RAM */
   .text            : >> CMBANK0_SECTOR0 | CMBANK0_SECTOR1 | CMBANK0_SECTOR2 |
                         CMBANK0_SECTOR3 | CMBANK0_SECTOR4 | CMBANK0_SECTOR5
   .cinit           : > CMBANK0_SECTOR0  | CMBANK0_SECTOR1 | CMBANK0_SECTOR2 |
                        CMBANK0_SECTOR3  | CMBANK0_SECTOR4 | CMBANK0_SECTOR5
   .pinit           : > CMBANK0_SECTOR0 | CMBANK0_SECTOR1
   .binit           : > CMBANK0_SECTOR0 | CMBANK0_SECTOR1
   .switch          : >> CMBANK0_SECTOR0 | CMBANK0_SECTOR1
   .sysmem          : > S2RAM
   .stack           : > C1RAM
   .ebss            : > C1RAM
   .econst          : >> CMBANK0_SECTOR0 | CMBANK0_SECTOR1 | CMBANK0_SECTOR2 |
                         CMBANK0_SECTOR3 | CMBANK0_SECTOR4 | CMBANK0_SECTOR5
   .esysmem         : > C1RAM
   .data            : > S3RAM
   .bss             : > S3RAM
   .const           : >> CMBANK0_SECTOR0 | CMBANK0_SECTOR1 | CMBANK0_SECTOR2 |
                         CMBANK0_SECTOR3 | CMBANK0_SECTOR4 | CMBANK0_SECTOR5

    .TI.ramfunc : {} LOAD = CMBANK0_SECTOR0 | CMBANK0_SECTOR1,
                           RUN = S0RAM | S1RAM | S2RAM, table(BINIT),
                           LOAD_START(RamfuncsLoadStart),
                           LOAD_SIZE(RamfuncsLoadSize),
                           LOAD_END(RamfuncsLoadEnd),
                           RUN_START(RamfuncsRunStart),
                           RUN_SIZE(RamfuncsRunSize),
                           RUN_END(RamfuncsRunEnd),
                           ALIGN(8)

    MSGRAM_CM_TO_CPU1 : > CMTOCPU1MSGRAM0, type=NOINIT
    MSGRAM_CM_TO_CPU2 : > CMTOCPU2MSGRAM0, type=NOINIT
    MSGRAM_CPU1_TO_CM : > CPU1TOCMMSGRAM0, type=NOINIT
    MSGRAM_CPU2_TO_CM : > CPU2TOCMMSGRAM0, type=NOINIT
}
