/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements an entropy source based on TRNG.
 *
 */

#include <openthread/platform/entropy.h>

#include <stdio.h>
#include <wiced_platform.h>

#ifndef ENTROPY_DEBUG
#define ENTROPY_DEBUG 0
#endif // ENTROPY_DEBUG

#if (ENTROPY_DEBUG != 0)
#define ENTROPY_TRACE(format, ...) printf(format, ##__VA_ARGS__)
#else
#define ENTROPY_TRACE(...)
#endif

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength)
{
    ENTROPY_TRACE("%s (%p, %d)\n", __FUNCTION__, aOutput, aOutputLength);

    switch (wiced_platform_entropy_get(aOutput, aOutputLength))
    {
    case WICED_BADARG:
        return OT_ERROR_INVALID_ARGS;
    case WICED_SUCCESS:
        return OT_ERROR_NONE;
    default:
        return OT_ERROR_FAILED;
    }
}
