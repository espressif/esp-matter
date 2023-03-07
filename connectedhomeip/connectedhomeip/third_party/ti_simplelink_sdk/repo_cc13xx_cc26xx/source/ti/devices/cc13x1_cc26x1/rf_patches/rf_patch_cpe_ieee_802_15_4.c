/******************************************************************************
*  Filename:       rf_patch_cpe_ieee_802_15_4.c
*
*  Description:    RF Core patch for IEEE 802.15.4-2006 in CC26x1
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
   0x0000a125,
   0x0000a351,
   0x0000a221,
};
#define _NWORD_PATCHIMAGE_IEEE_802_15_4 3

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
#define _IEEE_802_15_4_PATCH_VEC_OFFSET 0x04E0

PATCH_FUN_SPEC void enterIeee802154CpePatch(void)
{
#if (_NWORD_PATCHIMAGE_IEEE_802_15_4 > 0)
   uint32_t *pPatchVec = (uint32_t *) (_IEEE_802_15_4_CPERAM_START + _IEEE_802_15_4_PATCH_VEC_OFFSET);

   memcpy(pPatchVec, patchImageIeee802154, sizeof(patchImageIeee802154));
#endif
}

PATCH_FUN_SPEC void enterIeee802154SysPatch(void)
{
}

PATCH_FUN_SPEC void configureIeee802154Patch(void)
{
   uint8_t *pPatchTab = (uint8_t *) (_IEEE_802_15_4_CPERAM_START + _IEEE_802_15_4_PATCH_TAB_OFFSET);


   pPatchTab[77] = 0;
   pPatchTab[12] = 1;
   pPatchTab[164] = 2;
}

PATCH_FUN_SPEC void applyIeee802154Patch(void)
{
   enterIeee802154SysPatch();
   enterIeee802154CpePatch();
   configureIeee802154Patch();
}

void refreshIeee802154Patch(void)
{
   enterIeee802154CpePatch();
   configureIeee802154Patch();
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


