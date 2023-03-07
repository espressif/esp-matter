/******************************************************************************
*  Filename:       rf_patch_mce_wb_dsss.h
*
*  Description: RF core patch for WB-DSSS support for CC13x2
*
*  Copyright (c) 2015-2020, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#include <stdint.h>
#include "rf_patch_mce_wb_dsss.h"
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_types.h)

#ifndef MCE_PATCH_TYPE
#define MCE_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC
#endif

#ifndef RFC_MCERAM_BASE
#define RFC_MCERAM_BASE 0x21008000
#endif

#ifndef MCE_PATCH_MODE
#define MCE_PATCH_MODE 0
#endif

MCE_PATCH_TYPE patchWbDsssMce[303] = {
   0x2fcf6076,
   0xdb3e0f9d,
   0x7f7f0303,
   0x00008080,
   0x00020001,
   0x00000003,
   0x000c0003,
   0x00cc000f,
   0x003c00c3,
   0xcccc0033,
   0x33cccc33,
   0x0f003333,
   0x04040f0f,
   0x02060305,
   0x00080107,
   0x000a0009,
   0x0c00000b,
   0x0a000b00,
   0x08000900,
   0x06020701,
   0x04040503,
   0x02020303,
   0x00000101,
   0x00000000,
   0x0c0c0000,
   0x0a0a0b0b,
   0x08080909,
   0x06060707,
   0x04040505,
   0x06060505,
   0x08080707,
   0x0a0a0909,
   0x00000b0b,
   0x00000000,
   0x00000000,
   0x02020101,
   0x00030303,
   0x001f0007,
   0x00000000,
   0x000f0400,
   0x03870000,
   0x48c80001,
   0x80000043,
   0x00800004,
   0x000006f0,
   0x0524091e,
   0x00070054,
   0x280a0000,
   0x00000028,
   0x7f7f001f,
   0x00000148,
   0x00000000,
   0x333c3c33,
   0x3cc3cccc,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x66587223,
   0x73057303,
   0x73047203,
   0x72047306,
   0x72917391,
   0xc7c0b008,
   0x8001a008,
   0x90010001,
   0x08019010,
   0x720d9001,
   0x720f720e,
   0xb0d07210,
   0xb1107100,
   0x721ba0d0,
   0x10208162,
   0x06703952,
   0x16300020,
   0x14011101,
   0x60af6c01,
   0x60ca60b1,
   0x60af614a,
   0x60af60af,
   0x122060b0,
   0x121060a6,
   0x73117223,
   0x73137312,
   0x001081b1,
   0xb07091b0,
   0x60a5608d,
   0xc49160a5,
   0xc4e0c2b2,
   0x6f131820,
   0x16116e23,
   0x68b51612,
   0xc750c4f2,
   0x40c21820,
   0x6e231203,
   0x68bf1612,
   0x9c807840,
   0xb63c7272,
   0x94807860,
   0x60a59490,
   0x73127311,
   0x91f0c000,
   0x8c80c009,
   0x06703980,
   0xc0f41610,
   0xc036c0b5,
   0x44dc1e10,
   0xc008c077,
   0x60edc01e,
   0x44e21e20,
   0xc018c0b7,
   0x60edc03e,
   0x44e81e40,
   0xc038c0f7,
   0x60edc07e,
   0x44a31e80,
   0xc078c137,
   0x1062c0fe,
   0x10831612,
   0xe0301613,
   0x9a239a12,
   0x8190659d,
   0x81709640,
   0xb0e892c0,
   0xb1287100,
   0xb230a0e8,
   0xb003b630,
   0xb002b013,
   0xb0e0b012,
   0xb1207100,
   0x22f08170,
   0xc0f04503,
   0x84c092c0,
   0x71009640,
   0x84d0b120,
   0x71009640,
   0x84a0b120,
   0x71009640,
   0x84b0b120,
   0x81df9640,
   0x82106533,
   0x411f2210,
   0x10606119,
   0xc00f1620,
   0x69216533,
   0xc030a0e1,
   0xc0409910,
   0xb1119930,
   0x7100b0d1,
   0xa0d1b111,
   0xa0037291,
   0x7223a002,
   0x061f60a5,
   0x00f9306f,
   0x04411091,
   0x898a9971,
   0x1091061a,
   0x99710451,
   0x061b898b,
   0x14ba311b,
   0x147a3919,
   0x71006fa3,
   0x9643b120,
   0x700092ce,
   0xc1f0b118,
   0x78509440,
   0x721e9450,
   0x39878c87,
   0x99770677,
   0x16188988,
   0x455a1e07,
   0x6165c006,
   0x455e1e17,
   0x6165c006,
   0x45621e37,
   0x6165c036,
   0x44a31e77,
   0x61a3c336,
   0x959084a0,
   0x95a084b0,
   0x95b084c0,
   0x95c084d0,
   0xb016b006,
   0xb014b004,
   0xb012b002,
   0x90307810,
   0x78209050,
   0x90609040,
   0xb072b235,
   0x93f0c0b0,
   0xb0f6b136,
   0xb0737100,
   0xb136b127,
   0xb0e7a0f6,
   0x7100ba3e,
   0xb041b127,
   0xc0f0b061,
   0xba3f93f0,
   0x87208b14,
   0x31141404,
   0xa0449704,
   0xb1277100,
   0xb06db04d,
   0xb231b074,
   0xb1277100,
   0x9a007000,
   0x220089f0,
   0xb9e0459e,
   0xd0407000,
   0x659d9a17,
   0x6f10e471,
   0xc1009750,
   0xc7d2e481,
   0x6e236f13,
   0x16121611,
   0x1e0769ac,
   0x6566420e,
   0xb06fb04f,
   0xc004c0f5,
   0xb76065fe,
   0x39208780,
   0x49bf1c54,
   0x161491c0,
   0x1e018181,
   0x162141b8,
   0x4db81c41,
   0x797ea0e7,
   0x979e798f,
   0x120497af,
   0x8780b760,
   0x16143920,
   0x41d41c54,
   0x61cc91c0,
   0xc050a235,
   0xa0e7659d,
   0x7206a0e4,
   0x72047202,
   0x72047203,
   0x73067305,
   0x72917391,
   0x107060a5,
   0x71001209,
   0xba3db127,
   0x31838b63,
   0x20063d83,
   0x0bf341ee,
   0x69e51439,
   0x16293c89,
   0x1e893d29,
   0xc07949f7,
   0xc00361fc,
   0x1c391a83,
   0x10394dfc,
   0x700006f9,
   0xc29a65e3,
   0x6fab149a,
   0x149ac39a,
   0xc19a6fac,
   0x149a65e3,
   0x14db6fad,
   0x979c14dc,
   0x700097ab,
   0x659dc060,
   0xb04f6566,
   0xc0f5b06f,
   0x7100c004,
   0x7100b127,
   0x89acb127,
   0x979c89bb,
   0xb76097ab,
   0x39208780,
   0x4a231c54,
   0x161491c0,
   0x1e018181,
   0x16214215,
   0x4e151c41,
   0x797ea0e7,
   0x979e798f,
   0x120497af,
   0x8780b760,
   0x16143920,
   0x42381c54,
   0x623091c0,
   0xc070a235,
   0xa0e7659d,
   0x7206a0e4,
   0x72047202,
   0x72047203,
   0x73067305,
   0x72917391,
   0x000160a5,
   0x00080018,
   0x001a0003,
   0x002c000a,
   0x003e0011,
   0x00080003,
   0x001a0018,
   0x002c0011,
   0x003e000a,
   0xc8018630,
   0x04103151,
   0x70009630
};

PATCH_FUN_SPEC void rf_patch_mce_wb_dsss(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 303; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchWbDsssMce[i];
   }
#else
   const uint32_t *pS = patchWbDsssMce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 37;

   do {
      t1 = *pS++;
      t2 = *pS++;
      t3 = *pS++;
      t4 = *pS++;
      t5 = *pS++;
      t6 = *pS++;
      t7 = *pS++;
      t8 = *pS++;
      *pD++ = t1;
      *pD++ = t2;
      *pD++ = t3;
      *pD++ = t4;
      *pD++ = t5;
      *pD++ = t6;
      *pD++ = t7;
      *pD++ = t8;
   } while (--nIterations);

   t1 = *pS++;
   t2 = *pS++;
   t3 = *pS++;
   t4 = *pS++;
   t5 = *pS++;
   t6 = *pS++;
   t7 = *pS++;
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
   *pD++ = t4;
   *pD++ = t5;
   *pD++ = t6;
   *pD++ = t7;
#endif
}
