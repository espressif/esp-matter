/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef PKI_DBG_H
#define PKI_DBG_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"
#include "pka_hw_defs.h"
#include "cc_pal_abort.h"


#ifdef GMP_DEBUG
#include "gmp-utils.h"
#endif

#if defined PKA_DEBUG && defined DEBUG

#include <stdio.h>
#include "cc_pal_abort.h"
#define PKA_PRINTF printf
#define ASSERT(x)  if(!(x)) {CC_PalAbort(x);}
#define PKA_ASSERT(x, string) if(!(x)) {CC_PalAbort(string);}

        /* if the value is defined, then debug printing is performed
        in the parts of code, where the global variable gDoDebugPrint
        is set to 1 in run time */
        //#define DBG_PRINT_ON_FAIL_ONLY 1
        //#define USE_GMP_TEST_DEBUG_BUFFER 1
        //#define ECC_DEBUG 1

        /* reference to temp buffers used for debugging of PKA operations */
        extern uint32_t tempRes[PKA_MAX_REGISTER_SIZE_IN_32BIT_WORDS];
        extern uint32_t tempRes1[PKA_MAX_REGISTER_SIZE_IN_32BIT_WORDS];

void PkiDbgPrintReg(
                  const char*  label,
                  const uint32_t reg);

void PkiDbgPrintUint32BuffAsNum(
                  const char     *label,
                  const uint32_t *pBuf,
                  uint32_t  sizeWords);

void PkiDbgPrintUint8BuffAsNum(const char     *label,
                     const uint8_t *pBuf,
                     uint32_t  sizeBytes);
void PkiDbgPrintUint8Buff(const char     *label,
                     const uint8_t *pBuf,
                     uint32_t  sizeBytes);

CCError_t PkiDbgExecOperation(uint32_t       Opcode,
            uint32_t       lenId,
            uint32_t       isAImmed,
            uint32_t       OpA,
            uint32_t       isBImmed,
            uint32_t       OpB,
            uint32_t       ResDiscard,
            uint32_t       Res,
            uint32_t       Tag);

/* Special debug prints */
#define PKA_START_FUNC printf("\n[ %s\n",  __func__)
#define PKA_FINISH_FUNC printf("] %s\n",  __func__)
#define PPR(reg) printf(#reg); PkiDbgPrintReg("=",reg)
#define PPB(buf, size) {printf(#buf); PkiDbgPrintUint32BuffAsNum("=", buf, size);}
/* macro for printing one PKA register */
#define PKI_DBG_PRINT_REG(label, reg) \
     do{PKA_PRINTF("%s",label);       \
        PkiDbgPrintReg(#reg"=",reg); } while (0)
/* macro for printing up to 4 PKA regs; if some regs not need,  *
*  then set value RES_DISCARD                                   */
#define PKI_DBG_PRINT_REGS(label, reg1, reg2, reg3, reg4) \
     do{PKA_PRINTF("%s",label);                           \
        if(reg1<32) PkiDbgPrintReg(#reg1"=",reg1);        \
        if(reg2<32) PkiDbgPrintReg(#reg2"=",reg2);        \
        if(reg3<32) PkiDbgPrintReg(#reg3"=",reg3);        \
        if(reg4<32) PkiDbgPrintReg(#reg4"=",reg4);} while(0)

/***************************************/
#else  // not PKA_DEBUG

#define PkiDbgPrintReg(label, reg)
#define PkiDbgPrintUint32BuffAsNum(label, pBuf, sizeWords)
#define PkiDbgPrintUint8BuffAsNum(label, pBuf, sizeBytes)
#define PkiDbgPrintUint8Buff(label, pBuf, sizeBytes)

#define PKA_PRINTF(format, ...)  do{}while(0)
#define ASSERT(x)  do{}while(0)
#define PKA_ASSERT(x, string)

#define PKA_START_FUNC
#define PKA_FINISH_FUNC
#define PPR(reg)
#define PPB(buf,size)
#define PKI_DBG_PRINT_REGS(label, reg1, reg2, res3, reg4)
#define PKI_DBG_PRINT_REG(label, reg)

#endif /* end of if/else PKA_DEBUG */


#ifdef __cplusplus
}
#endif

#endif
