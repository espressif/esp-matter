/******************************************************************************
*  Filename:       rf_patch_cpe_prop.c
*
*  Description:    RF Core patch for proprietary modes in CC26x1
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
   0x210004f9,
   0x0000a125,
   0x0000a29d,
   0x0000a30f,
   0x0000a351,
   0x0000a05d,
   0x4903b510,
   0x47882000,
   0x47804802,
   0x0000bd10,
   0x00006365,
   0x0000a075,
};
#define _NWORD_PATCHIMAGE_PROP 12

#define _NWORD_PATCHSYS_PROP 0

#define _IRQ_PATCH_0 0x0000a091


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
#define _PROP_PATCH_VEC_OFFSET 0x04E0

PATCH_FUN_SPEC void enterPropCpePatch(void)
{
#if (_NWORD_PATCHIMAGE_PROP > 0)
   uint32_t *pPatchVec = (uint32_t *) (_PROP_CPERAM_START + _PROP_PATCH_VEC_OFFSET);

   memcpy(pPatchVec, patchImageProp, sizeof(patchImageProp));
#endif
}

PATCH_FUN_SPEC void enterPropSysPatch(void)
{
}

PATCH_FUN_SPEC void configurePropPatch(void)
{
   uint8_t *pPatchTab = (uint8_t *) (_PROP_CPERAM_START + _PROP_PATCH_TAB_OFFSET);
   uint32_t *pIrqPatch = (uint32_t *) (_PROP_CPERAM_START + _PROP_IRQPATCH_OFFSET);


   pPatchTab[63] = 0;
   pPatchTab[77] = 1;
   pPatchTab[152] = 2;
   pPatchTab[151] = 3;
   pPatchTab[12] = 4;
   pPatchTab[65] = 5;

   pIrqPatch[1] = _IRQ_PATCH_0;
}

PATCH_FUN_SPEC void applyPropPatch(void)
{
   enterPropSysPatch();
   enterPropCpePatch();
   configurePropPatch();
}

void refreshPropPatch(void)
{
   enterPropCpePatch();
   configurePropPatch();
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


