/******************************************************************************
*  Filename:       rf_patch_cpe_ieee_802_15_4.c
*
*  Description: RF core patch for IEEE 802.15.4-2006 support ("IEEE" API command set) in CC13x2 and CC26x2
*
*  Copyright (c) 2015-2021, Texas Instruments Incorporated
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
//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

#include "rf_patch_cpe_ieee_802_15_4.h"

#ifndef CPE_PATCH_TYPE
#define CPE_PATCH_TYPE static const uint32_t
#endif

#ifndef SYS_PATCH_TYPE
#define SYS_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC static
#endif

#ifndef _APPLY_PATCH_TAB
#define _APPLY_PATCH_TAB
#endif


CPE_PATCH_TYPE patchImageIeee802154[] = {
   0x2100405d,
   0x21004089,
   0x21004095,
   0x2100413b,
   0x79654c07,
   0xf809f000,
   0x40697961,
   0xd5030749,
   0x4a042101,
   0x60110389,
   0xb570bd70,
   0x47084902,
   0x21000380,
   0x40041108,
   0x0000592d,
   0xf80af000,
   0x47004800,
   0x00007f57,
   0xf80af000,
   0x47004800,
   0x0000881b,
   0x490c6b80,
   0x0f000700,
   0x47707148,
   0x490a4a09,
   0x79502318,
   0x7e4956cb,
   0x428118c0,
   0x4608dd01,
   0x280fe002,
   0x200fdd00,
   0x090989d1,
   0x43010109,
   0x477081d1,
   0x210002e0,
   0x21000088,
   0x4b272201,
   0x48252102,
   0xb5104718,
   0x47804825,
   0x6a404825,
   0xd10d2800,
   0x200a4924,
   0x28005608,
   0x1c40da09,
   0x4608d007,
   0x6a403820,
   0x42884920,
   0x4780d100,
   0xf7ffbd10,
   0xbd10ffe3,
   0x4b1e2100,
   0x46084a1c,
   0x20014718,
   0x0240491c,
   0x481c6008,
   0x780122fb,
   0x70014011,
   0x38ec4812,
   0xb5104700,
   0x47884918,
   0x39204911,
   0xd0072801,
   0xd5040402,
   0x85082000,
   0x62484814,
   0xbd102001,
   0x4b096a4a,
   0x429a3bec,
   0x4a11d101,
   0x4907e005,
   0x4b106a4a,
   0xd1f2429a,
   0x624a4a0f,
   0x0000bd10,
   0x00000806,
   0x0000069f,
   0x000292a1,
   0x21000108,
   0x21000154,
   0x00029263,
   0x210040e7,
   0x0002b4b5,
   0xe000e180,
   0x21000380,
   0x000296f7,
   0x210040dd,
   0x21004123,
   0x00029569,
   0x21004119,
};
#define _NWORD_PATCHIMAGE_IEEE_802_15_4 90

#define _NWORD_PATCHCPEHD_IEEE_802_15_4 0

#define _NWORD_PATCHSYS_IEEE_802_15_4 0



#ifndef _IEEE_802_15_4_SYSRAM_START
#define _IEEE_802_15_4_SYSRAM_START 0x20000000
#endif

#ifndef _IEEE_802_15_4_CPERAM_START
#define _IEEE_802_15_4_CPERAM_START 0x21000000
#endif

#define _IEEE_802_15_4_SYS_PATCH_FIXED_ADDR 0x20000000

#define _IEEE_802_15_4_PATCH_VEC_ADDR_OFFSET 0x03D0
#define _IEEE_802_15_4_PATCH_TAB_OFFSET 0x03D4
#define _IEEE_802_15_4_IRQPATCH_OFFSET 0x0480
#define _IEEE_802_15_4_PATCH_VEC_OFFSET 0x404C

#define _IEEE_802_15_4_PATCH_CPEHD_OFFSET 0x04E0

#ifndef _IEEE_802_15_4_NO_PROG_STATE_VAR
static uint8_t bIeee802154PatchEntered = 0;
#endif

PATCH_FUN_SPEC void enterIeee802154CpePatch(void)
{
#if (_NWORD_PATCHIMAGE_IEEE_802_15_4 > 0)
   uint32_t *pPatchVec = (uint32_t *) (_IEEE_802_15_4_CPERAM_START + _IEEE_802_15_4_PATCH_VEC_OFFSET);

   memcpy(pPatchVec, patchImageIeee802154, sizeof(patchImageIeee802154));
#endif
}

PATCH_FUN_SPEC void enterIeee802154CpeHdPatch(void)
{
#if (_NWORD_PATCHCPEHD_IEEE_802_15_4 > 0)
   uint32_t *pPatchCpeHd = (uint32_t *) (_IEEE_802_15_4_CPERAM_START + _IEEE_802_15_4_PATCH_CPEHD_OFFSET);

   memcpy(pPatchCpeHd, patchCpeHd, sizeof(patchCpeHd));
#endif
}

PATCH_FUN_SPEC void enterIeee802154SysPatch(void)
{
}

PATCH_FUN_SPEC void configureIeee802154Patch(void)
{
   uint8_t *pPatchTab = (uint8_t *) (_IEEE_802_15_4_CPERAM_START + _IEEE_802_15_4_PATCH_TAB_OFFSET);


   pPatchTab[76] = 0;
   pPatchTab[140] = 1;
   pPatchTab[150] = 2;
   pPatchTab[164] = 3;
}

PATCH_FUN_SPEC void applyIeee802154Patch(void)
{
#ifdef _IEEE_802_15_4_NO_PROG_STATE_VAR
   enterIeee802154SysPatch();
   enterIeee802154CpePatch();
#else
   if (!bIeee802154PatchEntered)
   {
      enterIeee802154SysPatch();
      enterIeee802154CpePatch();
      bIeee802154PatchEntered = 1;
   }
#endif
   enterIeee802154CpeHdPatch();
   configureIeee802154Patch();
}

void refreshIeee802154Patch(void)
{
   enterIeee802154CpeHdPatch();
   configureIeee802154Patch();
}

void cleanIeee802154Patch(void)
{
#ifndef _IEEE_802_15_4_NO_PROG_STATE_VAR
   bIeee802154PatchEntered = 0;
#endif
}

void rf_patch_cpe_ieee_802_15_4(void)
{
   applyIeee802154Patch();
}


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


