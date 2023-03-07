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

/**
 * @file
 *   This file provides commissioner steering data calculations
 */

#ifndef OTBR_UTILS_STEERING_DATA_HPP_
#define OTBR_UTILS_STEERING_DATA_HPP_

#include "openthread-br/config.h"

#include <stdint.h>
#include <string.h>

namespace otbr {

/**
 * This class represents Steering Data
 *
 */
class SteeringData
{
public:
    enum
    {
        kMaxSizeOfBloomFilter = 16, ///< Max length of bloom filter in bytes.
        kSizeJoinerId         = 8,  ///< Size of Extended Joiner ID.
    };

    /**
     * This method initializes the bloom filter.
     *
     * @param[in] aLength  The length of the bloom filter in bytes.
     *
     */
    void Init(uint8_t aLength);

    /**
     * This method sets all bits in the bloom filter to zero.
     *
     */
    void Clear(void) { memset(mBloomFilter, 0, sizeof(mBloomFilter)); }

    /**
     * Ths method sets all bits in the bloom filter to one.
     *
     */
    void Set(void) { memset(mBloomFilter, 0xff, sizeof(mBloomFilter)); }

    /**
     * This method sets bit @p aBit.
     *
     * @param[in] aBit  The bit offset.
     *
     */
    void SetBit(uint8_t aBit) { mBloomFilter[mLength - 1 - (aBit / 8)] |= 1 << (aBit % 8); }

    /**
     * This method computes the Bloom Filter.
     *
     * @param[in] aJoinerId  Extended address
     *
     */
    void ComputeBloomFilter(const uint8_t *aJoinerId);

    /**
     * This method computes joiner id from EUI64.
     *
     * @param[in]  aEui64     A pointer to EUI64.
     * @param[out] aJoinerId  A pointer to receive joiner id. This pointer can be the same as @p aEui64.
     *
     */
    static void ComputeJoinerId(const uint8_t *aEui64, uint8_t *aJoinerId);

    /**
     * This method returns a pointer to the bloom filter.
     *
     * @returns A pointer to the computed bloom filter.
     *
     */
    const uint8_t *GetBloomFilter(void) const { return mBloomFilter; }

    /**
     * This method returns the length of the bloom filter.
     *
     */
    uint8_t GetLength(void) const { return mLength; }

private:
    uint8_t mBloomFilter[kMaxSizeOfBloomFilter];
    uint8_t mLength;
};

} /* namespace otbr */

#endif // OTBR_UTILS_STEERING_DATA_HPP_
