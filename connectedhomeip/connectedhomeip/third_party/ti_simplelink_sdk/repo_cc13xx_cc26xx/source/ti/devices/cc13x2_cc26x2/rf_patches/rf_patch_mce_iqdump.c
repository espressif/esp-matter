/******************************************************************************
*  Filename:       rf_patch_mce_iqdump.h
*
*  Description: RF core patch for IQ-dump support in CC13x2 PG2.1 and CC26x2 PG2.1
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
#include "rf_patch_mce_iqdump.h"
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

MCE_PATCH_TYPE patchIqdumpMce[363] = {
   0x2fcf6030,
   0x00013f9d,
   0xff00003f,
   0x07ff0fff,
   0x0300f800,
   0x00068080,
   0x00170003,
   0x00003d1f,
   0x08000000,
   0x0000000f,
   0x00000387,
   0x00434074,
   0x00828000,
   0x06f00080,
   0x091e0000,
   0x00540510,
   0x00000007,
   0x00505014,
   0xc02f0000,
   0x017f0c30,
   0x00000000,
   0x00000000,
   0x00000000,
   0x0000aa00,
   0x66ca7223,
   0xa4e5a35d,
   0x73057303,
   0x73047203,
   0x72047306,
   0x72917391,
   0xffc0b008,
   0xa0089010,
   0x720e720d,
   0x7210720f,
   0x7100b0d0,
   0xa0d0b110,
   0x8162721b,
   0x39521020,
   0x00200670,
   0x11011630,
   0x6c011401,
   0x60886087,
   0x613e6104,
   0x60876087,
   0x60876087,
   0x60886087,
   0x61e46104,
   0x60876087,
   0x60876087,
   0x60886087,
   0x614e6104,
   0x60876087,
   0x60876087,
   0x60886087,
   0x62006104,
   0x60876087,
   0x60876087,
   0x60886087,
   0x61866104,
   0x60876087,
   0x60876087,
   0x61126088,
   0x1210614e,
   0x73117223,
   0x73137312,
   0x001081b1,
   0xb07091b0,
   0x607d6044,
   0x66d0c030,
   0xc2b2c0c1,
   0x1820c4e0,
   0x6e236f13,
   0x16121611,
   0x7830688e,
   0x78a099c0,
   0x94909480,
   0xc750c4f2,
   0x40a01820,
   0x6e231203,
   0x689d1612,
   0x999078b0,
   0xb63c7263,
   0x8190607d,
   0x81709640,
   0x2a703980,
   0x16111001,
   0x84b484a2,
   0xc0f5c0f3,
   0x1c01c200,
   0xc10040c9,
   0x40bf1c10,
   0x10134cc1,
   0x18301803,
   0x1a101a13,
   0x68bc3912,
   0x13f360c9,
   0x13f360c9,
   0xc1001015,
   0x1a151850,
   0x39141a10,
   0xb0e868c7,
   0xb1287100,
   0xb230a0e8,
   0x8990b910,
   0xb1119930,
   0x7100b0d1,
   0xb012b002,
   0xa0d1b111,
   0xb6307291,
   0xb013b003,
   0xb0e0722c,
   0xb1207100,
   0x92c08170,
   0xb1207100,
   0x22f08170,
   0x13f044e2,
   0x40ee1c03,
   0x964292c3,
   0xb1207100,
   0x964492c5,
   0xb1207100,
   0xa630b0e0,
   0xa0e17000,
   0x9910c030,
   0x9930c040,
   0xb0d1b111,
   0xb1117100,
   0x7291a0d1,
   0xa002a003,
   0x7000a230,
   0x73127311,
   0x66d0c040,
   0x91f0c100,
   0xb63364a5,
   0xb0d3b113,
   0xa0d37100,
   0x607d64f5,
   0x73127311,
   0x66d0c050,
   0x91f0c030,
   0xb0e8b634,
   0xb1287100,
   0xb230a0e8,
   0xb012b002,
   0xb013b003,
   0x92f01200,
   0xb0e1b121,
   0xb1217100,
   0x06208210,
   0x45261e20,
   0x66d0c060,
   0xb1217100,
   0x92f181d1,
   0x00000000,
   0x82120000,
   0x1e220622,
   0xc070412e,
   0xa63466d0,
   0x607d64f5,
   0xa0f0a0d2,
   0x7311a0f3,
   0x66447312,
   0x66d0c080,
   0xc035b0d2,
   0x9b757100,
   0xb074ba38,
   0x6148b112,
   0xa0f0a0d2,
   0x7311a0f3,
   0x66447312,
   0xc000c18b,
   0x120c91e0,
   0x786a1218,
   0x788e787d,
   0xb07410a9,
   0xc050b0d2,
   0x7100b112,
   0xc0906960,
   0xc03566d0,
   0x7100b112,
   0x8bf09b75,
   0x8ca165d9,
   0x41732201,
   0x1ca81080,
   0x12084572,
   0x65d01618,
   0x65d98c00,
   0x22018ca1,
   0x1090417e,
   0x1e091a19,
   0x10a9457e,
   0x818465d0,
   0x41661e04,
   0x1c4c14bc,
   0x61664eb3,
   0xa0f0a0d2,
   0x7311a0f3,
   0x66447312,
   0x120c721e,
   0xb0741205,
   0xc050b0d2,
   0x7100b112,
   0xc0a06992,
   0x789d66d0,
   0xb11289ce,
   0x8c907100,
   0x41a42200,
   0x22108230,
   0xb23145a4,
   0x66d0c0b0,
   0x8ab29a3d,
   0x3d823182,
   0x31808af0,
   0x18023d80,
   0x1e0e063e,
   0x1e2e41c6,
   0x1e3e41b8,
   0x105641bf,
   0x3d161426,
   0x61c71065,
   0x31261056,
   0x14261856,
   0x10653d26,
   0x105661c7,
   0x18563136,
   0x3d361426,
   0x61c71065,
   0x39761026,
   0x818491c6,
   0x41991e04,
   0x1c4c161c,
   0x61994eb3,
   0xc0b01001,
   0x391191c1,
   0x10001000,
   0x69d21000,
   0x31307000,
   0x1cd03d30,
   0x1ce04de0,
   0x700049e2,
   0x700010d0,
   0x700010e0,
   0x66d0c0c0,
   0xa0f0a0d2,
   0x7311a0f3,
   0x66447312,
   0xb0f0b130,
   0x80b07100,
   0x45f32200,
   0xb23161ee,
   0x66d0c0d0,
   0xa0f0b130,
   0xc035b0d2,
   0x9b757100,
   0xb074ba38,
   0x61fab112,
   0x66d0c0e0,
   0xa0f0a0d2,
   0x7311a0f3,
   0x66447312,
   0xc000c18b,
   0x120c91e0,
   0x786a1218,
   0x788e787d,
   0xb13010a9,
   0x7100b0f0,
   0x220080b0,
   0x62134618,
   0xb231b074,
   0x66d0c0f0,
   0xa0f0b130,
   0xc020b0d2,
   0x7100b112,
   0xc0356a20,
   0x7100b112,
   0x8bf09b75,
   0x8ca165d9,
   0x42312201,
   0x1ca81080,
   0x12084630,
   0x65d01618,
   0x65d98c00,
   0x22018ca1,
   0x1090423c,
   0x1e091a19,
   0x10a9463c,
   0x818465d0,
   0x42241e04,
   0x1c4c14bc,
   0x62244eb3,
   0x22308240,
   0xb0d5464c,
   0xb1157100,
   0x6244a0d5,
   0x66d0c100,
   0xb006b118,
   0xb004b016,
   0xb002b014,
   0x8440b012,
   0x04207842,
   0x39838173,
   0x94e32a73,
   0x1832c1f2,
   0x10213162,
   0x00123151,
   0x94400020,
   0x16101030,
   0x22103930,
   0x1220426a,
   0x10033150,
   0x16303180,
   0x12029380,
   0x22731204,
   0x84a0427c,
   0x89829970,
   0x84c01a82,
   0x89849970,
   0x627e1a84,
   0x42892263,
   0x997084b0,
   0x1a808980,
   0x84d01402,
   0x89809970,
   0x14041a80,
   0x84b06295,
   0x04107851,
   0x89829970,
   0x84d01a42,
   0x04107851,
   0x89849970,
   0x31521a44,
   0x39633154,
   0x16130633,
   0x38343832,
   0x39823182,
   0x00423184,
   0x84a09722,
   0x84b09590,
   0x84c095a0,
   0x84d095b0,
   0x781095c0,
   0x90509030,
   0x90407820,
   0xb2359060,
   0x9170cd90,
   0xa2357000,
   0x7100b112,
   0xb112a0d2,
   0x81b0ba3c,
   0x39248b54,
   0x00043184,
   0xc11091b4,
   0x739166d0,
   0x66ca7291,
   0x72027206,
   0x73057204,
   0x607d7306,
   0xc8018630,
   0x04103151,
   0x70009630,
   0x89f09a00,
   0x46d12200,
   0x7000b9e0
};

PATCH_FUN_SPEC void rf_patch_mce_iqdump(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 363; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchIqdumpMce[i];
   }
#else
   const uint32_t *pS = patchIqdumpMce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 45;

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
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
#endif
}
