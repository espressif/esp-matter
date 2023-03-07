/******************************************************************************
*  Filename:       rf_patch_rfe_genook.h
*
*  Description: RF core patch for General OOK support in CC13x2 and CC26x2.
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
#include "rf_patch_rfe_genook.h"
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_types.h)

#ifndef RFE_PATCH_TYPE
#define RFE_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC
#endif

#ifndef RFC_RFERAM_BASE
#define RFC_RFERAM_BASE 0x2100C000
#endif

#ifndef RFE_PATCH_MODE
#define RFE_PATCH_MODE 0
#endif

RFE_PATCH_TYPE patchGenookRfe[383] = {
   0x00006122,
   0x11011000,
   0x004d1203,
   0x002e24f1,
   0x0a940018,
   0x003ffffe,
   0x00ff007f,
   0x000003ff,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x40300000,
   0x40014000,
   0x40074003,
   0x404f400f,
   0x41cf40cf,
   0x47cf43cf,
   0x3fcf4fcf,
   0x1fcf2fcf,
   0x00000fcf,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x9100c050,
   0xc0707000,
   0x70009100,
   0x00213182,
   0xb1109131,
   0x81017000,
   0xa100b101,
   0x91323182,
   0x9101b110,
   0x81411011,
   0x406d2241,
   0x700006f1,
   0xc0501025,
   0xc3f49100,
   0x1420c2b0,
   0x10316f03,
   0xc0220441,
   0x00213182,
   0xb1109131,
   0x10313963,
   0xc0820441,
   0x00213182,
   0xb1109131,
   0x10313963,
   0x3182c0a2,
   0x91310021,
   0x1050b110,
   0x14053115,
   0x70009255,
   0xc2b2645d,
   0x06311031,
   0x02c13161,
   0xc1126460,
   0x39211031,
   0x31510671,
   0x646002e1,
   0x645d7000,
   0x7100b054,
   0xb064a054,
   0x220080f0,
   0xc11140a4,
   0x6460c122,
   0x82b1645a,
   0x39813181,
   0x6460c0e2,
   0x68b5c300,
   0x1240645d,
   0xb03290b0,
   0x395382a3,
   0x64943953,
   0x68bfc360,
   0x90b01280,
   0x7000b032,
   0xc101645d,
   0x6460c122,
   0xc0c2c101,
   0x82a36460,
   0x12c06494,
   0xb03290b0,
   0x645d7000,
   0xc081c272,
   0xc1226460,
   0x6460c111,
   0xc111c002,
   0xc0626460,
   0x6460c331,
   0xc111c362,
   0xc3026460,
   0x6460c111,
   0x395382a3,
   0xc3e26494,
   0x22116465,
   0xc24240e7,
   0x6460c881,
   0xc111c252,
   0xc2726460,
   0x6460cee1,
   0xc881c202,
   0xc2026460,
   0x6460c801,
   0x68fbc170,
   0x645d7000,
   0xc801c242,
   0xc2526460,
   0x6460c011,
   0xc0e1c272,
   0xc0026460,
   0x6460c101,
   0xc301c062,
   0xc1226460,
   0x6460c101,
   0xc101c362,
   0xc3026460,
   0x6460c101,
   0x649482a3,
   0x00007000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x72057306,
   0x720e720b,
   0x7100b050,
   0xb0608081,
   0x8092a050,
   0x224180a2,
   0x80804543,
   0x0410c1f1,
   0x11011630,
   0x6c011401,
   0x615e615e,
   0x615e615e,
   0x615e615e,
   0x615e615e,
   0x615e615e,
   0x615e615e,
   0x809162d5,
   0x0421c0f2,
   0x80823121,
   0x14122a42,
   0x11011632,
   0x6c011421,
   0x617b616e,
   0x62d5616e,
   0x615e615f,
   0x615e615e,
   0x617b616e,
   0x62d5616e,
   0x615e615f,
   0x615e615e,
   0x64d16163,
   0x64fd657b,
   0x12106163,
   0x616690b0,
   0x9050c010,
   0x906078a0,
   0x1210720e,
   0x61269030,
   0x91a07850,
   0x92607860,
   0x92707870,
   0x92807880,
   0x92907890,
   0x90a0c1a0,
   0xa0bc6163,
   0xb060a0e1,
   0x80f0a054,
   0x45862250,
   0x22008040,
   0x617d46a2,
   0x66f7cff0,
   0x393080f0,
   0x22100630,
   0x7841418f,
   0x22006194,
   0x78314193,
   0x78216194,
   0x827d91e1,
   0x39408280,
   0x0410c0f1,
   0xc0121007,
   0x82693072,
   0x0419c0f1,
   0xc0f1826a,
   0x041a394a,
   0xc0f1826e,
   0x041e398e,
   0x10bc10ab,
   0x647210c2,
   0x78e7c00f,
   0xb003b013,
   0xb0536646,
   0xb013b050,
   0xc082661c,
   0x662d6682,
   0xb0637100,
   0x22018041,
   0x80f046a2,
   0x41b82250,
   0x45b82210,
   0x46c022f0,
   0x6682c082,
   0x392010f0,
   0x82239210,
   0x1030664c,
   0x4dd618d3,
   0x16130bf3,
   0x49ea1ce3,
   0x82339213,
   0x61db143b,
   0x49ea1ce3,
   0x82339213,
   0x1cab183b,
   0x1c9b4e29,
   0x1cbc4a2b,
   0x10b241ea,
   0x22d08260,
   0x80f041e8,
   0x45ea2210,
   0x65eb6472,
   0x10c061b8,
   0x49fd18b0,
   0x39101003,
   0x41f41e00,
   0x3807380f,
   0x420d2203,
   0x392010f0,
   0x1070180f,
   0x18073920,
   0x1003620d,
   0x1801c001,
   0x1e013911,
   0x301f4205,
   0x22033017,
   0x10f0420d,
   0x140f3920,
   0x39201070,
   0x66991407,
   0x06f08280,
   0x80f13110,
   0x06313931,
   0x42171e01,
   0xb0633810,
   0x6a177100,
   0x700010bc,
   0x06f08280,
   0x80f13110,
   0x06313931,
   0x42251e01,
   0xb0633810,
   0x6a257100,
   0x10ab7000,
   0x109b61df,
   0x10f261df,
   0x92123922,
   0x82518222,
   0x81a11812,
   0x82911812,
   0x3d813181,
   0x4a3e1c12,
   0xb032b0e1,
   0x66f7cfe0,
   0x1421c7f1,
   0xc8124e42,
   0x91c291b2,
   0x7000b031,
   0xc0061208,
   0x91b0c800,
   0x700091c0,
   0x10308251,
   0x81a11810,
   0x14061810,
   0x829280e1,
   0x3d823182,
   0x4a5e1c20,
   0x46682211,
   0xb032b0e1,
   0x66f7cfd0,
   0x42682211,
   0x1c201a32,
   0xa0e14e68,
   0xdfc0b032,
   0x66f79342,
   0x39418281,
   0x1e0106f1,
   0x16184273,
   0x3010c010,
   0x46811c08,
   0xc7f13c16,
   0x4e771461,
   0x91b6c816,
   0x318181c1,
   0x1c163d81,
   0x91c64a7e,
   0xc006b031,
   0x70001208,
   0x31238203,
   0x187110f1,
   0x10153c21,
   0x4e8b1c37,
   0x1037628d,
   0x1417628e,
   0x4a911c3f,
   0x103f6293,
   0x1e016299,
   0x12114696,
   0x42991e0f,
   0x10f1181f,
   0x39311471,
   0x063080e0,
   0x14103121,
   0x700090e0,
   0x81b28251,
   0x3d823182,
   0x9341efb0,
   0x66f79352,
   0x64fda003,
   0x81b16163,
   0x3d813181,
   0x39808290,
   0x1cf11801,
   0x14014abf,
   0x22c080b0,
   0xb0bc46bf,
   0xefa0b033,
   0x935f9341,
   0x700066f7,
   0xb063a003,
   0xb054b064,
   0x64fdb0e0,
   0x80407100,
   0x46a22200,
   0x64d1b064,
   0x7100a0e0,
   0x22008040,
   0xb06446a2,
   0xb003a054,
   0xcf9061b8,
   0x64a366f7,
   0xb054b0e1,
   0xb064645d,
   0x80f07100,
   0x46f22240,
   0x42e32210,
   0xc20162ee,
   0x6460c0c2,
   0x318080a0,
   0x6ae93940,
   0xc0c2c101,
   0x62db6460,
   0xc0c2c331,
   0x62db6460,
   0xcf80a054,
   0x64c466f7,
   0x93306163,
   0x22008320,
   0xb31046f8,
   0x00007000
};

PATCH_FUN_SPEC void rf_patch_rfe_genook(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 383; i++) {
      HWREG(RFC_RFERAM_BASE + 4 * i) = patchGenookRfe[i];
   }
#else
   const uint32_t *pS = patchGenookRfe;
   volatile unsigned long *pD = &HWREG(RFC_RFERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 47;

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
