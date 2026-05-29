/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Multi-stage tests (see ESP-IDF unit test guide):
 *   https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/unit-tests.html#multi-stage-test-cases
 *
 * On device: run sub-test (1), wait for reboot, run the same menu entry and sub-test (2).
 * In CI/QEMU: pytest calls run_all_single_board_cases() which runs both stages automatically.
 */

#include <string.h>
#include <unity.h>
#include <esp_system.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model.h>
#include <esp_matter_cluster.h>
#include <nvs_flash.h>

namespace esp_matter::attribute {
esp_err_t get_val_internal(attribute_t *attribute, esp_matter_attr_val_t *val);
esp_err_t store_val_in_nvs(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                           const esp_matter_attr_val_t &val);
esp_err_t erase_val_in_nvs(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id);
} // namespace esp_matter::attribute

using namespace esp_matter;

static constexpr uint32_t k_cluster_id = 0xFFF2;
static constexpr uint32_t k_attribute_id = 0x10001;
static constexpr uint16_t k_max_size = 32;

static esp_err_t init_nvs()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        TEST_ASSERT_EQUAL(ESP_OK, nvs_flash_erase());
        err = nvs_flash_init();
    }
    TEST_ASSERT_EQUAL(ESP_OK, err);
    return esp_matter_nvs_init();
}

static cluster_t *add_test_cluster(endpoint_t *endpoint)
{
    cluster_t *cluster = cluster::create(endpoint, k_cluster_id, CLUSTER_FLAG_SERVER);
    TEST_ASSERT_NOT_NULL(cluster);
    cluster::global::attribute::create_feature_map(cluster, 0);
    cluster::global::attribute::create_cluster_revision(cluster, 1);
    return cluster;
}

static void check_max_and_stored(attribute_t *attr)
{
    esp_matter_attr_val_t val;
    TEST_ASSERT_EQUAL(ESP_OK, attribute::get_val_internal(attr, &val));
    TEST_ASSERT_EQUAL(k_max_size, val.val.a.max);
    TEST_ASSERT_EQUAL_STRING("stored", reinterpret_cast<char *>(val.val.a.b));
}

static void attr_create_max_before_reboot()
{
    TEST_ASSERT_EQUAL(ESP_OK, init_nvs());

    if (node::get() != nullptr) {
        node::destroy_raw();
    }

    node::config_t node_config;
    node_t *node = node::create(&node_config, nullptr, nullptr);
    TEST_ASSERT_NOT_NULL(node);

    endpoint_t *ep = endpoint::create(node, ENDPOINT_FLAG_NONE, nullptr);
    TEST_ASSERT_NOT_NULL(ep);
    uint16_t ep_id = endpoint::get_id(ep);
    TEST_ASSERT_EQUAL(1, ep_id);

    cluster_t *cluster = add_test_cluster(ep);
    attribute::erase_val_in_nvs(ep_id, k_cluster_id, k_attribute_id);

    char stored[] = "stored";
    esp_matter_attr_val_t nvs_val = esp_matter_char_str(stored, strlen(stored));
    TEST_ASSERT_EQUAL(ESP_OK, attribute::store_val_in_nvs(ep_id, k_cluster_id, k_attribute_id, nvs_val));

    char empty[] = "";
    esp_matter_attr_val_t def = esp_matter_char_str(empty, 0);
    attribute_t *attr = attribute::create(cluster, k_attribute_id, ATTRIBUTE_FLAG_NONVOLATILE, def, k_max_size);
    TEST_ASSERT_NOT_NULL(attr);
    check_max_and_stored(attr);

    esp_restart();
}

static void attr_create_max_after_reboot()
{
    TEST_ASSERT_EQUAL(ESP_RST_SW, esp_reset_reason());
    TEST_ASSERT_EQUAL(ESP_OK, init_nvs());

    if (node::get() != nullptr) {
        node::destroy_raw();
    }

    node::config_t node_config;
    node_t *node = node::create(&node_config, nullptr, nullptr);
    TEST_ASSERT_NOT_NULL(node);

    endpoint_t *ep = endpoint::create(node, ENDPOINT_FLAG_NONE, nullptr);
    TEST_ASSERT_NOT_NULL(ep);
    TEST_ASSERT_EQUAL(1, endpoint::get_id(ep));

    cluster_t *cluster = add_test_cluster(ep);

    char empty[] = "";
    esp_matter_attr_val_t def = esp_matter_char_str(empty, 0);
    attribute_t *attr = attribute::create(cluster, k_attribute_id, ATTRIBUTE_FLAG_NONVOLATILE, def, k_max_size);
    TEST_ASSERT_NOT_NULL(attr);
    check_max_and_stored(attr);

    attribute::erase_val_in_nvs(1, k_cluster_id, k_attribute_id);
    cluster::destroy(cluster);
}

TEST_CASE_MULTIPLE_STAGES("attribute::create preserves max after reboot",
                          "[attribute][create][reset=SW_CPU_RESET]",
                          attr_create_max_before_reboot, attr_create_max_after_reboot);
