/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <unity.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <nvs_flash.h>

#include "common.h"

using namespace esp_matter;
using namespace chip::app::Clusters;

static node_t *test_node = nullptr;
static endpoint_t *test_endpoint = nullptr;
static uint16_t test_endpoint_id = 0;

struct callback_record_t {
    bool called;
    attribute::callback_type_t type;
    uint16_t endpoint_id;
    uint32_t cluster_id;
    uint32_t attribute_id;
    esp_matter_attr_val_t val;
    void reset()
    {
        called = false;
        type = esp_matter::attribute::PRE_UPDATE;
        endpoint_id = 0;
        cluster_id = 0;
        attribute_id = 0;
        val = esp_matter_attr_val();
    }

};

static callback_record_t cb_pre_update;
static callback_record_t cb_post_update;

static void reset_callback_records()
{
    cb_pre_update.reset();
    cb_post_update.reset();
}

static esp_err_t test_attribute_callback(attribute::callback_type_t type, uint16_t endpoint_id,
                                         uint32_t cluster_id, uint32_t attribute_id,
                                         esp_matter_attr_val_t *val, void *priv_data)
{
    callback_record_t *record = nullptr;
    if (type == attribute::PRE_UPDATE) {
        record = &cb_pre_update;
    } else if (type == attribute::POST_UPDATE) {
        record = &cb_post_update;
    } else {
        return ESP_OK;
    }
    record->called = true;
    record->type = type;
    record->endpoint_id = endpoint_id;
    record->cluster_id = cluster_id;
    record->attribute_id = attribute_id;
    record->val = *val;
    return ESP_OK;
}

void setup_for_update_report()
{
    static bool setup_done = false;
    if (setup_done) {
        return;
    }

    esp_err_t err = nvs_flash_init();
    TEST_ASSERT_EQUAL(ESP_OK, err);

    node::config_t node_config;
    test_node = node::create(&node_config, test_attribute_callback, nullptr);
    TEST_ASSERT_NOT_NULL(test_node);

    endpoint::extended_color_light::config_t light_config;
    light_config.on_off.on_off = false;
    light_config.level_control.current_level = nullable<uint8_t>(100);
    test_endpoint = endpoint::extended_color_light::create(test_node, &light_config, ENDPOINT_FLAG_NONE, nullptr);
    TEST_ASSERT_NOT_NULL(test_endpoint);

    test_endpoint_id = endpoint::get_id(test_endpoint);

    test::suppress_matter_logs();
    err = esp_matter::start(nullptr);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    setup_done = true;
}

void teardown_for_update_report()
{
    reset_callback_records();
}

// ============================================================
// attribute::report() tests
// ============================================================

TEST_CASE("report returns ESP_ERR_INVALID_ARG for null val", "[report][invalid]")
{
    setup_for_update_report();

    esp_err_t err = attribute::report(test_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, nullptr);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    teardown_for_update_report();
}

TEST_CASE("report returns ESP_ERR_INVALID_ARG for invalid endpoint", "[report][invalid]")
{
    setup_for_update_report();

    esp_matter_attr_val_t val = esp_matter_attr_val(true);
    esp_err_t err = attribute::report(chip::kInvalidEndpointId, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    teardown_for_update_report();
}

TEST_CASE("report returns ESP_ERR_INVALID_ARG for invalid cluster", "[report][invalid]")
{
    setup_for_update_report();

    esp_matter_attr_val_t val = esp_matter_attr_val(true);
    esp_err_t err = attribute::report(test_endpoint_id, chip::kInvalidClusterId, OnOff::Attributes::OnOff::Id, &val);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    teardown_for_update_report();
}

TEST_CASE("report returns ESP_ERR_INVALID_ARG for invalid attribute", "[report][invalid]")
{
    setup_for_update_report();

    esp_matter_attr_val_t val = esp_matter_attr_val(true);
    esp_err_t err = attribute::report(test_endpoint_id, OnOff::Id, chip::kInvalidAttributeId, &val);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    teardown_for_update_report();
}

TEST_CASE("report bool updates stored value", "[report][bool]")
{
    setup_for_update_report();

    esp_matter_attr_val_t val = esp_matter_attr_val(true);
    esp_err_t err = attribute::report(test_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_matter_attr_val_t retrieved;
    err = attribute::get_val(test_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &retrieved);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BOOLEAN, retrieved.type);
    TEST_ASSERT_EQUAL(true, retrieved.val.b);

    val = esp_matter_attr_val(false);
    err = attribute::report(test_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    err = attribute::get_val(test_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &retrieved);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(false, retrieved.val.b);

    teardown_for_update_report();
}

TEST_CASE("report nullable uint8 updates stored value", "[report][nullable][uint8]")
{
    setup_for_update_report();

    esp_matter_attr_val_t val = esp_matter_attr_val(nullable<uint8_t>(150));
    esp_err_t err = attribute::report(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_matter_attr_val_t retrieved;
    err = attribute::get_val(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &retrieved);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_NULLABLE_UINT8, retrieved.type);
    nullable<uint8_t> data(retrieved.val.u8);
    TEST_ASSERT_FALSE(data.is_null());
    TEST_ASSERT_EQUAL(150, data.value());

    teardown_for_update_report();
}

TEST_CASE("report same value returns ESP_OK", "[report][no_change]")
{
    setup_for_update_report();

    esp_matter_attr_val_t val = esp_matter_attr_val(false);
    esp_err_t err = attribute::report(test_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    err = attribute::report(test_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    teardown_for_update_report();
}

// ============================================================
// report() must NOT invoke attribute callbacks
// ============================================================

TEST_CASE("report does not call attribute callbacks", "[report][callback]")
{
    setup_for_update_report();
    reset_callback_records();

    esp_matter_attr_val_t val = esp_matter_attr_val(nullable<uint8_t>(42));
    esp_err_t err = attribute::report(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    TEST_ASSERT_FALSE(cb_pre_update.called);
    TEST_ASSERT_FALSE(cb_post_update.called);

    teardown_for_update_report();
}

// ============================================================
// update() MUST invoke attribute callbacks with correct values
// ============================================================

TEST_CASE("update calls PRE_UPDATE and POST_UPDATE callbacks", "[update][callback]")
{
    setup_for_update_report();
    reset_callback_records();

    esp_matter_attr_val_t val = esp_matter_attr_val(true);
    esp_err_t err = attribute::update(test_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    TEST_ASSERT_TRUE(cb_pre_update.called);
    TEST_ASSERT_EQUAL(attribute::PRE_UPDATE, cb_pre_update.type);
    TEST_ASSERT_EQUAL(test_endpoint_id, cb_pre_update.endpoint_id);
    TEST_ASSERT_EQUAL(OnOff::Id, cb_pre_update.cluster_id);
    TEST_ASSERT_EQUAL(OnOff::Attributes::OnOff::Id, cb_pre_update.attribute_id);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BOOLEAN, cb_pre_update.val.type);
    TEST_ASSERT_EQUAL(true, cb_pre_update.val.val.b);

    TEST_ASSERT_TRUE(cb_post_update.called);
    TEST_ASSERT_EQUAL(attribute::POST_UPDATE, cb_post_update.type);
    TEST_ASSERT_EQUAL(test_endpoint_id, cb_post_update.endpoint_id);
    TEST_ASSERT_EQUAL(OnOff::Id, cb_post_update.cluster_id);
    TEST_ASSERT_EQUAL(OnOff::Attributes::OnOff::Id, cb_post_update.attribute_id);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BOOLEAN, cb_post_update.val.type);
    TEST_ASSERT_EQUAL(true, cb_post_update.val.val.b);

    teardown_for_update_report();
}

TEST_CASE("update calls callbacks with correct value for nullable uint8", "[update][callback][nullable][uint8]")
{
    setup_for_update_report();
    reset_callback_records();

    esp_matter_attr_val_t val = esp_matter_attr_val(nullable<uint8_t>(77));
    esp_err_t err = attribute::update(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    TEST_ASSERT_TRUE(cb_pre_update.called);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_NULLABLE_UINT8, cb_pre_update.val.type);
    TEST_ASSERT_EQUAL(77, cb_pre_update.val.val.u8);

    TEST_ASSERT_TRUE(cb_post_update.called);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_NULLABLE_UINT8, cb_post_update.val.type);
    TEST_ASSERT_EQUAL(77, cb_post_update.val.val.u8);

    teardown_for_update_report();
}

// ============================================================
// update() and report() behavioral parity (stored value)
// ============================================================

TEST_CASE("update and report both store the value correctly", "[update][report_parity]")
{
    setup_for_update_report();

    esp_matter_attr_val_t val = esp_matter_attr_val(nullable<uint8_t>(55));
    esp_err_t err = attribute::update(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_matter_attr_val_t after_update;
    err = attribute::get_val(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &after_update);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(55, after_update.val.u8);

    val = esp_matter_attr_val(nullable<uint8_t>(88));
    err = attribute::report(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_matter_attr_val_t after_report;
    err = attribute::get_val(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &after_report);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(88, after_report.val.u8);

    TEST_ASSERT_EQUAL(after_update.type, after_report.type);

    teardown_for_update_report();
}
