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
#include "OTARequestorDriverImpl.h"
#include "app/clusters/ota-requestor/BDXDownloader.h"
#include "app/clusters/ota-requestor/OTARequestor.h"
#include "platform/OTARequestorInterface.h"

using chip::BDXDownloader;
using chip::OTAImageProcessorImpl;
using chip::OTARequestor;
using chip::OTARequestorDriverImpl;
using chip::Server;

static const char *TAG = "esp_matter_ota";
OTARequestor gRequestorCore;
OTARequestorDriverImpl gRequestorUser;
BDXDownloader gDownloader;
OTAImageProcessorImpl gImageProcessor;

static esp_err_t apply_image_handler(int argc, char **argv)
{
    chip::OTARequestor *requestor = reinterpret_cast<chip::OTARequestor *>(chip::GetRequestorInstance());
    requestor->ApplyUpdate();
    return ESP_OK;
}

static esp_err_t esp_matter_console_ota_handler(int argc, char **argv)
{
    if (argc == 1 && strncmp(argv[0], "apply", sizeof("apply")) == 0) {
        return apply_image_handler(argc, argv);
    } else {
        ESP_LOGE(TAG, "Incorrect arguments");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void esp_matter_console_ota_register_commands()
{
    esp_matter_console_command_t command = {
        .name = "ota",
        .description = "ota command. Usage matter esp ota <ota_command>. OTA command: apply",
        .handler = esp_matter_console_ota_handler,
    };
    esp_matter_console_add_command(&command);
}

void matter_ota_requestor_init(void)
{
    chip::SetRequestorInstance(&gRequestorCore);
    Server *server = &(Server::GetInstance());
    gRequestorCore.SetServerInstance(server);
    gRequestorCore.SetOtaRequestorDriver(&gRequestorUser);
    gImageProcessor.SetOTADownloader(&gDownloader);
    gDownloader.SetImageProcessorDelegate(&gImageProcessor);
    gRequestorCore.SetBDXDownloader(&gDownloader);

    esp_matter_console_ota_register_commands();
}
