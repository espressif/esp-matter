/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 * This file includes definitions for OpenThread daemon client.
 */

#ifndef OTBR_WEB_WEB_SERVICE_OT_CLIENT_HPP_
#define OTBR_WEB_WEB_SERVICE_OT_CLIENT_HPP_

#include "openthread-br/config.h"

#include <stdint.h>

namespace otbr {
namespace Web {

#define OT_SCANNED_NET_BUFFER_SIZE 250
#define OT_SET_MAX_DATA_SIZE 250
#define OT_NETWORK_NAME_MAX_SIZE 17
#define OT_HARDWARE_ADDRESS_SIZE 8
#define OT_PREFIX_SIZE 8
#define OT_ROUTER_ROLE 2

struct WpanNetworkInfo
{
    char     mNetworkName[OT_NETWORK_NAME_MAX_SIZE];
    bool     mAllowingJoin;
    uint16_t mPanId;
    uint16_t mChannel;
    uint64_t mExtPanId;
    int8_t   mRssi;
    uint8_t  mHardwareAddress[OT_HARDWARE_ADDRESS_SIZE];
    uint8_t  mPrefix[OT_PREFIX_SIZE];
};

/**
 * This class implements functionality of OpenThread client.
 *
 */
class OpenThreadClient
{
public:
    /**
     * This constructor creates an OpenThread client.
     *
     * @param[in] aNetifName  The Thread network interface name.
     *
     */
    OpenThreadClient(const char *aNetifName);

    /**
     * This destructor destories an OpenThread client.
     *
     */
    ~OpenThreadClient(void);

    /**
     * This method connects to OpenThread daemon.
     *
     * @retval TRUE   Successfully connected to the daemon.
     * @retval FALSE  Failed to connected to the daemon.
     *
     */
    bool Connect(void);

    /**
     * This method executes OpenThread CLI.
     *
     * @param[in] aFormat  C style format string.
     * @param[in] ...      C style format arguments.
     *
     * @returns A pointer to the output if succeeded, otherwise nullptr.
     *
     */
    char *Execute(const char *aFormat, ...);

    /**
     * This method reads from OpenThread CLI.
     *
     * @param[in] aResponse  The expected read response.
     * @param[in] aTimeout   Timeout for the read, in ms.
     *
     * @returns A pointer to the output if the expected response is found, otherwise nullptr.
     *
     */
    char *Read(const char *aResponse, int aTimeout);

    /**
     * This method scans Thread network.
     *
     * @param[out] aNetworks  A pointer to the buffer to store network information.
     * @param[in]  aLength    Number of entries in @p aNetworks.
     *
     * @returns Number of entries found. 0 if none found.
     *
     */
    int Scan(WpanNetworkInfo *aNetworks, int aLength);

    /**
     * This method performs factory reset.
     *
     */
    bool FactoryReset(void);

private:
    void Disconnect(void);
    void DiscardRead(void);

    enum
    {
        kBufferSize     = 1024, ///< Maximum command line input and output buffer.
        kDefaultTimeout = 800,  ///< Default timeout(ms) waiting for a command finish.
    };

    const char *mNetifName;
    char        mBuffer[kBufferSize];
    int         mTimeout; /// Timeout in milliseconds
    int         mSocket;
};

} // namespace Web
} // namespace otbr

#endif // OTBR_WEB_WEB_SERVICE_OT_CLIENT_HPP_
