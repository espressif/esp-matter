/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <esp_log.h>
#include <esp_matter_console.h>
#include <string.h>

#include "OTAImageProcessorImpl.h"
#include "platform/GenericOTARequestorDriver.h"
#include "app/clusters/ota-requestor/BDXDownloader.h"
#include "app/clusters/ota-requestor/OTARequestor.h"
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

void matter_ota_requestor_init(void)
{
    chip::SetRequestorInstance(&gRequestorCore);
    gRequestorCore.Init(&(Server::GetInstance()), &gRequestorUser, &gDownloader);
    gImageProcessor.SetOTADownloader(&gDownloader);
    gDownloader.SetImageProcessorDelegate(&gImageProcessor);
    gRequestorUser.Init(&gRequestorCore, &gImageProcessor);
}
#endif
