/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unity.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_macros.h>
#include <esp_matter_data_model.h>

#include "cluster_lifecycle_common.h"

#include <app-common/zap-generated/ids/Clusters.h>

using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace chip::app::Clusters;
using esp_matter::endpoint::validation::validate_optional_clusters;

static constexpr uint32_t k_mask_a = (1 << 0);
static constexpr uint32_t k_mask_b = (1 << 1);

TEST_CASE("validate_optional_clusters rejects empty mask", "[optional_clusters][validation]")
{
    TEST_ASSERT_FALSE(validate_optional_clusters(0, "test_device", {k_mask_a, k_mask_b}));
}

TEST_CASE("validate_optional_clusters accepts each configured mask bit", "[optional_clusters][validation]")
{
    TEST_ASSERT_TRUE(validate_optional_clusters(k_mask_a, "test_device", {k_mask_a, k_mask_b}));
    TEST_ASSERT_TRUE(validate_optional_clusters(k_mask_b, "test_device", {k_mask_a, k_mask_b}));
    TEST_ASSERT_TRUE(validate_optional_clusters(k_mask_a | k_mask_b, "test_device", {k_mask_a, k_mask_b}));
}

TEST_CASE("validate_optional_clusters rejects unrelated mask bits", "[optional_clusters][validation]")
{
    TEST_ASSERT_FALSE(validate_optional_clusters(1 << 2, "test_device", {k_mask_a, k_mask_b}));
}

TEST_CASE("electrical_sensor create fails when no O.a+ cluster is selected", "[optional_clusters][electrical_sensor]")
{
    node_t *node = test::get_or_create_node();

    endpoint::electrical_sensor::config_t config;
    config.power_topology.feature_flags = power_topology::feature::node_topology::get_id();
    TEST_ASSERT_EQUAL(0u, config.optional_clusters_mask);

    endpoint_t *endpoint =
        endpoint::electrical_sensor::create(node, &config, ENDPOINT_FLAG_DESTROYABLE, nullptr);
    TEST_ASSERT_NULL(endpoint);
}

TEST_CASE("electrical_sensor create adds only selected O.a+ clusters", "[optional_clusters][electrical_sensor]")
{
    node_t *node = test::get_or_create_node();
    test::start_matter_if_needed();

    endpoint::electrical_sensor::config_t config;
    config.power_topology.feature_flags = power_topology::feature::node_topology::get_id();
    config.with_electrical_power_measurement().feature_flags =
              electrical_power_measurement::feature::alternating_current::get_id();

    endpoint_t *endpoint =
        endpoint::electrical_sensor::create(node, &config, ENDPOINT_FLAG_DESTROYABLE, nullptr);
    TEST_ASSERT_NOT_NULL(endpoint);
    TEST_ASSERT_NOT_NULL(cluster::get(endpoint, ElectricalPowerMeasurement::Id));
    TEST_ASSERT_NULL(cluster::get(endpoint, ElectricalEnergyMeasurement::Id));

    TEST_ASSERT_EQUAL(ESP_OK, endpoint::destroy(node, endpoint));
}

TEST_CASE("cook_surface create fails when no O.a+ cluster is selected", "[optional_clusters][cook_surface]")
{
    node_t *node = test::get_or_create_node();

    endpoint::cook_surface::config_t config;
    config.temperature_control.feature_flags = temperature_control::feature::temperature_number::get_id();
    endpoint_t *endpoint = endpoint::cook_surface::create(node, &config, ENDPOINT_FLAG_DESTROYABLE, nullptr);
    TEST_ASSERT_NULL(endpoint);
}

TEST_CASE("cook_surface create adds only selected O.a+ clusters", "[optional_clusters][cook_surface]")
{
    node_t *node = test::get_or_create_node();
    test::start_matter_if_needed();

    endpoint::cook_surface::config_t config;
    config.temperature_control.feature_flags = temperature_control::feature::temperature_number::get_id();
    config.with_temperature_measurement();

    endpoint_t *endpoint = endpoint::cook_surface::create(node, &config, ENDPOINT_FLAG_DESTROYABLE, nullptr);
    TEST_ASSERT_NOT_NULL(endpoint);
    TEST_ASSERT_NULL(cluster::get(endpoint, TemperatureControl::Id));
    TEST_ASSERT_NOT_NULL(cluster::get(endpoint, TemperatureMeasurement::Id));

    TEST_ASSERT_EQUAL(ESP_OK, endpoint::destroy(node, endpoint));
}

TEST_CASE("electrical_energy_tariff create fails when no O.a+ cluster is selected",
          "[optional_clusters][electrical_energy_tariff]")
{
    node_t *node = test::get_or_create_node();

    endpoint::electrical_energy_tariff::config_t config;
    endpoint_t *endpoint =
        endpoint::electrical_energy_tariff::create(node, &config, ENDPOINT_FLAG_DESTROYABLE, nullptr);
    TEST_ASSERT_NULL(endpoint);
}

TEST_CASE("electrical_energy_tariff create adds only selected O.a+ clusters",
          "[optional_clusters][electrical_energy_tariff]")
{
    node_t *node = test::get_or_create_node();
    test::start_matter_if_needed();

    endpoint::electrical_energy_tariff::config_t config;
    config.with_commodity_tariff().feature_flags = commodity_tariff::feature::pricing::get_id();

    endpoint_t *endpoint =
        endpoint::electrical_energy_tariff::create(node, &config, ENDPOINT_FLAG_DESTROYABLE, nullptr);
    TEST_ASSERT_NOT_NULL(endpoint);
    TEST_ASSERT_NULL(cluster::get(endpoint, CommodityPrice::Id));
    TEST_ASSERT_NOT_NULL(cluster::get(endpoint, CommodityTariff::Id));

    TEST_ASSERT_EQUAL(ESP_OK, endpoint::destroy(node, endpoint));
}
