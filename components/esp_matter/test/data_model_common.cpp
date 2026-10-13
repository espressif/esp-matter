/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unity.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model.h>
#include <esp_matter_cluster.h>
#include <esp_matter_data_model_priv.h>

#include "cluster_lifecycle_common.h"

#include <app-common/zap-generated/ids/Clusters.h>

using namespace esp_matter;

static constexpr uint32_t k_feat_a = (1 << 0);
static constexpr uint32_t k_feat_b = (1 << 1);
static constexpr uint32_t k_feat_c = (1 << 2);

/* -------------------------------------------------------------------------- */
/* validate_features (O.a "exactly one" / O.a+ "at least one")                 */
/* -------------------------------------------------------------------------- */

TEST_CASE("validate_features exactly-one accepts a single enabled feature", "[data_model_common][validate_features]")
{
    TEST_ASSERT_TRUE(validate_features(k_feat_a, feature_policy::k_exact_one, "a|b", {k_feat_a, k_feat_b}));
    TEST_ASSERT_TRUE(validate_features(k_feat_b, feature_policy::k_exact_one, "a|b", {k_feat_a, k_feat_b}));
}

TEST_CASE("validate_features exactly-one rejects zero or more than one", "[data_model_common][validate_features]")
{
    TEST_ASSERT_FALSE(validate_features(0, feature_policy::k_exact_one, "a|b", {k_feat_a, k_feat_b}));
    TEST_ASSERT_FALSE(validate_features(k_feat_a | k_feat_b, feature_policy::k_exact_one, "a|b", {k_feat_a, k_feat_b}));
}

TEST_CASE("validate_features at-least-one accepts one or more", "[data_model_common][validate_features]")
{
    TEST_ASSERT_TRUE(validate_features(k_feat_a, feature_policy::k_at_least_one, "a|b", {k_feat_a, k_feat_b}));
    TEST_ASSERT_TRUE(validate_features(k_feat_a | k_feat_b, feature_policy::k_at_least_one, "a|b", {k_feat_a, k_feat_b}));
}

TEST_CASE("validate_features at-least-one rejects zero", "[data_model_common][validate_features]")
{
    TEST_ASSERT_FALSE(validate_features(0, feature_policy::k_at_least_one, "a|b", {k_feat_a, k_feat_b}));
}

TEST_CASE("validate_features only counts the features in the checked set", "[data_model_common][validate_features]")
{
    /* k_feat_c is enabled but not part of the {a, b} set being validated. */
    TEST_ASSERT_FALSE(validate_features(k_feat_c, feature_policy::k_exact_one, "a|b", {k_feat_a, k_feat_b}));
    TEST_ASSERT_FALSE(validate_features(k_feat_c, feature_policy::k_at_least_one, "a|b", {k_feat_a, k_feat_b}));
    /* a is set (counts), c is set (ignored) -> exactly one match. */
    TEST_ASSERT_TRUE(validate_features(k_feat_a | k_feat_c, feature_policy::k_exact_one, "a|b", {k_feat_a, k_feat_b}));
}

/* -------------------------------------------------------------------------- */
/* update_feature_map / get_feature_map_value                                 */
/* -------------------------------------------------------------------------- */

static cluster_t *create_cluster_with_feature_map(node_t *node, endpoint_t **out_endpoint)
{
    endpoint_t *endpoint = endpoint::create(node, ENDPOINT_FLAG_DESTROYABLE, nullptr);
    TEST_ASSERT_NOT_NULL(endpoint);
    cluster_t *cluster = cluster::create(endpoint, chip::app::Clusters::OnOff::Id, CLUSTER_FLAG_SERVER);
    TEST_ASSERT_NOT_NULL(cluster);
    TEST_ASSERT_NOT_NULL(cluster::global::attribute::create_feature_map(cluster, 0));
    *out_endpoint = endpoint;
    return cluster;
}

TEST_CASE("update_feature_map ORs bits in and get_feature_map_value reads them back",
          "[data_model_common][feature_map]")
{
    /* These helpers operate on the esp-matter attribute store during data-model construction, so no
       started Matter stack is required (and starting it pulls in networking that aborts off-QEMU). */
    node_t *node = test::get_or_create_node();
    endpoint_t *endpoint = nullptr;
    cluster_t *cluster = create_cluster_with_feature_map(node, &endpoint);

    TEST_ASSERT_EQUAL_UINT32(0, cluster::get_feature_map_value(cluster));

    TEST_ASSERT_EQUAL(ESP_OK, cluster::update_feature_map(cluster, k_feat_a));
    TEST_ASSERT_EQUAL_UINT32(k_feat_a, cluster::get_feature_map_value(cluster));

    /* A second update must accumulate (OR), not overwrite. */
    TEST_ASSERT_EQUAL(ESP_OK, cluster::update_feature_map(cluster, k_feat_b));
    TEST_ASSERT_EQUAL_UINT32(k_feat_a | k_feat_b, cluster::get_feature_map_value(cluster));

    TEST_ASSERT_EQUAL(ESP_OK, endpoint::destroy(node, endpoint));
}

TEST_CASE("get_feature_map_value returns 0 without a FeatureMap attribute or cluster",
          "[data_model_common][feature_map]")
{
    node_t *node = test::get_or_create_node();
    endpoint_t *endpoint = endpoint::create(node, ENDPOINT_FLAG_DESTROYABLE, nullptr);
    TEST_ASSERT_NOT_NULL(endpoint);
    cluster_t *cluster = cluster::create(endpoint, chip::app::Clusters::OnOff::Id, CLUSTER_FLAG_SERVER);
    TEST_ASSERT_NOT_NULL(cluster);

    TEST_ASSERT_EQUAL_UINT32(0, cluster::get_feature_map_value(cluster));
    TEST_ASSERT_EQUAL_UINT32(0, cluster::get_feature_map_value(nullptr));

    TEST_ASSERT_EQUAL(ESP_OK, endpoint::destroy(node, endpoint));
}

TEST_CASE("update_feature_map rejects a null cluster", "[data_model_common][feature_map]")
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, cluster::update_feature_map(nullptr, k_feat_a));
}

static esp_err_t reject_pre_update_cb(attribute::callback_type_t type, uint16_t, uint32_t, uint32_t,
                                      esp_matter_attr_val_t *, void *)
{
    return (type == attribute::PRE_UPDATE) ? ESP_FAIL : ESP_OK;
}

TEST_CASE("update_feature_map bypasses the application PRE_UPDATE callback",
          "[data_model_common][feature_map]")
{
    node_t *node = test::get_or_create_node();
    endpoint_t *endpoint = nullptr;
    cluster_t *cluster = create_cluster_with_feature_map(node, &endpoint);

    /* Simulate a strict application callback that rejects every PRE_UPDATE. The feature map is written
       during data-model construction and must not depend on the app callback, so update_feature_map has
       to succeed and set the bit even though the callback would reject it. If update_feature_map is ever
       changed to attribute::set_val() (call_callbacks = true), it returns an error here and the bit is
       never set. */
    attribute::set_callback(reject_pre_update_cb);
    esp_err_t err = cluster::update_feature_map(cluster, k_feat_a);
    attribute::set_callback(nullptr); /* restore before any assert that could longjmp out */

    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL_UINT32(k_feat_a, cluster::get_feature_map_value(cluster));

    TEST_ASSERT_EQUAL(ESP_OK, endpoint::destroy(node, endpoint));
}
