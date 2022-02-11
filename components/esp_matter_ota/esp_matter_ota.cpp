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
#include <esp_matter_ota.h>
#include <string.h>

#include "app/clusters/ota-requestor/BDXDownloader.h"
#include "app/clusters/ota-requestor/OTARequestor.h"
#include "OTAImageProcessorImpl.h"
#include "platform/GenericOTARequestorDriver.h"
#include "platform/OTARequestorInterface.h"

using chip::BDXDownloader;
using chip::OTAImageProcessorImpl;
using chip::OTARequestor;
using chip::DeviceLayer::GenericOTARequestorDriver;
using chip::Server;
#if CONFIG_ENABLE_OTA_REQUESTOR 
OTARequestor gRequestorCore;
GenericOTARequestorDriver gRequestorUser;
BDXDownloader gDownloader;
OTAImageProcessorImpl gImageProcessor;

void esp_matter_ota_requestor_init(void)
{
    chip::SetRequestorInstance(&gRequestorCore);
    gRequestorCore.Init(&(Server::GetInstance()), &gRequestorUser, &gDownloader);
    gImageProcessor.SetOTADownloader(&gDownloader);
    gDownloader.SetImageProcessorDelegate(&gImageProcessor);
    gRequestorUser.Init(&gRequestorCore, &gImageProcessor);
}
#endif
