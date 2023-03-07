/*
 *    Copyright (c) 2022, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *    @file
 *          Provides implementations for several functions defined in
 *          openthread/src/core/crypto/crypto_platform.cpp. The purpose
 *          is to bypass mbedtls_entropy_func and use a strong source
 *          for DRBG seed.
 *
 *          Note: this file was not added to any ot-nxp standalone builds.
 *          Currently, it is used only in Matter.
 *
 *          mbedtls_entropy_func is using an accumulator for entropy.
 *          It starts a SHA256 process (1) to update the current accumulator,
 *          but it also starts another SHA256 process (2), before the previous
 *          one is finished, to compute the output hash, which will be the seed.
 *          Furthermore, (1) finishes at the next call of mbedtls_entropy_func.
 *          When using hardware SHA256, this could block access to it, since we may
 *          run into a case when mbedtls_entropy_func is not called anymore, thus
 *          the SHA256 mutex will never be unlocked. This can happen in both Matter
 *          and OT, if usage of hardware SHA256 is enabled.
 */

#include <assert.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <openthread-core-config.h>
#include <utils/code_utils.h>
#include <openthread/platform/entropy.h>

static mbedtls_ctr_drbg_context sCtrDrbgContext;

/* mbedtls_entropy_func alternative, which uses hardware TRNG
 * to gather a seed. data argument used to be a pointer to an
 * entropy context structure, but is never used (third argument
 * of mbedtls_ctr_drb_seed call can be set to NULL).
 */
static int strong_entropy_func(void *data, unsigned char *output, size_t len)
{
    int     result = MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    otError status = OT_ERROR_NONE;

    status = otPlatEntropyGet((uint8_t *)output, (uint16_t)len);
    otEXPECT_ACTION(status != OT_ERROR_NONE, result = 0);

exit:
    OT_UNUSED_VARIABLE(data);
    return result;
}

void otPlatCryptoRandomInit(void)
{
    mbedtls_ctr_drbg_init(&sCtrDrbgContext);

    int rval = mbedtls_ctr_drbg_seed(&sCtrDrbgContext, strong_entropy_func, NULL, NULL, 0);
    assert(rval == 0);
    OT_UNUSED_VARIABLE(rval);
}

void otPlatCryptoRandomDeinit(void)
{
    mbedtls_ctr_drbg_free(&sCtrDrbgContext);
}

otError otPlatCryptoRandomGet(uint8_t *aBuffer, uint16_t aSize)
{
    otError status = OT_ERROR_FAILED;
    int     result = 0;

    result = mbedtls_ctr_drbg_random(&sCtrDrbgContext, (unsigned char *)aBuffer, (size_t)aSize);
    otEXPECT_ACTION(result != 0, status = OT_ERROR_NONE);

exit:
    return status;
}
