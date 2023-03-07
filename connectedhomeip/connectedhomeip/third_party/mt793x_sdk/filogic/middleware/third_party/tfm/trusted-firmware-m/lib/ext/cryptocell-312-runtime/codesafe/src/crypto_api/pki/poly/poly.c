/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "pka.h"
#include "pka_error.h"
#include "pka_hw_defs.h"
#include "mbedtls_cc_poly.h"
#include "mbedtls_cc_poly_error.h"
#include "cc_common.h"
#include "poly.h"


/* PKA registers*/

#define  PRIME_REG  regTemps[0]
#define  NP_REG     regTemps[1]
#define  ACC_REG    regTemps[2]
#define  KEY_R_REG  regTemps[3]
#define  KEY_S_REG  regTemps[4]
#define  DATA_REG   regTemps[5]
#define  POLY_PKA_REGS_NUM (6+2) // +2 temp registers

/* Macro for read non aligned word from RAM */
#define GET_NON_ALIGNED_WORD(m_w0, m_w1, m_shift, m_shift_)    (((m_w0)>>(m_shift)) | ((m_w1)<<(m_shift_)))

/* Global buffers  */
extern const int8_t regTemps[PKA_MAX_COUNT_OF_PHYS_MEM_REGS];

/* Mask for Key "r" buffer: clearing 4 high bits of indexes 3, 7, 11, 15; clearing low 2 bits of indexes 4,8,12 */
static const uint32_t g_PolyMaskKeyR[CC_POLY_KEY_SIZE_IN_WORDS/2] = {0x0fffffff,0x0ffffffc,0x0ffffffc,0x0ffffffc};
/* POLY PRIME  3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB */
static const uint32_t g_PolyPrime[POLY_PRIME_SIZE_IN_WORDS] = {0xfffffffb,0xffffffff,0xffffffff,0xffffffff,0x3};
/* Barrett tag Np = 800000000 00000000*/
static const uint32_t g_PolyNp[] = {0x00000000,0x00000000,0x00000080};


/**
 * The function loads  remaining bytes (not full block) of data together with
 * setting of rxtra bit according to Poly1305 algorithm.
 *
 * Assuming register size is 128+64 bits according to PKA multiplier 64x!6
 * Note - this function assumes the PKA engine is already working
 *
 */
static void PolyAccRemainBlock(
        const uint8_t *pSrc,     /*!< [in] Pointer to source (little endian) buffer, aligned down to 32-bit word . */
        uint32_t sizeBytes,      /*!< [in] Size of remaining data in words, should be in range [1...4]. */
        bool isPolyAeadMode,     /*!< [in] Flag indicating padding 0's to short buffer */
        const uint32_t dataRegSramAddr)/*!< [in] SRAM address of DATA_REG (not changed during calculations) */
{
    uint32_t  tmp[CC_POLY_BLOCK_SIZE_IN_WORDS+2] = {0};
    uint32_t  i;

    /* load block into words buffer and set high bit 1 */
    CC_PalMemCopy((uint8_t*)tmp, pSrc, sizeBytes);
    if(isPolyAeadMode) {
        sizeBytes = CC_POLY_BLOCK_SIZE_IN_BYTES;
    }
    ((uint8_t*)tmp)[sizeBytes] = 1;

    /* set address */
    PKA_WAIT_ON_PKA_DONE();
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_ADDR), dataRegSramAddr);

    /* load block into PKA reg. */
    for(i = 0; i < CC_POLY_BLOCK_SIZE_IN_WORDS+2; i++) {
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), tmp[i]);
    }

    /* acumulate the data and do modular multiplication by keyR */
    PKA_MOD_ADD(LEN_ID_N_BITS, ACC_REG, ACC_REG, DATA_REG);
    PKA_MOD_MUL(LEN_ID_N_BITS, ACC_REG, ACC_REG, KEY_R_REG);

    return;
}



/***********    PolyAccCalc   function      **********************/

/**
 * @brief performs internal opeartions on PKA buffer to calculate the POLY accumulator.
 *  Acc = ((Acc+block)*r) % p.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static uint32_t PolyAccCalc(const uint8_t *pSrc,     /*!< [in] The pointer to the data buff. */
                            size_t   size,      /*!< [in] The size in bytes of data. */
                            bool     isPolyAeadMode)      /*!< [in] Flag indicating padding 0's to short buffer */
{
    uint32_t shift, shift_; /* shift left (in bits) needed for aligning data to 32-bit words */
    uint32_t remSize, blocksCount;
    uint32_t *pSrc32;
    uint32_t i;
    uint32_t dataRegSramAddr;
    uint32_t word0 = 0;

    /* count of full blocks */
    blocksCount = size / CC_POLY_BLOCK_SIZE_IN_BYTES;
    /* remining data: count of bytes in not full 32-bit word */
    remSize = size % CC_POLY_BLOCK_SIZE_IN_BYTES;

    /* count of non aligned bytes and aligned pointer */
    shift = (size_t)pSrc % CC_32BIT_WORD_SIZE;
    shift_ = CC_32BIT_WORD_SIZE - shift;
    pSrc32 = (uint32_t*)((uint32_t)pSrc - shift);


    /* set first non aligned bytes into word0 */
    if(shift) {

        word0 = (uint32_t)pSrc[0];
        if(shift_ > 1){
            word0 |= ((uint32_t)pSrc[1] << 8);
        }
        if(shift_ > 2){
            word0 |= ((uint32_t)pSrc[2] << 16);
        }

        shift = shift << 3; /*now shift is in bits*/
        shift_ = shift_ << 3;
        word0 <<= shift;
    }

    /*---------------------*/
    /* process full blocks */
    /*---------------------*/

    /* get DATA_REG address */
    PKA_GET_REG_ADDRESS(DATA_REG, dataRegSramAddr/*reg.addr*/);

    for(i = 0; i < blocksCount; i++) {

        /* set address of DATA_REG PKA register */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_ADDR), dataRegSramAddr);
        PKA_WAIT_ON_PKA_DONE();

        /* load block of 4 words */
        if(shift) {
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA),
                                  (uint32_t)GET_NON_ALIGNED_WORD(word0,     pSrc32[1], shift, shift_));
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA),
                                  (uint32_t)GET_NON_ALIGNED_WORD(pSrc32[1], pSrc32[2], shift, shift_));
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA),
                                  (uint32_t)GET_NON_ALIGNED_WORD(pSrc32[2], pSrc32[3], shift, shift_));
            word0  = pSrc32[4];
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA),
                                  (uint32_t)GET_NON_ALIGNED_WORD(pSrc32[3], word0, shift, shift_));
        } else {
            /* write data block */
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), pSrc32[0]);
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), pSrc32[1]);
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), pSrc32[2]);
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), pSrc32[3]);
        }
        pSrc32 += CC_POLY_BLOCK_SIZE_IN_WORDS;

        /* set MSBit 129 and zeroe other high bits of register */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), 1UL);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), 0UL);

        /* acumulate the data and do modular multiplication by keyR */
        PKA_MOD_ADD(LEN_ID_N_BITS, ACC_REG, ACC_REG, DATA_REG);
        PKA_MOD_MUL(LEN_ID_N_BITS, ACC_REG, ACC_REG, KEY_R_REG);
    }

    /*-----------------------------------*/
    /* process remainig (not full) block */
    /*-----------------------------------*/
    if(remSize) {
        PolyAccRemainBlock(pSrc + blocksCount*CC_POLY_BLOCK_SIZE_IN_BYTES,
                           remSize, isPolyAeadMode, dataRegSramAddr);
    }
    return CC_OK;
}



/***********    PolyMacCalc   function      **********************/
/**
 * @brief Generates the POLY mac according to RFC 7539 section 2.5.1
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t PolyMacCalc(mbedtls_poly_key  key,        /*!< [in] Poniter to 256 bits of KEY. */
                      const uint8_t         *pAddData,  /*!< [in] Optional - pointer to additional data if any */
                      size_t            addDataSize,    /*!< [in] The size in bytes of the additional data */
                      const uint8_t     *pDataIn,   /*!< [in] Pointer to data buffer to calculate MAC on */
                      size_t            dataInSize, /*!< [in] The size in bytes of the additional data */
                      mbedtls_poly_mac  macRes,     /*!< [out] The calculated MAC */
                      bool              isPolyAeadMode)  /*!< [in] Boolean indicating if the Poly MAC operation is part of AEAD or just poly */

{
    uint32_t  rc = CC_OK;
    uint32_t  *pKeyR = key;
    uint32_t  lastBlock[CC_POLY_BLOCK_SIZE_IN_WORDS];
    uint32_t  i = 0;
    uint32_t pkaRegsNum = POLY_PKA_REGS_NUM;

    // verify inputs
    if ((key == NULL) ||
        ((pDataIn == NULL) ^ (dataInSize == 0)) ||
        (macRes == NULL) ||
        ((pAddData == NULL) ^ (addDataSize == 0))) {
        return CC_POLY_DATA_INVALID_ERROR;
    }

    /* clamp "KeyR"  */
    for (i = 0; i < CC_POLY_KEY_SIZE_IN_WORDS/2; i++) {
        pKeyR[i] = pKeyR[i] & g_PolyMaskKeyR[i];
    }

    /* initialize the PKA engine on default mode with size of modulus  */
    rc = PkaInitAndMutexLock(POLY_PRIME_SIZE_IN_BITS, &pkaRegsNum);
    if (rc != CC_OK) {
        return rc;
    }
    // clear ACC_REG registers
    PKA_CLEAR(LEN_ID_MAX_BITS, ACC_REG );

    // set values in PKA register for the MAC operation:
    // 1. set the prime number to ((1<<130) -5)
    PkaCopyDataIntoPkaReg(PRIME_REG, LEN_ID_MAX_BITS, g_PolyPrime, CALC_32BIT_WORDS_FROM_BYTES(sizeof(g_PolyPrime)));

    // 2. calculate NP for modulus operation
    PkaCopyDataIntoPkaReg(NP_REG, LEN_ID_MAX_BITS, g_PolyNp, CALC_32BIT_WORDS_FROM_BYTES(sizeof(g_PolyNp)));

    // 3. Copy pKeyR to PKA register #2
    PkaCopyDataIntoPkaReg(KEY_R_REG, LEN_ID_MAX_BITS, pKeyR, CC_POLY_KEY_SIZE_IN_WORDS/2);
    // 4. Copy pKeyS to PKA register #3
    PkaCopyDataIntoPkaReg(KEY_S_REG, LEN_ID_MAX_BITS, pKeyR + CC_POLY_KEY_SIZE_IN_WORDS/2, CC_POLY_KEY_SIZE_IN_WORDS/2);

    /* clear some registers  */
    PKA_CLEAR(LEN_ID_MAX_BITS, DATA_REG);
    PKA_CLEAR(LEN_ID_MAX_BITS, ACC_REG);
    PKA_CLEAR(LEN_ID_MAX_BITS, PKA_REG_T0);
    PKA_CLEAR(LEN_ID_MAX_BITS, PKA_REG_T1);

    /* 5. Process the input data               */
    /*-----------------------------------------*/
    /* process the additional Data */
    if(addDataSize) {
        rc = PolyAccCalc(pAddData, addDataSize, isPolyAeadMode);
        if (rc != CC_OK) {
            goto end_func;
        }
    }
    /*  process the DataIn  input */
    if(dataInSize) {
        rc = PolyAccCalc(pDataIn, dataInSize, isPolyAeadMode);
        if (rc != CC_OK) {
            goto end_func;
        }
    }

    /* on Chacha-Poly mode process the sizes  */
    // RL change name isPolyAeadMode to chaChaPolyMode
    if (isPolyAeadMode) {
        /* Fill lastBlock with 64-bit LE words: addDataSize | dataInSize */
        lastBlock[0] = addDataSize;
        lastBlock[1] = 0;
        lastBlock[2] = dataInSize;
        lastBlock[3] = 0;

        rc = PolyAccCalc((uint8_t*)lastBlock, CC_POLY_BLOCK_SIZE_IN_BYTES, false/*isPolyAeadMode*/);
        if (rc != 0) {
            goto end_func;
        }
    }

    //6. acc = acc+pkeyS
    PKA_ADD(LEN_ID_N_BITS, ACC_REG, ACC_REG, KEY_S_REG);

    //7. copy acc into macRes
    PkaCopyDataFromPkaReg(macRes, CC_POLY_MAC_SIZE_IN_WORDS, ACC_REG);

    end_func:
    PkaFinishAndMutexUnlock(pkaRegsNum);
    return rc;

}

