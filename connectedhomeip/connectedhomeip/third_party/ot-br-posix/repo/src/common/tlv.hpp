/*
 *    Copyright (c) 2017, The OpenThread Authors.
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
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *    POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes definition for Thread Management Framework(TMF) Tlv.
 */

#ifndef OTBR_COMMON_TLV_HPP_
#define OTBR_COMMON_TLV_HPP_

#include "openthread-br/config.h"

#include <stdint.h>
#include <string.h>

namespace otbr {

/**
 * This class implements TMF Tlv functionality.
 *
 */
class Tlv
{
    enum
    {
        kLengthEscape = 0xff, ///< This length value indicates the actual length is of two-bytes length.
    };

public:
    /**
     * This method returns the Tlv type.
     *
     * @returns The Tlv type.
     *
     */
    uint8_t GetType(void) const { return mType; }

    /**
     * This method sets the Tlv type.
     *
     */
    void SetType(uint8_t aType) { mType = aType; }

    /**
     * This method returns the Tlv length.
     *
     * @returns The Tlv length.
     *
     */
    uint16_t GetLength(void) const
    {
        return (mLength != kLengthEscape ? mLength : static_cast<uint16_t>((&mLength)[1] << 8 | (&mLength)[2]));
    }

    /**
     * This method sets the length.
     */
    void SetLength(uint16_t aLength, bool aForceExtended = false)
    {
        if (aLength >= kLengthEscape || aForceExtended)
        {
            mLength       = kLengthEscape;
            (&mLength)[1] = (aLength >> 8);
            (&mLength)[2] = (aLength & 0xff);
        }
        else
        {
            mLength = static_cast<uint8_t>(aLength);
        }
    }

    /**
     * This method returns a pointer to the value.
     *
     * @returns The Tlv value.
     *
     */
    const void *GetValue(void) const
    {
        return reinterpret_cast<const uint8_t *>(this) + sizeof(mType) +
               (mLength != kLengthEscape ? sizeof(mLength) : (sizeof(uint16_t) + sizeof(mLength)));
    }

    /**
     * This method returns the value as a uint16_t.
     *
     * @returns The uint16_t value.
     *
     */
    uint16_t GetValueUInt16(void) const
    {
        const uint8_t *p = static_cast<const uint8_t *>(GetValue());

        return static_cast<uint16_t>(p[0] << 8 | p[1]);
    }

    /**
     * This method returns the value as a uint8_t.
     *
     * @returns The uint8_t value.
     *
     */
    uint8_t GetValueUInt8(void) const { return *static_cast<const uint8_t *>(GetValue()); }

    /**
     * This method sets a uint64_t as the value.
     *
     * @param[in] aValue  The uint64_t value.
     *
     */
    void SetValue(uint64_t aValue)
    {
        uint8_t *value;

        SetLength(sizeof(aValue), false);
        value = static_cast<uint8_t *>(GetValue());
        for (int i = 0; i < int{sizeof(aValue)}; ++i)
        {
            value[i] = (aValue >> (8 * (sizeof(aValue) - i - 1))) & 0xff;
        }
    }

    /**
     * This method sets a uint32_t as the value.
     *
     * @param[in] aValue  The uint32_t value.
     *
     */
    void SetValue(uint32_t aValue)
    {
        uint8_t *value;

        SetLength(sizeof(aValue), false);
        value = static_cast<uint8_t *>(GetValue());
        for (int i = 0; i < int{sizeof(aValue)}; ++i)
        {
            value[i] = (aValue >> (8 * (sizeof(aValue) - i - 1))) & 0xff;
        }
    }

    /**
     * This method sets uint16_t as the value.
     *
     * @param[in] aValue  The uint16_t value.
     *
     */
    void SetValue(uint16_t aValue)
    {
        uint8_t *value;

        SetLength(sizeof(aValue), false);
        value    = static_cast<uint8_t *>(GetValue());
        value[0] = (aValue >> 8);
        value[1] = (aValue & 0xff);
    }

    /**
     * This method sets uint8_t as the value.
     *
     * @param[in] aValue  The uint8_t value.
     *
     */
    void SetValue(uint8_t aValue)
    {
        SetLength(sizeof(aValue), false);
        *static_cast<uint8_t *>(GetValue()) = aValue;
    }

    /**
     * This method sets int8_t as the value.
     *
     * @param[in] aValue  The int8_t value.
     *
     */
    void SetValue(int8_t aValue)
    {
        SetLength(sizeof(aValue), false);
        *static_cast<int8_t *>(GetValue()) = aValue;
    }

    /**
     * This method copies the value.
     */
    void SetValue(const void *aValue, uint16_t aLength, bool aForceExtended = false)
    {
        SetLength(aLength, aForceExtended);
        memcpy(GetValue(), aValue, aLength);
    }

    /**
     * This method returns the pointer to the next Tlv.
     *
     * @returns A pointer to the next Tlv.
     *
     */
    const Tlv *GetNext(void) const
    {
        return reinterpret_cast<const Tlv *>(static_cast<const uint8_t *>(GetValue()) + GetLength());
    }

    /**
     * This method returns the pointer to the next Tlv.
     *
     * @returns A pointer to the next Tlv.
     *
     */
    Tlv *GetNext(void) { return reinterpret_cast<Tlv *>(static_cast<uint8_t *>(GetValue()) + GetLength()); }

private:
    void *GetValue(void)
    {
        return reinterpret_cast<uint8_t *>(this) + sizeof(mType) +
               (mLength != kLengthEscape ? sizeof(mLength) : (sizeof(uint16_t) + sizeof(mLength)));
    }
    uint8_t mType;
    uint8_t mLength;
};

namespace Meshcop {

enum
{
    kState                   = 16,
    kCommissionerId          = 10,
    kCommissionerSessionId   = 11,
    kJoinerDtlsEncapsulation = 17,
    kSteeringData            = 8,
    kJoinerUdpPort           = 18,
    kJoinerIid               = 19,
    kJoinerRouterLocator     = 20,
    kJoinerRouterKek         = 21,
    kUdpEncapsulation        = 48,
    kIPv6Address             = 49,
};

enum
{
    kStateAccepted = 1,
    kStatePending  = 0,
    kStateRejected = -1,
};

} // namespace Meshcop

} // namespace otbr

#endif // OTBR_COMMON_TLV_HPP_
