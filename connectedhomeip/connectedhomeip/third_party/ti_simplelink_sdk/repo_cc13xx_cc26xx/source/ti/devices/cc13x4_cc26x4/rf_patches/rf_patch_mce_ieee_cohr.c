/******************************************************************************
*  Filename:       rf_patch_mce_ieee_cohr.c
*
*  Description: RF core patch for IEEE 802.15.4 support ("IEEE" API command set) in CC13x4 and CC26x4
*
*  Copyright (c) 2021, Texas Instruments Incorporated
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
#include "rf_patch_mce_ieee_cohr.h"
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

MCE_PATCH_TYPE patchIeeeCohrMce[326] = { 
   0xf70360c8,
   0xc39b9b3a,
   0x2fca744a,
   0x079d0fcf,
   0x7f7f01e0,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x000000a7,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000003,
   0x0000001f,
   0x80000000,
   0x0004000c,
   0x000114c4,
   0x00000009,
   0x018a8000,
   0x0f900000,
   0x121d002e,
   0x00000a11,
   0x00000b60,
   0x00404010,
   0x001c0000,
   0x041e1e1e,
   0x00008004,
   0x00003c00,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x40200209,
   0x0000001f,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x10000000,
   0x000000a7,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000003,
   0x0000001f,
   0x80000000,
   0x0004000c,
   0x00010000,
   0x00000009,
   0x018a8000,
   0x0fb00000,
   0x111e002e,
   0x00000a11,
   0x00000b60,
   0x00404010,
   0x001c0000,
   0x041e1e1e,
   0x00008004,
   0x00003600,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x40200209,
   0x0000001f,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x10000000,
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
   0x720d7223,
   0x720f720e,
   0x72687210,
   0x7203a35d,
   0x73057204,
   0x73967306,
   0xc7c07296,
   0xb0d09010,
   0x6686c030,
   0xa0d07100,
   0x721bb110,
   0x10208162,
   0x06703952,
   0x16300020,
   0x14011101,
   0x60f76c01,
   0x61236108,
   0x6192615a,
   0x60f760f7,
   0x60f760f7,
   0x61236103,
   0x6192615a,
   0x60f760f7,
   0x60fa60f7,
   0x60fb1220,
   0x73111210,
   0x73137312,
   0x001081b1,
   0xb07091b0,
   0xc14160d7,
   0xc680c192,
   0x610c1820,
   0xc192c641,
   0x1820c680,
   0x6e236f13,
   0x16121611,
   0xc682690c,
   0x1820ca10,
   0x12034119,
   0x16126e23,
   0x78806916,
   0xc1409cd0,
   0x83309ce0,
   0x12209d00,
   0xb68c99e0,
   0x729660fa,
   0x91f0c130,
   0x1213b113,
   0xb0e8b128,
   0xb1287100,
   0x9233a0e8,
   0x1e008cd0,
   0x99804136,
   0xb111b960,
   0x7100b0d1,
   0xb002b012,
   0xb013b683,
   0xb111b003,
   0x7296a0d1,
   0x7100b0d3,
   0xa0d3b113,
   0xc0001000,
   0xc0209960,
   0xb0d19980,
   0x9960c030,
   0xb1117100,
   0x7296a0d1,
   0xa0037268,
   0x7268a002,
   0x73067305,
   0x72967396,
   0xc7c0a230,
   0x60fa9010,
   0x94c07810,
   0x960095e0,
   0x96409620,
   0x94d07820,
   0x961095f0,
   0x96509630,
   0x95c07830,
   0x95d07840,
   0x8240b68b,
   0x45732230,
   0x7100b0d5,
   0xa0d5b115,
   0x7296616b,
   0xc030a3a7,
   0xb00291e0,
   0xb006b004,
   0xb121b00d,
   0x7100b0e1,
   0xb00ca0e1,
   0xa00cb01c,
   0x22808160,
   0x78604187,
   0x78506188,
   0x90309050,
   0x90607870,
   0xb1219040,
   0x7100b0e1,
   0xa0e1b072,
   0xb01ca00c,
   0x13f28d00,
   0x04203962,
   0x13f28331,
   0x31523152,
   0x00100421,
   0x10019330,
   0x8460b235,
   0x84613180,
   0x39813181,
   0x94500010,
   0x06f58195,
   0x1217120e,
   0x12f01206,
   0xc1c09440,
   0xb4729440,
   0xb14cb011,
   0xae90b10c,
   0x1203ae91,
   0x12081204,
   0xcff0120a,
   0xb3a769bc,
   0xb0737100,
   0xb441a472,
   0xa10cb14c,
   0xb0e1c1a0,
   0x7100b121,
   0xa0e169c6,
   0xbea1b00c,
   0xc040be91,
   0x71006686,
   0xae91b07e,
   0xbea2be92,
   0xbbce7100,
   0x31808e60,
   0x3d803120,
   0x10013d20,
   0x18103140,
   0x13f23d20,
   0x04203962,
   0x04218331,
   0x04201410,
   0x83310bf2,
   0x14100421,
   0xbea19330,
   0xae92be91,
   0xbea17100,
   0xb07f7100,
   0x7100bea1,
   0xbea1b07f,
   0x8e5fbbce,
   0x42141e36,
   0x421b1e0f,
   0x42041e16,
   0x06f08190,
   0x45921c0f,
   0x621b1216,
   0x39408190,
   0x1c0f06f0,
   0x12054592,
   0x12361202,
   0x9070c300,
   0xc050b230,
   0x78906686,
   0x621b9450,
   0x91cf1647,
   0x1e008180,
   0x1c70421b,
   0x71004a29,
   0xb07fbea1,
   0x22008090,
   0xb13c44dc,
   0xae91b0fc,
   0xbe917100,
   0x663ba0fc,
   0xc06061f6,
   0xa2356686,
   0x72047203,
   0x73067305,
   0xa004a002,
   0x7268a006,
   0x73067305,
   0x72967396,
   0x9010c7c0,
   0xbbce60fa,
   0x10fb8e5f,
   0x4a411e8b,
   0xc0cc1a8b,
   0x18b0c080,
   0x11011630,
   0x6c011401,
   0x908c908c,
   0x908c908c,
   0x908c908c,
   0x908c908c,
   0x00000000,
   0x1000ba8e,
   0xb0828679,
   0xba8e1000,
   0xb0838673,
   0x1000b083,
   0x8674ba8e,
   0x4a671e8f,
   0x4e6e1c39,
   0x4e711c49,
   0x311a3118,
   0x1c397000,
   0x1c494a6e,
   0x31184a71,
   0x7000311a,
   0x311a3318,
   0x31186273,
   0x99c8331a,
   0x1e8e89de,
   0x99ca4e81,
   0x1e8e89de,
   0x70004e7c,
   0xc00ac008,
   0xb080b085,
   0xc0087000,
   0xb084c00a,
   0x7000b071,
   0x8a409a50,
   0x46872200,
   0x7000ba30
};

PATCH_FUN_SPEC void rf_patch_mce_ieee_cohr(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 326; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchIeeeCohrMce[i];
   }
#else
   const uint32_t *pS = patchIeeeCohrMce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 40;

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
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
   *pD++ = t4;
   *pD++ = t5;
   *pD++ = t6;
#endif
}