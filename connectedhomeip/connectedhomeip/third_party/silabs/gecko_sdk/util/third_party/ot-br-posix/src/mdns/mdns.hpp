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
 *   This file includes definitions for mDNS publisher.
 */

#ifndef OTBR_AGENT_MDNS_HPP_
#define OTBR_AGENT_MDNS_HPP_

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <sys/select.h>

#include "common/callback.hpp"
#include "common/code_utils.hpp"
#include "common/time.hpp"
#include "common/types.hpp"

namespace otbr {

namespace Mdns {

/**
 * @addtogroup border-router-mdns
 *
 * @brief
 *   This module includes definition for mDNS publisher.
 *
 * @{
 */

/**
 * This interface defines the functionality of mDNS publisher.
 *
 */
class Publisher : private NonCopyable
{
public:
    /**
     * This structure represents a name/value pair of the TXT record.
     *
     */
    struct TxtEntry
    {
        std::string          mName;  ///< The name of the TXT entry.
        std::vector<uint8_t> mValue; ///< The value of the TXT entry.

        TxtEntry(const char *aName, const char *aValue)
            : TxtEntry(aName, reinterpret_cast<const uint8_t *>(aValue), strlen(aValue))
        {
        }

        TxtEntry(const char *aName, const uint8_t *aValue, size_t aValueLength)
            : TxtEntry(aName, strlen(aName), aValue, aValueLength)
        {
        }

        TxtEntry(const char *aName, size_t aNameLength, const uint8_t *aValue, size_t aValueLength)
            : mName(aName, aNameLength)
            , mValue(aValue, aValue + aValueLength)
        {
        }

        bool operator==(const TxtEntry &aOther) const { return mName == aOther.mName && mValue == aOther.mValue; }
    };

    typedef std::vector<TxtEntry>    TxtList;
    typedef std::vector<std::string> SubTypeList;

    /**
     * This structure represents information of a discovered service instance.
     *
     */
    struct DiscoveredInstanceInfo
    {
        bool                    mRemoved    = false; ///< The Service Instance is removed.
        uint32_t                mNetifIndex = 0;     ///< Network interface.
        std::string             mName;               ///< Instance name.
        std::string             mHostName;           ///< Full host name.
        std::vector<Ip6Address> mAddresses;          ///< IPv6 addresses.
        uint16_t                mPort     = 0;       ///< Port.
        uint16_t                mPriority = 0;       ///< Service priority.
        uint16_t                mWeight   = 0;       ///< Service weight.
        std::vector<uint8_t>    mTxtData;            ///< TXT RDATA bytes.
        uint32_t                mTtl = 0;            ///< Service TTL.
    };

    /**
     * This structure represents information of a discovered host.
     *
     */
    struct DiscoveredHostInfo
    {
        std::string             mHostName;  ///< Full host name.
        std::vector<Ip6Address> mAddresses; ///< IP6 addresses.
        uint32_t                mTtl = 0;   ///< Host TTL.
    };

    /**
     * This function is called to notify a discovered service instance.
     *
     */
    using DiscoveredServiceInstanceCallback =
        std::function<void(const std::string &aType, const DiscoveredInstanceInfo &aInstanceInfo)>;

    /**
     * This function is called to notify a discovered host.
     *
     */
    using DiscoveredHostCallback =
        std::function<void(const std::string &aHostName, const DiscoveredHostInfo &aHostInfo)>;

    /**
     * mDNS state values.
     *
     */
    enum class State
    {
        kIdle,  ///< Unable to publish service.
        kReady, ///< Ready to publish service.
    };

    /** The callback for receiving mDNS publisher state changes. */
    using StateCallback = std::function<void(State aNewState)>;

    /** The callback for receiving the result of a operation. */
    using ResultCallback = OnceCallback<void(otbrError aError)>;

    /**
     * This method starts the mDNS publisher.
     *
     * @retval OTBR_ERROR_NONE  Successfully started mDNS publisher;
     * @retval OTBR_ERROR_MDNS  Failed to start mDNS publisher.
     *
     */
    virtual otbrError Start(void) = 0;

    /**
     * This method stops the mDNS publisher.
     *
     */
    virtual void Stop(void) = 0;

    /**
     * This method checks if publisher has been started.
     *
     * @retval true   Already started.
     * @retval false  Not started.
     *
     */
    virtual bool IsStarted(void) const = 0;

    /**
     * This method publishes or updates a service.
     *
     * @param[in] aHostName     The name of the host which this service resides on. If an empty string is
     *                          provided, this service resides on local host and it is the implementation
     *                          to provide specific host name. Otherwise, the caller MUST publish the host
     *                          with method PublishHost.
     * @param[in] aName         The name of this service.
     * @param[in] aType         The type of this service.
     * @param[in] aSubTypeList  A list of service subtypes.
     * @param[in] aPort         The port number of this service.
     * @param[in] aTxtList      A list of TXT name/value pairs.
     * @param[in] aCallback     The callback for receiving the publishing result. `OTBR_ERROR_NONE` will be
     *                          returned if the operation is successful and all other values indicate a
     *                          failure. Specifically, `OTBR_ERROR_DUPLICATED` indicates that the name has
     *                          already been published and the caller can re-publish with a new name if an
     *                          alternative name is available/acceptable.
     *
     */
    void PublishService(const std::string &aHostName,
                        const std::string &aName,
                        const std::string &aType,
                        const SubTypeList &aSubTypeList,
                        uint16_t           aPort,
                        const TxtList &    aTxtList,
                        ResultCallback &&  aCallback);

    /**
     * This method un-publishes a service.
     *
     * @param[in] aName      The name of this service.
     * @param[in] aType      The type of this service.
     * @param[in] aCallback  The callback for receiving the publishing result.
     *
     */
    virtual void UnpublishService(const std::string &aName, const std::string &aType, ResultCallback &&aCallback) = 0;

    /**
     * This method publishes or updates a host.
     *
     * Publishing a host is advertising an AAAA RR for the host name. This method should be called
     * before a service with non-empty host name is published.
     *
     * @param[in] aName      The name of the host.
     * @param[in] aAddress   The address of the host.
     * @param[in] aCallback  The callback for receiving the publishing result.`OTBR_ERROR_NONE` will be
     *                       returned if the operation is successful and all other values indicate a
     *                       failure. Specifically, `OTBR_ERROR_DUPLICATED` indicates that the name has
     *                       already been published and the caller can re-publish with a new name if an
     *                       alternative name is available/acceptable.
     *
     */
    void PublishHost(const std::string &aName, const std::vector<uint8_t> &aAddress, ResultCallback &&aCallback);

    /**
     * This method un-publishes a host.
     *
     * @param[in] aName      A host name.
     * @param[in] aCallback  The callback for receiving the publishing result.
     *
     */
    virtual void UnpublishHost(const std::string &aName, ResultCallback &&aCallback) = 0;

    /**
     * This method subscribes a given service or service instance.
     *
     * If @p aInstanceName is not empty, this method subscribes the service instance. Otherwise, this method subscribes
     * the service. mDNS implementations should use the `DiscoveredServiceInstanceCallback` function to notify
     * discovered service instances.
     *
     * @note Discovery Proxy implementation guarantees no duplicate subscriptions for the same service or service
     * instance.
     *
     * @param[in] aType          The service type.
     * @param[in] aInstanceName  The service instance to subscribe, or empty to subscribe the service.
     *
     */
    virtual void SubscribeService(const std::string &aType, const std::string &aInstanceName) = 0;

    /**
     * This method unsubscribes a given service or service instance.
     *
     * If @p aInstanceName is not empty, this method unsubscribes the service instance. Otherwise, this method
     * unsubscribes the service.
     *
     * @note Discovery Proxy implementation guarantees no redundant unsubscription for a service or service instance.
     *
     * @param[in] aType          The service type.
     * @param[in] aInstanceName  The service instance to unsubscribe, or empty to unsubscribe the service.
     *
     */
    virtual void UnsubscribeService(const std::string &aType, const std::string &aInstanceName) = 0;

    /**
     * This method subscribes a given host.
     *
     * mDNS implementations should use the `DiscoveredHostCallback` function to notify discovered hosts.
     *
     * @note Discovery Proxy implementation guarantees no duplicate subscriptions for the same host.
     *
     * @param[in] aHostName  The host name (without domain).
     *
     */
    virtual void SubscribeHost(const std::string &aHostName) = 0;

    /**
     * This method unsubscribes a given host.
     *
     * @note Discovery Proxy implementation guarantees no redundant unsubscription for a host.
     *
     * @param[in] aHostName  The host name (without domain).
     *
     */
    virtual void UnsubscribeHost(const std::string &aHostName) = 0;

    /**
     * This method sets the callbacks for subscriptions.
     *
     * @param[in] aInstanceCallback  The callback function to receive discovered service instances.
     * @param[in] aHostCallback      The callback function to receive discovered hosts.
     *
     * @returns  The Subscriber ID for the callbacks.
     *
     */
    uint64_t AddSubscriptionCallbacks(DiscoveredServiceInstanceCallback aInstanceCallback,
                                      DiscoveredHostCallback            aHostCallback);

    /**
     * This method cancels callbacks for subscriptions.
     *
     * @param[in] aSubscriberId  The Subscriber ID previously returned by `AddSubscriptionCallbacks`.
     *
     */
    void RemoveSubscriptionCallbacks(uint64_t aSubscriberId);

    /**
     * This method returns the mDNS statistics information of the publisher.
     *
     * @returns  The MdnsTelemetryInfo of the publisher.
     *
     */
    const MdnsTelemetryInfo &GetMdnsTelemetryInfo() const { return mTelemetryInfo; }

    virtual ~Publisher(void) = default;

    /**
     * This function creates a mDNS publisher.
     *
     * @param[in] aCallback  The callback for receiving mDNS publisher state changes.
     *
     * @returns A pointer to the newly created mDNS publisher.
     *
     */
    static Publisher *Create(StateCallback aCallback);

    /**
     * This function destroys the mDNS publisher.
     *
     * @param[in] aPublisher  A pointer to the publisher.
     *
     */
    static void Destroy(Publisher *aPublisher);

    /**
     * This function writes the TXT entry list to a TXT data buffer. The TXT entries
     * will be sorted by their keys.
     *
     * The output data is in standard DNS-SD TXT data format.
     * See RFC 6763 for details: https://tools.ietf.org/html/rfc6763#section-6.
     *
     * @param[in]  aTxtList  A TXT entry list.
     * @param[out] aTxtData  A TXT data buffer.
     *
     * @retval OTBR_ERROR_NONE          Successfully write the TXT entry list.
     * @retval OTBR_ERROR_INVALID_ARGS  The @p aTxtList includes invalid TXT entry.
     *
     * @sa DecodeTxtData
     *
     */
    static otbrError EncodeTxtData(const TxtList &aTxtList, std::vector<uint8_t> &aTxtData);

    /**
     * This function decodes a TXT entry list from a TXT data buffer.
     *
     * The input data should be in standard DNS-SD TXT data format.
     * See RFC 6763 for details: https://tools.ietf.org/html/rfc6763#section-6.
     *
     * @param[out]  aTxtList    A TXT entry list.
     * @param[in]   aTxtData    A pointer to TXT data.
     * @param[in]   aTxtLength  The TXT data length.
     *
     * @retval OTBR_ERROR_NONE          Successfully decoded the TXT data.
     * @retval OTBR_ERROR_INVALID_ARGS  The @p aTxtdata has invalid TXT format.
     *
     * @sa EncodeTxtData
     *
     */
    static otbrError DecodeTxtData(TxtList &aTxtList, const uint8_t *aTxtData, uint16_t aTxtLength);

protected:
    static constexpr uint8_t kMaxTextEntrySize = 255;

    class Registration
    {
    public:
        ResultCallback mCallback;
        Publisher *    mPublisher;

        Registration(ResultCallback &&aCallback, Publisher *aPublisher)
            : mCallback(std::move(aCallback))
            , mPublisher(aPublisher)
        {
        }
        virtual ~Registration(void);

        // Tells whether the service registration has been completed (typically by calling
        // `ServiceRegistration::Complete`).
        bool IsCompleted() const { return mCallback.IsNull(); }

    protected:
        // Completes the service registration with given result/error.
        void TriggerCompleteCallback(otbrError aError)
        {
            if (!IsCompleted())
            {
                std::move(mCallback)(aError);
            }
        }
    };

    class ServiceRegistration : public Registration
    {
    public:
        std::string mHostName;
        std::string mName;
        std::string mType;
        SubTypeList mSubTypeList;
        uint16_t    mPort;
        TxtList     mTxtList;

        ServiceRegistration(std::string      aHostName,
                            std::string      aName,
                            std::string      aType,
                            SubTypeList      aSubTypeList,
                            uint16_t         aPort,
                            TxtList          aTxtList,
                            ResultCallback &&aCallback,
                            Publisher *      aPublisher)
            : Registration(std::move(aCallback), aPublisher)
            , mHostName(std::move(aHostName))
            , mName(std::move(aName))
            , mType(std::move(aType))
            , mSubTypeList(SortSubTypeList(std::move(aSubTypeList)))
            , mPort(aPort)
            , mTxtList(SortTxtList(std::move(aTxtList)))
        {
        }
        ~ServiceRegistration(void) override { OnComplete(OTBR_ERROR_ABORTED); }

        void Complete(otbrError aError);

        void OnComplete(otbrError aError);

        // Tells whether this `ServiceRegistration` object is outdated comparing to the given parameters.
        bool IsOutdated(const std::string &aHostName,
                        const std::string &aName,
                        const std::string &aType,
                        const SubTypeList &aSubTypeList,
                        uint16_t           aPort,
                        const TxtList &    aTxtList) const;
    };

    class HostRegistration : public Registration
    {
    public:
        std::string          mName;
        std::vector<uint8_t> mAddress;

        HostRegistration(std::string          aName,
                         std::vector<uint8_t> aAddress,
                         ResultCallback &&    aCallback,
                         Publisher *          aPublisher)
            : Registration(std::move(aCallback), aPublisher)
            , mName(std::move(aName))
            , mAddress(std::move(aAddress))
        {
        }

        ~HostRegistration(void) { OnComplete(OTBR_ERROR_ABORTED); }

        void Complete(otbrError aError);

        void OnComplete(otbrError);

        // Tells whether this `HostRegistration` object is outdated comparing to the given parameters.
        bool IsOutdated(const std::string &aName, const std::vector<uint8_t> &aAddress) const;
    };

    using ServiceRegistrationPtr = std::unique_ptr<ServiceRegistration>;
    using ServiceRegistrationMap = std::map<std::string, ServiceRegistrationPtr>;
    using HostRegistrationPtr    = std::unique_ptr<HostRegistration>;
    using HostRegistrationMap    = std::map<std::string, HostRegistrationPtr>;

    static SubTypeList SortSubTypeList(SubTypeList aSubTypeList);
    static TxtList     SortTxtList(TxtList aTxtList);
    static std::string MakeFullServiceName(const std::string &aName, const std::string &aType);
    static std::string MakeFullHostName(const std::string &aName);

    virtual void PublishServiceImpl(const std::string &aHostName,
                                    const std::string &aName,
                                    const std::string &aType,
                                    const SubTypeList &aSubTypeList,
                                    uint16_t           aPort,
                                    const TxtList &    aTxtList,
                                    ResultCallback &&  aCallback)                            = 0;
    virtual void PublishHostImpl(const std::string &         aName,
                                 const std::vector<uint8_t> &aAddress,
                                 ResultCallback &&           aCallback)                               = 0;
    virtual void OnServiceResolveFailedImpl(const std::string &aType,
                                            const std::string &aInstanceName,
                                            int32_t            aErrorCode)                            = 0;
    virtual void OnHostResolveFailedImpl(const std::string &aHostName, int32_t aErrorCode) = 0;

    virtual otbrError DnsErrorToOtbrError(int32_t aError) = 0;

    void AddServiceRegistration(ServiceRegistrationPtr &&aServiceReg);
    void RemoveServiceRegistration(const std::string &aName, const std::string &aType, otbrError aError);
    ServiceRegistration *FindServiceRegistration(const std::string &aName, const std::string &aType);
    void                 OnServiceResolved(const std::string &aType, const DiscoveredInstanceInfo &aInstanceInfo);
    void OnServiceResolveFailed(const std::string &aType, const std::string &aInstanceName, int32_t aErrorCode);
    void OnServiceRemoved(uint32_t aNetifIndex, const std::string &aType, const std::string &aInstanceName);
    void OnHostResolved(const std::string &aHostName, const DiscoveredHostInfo &aHostInfo);
    void OnHostResolveFailed(const std::string &aHostName, int32_t aErrorCode);

    // Handles the cases that there is already a registration for the same service.
    // If the returned callback is completed, current registration should be considered
    // success and no further action should be performed.
    ResultCallback HandleDuplicateServiceRegistration(const std::string &aHostName,
                                                      const std::string &aName,
                                                      const std::string &aType,
                                                      const SubTypeList &aSubTypeList,
                                                      uint16_t           aPort,
                                                      const TxtList &    aTxtList,
                                                      ResultCallback &&  aCallback);

    ResultCallback HandleDuplicateHostRegistration(const std::string &         aName,
                                                   const std::vector<uint8_t> &aAddress,
                                                   ResultCallback &&           aCallback);

    void              AddHostRegistration(HostRegistrationPtr &&aHostReg);
    void              RemoveHostRegistration(const std::string &aName, otbrError aError);
    HostRegistration *FindHostRegistration(const std::string &aName);

    static void UpdateMdnsResponseCounters(otbr::MdnsResponseCounters &aCounters, otbrError aError);
    static void UpdateEmaLatency(uint32_t &aEmaLatency, uint32_t aLatency, otbrError aError);

    void UpdateServiceRegistrationEmaLatency(const std::string &aInstanceName,
                                             const std::string &aType,
                                             otbrError          aError);
    void UpdateHostRegistrationEmaLatency(const std::string &aHostName, otbrError aError);
    void UpdateServiceInstanceResolutionEmaLatency(const std::string &aInstanceName,
                                                   const std::string &aType,
                                                   otbrError          aError);
    void UpdateHostResolutionEmaLatency(const std::string &aHostName, otbrError aError);

    ServiceRegistrationMap mServiceRegistrations;
    HostRegistrationMap    mHostRegistrations;

    uint64_t mNextSubscriberId = 1;

    std::map<uint64_t, std::pair<DiscoveredServiceInstanceCallback, DiscoveredHostCallback>> mDiscoveredCallbacks;
    // {instance name, service type} -> the timepoint to begin service registration
    std::map<std::pair<std::string, std::string>, Timepoint> mServiceRegistrationBeginTime;
    // host name -> the timepoint to begin host registration
    std::map<std::string, Timepoint> mHostRegistrationBeginTime;
    // {instance name, service type} -> the timepoint to begin service resolution
    std::map<std::pair<std::string, std::string>, Timepoint> mServiceInstanceResolutionBeginTime;
    // host name -> the timepoint to begin host resolution
    std::map<std::string, Timepoint> mHostResolutionBeginTime;

    otbr::MdnsTelemetryInfo mTelemetryInfo{};
};

/**
 * @}
 */

} // namespace Mdns

} // namespace otbr

#endif // OTBR_AGENT_MDNS_HPP_
