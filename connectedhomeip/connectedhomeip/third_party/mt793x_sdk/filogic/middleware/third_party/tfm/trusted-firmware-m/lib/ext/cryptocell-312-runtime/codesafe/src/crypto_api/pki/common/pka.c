/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include "cc_hal_plat.h"
#include "cc_sram_map.h"
#include "dx_crys_kernel.h"
#include "cc_regs.h"
#include "pka_hw_defs.h"
#include "pki.h"
#include "pki_dbg.h"
#include "pka.h"
#include "ec_wrst.h"
#include "pka_error.h"
#include "cc_common_math.h"
#include "cc_hal.h"
#include "cc_int_general_defs.h"

extern CC_PalMutex CCAsymCryptoMutex;

/* Maximum allowed PKA registers are 32 (PKA_MAX_COUNT_OF_PHYS_MEM_REGS): first 2 (PKA_REG_N & PKA_REG_NP) servers for N (modulus) and Np respectivly.
   last 2 (PKA_REG_T0 & PKA_REG_T1) are reserved for HW use. so we have total of 28 registers for SW usage
   list of maximum 28 allowed temp PKA registers for functions.
   Note: last 2 are numbered 0xFF - for debug goals */
const int8_t regTemps[PKA_MAX_COUNT_OF_PHYS_MEM_REGS] = {PKA_REG_N, PKA_REG_NP,
        0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
        0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,
        0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,PKA_REG_T0,PKA_REG_T1};

#if defined PKA_DEBUG && defined DEBUG
uint32_t tempRes[PKA_MAX_REGISTER_SIZE_IN_32BIT_WORDS];
uint32_t tempRes1[PKA_MAX_REGISTER_SIZE_IN_32BIT_WORDS];
#endif


/***********      PkaSetRegsMapTab function      **********************/
/**
 * @brief This function initializes the PKA registers sizes table.
 *
 *   The function checks input parameters and sets the physical memory registers mapping-table
 *   according to parameters, passed by the user:
 *     - start address of register 0 is the start address of PKA data registers memory
 *       CC_SRAM_PKA_BASE_ADDRESS (defined in cc_sram_map.h file);
 *     - special registers are set as follows: N=0,NP=1,T0=30,T1=31;
 *     - all registers have the same size, equalled to given size;
 *
 * @return - None.
 *
 */
static void PkaSetRegsMapTab(int32_t   countOfRegs,         /*!< [in] The count of registeres, requirred by the user. */
                              int32_t   regSizeInPkaWords)  /*!< [in] Size of registers in PKA big words (e.g. 128-bit words). */
{
        uint32_t  currentAddr;
        int32_t i;

        /* start addres of PKA mem. */
        currentAddr = CC_SRAM_PKA_BASE_ADDRESS;

        /* set addresses of the user requested registers (excluding T0,T1) */
        for (i = 0; i < PKA_MAX_COUNT_OF_PHYS_MEM_REGS-2; i++) {
                if (i < countOfRegs - 2) {
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, MEMORY_MAP0) + i*sizeof(uint32_t), currentAddr);
                        currentAddr += regSizeInPkaWords*PKA_WORD_SIZE_IN_32BIT_WORDS;
                } else {
                        /* write designation, that PKI entry is not in use */
                        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, MEMORY_MAP0)+ i*sizeof(uint32_t), PKA_ADDRESS_ENTRY_NOT_USED);
                }
        }
        /* set addresses of 2 temp registers: T0=30, T1=31 */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, MEMORY_MAP0) + 30*sizeof(uint32_t), currentAddr);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, MEMORY_MAP0) + 31*sizeof(uint32_t),
                                currentAddr + regSizeInPkaWords*PKA_WORD_SIZE_IN_32BIT_WORDS);

        /* set default virtual addresses of N,NP,T0,T1 registers into N_NP_T0_T1_Reg */
        PKA_DEFAULT_N_NP_T0_T1_REG();

        return;

}


/***********      PkaDivLongNum function      **********************/
/**
 * @brief The function divides long number A*(2^S) by B:
 *            Res =  A*(2^S) / B,  remainder A = A*(2^S) % B.
 *        where: A,B - are numbers of size, which is not grate than, maximal operands size,
 *               and B > 2^S;
 *               S  - exponent of binary factor of A.
 *               ^  - exponentiation operator.
 *
 *        The function algorithm:
 *
 *        1. Let nWords = S/32; nBits = S % 32;
 *        2. Set Res = 0, rT1 = OpA;
 *        3. for(i=0; i<=nWords; i++) do:
 *            3.1. if(i < nWords )
 *                   s1 = 32;
 *                 else
 *                   s1 = nBits;
 *            3.2. rT1 = rT1 << s1;
 *            3.3. call PKA_div for calculating the quotient and remainder:
 *                      rT2 = floor(rT1/opB) //quotient;
 *                      rT1 = rT1 % opB      //remainder (is in rT1 register);
 *            3.4. Res = (Res << s1) + rT2;
 *           end do;
 *        4. Exit.
 *
 *        Assuming:
 *                  - 5 PKA registers are used: OpA, OpB, Res, rT1, rT2.
 *                  - The registers sizes and mapping tables are set on default mode
 *                    according to operands size.
 *                  - The PKA clocks are initialized.
 *        NOTE !   Operand OpA shall be overwritten by remainder.
 *
 * @return  CC_OK On success, otherwise indicates failure
 *
 */
static CCError_t PkaDivLongNum(uint8_t      lenId, /*!< [in] ID of operation size (modSize+32). */
                                 int8_t       OpA,   /*!< [in] Operand A: virtual register pointer of A . */
                                 uint32_t     S,     /*!< [in] exponent of binary factor of A. */
                                 int8_t       OpB,   /*!< [in] Operand B: virtual register pointer of B. */
                                 int8_t       Res,   /*!< [out] Virtual register pointer for result quotient. */
                                 int8_t       rT1,   /*!< [in] Virtual pointer to remainder. */
                                 int8_t       rT2)   /*!< [in] Virtual pointer of temp register. */
{
        uint32_t  nBits, nWords;
        uint32_t  i;
        int8_t s1 = 0; /* current shift count */


        /* calculate shifting parameters (words and bits ) */
        nWords = (CALC_FULL_32BIT_WORDS((uint32_t)S));
        nBits  = (uint32_t)S % CC_BITS_IN_32BIT_WORD;

        /* copy operand OpA (including extra word) into temp reg rT1 */
        PKA_COPY(LEN_ID_MAX_BITS, rT1/*dst*/, OpA/*src*/);

        /* set Res = 0 (including extra word) */
        PKA_2CLEAR(LEN_ID_MAX_BITS, Res/*dst*/);


        /* Step 1.  Shifting and dividing loop                */
        for (i = 0; i < nWords; i++) {
                /* 3.1 set shift value s1  */
                if (i > 0)
                        s1 = CC_BITS_IN_32BIT_WORD;
                else
                        s1 = nBits;

                /* 3.2. shift: rT1 = rT1 * 2**s1 (in code (s1-1), because PKA performs S+1 shifts) */
                if (s1 > 0) {
                        PKA_SHL_FILL0( lenId+1, rT1/*Res*/, rT1/*OpA*/, (s1-1)/*S*/);
                }

                /* 3.3. perform PKA_DIV for calculating a quotient rT2 = floor(rT1 / N)
                        and remainder rT1 = rT1 % OpB  */
                PKA_DIV( lenId+1, rT2/*Res*/, rT1 /*OpA*/, OpB /*B*/);

#ifdef LLF_PKI_PKA_DEBUG_
                /* debug copy result into temp buffer */
                CC_PalMemSetZero((uint8_t*)tempRes, sizeof(tempRes));
                PkaCopyDataFromPkaReg(
                                      tempRes/*dst_ptr*/, RegSizeWords,
                                      rT1/*srcReg*/);
#endif

                /* 3.4. Res = Res * 2**s1 + Res;   */
                if (s1 > 0) {
                        PKA_SHL_FILL0( lenId+1, Res /*Res*/, Res /*OpA*/, (s1-1)/*S*/);
                }

                PKA_ADD( lenId+1, Res   /*Res*/, Res /*OpA*/, rT2 /*OpB*/);
        }

        PKA_WAIT_ON_PKA_DONE();

        return CC_OK;

}



/***********        PkaModDivideBy2            **********************/
/**
 * @brief This function performs modular division by 2: rRes = rX / 2 mod rN.
 *
 * @return - None.
 *
 */
void  PkaModDivideBy2(uint32_t    lenId,  /*!< [in]  ID of entry of regsSizesTable containing rX modulus exact length. */
                      uint32_t    rX,    /*!< [in]  Virtual pointer to PKA register X.                                */
                      uint32_t    rN,    /*!< [out]  Virtual pointer to PKA register, containing the modulus N.        */
                      uint32_t    rRes)  /*!< [out]  Virtual pointer to PKA register, containing the result.           */
{
        uint32_t bitVal = 0;

    if (rX != rRes) {
                PKA_COPY(LEN_ID_MAX_BITS, rRes/*dst*/, rX/*src*/);
        }

        /* if the vector rX is odd, then add the modulus and then  divide by 2 */

        PKA_READ_BIT0(lenId+1, rRes/*regNum*/, bitVal);
        if (bitVal == 1) {
                PKA_ADD(lenId+1, rRes/*Res*/, rRes/*P*/, rN/*OpB=N=0*/);
        }

        /* divide by 2 */
        PKA_SHR_FILL0(lenId+1, rRes/*Res*/, rRes/*P*/, 1-1/*S*/);


}


/***********   PkaGetRegEffectiveSizeInBits  function **********************/
/**
 * @brief This function returns effective size in bits of data placed in PKA register.
 *
 * @return - Effective size of data in register (bits).
 *
 */
uint32_t   PkaGetRegEffectiveSizeInBits(uint32_t  reg) /*!< [in] Register virt. pointer. */
{
        // RL Do resistant and add flag to arg.
        int size = 1, i;
        uint32_t  addr;
        uint32_t  currWord = 0, mask = 1Ul << 31;

        /* read register address and full operation size in bits */
        PKA_GET_REG_ADDRESS(reg, addr);
        PKA_GET_REG_SIZE(size, LEN_ID_MAX_BITS/*lenID*/);

        /* register size in words */
        size = CALC_FULL_32BIT_WORDS(size);

        /* read words and find MSWord */
        for (i = size-1 ; i >= 0  ; i--) {
                PKA_HW_READ_VALUE_FROM_PKA_MEM(addr + i, currWord);
                if (currWord != 0)
                        break;
        }

        size = CC_BITS_IN_32BIT_WORD*(i+1); //in bits

        if (currWord == 0)
                return size;

        /* find number of bits in the MS word */
        for (i = 1; i <= CC_BITS_IN_32BIT_WORD; i++) {
                if (currWord & mask)
                        break;
                size--;
                mask >>= 1;
        }

        return size;
}


/***********     PkaGetNextMsBit  function     **********************/
/**
 * @brief The function returns MS-bit from register r.
 *
 *
 * @author reuvenl (6/12/2014)
 *
 * @return uint32_t - bit's value
 */
uint32_t  PkaGetNextMsBit(uint32_t rX,      /*!< [in] Register virt. pointer. */
              int32_t i,            /*!< [in] Index of the requirred bit. */
              uint32_t *pW,         /*!< [in] Pointer to 32-bit current word, which must be saved by
                            caller through reading bits from the register. */
              uint32_t *pIsNew)     /*!< [in] Pointer to indicator is this a new start (pIsNew=1) of
                            the function for this register. The value is updated
                            to 0 by the function after first start. */
{
        uint32_t b;

        if (*pIsNew || (i & 31UL) == 31) {
                PKA_READ_WORD_FROM_REG(*pW, i>>5, rX);
                /* ones only */
                if ((i & 31UL) != 31)
                        *pW <<= (31 - (i & 31UL));
                *pIsNew = 0;
        }

        b = *pW >> 31;
        *pW <<= 1;

        return b;
}



/***********     PkaGet2MsBits  function     **********************/
/**
 * @brief The function returns 2 MS-bits from register r.
 *
 * @author reuvenl (6/12/2014)
 *
 * @return uint32_t - bit's value
 */
uint32_t PkaGet2MsBits(uint32_t rX,        /*!< [in] Register virt. pointer. */
            int32_t i,         /*!< [in] Index of the required bit. */
            uint32_t *pW,      /*!< [in] Pointer to 32-bit current word, which must be saved by
                            caller through reading bits from the register. */
            uint32_t *pIsNew)  /*!< [in] Pointer to indicator is it a new start other function
                            for this register or not. The value is updated
                            to FALSE by the function after start. */
{
        uint32_t b;

        PKA_ASSERT(!(i&1), "PkaGet2MsBits: even bit");

        if (*pIsNew || (i & 0x1F) == 30) {
                PKA_READ_WORD_FROM_REG(*pW, i>>5, rX);
                *pIsNew = 0;
        }

        b = (*pW >> (i&0x1F)) & 0x3;

        return b;
}


/***********     PkaFinishAndMutexUnlock  function     **********************/
/**
 * @brief This function clears the PKA memory and clock, and unlocks the Asymmetric mutex.
 *
 * @return  None
 */
void PkaSetLenIds(uint32_t  sizeInBits,  /*!< [in] The exact size operation for the LenId. */
          uint32_t  lenId)       /*!< [in] ID of entry of regsSizesTable defines register length
                                with word extension. */
{

        PKA_SET_REG_SIZE(sizeInBits, lenId);
        PKA_SET_REG_SIZE(GET_FULL_OP_SIZE_BITS(sizeInBits), lenId+1);

}


/***********     PkaInitAndMutexLock  function     **********************/
/**
 * @brief This function Inialize the PKA memory and clock, and locks the Asymmetric mutex.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */

CCError_t PkaInitAndMutexLock(uint32_t  sizeInBits,   /*!< [in] Operation (modulus) exact size in bits. The value must
                                   be in interval from defined min. to  max. size in bits. */
                uint32_t *pkaRegCount)  /*!< [in/out] As input - required registers for operation.
                                   As output - actual available regs, must be at least as required. */
{
        CCError_t err = CC_OK;

        err = CC_PalMutexLock(&CCAsymCryptoMutex, CC_INFINITE);
        if (err != CC_SUCCESS) {
                CC_PalAbort("Fail to acquire mutex\n");
        }

        /* verify that the device is not in fatal error state before activating the PKA engine */
        CC_IS_FATAL_ERR_ON(err);
        if (err == CC_TRUE) {
                err = PKA_FATAL_ERR_STATE_ERROR;
                goto EndUnlockMutex;
        }

        /* increase CC counter at the beginning of each operation */
        err = CC_IS_WAKE;
        if (err != CC_SUCCESS) {
            CC_PalAbort("Fail to increase PM counter\n");
        }

        /* initialize the PKA engine on default mode with size of registers       */
        err = PkaInitPka(sizeInBits, 0 /*sizeInWords*/, pkaRegCount);

        if (err != CC_SUCCESS) {
            /* decrease CC counter at the end of each operation */
            if (CC_IS_IDLE != CC_SUCCESS) {
                CC_PalAbort("Fail to decrease PM counter\n");
            }
        }

EndUnlockMutex:
        if (err != CC_SUCCESS) {
            if (CC_PalMutexUnlock(&CCAsymCryptoMutex) != 0) {
                    CC_PalAbort("Fail to release mutex\n");
            }
        }

        return err;
}


/***********     PkaFinishAndMutexUnlock  function     **********************/
/**
 * @brief This function clears the PKA memory and clock, and unlocks the Asymmetric mutex.
 *
 * @return  None
 */
void PkaFinishAndMutexUnlock(uint32_t pkaRegCount) /*!< [in] Number of registers to clear. */
{
        // clear used registers
        if (pkaRegCount > 0) {
                pkaRegCount = CC_MIN(PKA_MAX_COUNT_OF_PHYS_MEM_REGS-2, pkaRegCount);
                /* clear used PKA registers for security goals */
                PkaClearBlockOfRegs(PKA_REG_N/*FirstReg*/, pkaRegCount, LEN_ID_MAX_BITS/*LenID*/);
        }

        /* Finish PKA operations (waiting PKI done and close PKA clocks) */
        PkaFinishPka();

        /* decrease CC counter at the end of each operation */
        if (CC_IS_IDLE != CC_SUCCESS) {
            CC_PalAbort("Fail to decrease PM counter\n");
        }

        /* release the hardware semaphore */
        if (CC_PalMutexUnlock(&CCAsymCryptoMutex) != CC_SUCCESS) {
                CC_PalAbort("Fail to release mutex\n");
        }

}


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
 * @return - None.
 *
 */
void PkaSetRegsSizesTab(uint32_t     opSizeInBits,   /*!< [in] Size of PKA operations (modulus) in bits. The value must be in interval
 *                                              from defined Min. to Max. size bits */
                         int32_t      regSizeInPkaWords) /*!< [in] Size of registers in PKA big words (e.g. 128-bit words). */
{

        uint32_t  i;

        /* Set exact op. size */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_L0), opSizeInBits);
        /* Set size with extra bits aligned to big words */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_L0) + CC_32BIT_WORD_SIZE, GET_FULL_OP_SIZE_BITS(opSizeInBits));

        /* remaining entries set to PKA_SIZE_ENTRY_NOT_USED for debugging goals */
        for (i = 2; i < PKA_NUM_OF_PKA_LEN_IDS_REGS; i++) {
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_L0) + CC_32BIT_WORD_SIZE*i,
                                                     regSizeInPkaWords*CC_PKA_WORD_SIZE_IN_BITS);
        }

        return;

}

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
 * @return  CC_OK On success, otherwise indicates failure
 *
 */
CCError_t PkaInitPka(uint32_t   opSizeInBits,        /*!< [in] Operation (modulus) exact size in bits. The value must
                                be in interval from defined min. to  max. size in bits. */
                       uint32_t   regSizeInPkaWords,     /*!< [in] PKA register size. not exact for operation (== modulus). */
                       uint32_t   *pRegsCount)       /*!< [in/out] As input - required registers for operation (including PKA
                                                               temp registers T0,T1). As output - actual available regs, must be
                                                               at least as required. */
{
        int32_t  regsCount;
        uint32_t  regSizeIn32BitWords;
        uint32_t  minRegSizeInPkaWords;
        uint32_t mask = 0;

        if (opSizeInBits < PKA_MIN_OPERATION_SIZE_BITS ) {
                return  PKA_REGISTER_SIZES_ERROR;
        }

        /* calculate pka register size */
        if (opSizeInBits < (2*(CC_PKA_WORD_SIZE_IN_BITS+PKA_EXTRA_BITS))) {
                regSizeIn32BitWords = CALC_FULL_32BIT_WORDS(opSizeInBits+CC_PKA_WORD_SIZE_IN_BITS+PKA_EXTRA_BITS-1);
                if ((opSizeInBits+CC_PKA_WORD_SIZE_IN_BITS+PKA_EXTRA_BITS-1) % CC_BITS_IN_32BIT_WORD) {
                        regSizeIn32BitWords++;
                }
        } else {
                regSizeIn32BitWords = CALC_FULL_32BIT_WORDS(opSizeInBits);
        }

        minRegSizeInPkaWords = GET_FULL_OP_SIZE_PKA_WORDS(regSizeIn32BitWords*CC_BITS_IN_32BIT_WORD);

        /* check given regs size or set it, if is not given */
        if (regSizeInPkaWords > 0) {
                if (regSizeInPkaWords < minRegSizeInPkaWords)
                        return PKA_REGISTER_SIZES_ERROR;
        } else {
                regSizeInPkaWords = minRegSizeInPkaWords;
        }

        /* actually avaliable count of PKA registers */
        regsCount = CC_MIN(CC_SRAM_PKA_SIZE_IN_BYTES / (regSizeInPkaWords*PKA_WORD_SIZE_IN_BYTES),
                             PKA_MAX_COUNT_OF_PHYS_MEM_REGS);

        if (pRegsCount != NULL) {
#ifdef PKA_DEBUG
                // checking number of registers are enough to execute this function
                if ((size_t)regsCount < *pRegsCount) {
                        return PKA_REGS_COUNT_ERROR;
                }
#endif
                *pRegsCount = regsCount;
        }

        /* Mask PKA interrupt */
        mask = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IMR));
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, PKA_EXP_MASK, mask, 1);
        CC_HalMaskInterrupt(mask);

        /*     enabling the PKA clocks      */
        CC_HAL_WRITE_REGISTER( CC_REG_OFFSET(CRY_KERNEL, PKA_CLK_ENABLE), 0x1UL );

        /* setting the PKA registers mapping table */
        PkaSetRegsMapTab(regsCount, regSizeInPkaWords);

        /* setting the PKA registers sizes table   */
        PkaSetRegsSizesTab(opSizeInBits, regSizeInPkaWords);

        return CC_OK;

}


/***********     PkaCalcNpIntoPkaReg  function      **********************/
/**
 * The function uses physical data pointers to calculate and output
 * the Barrett tag Np.
 *
 *  For RSA it uses truncated sizes:
 *      Np = truncated(2^(3*A+3*X-1) / ceiling(n/(2^(N-2*A-2*X)));
 *  For ECC - full sizes of not truncated input arguments:
 *      Np = truncated(2^(N+A+X-1) / n);
 *
 *      function assumes modulus in PKA reg 0, and output is to PKA reg 1
 *
 * @author reuvenl (5/1/2014)
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t  PkaCalcNpIntoPkaReg(uint32_t lenId,  /*!< [in] ID of entry of regsSizesTable defines register length
                                with word extension. */
                                  uint32_t  sizeNbits,  /*!< [in] The exact size of the modulus. */
                                  int8_t regN,          /*!< [in] Virtual address (number) holding the modulus n. */
                                  int8_t regNp,     /*!< [out] Virtual address (number) holding Np. */
                                  int8_t regTemp1,  /*!< [in] Virtual address (number) for temporary usage. */
                                  int8_t regTempN)  /*!< [in] Virtual address (number) for temporary usage. */
{
        CCError_t err = 0;
        int32_t i;
        uint32_t  A = CC_PKA_WORD_SIZE_IN_BITS;
        uint32_t  X = PKA_EXTRA_BITS;
        int32_t wT,bNom,wNom; /*Sizes in words and bits  */
        uint32_t val;
        int32_t sh, st;

        // clear temp registers
        PKA_2CLEAR(LEN_ID_MAX_BITS, regTemp1);
        PKA_2CLEAR(LEN_ID_MAX_BITS, regTempN);
        PKA_2CLEAR(LEN_ID_MAX_BITS, regNp);

        // copy modulus (regN) into temprarty register - regTempN
        PKA_COPY(LEN_ID_MAX_BITS /* LenID */, regTempN /* OpDest */, regN /* OpSrc */);

        if (sizeNbits <= (2*A + 2*X)) {
                wNom = CALC_FULL_32BIT_WORDS(sizeNbits+A+X-1);
                /* Sizes of nominator (N+A+X-1) in 32-bit words */
                bNom = (sizeNbits+A+X-1) % CC_BITS_IN_32BIT_WORD; /*remain bits*/
                if (bNom) {
                        val = 1UL << bNom;
                } else {
                        wNom++;
                        val = 1UL;
                }

                /* Set rT2 = 2^(N+A+X-1) */
                PKA_WRITE_WORD_TO_REG(val, wNom-1, regTemp1);
                // use LEN_ID_MAX_BITS for small sizes, since lenId is exact mod size which is not enought in this case!!!
                PKA_DIV(LEN_ID_MAX_BITS/*LenID*/, regNp, regTemp1, regTempN);
        }
        /* If  (N > 2*A + 2*X) - truncated */
        else {
                /* Set rT1 = 2^D, where D=(3*A+3*X-1) division nominator size */
                wNom = CALC_FULL_32BIT_WORDS(3*A + 3*X - 1); /*words count in nominator */
                /* Calc. sizes of Nominator */
                bNom = (3*A + 3*X - 1) % CC_BITS_IN_32BIT_WORD; /*remain bits count*/
                if (bNom) {
                        val = 1UL << bNom;
                } else {
                        wNom++;
                        val = 1UL;
                }

                /* Set rT1 = 2^D, where D=(3*A+3*X-1) */
                PKA_WRITE_WORD_TO_REG(val, wNom-1, regTemp1);

                /* Set rN = high part of the modulus as divisor */
                /* count low bits to truncate the modulus */
                st = sizeNbits - 2*A - 2*X;
                /* count of words to truncate */
                wT = st / CC_BITS_IN_32BIT_WORD;
                /* shift for truncation */
                sh = st % CC_BITS_IN_32BIT_WORD;


                /* prevent further ceiling increment, if it not needed */
                PKA_SUB_IM(lenId+1/*LenID*/, regTempN, regTempN, 1/*OpBIm*/);

                /* truncate modulus by words and then by bits */
                for (i=0; i<wT; i++) {
                        PKA_SHR_FILL0(lenId+1/*LenID*/, regTempN, regTempN, CC_BITS_IN_32BIT_WORD-1);
                }
                if (sh) {
                        PKA_SHR_FILL0(lenId+1/*LenID*/, regTempN, regTempN, sh-1);
                }

                /* Ceiling */
                PKA_ADD_IM(lenId+1/*LenID*/, regTempN, regTempN, 1/*OpBIm*/);
                PKA_DIV(LEN_ID_MAX_BITS/*LenID*/, regNp, regTemp1, regTempN);  //use LEN_ID_MAX_BITS to make sure we catch the whole size
        }

        // clear temp registers
        PKA_2CLEAR(LEN_ID_MAX_BITS, regTemp1);
        PKA_2CLEAR(LEN_ID_MAX_BITS, regTempN);

        return err;
}


/***********      PkaClearBlockOfRegs function      **********************/
/**
 * @brief This function clears block of PKA registers + temp registers 30,31.
 *
 *        Assumings: - PKA is initialized properly.
 *                   - Length of extended (by word) registers is placed into LenID entry of
 *                sizes table.
 *               - Meets condition: firstReg <= 30.
 *               - All registers, given to cleaning, are inside the allowed memory.
 *
 * @return - None.
 *
 */
void PkaClearBlockOfRegs( uint32_t  firstReg,      /*!< [in] Virtual address (number) of first register in block. */
                           int32_t   countOfRegs,  /*!< [in] Count of registers to clear. */
                           uint32_t  lenId)        /*!< [in] ID of entry of regsSizesTable defines register length
                                with word extension. */
{
    int32_t i;
    uint32_t size,  addr;

    /* check registers count */
    PKA_ASSERT(!(firstReg >= 32 || firstReg + countOfRegs > 32), "PkaClearBlockOfRegs: firstReg > 32 or firstReg + countOfRegs > 32");

    /* calculate size of register in words */
    PKA_GET_REG_SIZE(size, lenId);
    size = CALC_FULL_32BIT_WORDS(size);

    /* clear ordinary and temp registers without PKA operations */

    for (i = 0; i < countOfRegs; i++) {
            PKA_GET_REG_ADDRESS(firstReg+i/*VirtReg*/,addr/*physAddr*/);
            PKA_HW_CLEAR_PKA_MEM(addr, size);
    }
    PKA_GET_REG_ADDRESS(PKA_REG_T1/*VirtReg*/,addr/*physAddr*/);
    PKA_HW_CLEAR_PKA_MEM(addr, size);
    PKA_GET_REG_ADDRESS(PKA_REG_T0/*VirtReg*/,addr/*physAddr*/);
    PKA_HW_CLEAR_PKA_MEM(addr, size);

    return;
}

/***********      PkaCopyDataFromPkaReg      **********************/
/**
 * @brief This function copies data from PKA register into output buffer .
 *
 *        Assumings: - PKA is initialized.
 *                   - Length of extended (by word) registers is placed into LenID entry of
 *                     sizes table.
 *                   - If the extra word of register must be cleared also the user must
 *                     set LenID according to extended register size
 *
 * @return - None.
 *
 */
void PkaCopyPkaRegIntoBeByteBuff(uint8_t *dst_ptr,   /*!< [out] Virtual address (number) of source PKA register. */
                            uint32_t  sizeWords, /*!< [in] Buffer size in words. */
                            uint32_t  srcReg)    /*!< [in] Source register. */
{
        uint32_t  currAddr;
        uint32_t  tempWord;
        int32_t  ii;

        /* copy data from src buffer into PKA register with 0-padding  *
        *  in the last PKA-word                       */
        PKA_GET_REG_ADDRESS(srcReg, currAddr);

        PKA_MUTEX_LOCK;
        PKA_WAIT_ON_PKA_DONE();
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_RADDR), currAddr);
        for(ii = sizeWords-1; ii >= 0; ii--) {
                WAIT_SRAM_DATA_READY;
                tempWord = CC_HAL_READ_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_RDATA));
                tempWord = CC_COMMON_REVERSE32(tempWord);
                CC_PalMemCopy(&dst_ptr[ii*CC_32BIT_WORD_SIZE], (uint8_t *)&tempWord, CC_32BIT_WORD_SIZE);
        }
        WAIT_SRAM_DATA_READY;
        PKA_MUTEX_UNLOCK;
        return;

}


/***********      PkaCopyDataFromPkaReg      **********************/
/**
 * @brief This function copies data from PKA register into output buffer .
 *
 *        Assumings: - PKA is initialized.
 *                   - Length of extended (by word) registers is placed into LenID entry of
 *                     sizes table.
 *                   - If the extra word of register must be cleared also the user must
 *                     set LenID according to extended register size
 *
 * @return - None.
 *
 */
void PkaCopyDataFromPkaReg(uint32_t *dst_ptr,    /*!< [out] Pointer to destination buffer. */
                            uint32_t  sizeWords, /*!< [in]  Source size in words. */
                            uint32_t  srcReg)    /*!< [in]  Virtual address (number) of source PKA register. */
{
        uint32_t  currAddr;

        PKA_GET_REG_ADDRESS(srcReg, currAddr/*PhysAddr*/);
        PKA_HW_READ_BLOCK_FROM_PKA_MEM(currAddr, dst_ptr, sizeWords );

        return;

}


void PkaCopyByteBuffIntoPkaReg(uint32_t dstReg,     /*!< [out] Virtual address (number) of destination register. */
                            uint32_t lenId,         /*!< [in] ID of entry of regsSizesTable defines registers length with word extension. */
                            const uint8_t *src_ptr, /*!< [in] Pointer to source buffer big endian. */
                            uint32_t size)          /*!< [in] Data size in bytes. */
{
        uint32_t  currAddr;
        uint32_t  regSize;
        uint32_t  sizeWords;
        uint32_t  tempWord;
        uint32_t  remainBytes = size % CC_32BIT_WORD_SIZE;
        uint32_t ii;

        /* copy data from src buffer into PKA register with 0-padding  *
        *  in the last PKA-word                       */
        PKA_GET_REG_ADDRESS(dstReg, currAddr);
    size -= remainBytes;

        PKA_MUTEX_LOCK;
        PKA_WAIT_ON_PKA_DONE();
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_ADDR), currAddr);
        for(ii = 0; ii< size; ii+=CC_32BIT_WORD_SIZE) {
                CC_PalMemCopy((uint8_t *)&tempWord, &src_ptr[ii], CC_32BIT_WORD_SIZE);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), tempWord);
                WAIT_SRAM_DATA_READY;
        }
    if (remainBytes > 0) {
        tempWord = 0;
                CC_PalMemCopy((uint8_t *)&tempWord, &src_ptr[ii], remainBytes);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), tempWord);
                WAIT_SRAM_DATA_READY;
    }
        PKA_MUTEX_UNLOCK;
        /* data size aligned to full PKA-word */
        sizeWords =  ((CALC_32BIT_WORDS_FROM_BYTES(size+remainBytes)+PKA_WORD_SIZE_IN_32BIT_WORDS-1)/PKA_WORD_SIZE_IN_32BIT_WORDS) * \
                     PKA_WORD_SIZE_IN_32BIT_WORDS;
        currAddr = currAddr + sizeWords;

        /* register size in bits */
        PKA_GET_REG_SIZE(regSize, lenId);
        regSize = CALC_FULL_32BIT_WORDS(regSize);

        /* zeroe not significant high words of the register */
        if (regSize > sizeWords) {
                PKA_HW_CLEAR_PKA_MEM(currAddr, regSize - sizeWords);
        }

        return;
}

/***********      PkaCopyDataIntoPkaReg function      **********************/
/**
* @brief This function  copies source data BE byte buffer into PKA register LE word buffer.
*
*        Assumings: - PKA is initialized.
*                   - Length of extended (by word) registers is placed into LenID entry of
*                     sizes table.
*                   - If the extra word of register must be cleared also the user must
*                     set LenID according to extended register size
*
* @return - None.
*
*/
void PkaCopyBeByteBuffIntoPkaReg(uint32_t dstReg,      /*!< [out] Virtual address (number) of destination register. */
                            uint32_t lenId,            /*!< [in] ID of entry of regsSizesTable defines registers length with word extension. */
                            const  uint8_t *src_ptr,   /*!< [in] Pointer to source buffer big endian. */
                            uint32_t sizeWords)        /*!< [in] Data size in word. */
{
        uint32_t  currAddr;
        uint32_t  regSize;
        uint32_t  tempWord;
        uint32_t  size = sizeWords*CC_32BIT_WORD_SIZE;
        int32_t ii;

        /* copy data from src buffer into PKA register with 0-padding  *
        *  in the last PKA-word                       */
        PKA_GET_REG_ADDRESS(dstReg, currAddr);

        PKA_MUTEX_LOCK;
        PKA_WAIT_ON_PKA_DONE();
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_ADDR), currAddr);
        for(ii = size-CC_32BIT_WORD_SIZE; ii >= 0; ii-=CC_32BIT_WORD_SIZE) {
                CC_PalMemCopy((uint8_t *)&tempWord, &src_ptr[ii], CC_32BIT_WORD_SIZE);
                tempWord = CC_COMMON_REVERSE32(tempWord);
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), tempWord);
                WAIT_SRAM_DATA_READY;
        }
        PKA_MUTEX_UNLOCK;
        /* data size aligned to full PKA-word */
        sizeWords =  ((sizeWords+PKA_WORD_SIZE_IN_32BIT_WORDS-1)/PKA_WORD_SIZE_IN_32BIT_WORDS) * \
                     PKA_WORD_SIZE_IN_32BIT_WORDS;
        currAddr = currAddr + sizeWords;

        /* register size in bits */
        PKA_GET_REG_SIZE(regSize, lenId);
        regSize = CALC_FULL_32BIT_WORDS(regSize);

        /* zeroe not significant high words of the register */
        if (regSize > sizeWords) {
                PKA_HW_CLEAR_PKA_MEM(currAddr, regSize - sizeWords);
        }

#if defined PKA_DEBUG && defined DEBUG
        /*! PKA_DEBUG */
        PkaCopyDataFromPkaReg(tempRes, regSize, dstReg);
#endif
        return;
}


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
 * @return - None.
*
*/
void PkaCopyDataIntoPkaReg(uint32_t    dstReg,      /*!< [out] Virtual address (number) of destination register. */
                            uint32_t    lenId,          /*!< [in] ID of entry of regsSizesTable defines registers length with word extension. */
                            const  uint32_t  *src_ptr,  /*!< [in] Pointer to source buffer. */
                            uint32_t    sizeWords ) /*!< [in] Data size in words. */
{
        uint32_t  currAddr;
        uint32_t  regSize;

        /* copy data from src buffer into PKA register with 0-padding  *
        *  in the last PKA-word                       */
        PKA_GET_REG_ADDRESS(dstReg, currAddr);
        PKA_HW_LOAD_BLOCK_TO_PKA_MEM(currAddr, src_ptr, sizeWords);

        /* data size aligned to full PKA-word */
        sizeWords =  ((sizeWords+PKA_WORD_SIZE_IN_32BIT_WORDS-1)/PKA_WORD_SIZE_IN_32BIT_WORDS) * \
                     PKA_WORD_SIZE_IN_32BIT_WORDS;
        currAddr = currAddr + sizeWords;

        /* register size in words */
        PKA_GET_REG_SIZE(regSize, lenId);
        regSize = CALC_FULL_32BIT_WORDS(regSize);

        /* zeroe not significant high words of the register */
        if (regSize > sizeWords) {
                PKA_HW_CLEAR_PKA_MEM(currAddr, regSize - sizeWords);
        }

#if defined PKA_DEBUG && defined DEBUG
        /*! PKA_DEBUG */
        PkaCopyDataFromPkaReg(tempRes, regSize, dstReg);
#endif
        return;
}


/***********      PkaFinishPka function      **********************/
/**
* @brief This function  disables PKA clock .
*
 * @return - None.
*
*/
void PkaFinishPka(void)
{
        /*     disable the PKA clocks      */
        CC_HAL_WRITE_REGISTER( CC_REG_OFFSET(CRY_KERNEL,PKA_CLK_ENABLE) , 0x0UL );
        return;
}


/***********      PkaClearPkaRegWords function      **********************/
/**
* @brief This function  clears words in a PKA register .
*
* @return - None.
*
*/
void PkaClearPkaRegWords(uint32_t    pkaReg,        /*!< [in/out] Virtual address (number) of the register. */
                          uint32_t   addrWordOffset )   /*!< [in] Word offset to start clearing. */
{
        uint32_t  currAddr;
        uint32_t  regSize;

        /* copy data from src buffer into PKA register with 0-padding  *
        *  in the last PKA-word                       */
        PKA_GET_REG_ADDRESS(pkaReg, currAddr);
        currAddr = currAddr + addrWordOffset;

        /* register size in words */
        PKA_GET_REG_SIZE(regSize, LEN_ID_MAX_BITS);
        regSize = CALC_FULL_32BIT_WORDS(regSize);

        /* zeroe not significant high words of the register */
        if (regSize > addrWordOffset) {
                PKA_HW_CLEAR_PKA_MEM(currAddr, regSize - addrWordOffset);
        }

        return;
}


/***********     PkaIsRegModEqual  function      **********************/
/**
 * The function returns result (x == y mod n).
 * Assumed: n - in reg. 0, lenId = 1.
 *
 * @author reuvenl (6/20/2014)
 *
 * @return True if equal, otherwise false
 */
bool PkaIsRegModEqual(uint32_t reg1,     /*!< [in] Virtual address (number) of the register1. */
                uint32_t reg2,     /*!< [in] Virtual address (number) of the register2. */
                uint32_t regTmp1,  /*!< [in] Virtual address (number) of the tmp register1. */
                uint32_t regTmp2)  /*!< [in] Virtual address (number) of the tmp register2. */
{
        uint32_t status;

        PKA_REDUCE(LEN_ID_N_BITS, reg1, regTmp1);
        PKA_REDUCE(LEN_ID_N_BITS, reg2, regTmp2);
        PKA_COMPARE_STATUS(LEN_ID_N_PKA_REG_BITS, regTmp1, regTmp2, status);
        return(bool)(status==1);
}


/***********      PkaGetBitFromPkaReg function     **********************/
/**
 * @brief This function returns bit i from PKA register.
 *
 * @return - returns the bit number i (counting from left).
 *
 */
uint8_t  PkaGetBitFromPkaReg(uint32_t   rX,    /*!< [in] Virtual pointer to PKA register. */
                             uint32_t LenID,   /*!< [in] ID of entry of regsSizesTable containing rX register length
                            with word extension. */
                             int32_t  i,       /*!< [in] Number of bit to be tested. */
                             uint32_t   rT)    /*!< [in] Temp register. If it is not necessary to keep rX, then
                            set rT=rX for saving memory space. */
{
        uint32_t j;
        uint32_t numWords, numBits; /* number shifts by word and by bit */
        uint32_t bitVal;

        /* copy extended rX=>rT */
        if (rX != rT)
                PKA_COPY(LEN_ID_MAX_BITS, rT/*dst*/, rX/*src*/);

        /* number shifts */
        numWords = i / CC_BITS_IN_32BIT_WORD;
        numBits =  i % CC_BITS_IN_32BIT_WORD;

        /* shift by words */
        for (j = 0; j < numWords; j++) {
                PKA_SHR_FILL0(LenID+1, rT/*Result*/, rT/*N*/, CC_BITS_IN_32BIT_WORD-1/*S*/);
        }

        /* shift by bits */
        if (numBits >= 1)
                PKA_SHR_FILL0(LenID+1, rT/*Result*/, rT/*N*/, numBits-1/*S*/);

        /* test LS Bit */
        PKA_READ_BIT0( LenID+1, rT/**/, bitVal);
        return(bitVal);


}


/***********      PkaExecFullModInv function     **********************/
/**
 * @brief This function calculates modular inversion Res = 1/B mod N for both odd and even modulus.
 *
 *        The function works with virtual pointers to PKA registers (sequence numbers)
 *        and does the following:
 *
 *        1. Checks the parity of modulus N (in register 0) and operand B. If they both are even,
 *           returns an error (inverse is not exist)
 *        2. If the modulus is odd, then calls the LLF_PKI_PKA_ModInv function for calculating
 *           the inverse.
 *        3. If the modulus is even, then the function performs the following:
 *           3.1  Saves modulus N: rT0<=N;
 *           3.2. Sets B into reg N: N<=B.
 *           3.3. Res = N^-1 mod B (call LLF_PKI_PKA_ModInv ); Restore mod: N<=rT0;
 *           3.4. rT0 = high(N*N^-1) = LLF_PKI_PKA_HMul(N,Res,rT0);
 *           3.5. Shift right rT0 >> 32;
 *           3.6. rT1 = low(N*N^-1) = LLF_PKI_PKA_LMul(N,Res,rT1);
 *           3.7. Res = rT0 / B : call LLF_PKI_PKA_LongDiv(rT0,B,Res);
 *           3.7. rT0 = rT1 / B : call LLF_PKI_PKA_Div(rT1,B,rT0);
 *           3.8. Res = Res + rT0 : ModAdd(Res,rT0,Res);
 *           3.9. If reminder of division > 0, then Res= Res+1;
 *           3.10. Res = N-Res;
 *        4. Exit.
 *
 *     NOTE:
 *       -  The operand B shal be rewritten by GCD(N,B).
 *       -  The function needs 6 PKA regs: N(0), OpB, Res, rT0, rT1, rT2.
 *       -  PKA sizes table entrys must be set:  0 - exact modSizeBits, 1 - modSizeBits+32 bits,
 *       -  Before executing modular operations, the modulus must be set into r0 register of PKA.
 *       -  The function not checks the input parameters, because they must be checked previously.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t PkaExecFullModInv(int8_t         OpB,  /*!< [in] Operand B: virtual register pointer. Valid values: 0 <= OpA <= 31. */
                              int8_t         Res,  /*!< [out] Virtual register pointer for result data. Valid values: 0 <= Res <= 31. */
                              int8_t         rT0,  /*!< [in] The virtual pointers to temp register. */
                              int8_t         rT1,  /*!< [in] The virtual pointers to temp register. */
                              int8_t         rT2,  /*!< [in] The virtual pointers to temp register. */
                              int8_t         rT3)  /*!< [in] The virtual pointers to temp register. */
{
        CCError_t error = CC_OK;
        uint32_t ModSizeBits, ModSizeWords;
        uint32_t status;
        uint32_t bitVal;
        /* virtual pointer to modulus register, by default: N=0 */
        uint8_t  N = PKA_REG_N;

        /* get modulus size */
        PKA_GET_REG_SIZE(ModSizeBits, LEN_ID_N_BITS);
        ModSizeWords = CALC_FULL_32BIT_WORDS(ModSizeBits);

        /* Step 1.  Check the parity of the modulus  */
        /* test: is the modulus even? */
        PKA_READ_BIT0(LEN_ID_N_PKA_REG_BITS, PKA_REG_N/*N*/, bitVal);
        if (bitVal == 1 /*odd N*/) {
                /* Step 2.  Process case of odd modulus      */
                PKA_MOD_INV(LEN_ID_N_BITS, Res, OpB);
        } else { /*even N*/
                /* Step 3. Process case of even modulus      */
        /* in case of even B: calculate GCD and return error message, */
                /*  that inverse does not exists                              */
                /* check, is the operand B odd or even */
                PKA_READ_BIT0(LEN_ID_N_PKA_REG_BITS, OpB, bitVal);
                if (bitVal == 0) {
                        return PKA_INVERSION_NOT_EXISTS_ERROR;
                }

                /* in case of odd B: calculate modular inverse and GCD        */
                /* 3.1. Save previous modulus also into rT0 and into rT1 (rT1 - working copy)*/
                PKA_COPY(LEN_ID_MAX_BITS/*LenID*/, rT0/*OpDest*/, N/*OpSrc*/);
                PKA_COPY(LEN_ID_MAX_BITS/*LenID*/, rT1/*OpDest*/, N/*OpSrc*/);

                /* 3.2. Set OpB into modulus register 0 ) */
                PKA_COPY(LEN_ID_MAX_BITS/*LenID*/, N/*OpDest*/, OpB/*OpSrc*/);

                /* 3.3 Calculate Res =  1/N mod B  */
                PKA_MOD_INV(LEN_ID_N_BITS/*LenID*/, Res, rT1/*mod N*/);


                /* restore modulus */
                PKA_COPY(LEN_ID_MAX_BITS/*LenID*/, N/*OpDest*/, rT0/*OpSrc*/);

                /* 3.4. Calculate rT0 = PKA_MUL_HIGH(N*Res) i.e. HighHalf + 1 word of(N*Res)
                        Note: LenId=0, because this operation adds extra word itself */
                PKA_MUL_HIGH(LEN_ID_N_BITS, rT0/*Result*/, N, Res);


                /* 3.5. Shift right rT0 for deleting 1 low word - no need in new HW */

                /* 3.6. Calculate rT2 = PKA_MUL_LOW(N*Res) i.e. LowHalf of(N*Res) */
                PKA_MUL_LOW(LEN_ID_N_BITS, rT2/*Result*/, N, Res);

                /* 3.6. Divide long num Res = (rT1 * 2**(ModSizeBits - 32))/B */
                error = PkaDivLongNum(LEN_ID_N_BITS,              /*LenID of exact size*/
                                       rT0,                /*numerator*/
                                       CC_BITS_IN_32BIT_WORD*ModSizeWords+CC_BITS_IN_32BIT_WORD,     /*Shift*/
                                       OpB,                /*divider*/
                                       Res,                /*result*/
                                       rT1,
                                       rT3);

                if (error != CC_OK) {
                        return error;
                }

                /* 3.7. Subtract 1 from low part and divide it by B */
                PKA_SUB_IM(LEN_ID_N_PKA_REG_BITS, rT2/*Result*/, rT2/*numerat*/, 1/*OpB*/);
                PKA_DIV(LEN_ID_N_PKA_REG_BITS, rT0/*Result*/, rT2/*numerat*/, OpB/*divider*/);

                /* 3.8. Calculate: Res = Res+rT0, Res=Res+1, Res = N - Res; */
                PKA_ADD(LEN_ID_N_PKA_REG_BITS, Res, Res, rT0);

                /* 3.9. If remainder rT2 is not 0, then add 1 to rT0 result */
                PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, rT2/*OpA*/, 0/*OpB*/, status);
                if (status != 1) {
                        PKA_ADD_IM(LEN_ID_N_PKA_REG_BITS, Res, Res, 1 );
                }
                /* 3.10. Res = N - Res; */
                PKA_SUB(LEN_ID_N_PKA_REG_BITS, Res, N, Res);
        }

        return error;
}




