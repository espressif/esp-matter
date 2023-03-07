/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT HOST_LOG_MASK_SECURE_BOOT

#include "rsa_pki_pka.h"
#include "rsa_bsv.h"
#include "secureboot_stage_defs.h"


/************************ Defines ******************************/

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/
/* SRAM address used for the PKA */
const uint32_t g_SramPkaAddr = 0;

/************************ Public Functions ******************************/

#ifdef PKA_DEBUG
  uint8_t tempRes[268];
#endif

/* ***************************************************************************** */
/* *********      RSA PKI PKA initialisation functions and macros      ********* */
/* ***************************************************************************** */

/***********      PkaSetRegsSizesTab function      **********************/
/**
 * @brief This function initializes the PKA registers sizes table.
 *
 *      The function sets sizes table as follows:
 *            -  tab[0] = MaxSizeBits; //maximal size, usually this is exact modulus size in bits
 *            -  tab[1] = Extended size with extra bits, aligned to big words.
 *            -  other entrie,
        uint32_t  Xs = PKA_SIZE_ENTRY_NOT_USED, means - not used.
 *
 * @param[in] opSizeInBits - Size of PKA operations (modulus) in bits. The value must be in interval
 *                          from defined Min. to Max. size bits.
 * @param[in] regSizeInPkaWords - Sise of registers in PKA big words (e.g. 128-bit words).
 *
 * @return - no return value
 *
 */
void RSA_PKA_SetRegsSizesTab( uint32_t     opSizeInBits,
                  int32_t      regSizeInPkaWords,
                  unsigned long  virtualHwBaseAddr)
{
    /* LOCAL DECLARATIONS */

    uint32_t  i;

    /* FUNCTION LOGIC */

    /* Set exact op. size */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(virtualHwBaseAddr, PKA_L0), opSizeInBits);
    /* Set register size (with extra bits) aligned to PKA big words */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(virtualHwBaseAddr, PKA_L0) + 4,
                  regSizeInPkaWords*RSA_PKA_BIG_WORD_SIZE_IN_BITS);

    /* remaining entries set to PKA_SIZE_ENTRY_NOT_USED for debugging goals */
    for (i = 2; i < RSA_PKA_MAX_COUNT_OF_REGS_SIZES; i++) {
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(virtualHwBaseAddr, PKA_L0) + 4*i, PKA_SIZE_ENTRY_NOT_USED);
    }

    return;

} /* END of the finction  PkaSetRegsSizesTab */


/***********      PkaSetRegsMapTab function      **********************/
/**
 * @brief This function initializes the PKA registers sizes table.
 *
 *   The function checks input parameters and sets the physical memory registers mapping-table
 *   according to parameters, passed by the user:
 *     - start address of register 0 is the start address of PKA data registers memory
 *       PKA_SRAM_REGS_MEM_OFFSET_WORDS (defined in pka_hw_defs.h file);
 *     - special registers are set as follows: N=0,NP=1,T0=30,T1=31;
 *     - all registers have the same size, equalled to given size;
 *
 * @param[in] countOfRegs  - The count of registeres, requirred by the user.
 * @param[in] regSizeInPkaWords - Sise of registers in PKA big words (e.g. 128-bit words).
 * @param[in] virtualHwBaseAddr - cc base address
 *
 * @return - no return value
 *
 */
void RSA_PKA_SetRegsMapTab(int32_t   countOfRegs,
               int32_t   regSizeInPkaWords,
               unsigned long  virtualHwBaseAddr)
{
    /* LOCAL DECLARATIONS */

    uint32_t  currentAddr;
    int32_t i;

    /* FUNCTION LOGIC */
    /* start addres of PKA mem. */
    currentAddr = RSA_PKA_SRAM_REGS_MEM_OFFSET_WORDS;

    /* set addresses of the user requested registers (excluding T0,T1) */
    for (i = 0; i < countOfRegs-2; i++) {
                SB_HAL_WRITE_REGISTER(SB_REG_ADDR(virtualHwBaseAddr, MEMORY_MAP0) +
                                      i*sizeof(uint32_t), currentAddr);
                currentAddr += regSizeInPkaWords*RSA_PKA_BIG_WORD_SIZE_IN_32_BIT_WORDS;
    }
    /* set addresses of 2 temp registers: T0=30, T1=31 */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(virtualHwBaseAddr, MEMORY_MAP0) + 30*sizeof(uint32_t), currentAddr);
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(virtualHwBaseAddr, MEMORY_MAP0) + 31*sizeof(uint32_t),
                  currentAddr + regSizeInPkaWords*RSA_PKA_BIG_WORD_SIZE_IN_32_BIT_WORDS);

    /* set default virtual addresses of N,NP,T0,T1 registers into N_NP_T0_T1_Reg */
    SB_HAL_WRITE_REGISTER(SB_REG_ADDR(virtualHwBaseAddr, N_NP_T0_T1_ADDR),
                  (uint32_t)RSA_HW_PKI_PKA_N_NP_T0_T1_REG_DEFAULT_VAL);

    return;

}  /* END of the finction  PkaSetRegsMapTab */


/***********      PkaInitPka function      **********************/
/**
 * @brief This function initializes the PKA engine.
 *
 *    The function performs the following:
 *      - initializes the PKA_SizesTable, PKA_MappingTable and special register
 *        N_NP_T0_T1 according to user passed register sizes, registers mapping
 *        and default N_NP_T0_T1 value.
 *
 *    The function calls the PkaSetRegsSizesTab  and PkaSetRegsMapTab
 *    functions and sets N_NP_T0_T1 value into N_NP_T0_T1 register.
 *    Notes:
 *            - See remarks to PkaSetRegsSizesTab and PkaSetRegsMapTab functions.
 *            - The function allocates one additional word for each register if it is needed for extra bits.
 *
 * @return CCError_t - On success CC_OK is returned, on failure an error code:
 *                       PKA_REGISTER_SIZES_ERROR
 *                       PKA_ENTRIES_COUNT_ERROR
 *                       PKA_NOT_ENOUGH_MEMORY_ERROR
 *
 */
void RSA_PKA_InitPka( uint32_t   opSizeInBits,          /*in - modulus size in bits*/
              uint32_t   regsCount,             /*in*/
              unsigned long  virtualHwBaseAddr) /*in*/
{

    /* LOCAL DECLARATIONS */

    uint32_t  regSizeInPkaWords;

    /*     enabling the PKA clocks      */
    SB_HAL_WRITE_REGISTER( SB_REG_ADDR(virtualHwBaseAddr, PKA_CLK_ENABLE), 0x1UL );

    /* minimal needed regs size */
    regSizeInPkaWords = GET_FULL_OP_SIZE_PKA_WORDS(opSizeInBits);


    /* setting the PKA registers mapping table */
    /*-----------------------------------------*/
    RSA_PKA_SetRegsMapTab(regsCount, regSizeInPkaWords, virtualHwBaseAddr);

    /* setting the PKA registers sizes table   */
    /*-----------------------------------------*/
    RSA_PKA_SetRegsSizesTab(opSizeInBits, regSizeInPkaWords, virtualHwBaseAddr);

    /* ......  End of function ...... */
    return;

}


/***********      RSA_HW_PKI_PKA_FinishPKA function      **********************/
/**
 * @brief This function ends the PKA engine session and disables PKA clocks.
 *
 * @param[in] virtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_FinishPKA( unsigned long virtualHwBaseAddr /*in*/ )
 {

   /* FUNCTION LOGIC */

   /* close PKA clocks */
   SB_HAL_WRITE_REGISTER( SB_REG_ADDR(virtualHwBaseAddr, PKA_CLK_ENABLE), 0x0UL );

 }



/*******************************************************************************/
/************    RSA PKI PKA mathmatic functions and macros     ****************/
/*******************************************************************************/


/***********    PkaDbgExecOperation (with virtual pointers)     ******************/
/**
 * @brief This function executes any allowed PKA mathematic operation according to
 *        user passed Opcode.
 *
 *        The function receives code of operation, virtual pointers to PKI registers
 *        (sequence number), for arguments and result, and operates PKA machine by writing
 *        full operation code into OPCODE register. Then the function calls macros for
 *        waiting the PKA pipe ready signal.
 *        If opcode is illegal or one of operands is illegal, the function returns an
 *        error code defined in llf_pki_error.h file.
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
 * @param[in] Opcode   - The operation code according HW PKA definitions. Valid values: 0 - max Opcode.
 * @param[in] LenID    - ID of the length of operands according to register sizes table
 *                       (means the number of entry in the table). Valid values: 0...7.
 * @param[in] IsAImmed - If IsAImmed = 1, then operand A treated as immediate value, else -
 *                       as virtual register pointer. Valid values: 0,1.
 * @param[in] OpA      - Operand A: an immediate value or virtual register pointer, according to IsAImmed
 *                       IsAImmed parameter. Valid values: 0 <= OpA <= 31.
 * @param[in] IsBImmed - If IsBImmed = 1, then operand B treated as immediate value, else -
 *                       as virtual register pointer. Valid values: 0,1.
 * @param[in] OpB      - Operand B: an immediate value or virtual register pointer, according to IsAImmed
 *                       IsBImmed parameter. Valid values: 0 <= OpA <= 31.
 * @param[in] ResDiscard -  If ResDiscard = 1, then result is discarded.
 * @param[in] Res        - Virtual register pointer for result data.
 *                         Valid values: 0 <= Res <= 31. Value Res = RES_DISCARD means result must be discarded.
 * @param[in] Tag        - The user defined value (Tag <= 31), used for indication goals.
 *
 *
 * @return CCError_t - On success CC_OK is returned, on failure an error code:
 *                       PKA_ILLEGAL_OPCODE_ERROR
 *                       PKA_ILLEGAL_OPERAND_LEN_ERROR
 *                       PKA_ILLEGAL_OPERAND_TYPE_ERROR
 *                       PKA_ILLEGAL_OPERAND_ERROR
 *                       PKA_INVERSION_NOT_EXISTS_ERROR
 */


CCError_t _RSA_PKA_ExecOperation( uint32_t      Opcode,      /*in*/
                uint8_t       LenID,         /*in*/
                uint8_t       IsAImmed,      /*in*/
                int8_t        OpA,       /*in*/
                uint8_t       IsBImmed,      /*in*/
                int8_t        OpB,       /*in*/
                uint8_t       ResDiscard,    /*in*/
                int8_t        Res,       /*in*/
                uint8_t       Tag,       /*in*/
                    unsigned long      virtualHwBaseAddr)
{

    /* LOCAL DECLARATIONS */

    /* error identification */
    CCError_t Error = CC_OK;

    /* full Operation Code word */
    uint32_t FullOpCode;

    /* FUNCTION LOGIC */

    /* if Res == RES_DISCARD , then result is discarded */
    if (Res == (int8_t)RES_DISCARD) {
        ResDiscard = 1;
        Res = 0;
    }


    /*************************************************/
    /*      main PKI operation of this function      */
    /*************************************************/

    FullOpCode = RSA_PKA_FullOpCode(Opcode, LenID, IsAImmed, OpA, IsBImmed, OpB, ResDiscard, Res, Tag);
    RSA_PKA_WAIT_ON_PKA_PIPE_READY(virtualHwBaseAddr);
    SB_HAL_WRITE_REGISTER( SB_REG_ADDR(virtualHwBaseAddr, OPCODE), FullOpCode);


    /*************************************************/
    /* finishing operations for different cases      */
    /*************************************************/

    return Error;

} /* END OF function PkaDbgExecOperation */


/***********      PkaCopyDataIntoPkaReg function      **********************/
/**
* @brief This function  copies source data into PKA register .
*
*        Assumings: - PKA is initialized.
*                   - Length of extended (by word) registers is placed into LenID entry of
*                     sizes table.
*                   - If the extra word of register must be cleared also the user must
*                     set LenID according to extended register size
*
* @param[in] dstReg       - Virtual address (number) of destination register.
* @param[in] LenId        - ID of entry of regsSizesTable, defining full register length,
*                           aligned to PKA word.
* @param[in] src_ptr      - Pointer to source buffer.
* @param[in] sizeWords    - Data size in words.
*
* @return - no return parameters.
*
*/
void RSA_HW_PKI_PKA_CopyDataIntoPkaReg(uint32_t    dstReg,    /*out*/
                       uint32_t    LenID,     /*in*/
                       const  uint32_t  *src_ptr, /*in*/
                       uint32_t    sizeWords,  /*in*/
                       unsigned long  virtualHwBaseAddr)
{

    /* LOCAL DECLARATIONS */


    /* current register address and size */
    uint32_t  currAddr;
    uint32_t  regSizeWords;


    /* FUNCTION LOGIC */

    /* copy data from src buffer into PKA register with 0-padding  *
    *  in the last PKA-word                       */

    RSA_PKA_WAIT_ON_PKA_DONE(virtualHwBaseAddr);
        /* register size in 32-bits words */
    RSA_PKA_ReadRegSize(regSizeWords, LenID, virtualHwBaseAddr);/*temporary in Bits*/
    regSizeWords = CALC_FULL_32BIT_WORDS(regSizeWords);

        currAddr = RSA_PKA_GetRegAddress(dstReg, virtualHwBaseAddr);
        RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM(virtualHwBaseAddr, currAddr, src_ptr, sizeWords);

        /* zeroe not significant high words of the register */
        if (regSizeWords > sizeWords){
                currAddr = currAddr + sizeWords;
                RSA_HW_PKI_HW_CLEAR_PKA_MEM(virtualHwBaseAddr, currAddr, regSizeWords - sizeWords);
        }

#ifdef PKA_DEBUG
    /*! PKA_DEBUG */
//!! RL RSA_PKA_Copy(LenID/*LenID*/, dstReg, dstReg, 0/*Tag*/ , virtualHwBaseAddr);
#endif
    return;
} /* END OF function PkaCopyDataIntoPkaReg */



/***********      RSA_HW_PKI_PKA_CopyDataFromPkaReg      **********************/
/**
 * @brief This function copies data from PKA register into output buffer .
 *
 *        Assumings: - PKA is initialized.
 *                   - Length of extended (by word) registers is placed into LenID entry of
 *                     sizes table.
 *                   - If the extra word of register must be cleared also the user must
 *                     set LenID according to extended register size
 *
 * @param[in] srcReg       - Virtual address (number) of source PKA register.
 * @param[in] dst_ptr      - Pointer to destination buffer.
 * @param[in] sizeBytes - Source size in bytes.
 *
 * @return - no return parameters.
 *
 */
void RSA_HW_PKI_PKA_CopyDataFromPkaReg(uint32_t *dst_ptr,       /*out*/
                uint32_t  sizeWords,        /*in*/
                uint32_t  srcReg,       /*in*/
                unsigned long  virtualHwBaseAddr)
{

    /* LOCAL DECLARATIONS */

    /* current register address and size */
    uint32_t  currAddr = 0;

    /* FUNCTION LOGIC */

    RSA_PKA_WAIT_ON_PKA_DONE(virtualHwBaseAddr);
    currAddr = RSA_PKA_GetRegAddress(srcReg, virtualHwBaseAddr);
    RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM(virtualHwBaseAddr, currAddr, dst_ptr, sizeWords );

    return;

} /* END OF function PkaCopyDataFromPkaReg */

