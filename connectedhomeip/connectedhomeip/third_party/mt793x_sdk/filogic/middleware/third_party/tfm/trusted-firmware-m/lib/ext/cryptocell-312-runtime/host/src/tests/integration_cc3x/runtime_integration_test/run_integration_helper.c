/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "cc_pal_types.h"
#include "mbedtls/ctr_drbg.h"

#include "run_integration_helper.h"
#include "run_integration_test.h"
#include "run_integration_pal_log.h"
#include "run_integration_flash.h"

/**
 * Assigns into ppBuffAligned the next aligned 32bit address relative to pBuff.
 *
 * @param pBuff
 * @param ppBuffAligned
 */
void runIt_buffAlign32(uint8_t* pBuff, uint32_t **ppBuffAligned, uint32_t wantedBuffSize, const char * name)
{
    uint32_t address = (uint32_t)pBuff;
    uint32_t remainder = (sizeof(uint32_t) - (address % sizeof(uint32_t))) % sizeof(uint32_t);
    *ppBuffAligned = (uint32_t*)(address + remainder);

    CC_UNUSED_PARAM(wantedBuffSize);
    CC_UNUSED_PARAM(name);

#if defined(RUNIT_DEBUG_ALLOC)
    RUNIT_PRINT_DBG("%-30.30s pBuff[%"PRIxPTR"] ppBuffAligned[%"PRIxPTR"] wantedBuffSize[%04"PRIu32"]\n", name, (uintptr_t)pBuff, (uintptr_t)*ppBuffAligned, wantedBuffSize);
#endif
}

int runIt_unhexify(unsigned char *obuf, const char *ibuf)
{
    #define assert(a) do{} while(0)
    unsigned char c, c2;
    int len = strlen(ibuf) / 2;
    assert(strlen( ibuf ) % 2 == 0); /* must be even number of bytes */

    while (*ibuf != 0)
    {
        c = *ibuf++;
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'a' && c <= 'f')
            c -= 'a' - 10;
        else if (c >= 'A' && c <= 'F')
            c -= 'A' - 10;
        else
            assert(0);

        c2 = *ibuf++;
        if (c2 >= '0' && c2 <= '9')
            c2 -= '0';
        else if (c2 >= 'a' && c2 <= 'f')
            c2 -= 'a' - 10;
        else if (c2 >= 'A' && c2 <= 'F')
            c2 -= 'A' - 10;
        else
            assert(0);

        *obuf++ = (c << 4) | c2;
    }

    return len;
}

void runIt_hexify(unsigned char *obuf, const unsigned char *ibuf, int len)
{
    unsigned char l, h;

    while (len != 0)
    {
        h = *ibuf / 16;
        l = *ibuf % 16;

        if (h < 10)
            *obuf++ = '0' + h;
        else
            *obuf++ = 'a' + h - 10;

        if (l < 10)
            *obuf++ = '0' + l;
        else
            *obuf++ = 'a' + l - 10;

        ++ibuf;
        len--;
    }
}

/**
 * This function just returns data from rand().
 * Although predictable and often similar on multiple
 * runs, this does not result in identical random on
 * each run. So do not use this if the results of a
 * test depend on the random data that is generated.
 *
 * rng_state shall be NULL.
 */
static int runIt_rndStdRand(void *rng_state, unsigned char *output, size_t len)
{
#if !defined(__OpenBSD__)
    size_t i;

    if (rng_state != NULL)
        rng_state = NULL;

    for (i = 0; i < len; ++i)
        output[i] = rand();
#else
    if( rng_state != NULL )
    rng_state = NULL;

    arc4random_buf( output, len );
#endif /* !OpenBSD */

    return (0);
}

/**
 * This function returns random based on a buffer it receives.
 *
 * rng_state shall be a pointer to a rnd_buf_info structure.
 *
 * The number of bytes released from the buffer on each call to
 * the random function is specified by per_call. (Can be between
 * 1 and 4)
 *
 * After the buffer is empty it will return rand();
 */
int runIt_rndBufferRand(void *rng_state, unsigned char *output, size_t len)
{
    rnd_buf_info *info = (rnd_buf_info *) rng_state;
    size_t use_len;

    if (rng_state == NULL)
        return (runIt_rndStdRand( NULL, output, len));

    use_len = len;
    if (len > info->length)
        use_len = info->length;

    if (use_len)
    {
        memcpy(output, info->buf, use_len);
        info->buf += use_len;
        info->length -= use_len;
    }

    if (len - use_len > 0)
        return (runIt_rndStdRand( NULL, output + use_len, len - use_len));

    return (0);
}

int runIt_free(RunItPtr *ptrElement)
{
    int rc = 0;
    RUNIT_ASSERT(ptrElement != NULL);

    mbedtls_free(ptrElement->buf);

bail:
    return rc;
}

int runIt_malloc(RunItPtr *ptrElement, size_t length)
{
    int rc = 0;
    RUNIT_ASSERT(ptrElement != NULL);

    memset(ptrElement, 0, sizeof(*ptrElement));

    ptrElement->buf = mbedtls_calloc(1, length + RUNIT_ALIGN32_SLACK + sizeof(uint32_t));
    runIt_buffAlign32(ptrElement->buf, &(ptrElement->ba), length, "temp");
    ptrElement->length = length;

bail:
    return rc;
}

/**
 * @brief                   Function needed by some of CC API.
 *                          Converts between CC API signature to internal flash read function
 *                          This specific function implementation reads the flash content in newly allocated memory.
 *
 * @param flashAddress      [input]
 * @param memDst            [output]
 * @param sizeToRead        [input]
 * @param context           [input] not used
 *
 * @return                  CC_OK on success
 */
uint32_t runIt_flashReadWrap(void* flashAddress, /*! Flash address to read from */
                             uint8_t *memDst, /*! memory destination to read the data to */
                             uint32_t sizeToRead, /*! size to read from Flash (in bytes) */
                             void* context)
{
    CC_UNUSED_PARAM(context);

    if (runIt_flashRead((uint32_t)flashAddress, memDst, sizeToRead) != RUNIT_ERROR__OK)
        return 1;

    return 0;
}


int runIt_buildRandomBuffer(uint8_t *pBuf, size_t bufSize)
{
    uint32_t chunkSize = 0;
    uint32_t doneSize = 0;

    while (doneSize < bufSize)
    {
        chunkSize = (bufSize - doneSize > MBEDTLS_CTR_DRBG_MAX_REQUEST) ? MBEDTLS_CTR_DRBG_MAX_REQUEST : (bufSize - doneSize);
        if (gpRndContext->rndGenerateVectFunc(gpRndContext->rndState, pBuf + doneSize, chunkSize) == 0)
        {
            doneSize += chunkSize;
        }
        else
        {
            return RUNIT_ERROR__FAIL;
        }
    }

    return RUNIT_ERROR__OK;
}
