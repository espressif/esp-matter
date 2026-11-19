/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unity.h>
#include <esp_matter.h>
#include <esp_matter_bridge.h>
#include <esp_matter_mem.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <nvs_key_allocator.h>
#include <sdkconfig.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <platform/PlatformManager.h>

#include "cluster_lifecycle_common.h"

#if MAX_BRIDGED_DEVICE_COUNT > 0

using namespace esp_matter;
using esp_matter_bridge::device_persistent_info_t;
using esp_matter_bridge::device_t;

static uint16_t s_aggregator_endpoint_id = chip::kInvalidEndpointId;

static esp_err_t device_type_callback(endpoint_t *ep, uint32_t device_type_id, void *priv_data)
{
    if (device_type_id != ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID) {
        return ESP_ERR_INVALID_ARG;
    }
    endpoint::on_off_light::config_t config;
    return endpoint::on_off_light::add(ep, &config);
}

static node_t *setup_bridge()
{
    node_t *node = test::get_or_create_node();
    if (s_aggregator_endpoint_id == chip::kInvalidEndpointId) {
        endpoint::aggregator::config_t aggregator_config;
        endpoint_t *aggregator = endpoint::aggregator::create(node, &aggregator_config, ENDPOINT_FLAG_NONE, nullptr);
        TEST_ASSERT_NOT_NULL(aggregator);
        s_aggregator_endpoint_id = endpoint::get_id(aggregator);
        if (esp_matter::is_started()) {
            TEST_ASSERT_EQUAL(ESP_OK, endpoint::enable(aggregator));
        }
    }
    test::start_matter_if_needed();
    // initialize() reads the endpoint id array from NVS again, like after a restart.
    TEST_ASSERT_EQUAL(ESP_OK, esp_matter_bridge::initialize(node, device_type_callback));
    return node;
}

// Run the pending Matter task work now, so that its logs do not split the Unity result line.
static void wait_for_matter_work()
{
    SemaphoreHandle_t done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done);
    CHIP_ERROR err = chip::DeviceLayer::PlatformMgr().ScheduleWork([](intptr_t arg) {
        xSemaphoreGive(reinterpret_cast<SemaphoreHandle_t>(arg));
    }, reinterpret_cast<intptr_t>(done));
    bool finished = (err == CHIP_NO_ERROR) && (xSemaphoreTake(done, pdMS_TO_TICKS(5000)) == pdTRUE);
    // On a timeout the work can still use the semaphore, so it is not deleted.
    if (finished || err != CHIP_NO_ERROR) {
        vSemaphoreDelete(done);
    }
    TEST_ASSERT_TRUE(finished);
}

static bool endpoint_id_is_stored(uint16_t endpoint_id)
{
    uint16_t endpoint_ids[MAX_BRIDGED_DEVICE_COUNT];
    TEST_ASSERT_EQUAL(ESP_OK, esp_matter_bridge::get_bridged_endpoint_ids(endpoint_ids));
    for (size_t idx = 0; idx < MAX_BRIDGED_DEVICE_COUNT; ++idx) {
        if (endpoint_ids[idx] == endpoint_id) {
            return true;
        }
    }
    return false;
}

static nvs_handle_t open_bridge_nvs(nvs_open_mode_t mode)
{
    nvs_handle_t handle;
    TEST_ASSERT_EQUAL(ESP_OK, nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                                                      ESP_MATTER_BRIDGE_NAMESPACE, mode, &handle));
    return handle;
}

static esp_err_t read_stored_info(uint16_t endpoint_id, device_persistent_info_t *info, size_t *len)
{
    nvs_handle_t handle = open_bridge_nvs(NVS_READONLY);
    esp_err_t err = nvs_get_blob(handle, esp_matter_bridge::nvs_key_allocator::endpoint_pesistent_info(endpoint_id).KeyName(),
                                 info, len);
    nvs_close(handle);
    return err;
}

static void write_stored_info(uint16_t endpoint_id, const void *data, size_t len)
{
    nvs_handle_t handle = open_bridge_nvs(NVS_READWRITE);
    TEST_ASSERT_EQUAL(ESP_OK, nvs_set_blob(handle,
                                           esp_matter_bridge::nvs_key_allocator::endpoint_pesistent_info(endpoint_id).KeyName(),
                                           data, len));
    TEST_ASSERT_EQUAL(ESP_OK, nvs_commit(handle));
    nvs_close(handle);
}

// Create a bridged device and then drop its endpoint and its device object, but keep the stored info.
// This is the state after a restart, before resume_device() is called.
static uint16_t create_stored_device(node_t *node, device_persistent_info_t *stored_info)
{
    device_t *dev = esp_matter_bridge::create_device(node, s_aggregator_endpoint_id,
                                                     ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID, nullptr);
    TEST_ASSERT_NOT_NULL(dev);
    uint16_t endpoint_id = dev->persistent_info.device_endpoint_id;
    if (stored_info) {
        *stored_info = dev->persistent_info;
    }
    // The pending work needs the endpoint.
    wait_for_matter_work();
    TEST_ASSERT_EQUAL(ESP_OK, endpoint::destroy(node, dev->endpoint));
    esp_matter_mem_free(dev);
    return endpoint_id;
}

TEST_CASE("bridged device is stored, resumed and removed", "[bridge]")
{
    node_t *node = setup_bridge();
    device_persistent_info_t created_info = {};
    uint16_t endpoint_id = create_stored_device(node, &created_info);

    // The device info and the endpoint id array are in NVS.
    device_persistent_info_t stored_info = {};
    size_t len = sizeof(stored_info);
    TEST_ASSERT_EQUAL(ESP_OK, read_stored_info(endpoint_id, &stored_info, &len));
    TEST_ASSERT_EQUAL(sizeof(stored_info), len);
    TEST_ASSERT_EQUAL(endpoint_id, stored_info.device_endpoint_id);
    TEST_ASSERT_EQUAL(s_aggregator_endpoint_id, stored_info.parent_endpoint_id);
    TEST_ASSERT_EQUAL_UINT32(ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID, stored_info.device_type_id);

    // Read the endpoint id array from NVS again and resume the device.
    node = setup_bridge();
    TEST_ASSERT_TRUE(endpoint_id_is_stored(endpoint_id));
    device_t *dev = esp_matter_bridge::resume_device(node, endpoint_id, nullptr);
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_EQUAL(endpoint_id, endpoint::get_id(dev->endpoint));
    TEST_ASSERT_EQUAL(endpoint_id, dev->persistent_info.device_endpoint_id);
    TEST_ASSERT_EQUAL_UINT32(created_info.device_type_id, dev->persistent_info.device_type_id);
    wait_for_matter_work();

    // Remove the device. The endpoint, the device info and the endpoint id are gone.
    TEST_ASSERT_EQUAL(ESP_OK, esp_matter_bridge::remove_device(dev));
    TEST_ASSERT_NULL(endpoint::get(node, endpoint_id));
    TEST_ASSERT_FALSE(endpoint_id_is_stored(endpoint_id));
    len = sizeof(stored_info);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, read_stored_info(endpoint_id, &stored_info, &len));

    // The endpoint id is also removed from the array in NVS.
    setup_bridge();
    TEST_ASSERT_FALSE(endpoint_id_is_stored(endpoint_id));
    wait_for_matter_work();
}

TEST_CASE("resume_device rejects stored info that is too short", "[bridge]")
{
    node_t *node = setup_bridge();
    device_persistent_info_t stored_info = {};
    uint16_t endpoint_id = create_stored_device(node, &stored_info);

    // Store only a part of the device info. The device type is then 0, and the device type callback rejects it.
    write_stored_info(endpoint_id, &stored_info, sizeof(stored_info) - sizeof(stored_info.device_type_id));

    TEST_ASSERT_NULL(esp_matter_bridge::resume_device(node, endpoint_id, nullptr));
    TEST_ASSERT_NULL(endpoint::get(node, endpoint_id));

    TEST_ASSERT_EQUAL(ESP_OK, esp_matter_bridge::erase_bridged_device_info(endpoint_id));
    wait_for_matter_work();
}

TEST_CASE("resume_device rejects stored info of a different endpoint", "[bridge]")
{
    node_t *node = setup_bridge();
    device_persistent_info_t stored_info = {};
    uint16_t endpoint_id = create_stored_device(node, &stored_info);

    // The stored info has the endpoint id of a different device.
    stored_info.device_endpoint_id = endpoint_id + 1;
    write_stored_info(endpoint_id, &stored_info, sizeof(stored_info));

    TEST_ASSERT_NULL(esp_matter_bridge::resume_device(node, endpoint_id, nullptr));
    TEST_ASSERT_NULL(endpoint::get(node, endpoint_id));

    TEST_ASSERT_EQUAL(ESP_OK, esp_matter_bridge::erase_bridged_device_info(endpoint_id));
    wait_for_matter_work();
}

TEST_CASE("erase_bridged_device_info succeeds when there is no stored info", "[bridge]")
{
    node_t *node = setup_bridge();
    uint16_t endpoint_id = test::reserve_destroyable_endpoint_id(node);

    device_persistent_info_t stored_info = {};
    size_t len = sizeof(stored_info);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, read_stored_info(endpoint_id, &stored_info, &len));

    TEST_ASSERT_EQUAL(ESP_OK, esp_matter_bridge::erase_bridged_device_info(endpoint_id));
    wait_for_matter_work();
}

#endif // MAX_BRIDGED_DEVICE_COUNT > 0
