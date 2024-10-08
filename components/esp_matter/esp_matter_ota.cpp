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

#include <app/clusters/ota-requestor/BDXDownloader.h>
#include <app/clusters/ota-requestor/DefaultOTARequestor.h>
#include <app/clusters/ota-requestor/ExtendedOTARequestorDriver.h>
#include <app/clusters/ota-requestor/DefaultOTARequestorStorage.h>
#include <platform/ESP32/OTAImageProcessorImpl.h>

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
using namespace esp_matter::endpoint;
using namespace esp_matter::cluster;

#if CONFIG_ENABLE_OTA_REQUESTOR
DefaultOTARequestor gRequestorCore;
DefaultOTARequestorStorage gRequestorStorage;
ExtendedOTARequestorDriver gRequestorUser;
BDXDownloader gDownloader;
OTAImageProcessorImpl gImageProcessor;

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

static esp_err_t esp_matter_ota_override_impl(const esp_matter_ota_requestor_impl_t *impl)
{
#if CONFIG_ENABLE_OTA_REQUESTOR
    VerifyOrReturnError(impl != nullptr, ESP_ERR_INVALID_ARG);

    if (impl->driver != nullptr) {
        s_ota_requestor_impl.driver = impl->driver;
    }
    if (impl->image_processor != nullptr) {
        s_ota_requestor_impl.image_processor = impl->image_processor;
    }

    s_ota_requestor_impl.user_consent = impl->user_consent;

    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_ENABLE_OTA_REQUESTOR
}

void esp_matter_ota_requestor_start(void)
{
#if CONFIG_ENABLE_OTA_REQUESTOR
    VerifyOrReturn(chip::GetRequestorInstance() == nullptr);

    chip::SetRequestorInstance(&gRequestorCore);
    gRequestorStorage.Init(Server::GetInstance().GetPersistentStorage());

    gRequestorCore.Init(Server::GetInstance(), gRequestorStorage, *s_ota_requestor_impl.driver, gDownloader);

    s_ota_requestor_impl.image_processor->SetOTADownloader(&gDownloader);

    gDownloader.SetImageProcessorDelegate(s_ota_requestor_impl.image_processor);

    s_ota_requestor_impl.driver->SetUserConsentDelegate(s_ota_requestor_impl.user_consent);
    s_ota_requestor_impl.driver->Init(&gRequestorCore, s_ota_requestor_impl.image_processor);
#endif
}

#if CONFIG_ENABLE_ENCRYPTED_OTA
esp_err_t esp_matter_ota_requestor_encrypted_init(const char *key, uint16_t size)
{
    VerifyOrReturnError(key != nullptr, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(gImageProcessor.InitEncryptedOTA(chip::CharSpan{key, size}) == CHIP_NO_ERROR, ESP_ERR_INVALID_STATE);
    return ESP_OK;
}
#endif // CONFIG_ENABLE_ENCRYPTED_OTA


esp_err_t esp_matter_ota_requestor_set_config(const esp_matter_ota_config_t & config)
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
