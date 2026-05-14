/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <unity.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <nvs_flash.h>

#include "common.h"

#include <platform/DeviceInfoProvider.h>

namespace esp_matter::test {

class TestDeviceInfoProvider : public chip::DeviceLayer::DeviceInfoProvider {
public:
    FixedLabelIterator * IterateFixedLabel(chip::EndpointId) override
    {
        return nullptr;
    }
    UserLabelIterator * IterateUserLabel(chip::EndpointId) override
    {
        return nullptr;
    }
    SupportedLocalesIterator * IterateSupportedLocales() override
    {
        return nullptr;
    }
    SupportedCalendarTypesIterator * IterateSupportedCalendarTypes() override
    {
        return nullptr;
    }

protected:
    CHIP_ERROR SetUserLabelAt(chip::EndpointId, size_t, const UserLabelType &) override
    {
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR DeleteUserLabelAt(chip::EndpointId, size_t) override
    {
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR SetUserLabelLength(chip::EndpointId, size_t) override
    {
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR GetUserLabelLength(chip::EndpointId, size_t &val) override
    {
        val = 0;
        return CHIP_NO_ERROR;
    }
};

inline TestDeviceInfoProvider &get_test_device_info_provider()
{
    static TestDeviceInfoProvider provider;
    return provider;
}

inline node_t *get_or_create_node()
{
    node_t *node = node::get();
    if (node != nullptr) {
        return node;
    }

    esp_err_t err = nvs_flash_init();
    TEST_ASSERT_EQUAL(ESP_OK, err);

    node::config_t node_config;
    node = node::create(&node_config, nullptr, nullptr);
    TEST_ASSERT_NOT_NULL(node);
    return node;
}

inline void start_matter_if_needed()
{
    if (!esp_matter::is_started()) {
        suppress_matter_logs();
        chip::DeviceLayer::SetDeviceInfoProvider(&get_test_device_info_provider());
        esp_err_t err = esp_matter::start(nullptr);
        TEST_ASSERT_EQUAL(ESP_OK, err);
    }
}

inline uint16_t reserve_destroyable_endpoint_id(node_t *node)
{
    endpoint_t *endpoint = endpoint::create(node, ENDPOINT_FLAG_DESTROYABLE, nullptr);
    TEST_ASSERT_NOT_NULL(endpoint);
    uint16_t endpoint_id = endpoint::get_id(endpoint);

    esp_err_t err = endpoint::destroy(node, endpoint);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NULL(endpoint::get(node, endpoint_id));
    return endpoint_id;
}

} // namespace esp_matter::test
