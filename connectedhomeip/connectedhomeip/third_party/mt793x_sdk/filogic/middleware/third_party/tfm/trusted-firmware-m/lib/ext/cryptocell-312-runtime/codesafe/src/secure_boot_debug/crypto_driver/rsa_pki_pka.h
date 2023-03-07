/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef RSA_PKI_PKA_H
#define RSA_PKI_PKA_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#include "secureboot_basetypes.h"
#include "secureboot_stage_defs.h"


#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/


/* values for defining, that PKA entry is not in use */
#define PKA_SIZE_ENTRY_NOT_USED  0xFFFFFFFF
#define PKA_ADDRESS_ENTRY_NOT_USED  0xFFC

/* define NpCreateFlag values */
#define PKA_CreateNP  1
#define PKA_SetNP     0

/* define "discard result" value */
#define RES_DISCARD  0x3F


/* default LenID-s definitions. Last 3 names used in RSA KG.
   Note: extended sizes are in the entries LenID+1 (i.e. 1,3) accordingly */
#define LenIDn    0  /* for operation or modulus size*/
#define LenIDpq   2  /* for P, Q size */
#define LenID101  4  /* for P1,P2,Q1,Q2 101 bit size */
#define LenIDmin  6  /* for min allowed operation size (now 128 bit) */
#define LenIDmax  7  /* for max allowed operation size (now 2112 bit) */

/* define of PKA Tag for debug printing of PKA operations result
   into tempRes global buffer */
#define PKA_TAG_DebugPtint  1


/************************ Enums ********************************/

/* defines enumerator for all allowed PKA operations */
typedef enum
{
    PKA_Terminate_ID    =  0,
    PKA_Add_ID          =  1,
    PKA_AddIm_ID        =  2,
    PKA_Sub_ID          =  3,
    PKA_SubIm_ID        =  4,
    PKA_Neg_ID          =  5,

    PKA_ModAdd_ID       =  6,
    PKA_ModAddIm_ID     =  7,
    PKA_ModSub_ID       =  8,
    PKA_ModSubIm_ID     =  9,
    PKA_ModNeg_ID       = 10,

    PKA_And_ID          = 11,
    PKA_Tst0_ID         = 12,
    PKA_Clr0_ID         = 13,
    PKA_Clr_ID          = 14,

    PKA_OR_ID           = 15,
    PKA_Copy_ID         = 16,
    PKA_SetBit0_ID      = 17,

    PKA_XOR_ID          = 18,
    PKA_Flip0_ID        = 19,
    PKA_InvBits_ID      = 20,
    PKA_Compare_ID      = 21,

    PKA_SHR0_ID         = 22,
    PKA_SHR1_ID         = 23,
    PKA_SHL0_ID         = 24,
    PKA_SHL1_ID         = 25,

    PKA_LMul_ID         = 26,
    PKA_HMul_ID         = 27,
    PKA_ModMul_ID       = 28,
    PKA_ModMulNR_ID     = 29,
    PKA_ModExp_ID       = 30,

    PKA_Div_ID          = 31,
    PKA_ModInv_ID       = 32,

    PKA_OpOffMode

} RSA_HW_PKI_PKA_OpID_t;


/*  Enumerator, defining whether the functions performs default settings
    of PKI registers, or takes they from user passed arguments  */
typedef enum
{
      DefaultRegs       = 0,
      SetRegs           = 1,
      RegsSetOffMode

} RSA_HW_PKI_PKA_SetRegs_t;


/************************ Typedefs  ****************************/

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/* definition of the Registers sizes array type, used for initialisation
   of the PKA sizes table */
typedef uint32_t RSA_HW_PKI_PKA_RegsSizes_t[RSA_PKA_MAX_COUNT_OF_REGS_SIZES];

 /* This structure contains virtual pointers (numbers in range 0-31) of
    PKI registers RegsNum[]  and registers physical addresses RegsAddr[].
    For example: RegsNum[3] = 5 and RegsAddr[3] = 0x400 means, that address of
    register 5 is 0x400. The user may set into this structure only needed registers,
    used in planned PKA operations. */

typedef  struct
{
       uint32_t    RegsNum[RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS] ;
       uint32_t    RegsAddr[RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS] ;
}RSA_HW_PKI_PKA_RegsMap_t;

/************************ Public Variables **********************/

/************************ Public Functions ******************************/


/***********************************************************************************************/
/***********************************************************************************************/
/*                                                                                             */
/*                   RSA PKI FUNCTIONS, PERFORMING PKA OPERATIONS                              */
/*                                                                                             */
/***********************************************************************************************/
/***********************************************************************************************/



/*******************************************************************************/
/**********      RSA PKI PKA initialisation functions and macros      **********/
/*******************************************************************************/

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
                  unsigned long  VirtualHwBaseAddr);


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
 * @param[in] VirtualHwBaseAddr - ARM CryptoCell base address
 *
 * @return - no return value
 *
 */
void RSA_PKA_SetRegsMapTab(int32_t   countOfRegs,
               int32_t   regSizeInPkaWords,
               unsigned long  VirtualHwBaseAddr);



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
 *
 * @return void
 *
 */
 void RSA_PKA_InitPka(uint32_t   opSizeInBits,  /*in*/
                      uint32_t   regsCount,     /*in*/
                      unsigned long  virtualHwBaseAddr) /*out*/;


/***********      RSA_HW_PKI_PKA_FinishPKA function      **********************/
/**
 * @brief This function ends the PKA engine session and disables PKA clocks.
 *
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_FinishPKA( unsigned long VirtualHwBaseAddr /*in*/ );



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
                    unsigned long      VirtualHwBaseAddr);

/*************************************************************************/
/* Macros for calling PKA operations (names according to operation issue */
/*************************************************************************/

/*----------------------------------*/
/*   1.  ADD - SUBTRACT operations  */
/*----------------------------------*/

/*  Add:   Res =  OpA + OpB  */
#define   RSA_PKA_Add(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaAdd,(LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  AddIm:  Res =  OpA + OpBIm  */
#define   RSA_PKA_Add_IM(LenID, OpA, OpBIm, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaAddIm,(LenID), 0, (OpA), 1, (OpBIm), 0, (Res), (Tag), (VirtualHwBaseAddr) )
/*  SubIm:  Res =  OpA - OpBIm  */
#define   RSA_PKA_SUB_IM(LenID, OpA, OpBIm, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaSubIm,(LenID), 0, (OpA), 1, (OpBIm), 0, (Res), (Tag), (VirtualHwBaseAddr) )

/*----------------------------------*/
/*   2.  Logical   operations       */
/*----------------------------------*/

/*  Clr:  Res =  OpA & 0  - clears the operand A.  */
#define   RSA_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaAND, (LenID), 0, (OpA), 1, 0x00, 0, (OpA), (Tag), (VirtualHwBaseAddr))

/*  Clear:  for full clearing the actual register opA, this macro calls Clr operation twice.  */
#define   RSA_PKA_Clear(LenID, OpA, Tag, VirtualHwBaseAddr)   \
            RSA_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr);  \
            RSA_PKA_Clr(LenID, OpA, Tag, VirtualHwBaseAddr)

/*  Set0:  Res =  OpA || 1  : set bit0 = 1, other bits are not changed */
#define   RSA_PKA_Set0(LenID, OpA, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation(PkaOR, (LenID), 0, (OpA), 1, 0x01, 0, (Res), (Tag), (VirtualHwBaseAddr))

/*-----------------------------------------------------*/
/*   2.  Multiplication and other   operations         */
/*       Note:  See notes to PKA_ExecOperation */
/*-----------------------------------------------------*/
#define   RSA_PKA_Add(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaAdd,(LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr) )

/*  ModExp:  Res =  OpA ** OpB  mod N - modular exponentiation */
#define   RSA_HW_PKI_PKA_ModExp(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
             _RSA_PKA_ExecOperation( PkaModExp, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag), (VirtualHwBaseAddr))

/*  Copy:  OpDest =  OpSrc || 0  */
#define   RSA_PKA_Copy(LenID, OpDest, OpSrc, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation(PkaOR, (LenID), 0, (OpSrc), 1, 0x00, 0, (OpDest), (Tag), (VirtualHwBaseAddr))

/*  SHR0:  Res =  OpA >> (S+1) :   shifts right operand A by S+1 bits, insert 0 to left most bits */
#define   RSA_PKA_SHR0(LenID, OpA, S, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaSHR0, (LenID), 0, (OpA), 0, (S), 0, (Res), (Tag), (VirtualHwBaseAddr))

/*  SHL0:  Res =  OpA << (S+1) :   shifts left operand A by S+1 bits, insert 0 to right most bits */
#define   RSA_PKA_SHL0(LenID, OpA, S, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation(PkaSHL0, (LenID), 0, (OpA), 0, (S), 0, (Res), (Tag), (VirtualHwBaseAddr))

/*  Divide:  Res =  OpA / OpB , OpA = OpA mod OpB - division,  */
#define   RSA_PKA_Div(LenID, OpA, OpB, Res, Tag, VirtualHwBaseAddr)   \
            _RSA_PKA_ExecOperation( PkaDiv, (LenID), 0, (OpA), 0, (OpB), 0, (Res), (Tag),  (VirtualHwBaseAddr))



#define RSA_PKA_WRITE_WORD_TO_REG(Val,i,VirtReg, VirtualHwBaseAddr) {\
    uint32_t  currAddr;\
    RSA_PKA_WAIT_ON_PKA_DONE(VirtualHwBaseAddr); \
    currAddr = RSA_PKA_GetRegAddress(VirtReg, VirtualHwBaseAddr);\
    RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM(VirtualHwBaseAddr, currAddr+(i), (Val));\
}


/* ***************************************************************************** */
/* ************   Auxiliary functions used in PKA               **************** */
/* ***************************************************************************** */


/***********      RSA_HW_PKI_PKA_ClearBlockOfRegs function      **********************/
/**
 * @brief This function clears block of PKA registers.
 *
 *        Assumings: - PKA is initialised.
 *                   - Length of extended (by word) registers is plased into LenID entry of
 *                     sizes table.
 *
 * @param[in] firstReg    - Virtual address (number) of first register in block.
 * @param[in] countOfRegs - Count of registers to clear.
 * @param[in] LenId       - ID of entry of regsSizesTable defines registers lengts with word extension.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 */
 void RSA_HW_PKI_PKA_ClearBlockOfRegs(
                                uint8_t  firstReg,          /*in*/
                                uint8_t  countOfRegs,       /*in*/
                                uint8_t  LenID,             /*in*/
                                unsigned long VirtualHwBaseAddr  /*in*/ );



/***********      RSA_HW_PKI_PKA_CopyDataIntoPkaReg function      **********************/
/**
 * @brief This function  copies source data into PKA register .
 *
 *        Assumings: - PKA is initialised.
 *                   - Length of extended (by word) registers is plased into LenID entry of
 *                     sizes table.
 *                   - If the extra word of register must be cleared also the user must
 *                     set LenID according to extended register size
 *
 * @param[in] dstReg       - Virtual address (number) of destination register.
 * @param[in] LenId        - ID of entry of regsSizesTable defines registers lengts with word extension.
 * @param[in] src_ptr      - Pointer to source buffer.
 * @param[in] sizeBytes    - Data size in bytes.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_CopyDataIntoPkaReg(uint32_t    dstReg,   /*out*/
                       uint32_t    LenID,     /*in*/
                       const  uint32_t  *src_ptr, /*in*/
                       uint32_t    sizeWords,  /*in*/
                       unsigned long  VirtualHwBaseAddr);




/***********      RSA_HW_PKI_PKA_CopyDataFromPkaReg      **********************/
/**
 * @brief This function copies data from PKA register into output buffer .
 *
 *        Assumings: - PKA is initialised.
 *                   - Length of extended (by word) registers is plased into LenID entry of
 *                     sizes table.
 *                   - If the extra word of register must be cleared also the user must
 *                     set LenID according to extended register size
 *
 * @param[in] srcReg       - Virtual address (number) of source PKA register.
 * @param[in] dst_ptr      - Pointer to destination buffer.
 * @param[in] sizeBytes - Source size in bytes.
 * @param[in] VirtualHwBaseAddr -  Virtual HW base address, passed by user.
 *
 * @return - no return parameters.
 *
 */
 void RSA_HW_PKI_PKA_CopyDataFromPkaReg(uint32_t *dst_ptr,      /*out*/
                uint32_t  sizeWords,        /*in*/
                uint32_t  srcReg,       /*in*/
                unsigned long  VirtualHwBaseAddr);


 /***********      RSA_HW_PKI_SET_MSB_ADDR      **********************/
/**
 * @brief This function sets the PKI MSB register according ot the SRAM address .
 *
 *        Assumings: - PKA registers are accessable.
 *                   - SRAM address is multiple of 4KB.
 *
 * @param[in] pkaAdrress       - base address of the SRAM for PKA.
 *
 * @return - no return parameters.
 *
 */
void RSA_HW_PKI_SET_MSB_ADDR(unsigned long virtualAddress, uint32_t pkaAdrress);

/**
 * The function uses physical data pointers to calculate
 * the Barrett tag Np. Np is saved into Register indexed by regNp.
 *
 *  For RSA it uses truncated sizes:
 *      Np = truncated(2^(3*A+3*X-1) / ceiling(n/(2^(N-2*A-2*X)));
 *
 *      function assumes modulus in PKA reg 0, and output is to PKA reg 1
 *
 */
uint32_t  RSA_HW_PKA_CalcNpIntoPkaReg(uint32_t lenId,
                uint32_t    sizeNbits,
                int8_t      regN,
                int8_t      regNp,   // out
                int8_t      regTemp1,
                int8_t      regTempN,
                unsigned long VirtualHwBaseAddr) ;

#ifdef __cplusplus
}
#endif

#endif
