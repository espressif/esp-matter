/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unity.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model.h>
#include <nvs_flash.h>

using namespace esp_matter;

// Test data model
static node_t *test_node = nullptr;
static endpoint_t *test_endpoint = nullptr;
static cluster_t *test_cluster = nullptr;
static uint16_t test_endpoint_id = 1;
static uint32_t test_cluster_id = 0xFFF1; // Custom test cluster

static bool node_created = false;

void setup_for_get_val_type()
{
    if (!node_created) {
        esp_err_t err = nvs_flash_init();
        TEST_ASSERT_EQUAL(ESP_OK, err);

        node::config_t node_config;
        test_node = node::create(&node_config, nullptr, nullptr);
        TEST_ASSERT_NOT_NULL(test_node);

        test_endpoint = endpoint::create(test_node, ENDPOINT_FLAG_NONE, nullptr);
        TEST_ASSERT_NOT_NULL(test_endpoint);
        test_endpoint_id = endpoint::get_id(test_endpoint);

        node_created = true;
    }

    // Fresh cluster per test case
    test_cluster = cluster::create(test_endpoint, test_cluster_id, CLUSTER_FLAG_SERVER);
    TEST_ASSERT_NOT_NULL(test_cluster);
}

void teardown_for_get_val_type()
{
    // Only destroy the cluster, not the endpoint — endpoint is reused across test cases
    // since node/endpoint creation involves starting the Matter stack which cannot be cleanly restarted.
    if (test_cluster) {
        cluster::destroy(test_cluster);
        test_cluster = nullptr;
    }
}

TEST_CASE("invalid inputs to get_val_type", "[get_val_type][invalid]")
{
    setup_for_get_val_type();

    esp_matter_val_type_t type = attribute::get_val_type((attribute_t *)nullptr);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INVALID, type);

    type = attribute::get_val_type(0xFFFF, 0x0006, 0x0000);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INVALID, type);

    type = attribute::get_val_type(1, 0xFFFFFFFF, 0x0000);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INVALID, type);

    type = attribute::get_val_type(1, 0x0006, 0xFFFFFFFF);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INVALID, type);

    teardown_for_get_val_type();
}

// Test all primitive types from attribute_utils.h
TEST_CASE("get_val_type for all primitive types", "[get_val_type][all_types]")
{
    setup_for_get_val_type();

    uint32_t attr_id = 1; // Start attribute IDs from 1

    // Test 1: Boolean
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(true);
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BOOLEAN, type);

        // Test with IDs
        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BOOLEAN, type);
    }

    // Test 2: Float
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<float>(3.14f));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_FLOAT, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_FLOAT, type);
    }

    // Test 3: INT8
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<int8_t>(-128));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INT8, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INT8, type);
    }

    // Test 4: UINT8
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<uint8_t>(255));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT8, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT8, type);
    }

    // Test 5: INT16
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<int16_t>(-32768));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INT16, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INT16, type);
    }

    // Test 6: UINT16
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<uint16_t>(65535));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT16, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT16, type);
    }

    // Test 7: INT32
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<int32_t>(-2147483648));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INT32, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INT32, type);
    }

    // Test 8: UINT32
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<uint32_t>(0xFFFFFFFF));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT32, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT32, type);
    }

    // Test 9: INT64
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<int64_t>(-9223372036854775807LL));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INT64, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INT64, type);
    }

    // Test 10: UINT64
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFFULL));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT64, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT64, type);
    }

    // Test 11: ENUM8
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<uint8_t>(5), esp_matter_attr_val::uint_sub_type::k_enum);
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_ENUM8, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_ENUM8, type);
    }

    // Test 12: ENUM16
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<uint16_t>(1000), esp_matter_attr_val::uint_sub_type::k_enum);
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_ENUM16, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_ENUM16, type);
    }

    // Test 13: BITMAP8
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<uint8_t>(0xAB), esp_matter_attr_val::uint_sub_type::k_bitmap);
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BITMAP8, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BITMAP8, type);
    }

    // Test 14: BITMAP16
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<uint16_t>(0xABCD), esp_matter_attr_val::uint_sub_type::k_bitmap);
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BITMAP16, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BITMAP16, type);
    }

    // Test 15: BITMAP32
    {
        esp_matter_attr_val_t val = esp_matter_attr_val(static_cast<uint32_t>(0xABCD1234), esp_matter_attr_val::uint_sub_type::k_bitmap);
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BITMAP32, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BITMAP32, type);
    }

    // Test 16: CHAR_STRING
    {
        char test_str[] = "TestString";
        esp_matter_attr_val_t val = esp_matter_attr_val(test_str, strlen(test_str));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_CHAR_STRING, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_CHAR_STRING, type);
    }

    // Test 17: OCTET_STRING
    {
        uint8_t test_octets[] = {0x01, 0x02, 0x03, 0x04};
        esp_matter_attr_val_t val = esp_matter_attr_val(test_octets, sizeof(test_octets));
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_OCTET_STRING, type);

        type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id - 1);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_OCTET_STRING, type);
    }

    // Test 18: ARRAY
    {
        // apparently our internal impl wants this to be malloced,
        // otherwise at the time of destruction, we will try to free a stack allocated buffer
        uint8_t *test_array = (uint8_t *)malloc(3 * sizeof(uint8_t));
        TEST_ASSERT_NOT_NULL(test_array);
        test_array[0] = 1; test_array[1] = 2; test_array[2] = 3;

        esp_matter_attr_val_t val = esp_matter_attr_val(test_array, (uint16_t)3 * sizeof(uint8_t), (uint16_t)3);
        attribute_t *attr = attribute::create(test_cluster, attr_id++, ATTRIBUTE_FLAG_NONE, val);
        TEST_ASSERT_NOT_NULL(attr);

        esp_matter_val_type_t type = attribute::get_val_type(attr);
        TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_ARRAY, type);

        // no need to free test_array here, it will be freed when the attribute is destroyed
    }

    teardown_for_get_val_type();
}

// Test that type persists after value updates
TEST_CASE("get_val_type persists after attribute update", "[get_val_type][persistence]")
{
    setup_for_get_val_type();

    uint32_t attr_id = 100;
    // Create a UINT8 attribute
    esp_matter_attr_val_t initial_val = esp_matter_attr_val(static_cast<uint8_t>(10));
    attribute_t *attr = attribute::create(test_cluster, attr_id, ATTRIBUTE_FLAG_NONE, initial_val);
    TEST_ASSERT_NOT_NULL(attr);

    // Verify initial type
    esp_matter_val_type_t type = attribute::get_val_type(attr);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT8, type);

    // Update value
    esp_matter_attr_val_t new_val = esp_matter_attr_val(static_cast<uint8_t>(200));
    esp_err_t err = attribute::set_val(attr, &new_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // Type should remain the same
    type = attribute::get_val_type(attr);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT8, type);

    // Verify with IDs as well
    type = attribute::get_val_type(test_endpoint_id, test_cluster_id, attr_id);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT8, type);

    teardown_for_get_val_type();
}

// Test multiple attributes in same cluster
TEST_CASE("get_val_type for multiple attributes in same cluster", "[get_val_type][multiple]")
{
    setup_for_get_val_type();

    // Create attributes with different types
    esp_matter_attr_val_t bool_val = esp_matter_attr_val(false);
    attribute_t *bool_attr = attribute::create(test_cluster, 1, ATTRIBUTE_FLAG_NONE, bool_val);
    TEST_ASSERT_NOT_NULL(bool_attr);

    esp_matter_attr_val_t uint8_val = esp_matter_attr_val(static_cast<uint8_t>(42));
    attribute_t *uint8_attr = attribute::create(test_cluster, 2, ATTRIBUTE_FLAG_NONE, uint8_val);
    TEST_ASSERT_NOT_NULL(uint8_attr);

    esp_matter_attr_val_t uint16_val = esp_matter_attr_val(static_cast<uint16_t>(1234));
    attribute_t *uint16_attr = attribute::create(test_cluster, 3, ATTRIBUTE_FLAG_NONE, uint16_val);
    TEST_ASSERT_NOT_NULL(uint16_attr);

    esp_matter_attr_val_t int32_val = esp_matter_attr_val(static_cast<int32_t>(-9999));
    attribute_t *int32_attr = attribute::create(test_cluster, 4, ATTRIBUTE_FLAG_NONE, int32_val);
    TEST_ASSERT_NOT_NULL(int32_attr);

    // Verify all types via attribute handle
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BOOLEAN, attribute::get_val_type(bool_attr));
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT8, attribute::get_val_type(uint8_attr));
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT16, attribute::get_val_type(uint16_attr));
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INT32, attribute::get_val_type(int32_attr));

    // Verify all types via IDs
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BOOLEAN,
                      attribute::get_val_type(test_endpoint_id, test_cluster_id, 1));
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT8,
                      attribute::get_val_type(test_endpoint_id, test_cluster_id, 2));
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT16,
                      attribute::get_val_type(test_endpoint_id, test_cluster_id, 3));
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_INT32,
                      attribute::get_val_type(test_endpoint_id, test_cluster_id, 4));

    teardown_for_get_val_type();
}
