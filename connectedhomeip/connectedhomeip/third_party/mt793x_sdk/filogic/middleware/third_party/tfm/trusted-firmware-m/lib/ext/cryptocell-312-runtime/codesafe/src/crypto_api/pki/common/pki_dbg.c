/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_mem.h"
#include "cc_common_math.h"
#include "pki.h"
#include "pka_ec_wrst.h"
#include "pka_error.h"
#include "pki_dbg.h"

/* temp buffers used for debugging of PKA operations */
#if defined PKA_DEBUG && defined DEBUG

uint32_t tempRes[PKA_MAX_REGISTER_SIZE_IN_32BIT_WORDS];
uint32_t tempRes1[PKA_MAX_REGISTER_SIZE_IN_32BIT_WORDS];

/***********     PkiDbgPrintReg  function     **********************/
/**
 * @brief The function prints label and PKA register as big endian bytes array.
 *
 * @author reuvenl (8/25/2013)
 *
 * @return  None
 */
void PkiDbgPrintReg(const char*  label,  /*!< [in] Label string. */
                   const uint32_t reg)   /*!< [in] Register virt. pointer. */
{
        uint32_t tmp[PKA_MAX_REGISTER_SIZE_IN_32BIT_WORDS] = {0};
        uint32_t sizeBits;
        uint32_t sizeBytes;
        uint32_t sizeWords;
        int32_t i;

        if (reg > PKA_REG_T1) {
                PKA_PRINTF("Can't print reg %d, reg is too big \n", reg);
                exit(1);
        }
        sizeBits = PkaGetRegEffectiveSizeInBits(reg);
        sizeBytes = CALC_FULL_BYTES(sizeBits);
        sizeWords = CALC_FULL_32BIT_WORDS(sizeBits);
        if ((sizeBytes > sizeof(tmp)) ||
            ((sizeBits == 0) && (reg < PKA_REG_T0))) {
                PKA_PRINTF("Can't print reg %d, size in %d\n", reg, sizeBytes);
        }
        PkaCopyDataFromPkaReg(tmp, sizeWords, reg);

        PKA_PRINTF("%s [%d] ", label, sizeBits);
        for (i=(sizeBytes - 1); i>=0; i--) {
                PKA_PRINTF("%02X", ((uint8_t*)tmp)[i] & 0xFF);
        }
        PKA_PRINTF("\n");
}


/***********     PkiDbgPrintUint32BuffAsNum  function     **********************/
/**
 * @brief This The function prints the label and 32-bit words buffer (LS-word is
 *  a left most) as a big hexadecimal number (MS-digit is a left most).
 *
 * @author reuvenl (8/25/2013)
 *
 * @return  None
 */
void PkiDbgPrintUint32BuffAsNum(const char     *label, /*!< [in] Label string. */
                     const uint32_t *pBuf,  /*!< [in] 32-bit words buffer to print. */
                     uint32_t  sizeWords)   /*!< [in] size of pBuff in 32-bit words. */
{
        uint32_t sizeBits;
        int32_t i;

        sizeBits = CC_CommonGetWordsCounterEffectiveSizeInBits(pBuf, sizeWords);

        PKA_PRINTF("%s [%d] ", label, sizeBits);
        for (i=(sizeWords - 1); i>=0; i--) {
                PKA_PRINTF("%08X", pBuf[i]);
        }
        PKA_PRINTF("\n");
}

/***********     PkiDbgPrintUint8BuffAsNum  function     **********************/
/**
 * @brief This The function prints the label and bytes array (LS-byte is
 *  a left most) as a big hexadecimal number (MS-digit is a left most).
 *
 * @author reuvenl (14/02/2016)
 *
 * @return  None
 */
void PkiDbgPrintUint8BuffAsNum(const char     *label, /*!< [in] Label string. */
                     const uint8_t *pBuf,  /*!< [in] 32-bit words buffer to print. */
                     uint32_t  sizeBytes)   /*!< [in] size of pBuff in 32-bit words. */
{
        uint32_t sizeBits;
        int32_t i;

        sizeBits = CC_CommonGetBytesCounterEffectiveSizeInBits(pBuf, sizeBytes);

        PKA_PRINTF("%s [%d] ", label, sizeBits);
        for (i=(sizeBytes - 1); i>=0; i--) {
                PKA_PRINTF("%02X", pBuf[i]);
        }
        PKA_PRINTF("\n");
}

/***********     PkiDbgPrintUintBuf  function     **********************/
/**
 * @brief This The function prints the label and bytes array as string.
 *
 * @author reuvenl (14/02/2016)
 *
 * @return  None
 */
void PkiDbgPrintUint8Buff(const char     *label, /*!< [in] Label string. */
                     const uint8_t *pBuf,  /*!< [in] 32-bit words buffer to print. */
                     uint32_t  sizeBytes)   /*!< [in] size of pBuff in 32-bit words. */
{
        uint32_t sizeBits;
        uint32_t i;

        sizeBits = CC_CommonGetBytesCounterEffectiveSizeInBits(pBuf, sizeBytes);

        PKA_PRINTF("%s [%d] ", label, sizeBits);
        for (i=0; i<sizeBytes; i++) {
                PKA_PRINTF("%02X", pBuf[i]);
        }
        PKA_PRINTF("\n");
}
#endif

#if defined PKA_DEBUG && defined DEBUG
/***********    PkiDbgExecOperation (with virtual pointers)     ******************/
/**
 * @brief This function executes any allowed PKA mathematic operation according to
 *        user passed Opcode.
 *
 *        The function receives code of operation, virtual pointers to PKI registers
 *        (sequence number), for arguments and result, and operates PKA machine by writing
 *        full operation code into OPCODE register. Then the function calls macros for
 *        waiting the PKA pipe ready signal.
 *        If opcode is illegal or one of operands is illegal, the function returns an
 *        error code defined in pka_error.h file.
 *
 *        The user don't call this function directly. For user convenience, in llf_pki.h file  are
 *        given some macros for calling this function according to each performed operation.
 *
 *     NOTES:
 *       -  Before executing modular operations, the modulus must be set into N=r0 register of PKA.
 *       -  Before modular multiplication and exponentiation must be calculated and set into NP=r1
 *          register the Barrett modulus tag NP = 2**(sizeN+132) / N.
 *       -  In operations with immediate operands (IsImmediate bit = 1), the operand value (5-bit)
 *          is treated as sign-extended. That means: low 4 bits are treated as unsigned operand
 *          value in range 0-15 and bit 5 is a sign (with extension to all high bits of register,
 *          in which the full operand shall be set).
 *       -  In shift operations the 5-bits shift operand is treated as unsigned value in range 0-31
 *          (count of shifts is equaled to shift operand value + 1).
 *       -  The LMul operation gives the low half of multiplication result of length equaled to
 *          operation size. The leading not significant bits of the operands and result (including
 *          the the extra word) must be zeroed.
 *       -  The HMul operation gives the high half of multiplication result plus one high word of low
 *          half of full multiplication result. Therefore this result is by one word large, than
 *          operation size. The leading not significant bits of the operands and result,
 *          including extra word must be zeroed.
 *       -  The ModInv operation calculates Res = 1/OpB mod N for odd modulus. Operand A is ignored.
 *          In case of even modulus the function returns an error. Therefore in this case
 *          (also for odd modulus) the user may call the PkaExecFullModInv function.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t PkiDbgExecOperation(uint32_t       Opcode,      /*!< [in] The operation code according HW PKA definitions. Valid values: 0 - max Opcode. */
                               uint32_t       lenId,        /*!< [in] ID of the length of operands according to register sizes table
                                    (means the number of entry in the table). Valid values: 0...7. */
                               uint32_t       isAImmed,     /*!< [in] If isBImmed = 1, then operand A treated as immediate value, else -
                                    as virtual register pointer. Valid values:0,1. */
                               uint32_t       OpA,          /*!< [in] Operand A: an immediate value or virtual register pointer,
                                    according to isAImmed parameter. */
                               uint32_t       isBImmed,     /*!< [in] If isBImmed = 1, then operand B treated as immediate value, else -
                                    as virtual register pointer. Valid values:0,1. */
                               uint32_t       OpB,          /*!< [in] Operand B: an immediate value or virtual register pointer,
                                    according to isBImmed parameter. */
                               uint32_t       ResDiscard,   /*!< [in] If ResDiscard = 1, then result is discarded. */
                               uint32_t       Res,          /*!< [Out] Virtual register pointer for result data.
                                    Value Res = RES_DISCARD means result must be discarded. */
                               uint32_t       Tag )         /*!< [in] The user defined value (Tag <= 31), used for indication goals. */

{
        CCError_t Error = CC_OK;
        uint32_t FullOpCode;
        uint32_t status;
        uint8_t  OpPrint;
        uint32_t opSizeInBits, RegSizeWords;

        /* if Res == RES_DISCARD , then result is discarded */
        if (Res == (int8_t)RES_DISCARD) {
                ResDiscard = 1;
                Res = 0;
        }

        /* set operation size according to lenId+1 for debug copy and clearing registers */
        if (lenId == LEN_ID_MAX_BITS) {
                PKA_GET_REG_SIZE(opSizeInBits, LEN_ID_MAX_BITS);
        } else if (lenId & 1) {
                PKA_GET_REG_SIZE(opSizeInBits, lenId);
        } else {
                PKA_GET_REG_SIZE(opSizeInBits, lenId+1);
        }

        RegSizeWords = PKA_WORD_SIZE_IN_32BIT_WORDS*((opSizeInBits+CC_PKA_WORD_SIZE_IN_BITS-1)/CC_PKA_WORD_SIZE_IN_BITS);

        /*************************************************/
        /*      check input parameters                   */
        /*************************************************/

        if (Opcode  > PKA_MAX_OPCODE) {
                Error =  PKA_ILLEGAL_OPCODE_ERROR;
                goto End;
        }

        if (lenId >= LEN_ID_MAX) {
                Error = PKA_ILLEGAL_OPERAND_LEN_ERROR;
                goto End;
        }

        if (isAImmed   > 1 ||
            isBImmed   > 1 ||
            ResDiscard > 1) {
                Error = PKA_ILLEGAL_OPERAND_TYPE_ERROR;
                goto End;
        }

        if ((OpA >  31)      ||
            (OpB >  31)      ||
            (Res >  31)      ||
            (Tag >  31)) {
                Error = PKA_ILLEGAL_OPERAND_ERROR;
                goto End;
        }

        /* for ModInv and Div operation check, that modulus or divider are not 0 */
        if (Opcode == PKA_OPCODE_ID_MODINV || Opcode == PKA_OPCODE_ID_DIVISION) {
                int8_t OpT;  /* number of register to test its Value = 0 */

                /* Set OpT: 0 - for ModInv, OpB - for Div */
                if (Opcode == PKA_OPCODE_ID_MODINV) {
                        Error = PKA_MODULUS_IS_NULL_ERROR;
                        OpT = 0;
                } else {
                        Error = PKA_DIVIDER_IS_NULL_ERROR;
                        OpT = OpB;
                }

                /* Create full opcode word for add immediate 0 operation */
                FullOpCode = PKA_SET_FULL_OPCODE(PKA_OPCODE_ID_ADD, lenId,0/*isAImmed*/,OpT/*N*/,1,0/*imm 0*/,1/*ResDiscard*/,0/*dumm*/,Tag);

                /* write full opcode into PKA OPCODE register */
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL,OPCODE), FullOpCode);

                /* test zero bit of STATUS register */
                PKA_GET_STATUS_ALU_OUT_ZERO(status);
                if (status == 1)
                        goto End;
                else
                        Error = CC_OK;
        }

        /* for ModInv operation check, that OpB is odd, else return Error (can't calculate,
           the user must use other function) */
        if (Opcode == PKA_OPCODE_ID_MODINV) {
                /* Create full opcode word for Test bit 0 operation */
                FullOpCode = PKA_SET_FULL_OPCODE(PKA_OPCODE_ID_AND, lenId, 0/*isAImmed*/, 0/*N*/, 1, 1/*imm 1*/, 1/*ResDiscard*/, 0/*dumm*/, Tag);

                /* write full opcode into PKA OPCODE register */
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, OPCODE), FullOpCode);

                /* test zero bit of STATUS register */
                PKA_GET_STATUS_ALU_OUT_ZERO(status);

                if (status == 1) {
                        Error = PKA_MOD_EVEN_USE_OTHER_FUNC_ERROR;
                        goto End;
                }
        }


        /*************************************************/
        /*      main PKI operation of this function      */
        /*************************************************/

        FullOpCode = PKA_SET_FULL_OPCODE(Opcode, lenId, isAImmed, OpA, isBImmed, OpB, ResDiscard, Res, Tag);
        PKA_WAIT_ON_PKA_PIPE_READY();
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, OPCODE), FullOpCode);


        /*************************************************/
        /* finishing operations for different cases      */
        /*************************************************/
        if (Opcode == PKA_OPCODE_ID_DIVISION) {
                /* for Div operation check, that OpB != 0, else return Error */
                PKA_GET_STATUS_DIV_BY_ZERO(status);
                if (status == 1) {
                        Error = PKA_DIVIDER_IS_NULL_ERROR;
                        goto End;
                }
        }

        /* wait for PKA done bit */
        PKA_WAIT_ON_PKA_DONE();

        /* if operation Tag = Print, then copy result into tempRes buffer */
        if (1/*Tag == PKA_TAG_DebugPtint*/ &&
            ResDiscard == 0 && Opcode != PKA_OPCODE_ID_TERMINATE && Opcode != PKA_OPCODE_ID_SEPINT) {
                CC_PalMemSetZero(tempRes, sizeof(tempRes));
                PkaCopyDataFromPkaReg(tempRes/*dst_ptr*/, RegSizeWords, Res/*srcReg*/);

                if (Opcode == PKA_OPCODE_ID_DIVISION || Opcode == PKA_OPCODE_ID_MODINV) {
                        if (Opcode == PKA_OPCODE_ID_DIVISION)
                                OpPrint = OpA;
                        else
                                OpPrint = OpB;

                        PkaCopyDataFromPkaReg(tempRes1/*dst_ptr*/, RegSizeWords, OpPrint/*srcReg*/);
                }
        }

        End:

        return Error;

}

#endif /* #if PKA_EXEC_OP_DEBUG */

