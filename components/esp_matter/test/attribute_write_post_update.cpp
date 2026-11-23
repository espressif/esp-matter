/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <unity.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model_provider.h>
#include <nvs_flash.h>

#include <access/SubjectDescriptor.h>
#include <app/AttributeValueDecoder.h>
#include <app/ConcreteAttributePath.h>
#include <app/data-model-provider/ActionReturnStatus.h>
#include <app/data-model-provider/OperationTypes.h>
#include <lib/core/TLVReader.h>
#include <lib/core/TLVWriter.h>
#include <platform/PlatformManager.h>

#include "common.h"

using namespace esp_matter;
using namespace chip::app::Clusters;

// These tests drive provider::WriteAttribute directly to verify that the application
// POST_UPDATE callback fires only when the incoming value was successfully decoded. A
// decode failure must never forward an uninitialized value to the callback, across every
// write branch (code-driven registry, AAI, esp-matter store) that shares the gate.

static node_t *test_node = nullptr;
static endpoint_t *test_endpoint = nullptr;
static uint16_t test_endpoint_id = 0;

static bool post_update_called = false;
static esp_matter_attr_val_t post_update_val;

static esp_err_t test_attribute_callback(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    if (type == attribute::POST_UPDATE) {
        post_update_called = true;
        post_update_val = *val;
    }
    return ESP_OK;
}

static void setup()
{
    static bool setup_done = false;
    if (!setup_done) {
        TEST_ASSERT_EQUAL(ESP_OK, nvs_flash_init());
        node::config_t node_config;
        test_node = node::create(&node_config, test_attribute_callback, nullptr);
        TEST_ASSERT_NOT_NULL(test_node);
        endpoint::extended_color_light::config_t light_config;
        test_endpoint = endpoint::extended_color_light::create(test_node, &light_config, ENDPOINT_FLAG_NONE, nullptr);
        TEST_ASSERT_NOT_NULL(test_endpoint);
        test_endpoint_id = endpoint::get_id(test_endpoint);
        test::suppress_matter_logs();
        TEST_ASSERT_EQUAL(ESP_OK, esp_matter::start(nullptr));
        setup_done = true;
    }
    post_update_called = false;
    post_update_val = esp_matter_attr_val();
}

// Encode a value element with `put` into a decoder and drive it through WriteAttribute.
template <typename PutFn>
static chip::app::DataModel::ActionReturnStatus write_attribute(uint32_t cluster_id, uint32_t attribute_id, PutFn put)
{
    uint8_t buffer[128];
    chip::TLV::TLVWriter writer;
    writer.Init(buffer);
    put(writer);
    TEST_ASSERT_TRUE(writer.Finalize() == CHIP_NO_ERROR);

    chip::TLV::TLVReader reader;
    reader.Init(buffer, writer.GetLengthWritten());
    TEST_ASSERT_TRUE(reader.Next() == CHIP_NO_ERROR);

    chip::Access::SubjectDescriptor subject;
    chip::app::AttributeValueDecoder decoder(reader, subject);
    chip::app::ConcreteDataAttributePath path(test_endpoint_id, cluster_id, attribute_id);
    chip::app::DataModel::WriteAttributeRequest request(path, subject);
    // The IM holds the stack lock around WriteAttribute; hold it here too, as the normal
    // caller (set_val_via_write_attribute) does, otherwise the data-model path asserts.
    chip::DeviceLayer::StackLock lock;
    return data_model::provider::get_instance().WriteAttribute(request, decoder, true /* call_callbacks */);
}

// LevelControl::Options (0x000F) is a writable bitmap8, mandatory on the light endpoint.
TEST_CASE("write with valid value fires POST_UPDATE with the decoded value", "[write_post_update]")
{
    setup();

    auto status = write_attribute(LevelControl::Id, LevelControl::Attributes::Options::Id,
    [](chip::TLV::TLVWriter & w) {
        TEST_ASSERT_TRUE(w.Put(chip::TLV::AnonymousTag(), (uint8_t)0x01) == CHIP_NO_ERROR);
    });

    TEST_ASSERT_TRUE(status.IsSuccess());
    TEST_ASSERT_TRUE(post_update_called);
    TEST_ASSERT_EQUAL(0x01, post_update_val.val.u8);
}

TEST_CASE("write with undecodable value does not fire POST_UPDATE", "[write_post_update]")
{
    setup();

    // Encode a boolean where the uint8 attribute is expected: decode fails on every write branch,
    // so POST_UPDATE must not fire and the write must not report success.
    auto status = write_attribute(LevelControl::Id, LevelControl::Attributes::Options::Id,
    [](chip::TLV::TLVWriter & w) {
        TEST_ASSERT_TRUE(w.PutBoolean(chip::TLV::AnonymousTag(), true) == CHIP_NO_ERROR);
    });

    TEST_ASSERT_FALSE(status.IsSuccess());
    TEST_ASSERT_FALSE(post_update_called);
}
