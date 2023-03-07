/*
 *  Copyright (c) 2017-2018, The OpenThread Authors.
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

#include "utils/steering_data.hpp"

#include <assert.h>
#include <mbedtls/sha256.h>

#include "utils/crc16.hpp"

namespace otbr {

void SteeringData::Init(uint8_t aLength)
{
    assert(aLength <= kMaxSizeOfBloomFilter);

    mLength = aLength;

    Clear();
}

void SteeringData::ComputeJoinerId(const uint8_t *aEui64, uint8_t *aJoinerId)
{
    const size_t           kSizeHashSha256Output = 32;
    const size_t           kSizeEui64            = 8;
    uint8_t                hash[kSizeHashSha256Output];
    mbedtls_sha256_context sha256;

    mbedtls_sha256_init(&sha256);
    mbedtls_sha256_starts(&sha256, 0);
    mbedtls_sha256_update(&sha256, aEui64, kSizeEui64);
    mbedtls_sha256_finish(&sha256, hash);

    memcpy(aJoinerId, hash, kSizeJoinerId);
    aJoinerId[0] |= 2;
}

void SteeringData::ComputeBloomFilter(const uint8_t *aJoinerId)
{
    Crc16          ccitt(Crc16::kCcitt);
    Crc16          ansi(Crc16::kAnsi);
    const uint16_t numBits = mLength * 8;

    for (size_t i = 0; i < kSizeJoinerId; i++)
    {
        uint8_t byte = aJoinerId[i];
        ccitt.Update(byte);
        ansi.Update(byte);
    }

    SetBit(static_cast<uint8_t>(ccitt.Get() % numBits));
    SetBit(static_cast<uint8_t>(ansi.Get() % numBits));
}

} // namespace otbr
