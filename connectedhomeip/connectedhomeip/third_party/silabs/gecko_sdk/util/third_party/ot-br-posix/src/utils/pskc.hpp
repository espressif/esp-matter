/*
 *  Copyright (c) 2017, The OpenThread Authors.
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
 *   This file implements the generating pskc function.
 */

#ifndef OTBR_UTILS_PSKC_HPP_
#define OTBR_UTILS_PSKC_HPP_

#include "openthread-br/config.h"

#define OT_EXTENDED_PAN_ID_LENGTH 8
#define OT_ITERATION_COUNTS 16384
#define OT_PASSPHRASE_MAX_LENGTH 255
#define OT_PBKDF2_SALT_MAX_LENGTH 30
#define OT_PSKC_LENGTH 16

#include <stdint.h>
#include <string.h>

#include <mbedtls/cmac.h>

namespace otbr {
namespace Psk {

enum
{
    kPskcStatus_Ok              = 0,
    kPskcStatus_InvalidArgument = 1
};

class Pskc
{
public:
    /**
     * This method computes the PSKc.
     *
     * @param[in] aExtPanId     A pointer to extended PAN ID.
     * @param[in] aNetworkName  A pointer to network name.
     * @param[in] aPassphrase   A pointer to passphrase.
     *
     * @returns The pointer to PSKc value.
     *
     */
    const uint8_t *ComputePskc(const uint8_t *aExtPanId, const char *aNetworkName, const char *aPassphrase);

private:
    void SetSalt(const uint8_t *aExtPanId, const char *aNetworkName);

    char     mSalt[OT_PBKDF2_SALT_MAX_LENGTH];
    uint16_t mSaltLen;
    uint8_t  mPskc[OT_PSKC_LENGTH];
};

} // namespace Psk
} // namespace otbr

#endif // OTBR_UTILS_PSKC_HPP_
