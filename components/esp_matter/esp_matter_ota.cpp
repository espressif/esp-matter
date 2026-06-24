// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <esp_log.h>
#include <string.h>

#include <platform/KvsPersistentStorageDelegate.h>
#include <platform/KeyValueStoreManager.h>
#include <app/clusters/ota-requestor/BDXDownloader.h>
#include <app/clusters/ota-requestor/DefaultOTARequestor.h>
#include <app/clusters/ota-requestor/ExtendedOTARequestorDriver.h>
#include <app/clusters/ota-requestor/DefaultOTARequestorStorage.h>
#include <app/clusters/ota-requestor/DefaultOTARequestorEventGenerator.h>
#include <app/clusters/ota-requestor/OTARequestorAttributes.h>
#include <app/clusters/ota-requestor/CodegenIntegration.h>
#ifdef CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM
#ifndef EXTERNAL_ESP32OTAIMAGEPROCESSORIMPL_HEADER
#error "Please define EXTERNAL_ESP32OTAIMAGEPROCESSORIMPL_HEADER in your external platform gn/cmake file"
#endif // !EXTERNAL_ESP32OTAIMAGEPROCESSORIMPL_HEADER
#include EXTERNAL_ESP32OTAIMAGEPROCESSORIMPL_HEADER
#else // CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM
#include <platform/ESP32/OTAImageProcessorImpl.h>
#endif // !CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM

#if CONFIG_ENABLE_OTA_REQUESTOR && CONFIG_AUTO_UPDATE_RCP && CONFIG_OPENTHREAD_BORDER_ROUTER
#include "esp_rcp_ota.h"
#endif

#include <esp_matter.h>
#include <esp_matter_ota.h>
#include <zap-generated/endpoint_config.h>

using chip::BDXDownloader;
using chip::DefaultOTARequestor;
using chip::DefaultOTARequestorStorage;
using chip::OTAImageProcessorImpl;
using chip::Server;
using chip::DeviceLayer::ExtendedOTARequestorDriver;

using namespace esp_matter;

#if CONFIG_ENABLE_OTA_REQUESTOR && CONFIG_AUTO_UPDATE_RCP && CONFIG_OPENTHREAD_BORDER_ROUTER
class OTARcpProcessorImpl : public chip::OTAImageProcessorImpl::OTARcpProcessorDelegate {
public:
    esp_err_t OnOtaRcpPrepareDownload() override;
    esp_err_t OnOtaRcpProcessBlock(const uint8_t * buffer, size_t bufLen, size_t  &rcpOtaReceivedLen) override;
    esp_err_t OnOtaRcpFinalize() override;
    esp_err_t OnOtaRcpAbort() override;

private:
    void ResetRcpOtaState()
    {
        mRcpOtaHandle          = 0;
        mBrFirmwareSize        = 0;
        mRcpFirmwareDownloaded = false;
    }
    esp_rcp_ota_handle_t mRcpOtaHandle;
    bool mRcpFirmwareDownloaded;
    uint32_t mBrFirmwareSize;
};

esp_err_t OTARcpProcessorImpl::OnOtaRcpPrepareDownload()
{
    ResetRcpOtaState();
    return esp_rcp_ota_begin(&mRcpOtaHandle);
}

esp_err_t OTARcpProcessorImpl::OnOtaRcpProcessBlock(const uint8_t * buffer, size_t bufLen, size_t  &rcpOtaReceivedLen)
{
    esp_err_t err = ESP_OK;

    if (!mRcpFirmwareDownloaded) {
        err = esp_rcp_ota_receive(mRcpOtaHandle, buffer, bufLen, &rcpOtaReceivedLen);

        if (esp_rcp_ota_get_state(mRcpOtaHandle) == ESP_RCP_OTA_STATE_FINISHED) {
            mBrFirmwareSize        = esp_rcp_ota_get_subfile_size(mRcpOtaHandle, FILETAG_HOST_FIRMWARE);
            mRcpFirmwareDownloaded = true;
        }
    } else if (mBrFirmwareSize > 0) {
        rcpOtaReceivedLen = 0;
    } else {
        err = ESP_FAIL;
    }

    return err;
}

esp_err_t OTARcpProcessorImpl::OnOtaRcpFinalize()
{
    esp_err_t err = esp_rcp_ota_end(mRcpOtaHandle);
    ResetRcpOtaState();
    return err;
}

esp_err_t OTARcpProcessorImpl::OnOtaRcpAbort()
{
    esp_err_t err = esp_rcp_ota_abort(mRcpOtaHandle);
    ResetRcpOtaState();
    return err;
}

OTARcpProcessorImpl gOtaRcpDelegate;
#endif // CONFIG_ENABLE_OTA_REQUESTOR && CONFIG_AUTO_UPDATE_RCP && CONFIG_OPENTHREAD_BORDER_ROUTER

#if CONFIG_ENABLE_OTA_REQUESTOR
DefaultOTARequestor gRequestorCore;
DefaultOTARequestorStorage gRequestorStorage;
ExtendedOTARequestorDriver gRequestorUser;
BDXDownloader gDownloader;
OTAImageProcessorImpl gImageProcessor;
// OTARequestorAttributes and event generator are owned by the integration layer
// (data_model_provider/clusters/ota_software_update_requestor/integration.cpp)
// and accessed via chip::GetOTARequestorAttributes() / chip::GetDefaultOTARequestorEventGenerator().

static esp_matter_ota_requestor_impl_t s_ota_requestor_impl = {
    .driver = &gRequestorUser,
    .image_processor = &gImageProcessor,
};
#endif // CONFIG_ENABLE_OTA_REQUESTOR

esp_err_t esp_matter_ota_requestor_init(void)
{
#if (CONFIG_ENABLE_OTA_REQUESTOR && (FIXED_ENDPOINT_COUNT == 0))
    endpoint::ota_requestor::config_t config;
    node_t *root_node = esp_matter::node::get();
    endpoint_t *root_node_endpoint = esp_matter::endpoint::get(root_node, 0);

    if (!root_node || !root_node_endpoint) {
        return ESP_FAIL;
    }

    return endpoint::ota_requestor::add(root_node_endpoint, &config);
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

#if CONFIG_ENABLE_OTA_REQUESTOR
static esp_err_t esp_matter_ota_override_impl(const esp_matter_ota_requestor_impl_t *impl)
{
    VerifyOrReturnError(impl != nullptr, ESP_ERR_INVALID_ARG);

    if (impl->driver != nullptr) {
        s_ota_requestor_impl.driver = impl->driver;
    }
    if (impl->image_processor != nullptr) {
        s_ota_requestor_impl.image_processor = impl->image_processor;
    }

    s_ota_requestor_impl.user_consent = impl->user_consent;

    return ESP_OK;
}
#endif // CONFIG_ENABLE_OTA_REQUESTOR

void esp_matter_ota_requestor_start(void)
{
#if CONFIG_ENABLE_OTA_REQUESTOR
    VerifyOrReturn(chip::GetRequestorInstance() == nullptr);

    chip::SetRequestorInstance(&gRequestorCore);

    // Server::GetPersistentStorage() may not be available yet at this point in boot.
    // Use the initParams storage delegate which is set up before Server::Init().
    static chip::KvsPersistentStorageDelegate sOtaStorageDelegate;
    chip::DeviceLayer::PersistedStorage::KeyValueStoreManager &kvsManager =
        chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr();
    CHIP_ERROR err = sOtaStorageDelegate.Init(&kvsManager);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE("OTARequestor", "Failed to init OTARequestor storage delegate: %" CHIP_ERROR_FORMAT, err.Format());
        return;
    }
    gRequestorStorage.Init(sOtaStorageDelegate);

    if (gRequestorCore.Init(Server::GetInstance(), gRequestorStorage, *s_ota_requestor_impl.driver, gDownloader,
                            chip::GetOTARequestorAttributes(), chip::GetDefaultOTARequestorEventGenerator()) != CHIP_NO_ERROR) {
        ESP_LOGE("OTARequestor", "Failed to init OTARequestor core");
        return;
    }

    gImageProcessor.SetOTADownloader(&gDownloader);
#if CONFIG_AUTO_UPDATE_RCP && CONFIG_OPENTHREAD_BORDER_ROUTER
    gImageProcessor.SetOtaRcpDelegate(&gOtaRcpDelegate);
#endif

    gDownloader.SetImageProcessorDelegate(s_ota_requestor_impl.image_processor);

    s_ota_requestor_impl.driver->SetUserConsentDelegate(s_ota_requestor_impl.user_consent);
    s_ota_requestor_impl.driver->Init(&gRequestorCore, s_ota_requestor_impl.image_processor);
#endif
}

#ifdef CONFIG_ENABLE_ENCRYPTED_OTA
esp_err_t esp_matter_ota_requestor_encrypted_init(const char *key, uint16_t size)
{
    VerifyOrReturnError(key != nullptr, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(gImageProcessor.InitEncryptedOTA(chip::CharSpan{key, size}) == CHIP_NO_ERROR, ESP_ERR_INVALID_STATE);
    return ESP_OK;
}
#endif // CONFIG_ENABLE_ENCRYPTED_OTA

esp_err_t esp_matter_ota_requestor_set_config(const esp_matter_ota_config_t  &config)
{
#if CONFIG_ENABLE_OTA_REQUESTOR
    if (config.periodic_query_timeout) {
        gRequestorUser.SetPeriodicQueryTimeout(config.periodic_query_timeout);
    }
    if (config.watchdog_timeout) {
        gRequestorUser.SetWatchdogTimeout(config.watchdog_timeout);
    }
    if (config.impl != nullptr) {
        esp_matter_ota_override_impl(config.impl);
    }

    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_ENABLE_OTA_REQUESTOR
}

// Weak plugin init stub — overridden by upstream CodegenIntegration.cpp when ZAP build path is used
__attribute__((weak)) void MatterOtaSoftwareUpdateRequestorPluginServerInitCallback() {}
