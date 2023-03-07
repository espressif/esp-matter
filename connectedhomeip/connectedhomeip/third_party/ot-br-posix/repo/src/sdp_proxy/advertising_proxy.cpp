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
 *   The file implements the Advertising Proxy.
 */

#define OTBR_LOG_TAG "ADPROXY"

#include "sdp_proxy/advertising_proxy.hpp"

#if OTBR_ENABLE_SRP_ADVERTISING_PROXY

#if !OTBR_ENABLE_MDNS_AVAHI && !OTBR_ENABLE_MDNS_MDNSSD && !OTBR_ENABLE_MDNS_MOJO
#error "The Advertising Proxy requires OTBR_ENABLE_MDNS_AVAHI, OTBR_ENABLE_MDNS_MDNSSD or OTBR_ENABLE_MDNS_MOJO"
#endif

#include <string>

#include <assert.h>

#include "common/code_utils.hpp"
#include "common/dns_utils.hpp"
#include "common/logging.hpp"

namespace otbr {

static otError OtbrErrorToOtError(otbrError aError)
{
    otError error;

    switch (aError)
    {
    case OTBR_ERROR_NONE:
        error = OT_ERROR_NONE;
        break;

    case OTBR_ERROR_NOT_FOUND:
        error = OT_ERROR_NOT_FOUND;
        break;

    case OTBR_ERROR_PARSE:
        error = OT_ERROR_PARSE;
        break;

    case OTBR_ERROR_NOT_IMPLEMENTED:
        error = OT_ERROR_NOT_IMPLEMENTED;
        break;

    case OTBR_ERROR_INVALID_ARGS:
        error = OT_ERROR_INVALID_ARGS;
        break;

    case OTBR_ERROR_DUPLICATED:
        error = OT_ERROR_DUPLICATED;
        break;

    case OTBR_ERROR_INVALID_STATE:
        error = OT_ERROR_INVALID_STATE;
        break;

    default:
        error = OT_ERROR_FAILED;
        break;
    }

    return error;
}

AdvertisingProxy::AdvertisingProxy(Ncp::ControllerOpenThread &aNcp, Mdns::Publisher &aPublisher)
    : mNcp(aNcp)
    , mPublisher(aPublisher)
{
    mNcp.RegisterResetHandler(
        [this]() { otSrpServerSetServiceUpdateHandler(GetInstance(), AdvertisingHandler, this); });
}

otbrError AdvertisingProxy::Start(void)
{
    otSrpServerSetServiceUpdateHandler(GetInstance(), AdvertisingHandler, this);

    otbrLogInfo("Started");

    return OTBR_ERROR_NONE;
}

void AdvertisingProxy::Stop()
{
    // Outstanding updates will fail on the SRP server because of timeout.
    // TODO: handle this case gracefully.

    // Stop receiving SRP server events.
    if (GetInstance() != nullptr)
    {
        otSrpServerSetServiceUpdateHandler(GetInstance(), nullptr, nullptr);
    }

    otbrLogInfo("Stopped");
}

void AdvertisingProxy::AdvertisingHandler(otSrpServerServiceUpdateId aId,
                                          const otSrpServerHost *    aHost,
                                          uint32_t                   aTimeout,
                                          void *                     aContext)
{
    static_cast<AdvertisingProxy *>(aContext)->AdvertisingHandler(aId, aHost, aTimeout);
}

void AdvertisingProxy::AdvertisingHandler(otSrpServerServiceUpdateId aId,
                                          const otSrpServerHost *    aHost,
                                          uint32_t                   aTimeout)
{
    OTBR_UNUSED_VARIABLE(aTimeout);

    OutstandingUpdate *update = nullptr;
    otbrError          error  = OTBR_ERROR_NONE;

    mOutstandingUpdates.emplace_back();
    update      = &mOutstandingUpdates.back();
    update->mId = aId;

    error = PublishHostAndItsServices(aHost, update);

    if (error != OTBR_ERROR_NONE || update->mCallbackCount == 0)
    {
        mOutstandingUpdates.pop_back();
        otSrpServerHandleServiceUpdateResult(GetInstance(), aId, OtbrErrorToOtError(error));
    }
}

void AdvertisingProxy::OnMdnsPublishResult(otSrpServerServiceUpdateId aUpdateId, otbrError aError)
{
    for (auto update = mOutstandingUpdates.begin(); update != mOutstandingUpdates.end(); ++update)
    {
        if (update->mId != aUpdateId)
        {
            continue;
        }

        if (aError != OTBR_ERROR_NONE || update->mCallbackCount == 1)
        {
            // Erase before notifying OpenThread, because there are chances that new
            // elements may be added to `otSrpServerHandleServiceUpdateResult` and
            // the iterator will be invalidated.
            mOutstandingUpdates.erase(update);
            otSrpServerHandleServiceUpdateResult(GetInstance(), aUpdateId, OtbrErrorToOtError(aError));
        }
        else
        {
            --update->mCallbackCount;
            otbrLogInfo("Waiting for more publishing callbacks %d", update->mCallbackCount);
        }
        break;
    }
}

void AdvertisingProxy::PublishAllHostsAndServices(void)
{
    const otSrpServerHost *host = nullptr;

    VerifyOrExit(mPublisher.IsStarted(), mPublisher.Start());

    otbrLogInfo("Publish all hosts and services");
    while ((host = otSrpServerGetNextHost(GetInstance(), host)))
    {
        PublishHostAndItsServices(host, nullptr);
    }

exit:
    return;
}

otbrError AdvertisingProxy::PublishHostAndItsServices(const otSrpServerHost *aHost, OutstandingUpdate *aUpdate)
{
    otbrError                  error = OTBR_ERROR_NONE;
    std::string                hostName;
    std::string                hostDomain;
    const otIp6Address *       hostAddress;
    uint8_t                    hostAddressNum;
    bool                       hostDeleted;
    const otSrpServerService * service;
    otSrpServerServiceUpdateId updateId     = 0;
    bool                       hasUpdate    = false;
    std::string                fullHostName = otSrpServerHostGetFullName(aHost);

    otbrLogInfo("Advertise SRP service updates: host=%s", fullHostName.c_str());

    SuccessOrExit(error = SplitFullHostName(fullHostName, hostName, hostDomain));
    hostAddress = otSrpServerHostGetAddresses(aHost, &hostAddressNum);
    hostDeleted = otSrpServerHostIsDeleted(aHost);

    if (aUpdate)
    {
        hasUpdate = true;
        updateId  = aUpdate->mId;
        aUpdate->mCallbackCount++;
        aUpdate->mHostName = hostName;
        service            = nullptr;
        while ((service = otSrpServerHostFindNextService(aHost, service, OT_SRP_SERVER_FLAGS_BASE_TYPE_SERVICE_ONLY,
                                                         /* aServiceName */ nullptr, /* aInstanceName */ nullptr)))
        {
            aUpdate->mCallbackCount++;
        }
    }

    service = nullptr;
    while ((service = otSrpServerHostFindNextService(aHost, service, OT_SRP_SERVER_FLAGS_BASE_TYPE_SERVICE_ONLY,
                                                     /* aServiceName */ nullptr, /* aInstanceName */ nullptr)))
    {
        std::string fullServiceName = otSrpServerServiceGetFullName(service);
        std::string serviceName;
        std::string serviceType;
        std::string serviceDomain;

        SuccessOrExit(error = SplitFullServiceInstanceName(fullServiceName, serviceName, serviceType, serviceDomain));

        if (!hostDeleted && !otSrpServerServiceIsDeleted(service))
        {
            Mdns::Publisher::TxtList     txtList     = MakeTxtList(service);
            Mdns::Publisher::SubTypeList subTypeList = MakeSubTypeList(service);

            otbrLogDebug("Publish SRP service '%s'", fullServiceName.c_str());
            mPublisher.PublishService(
                hostName, serviceName, serviceType, subTypeList, otSrpServerServiceGetPort(service), txtList,
                [this, hasUpdate, updateId, fullServiceName](otbrError aError) {
                    otbrLogResult(aError, "Handle publish SRP service '%s'", fullServiceName.c_str());
                    if (hasUpdate)
                    {
                        OnMdnsPublishResult(updateId, aError);
                    }
                });
        }
        else
        {
            otbrLogDebug("Unpublish SRP service '%s'", fullServiceName.c_str());
            mPublisher.UnpublishService(
                serviceName, serviceType, [this, hasUpdate, updateId, fullServiceName](otbrError aError) {
                    // Treat `NOT_FOUND` as success when unpublishing service
                    aError = (aError == OTBR_ERROR_NOT_FOUND) ? OTBR_ERROR_NONE : aError;
                    otbrLogResult(aError, "Handle unpublish SRP service '%s'", fullServiceName.c_str());
                    if (hasUpdate)
                    {
                        OnMdnsPublishResult(updateId, aError);
                    }
                });
        }
    }

    if (!hostDeleted)
    {
        std::vector<uint8_t> firstHostAddress{std::begin(hostAddress[0].mFields.m8),
                                              std::end(hostAddress[0].mFields.m8)};

        // TODO: select a preferred address or advertise all addresses from SRP client.
        otbrLogDebug("Publish SRP host '%s'", fullHostName.c_str());
        mPublisher.PublishHost(
            hostName, firstHostAddress,
            Mdns::Publisher::ResultCallback([this, hasUpdate, updateId, fullHostName](otbrError aError) {
                otbrLogResult(aError, "Handle publish SRP host '%s'", fullHostName.c_str());
                if (hasUpdate)
                {
                    OnMdnsPublishResult(updateId, aError);
                }
            }));
    }
    else
    {
        otbrLogDebug("Unpublish SRP host '%s'", fullHostName.c_str());
        mPublisher.UnpublishHost(hostName, [this, hasUpdate, updateId, fullHostName](otbrError aError) {
            // Treat `NOT_FOUND` as success when unpublishing host.
            aError = (aError == OTBR_ERROR_NOT_FOUND) ? OTBR_ERROR_NONE : aError;
            otbrLogResult(aError, "Handle unpublish SRP host '%s'", fullHostName.c_str());
            if (hasUpdate)
            {
                OnMdnsPublishResult(updateId, aError);
            }
        });
    }

exit:
    if (error != OTBR_ERROR_NONE)
    {
        if (hasUpdate)
        {
            otbrLogInfo("Failed to advertise SRP service updates (id = %u)", updateId);
        }
    }
    return error;
}

Mdns::Publisher::TxtList AdvertisingProxy::MakeTxtList(const otSrpServerService *aSrpService)
{
    const uint8_t *          txtData;
    uint16_t                 txtDataLength = 0;
    otDnsTxtEntryIterator    iterator;
    otDnsTxtEntry            txtEntry;
    Mdns::Publisher::TxtList txtList;

    txtData = otSrpServerServiceGetTxtData(aSrpService, &txtDataLength);

    otDnsInitTxtEntryIterator(&iterator, txtData, txtDataLength);

    while (otDnsGetNextTxtEntry(&iterator, &txtEntry) == OT_ERROR_NONE)
    {
        txtList.emplace_back(txtEntry.mKey, txtEntry.mValue, txtEntry.mValueLength);
    }

    return txtList;
}

Mdns::Publisher::SubTypeList AdvertisingProxy::MakeSubTypeList(const otSrpServerService *aSrpService)
{
    const otSrpServerHost *      host         = otSrpServerServiceGetHost(aSrpService);
    const char *                 instanceName = otSrpServerServiceGetInstanceName(aSrpService);
    const otSrpServerService *   subService   = nullptr;
    Mdns::Publisher::SubTypeList subTypeList;

    while ((subService = otSrpServerHostFindNextService(
                host, subService, (OT_SRP_SERVER_SERVICE_FLAG_SUB_TYPE | OT_SRP_SERVER_SERVICE_FLAG_ACTIVE),
                /* aServiceName */ nullptr, instanceName)) != nullptr)
    {
        char subLabel[OT_DNS_MAX_LABEL_SIZE];

        if (otSrpServerServiceGetServiceSubTypeLabel(subService, subLabel, sizeof(subLabel)) == OT_ERROR_NONE)
        {
            subTypeList.emplace_back(subLabel);
        }
        else
        {
            otbrLogWarning("Failed to retrieve subtype of SRP service: %s", otSrpServerServiceGetFullName(aSrpService));
        }
    }

    return subTypeList;
}

} // namespace otbr

#endif // OTBR_ENABLE_SRP_ADVERTISING_PROXY
