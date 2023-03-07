/******************************************************************************
*  Filename:       rf_patch_mce_bt5.h
*
*  Description: RF core patch for Bluetooth 5 support ("BLE" and "BLE5" API command sets) in CC13x2 and CC26x2
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
#include "rf_patch_mce_bt5.h"
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

MCE_PATCH_TYPE patchBle5Mce[189] = { 
   0x2dee6039,
   0x2dc22dce,
   0x2fef0f54,
   0x5f970fef,
   0x5000179d,
   0x40000240,
   0x0000000c,
   0x00091488,
   0x000105f9,
   0x008b8000,
   0x0f500080,
   0x0a1d0000,
   0x00000590,
   0x04000005,
   0x00c17b20,
   0x000f0000,
   0x017f7f27,
   0x000c8000,
   0x14980000,
   0x05f90009,
   0x80000000,
   0x0080008b,
   0x00000f70,
   0x0590121d,
   0x00050000,
   0x7b200400,
   0x000000c1,
   0x7f26000f,
   0x720d017f,
   0x720f720e,
   0xa35d7210,
   0x654aa4e5,
   0x7100b0d0,
   0xb110a0d0,
   0x8162721b,
   0x39621020,
   0x00200630,
   0x11011630,
   0x6c011401,
   0x605c605c,
   0x605c605c,
   0x605c605c,
   0x605c605c,
   0x6065605c,
   0x60cd6083,
   0x605d1220,
   0x73111210,
   0x73137312,
   0x001081b1,
   0xb07091b0,
   0xc2216040,
   0xc302c5fb,
   0x1820c460,
   0x6e236f13,
   0x16121611,
   0x94eb686a,
   0x9c8378a3,
   0x1e008170,
   0xc030445c,
   0xc1f092b0,
   0xc63292d0,
   0x1820c9c0,
   0x12034081,
   0x16126e23,
   0xb63c687e,
   0xb005605c,
   0xc1007291,
   0xb12891f0,
   0xb113b120,
   0xb0e8b111,
   0xb1287100,
   0xb230a0e8,
   0x8c80b910,
   0xb1119930,
   0x84b484a2,
   0xb0d1c0f3,
   0xb0127100,
   0xb111b002,
   0x7291a0d1,
   0xb630722c,
   0xb003b013,
   0x7100b0e0,
   0x8170b120,
   0x819092c0,
   0x71009640,
   0x92c3b120,
   0x71009642,
   0x9644b120,
   0xb1207100,
   0xa630a0e0,
   0xb0d3b633,
   0xb1137100,
   0xc030a0d3,
   0xc0209910,
   0xb0d19930,
   0xb1117100,
   0x7291a0d1,
   0xa002a003,
   0x7305656e,
   0x73917306,
   0xa2307291,
   0x9010c7c0,
   0x64f3605c,
   0x65247860,
   0x68d1c5f0,
   0xb0d5652a,
   0xc031b115,
   0x80907100,
   0x45342250,
   0xa910b074,
   0xb9107393,
   0x7313720f,
   0xa0d5b231,
   0x8b40b35d,
   0x97003920,
   0xb234653f,
   0x6558c261,
   0x44ed22f3,
   0xb0786566,
   0x6546c2a3,
   0x6574c030,
   0x84b0605c,
   0x997095a0,
   0x100e8980,
   0x959084a0,
   0x89809970,
   0xc080140e,
   0xc080180e,
   0x8351180e,
   0x39213981,
   0x16310661,
   0x14101100,
   0x311e6c00,
   0x318e311e,
   0x972e398e,
   0x22308240,
   0xb0d54516,
   0xb1157100,
   0x610ea0d5,
   0x91e0c100,
   0x99107810,
   0x789a13e7,
   0xc540b4e5,
   0xb0029010,
   0xb006b004,
   0x78507000,
   0x90309050,
   0x90607870,
   0x70009040,
   0xb06cb235,
   0xb089b011,
   0xa0d1b4e5,
   0xb13da0d0,
   0x7000b0fd,
   0x8a439a31,
   0x31338a54,
   0x31343d53,
   0x96933d54,
   0xb05396a4,
   0xa4e560d4,
   0xb064a044,
   0x1e038183,
   0x7000413f,
   0x91179933,
   0xa35d7100,
   0x73917223,
   0x72037291,
   0x656e7204,
   0x9000d030,
   0x9010ffc0,
   0x7305a008,
   0x70007306,
   0x8940895b,
   0x312318b0,
   0x18838ca8,
   0x18131803,
   0xa9109933,
   0xb910b111,
   0x7000b0d1,
   0x416c1e08,
   0xb7447100,
   0x9938b111,
   0x70007100,
   0xc8018630,
   0x04103151,
   0x70009630,
   0x89f09a00,
   0x45752200,
   0x7000b9e0
};

PATCH_FUN_SPEC void rf_patch_mce_bt5(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 189; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchBle5Mce[i];
   }
#else
   const uint32_t *pS = patchBle5Mce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 23;

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
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
   *pD++ = t4;
   *pD++ = t5;
#endif
}
