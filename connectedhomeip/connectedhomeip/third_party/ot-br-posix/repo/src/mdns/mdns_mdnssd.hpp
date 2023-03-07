/*
 *    Copyright (c) 2018, The OpenThread Authors.
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
 *   This file includes definition for mDNS publisher.
 */

#ifndef OTBR_AGENT_MDNS_MDNSSD_HPP_
#define OTBR_AGENT_MDNS_MDNSSD_HPP_

#include <array>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <assert.h>
#include <dns_sd.h>

#include "common/code_utils.hpp"
#include "common/mainloop.hpp"
#include "common/types.hpp"
#include "mdns/mdns.hpp"

namespace otbr {

namespace Mdns {

/**
 * This class implements mDNS publisher with mDNSResponder.
 *
 */
class PublisherMDnsSd : public MainloopProcessor, public Publisher
{
public:
    explicit PublisherMDnsSd(StateCallback aCallback);

    ~PublisherMDnsSd(void) override;

    // Implementation of Mdns::Publisher.

    void UnpublishService(const std::string &aName, const std::string &aType, ResultCallback &&aCallback) override;

    void      UnpublishHost(const std::string &aName, ResultCallback &&aCallback) override;
    void      SubscribeService(const std::string &aType, const std::string &aInstanceName) override;
    void      UnsubscribeService(const std::string &aType, const std::string &aInstanceName) override;
    void      SubscribeHost(const std::string &aHostName) override;
    void      UnsubscribeHost(const std::string &aHostName) override;
    otbrError Start(void) override;
    bool      IsStarted(void) const override;
    void      Stop(void) override;

    // Implementation of MainloopProcessor.

    void Update(MainloopContext &aMainloop) override;
    void Process(const MainloopContext &aMainloop) override;

protected:
    void      PublishServiceImpl(const std::string &aHostName,
                                 const std::string &aName,
                                 const std::string &aType,
                                 const SubTypeList &aSubTypeList,
                                 uint16_t           aPort,
                                 const TxtList &    aTxtList,
                                 ResultCallback &&  aCallback) override;
    void      PublishHostImpl(const std::string &         aName,
                              const std::vector<uint8_t> &aAddress,
                              ResultCallback &&           aCallback) override;
    void      OnServiceResolveFailedImpl(const std::string &aType,
                                         const std::string &aInstanceName,
                                         int32_t            aErrorCode) override;
    void      OnHostResolveFailedImpl(const std::string &aHostName, int32_t aErrorCode) override;
    otbrError DnsErrorToOtbrError(int32_t aErrorCode) override;

private:
    static constexpr uint32_t kDefaultTtl = 10;

    class DnssdServiceRegistration : public ServiceRegistration
    {
    public:
        DnssdServiceRegistration(const std::string &aHostName,
                                 const std::string &aName,
                                 const std::string &aType,
                                 const SubTypeList &aSubTypeList,
                                 uint16_t           aPort,
                                 const TxtList &    aTxtList,
                                 ResultCallback &&  aCallback,
                                 DNSServiceRef      aServiceRef,
                                 PublisherMDnsSd *  aPublisher)
            : ServiceRegistration(aHostName,
                                  aName,
                                  aType,
                                  aSubTypeList,
                                  aPort,
                                  aTxtList,
                                  std::move(aCallback),
                                  aPublisher)
            , mServiceRef(aServiceRef)
        {
        }

        ~DnssdServiceRegistration(void) override;
        const DNSServiceRef &GetServiceRef() const { return mServiceRef; }

    private:
        DNSServiceRef mServiceRef;
    };

    class DnssdHostRegistration : public HostRegistration
    {
    public:
        DnssdHostRegistration(const std::string &         aName,
                              const std::vector<uint8_t> &aAddress,
                              ResultCallback &&           aCallback,
                              DNSServiceRef               aServiceRef,
                              DNSRecordRef                aRecordRef,
                              Publisher *                 aPublisher)
            : HostRegistration(aName, aAddress, std::move(aCallback), aPublisher)
            , mServiceRef(aServiceRef)
            , mRecordRef(aRecordRef)
        {
        }

        ~DnssdHostRegistration(void) override;
        const DNSServiceRef &GetServiceRef() const { return mServiceRef; }
        const DNSRecordRef & GetRecordRef() const { return mRecordRef; }

    private:
        DNSServiceRef mServiceRef;
        DNSRecordRef  mRecordRef;
    };

    struct ServiceRef : private ::NonCopyable
    {
        DNSServiceRef mServiceRef;

        explicit ServiceRef(void)
            : mServiceRef(nullptr)
        {
        }

        ~ServiceRef() { Release(); }

        void Update(MainloopContext &aMainloop) const;
        void Process(const MainloopContext &aMainloop, std::vector<DNSServiceRef> &aReadyServices) const;
        void Release(void);
        void DeallocateServiceRef(void);
    };

    struct ServiceSubscription;

    struct ServiceInstanceResolution : public ServiceRef
    {
        explicit ServiceInstanceResolution(ServiceSubscription &aSubscription,
                                           std::string          aInstanceName,
                                           std::string          aType,
                                           std::string          aDomain,
                                           uint32_t             aNetifIndex)
            : ServiceRef()
            , mSubscription(&aSubscription)
            , mInstanceName(std::move(aInstanceName))
            , mTypeEndWithDot(std::move(aType))
            , mDomain(std::move(aDomain))
            , mNetifIndex(aNetifIndex)
        {
        }

        void      Resolve(void);
        otbrError GetAddrInfo(uint32_t aInterfaceIndex);
        void      FinishResolution(void);

        static void HandleResolveResult(DNSServiceRef        aServiceRef,
                                        DNSServiceFlags      aFlags,
                                        uint32_t             aInterfaceIndex,
                                        DNSServiceErrorType  aErrorCode,
                                        const char *         aFullName,
                                        const char *         aHostTarget,
                                        uint16_t             aPort, // In network byte order.
                                        uint16_t             aTxtLen,
                                        const unsigned char *aTxtRecord,
                                        void *               aContext);
        void        HandleResolveResult(DNSServiceRef        aServiceRef,
                                        DNSServiceFlags      aFlags,
                                        uint32_t             aInterfaceIndex,
                                        DNSServiceErrorType  aErrorCode,
                                        const char *         aFullName,
                                        const char *         aHostTarget,
                                        uint16_t             aPort, // In network byte order.
                                        uint16_t             aTxtLen,
                                        const unsigned char *aTxtRecord);
        static void HandleGetAddrInfoResult(DNSServiceRef          aServiceRef,
                                            DNSServiceFlags        aFlags,
                                            uint32_t               aInterfaceIndex,
                                            DNSServiceErrorType    aErrorCode,
                                            const char *           aHostName,
                                            const struct sockaddr *aAddress,
                                            uint32_t               aTtl,
                                            void *                 aContext);
        void        HandleGetAddrInfoResult(DNSServiceRef          aServiceRef,
                                            DNSServiceFlags        aFlags,
                                            uint32_t               aInterfaceIndex,
                                            DNSServiceErrorType    aErrorCode,
                                            const char *           aHostName,
                                            const struct sockaddr *aAddress,
                                            uint32_t               aTtl);

        ServiceSubscription *  mSubscription;
        std::string            mInstanceName;
        std::string            mTypeEndWithDot;
        std::string            mDomain;
        uint32_t               mNetifIndex;
        DiscoveredInstanceInfo mInstanceInfo;
    };

    struct ServiceSubscription : public ServiceRef
    {
        explicit ServiceSubscription(PublisherMDnsSd &aMDnsSd, std::string aType, std::string aInstanceName)
            : ServiceRef()
            , mMDnsSd(&aMDnsSd)
            , mType(std::move(aType))
            , mInstanceName(std::move(aInstanceName))
        {
        }

        void Browse(void);
        void Resolve(uint32_t           aNetifIndex,
                     const std::string &aInstanceName,
                     const std::string &aType,
                     const std::string &aDomain);
        void RemoveInstanceResolution(ServiceInstanceResolution &aInstanceResolution);
        void UpdateAll(MainloopContext &aMainloop) const;
        void ProcessAll(const MainloopContext &aMainloop, std::vector<DNSServiceRef> &aReadyServices) const;

        static void HandleBrowseResult(DNSServiceRef       aServiceRef,
                                       DNSServiceFlags     aFlags,
                                       uint32_t            aInterfaceIndex,
                                       DNSServiceErrorType aErrorCode,
                                       const char *        aInstanceName,
                                       const char *        aType,
                                       const char *        aDomain,
                                       void *              aContext);
        void        HandleBrowseResult(DNSServiceRef       aServiceRef,
                                       DNSServiceFlags     aFlags,
                                       uint32_t            aInterfaceIndex,
                                       DNSServiceErrorType aErrorCode,
                                       const char *        aInstanceName,
                                       const char *        aType,
                                       const char *        aDomain);

        PublisherMDnsSd *mMDnsSd;
        std::string      mType;
        std::string      mInstanceName;

        std::vector<std::unique_ptr<ServiceInstanceResolution>> mResolvingInstances;
    };

    struct HostSubscription : public ServiceRef
    {
        explicit HostSubscription(PublisherMDnsSd &aMDnsSd, std::string aHostName)
            : ServiceRef()
            , mMDnsSd(&aMDnsSd)
            , mHostName(std::move(aHostName))
        {
        }

        void        Resolve(void);
        static void HandleResolveResult(DNSServiceRef          aServiceRef,
                                        DNSServiceFlags        aFlags,
                                        uint32_t               aInterfaceIndex,
                                        DNSServiceErrorType    aErrorCode,
                                        const char *           aHostName,
                                        const struct sockaddr *aAddress,
                                        uint32_t               aTtl,
                                        void *                 aContext);
        void        HandleResolveResult(DNSServiceRef          aServiceRef,
                                        DNSServiceFlags        aFlags,
                                        uint32_t               aInterfaceIndex,
                                        DNSServiceErrorType    aErrorCode,
                                        const char *           aHostName,
                                        const struct sockaddr *aAddress,
                                        uint32_t               aTtl);

        PublisherMDnsSd *  mMDnsSd;
        std::string        mHostName;
        DiscoveredHostInfo mHostInfo;
    };

    using ServiceSubscriptionList = std::vector<std::unique_ptr<ServiceSubscription>>;
    using HostSubscriptionList    = std::vector<std::unique_ptr<HostSubscription>>;

    static void HandleServiceRegisterResult(DNSServiceRef         aService,
                                            const DNSServiceFlags aFlags,
                                            DNSServiceErrorType   aError,
                                            const char *          aName,
                                            const char *          aType,
                                            const char *          aDomain,
                                            void *                aContext);
    void        HandleServiceRegisterResult(DNSServiceRef         aService,
                                            const DNSServiceFlags aFlags,
                                            DNSServiceErrorType   aError,
                                            const char *          aName,
                                            const char *          aType,
                                            const char *          aDomain);
    static void HandleRegisterHostResult(DNSServiceRef       aHostsConnection,
                                         DNSRecordRef        aHostRecord,
                                         DNSServiceFlags     aFlags,
                                         DNSServiceErrorType aErrorCode,
                                         void *              aContext);
    void        HandleRegisterHostResult(DNSServiceRef       aHostsConnection,
                                         DNSRecordRef        aHostRecord,
                                         DNSServiceFlags     aFlags,
                                         DNSServiceErrorType aErrorCode);

    static std::string MakeRegType(const std::string &aType, SubTypeList aSubTypeList);

    ServiceRegistration *FindServiceRegistration(const DNSServiceRef &aServiceRef);
    HostRegistration *   FindHostRegistration(const DNSServiceRef &aServiceRef, const DNSRecordRef &aRecordRef);

    DNSServiceRef mHostsRef;
    State         mState;
    StateCallback mStateCallback;

    ServiceSubscriptionList mSubscribedServices;
    HostSubscriptionList    mSubscribedHosts;
};

/**
 * @}
 */

} // namespace Mdns

} // namespace otbr

#endif // OTBR_AGENT_MDNS_MDNSSD_HPP_
