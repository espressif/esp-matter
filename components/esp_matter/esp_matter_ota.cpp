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
#include <app/clusters/ota-requestor/DefaultOTARequestorDriver.h>
#include <app/clusters/ota-requestor/DefaultOTARequestorStorage.h>
#include <platform/ESP32/OTAImageProcessorImpl.h>

#include <esp_matter.h>

using chip::BDXDownloader;
using chip::DefaultOTARequestor;
using chip::DefaultOTARequestorStorage;
using chip::OTAImageProcessorImpl;
using chip::Server;
using chip::DeviceLayer::DefaultOTARequestorDriver;

using namespace esp_matter;
using namespace esp_matter::endpoint;
using namespace esp_matter::cluster;

#if CONFIG_ENABLE_OTA_REQUESTOR
DefaultOTARequestor gRequestorCore;
DefaultOTARequestorStorage gRequestorStorage;
DefaultOTARequestorDriver gRequestorUser;
BDXDownloader gDownloader;
OTAImageProcessorImpl gImageProcessor;
#endif

esp_err_t esp_matter_ota_requestor_init(void)
{
#if (CONFIG_ENABLE_OTA_REQUESTOR && (FIXED_ENDPOINT_COUNT == 0))
    ota_requestor::config_t config;
    node_t *root_node = esp_matter::node::get();
    endpoint_t *root_node_endpoint = esp_matter::endpoint::get(root_node, 0);

    if (!root_node || !root_node_endpoint) {
        return ESP_FAIL;
    }

    cluster_t *cluster_p = ota_provider::create(root_node_endpoint, NULL, CLUSTER_FLAG_CLIENT);
    cluster_t *cluster_r = ota_requestor::create(root_node_endpoint, &config, CLUSTER_FLAG_SERVER);

    if (!cluster_p || !cluster_r) {
        return ESP_FAIL;
    }

    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

void esp_matter_ota_requestor_start(void)
{
#if CONFIG_ENABLE_OTA_REQUESTOR
    VerifyOrReturn(chip::GetRequestorInstance() == nullptr);
    chip::SetRequestorInstance(&gRequestorCore);
    gRequestorStorage.Init(Server::GetInstance().GetPersistentStorage());
    gRequestorCore.Init(Server::GetInstance(), gRequestorStorage, gRequestorUser, gDownloader);
    gImageProcessor.SetOTADownloader(&gDownloader);
    gDownloader.SetImageProcessorDelegate(&gImageProcessor);
    gRequestorUser.Init(&gRequestorCore, &gImageProcessor);
#endif
}
