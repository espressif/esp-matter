/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "app_driver.h"
#include "app_matter.h"
#include "app_constants.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

#include "app/common/gen/att-storage.h"
#include "app/common/gen/attribute-id.h"
#include "app/common/gen/attribute-type.h"
#include "app/common/gen/cluster-id.h"
#include "app/server/Mdns.h"
#include "app/server/Server.h"
#include "app/util/af.h"
#include "app/util/basic-types.h"
#include "platform/CHIPDeviceLayer.h"
#include "core/CHIPError.h"
#include "lib/shell/Engine.h"
#include "lib/support/CHIPMem.h"

using chip::AttributeId;
using chip::ClusterId;
using chip::EndpointId;
using chip::DeviceLayer::ChipDeviceEvent;
using chip::DeviceLayer::ConnectivityMgr;
using chip::DeviceLayer::DeviceEventType::PublicEventTypes;
using chip::DeviceLayer::PlatformMgr;

static void on_on_off_attribute_changed(chip::EndpointId endpoint, chip::AttributeId attribute, uint8_t *value, size_t size)
{
    if (attribute == ZCL_ON_OFF_ATTRIBUTE_ID) {
        app_driver_update_and_report_power(*value, APP_DRIVER_SRC_MATTER);
    } else {
        ESP_LOGW(APP_LOG_TAG, "Unknown attribute in On/Off cluster: %d", attribute);
    }
}

static void on_level_control_atrribute_changed(chip::EndpointId endpoint, chip::AttributeId attribute, uint8_t *value,
        size_t size)
{
    if (attribute == ZCL_CURRENT_LEVEL_ATTRIBUTE_ID) {
        app_driver_update_and_report_brightness(*value, APP_DRIVER_SRC_MATTER);
    } else {
        ESP_LOGW(APP_LOG_TAG, "Unknown attribute in level control cluster: %d", attribute);
    }
}

static void on_color_control_attribute_changed(chip::EndpointId endpoint, chip::AttributeId attribute, uint8_t *value,
        size_t size)
{
    if (attribute == ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID) {
        // remap hue to [0, 359]
        uint16_t hue = REMAP_TO_RANGE(static_cast<uint16_t>(*value), HUE_ATTRIBUTE_MAX, HUE_MAX);
        app_driver_update_and_report_hue(hue, APP_DRIVER_SRC_MATTER);
    } else if (attribute == ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID) {
        // remap saturation to [0, 100]
        uint8_t saturation = REMAP_TO_RANGE(static_cast<uint16_t>(*value), SATURATION_ATTRIBUTE_MAX, SATURATION_MAX);
        app_driver_update_and_report_saturation(saturation, APP_DRIVER_SRC_MATTER);
    } else {
        ESP_LOGW(APP_LOG_TAG, "Unknown attribute in level control cluster: %d", attribute);
    }
}

static void on_device_event(const ChipDeviceEvent *event, intptr_t arg)
{
    if (event->Type == PublicEventTypes::kInterfaceIpAddressChanged) {
        chip::app::Mdns::StartServer();
    }

    ESP_LOGI(APP_LOG_TAG, "Current free heap: %zu", heap_caps_get_free_size(MALLOC_CAP_8BIT));
}

static void update_matter_power(bool power)
{
    EmberAfStatus status;

    status = emberAfWriteAttribute(1, ZCL_ON_OFF_CLUSTER_ID, ZCL_ON_OFF_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                                   (uint8_t *)&power, ZCL_BOOLEAN_ATTRIBUTE_TYPE);
    assert(status == EMBER_ZCL_STATUS_SUCCESS);
}

static void update_matter_brightness(uint8_t brightness)
{
    EmberAfStatus status;

    status = emberAfWriteAttribute(1, ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_CURRENT_LEVEL_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                                   &brightness, ZCL_INT8U_ATTRIBUTE_TYPE);
    assert(status == EMBER_ZCL_STATUS_SUCCESS);
}

static void update_matter_hue(uint16_t hue)
{
    EmberAfStatus status;
    uint8_t hue_attribute = REMAP_TO_RANGE(hue, HUE_MAX, HUE_ATTRIBUTE_MAX);

    status = emberAfWriteAttribute(1, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID,
                                   CLUSTER_MASK_SERVER, &hue_attribute, ZCL_INT8U_ATTRIBUTE_TYPE);
    assert(status == EMBER_ZCL_STATUS_SUCCESS);
}

static void update_matter_saturation(uint8_t saturation)
{
    EmberAfStatus status;
    uint8_t saturation_attribute = REMAP_TO_RANGE(static_cast<uint16_t>(saturation), SATURATION_MAX, SATURATION_ATTRIBUTE_MAX);

    status = emberAfWriteAttribute(1, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID,
                                   CLUSTER_MASK_SERVER, &saturation_attribute, ZCL_INT8U_ATTRIBUTE_TYPE);
    assert(status == EMBER_ZCL_STATUS_SUCCESS);
}


void emberAfPostAttributeChangeCallback(EndpointId endpoint, ClusterId cluster, AttributeId attribute, uint8_t mask,
                                        uint16_t manufacturer, uint8_t type, uint16_t size, uint8_t *value)
{
    ESP_LOGI(APP_LOG_TAG, "Handle cluster ID: %d", cluster);
    if (cluster == ZCL_ON_OFF_CLUSTER_ID) {
        on_on_off_attribute_changed(endpoint, attribute, value, size);
    } else if (cluster == ZCL_LEVEL_CONTROL_CLUSTER_ID) {
        on_level_control_atrribute_changed(endpoint, attribute, value, size);
    } else if (cluster == ZCL_COLOR_CONTROL_CLUSTER_ID) {
        on_color_control_attribute_changed(endpoint, attribute, value, size);
    }
}

esp_err_t app_matter_init()
{
    app_driver_param_callback_t callbacks = {
        .update_power = update_matter_power,
        .update_brightness = update_matter_brightness,
        .update_hue = update_matter_hue,
        .update_saturation = update_matter_saturation,
        .update_temperature = NULL,
    };

    if (PlatformMgr().InitChipStack() != CHIP_NO_ERROR) {
        ESP_LOGE(APP_LOG_TAG, "Failed to initialize CHIP stack");
        return ESP_FAIL;
    }
    ConnectivityMgr().SetBLEAdvertisingEnabled(true);
    if (chip::Platform::MemoryInit() != CHIP_NO_ERROR) {
        ESP_LOGE(APP_LOG_TAG, "Failed to initialize CHIP memory pool");
        return ESP_ERR_NO_MEM;
    }
    if (PlatformMgr().StartEventLoopTask() != CHIP_NO_ERROR) {
        chip::Platform::MemoryShutdown();
        ESP_LOGE(APP_LOG_TAG, "Failed to launch Matter main task");
        return ESP_FAIL;
    }
    PlatformMgr().AddEventHandler(on_device_event, static_cast<intptr_t>(NULL));

    InitServer();

    app_driver_register_src(APP_DRIVER_SRC_MATTER, &callbacks);

    return ESP_OK;
}
