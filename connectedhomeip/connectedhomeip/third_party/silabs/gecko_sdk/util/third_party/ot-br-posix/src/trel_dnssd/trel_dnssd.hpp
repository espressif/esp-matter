/*
 *    Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes definitions for TREL DNS-SD over mDNS.
 */

#ifndef OTBR_AGENT_TREL_DNSSD_HPP_
#define OTBR_AGENT_TREL_DNSSD_HPP_

#if OTBR_ENABLE_TREL

#include <assert.h>
#include <utility>

#include <openthread/instance.h>

#include "common/types.hpp"
#include "mdns/mdns.hpp"
#include "ncp/ncp_openthread.hpp"

namespace otbr {

namespace TrelDnssd {

/**
 * @addtogroup border-router-trel-dnssd
 *
 * @brief
 *   This module includes definition for TREL DNS-SD over mDNS.
 *
 * @{
 */

class TrelDnssd
{
public:
    /**
     * This constructor initializes the TrelDnssd instance.
     *
     * @param[in] aNcp        A reference to the OpenThread Controller instance.
     * @param[in] aPublisher  A reference to the mDNS Publisher.
     *
     */
    explicit TrelDnssd(Ncp::ControllerOpenThread &aNcp, Mdns::Publisher &aPublisher);

    /**
     * This method initializes the TrelDnssd instance.
     *
     * @param[in] aTrelNetif  The network interface for discovering TREL peers.
     *
     */
    void Initialize(std::string aTrelNetif);

    /**
     * This method starts browsing for TREL peers.
     *
     */
    void StartBrowse(void);

    /**
     * This method stops browsing for TREL peers.
     *
     */
    void StopBrowse(void);

    /**
     * This method registers the TREL service to DNS-SD.
     *
     * @param[in] aPort         The UDP port of TREL service.
     * @param[in] aTxtData      The TXT data of TREL service.
     * @param[in] aTxtLength    The TXT length of TREL service.
     *
     */
    void RegisterService(uint16_t aPort, const uint8_t *aTxtData, uint8_t aTxtLength);

    /**
     * This method removes the TREL service from DNS-SD.
     *
     */
    void UnregisterService(void);

    /**
     * This method notifies that mDNS Publisher is ready.
     *
     */
    void OnMdnsPublisherReady(void);

private:
    static constexpr size_t   kPeerCacheSize             = 256;
    static constexpr uint16_t kCheckNetifReadyIntervalMs = 5000;

    struct RegisterInfo
    {
        uint16_t                               mPort = 0;
        std::vector<Mdns::Publisher::TxtEntry> mTxtEntries;
        std::string                            mInstanceName;

        bool IsValid(void) const { return mPort > 0; }
        bool IsPublished(void) const { return !mInstanceName.empty(); }
        void Assign(uint16_t aPort, const uint8_t *aTxtData, uint8_t aTxtLength);
        void Clear(void);
    };

    using Clock = std::chrono::system_clock;

    struct Peer
    {
        static const char kTxtRecordExtAddressKey[];

        explicit Peer(std::vector<uint8_t> aTxtData, const otSockAddr &aSockAddr)
            : mDiscoverTime(Clock::now())
            , mTxtData(std::move(aTxtData))
            , mSockAddr(aSockAddr)
        {
            ReadExtAddrFromTxtData();
        }

        void ReadExtAddrFromTxtData(void);

        Clock::time_point    mDiscoverTime;
        std::vector<uint8_t> mTxtData;
        otSockAddr           mSockAddr;
        otExtAddress         mExtAddr;
        bool                 mValid = false;
    };

    using PeerMap = std::map<std::string, Peer>;

    bool        IsInitialized(void) const { return !mTrelNetif.empty(); }
    bool        IsReady(void) const;
    void        OnBecomeReady(void);
    void        CheckTrelNetifReady(void);
    std::string GetTrelInstanceName(void);
    void        PublishTrelService(void);
    void        UnpublishTrelService(void);
    static void HandlePublishTrelServiceError(otbrError aError);
    static void HandleUnpublishTrelServiceError(otbrError aError);
    void        OnTrelServiceInstanceResolved(const std::string &                            aType,
                                              const Mdns::Publisher::DiscoveredInstanceInfo &aInstanceInfo);
    void        OnTrelServiceInstanceAdded(const Mdns::Publisher::DiscoveredInstanceInfo &aInstanceInfo);
    void        OnTrelServiceInstanceRemoved(const std::string &aInstanceName);

    void     NotifyRemovePeer(const Peer &aPeer);
    void     CheckPeersNumLimit(void);
    void     RemoveAllPeers(void);
    uint16_t CountDuplicatePeers(const Peer &aPeer);

    Mdns::Publisher &          mPublisher;
    Ncp::ControllerOpenThread &mNcp;
    TaskRunner                 mTaskRunner;
    std::string                mTrelNetif;
    uint32_t                   mTrelNetifIndex = 0;
    uint64_t                   mSubscriberId   = 0;
    RegisterInfo               mRegisterInfo;
    PeerMap                    mPeers;
    bool                       mMdnsPublisherReady = false;
};

/**
 * @}
 */

} // namespace TrelDnssd

} // namespace otbr

#endif // OTBR_ENABLE_TREL

#endif // OTBR_AGENT_TREL_DNSSD_HPP_
