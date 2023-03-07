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
 *   This file includes definitions for 802.15.4 frame processing.
 */

#ifndef IEEE802154_PACKET_UTILS_HPP_
#define IEEE802154_PACKET_UTILS_HPP_

#include "openthread-core-config.h"

#include <openthread/platform/crypto.h>

#include <stdint.h>
#include "common/error.hpp"
#include "crypto/aes_ecb.hpp"
#include "mac/mac_types.hpp"

/**
 * This class implements Packet Processing.
 *
 */
class TxSecurityProcessing
{
public:
    enum
    {
        kBlockSize = 16, ///< AES-128 block size (bytes).
        kKeyBits   = 128 
    };

    /**
     * This method sets the key.
     *
     * @param[in]  aKey    Pointer to the AES Key to use.
     *
     */
    void SetKey(const uint8_t* aKey) { mKey = aKey; }

    /**
     * This method initializes the AES CCM computation.
     *
     * @param[in]  aHeaderLength     Length of header in bytes.
     * @param[in]  aPlainTextLength  Length of plaintext in bytes.
     * @param[in]  aTagLength        Length of tag in bytes (must be even and in `[kMinTagLength, kMaxTagLength]`).
     * @param[in]  aNonce            A pointer to the nonce.
     * @param[in]  aNonceLength      Length of nonce in bytes.
     *
     */
    void Init(  uint32_t         aHeaderLength,
                uint32_t         aPlainTextLength,
                uint8_t          aTagLength,
                const void       *aNonce,
                uint8_t          aNonceLength);


    /**
     * This method processes the header.
     *
     * @param[in]  aHeader        A pointer to the header.
     * @param[in]  aHeaderLength  Length of header in bytes.
     *
     */
    void Header(const void *aHeader, uint32_t aHeaderLength);

    /**
     * This method processes the payload.
     *
     * @param[inout]  aPlainText   A pointer to the plaintext.
     * @param[inout]  aCipherText  A pointer to the ciphertext.
     * @param[in]     aLength      Payload length in bytes.
     * @param[in]     aMode        Mode to indicate whether to encrypt (`kEncrypt`) or decrypt (`kDecrypt`).
     *
     */
    void Payload(void *aPlainText, void *aCipherText, uint32_t aLength);

    /**
     * This method returns the tag length in bytes.
     *
     * @returns The tag length in bytes.
     *
     */
    uint8_t GetTagLength(void) const { return mTagLength; }

    /**
     * This method generates the tag.
     *
     * @param[out]  aTag        A pointer to the tag (must have `GetTagLength()` bytes).
     *
     */
    void Finalize(void *aTag);

private:
    uint8_t         mBlock[kBlockSize];
    uint8_t         mCtr[kBlockSize];
    uint8_t         mCtrPad[kBlockSize];
    const uint8_t*  mKey;
    uint32_t        mHeaderLength;
    uint32_t        mHeaderCur;
    uint32_t        mPlainTextLength;
    uint32_t        mPlainTextCur;
    uint16_t        mBlockLength;
    uint16_t        mCtrLength;
    uint8_t         mNonceLength;
    uint8_t         mTagLength;
};

/**
 * @}
 *
 */

#endif // IEEE802154_PACKET_UTILS_HPP_