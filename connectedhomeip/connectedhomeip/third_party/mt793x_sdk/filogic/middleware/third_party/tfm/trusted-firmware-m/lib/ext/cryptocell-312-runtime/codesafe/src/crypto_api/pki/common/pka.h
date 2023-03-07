/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef PKA_H
#define PKA_H

#include "cc_pal_types.h"
#include "cc_error.h"
#include "pka_hw_defs.h"
#include "cc_pka_defs_hw.h"
#include "pki_dbg.h"


#ifdef __cplusplus
extern "C"
{
#endif


/* values for defining, that PKA entry is not in use */
#define PKA_SIZE_ENTRY_NOT_USED  0xFFFFFFFF
#define PKA_ADDRESS_ENTRY_NOT_USED  0xFFC

/* define result discard value */
#define RES_DISCARD  0x3F


#define SWAP_INT8(x,y)  { \
   uint32_t  temp; \
   temp = (x); x = (y); y = temp; \
}



/* if you want to execute operation using function defined in pki_dbg.c,
 then change the define of PKA_EXEC_OP_DEBUG to 1, else define it as empty.
 Make sure the library is compiled with flag DEBUG=1, so pki_dbg.c exists in library */
#define PKA_EXEC_OP_DEBUG 0
#if (PKA_EXEC_OP_DEBUG  && defined PKA_DEBUG && defined DEBUG)
    #define PKA_EXEC_OPERATION PkiDbgExecOperation
#else   // not debug mode
    #define PKA_EXEC_OPERATION(Opcode,lenId,isAImmed,OpA,isBImmed,OpB,ResDiscard,Res,Tag) { \
        uint32_t fullOpCode; \
        fullOpCode = PKA_SET_FULL_OPCODE((Opcode),(lenId),(isAImmed),(OpA),(isBImmed),(OpB),(ResDiscard),(Res),(Tag)); \
        PKA_WAIT_ON_PKA_PIPE_READY(); \
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, OPCODE), fullOpCode); \
    }
#endif

/*************************************************************************/
/* Macros for calling PKA operations (names according to operation issue */
/*************************************************************************/

/*----------------------------------*/
/*   1.  ADD - SUBTRACT operations  */
/*----------------------------------*/
/*  Add:   Res =  OpA + OpB  */
#define   PKA_ADD(lenId, Res, OpA, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_ADD,(lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  AddIm:  Res =  OpA + OpBIm  */
#define   PKA_ADD_IM(lenId, Res, OpA, OpBIm)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_ADD,(lenId), 0, (OpA), 1, (OpBIm), 0, (Res), 0 )
/*  Sub:  Res =  OpA - OpB  */
#define   PKA_SUB(lenId, Res, OpA, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_SUB,(lenId),0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  SubIm:  Res =  OpA - OpBIm  */
#define   PKA_SUB_IM(lenId, Res, OpA, OpBIm)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_SUB,(lenId), 0, (OpA), 1, (OpBIm), 0, (Res), 0 )
/*  Neg:  Res =  0 - OpB  */
#define   PKA_NEG(lenId, Res, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_SUB, (lenId), 1, 0, 0, (OpB), 0, (Res), 0 )
/*  ModAdd:  Res =  (OpA + OpB) mod N  */
#define   PKA_MOD_ADD(lenId, Res, OpA, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODADD, (lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  ModAddIm:  Res =  (OpA + OpBIm) mod N  */
#define   PKA_MOD_ADD_IM(lenId, Res, OpA, OpBIm)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODADD, (lenId), 0, (OpA), 1, (OpBIm), 0, (Res), 0 )
/*  ModSub:  Res =  (OpA - OpB) mod N  */
#define   PKA_MOD_SUB(lenId, Res, OpA, OpB)   \
             PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODSUB, (lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  ModSubIm:  Res =  (OpA - OpBIm) mod N  */
#define   PKA_MOD_SUB_IM(lenId, Res, OpA, OpBIm)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODSUB, (lenId), 0, (OpA), 1, (OpBIm), 0, (Res), 0 )
/*  ModNeg:  Res =  (0 - OpB) mod N  */
#define   PKA_MOD_NEG(lenId, Res, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODSUB, (lenId), 1, 0, 0, (OpB), 0, (Res), 0 )


/*----------------------------------*/
/*   2.  Logical   operations       */
/*----------------------------------*/

/*  AND:  Res =  OpA & OpB  */
#define   PKA_AND(lenId, Res, OpA, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_AND, (lenId), 0, (OpA), 0, (OpB), 0, (Res) , 0 )
/*  AndIm:  Res =  OpA & OpB  */
#define   PKA_AND_IM(lenId, Res, OpA, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_AND, (lenId), 0, (OpA), 1, (OpB), 0, (Res) , 0 )
/*  Tst0:  OpA & 0x1 -  tests the bit 0 of operand A. If bit0 = 0, then ZeroOfStatus = 1, else 0  */
#define   PKA_TEST_BIT0(lenId, OpA)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_AND, (lenId), 0, (OpA), 1, 0x01, 1, RES_DISCARD , 0 )
/*  TstBit:  OpA & (1<<i) -  tests the bit i of operand A. If biti = 0, then ZeroOfStatus = 1, else 0  */
#define   PKA_TEST_BIT(lenId, OpA, i)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_AND, (lenId), 0, (OpA), 1, 0x01<<(i), 1, RES_DISCARD , 0 )
/*  Clr0:  Res =  OpA & (-2)  - clears the bit 0 of operand A.  Note:  -2 = 0x1E  for 5-bit size */
#define   PKA_CLEAR_BIT0(lenId, Res, OpA)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_AND, (lenId), 0, (OpA), 1, 0x1E, 0, (Res), 0 )
/*  Clr:  Res =  OpA & 0  - clears the operand A.  */
#define   PKA_CLEAR(lenId, OpA)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_AND, (lenId), 0, (OpA), 1, 0x00, 0, (OpA), 0 )
/*  Clear:  for full clearing the actual register opA, this macro calls Clr operation twice.  */
#define   PKA_2CLEAR(lenId, OpA)   \
            PKA_CLEAR(lenId, OpA);  \
            PKA_CLEAR(lenId, OpA)
/*  OR:  Res =  OpA || OpB  */
#define   PKA_OR(lenId, Res, OpA, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_OR, (lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  OrIm:  Res =  OpA || OpB  */
#define   PKA_OR_IM(lenId, Res, OpA, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_OR, (lenId), 0, (OpA), 1, (OpB), 0, (Res), 0 )
/*  Copy:  OpDest =  OpSrc || 0  */
#define   PKA_COPY(lenId, OpDest, OpSrc)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_OR, (lenId), 0, (OpSrc), 1, 0x00, 0, (OpDest), 0 )
/*  Set0:  Res =  OpA || 1  : set bit0 = 1, other bits are not changed */
#define   PKA_SET_BIT0(lenId, Res, OpA)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_OR, (lenId), 0, (OpA), 1, 0x01, 0, (Res), 0 )
/*  Xor:  Res =  OpA ^ OpB  */
#define   PKA_XOR(lenId, Res, OpA, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_XOR, (lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  XorIm:  Res =  OpA ^ OpB  */
#define   PKA_XOR_IM(lenId, Res, OpA, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_XOR, (lenId), 0, (OpA), 1, (OpB), 0, (Res), 0 )
/*  Flip0:  OpA =  OpA || 1  - inverts the bit 0 of operand A  */
#define   PKA_FLIP_BIT0(lenId, Res, OpA)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_XOR, (lenId), 0, (OpA), 1, 0x01, 0, (Res), 0 )
/*  Invert:  Res =  OpA ^ 0xFFF.FF  :  inverts all bits of OpA .
                    Note: 0xFFFFF =  0x1F for 5 bits size of second operand */
#define   PKA_INVERT_BITS(lenId, Res, OpA)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_XOR, (lenId), 0, (OpA), 1, 0x1F, 0, (Res), 0 )
/*  Compare:  OpA ^ OpB . Rsult of compare in ZeroBitOfStatus:  If OpA == OpB then Z = 1 */
#define   PKA_COMPARE(lenId, OpA, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_XOR, (lenId), 0, (OpA), 0, (OpB), 1, (0), 0 )
/*  CompareImmediate:  OpA ^ OpB . Rsult of compare in ZeroBitOfStatus:  If OpA == OpB then status Z = 1 */
#define   PKA_COMPARE_IM(lenId, OpA, OpBim)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_XOR, (lenId), 0, (OpA), 1, (OpBim), 1, (0), 0 )


/*----------------------------------*/
/*   3.  SHIFT    operations  - S <= 31      */
/*----------------------------------*/

/*  SHR0:  Res =  OpA >> (S+1) :   shifts right operand A by S+1 bits, insert 0 to left most bits */
#define   PKA_SHR_FILL0(lenId, Res, OpA, S)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_SHR0, (lenId), 0, (OpA), 0, (S), 0, (Res), 0 )
/*  SHR1:  Res =  OpA >> (S+1) :   shifts right operand A by S+1 bits, insert 1 to left most bits */
#define   PKA_SHR_FILL1(lenId, OpA, S, Res)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_SHR1, (lenId), 0, (OpA), 0, (S), 0, (Res), 0 )
/*  SHL0:  Res =  OpA << (S+1) :   shifts left operand A by S+1 bits, insert 0 to right most bits */
#define   PKA_SHL_FILL0(lenId, Res, OpA, S)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_SHL0, (lenId), 0, (OpA), 0, (S), 0, (Res), 0 )
/*  SHL1:  Res =  OpA << (S+1) :   shifts left operand A by S+1 bits, insert 1 to right most bits */
#define   PKA_SHL_FILL1(lenId, OpA, S, Res)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_SHL1, (lenId), 0, (OpA), 0, (S), 0, (Res), 0 )


/*-----------------------------------------------------*/
/*   2.  Multiplication and other   operations         */
/*       Note:  See notes to PKA_ExecOperation */
/*-----------------------------------------------------*/

/*  RMul:  Res =  LowHalfOf(OpA * OpB), where size of operands and result is equaled to operation
           size, defined by lenId. Note: for receiving full result, the lenId must be set according
           to (sizeA + sizeB) and leading not significant bits of operands must be zeroed */
#define   PKA_MUL_LOW(lenId, Res, OpA, OpB)   \
             PKA_EXEC_OPERATION( PKA_OPCODE_ID_MULLOW, (lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  HMul:  Res =  HighHalfOf(OpA * OpB) + one high word of low half of (OpA * OpB), where size of
           operands is equaled to operation size, defined by lenId. Note: Size of operation result
           is by one word large, than operation size */
#define   PKA_MUL_HIGH(lenId, Res, OpA, OpB)   \
             PKA_EXEC_OPERATION( PKA_OPCODE_ID_MULHIGH, (lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  ModMul:  Res =  OpA * OpB  mod N - modular multiplication */
#define   PKA_MOD_MUL(lenId, Res, OpA, OpB)   \
             PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODMUL, (lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  ModMulN:  Res =  OpA * OpB  mod N - modular multiplication (final reduction is omitted)*
*   up to PKA_EXTRA_BITS extra bits                               */
#define   PKA_MOD_MUL_NFR(lenId, Res, OpA, OpB)   \
             PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODMULN, (lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  ModMulAcc:  Res =  OpA * OpB + OpC mod N - modular multiplication and     *
*   adding, result reduced                                */
#define   PKA_MOD_MUL_ACC(lenId, Res, OpA, OpB, OpC)   \
             PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODMLAC, (lenId), 0, (OpA), 0, (OpB), 0, (Res), (OpC) )
/*  ModMulAccN:  Res =  OpA * OpB + OpC mod N - modular multiplication and    *
*   acdding (final reduction is omitted) -  up to PKA_EXTRA_BITS extra bits                       */
#define   PKA_MOD_MUL_ACC_NFR(lenId, Res, OpA, OpB, OpC)   \
             PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODMLACNR, (lenId), 0, (OpA), 0, (OpB), 0, (Res), (OpC) )
/*  ModExp:  Res =  OpA ** OpB  mod N - modular exponentiation */
#define   PKA_MOD_EXP(lenId, Res, OpA, OpB)   \
             PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODEXP, (lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  Divide:  Res =  OpA / OpB , OpA = OpA mod OpB - division,  */
#define   PKA_DIV(lenId, Res, OpA, OpB) \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_DIVISION, (lenId), 0, (OpA), 0, (OpB), 0, (Res), 0 )
/*  ModInv:  Modular inversion: calculates   Res = 1/OpB mod N  */
#define   PKA_MOD_INV(lenId, Res, OpB)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_MODINV, (lenId), 1,1, 0,(OpB), 0,(Res), 0 )
/* Modular reduction: Res = OpB mod B  by subtract the modulus B     *
*   times, while Res > B. Counter C should be set in the Tag bits of Status   */
#define   PKA_REDUCE(lenId, Res, OpA)   \
            PKA_EXEC_OPERATION( PKA_OPCODE_ID_REDUCTION, (lenId), 0, (OpA), 0, 0/*opB not need*/, 0, (Res), 0/*Tag*/ )


/*************************************************
  **************  second Level macros ************
**************************************************/

 /* mod inversion using exponentiation, used when 'a' can be even number, but *
 *  runs at constant time                                                     */
#define PKA_MOD_INV_W_EXP(res,a,nm2)    {\
    PKA_SUB_IM(LEN_ID_N_PKA_REG_BITS,(nm2), 0/*n*/, 2); \
    PKA_MOD_EXP(LEN_ID_N_BITS,(res),(a),(nm2)); \
}

#define PKA_SET_VAL(a,v) {\
    PKA_AND_IM(LEN_ID_N_PKA_REG_BITS,a,a,0); \
    PKA_OR_IM( LEN_ID_N_PKA_REG_BITS,a,a,v); \
}

#define PKA_COMPARE_STATUS(lenId, a, b, stat) {\
    PKA_COMPARE(lenId,a,b); \
    PKA_GET_STATUS_ALU_OUT_ZERO(stat);\
}

#define PKA_COMPARE_IM_STATUS(lenId, a, b,stat)  {\
    PKA_COMPARE_IM(lenId,a,b); \
    PKA_GET_STATUS_ALU_OUT_ZERO(stat);\
}

#define PKA_READ_BIT0(lenId,reg,bitVal) {\
    PKA_TEST_BIT0(lenId,reg); \
    PKA_GET_STATUS_ALU_OUT_ZERO(bitVal); \
    (bitVal) = !(bitVal); \
}

/*uint32 b - bit i value, i-num. of LS bit, i <= 31*/
#define PKA_READ_BIT(bitVal,reg,i) {\
    PKA_TEST_BIT(1/*lenId*/,reg,(i),0); \
    PKA_GET_STATUS_ALU_OUT_ZERO((bitVal)); \
    (bitVal) = !(bitVal); \
}

#define PKA_READ_WORD_FROM_REG(val,i,virtReg) {\
    uint32_t addr; \
    PKA_GET_REG_ADDRESS(virtReg, addr);\
    PKA_HW_READ_VALUE_FROM_PKA_MEM(addr+(i), val); \
}

#define PKA_WRITE_WORD_TO_REG(Val,i,VirtReg) {\
    uint32_t addr;\
    PKA_GET_REG_ADDRESS((VirtReg),addr);\
    PKA_HW_LOAD_VALUE_TO_PKA_MEM(addr+(i),(Val));\
}

CCError_t PkaExecFullModInv(int8_t         OpB,
                              int8_t         Res,
                              int8_t         rT0,
                              int8_t         rT1,
                              int8_t         rT2,
                              int8_t         rT3 );

uint8_t  PkaGetBitFromPkaReg(uint32_t   rX,
                 uint32_t   lenId,
                 int32_t    i,
                 uint32_t   rT);


void  PkaModDivideBy2(uint32_t    lenId,
              uint32_t    rX,
              uint32_t    rN,
              uint32_t    rRes);


CCError_t PkaInitAndMutexLock(uint32_t  sizeInBits,
                uint32_t *pkaRegCount);

void PkaFinishAndMutexUnlock(uint32_t pkaRegCount);

void PkaSetLenIds(uint32_t  sizeInBits,
        uint32_t lenId);

void PkaSetRegsSizesTab(uint32_t    opSizeInBits,
             int32_t     regSizeInPkaWords);


CCError_t PkaInitPka(uint32_t   opSizeInBits,
            uint32_t   regSizeInPkaWords,
            uint32_t   *pRegsCount);

#define PKA_INIT_PKA_DEFAULT(opSizeInBits) \
        PkaInitPka((opSizeInBits), 0 /*sizeInWords*/, NULL/*(pRegsCount)*/)


void PkaFinishPka(void);


CCError_t  PkaCalcNpIntoPkaReg(uint32_t lenId,
                uint32_t  sizeNbits,
                int8_t regN,
                int8_t regNp,
                int8_t regTemp1,
                int8_t regTempN);

void PkaClearBlockOfRegs(uint32_t  firstReg,
                         int32_t   countOfRegs,
                         uint32_t  lenId);

void PkaClearPkaRegWords(uint32_t    dstReg,
              uint32_t    sizeWords);

void PkaCopyByteBuffIntoPkaReg(uint32_t    dstReg,
                 uint32_t    lenId,
                 const uint8_t  *src_ptr,
                 uint32_t    size);

void PkaCopyBeByteBuffIntoPkaReg(uint32_t    dstReg,
                 uint32_t    lenId,
                 const uint8_t  *src_ptr,
                 uint32_t    sizeWords);

void PkaCopyDataIntoPkaReg(uint32_t    dstReg,
                 uint32_t    lenId,
                 const uint32_t  *src_ptr,
                 uint32_t    sizeWords);

void PkaCopyPkaRegIntoBeByteBuff(uint8_t *dst_ptr,
                  uint32_t  sizeWords,
                  uint32_t  srcReg);

void PkaCopyDataFromPkaReg( uint32_t *dst_ptr,
                  uint32_t  sizeWords,
                  uint32_t  srcReg);

uint32_t PkaGetRegEffectiveSizeInBits(uint32_t  reg);

bool PkaIsRegModEqual(uint32_t reg1, uint32_t reg2, uint32_t regTmp1, uint32_t regTmp2);


uint32_t  PkaGetNextMsBit(uint32_t r, int32_t i, uint32_t *pW, uint32_t *pIsNew);


uint32_t  PkaGet2MsBits(uint32_t r, int32_t i, uint32_t *pW, uint32_t *pIsNew);

#ifdef __cplusplus
}
#endif

#endif
