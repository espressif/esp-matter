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
 *   This file includes definitions for Thread helper.
 */

#ifndef OTBR_THREAD_HELPER_HPP_
#define OTBR_THREAD_HELPER_HPP_

#include <chrono>
#include <functional>
#include <map>
#include <random>
#include <string>
#include <vector>

#include <openthread/instance.h>
#include <openthread/ip6.h>
#include <openthread/jam_detection.h>
#include <openthread/joiner.h>
#include <openthread/netdata.h>
#include <openthread/thread.h>

namespace otbr {
namespace Ncp {
class ControllerOpenThread;
}
} // namespace otbr

namespace otbr {
namespace agent {

/**
 * This class implements Thread helper.
 */
class ThreadHelper
{
public:
    using DeviceRoleHandler       = std::function<void(otDeviceRole)>;
    using ScanHandler             = std::function<void(otError, const std::vector<otActiveScanResult> &)>;
    using EnergyScanHandler       = std::function<void(otError, const std::vector<otEnergyScanResult> &)>;
    using ResultHandler           = std::function<void(otError)>;
    using AttachHandler           = std::function<void(otError, int64_t)>;
    using UpdateMeshCopTxtHandler = std::function<void(std::map<std::string, std::vector<uint8_t>>)>;
    using DatasetChangeHandler    = std::function<void(const otOperationalDatasetTlvs &)>;

    /**
     * The constructor of a Thread helper.
     *
     * @param[in] aInstance  The Thread instance.
     * @param[in] aNcp       The ncp controller.
     *
     */
    ThreadHelper(otInstance *aInstance, otbr::Ncp::ControllerOpenThread *aNcp);

    /**
     * This method adds a callback for device role change.
     *
     * @param[in] aHandler  The device role handler.
     *
     */
    void AddDeviceRoleHandler(DeviceRoleHandler aHandler);

    /**
     * This method adds a callback for active dataset change.
     *
     * @param[in]  aHandler   The active dataset change handler.
     */
    void AddActiveDatasetChangeHandler(DatasetChangeHandler aHandler);

    /**
     * This method permits unsecure join on port.
     *
     * @param[in] aPort     The port number.
     * @param[in] aSeconds  The timeout to close the port, 0 for never close.
     *
     * @returns The error value of underlying OpenThread api calls.
     *
     */
    otError PermitUnsecureJoin(uint16_t aPort, uint32_t aSeconds);

    /**
     * This method performs a Thread network scan.
     *
     * @param[in] aHandler  The scan result handler.
     *
     */
    void Scan(ScanHandler aHandler);

    /**
     * This method performs an IEEE 802.15.4 Energy Scan.
     *
     * @param[in] aScanDuration  The duration for the scan, in milliseconds.
     * @param[in] aHandler       The scan result handler.
     *
     */
    void EnergyScan(uint32_t aScanDuration, EnergyScanHandler aHandler);

    /**
     * This method attaches the device to the Thread network.
     *
     * @note The joiner start and the attach proccesses are exclusive
     *
     * @param[in] aNetworkName  The network name.
     * @param[in] aPanId        The pan id, UINT16_MAX for random.
     * @param[in] aExtPanId     The extended pan id, UINT64_MAX for random.
     * @param[in] aNetworkKey   The network key, empty for random.
     * @param[in] aPSKc         The pre-shared commissioner key, empty for random.
     * @param[in] aChannelMask  A bitmask for valid channels, will random select one.
     * @param[in] aHandler      The attach result handler.
     *
     */
    void Attach(const std::string &         aNetworkName,
                uint16_t                    aPanId,
                uint64_t                    aExtPanId,
                const std::vector<uint8_t> &aNetworkKey,
                const std::vector<uint8_t> &aPSKc,
                uint32_t                    aChannelMask,
                AttachHandler               aHandler);

    /**
     * This method detaches the device from the Thread network.
     *
     * @returns The error value of underlying OpenThread API calls.
     *
     */
    otError Detach(void);

    /**
     * This method attaches the device to the Thread network.
     *
     * @note The joiner start and the attach proccesses are exclusive, and the
     *       network parameter will be set through the active dataset.
     *
     * @param[in] aHandler  The attach result handler.
     *
     */
    void Attach(AttachHandler aHandler);

    /**
     * This method makes all nodes in the current network attach to the network specified by the dataset TLVs.
     *
     * @param[in] aDatasetTlvs  The dataset TLVs.
     * @param[in] aHandler      The result handler.
     *
     */
    void AttachAllNodesTo(const std::vector<uint8_t> &aDatasetTlvs, AttachHandler aHandler);

    /**
     * This method resets the OpenThread stack.
     *
     * @returns The error value of underlying OpenThread api calls.
     *
     */
    otError Reset(void);

    /**
     * This method triggers a thread join process.
     *
     * @note The joiner start and the attach proccesses are exclusive
     *
     * @param[in] aPskd             The pre-shared key for device.
     * @param[in] aProvisioningUrl  The provision url.
     * @param[in] aVendorName       The vendor name.
     * @param[in] aVendorModel      The vendor model.
     * @param[in] aVendorSwVersion  The vendor software version.
     * @param[in] aVendorData       The vendor custom data.
     * @param[in] aHandler          The join result handler.
     *
     */
    void JoinerStart(const std::string &aPskd,
                     const std::string &aProvisioningUrl,
                     const std::string &aVendorName,
                     const std::string &aVendorModel,
                     const std::string &aVendorSwVersion,
                     const std::string &aVendorData,
                     ResultHandler      aHandler);

    /**
     * This method tries to restore the network after reboot
     *
     * @returns The error value of underlying OpenThread api calls.
     *
     */
    otError TryResumeNetwork(void);

    /**
     * This method returns the underlying OpenThread instance.
     *
     * @returns The underlying instance.
     *
     */
    otInstance *GetInstance(void) { return mInstance; }

    /**
     * This method handles OpenThread state changed notification.
     *
     * @param[in] aFlags    A bit-field indicating specific state that has changed.  See `OT_CHANGED_*` definitions.
     *
     */
    void StateChangedCallback(otChangedFlags aFlags);

#if OTBR_ENABLE_DBUS_SERVER
    /**
     * This method sets a callback for calls of UpdateVendorMeshCopTxtEntries D-Bus API.
     *
     * @param[in] aHandler  The handler on MeshCoP TXT changes.
     *
     */
    void SetUpdateMeshCopTxtHandler(UpdateMeshCopTxtHandler aHandler)
    {
        mUpdateMeshCopTxtHandler = std::move(aHandler);
    }

    /**
     * This method handles MeshCoP TXT updates done by UpdateVendorMeshCopTxtEntries D-Bus API.
     *
     * @param[in] aUpdate  The key-value pairs to be updated in the TXT record.
     *
     */
    void OnUpdateMeshCopTxt(std::map<std::string, std::vector<uint8_t>> aUpdate);
#endif

    /**
     * This method logs OpenThread action result.
     *
     * @param[in] aAction  The action OpenThread performs.
     * @param[in] aError   The action result.
     *
     */
    static void LogOpenThreadResult(const char *aAction, otError aError);

private:
    static void ActiveScanHandler(otActiveScanResult *aResult, void *aThreadHelper);
    void        ActiveScanHandler(otActiveScanResult *aResult);

    static void EnergyScanCallback(otEnergyScanResult *aResult, void *aThreadHelper);
    void        EnergyScanCallback(otEnergyScanResult *aResult);

    static void JoinerCallback(otError aError, void *aThreadHelper);
    void        JoinerCallback(otError aResult);

    static void MgmtSetResponseHandler(otError aResult, void *aContext);
    void        MgmtSetResponseHandler(otError aResult);

    void    RandomFill(void *aBuf, size_t size);
    uint8_t RandomChannelFromChannelMask(uint32_t aChannelMask);

    void ActiveDatasetChangedCallback();

    otInstance *mInstance;

    otbr::Ncp::ControllerOpenThread *mNcp;

    ScanHandler                     mScanHandler;
    std::vector<otActiveScanResult> mScanResults;
    EnergyScanHandler               mEnergyScanHandler;
    std::vector<otEnergyScanResult> mEnergyScanResults;

    std::vector<DeviceRoleHandler>    mDeviceRoleHandlers;
    std::vector<DatasetChangeHandler> mActiveDatasetChangeHandlers;

    std::map<uint16_t, size_t> mUnsecurePortRefCounter;

    int64_t       mAttachDelayMs = 0;
    AttachHandler mAttachHandler;
    ResultHandler mJoinerHandler;

    otOperationalDatasetTlvs mAttachPendingDatasetTlvs = {};

    std::random_device mRandomDevice;

#if OTBR_ENABLE_DBUS_SERVER
    UpdateMeshCopTxtHandler mUpdateMeshCopTxtHandler;
#endif
};

} // namespace agent
} // namespace otbr

#endif // OTBR_THREAD_HELPER_HPP_
