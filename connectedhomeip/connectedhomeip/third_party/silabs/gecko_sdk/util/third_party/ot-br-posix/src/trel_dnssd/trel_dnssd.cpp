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
 *   This file includes implementation of TREL DNS-SD over mDNS.
 */

#if OTBR_ENABLE_TREL

#define OTBR_LOG_TAG "TrelDns"

#include "trel_dnssd/trel_dnssd.hpp"

#include <inttypes.h>
#include <net/if.h>

#include <openthread/instance.h>
#include <openthread/link.h>
#include <openthread/platform/trel.h>

#include "common/code_utils.hpp"
#include "utils/hex.hpp"
#include "utils/string_utils.hpp"

static const char kTrelServiceName[] = "_trel._udp";

static otbr::TrelDnssd::TrelDnssd *sTrelDnssd = nullptr;

void trelDnssdInitialize(const char *aTrelNetif)
{
    sTrelDnssd->Initialize(aTrelNetif);
}

void trelDnssdStartBrowse(void)
{
    sTrelDnssd->StartBrowse();
}

void trelDnssdStopBrowse(void)
{
    sTrelDnssd->StopBrowse();
}

void trelDnssdRegisterService(uint16_t aPort, const uint8_t *aTxtData, uint8_t aTxtLength)
{
    sTrelDnssd->RegisterService(aPort, aTxtData, aTxtLength);
}

void trelDnssdRemoveService(void)
{
    sTrelDnssd->UnregisterService();
}

namespace otbr {

namespace TrelDnssd {

TrelDnssd::TrelDnssd(Ncp::ControllerOpenThread &aNcp, Mdns::Publisher &aPublisher)
    : mPublisher(aPublisher)
    , mNcp(aNcp)
{
    sTrelDnssd = this;
}

void TrelDnssd::Initialize(std::string aTrelNetif)
{
    mTrelNetif = std::move(aTrelNetif);

    if (IsInitialized())
    {
        otbrLogDebug("Initialized on netif \"%s\"", mTrelNetif.c_str());
        CheckTrelNetifReady();
    }
    else
    {
        otbrLogDebug("Not initialized");
    }
}

void TrelDnssd::StartBrowse(void)
{
    VerifyOrExit(IsInitialized());

    otbrLogDebug("Start browsing %s services ...", kTrelServiceName);

    assert(mSubscriberId == 0);
    mSubscriberId = mPublisher.AddSubscriptionCallbacks(
        [this](const std::string &aType, const Mdns::Publisher::DiscoveredInstanceInfo &aInstanceInfo) {
            OnTrelServiceInstanceResolved(aType, aInstanceInfo);
        },
        /* aHostCallback */ nullptr);

    if (IsReady())
    {
        mPublisher.SubscribeService(kTrelServiceName, /* aInstanceName */ "");
    }

exit:
    return;
}

void TrelDnssd::StopBrowse(void)
{
    VerifyOrExit(IsInitialized());

    otbrLogDebug("Stop browsing %s service.", kTrelServiceName);
    assert(mSubscriberId > 0);

    mPublisher.RemoveSubscriptionCallbacks(mSubscriberId);
    mSubscriberId = 0;

    if (IsReady())
    {
        mPublisher.UnsubscribeService(kTrelServiceName, "");
    }

exit:
    return;
}

void TrelDnssd::RegisterService(uint16_t aPort, const uint8_t *aTxtData, uint8_t aTxtLength)
{
    assert(aPort > 0);
    assert(aTxtData != nullptr);

    VerifyOrExit(IsInitialized());

    otbrLogDebug("Register %s service: port=%u, TXT=%d bytes", kTrelServiceName, aPort, aTxtLength);
    otbrDump(OTBR_LOG_DEBUG, OTBR_LOG_TAG, "TXT", aTxtData, aTxtLength);

    if (mRegisterInfo.IsValid() && IsReady())
    {
        UnpublishTrelService();
    }

    mRegisterInfo.Assign(aPort, aTxtData, aTxtLength);

    if (IsReady())
    {
        PublishTrelService();
    }

exit:
    return;
}

void TrelDnssd::UnregisterService(void)
{
    VerifyOrExit(IsInitialized());

    otbrLogDebug("Remove %s service", kTrelServiceName);
    assert(mRegisterInfo.IsValid());

    if (IsReady())
    {
        UnpublishTrelService();
    }

    mRegisterInfo.Clear();

exit:
    return;
}

void TrelDnssd::OnMdnsPublisherReady(void)
{
    VerifyOrExit(IsInitialized());

    otbrLogDebug("mDNS Publisher is Ready");
    mMdnsPublisherReady = true;
    RemoveAllPeers();

    if (mRegisterInfo.IsPublished())
    {
        mRegisterInfo.mInstanceName = "";
    }

    OnBecomeReady();

exit:
    return;
}

void TrelDnssd::OnTrelServiceInstanceResolved(const std::string &                            aType,
                                              const Mdns::Publisher::DiscoveredInstanceInfo &aInstanceInfo)
{
    VerifyOrExit(StringUtils::EqualCaseInsensitive(aType, kTrelServiceName));
    VerifyOrExit(aInstanceInfo.mNetifIndex == mTrelNetifIndex);

    if (aInstanceInfo.mRemoved)
    {
        OnTrelServiceInstanceRemoved(aInstanceInfo.mName);
    }
    else
    {
        OnTrelServiceInstanceAdded(aInstanceInfo);
    }

exit:
    return;
}

std::string TrelDnssd::GetTrelInstanceName(void)
{
    const otExtAddress *extaddr = otLinkGetExtendedAddress(mNcp.GetInstance());
    std::string         name;
    char                nameBuf[sizeof(otExtAddress) * 2 + 1];

    Utils::Bytes2Hex(extaddr->m8, sizeof(otExtAddress), nameBuf);
    name = StringUtils::ToLowercase(nameBuf);

    assert(name.length() == sizeof(otExtAddress) * 2);

    otbrLogDebug("Using instance name %s", name.c_str());
    return name;
}

void TrelDnssd::PublishTrelService(void)
{
    assert(mRegisterInfo.IsValid());
    assert(!mRegisterInfo.IsPublished());
    assert(mTrelNetifIndex > 0);

    mRegisterInfo.mInstanceName = GetTrelInstanceName();
    mPublisher.PublishService(/* aHostName */ "", mRegisterInfo.mInstanceName, kTrelServiceName,
                              Mdns::Publisher::SubTypeList{}, mRegisterInfo.mPort, mRegisterInfo.mTxtEntries,
                              [](otbrError aError) { HandlePublishTrelServiceError(aError); });
}

void TrelDnssd::HandlePublishTrelServiceError(otbrError aError)
{
    if (aError != OTBR_ERROR_NONE)
    {
        otbrLogErr("Failed to publish TREL service: %s. TREL won't be working.", otbrErrorString(aError));
    }
}

void TrelDnssd::UnpublishTrelService(void)
{
    assert(mRegisterInfo.IsValid());
    assert(mRegisterInfo.IsPublished());

    mPublisher.UnpublishService(mRegisterInfo.mInstanceName, kTrelServiceName,
                                [](otbrError aError) { HandleUnpublishTrelServiceError(aError); });
    mRegisterInfo.mInstanceName = "";
}

void TrelDnssd::HandleUnpublishTrelServiceError(otbrError aError)
{
    if (aError != OTBR_ERROR_NONE)
    {
        otbrLogInfo("Failed to unpublish TREL service: %s", otbrErrorString(aError));
    }
}

void TrelDnssd::OnTrelServiceInstanceAdded(const Mdns::Publisher::DiscoveredInstanceInfo &aInstanceInfo)
{
    std::string        instanceName = StringUtils::ToLowercase(aInstanceInfo.mName);
    otPlatTrelPeerInfo peerInfo;

    // Remove any existing TREL service instance before adding
    OnTrelServiceInstanceRemoved(instanceName);

    otbrLogDebug("Peer discovered: %s hostname %s addresses %zu port %d priority %d "
                 "weight %d",
                 aInstanceInfo.mName.c_str(), aInstanceInfo.mHostName.c_str(), aInstanceInfo.mAddresses.size(),
                 aInstanceInfo.mPort, aInstanceInfo.mPriority, aInstanceInfo.mWeight);

    for (const auto &addr : aInstanceInfo.mAddresses)
    {
        otbrLogDebug("Peer address: %s", addr.ToString().c_str());
    }

    if (aInstanceInfo.mAddresses.empty())
    {
        otbrLogWarning("Peer %s does not have any IPv6 address, ignored", aInstanceInfo.mName.c_str());
        ExitNow();
    }

    peerInfo.mRemoved = false;
    memcpy(&peerInfo.mSockAddr.mAddress, &aInstanceInfo.mAddresses[0], sizeof(peerInfo.mSockAddr.mAddress));
    peerInfo.mSockAddr.mPort = aInstanceInfo.mPort;
    peerInfo.mTxtData        = aInstanceInfo.mTxtData.data();
    peerInfo.mTxtLength      = aInstanceInfo.mTxtData.size();

    {
        Peer peer(aInstanceInfo.mTxtData, peerInfo.mSockAddr);

        VerifyOrExit(peer.mValid, otbrLogWarning("Peer %s is invalid", aInstanceInfo.mName.c_str()));

        otPlatTrelHandleDiscoveredPeerInfo(mNcp.GetInstance(), &peerInfo);

        mPeers.emplace(instanceName, peer);
        CheckPeersNumLimit();
    }

exit:
    return;
}

void TrelDnssd::OnTrelServiceInstanceRemoved(const std::string &aInstanceName)
{
    std::string instanceName = StringUtils::ToLowercase(aInstanceName);
    auto        it           = mPeers.find(instanceName);

    VerifyOrExit(it != mPeers.end());

    otbrLogDebug("Peer removed: %s", instanceName.c_str());

    // Remove the peer only when all instances are removed because one peer can have multiple instances if expired
    // instances were not properly removed by mDNS.
    if (CountDuplicatePeers(it->second) == 0)
    {
        NotifyRemovePeer(it->second);
    }

    mPeers.erase(it);

exit:
    return;
}

void TrelDnssd::CheckPeersNumLimit(void)
{
    const PeerMap::value_type *oldestPeer = nullptr;

    VerifyOrExit(mPeers.size() >= kPeerCacheSize);

    for (const auto &entry : mPeers)
    {
        if (oldestPeer == nullptr || entry.second.mDiscoverTime < oldestPeer->second.mDiscoverTime)
        {
            oldestPeer = &entry;
        }
    }

    OnTrelServiceInstanceRemoved(oldestPeer->first);

exit:
    return;
}

void TrelDnssd::NotifyRemovePeer(const Peer &aPeer)
{
    otPlatTrelPeerInfo peerInfo;

    peerInfo.mRemoved   = true;
    peerInfo.mTxtData   = aPeer.mTxtData.data();
    peerInfo.mTxtLength = aPeer.mTxtData.size();
    peerInfo.mSockAddr  = aPeer.mSockAddr;

    otPlatTrelHandleDiscoveredPeerInfo(mNcp.GetInstance(), &peerInfo);
}

void TrelDnssd::RemoveAllPeers(void)
{
    for (const auto &entry : mPeers)
    {
        NotifyRemovePeer(entry.second);
    }

    mPeers.clear();
}

void TrelDnssd::CheckTrelNetifReady(void)
{
    assert(IsInitialized());

    if (mTrelNetifIndex == 0)
    {
        mTrelNetifIndex = if_nametoindex(mTrelNetif.c_str());

        if (mTrelNetifIndex != 0)
        {
            otbrLogDebug("Netif %s is ready: index = %" PRIu32, mTrelNetif.c_str(), mTrelNetifIndex);
            OnBecomeReady();
        }
        else
        {
            uint16_t delay = kCheckNetifReadyIntervalMs;

            otbrLogWarning("Netif %s is not ready (%s), will retry after %d seconds", mTrelNetif.c_str(),
                           strerror(errno), delay / 1000);
            mTaskRunner.Post(Milliseconds(delay), [this]() { CheckTrelNetifReady(); });
        }
    }
}

bool TrelDnssd::IsReady(void) const
{
    assert(IsInitialized());

    return mTrelNetifIndex > 0 && mMdnsPublisherReady;
}

void TrelDnssd::OnBecomeReady(void)
{
    if (IsReady())
    {
        otbrLogInfo("TREL DNS-SD Is Now Ready: Netif=%s(%" PRIu32 "), SubscriberId=%" PRIu64 ", Register=%s!",
                    mTrelNetif.c_str(), mTrelNetifIndex, mSubscriberId, mRegisterInfo.mInstanceName.c_str());

        if (mSubscriberId > 0)
        {
            mPublisher.SubscribeService(kTrelServiceName, /* aInstanceName */ "");
        }

        if (mRegisterInfo.IsValid())
        {
            PublishTrelService();
        }
    }
}

uint16_t TrelDnssd::CountDuplicatePeers(const TrelDnssd::Peer &aPeer)
{
    uint16_t count = 0;

    for (const auto &entry : mPeers)
    {
        if (&entry.second == &aPeer)
        {
            continue;
        }

        if (!memcmp(&entry.second.mSockAddr, &aPeer.mSockAddr, sizeof(otSockAddr)) &&
            !memcmp(&entry.second.mExtAddr, &aPeer.mExtAddr, sizeof(otExtAddress)))
        {
            count++;
        }
    }

    return count;
}

void TrelDnssd::RegisterInfo::Assign(uint16_t aPort, const uint8_t *aTxtData, uint8_t aTxtLength)
{
    otbrError error;

    OTBR_UNUSED_VARIABLE(error);

    assert(!IsPublished());
    assert(aPort > 0);

    mPort = aPort;
    mTxtEntries.clear();

    error = Mdns::Publisher::DecodeTxtData(mTxtEntries, aTxtData, aTxtLength);
    assert(error == OTBR_ERROR_NONE);
}

void TrelDnssd::RegisterInfo::Clear(void)
{
    assert(!IsPublished());

    mPort = 0;
    mTxtEntries.clear();
}

const char TrelDnssd::Peer::kTxtRecordExtAddressKey[] = "xa";

void TrelDnssd::Peer::ReadExtAddrFromTxtData(void)
{
    std::vector<Mdns::Publisher::TxtEntry> txtEntries;

    memset(&mExtAddr, 0, sizeof(mExtAddr));

    SuccessOrExit(Mdns::Publisher::DecodeTxtData(txtEntries, mTxtData.data(), mTxtData.size()));

    for (const auto &txtEntry : txtEntries)
    {
        if (StringUtils::EqualCaseInsensitive(txtEntry.mName, kTxtRecordExtAddressKey))
        {
            VerifyOrExit(txtEntry.mValue.size() == sizeof(mExtAddr));

            memcpy(mExtAddr.m8, txtEntry.mValue.data(), sizeof(mExtAddr));
            mValid = true;
            break;
        }
    }

exit:

    if (!mValid)
    {
        otbrLogInfo("Failed to dissect ExtAddr from peer TXT data");
    }

    return;
}

} // namespace TrelDnssd

} // namespace otbr

#endif // OTBR_ENABLE_TREL
