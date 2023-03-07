/*
 *    Copyright (c) 2020, The OpenThread Authors.
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
 *   The file implements the ND Proxy management.
 */

#define OTBR_LOG_TAG "NDPROXY"

#include "backbone_router/nd_proxy.hpp"

#if OTBR_ENABLE_DUA_ROUTING

#include <openthread/backbone_router_ftd.h>

#include <assert.h>
#include <net/if.h>
#include <netinet/icmp6.h>
#include <netinet/ip6.h>
#include <sys/ioctl.h>
#include <unistd.h>

#if __linux__
#include <linux/netfilter.h>
#else
#error "Platform not supported"
#endif

#include "backbone_router/constants.hpp"
#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "common/types.hpp"
#include "utils/system_utils.hpp"

namespace otbr {
namespace BackboneRouter {

void NdProxyManager::Enable(const Ip6Prefix &aDomainPrefix)
{
    otbrError error = OTBR_ERROR_NONE;

    VerifyOrExit(!IsEnabled());

    assert(aDomainPrefix.IsValid());
    mDomainPrefix = aDomainPrefix;

    SuccessOrExit(error = InitIcmp6RawSocket());
    SuccessOrExit(error = UpdateMacAddress());
    SuccessOrExit(error = InitNetfilterQueue());

    // Add ip6tables rule for unicast ICMPv6 messages
    VerifyOrExit(SystemUtils::ExecuteCommand(
                     "ip6tables -t raw -A PREROUTING -6 -d %s -p icmpv6 --icmpv6-type neighbor-solicitation -i %s -j "
                     "NFQUEUE --queue-num 88",
                     mDomainPrefix.ToString().c_str(), mBackboneInterfaceName.c_str()) == 0,
                 error = OTBR_ERROR_ERRNO);

exit:
    if (error != OTBR_ERROR_NONE)
    {
        FiniNetfilterQueue();
        FiniIcmp6RawSocket();
    }

    otbrLogResult(error, "NdProxyManager: %s", __FUNCTION__);
}

void NdProxyManager::Disable(void)
{
    otbrError error = OTBR_ERROR_NONE;

    VerifyOrExit(IsEnabled());

    FiniNetfilterQueue();
    FiniIcmp6RawSocket();

    // Remove ip6tables rule for unicast ICMPv6 messages
    VerifyOrExit(SystemUtils::ExecuteCommand(
                     "ip6tables -t raw -D PREROUTING -6 -d %s -p icmpv6 --icmpv6-type neighbor-solicitation -i %s -j "
                     "NFQUEUE --queue-num 88",
                     mDomainPrefix.ToString().c_str(), mBackboneInterfaceName.c_str()) == 0,
                 error = OTBR_ERROR_ERRNO);

exit:
    otbrLogResult(error, "NdProxyManager: %s", __FUNCTION__);
}

void NdProxyManager::Init(void)
{
    mBackboneIfIndex = if_nametoindex(mBackboneInterfaceName.c_str());
    VerifyOrDie(mBackboneIfIndex > 0, "if_nametoindex failed");
}

void NdProxyManager::Update(MainloopContext &aMainloop)
{
    if (mIcmp6RawSock >= 0)
    {
        FD_SET(mIcmp6RawSock, &aMainloop.mReadFdSet);
        aMainloop.mMaxFd = std::max(aMainloop.mMaxFd, mIcmp6RawSock);
    }

    if (mUnicastNsQueueSock >= 0)
    {
        FD_SET(mUnicastNsQueueSock, &aMainloop.mReadFdSet);
        aMainloop.mMaxFd = std::max(aMainloop.mMaxFd, mUnicastNsQueueSock);
    }
}

void NdProxyManager::Process(const MainloopContext &aMainloop)
{
    VerifyOrExit(IsEnabled());

    if (FD_ISSET(mIcmp6RawSock, &aMainloop.mReadFdSet))
    {
        ProcessMulticastNeighborSolicition();
    }

    if (FD_ISSET(mUnicastNsQueueSock, &aMainloop.mReadFdSet))
    {
        ProcessUnicastNeighborSolicition();
    }
exit:
    return;
}

void NdProxyManager::ProcessMulticastNeighborSolicition()
{
    struct msghdr     msghdr;
    sockaddr_in6      sin6;
    struct iovec      iovec;
    ssize_t           len;
    struct icmp6_hdr *icmp6header;
    struct cmsghdr *  cmsghdr;
    unsigned char     cbuf[2 * CMSG_SPACE(sizeof(struct in6_pktinfo))];
    uint8_t           packet[kMaxICMP6PacketSize];
    otbrError         error = OTBR_ERROR_NONE;
    bool              found = false;

    iovec.iov_len  = kMaxICMP6PacketSize;
    iovec.iov_base = packet;

    msghdr.msg_name       = &sin6;
    msghdr.msg_namelen    = sizeof(sin6);
    msghdr.msg_iov        = &iovec;
    msghdr.msg_iovlen     = 1;
    msghdr.msg_control    = cbuf;
    msghdr.msg_controllen = sizeof(cbuf);

    len = recvmsg(mIcmp6RawSock, &msghdr, 0);

    VerifyOrExit(len >= static_cast<ssize_t>(sizeof(struct icmp6_hdr)), error = OTBR_ERROR_ERRNO);

    {
        Ip6Address &src = *reinterpret_cast<Ip6Address *>(&sin6.sin6_addr);

        icmp6header = reinterpret_cast<icmp6_hdr *>(packet);

        // only process neighbor solicit
        VerifyOrExit(icmp6header->icmp6_type == ND_NEIGHBOR_SOLICIT, error = OTBR_ERROR_PARSE);

        otbrLogDebug("NdProxyManager: Received ND-NS from %s", src.ToString().c_str());

        for (cmsghdr = CMSG_FIRSTHDR(&msghdr); cmsghdr; cmsghdr = CMSG_NXTHDR(&msghdr, cmsghdr))
        {
            if (cmsghdr->cmsg_level != IPPROTO_IPV6)
            {
                continue;
            }

            switch (cmsghdr->cmsg_type)
            {
            case IPV6_PKTINFO:
                if (cmsghdr->cmsg_len == CMSG_LEN(sizeof(struct in6_pktinfo)))
                {
                    struct in6_pktinfo *pktinfo = (struct in6_pktinfo *)CMSG_DATA(cmsghdr);
                    Ip6Address &        dst     = *reinterpret_cast<Ip6Address *>(&pktinfo->ipi6_addr);
                    uint32_t            ifindex = pktinfo->ipi6_ifindex;

                    for (const Ip6Address &ipaddr : mNdProxySet)
                    {
                        if (ipaddr.ToSolicitedNodeMulticastAddress() == dst)
                        {
                            found = true;
                            break;
                        }
                    }

                    otbrLogDebug("NdProxyManager: dst=%s, ifindex=%d, proxying=%s", dst.ToString().c_str(), ifindex,
                                 found ? "Y" : "N");
                }
                break;

            case IPV6_HOPLIMIT:
                if (cmsghdr->cmsg_len == CMSG_LEN(sizeof(int)))
                {
                    int hops = *(int *)CMSG_DATA(cmsghdr);

                    otbrLogDebug("NdProxyManager: hops=%d (%s)", hops, hops == 255 ? "Good" : "Bad");

                    VerifyOrExit(hops == 255);
                }
                break;
            }
        }

        VerifyOrExit(found, error = OTBR_ERROR_NOT_FOUND);

        {
            struct nd_neighbor_solicit *ns     = reinterpret_cast<struct nd_neighbor_solicit *>(packet);
            Ip6Address &                target = *reinterpret_cast<Ip6Address *>(&ns->nd_ns_target);

            otbrLogInfo("NdProxyManager: send solicited NA for multicast NS: src=%s, target=%s", src.ToString().c_str(),
                        target.ToString().c_str());

            SendNeighborAdvertisement(target, src);
        }
    }

exit:
    otbrLogResult(error, "NdProxyManager: %s", __FUNCTION__);
}

void NdProxyManager::ProcessUnicastNeighborSolicition(void)
{
    otbrError error = OTBR_ERROR_NONE;
    char      packet[kMaxICMP6PacketSize];
    ssize_t   len;

    VerifyOrExit((len = recv(mUnicastNsQueueSock, packet, sizeof(packet), 0)) >= 0, error = OTBR_ERROR_ERRNO);
    VerifyOrExit(nfq_handle_packet(mNfqHandler, packet, len) == 0, error = OTBR_ERROR_ERRNO);

    error = OTBR_ERROR_NONE;

exit:
    otbrLogResult(error, "NdProxyManager: %s", __FUNCTION__);
}

void NdProxyManager::HandleBackboneRouterNdProxyEvent(otBackboneRouterNdProxyEvent aEvent, const otIp6Address *aDua)
{
    Ip6Address target;

    if (aEvent != OT_BACKBONE_ROUTER_NDPROXY_CLEARED)
    {
        assert(aDua != nullptr);
        target = Ip6Address(aDua->mFields.m8);
    }

    switch (aEvent)
    {
    case OT_BACKBONE_ROUTER_NDPROXY_ADDED:
    case OT_BACKBONE_ROUTER_NDPROXY_RENEWED:
    {
        bool isNewInsert = mNdProxySet.insert(target).second;

        if (isNewInsert)
        {
            JoinSolicitedNodeMulticastGroup(target);
        }

        SendNeighborAdvertisement(target, Ip6Address::GetLinkLocalAllNodesMulticastAddress());
        break;
    }
    case OT_BACKBONE_ROUTER_NDPROXY_REMOVED:
        mNdProxySet.erase(target);
        LeaveSolicitedNodeMulticastGroup(target);
        break;
    case OT_BACKBONE_ROUTER_NDPROXY_CLEARED:
        for (const Ip6Address &proxingTarget : mNdProxySet)
        {
            LeaveSolicitedNodeMulticastGroup(proxingTarget);
        }
        mNdProxySet.clear();
        break;
    }
}

void NdProxyManager::SendNeighborAdvertisement(const Ip6Address &aTarget, const Ip6Address &aDst)
{
    uint8_t                    packet[kMaxICMP6PacketSize];
    uint16_t                   len = 0;
    struct nd_neighbor_advert &na  = *reinterpret_cast<struct nd_neighbor_advert *>(packet);
    struct nd_opt_hdr &        opt = *reinterpret_cast<struct nd_opt_hdr *>(packet + sizeof(struct nd_neighbor_advert));
    bool                       isSolicited = !aDst.IsMulticast();
    sockaddr_in6               dst;
    otbrError                  error = OTBR_ERROR_NONE;
    otBackboneRouterNdProxyInfo aNdProxyInfo;

    VerifyOrExit(otBackboneRouterGetNdProxyInfo(mNcp.GetInstance(), reinterpret_cast<const otIp6Address *>(&aTarget),
                                                &aNdProxyInfo) == OT_ERROR_NONE,
                 error = OTBR_ERROR_OPENTHREAD);

    memset(packet, 0, sizeof(packet));

    na.nd_na_type = ND_NEIGHBOR_ADVERT;
    na.nd_na_code = 0;
    // set Solicited
    na.nd_na_flags_reserved = isSolicited ? ND_NA_FLAG_SOLICITED : 0;
    // set Router
    na.nd_na_flags_reserved |= ND_NA_FLAG_ROUTER;
    // set Override
    na.nd_na_flags_reserved |= aNdProxyInfo.mTimeSinceLastTransaction <= kDuaRecentTime ? ND_NA_FLAG_OVERRIDE : 0;

    memcpy(&na.nd_na_target, aTarget.m8, sizeof(Ip6Address));
    len += sizeof(struct nd_neighbor_advert);

    opt.nd_opt_type = ND_OPT_TARGET_LINKADDR;
    opt.nd_opt_len  = 1;

    memcpy(reinterpret_cast<uint8_t *>(&opt) + 2, mMacAddress.m8, sizeof(mMacAddress));

    len += (opt.nd_opt_len * 8);

    aDst.CopyTo(dst);

    VerifyOrExit(sendto(mIcmp6RawSock, packet, len, 0, reinterpret_cast<const sockaddr *>(&dst), sizeof(dst)) == len,
                 error = OTBR_ERROR_ERRNO);

exit:
    otbrLogResult(error, "NdProxyManager: %s", __FUNCTION__);
}

otbrError NdProxyManager::UpdateMacAddress(void)
{
    otbrError error = OTBR_ERROR_NONE;

#if !__APPLE__
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, mBackboneInterfaceName.c_str(), sizeof(ifr.ifr_name) - 1);

    VerifyOrExit(ioctl(mIcmp6RawSock, SIOCGIFHWADDR, &ifr) != -1, error = OTBR_ERROR_ERRNO);
    memcpy(mMacAddress.m8, ifr.ifr_hwaddr.sa_data, sizeof(mMacAddress));
#else
    ExitNow(error = OTBR_ERROR_NOT_IMPLEMENTED);
#endif
exit:
    otbrLogResult(error, "NdProxyManager: UpdateMacAddress to %s", mMacAddress.ToString().c_str());
    return error;
}

otbrError NdProxyManager::InitIcmp6RawSocket(void)
{
    otbrError           error = OTBR_ERROR_NONE;
    int                 on    = 1;
    int                 hops  = 255;
    struct icmp6_filter filter;

    mIcmp6RawSock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
    VerifyOrExit(mIcmp6RawSock >= 0, error = OTBR_ERROR_ERRNO);

#if __linux__
    VerifyOrExit(setsockopt(mIcmp6RawSock, SOL_SOCKET, SO_BINDTODEVICE, mBackboneInterfaceName.c_str(),
                            mBackboneInterfaceName.length()) == 0,
                 error = OTBR_ERROR_ERRNO);
#else  // __NetBSD__ || __FreeBSD__ || __APPLE__
    VerifyOrExit(
        setsockopt(mIcmp6RawSock, IPPROTO_IPV6, IPV6_BOUND_IF, mBackboneIfName.c_str(), mBackboneIfName.size()),
        error = OTBR_ERROR_ERRNO);
#endif // __linux__

    VerifyOrExit(setsockopt(mIcmp6RawSock, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof(on)) == 0,
                 error = OTBR_ERROR_ERRNO);
    VerifyOrExit(setsockopt(mIcmp6RawSock, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on, sizeof(on)) == 0,
                 error = OTBR_ERROR_ERRNO);
    VerifyOrExit(setsockopt(mIcmp6RawSock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, sizeof(hops)) == 0,
                 error = OTBR_ERROR_ERRNO);
    VerifyOrExit(setsockopt(mIcmp6RawSock, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &hops, sizeof(hops)) == 0,
                 error = OTBR_ERROR_ERRNO);

    ICMP6_FILTER_SETBLOCKALL(&filter);
    ICMP6_FILTER_SETPASS(ND_NEIGHBOR_SOLICIT, &filter);

    VerifyOrExit(setsockopt(mIcmp6RawSock, IPPROTO_ICMPV6, ICMP6_FILTER, &filter, sizeof(filter)) == 0,
                 error = OTBR_ERROR_ERRNO);
exit:
    if (error != OTBR_ERROR_NONE)
    {
        FiniIcmp6RawSocket();
    }

    return error;
}

void NdProxyManager::FiniIcmp6RawSocket(void)
{
    if (mIcmp6RawSock != -1)
    {
        close(mIcmp6RawSock);
        mIcmp6RawSock = -1;
    }
}

otbrError NdProxyManager::InitNetfilterQueue(void)
{
    otbrError error = OTBR_ERROR_ERRNO;

    VerifyOrExit((mNfqHandler = nfq_open()) != nullptr);
    VerifyOrExit(nfq_unbind_pf(mNfqHandler, AF_INET6) >= 0);
    VerifyOrExit(nfq_bind_pf(mNfqHandler, AF_INET6) >= 0);

    VerifyOrExit((mNfqQueueHandler = nfq_create_queue(mNfqHandler, 88, HandleNetfilterQueue, this)) != nullptr);
    VerifyOrExit(nfq_set_mode(mNfqQueueHandler, NFQNL_COPY_PACKET, 0xffff) >= 0);
    VerifyOrExit((mUnicastNsQueueSock = nfq_fd(mNfqHandler)) >= 0);

    error = OTBR_ERROR_NONE;

exit:
    otbrLogResult(error, "NdProxyManager: %s", __FUNCTION__);

    if (error != OTBR_ERROR_NONE)
    {
        FiniNetfilterQueue();
    }

    return error;
}

void NdProxyManager::FiniNetfilterQueue(void)
{
    if (mUnicastNsQueueSock != -1)
    {
        close(mUnicastNsQueueSock);
        mUnicastNsQueueSock = -1;
    }

    if (mNfqQueueHandler != nullptr)
    {
        nfq_destroy_queue(mNfqQueueHandler);
        mNfqQueueHandler = nullptr;
    }

    if (mNfqHandler != nullptr)
    {
        nfq_close(mNfqHandler);
        mNfqHandler = nullptr;
    }
}

int NdProxyManager::HandleNetfilterQueue(struct nfq_q_handle *aNfQueueHandler,
                                         struct nfgenmsg *    aNfMsg,
                                         struct nfq_data *    aNfData,
                                         void *               aContext)
{
    return static_cast<NdProxyManager *>(aContext)->HandleNetfilterQueue(aNfQueueHandler, aNfMsg, aNfData);
}

int NdProxyManager::HandleNetfilterQueue(struct nfq_q_handle *aNfQueueHandler,
                                         struct nfgenmsg *    aNfMsg,
                                         struct nfq_data *    aNfData)
{
    OTBR_UNUSED_VARIABLE(aNfMsg);

    struct nfqnl_msg_packet_hdr *ph;
    unsigned char *              data;
    uint32_t                     id      = 0;
    int                          ret     = 0;
    int                          len     = 0;
    int                          verdict = NF_ACCEPT;

    Ip6Address        dst;
    Ip6Address        src;
    struct icmp6_hdr *icmp6header = nullptr;
    struct ip6_hdr *  ip6header   = nullptr;
    otbrError         error       = OTBR_ERROR_NONE;

    if ((ph = nfq_get_msg_packet_hdr(aNfData)) != nullptr)
    {
        id = ntohl(ph->packet_id);
        otbrLogDebug("NdProxyManager: %s: id %d", __FUNCTION__, id);
    }

    VerifyOrExit((len = nfq_get_payload(aNfData, &data)) > 0, error = OTBR_ERROR_PARSE);

    ip6header = reinterpret_cast<struct ip6_hdr *>(data);
    src       = *reinterpret_cast<Ip6Address *>(&ip6header->ip6_src);
    dst       = *reinterpret_cast<Ip6Address *>(&ip6header->ip6_dst);

    VerifyOrExit(ip6header->ip6_nxt == IPPROTO_ICMPV6);

    otbrLogDebug("NdProxyManager: Handle Neighbor Solicitation: from %s to %s", src.ToString().c_str(),
                 dst.ToString().c_str());

    icmp6header = reinterpret_cast<struct icmp6_hdr *>(data + sizeof(struct ip6_hdr));
    VerifyOrExit(icmp6header->icmp6_type == ND_NEIGHBOR_SOLICIT);

    VerifyOrExit(mNdProxySet.find(dst) != mNdProxySet.end(), error = OTBR_ERROR_NOT_FOUND);

    {
        struct nd_neighbor_solicit &ns = *reinterpret_cast<struct nd_neighbor_solicit *>(data + sizeof(struct ip6_hdr));
        Ip6Address &                target = *reinterpret_cast<Ip6Address *>(&ns.nd_ns_target);

        otbrLogDebug("NdProxyManager: %s: target: %s, hoplimit %d", __FUNCTION__, target.ToString().c_str(),
                     ip6header->ip6_hlim);
        VerifyOrExit(ip6header->ip6_hlim == 255, error = OTBR_ERROR_PARSE);
        SendNeighborAdvertisement(target, src);
        verdict = NF_DROP;
    }

exit:
    ret = nfq_set_verdict(aNfQueueHandler, id, verdict, len, data);

    otbrLogResult(error, "NdProxyManager: %s (nfq_set_verdict id  %d, ret %d verdict %d)", __FUNCTION__, id, ret,
                  verdict);

    return ret;
}

void NdProxyManager::JoinSolicitedNodeMulticastGroup(const Ip6Address &aTarget) const
{
    ipv6_mreq  mreq;
    otbrError  error                     = OTBR_ERROR_NONE;
    Ip6Address solicitedMulticastAddress = aTarget.ToSolicitedNodeMulticastAddress();

    mreq.ipv6mr_interface = mBackboneIfIndex;
    solicitedMulticastAddress.CopyTo(mreq.ipv6mr_multiaddr);

    VerifyOrExit(setsockopt(mIcmp6RawSock, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq)) == 0,
                 error = OTBR_ERROR_ERRNO);
exit:
    otbrLogResult(error, "NdProxyManager: JoinSolicitedNodeMulticastGroup of %s: %s", aTarget.ToString().c_str(),
                  solicitedMulticastAddress.ToString().c_str());
}

void NdProxyManager::LeaveSolicitedNodeMulticastGroup(const Ip6Address &aTarget) const
{
    ipv6_mreq  mreq;
    otbrError  error                     = OTBR_ERROR_NONE;
    Ip6Address solicitedMulticastAddress = aTarget.ToSolicitedNodeMulticastAddress();

    mreq.ipv6mr_interface = mBackboneIfIndex;
    solicitedMulticastAddress.CopyTo(mreq.ipv6mr_multiaddr);

    VerifyOrExit(setsockopt(mIcmp6RawSock, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &mreq, sizeof(mreq)) == 0,
                 error = OTBR_ERROR_ERRNO);
exit:
    otbrLogResult(error, "NdProxyManager: LeaveSolicitedNodeMulticastGroup of %s: %s", aTarget.ToString().c_str(),
                  solicitedMulticastAddress.ToString().c_str());
}

} // namespace BackboneRouter
} // namespace otbr

#endif // OTBR_ENABLE_DUA_ROUTING
