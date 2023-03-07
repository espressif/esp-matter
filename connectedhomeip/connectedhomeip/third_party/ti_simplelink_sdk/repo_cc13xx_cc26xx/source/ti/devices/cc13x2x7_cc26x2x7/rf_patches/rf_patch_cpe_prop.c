/******************************************************************************
*  Filename:       rf_patch_cpe_prop.c
*
*  Description: RF core patch for proprietary radio support ("PROP" API command set) in CC13x2 and CC26x2
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

#include "rf_patch_cpe_prop.h"

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


CPE_PATCH_TYPE patchImageProp[] = {
   0x2100406d,
   0x210040d7,
   0x21004099,
   0x21004105,
   0x21004111,
   0x2100411d,
   0x21004135,
   0x2100414d,
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
   0xf819f000,
   0x296cb2e1,
   0x2804d00b,
   0x2806d001,
   0x4910d107,
   0x07c97809,
   0x7821d103,
   0xd4000709,
   0x490d2002,
   0x210c780a,
   0xd0024211,
   0x2280490b,
   0xb003600a,
   0xb5f0bdf0,
   0x4909b083,
   0x20004708,
   0x47884908,
   0x78014804,
   0x43912240,
   0x48067001,
   0x00004700,
   0x210000c8,
   0x21000133,
   0xe000e200,
   0x00031641,
   0x000063f7,
   0x00031b23,
   0xf882f000,
   0x47004800,
   0x00007f57,
   0xf882f000,
   0x47004800,
   0x0000881b,
   0x781a4b09,
   0x43a22408,
   0xd0002800,
   0x701a4322,
   0x47104a00,
   0x00008e83,
   0x78084903,
   0xd4010700,
   0x47004802,
   0x00004770,
   0x21000380,
   0x00007e5f,
   0x20284a04,
   0x48044790,
   0x60412101,
   0x4a044803,
   0x47106041,
   0x0000424f,
   0x40045000,
   0x40046000,
   0x00004285,
   0x4d1fb570,
   0xb2c47828,
   0x4780481e,
   0x28037828,
   0x2c03d134,
   0x481cd032,
   0x0d406880,
   0x481a07c2,
   0x31604601,
   0x2a003080,
   0x241fd003,
   0x8845570c,
   0x241ee002,
   0x8805570c,
   0xd01f2c00,
   0x4a154813,
   0x79006941,
   0x10484341,
   0x69494911,
   0x49101840,
   0x7f493940,
   0x05404790,
   0x42691540,
   0xdb0d4288,
   0xdc0b42a8,
   0x69994b0c,
   0x4602b288,
   0x43620c09,
   0x12520409,
   0xb2801880,
   0x61984308,
   0x0000bd70,
   0x210002e4,
   0x00004179,
   0x21000028,
   0x21000380,
   0x21000300,
   0x000081cb,
   0x40044040,
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
};
#define _NWORD_PATCHIMAGE_PROP 127

#define _NWORD_PATCHCPEHD_PROP 0

#define _NWORD_PATCHSYS_PROP 0

#define _IRQ_PATCH_0 0x21004171


#ifndef _PROP_SYSRAM_START
#define _PROP_SYSRAM_START 0x20000000
#endif

#ifndef _PROP_CPERAM_START
#define _PROP_CPERAM_START 0x21000000
#endif

#define _PROP_SYS_PATCH_FIXED_ADDR 0x20000000

#define _PROP_PATCH_VEC_ADDR_OFFSET 0x03D0
#define _PROP_PATCH_TAB_OFFSET 0x03D4
#define _PROP_IRQPATCH_OFFSET 0x0480
#define _PROP_PATCH_VEC_OFFSET 0x404C

#define _PROP_PATCH_CPEHD_OFFSET 0x04E0

#ifndef _PROP_NO_PROG_STATE_VAR
static uint8_t bPropPatchEntered = 0;
#endif

PATCH_FUN_SPEC void enterPropCpePatch(void)
{
#if (_NWORD_PATCHIMAGE_PROP > 0)
   uint32_t *pPatchVec = (uint32_t *) (_PROP_CPERAM_START + _PROP_PATCH_VEC_OFFSET);

   memcpy(pPatchVec, patchImageProp, sizeof(patchImageProp));
#endif
}

PATCH_FUN_SPEC void enterPropCpeHdPatch(void)
{
#if (_NWORD_PATCHCPEHD_PROP > 0)
   uint32_t *pPatchCpeHd = (uint32_t *) (_PROP_CPERAM_START + _PROP_PATCH_CPEHD_OFFSET);

   memcpy(pPatchCpeHd, patchCpeHd, sizeof(patchCpeHd));
#endif
}

PATCH_FUN_SPEC void enterPropSysPatch(void)
{
}

PATCH_FUN_SPEC void configurePropPatch(void)
{
   uint8_t *pPatchTab = (uint8_t *) (_PROP_CPERAM_START + _PROP_PATCH_TAB_OFFSET);
   uint32_t *pIrqPatch = (uint32_t *) (_PROP_CPERAM_START + _PROP_IRQPATCH_OFFSET);


   pPatchTab[76] = 0;
   pPatchTab[62] = 1;
   pPatchTab[64] = 2;
   pPatchTab[140] = 3;
   pPatchTab[150] = 4;
   pPatchTab[152] = 5;
   pPatchTab[151] = 6;
   pPatchTab[73] = 7;

   pIrqPatch[1] = _IRQ_PATCH_0;
}

PATCH_FUN_SPEC void applyPropPatch(void)
{
#ifdef _PROP_NO_PROG_STATE_VAR
   enterPropSysPatch();
   enterPropCpePatch();
#else
   if (!bPropPatchEntered)
   {
      enterPropSysPatch();
      enterPropCpePatch();
      bPropPatchEntered = 1;
   }
#endif
   enterPropCpeHdPatch();
   configurePropPatch();
}

void refreshPropPatch(void)
{
   enterPropCpeHdPatch();
   configurePropPatch();
}

void cleanPropPatch(void)
{
#ifndef _PROP_NO_PROG_STATE_VAR
   bPropPatchEntered = 0;
#endif
}

void rf_patch_cpe_prop(void)
{
   applyPropPatch();
}

#undef _IRQ_PATCH_0

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


