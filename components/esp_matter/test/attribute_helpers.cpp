/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <unity.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model.h>
#include <esp_matter_attribute_utils.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <clusters/shared/GlobalIds.h>

#include "cluster_lifecycle_common.h"

using namespace esp_matter;

// A custom cluster exercising the shared integration helpers.
static constexpr uint32_t k_test_cluster_id = 0xFFF1FC30;
// A second cluster deliberately created WITHOUT a FeatureMap attribute, so the "absent -> 0"
// path can be checked without mutating shared state (keeps the test idempotent on re-run).
static constexpr uint32_t k_no_fmap_cluster_id = 0xFFF1FC31;

static constexpr uint32_t k_stored_attr_id = 0x0001;
static constexpr uint32_t k_managed_attr_id = 0x0002;
static constexpr uint32_t k_missing_attr_id = 0x00F0;
static constexpr uint16_t k_stored_attr_value = 0xABCD;
static constexpr uint32_t k_feature_map_value = 0x0000000B;

static constexpr uint32_t k_accepted_cmd_id = 0x0010;
static constexpr uint32_t k_generated_cmd_id = 0x0011;
static constexpr uint32_t k_missing_cmd_id = 0x00F1;

static uint16_t s_endpoint_id = 0;

static esp_err_t dummy_command_callback(const chip::app::ConcreteCommandPath &command_path,
                                        chip::TLV::TLVReader &tlv_data, void *opaque_ptr)
{
    return ESP_OK;
}

static cluster_t *setup_helpers_test_cluster()
{
    static bool done = false;
    node_t *node = test::get_or_create_node();
    TEST_ASSERT_NOT_NULL(node);

    if (!done) {
        endpoint_t *endpoint = endpoint::create(node, ENDPOINT_FLAG_NONE, nullptr);
        TEST_ASSERT_NOT_NULL(endpoint);
        s_endpoint_id = endpoint::get_id(endpoint);

        cluster_t *cluster = cluster::create(endpoint, k_test_cluster_id, CLUSTER_FLAG_SERVER);
        TEST_ASSERT_NOT_NULL(cluster);

        TEST_ASSERT_NOT_NULL(
            attribute::create(cluster, k_stored_attr_id, ATTRIBUTE_FLAG_NONE, esp_matter_uint16(k_stored_attr_value)));
        TEST_ASSERT_NOT_NULL(
            attribute::create(cluster, k_managed_attr_id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_uint16(0)));

        TEST_ASSERT_NOT_NULL(
            command::create(cluster, k_accepted_cmd_id, COMMAND_FLAG_ACCEPTED, dummy_command_callback));
        TEST_ASSERT_NOT_NULL(
            command::create(cluster, k_generated_cmd_id, COMMAND_FLAG_GENERATED, nullptr));

        TEST_ASSERT_NOT_NULL(attribute::create(cluster, chip::app::Clusters::Globals::Attributes::FeatureMap::Id,
                                               ATTRIBUTE_FLAG_NONE, esp_matter_bitmap32(k_feature_map_value)));

        // A sibling cluster with no FeatureMap attribute, for the absent -> 0 case.
        cluster_t *no_fmap_cluster = cluster::create(endpoint, k_no_fmap_cluster_id, CLUSTER_FLAG_SERVER);
        TEST_ASSERT_NOT_NULL(no_fmap_cluster);

        done = true;
    }

    cluster_t *cluster = cluster::get(s_endpoint_id, k_test_cluster_id);
    TEST_ASSERT_NOT_NULL(cluster);
    return cluster;
}

TEST_CASE("is_attribute_enabled reports presence for stored and managed attributes", "[attribute_helpers]")
{
    cluster_t *cluster = setup_helpers_test_cluster();

    TEST_ASSERT_TRUE(is_attribute_enabled(cluster, k_stored_attr_id));
    // Managed-internally attributes have no stored value but still exist on the cluster.
    TEST_ASSERT_TRUE(is_attribute_enabled(cluster, k_managed_attr_id));
    TEST_ASSERT_FALSE(is_attribute_enabled(cluster, k_missing_attr_id));
    TEST_ASSERT_FALSE(is_attribute_enabled(nullptr, k_stored_attr_id));
}

TEST_CASE("is_command_enabled honors command flags", "[attribute_helpers]")
{
    cluster_t *cluster = setup_helpers_test_cluster();

    // Default flag is COMMAND_FLAG_ACCEPTED.
    TEST_ASSERT_TRUE(is_command_enabled(cluster, k_accepted_cmd_id));
    TEST_ASSERT_FALSE(is_command_enabled(cluster, k_generated_cmd_id));
    TEST_ASSERT_TRUE(is_command_enabled(cluster, k_generated_cmd_id, COMMAND_FLAG_GENERATED));
    TEST_ASSERT_FALSE(is_command_enabled(cluster, k_missing_cmd_id));
    TEST_ASSERT_FALSE(is_command_enabled(nullptr, k_accepted_cmd_id));
}

TEST_CASE("get_stored_attr_val reads stored values and rejects managed or missing attributes", "[attribute_helpers]")
{
    cluster_t *cluster = setup_helpers_test_cluster();

    esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
    TEST_ASSERT_EQUAL(ESP_OK, get_stored_attr_val(cluster, k_stored_attr_id, val));
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT16, val.type);
    TEST_ASSERT_EQUAL_UINT16(k_stored_attr_value, val.val.u16);

    // Managed-internally attributes carry no stored value.
    TEST_ASSERT_NOT_EQUAL(ESP_OK, get_stored_attr_val(cluster, k_managed_attr_id, val));
    // Missing attribute.
    TEST_ASSERT_NOT_EQUAL(ESP_OK, get_stored_attr_val(cluster, k_missing_attr_id, val));
}

TEST_CASE("read_feature_map_u32 returns the stored feature map and 0 when absent", "[attribute_helpers]")
{
    cluster_t *cluster = setup_helpers_test_cluster();

    // The test cluster has a FeatureMap attribute (created in setup); both the endpoint-id and
    // cluster-handle overloads must return it.
    TEST_ASSERT_EQUAL_UINT32(k_feature_map_value, read_feature_map_u32(s_endpoint_id, k_test_cluster_id));
    TEST_ASSERT_EQUAL_UINT32(k_feature_map_value, read_feature_map_u32(cluster));

    // The sibling cluster has no FeatureMap attribute -> both overloads fall back to 0.
    cluster_t *no_fmap_cluster = cluster::get(s_endpoint_id, k_no_fmap_cluster_id);
    TEST_ASSERT_NOT_NULL(no_fmap_cluster);
    TEST_ASSERT_EQUAL_UINT32(0, read_feature_map_u32(s_endpoint_id, k_no_fmap_cluster_id));
    TEST_ASSERT_EQUAL_UINT32(0, read_feature_map_u32(no_fmap_cluster));
}
