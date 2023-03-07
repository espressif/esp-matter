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

#define OTBR_LOG_TAG "UTILS"

#include "utils/thread_helper.hpp"

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include <string>

#include <openthread/border_router.h>
#include <openthread/channel_manager.h>
#include <openthread/jam_detection.h>
#include <openthread/joiner.h>
#include <openthread/thread_ftd.h>
#include <openthread/platform/radio.h>

#include "common/byteswap.hpp"
#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "common/tlv.hpp"
#include "ncp/ncp_openthread.hpp"

namespace otbr {
namespace agent {
namespace {
const Tlv *FindTlv(uint8_t aTlvType, const uint8_t *aTlvs, int aTlvsSize)
{
    const Tlv *result = nullptr;

    for (const Tlv *tlv = reinterpret_cast<const Tlv *>(aTlvs);
         reinterpret_cast<const uint8_t *>(tlv) + sizeof(Tlv) < aTlvs + aTlvsSize; tlv = tlv->GetNext())
    {
        if (tlv->GetType() == aTlvType)
        {
            ExitNow(result = tlv);
        }
    }

exit:
    return result;
}
} // namespace

ThreadHelper::ThreadHelper(otInstance *aInstance, otbr::Ncp::ControllerOpenThread *aNcp)
    : mInstance(aInstance)
    , mNcp(aNcp)
{
}

void ThreadHelper::StateChangedCallback(otChangedFlags aFlags)
{
    if (aFlags & OT_CHANGED_THREAD_ROLE)
    {
        otDeviceRole role = otThreadGetDeviceRole(mInstance);

        for (const auto &handler : mDeviceRoleHandlers)
        {
            handler(role);
        }

        if (role != OT_DEVICE_ROLE_DISABLED && role != OT_DEVICE_ROLE_DETACHED)
        {
            if (mAttachHandler != nullptr)
            {
                if (mAttachPendingDatasetTlvs.mLength == 0)
                {
                    AttachHandler handler = mAttachHandler;

                    mAttachHandler = nullptr;
                    handler(OT_ERROR_NONE, mAttachDelayMs);
                }
                else
                {
                    otOperationalDataset emptyDataset = {};
                    otError              error =
                        otDatasetSendMgmtPendingSet(mInstance, &emptyDataset, mAttachPendingDatasetTlvs.mTlvs,
                                                    mAttachPendingDatasetTlvs.mLength, MgmtSetResponseHandler, this);
                    if (error != OT_ERROR_NONE)
                    {
                        AttachHandler handler = mAttachHandler;

                        mAttachHandler            = nullptr;
                        mAttachPendingDatasetTlvs = {};
                        handler(error, 0);
                    }
                }
            }
            else if (mJoinerHandler != nullptr)
            {
                mJoinerHandler(OT_ERROR_NONE);
                mJoinerHandler = nullptr;
            }
        }
    }

    if (aFlags & OT_CHANGED_ACTIVE_DATASET)
    {
        ActiveDatasetChangedCallback();
    }
}

void ThreadHelper::ActiveDatasetChangedCallback()
{
    otError                  error;
    otOperationalDatasetTlvs datasetTlvs;

    SuccessOrExit(error = otDatasetGetActiveTlvs(mInstance, &datasetTlvs));

    for (const auto &handler : mActiveDatasetChangeHandlers)
    {
        handler(datasetTlvs);
    }

exit:
    if (error != OT_ERROR_NONE)
    {
        otbrLogWarning("Error handling active dataset change: %s", otThreadErrorToString(error));
    }
}

#if OTBR_ENABLE_DBUS_SERVER
void ThreadHelper::OnUpdateMeshCopTxt(std::map<std::string, std::vector<uint8_t>> aUpdate)
{
    if (mUpdateMeshCopTxtHandler)
    {
        mUpdateMeshCopTxtHandler(std::move(aUpdate));
    }
    else
    {
        otbrLogErr("No UpdateMeshCopTxtHandler");
    }
}
#endif

void ThreadHelper::AddDeviceRoleHandler(DeviceRoleHandler aHandler)
{
    mDeviceRoleHandlers.emplace_back(aHandler);
}

void ThreadHelper::Scan(ScanHandler aHandler)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(aHandler != nullptr);
    mScanHandler = aHandler;
    mScanResults.clear();

    error =
        otLinkActiveScan(mInstance, /*scanChannels =*/0, /*scanDuration=*/0, &ThreadHelper::ActiveScanHandler, this);

exit:
    if (error != OT_ERROR_NONE)
    {
        if (aHandler)
        {
            mScanHandler(error, {});
        }
        mScanHandler = nullptr;
    }
}

void ThreadHelper::EnergyScan(uint32_t aScanDuration, EnergyScanHandler aHandler)
{
    otError  error             = OT_ERROR_NONE;
    uint32_t preferredChannels = otPlatRadioGetPreferredChannelMask(mInstance);

    VerifyOrExit(aHandler != nullptr, error = OT_ERROR_BUSY);
    VerifyOrExit(aScanDuration < UINT16_MAX, error = OT_ERROR_INVALID_ARGS);
    mEnergyScanHandler = aHandler;
    mEnergyScanResults.clear();

    error = otLinkEnergyScan(mInstance, preferredChannels, static_cast<uint16_t>(aScanDuration),
                             &ThreadHelper::EnergyScanCallback, this);

exit:
    if (error != OT_ERROR_NONE)
    {
        if (aHandler)
        {
            mEnergyScanHandler(error, {});
        }
        mEnergyScanHandler = nullptr;
    }
}

void ThreadHelper::RandomFill(void *aBuf, size_t size)
{
    std::uniform_int_distribution<> dist(0, UINT8_MAX);
    uint8_t *                       buf = static_cast<uint8_t *>(aBuf);

    for (size_t i = 0; i < size; i++)
    {
        buf[i] = static_cast<uint8_t>(dist(mRandomDevice));
    }
}

void ThreadHelper::ActiveScanHandler(otActiveScanResult *aResult, void *aThreadHelper)
{
    ThreadHelper *helper = static_cast<ThreadHelper *>(aThreadHelper);

    helper->ActiveScanHandler(aResult);
}

void ThreadHelper::ActiveScanHandler(otActiveScanResult *aResult)
{
    if (aResult == nullptr)
    {
        if (mScanHandler != nullptr)
        {
            mScanHandler(OT_ERROR_NONE, mScanResults);
        }
    }
    else
    {
        mScanResults.push_back(*aResult);
    }
}

void ThreadHelper::EnergyScanCallback(otEnergyScanResult *aResult, void *aThreadHelper)
{
    ThreadHelper *helper = static_cast<ThreadHelper *>(aThreadHelper);

    helper->EnergyScanCallback(aResult);
}

void ThreadHelper::EnergyScanCallback(otEnergyScanResult *aResult)
{
    if (aResult == nullptr)
    {
        if (mEnergyScanHandler != nullptr)
        {
            mEnergyScanHandler(OT_ERROR_NONE, mEnergyScanResults);
        }
    }
    else
    {
        mEnergyScanResults.push_back(*aResult);
    }
}

uint8_t ThreadHelper::RandomChannelFromChannelMask(uint32_t aChannelMask)
{
    // 8 bit per byte
    constexpr uint8_t kNumChannels = sizeof(aChannelMask) * 8;
    uint8_t           channels[kNumChannels];
    uint8_t           numValidChannels = 0;

    for (uint8_t i = 0; i < kNumChannels; i++)
    {
        if (aChannelMask & (1 << i))
        {
            channels[numValidChannels++] = i;
        }
    }

    return channels[std::uniform_int_distribution<unsigned int>(0, numValidChannels - 1)(mRandomDevice)];
}

static otExtendedPanId ToOtExtendedPanId(uint64_t aExtPanId)
{
    otExtendedPanId extPanId;
    uint64_t        mask = UINT8_MAX;

    for (size_t i = 0; i < sizeof(uint64_t); i++)
    {
        extPanId.m8[i] = static_cast<uint8_t>((aExtPanId >> ((sizeof(uint64_t) - i - 1) * 8)) & mask);
    }

    return extPanId;
}

void ThreadHelper::Attach(const std::string &         aNetworkName,
                          uint16_t                    aPanId,
                          uint64_t                    aExtPanId,
                          const std::vector<uint8_t> &aNetworkKey,
                          const std::vector<uint8_t> &aPSKc,
                          uint32_t                    aChannelMask,
                          AttachHandler               aHandler)

{
    otError         error = OT_ERROR_NONE;
    otExtendedPanId extPanId;
    otNetworkKey    networkKey;
    otPskc          pskc;
    uint32_t        channelMask;
    uint8_t         channel;

    VerifyOrExit(aHandler != nullptr, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(mAttachHandler == nullptr && mJoinerHandler == nullptr, error = OT_ERROR_INVALID_STATE);
    VerifyOrExit(aNetworkKey.empty() || aNetworkKey.size() == sizeof(networkKey.m8), error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(aPSKc.empty() || aPSKc.size() == sizeof(pskc.m8), error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(aChannelMask != 0, error = OT_ERROR_INVALID_ARGS);

    while (aPanId == UINT16_MAX)
    {
        RandomFill(&aPanId, sizeof(aPanId));
    }

    if (aExtPanId != UINT64_MAX)
    {
        extPanId = ToOtExtendedPanId(aExtPanId);
    }
    else
    {
        *reinterpret_cast<uint64_t *>(&extPanId) = UINT64_MAX;

        while (*reinterpret_cast<uint64_t *>(&extPanId) == UINT64_MAX)
        {
            RandomFill(extPanId.m8, sizeof(extPanId.m8));
        }
    }

    if (!aNetworkKey.empty())
    {
        memcpy(networkKey.m8, &aNetworkKey[0], sizeof(networkKey.m8));
    }
    else
    {
        RandomFill(networkKey.m8, sizeof(networkKey.m8));
    }

    if (!aPSKc.empty())
    {
        memcpy(pskc.m8, &aPSKc[0], sizeof(pskc.m8));
    }
    else
    {
        RandomFill(pskc.m8, sizeof(pskc.m8));
    }

    if (!otIp6IsEnabled(mInstance))
    {
        SuccessOrExit(error = otIp6SetEnabled(mInstance, true));
    }

    SuccessOrExit(error = otThreadSetNetworkName(mInstance, aNetworkName.c_str()));
    SuccessOrExit(error = otLinkSetPanId(mInstance, aPanId));
    SuccessOrExit(error = otThreadSetExtendedPanId(mInstance, &extPanId));
    SuccessOrExit(error = otThreadSetNetworkKey(mInstance, &networkKey));

    channelMask = otPlatRadioGetPreferredChannelMask(mInstance) & aChannelMask;

    if (channelMask == 0)
    {
        channelMask = otLinkGetSupportedChannelMask(mInstance) & aChannelMask;
    }
    VerifyOrExit(channelMask != 0, otbrLogWarning("Invalid channel mask"), error = OT_ERROR_INVALID_ARGS);

    channel = RandomChannelFromChannelMask(channelMask);
    SuccessOrExit(otLinkSetChannel(mInstance, channel));

    SuccessOrExit(error = otThreadSetPskc(mInstance, &pskc));

    SuccessOrExit(error = otThreadSetEnabled(mInstance, true));
    mAttachDelayMs = 0;
    mAttachHandler = aHandler;

exit:
    if (error != OT_ERROR_NONE)
    {
        if (aHandler)
        {
            aHandler(error, 0);
        }
    }
}

void ThreadHelper::Attach(AttachHandler aHandler)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mAttachHandler == nullptr && mJoinerHandler == nullptr, error = OT_ERROR_INVALID_STATE);

    if (!otIp6IsEnabled(mInstance))
    {
        SuccessOrExit(error = otIp6SetEnabled(mInstance, true));
    }
    SuccessOrExit(error = otThreadSetEnabled(mInstance, true));
    mAttachHandler = aHandler;

exit:
    if (error != OT_ERROR_NONE)
    {
        if (aHandler)
        {
            aHandler(error, 0);
        }
    }
}

otError ThreadHelper::Detach(void)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = otThreadSetEnabled(mInstance, false));
    SuccessOrExit(error = otIp6SetEnabled(mInstance, false));

exit:
    return error;
}

otError ThreadHelper::Reset(void)
{
    mDeviceRoleHandlers.clear();
    otInstanceReset(mInstance);

    return OT_ERROR_NONE;
}

void ThreadHelper::JoinerStart(const std::string &aPskd,
                               const std::string &aProvisioningUrl,
                               const std::string &aVendorName,
                               const std::string &aVendorModel,
                               const std::string &aVendorSwVersion,
                               const std::string &aVendorData,
                               ResultHandler      aHandler)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(aHandler != nullptr, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(mAttachHandler == nullptr && mJoinerHandler == nullptr, error = OT_ERROR_INVALID_STATE);

    if (!otIp6IsEnabled(mInstance))
    {
        SuccessOrExit(error = otIp6SetEnabled(mInstance, true));
    }
    SuccessOrExit(error = otJoinerStart(mInstance, aPskd.c_str(), aProvisioningUrl.c_str(), aVendorName.c_str(),
                                        aVendorModel.c_str(), aVendorSwVersion.c_str(), aVendorData.c_str(),
                                        JoinerCallback, this));
    mJoinerHandler = aHandler;

exit:
    if (error != OT_ERROR_NONE)
    {
        if (aHandler)
        {
            aHandler(error);
        }
    }
}

void ThreadHelper::JoinerCallback(otError aError, void *aThreadHelper)
{
    ThreadHelper *helper = static_cast<ThreadHelper *>(aThreadHelper);

    helper->JoinerCallback(aError);
}

void ThreadHelper::JoinerCallback(otError aError)
{
    if (aError != OT_ERROR_NONE)
    {
        otbrLogWarning("Failed to join Thread network: %s", otThreadErrorToString(aError));
        mJoinerHandler(aError);
        mJoinerHandler = nullptr;
    }
    else
    {
        LogOpenThreadResult("Start Thread network", otThreadSetEnabled(mInstance, true));
    }
}

otError ThreadHelper::TryResumeNetwork(void)
{
    otError error = OT_ERROR_NONE;

    if (otLinkGetPanId(mInstance) != UINT16_MAX && otThreadGetDeviceRole(mInstance) == OT_DEVICE_ROLE_DISABLED)
    {
        if (!otIp6IsEnabled(mInstance))
        {
            SuccessOrExit(error = otIp6SetEnabled(mInstance, true));
            SuccessOrExit(error = otThreadSetEnabled(mInstance, true));
        }
    }

exit:
    if (error != OT_ERROR_NONE)
    {
        (void)otIp6SetEnabled(mInstance, false);
    }

    return error;
}

void ThreadHelper::LogOpenThreadResult(const char *aAction, otError aError)
{
    if (aError == OT_ERROR_NONE)
    {
        otbrLogInfo("%s: %s", aAction, otThreadErrorToString(aError));
    }
    else
    {
        otbrLogWarning("%s: %s", aAction, otThreadErrorToString(aError));
    }
}

void ThreadHelper::AttachAllNodesTo(const std::vector<uint8_t> &aDatasetTlvs, AttachHandler aHandler)
{
    constexpr uint32_t kDelayTimerMilliseconds = 300 * 1000;

    otError                  error = OT_ERROR_NONE;
    otOperationalDatasetTlvs datasetTlvs;
    otOperationalDataset     dataset;
    otOperationalDataset     emptyDataset{};
    otDeviceRole             role = otThreadGetDeviceRole(mInstance);
    Tlv *                    tlv;
    uint64_t                 pendingTimestamp = 0;
    timespec                 currentTime;

    assert(aHandler != nullptr);
    VerifyOrExit(mAttachHandler == nullptr && mJoinerHandler == nullptr, error = OT_ERROR_BUSY);

    VerifyOrExit(aDatasetTlvs.size() <= sizeof(datasetTlvs.mTlvs), error = OT_ERROR_INVALID_ARGS);
    std::copy(aDatasetTlvs.begin(), aDatasetTlvs.end(), datasetTlvs.mTlvs);
    datasetTlvs.mLength = aDatasetTlvs.size();

    SuccessOrExit(error = otDatasetParseTlvs(&datasetTlvs, &dataset));
    VerifyOrExit(dataset.mComponents.mIsActiveTimestampPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsNetworkKeyPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsNetworkNamePresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsExtendedPanIdPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsMeshLocalPrefixPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsPanIdPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsChannelPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsPskcPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsSecurityPolicyPresent, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(dataset.mComponents.mIsChannelMaskPresent, error = OT_ERROR_INVALID_ARGS);

    VerifyOrExit(FindTlv(OT_MESHCOP_TLV_PENDINGTIMESTAMP, datasetTlvs.mTlvs, datasetTlvs.mLength) == nullptr &&
                     FindTlv(OT_MESHCOP_TLV_DELAYTIMER, datasetTlvs.mTlvs, datasetTlvs.mLength) == nullptr,
                 error = OT_ERROR_INVALID_ARGS);

    // There must be sufficient space for a Pending Timestamp TLV and a Delay Timer TLV.
    VerifyOrExit(
        static_cast<int>(datasetTlvs.mLength +
                         (sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint64_t))    // Pending Timestamp TLV (10 bytes)
                         + (sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t))) // Delay Timer TLV (6 bytes)
            <= int{sizeof(datasetTlvs.mTlvs)},
        error = OT_ERROR_INVALID_ARGS);

    tlv = reinterpret_cast<Tlv *>(datasetTlvs.mTlvs + datasetTlvs.mLength);
    tlv->SetType(OT_MESHCOP_TLV_PENDINGTIMESTAMP);
    clock_gettime(CLOCK_REALTIME, &currentTime);
    pendingTimestamp |= (static_cast<uint64_t>(currentTime.tv_sec) << 16);
    pendingTimestamp |= (((static_cast<uint64_t>(currentTime.tv_nsec) * 32768 / 1000000000) & 0x7fff) << 1);
    tlv->SetValue(pendingTimestamp);

    tlv = tlv->GetNext();
    tlv->SetType(OT_MESHCOP_TLV_DELAYTIMER);
    tlv->SetValue(kDelayTimerMilliseconds);

    datasetTlvs.mLength = reinterpret_cast<uint8_t *>(tlv->GetNext()) - datasetTlvs.mTlvs;

    assert(datasetTlvs.mLength > 0);

    if (role == OT_DEVICE_ROLE_DISABLED || role == OT_DEVICE_ROLE_DETACHED)
    {
        otOperationalDataset existingDataset;
        bool                 hasActiveDataset;

        error = otDatasetGetActive(mInstance, &existingDataset);
        VerifyOrExit(error == OT_ERROR_NONE || error == OT_ERROR_NOT_FOUND);

        hasActiveDataset = (error == OT_ERROR_NONE);

        if (!hasActiveDataset)
        {
            SuccessOrExit(error = otDatasetSetActiveTlvs(mInstance, &datasetTlvs));
        }

        if (!otIp6IsEnabled(mInstance))
        {
            SuccessOrExit(error = otIp6SetEnabled(mInstance, true));
        }
        SuccessOrExit(error = otThreadSetEnabled(mInstance, true));

        if (hasActiveDataset)
        {
            mAttachDelayMs            = kDelayTimerMilliseconds;
            mAttachPendingDatasetTlvs = datasetTlvs;
        }
        else
        {
            mAttachDelayMs            = 0;
            mAttachPendingDatasetTlvs = {};
        }
        mAttachHandler = aHandler;
        ExitNow();
    }

    SuccessOrExit(error = otDatasetSendMgmtPendingSet(mInstance, &emptyDataset, datasetTlvs.mTlvs, datasetTlvs.mLength,
                                                      MgmtSetResponseHandler, this));
    mAttachDelayMs = kDelayTimerMilliseconds;
    mAttachHandler = aHandler;

exit:
    if (error != OT_ERROR_NONE)
    {
        aHandler(error, 0);
    }
}

void ThreadHelper::MgmtSetResponseHandler(otError aResult, void *aContext)
{
    static_cast<ThreadHelper *>(aContext)->MgmtSetResponseHandler(aResult);
}

void ThreadHelper::MgmtSetResponseHandler(otError aResult)
{
    AttachHandler handler;
    int64_t       attachDelayMs;

    LogOpenThreadResult("MgmtSetResponseHandler()", aResult);

    assert(mAttachHandler != nullptr);

    switch (aResult)
    {
    case OT_ERROR_NONE:
    case OT_ERROR_REJECTED:
        break;
    default:
        aResult = OT_ERROR_FAILED;
        break;
    }

    attachDelayMs             = mAttachDelayMs;
    handler                   = mAttachHandler;
    mAttachDelayMs            = 0;
    mAttachHandler            = nullptr;
    mAttachPendingDatasetTlvs = {};
    if (aResult == OT_ERROR_NONE)
    {
        handler(aResult, attachDelayMs);
    }
    else
    {
        handler(aResult, 0);
    }
}

#if OTBR_ENABLE_UNSECURE_JOIN
otError ThreadHelper::PermitUnsecureJoin(uint16_t aPort, uint32_t aSeconds)
{
    otError      error = OT_ERROR_NONE;
    otExtAddress steeringData;

    // 0xff to allow all devices to join
    memset(&steeringData.m8, 0xff, sizeof(steeringData.m8));
    SuccessOrExit(error = otIp6AddUnsecurePort(mInstance, aPort));
    otThreadSetSteeringData(mInstance, &steeringData);

    if (aSeconds > 0)
    {
        auto delay = Milliseconds(aSeconds * 1000);

        ++mUnsecurePortRefCounter[aPort];

        mNcp->PostTimerTask(delay, [this, aPort]() {
            assert(mUnsecurePortRefCounter.find(aPort) != mUnsecurePortRefCounter.end());
            assert(mUnsecurePortRefCounter[aPort] > 0);

            if (--mUnsecurePortRefCounter[aPort] == 0)
            {
                otExtAddress noneAddress;

                // 0 to clean steering data
                memset(&noneAddress.m8, 0, sizeof(noneAddress.m8));
                (void)otIp6RemoveUnsecurePort(mInstance, aPort);
                otThreadSetSteeringData(mInstance, &noneAddress);
                mUnsecurePortRefCounter.erase(aPort);
            }
        });
    }
    else
    {
        otExtAddress noneAddress;

        memset(&noneAddress.m8, 0, sizeof(noneAddress.m8));
        (void)otIp6RemoveUnsecurePort(mInstance, aPort);
        otThreadSetSteeringData(mInstance, &noneAddress);
    }

exit:
    return error;
}
#endif

void ThreadHelper::AddActiveDatasetChangeHandler(DatasetChangeHandler aHandler)
{
    mActiveDatasetChangeHandlers.push_back(std::move(aHandler));
}

} // namespace agent
} // namespace otbr
