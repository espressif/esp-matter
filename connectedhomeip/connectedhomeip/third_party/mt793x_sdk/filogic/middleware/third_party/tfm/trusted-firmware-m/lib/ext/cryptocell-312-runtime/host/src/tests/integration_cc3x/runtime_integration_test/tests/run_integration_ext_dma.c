/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <limits.h>

/* cc lib */
#include "cc_aes_defs.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"
#include "cc_hal_plat.h"
#if defined(CC_CONFIG_SUPPORT_EXT_DMA)
#include "mbedtls_aes_ext_dma.h"
#endif

/* mbedtls lib */
#include "mbedtls/cipher.h"
#include "mbedtls/timing.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"

/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static RunItError_t runIt_extDma(void);

/************************************************************
 *
 * static functions
 *
 ************************************************************/
static RunItError_t runIt_extDma(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined(CC_CONFIG_SUPPORT_EXT_DMA)
    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CTR.pdf */
    static const uint8_t KEY[] = { 0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE, 0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81, 0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7, 0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4 };
    static const uint8_t PLAIN[] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
    static const uint8_t CYPHER[] = { 0x60, 0x1E, 0xC3, 0x13, 0x77, 0x57, 0x89, 0xA5, 0xB7, 0xA7, 0xF5, 0x04, 0xBB, 0xF3, 0xD2, 0x28 };
    static const uint8_t NONCE[] = { 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF };

    uint32_t dataLen = sizeof(PLAIN);
    size_t keySize = sizeof(KEY);
    uint8_t *dataInBuff = NULL;
    uint32_t *dataInWords = NULL;
    uint32_t *outputBuffer = NULL;
    uint32_t k = 0, i =0;
    uint32_t sizeInWords = 0;
    uint32_t wordsToRW = 0;
    CCAesIv_t ivBuff = {0};
    uint8_t keyBuff[CC_AES_KEY_MAX_SIZE_IN_BYTES] = {0};

    RunItPtr dataInBuffPtr;
    RunItPtr outputBufferPtr;
    RunItPtr dataInWordsPtr;

    const char* TEST_NAME = "External DMA";
    RUNIT_SUB_TEST_START(TEST_NAME);

    sizeInWords = (dataLen + 3) / sizeof(uint32_t);
    /*
     * you DO NOT have to allocate the buffers in a DMAble address space.
     */
    ALLOC_AND_COPY(dataInBuffPtr, dataInBuff, PLAIN, dataLen);
    ALLOC32(outputBufferPtr, outputBuffer, dataLen);
    ALLOC32(dataInWordsPtr, dataInWords, sizeInWords * sizeof(uint32_t));

    memcpy(ivBuff, NONCE, sizeof(ivBuff));
    memcpy(keyBuff, KEY, sizeof(keyBuff));

    RUNIT_ASSERT_WITH_RESULT(mbedtls_aes_ext_dma_init(keySize * 8, CC_AES_ENCRYPT, CC_AES_MODE_CTR), CC_OK);
    RUNIT_ASSERT_WITH_RESULT(mbedtls_aes_ext_dma_set_key(CC_AES_MODE_CTR, keyBuff, keySize * 8), CC_OK);
    RUNIT_ASSERT_WITH_RESULT(mbedtls_aes_ext_dma_set_iv(CC_AES_MODE_CTR, ivBuff, 16), CC_OK);
    RUNIT_ASSERT_WITH_RESULT(mbedtls_aes_ext_dma_set_data_size(dataLen, CC_AES_MODE_CTR), CC_OK);

    memset(dataInWords, 0, sizeInWords * sizeof(uint32_t));
    memcpy(dataInWords, dataInBuff, dataLen);

    // DATA DIN and DATA DOUT are written and read in 4 Words chunks.
    while (k < sizeInWords)
    {
        if (sizeInWords - k > 4)
        {
            wordsToRW = 4;
        }
        else
        {
            wordsToRW = (sizeInWords - k);
        }

        for (i = 0; i < wordsToRW; i++)
        {
            CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, DIN_BUFFER), dataInWords[i + k]);
        }

        for (i = 0; i < wordsToRW; i++)
        {
            outputBuffer[i + k] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, DOUT_BUFFER));
        }

        k += 4;
    }

    RUNIT_ASSERT_WITH_RESULT(mbedtls_aes_ext_dma_finish(CC_AES_MODE_CTR, ivBuff, 16), CC_OK);

    RUNIT_PRINT_BUF(outputBuffer, sizeInWords * sizeof(uint32_t), "outputBuffer");
    RUNIT_ASSERT(memcmp(outputBuffer, CYPHER, sizeInWords * sizeof(uint32_t)) == 0);

bail:

    FREE_IF_NOT_NULL(dataInBuffPtr);
    FREE_IF_NOT_NULL(outputBufferPtr);
    FREE_IF_NOT_NULL(dataInWordsPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] PLAIN[%"PRIu32"B]",
                                   (uint32_t)keySize, (uint32_t)dataLen);
#endif /* CC_CONFIG_SUPPORT_EXT_DMA */
    return rc;
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_extDmaTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "External DMA";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_extDma() == RUNIT_ERROR__OK);

bail:

    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;
}
