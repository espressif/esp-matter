/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_matter_attribute_utils.h"
#include <unity.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <nvs_flash.h>

#include "common.h"

using namespace esp_matter;
using namespace chip::app::Clusters;

static node_t *test_node = nullptr;
static endpoint_t *root_endpoint = nullptr;
static endpoint_t *test_endpoint = nullptr;
static uint16_t test_endpoint_id = 0;
static uint16_t root_endpoint_id = 0;

// Configurable test values for ESP Matter managed attributes
static uint8_t test_current_level = 42;
static bool test_on_off = true;
static uint16_t test_identify_time = 10;
static uint8_t test_identify_type = 2;
static uint8_t test_color_mode = 1;
static uint16_t test_color_temp = 250;

void setup_for_get_val()
{
    static bool setup_done = false;
    if (setup_done) {
        return;
    }

    esp_err_t err = nvs_flash_init();
    TEST_ASSERT_EQUAL(ESP_OK, err);

    node::config_t node_config;
    test_node = node::create(&node_config, nullptr, nullptr);
    TEST_ASSERT_NOT_NULL(test_node);

    root_endpoint = endpoint::get(test_node, 0);
    TEST_ASSERT_NOT_NULL(root_endpoint);

    root_endpoint_id = endpoint::get_id(root_endpoint);
    TEST_ASSERT_EQUAL(0, root_endpoint_id);

    // basically we would need some dataset that we should use for the test
    // for now we are using the extended_color_light::config_t
    endpoint::extended_color_light::config_t light_config;
    light_config.on_off.on_off = test_on_off;
    light_config.level_control.current_level = nullable<uint8_t>(test_current_level);
    light_config.identify.identify_time = test_identify_time;
    light_config.identify.identify_type = test_identify_type;
    light_config.color_control.color_mode = test_color_mode;
    test_endpoint = endpoint::extended_color_light::create(test_node, &light_config, ENDPOINT_FLAG_NONE, nullptr);
    TEST_ASSERT_NOT_NULL(test_endpoint);

    test_endpoint_id = endpoint::get_id(test_endpoint);
    test::suppress_matter_logs();
    err = esp_matter::start(nullptr);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    setup_done = true;
}

void teardown_for_get_val()
{
    // TODO: Add proper cleanup once the Matter stack supports a clean shutdown sequence
    return;
}

TEST_CASE("get_val invalid inputs", "[get_val][invalid]")
{
    setup_for_get_val();

    esp_err_t err = attribute::get_val(nullptr, nullptr);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    esp_matter_attr_val_t val;
    err = attribute::get_val(nullptr, &val);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    attribute_t *attr = attribute::get(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id);
    TEST_ASSERT_NOT_NULL(attr);
    err = attribute::get_val(attr, nullptr);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    /// with ids as input
    err = attribute::get_val(chip::kInvalidEndpointId, chip::kInvalidClusterId, chip::kInvalidAttributeId, nullptr);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    err = attribute::get_val(chip::kInvalidEndpointId, chip::kInvalidClusterId, chip::kInvalidAttributeId, &val);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    teardown_for_get_val();
}

// Arrays - not supported for get_val
TEST_CASE("get_val array not supported", "[get_val][esp_matter_managed][invalid][array]")
{
    setup_for_get_val();

    esp_matter_attr_val_t val;
    esp_err_t err = attribute::get_val(test_endpoint_id, Descriptor::Id, Descriptor::Attributes::ServerList::Id, &val);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, err);

    err = attribute::get_val(test_endpoint_id, Descriptor::Id, Descriptor::Attributes::DeviceTypeList::Id, &val);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, err);

    teardown_for_get_val();
}

// Primitive Types - ESP Matter Managed

TEST_CASE("get_val bool - OnOff", "[get_val][esp_matter_managed][bool]")
{
    setup_for_get_val();

    attribute_t *attr = attribute::get(test_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id);
    TEST_ASSERT_NOT_NULL(attr);
    esp_matter_attr_val_t setable_val = esp_matter_attr_val(true);
    esp_err_t err = attribute::set_val(attr, &setable_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_matter_attr_val_t true_val;
    err = attribute::get_val(attr, &true_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BOOLEAN, true_val.type);
    TEST_ASSERT_EQUAL(true, true_val.val.b);

    setable_val = esp_matter_attr_val(false);
    err = attribute::set_val(attr, &setable_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_matter_attr_val_t false_val;
    err = attribute::get_val(attr, &false_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BOOLEAN, false_val.type);
    TEST_ASSERT_EQUAL(false, false_val.val.b);

    teardown_for_get_val();
}

TEST_CASE("get_val uint8 - ColorMode", "[get_val][esp_matter_managed][uint8]")
{
    setup_for_get_val();

    attribute_t *attr = attribute::get(test_endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorMode::Id);
    TEST_ASSERT_NOT_NULL(attr);
    esp_matter_attr_val_t setable_val = esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum);
    esp_err_t err = attribute::set_val(attr, &setable_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_matter_attr_val_t retrieved_val;
    err = attribute::get_val(attr, &retrieved_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_ENUM8, retrieved_val.type);
    TEST_ASSERT_EQUAL(0, retrieved_val.val.u8);

    setable_val = esp_matter_attr_val(static_cast<uint8_t>(1), esp_matter_attr_val::uint_sub_type::k_enum);
    err = attribute::set_val(attr, &setable_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    err = attribute::get_val(attr, &retrieved_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_ENUM8, retrieved_val.type);
    TEST_ASSERT_EQUAL(1, retrieved_val.val.u8);

    setable_val = esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum);
    err = attribute::set_val(attr, &setable_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    err = attribute::get_val(attr, &retrieved_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_ENUM8, retrieved_val.type);
    TEST_ASSERT_EQUAL(2, retrieved_val.val.u8);

    // Invalid enum value (ColorMode valid range is 0-2)
    setable_val = esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum);
    err = attribute::set_val(attr, &setable_val);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    teardown_for_get_val();
}

TEST_CASE("get_val uint16 - ColorTemperatureMireds", "[get_val][esp_matter_managed][uint16]")
{
    setup_for_get_val();

    esp_matter_attr_val_t val;
    esp_err_t err = attribute::get_val(test_endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT16, val.type);
    TEST_ASSERT_EQUAL(test_color_temp, val.val.u16);

    teardown_for_get_val();
}

TEST_CASE("get_val uint32 - Feature map", "[get_val][esp_matter_managed][uint32]")
{
    setup_for_get_val();

    esp_matter_attr_val_t val;
    esp_err_t err = attribute::get_val(test_endpoint_id, OnOff::Id, Globals::Attributes::FeatureMap::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BITMAP32, val.type);
    TEST_ASSERT_EQUAL(1, val.val.u32);

    teardown_for_get_val();
}

// Nullable Types - ESP Matter Managed

TEST_CASE("get_val nullable uint8 - CurrentLevel", "[get_val][esp_matter_managed][nullable][uint8]")
{
    setup_for_get_val();

    attribute_t *attr = attribute::get(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id);
    TEST_ASSERT_NOT_NULL(attr);
    esp_matter_attr_val_t setable_val = esp_matter_attr_val(nullable<uint8_t>(test_current_level));
    esp_err_t err = attribute::set_val(attr, &setable_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    esp_matter_attr_val_t retrieved_val;
    err = attribute::get_val(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &retrieved_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_NULLABLE_UINT8, retrieved_val.type);
    nullable<uint8_t> data(retrieved_val.val.u8);
    TEST_ASSERT_FALSE(data.is_null());
    TEST_ASSERT_EQUAL(test_current_level, data.value());
    setable_val = esp_matter_attr_val(nullable<uint8_t>());
    err = attribute::set_val(attr, &setable_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    err = attribute::get_val(test_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &retrieved_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_NULLABLE_UINT8, retrieved_val.type);
    nullable<uint8_t> null_data(retrieved_val.val.u8);
    TEST_ASSERT_TRUE(null_data.is_null());

    teardown_for_get_val();
}

TEST_CASE("get_val nullable uint16 - StartUpColorTemperatureMireds", "[get_val][esp_matter_managed][nullable][uint16]")
{
    setup_for_get_val();

    attribute_t *attr = attribute::get(test_endpoint_id, ColorControl::Id, ColorControl::Attributes::StartUpColorTemperatureMireds::Id);
    TEST_ASSERT_NOT_NULL(attr);

    // this is nvs type value, so it may screw up our tests if ran without clearing the nvs
    // hence, always set and verify the value

    esp_matter_attr_val_t setable_val = esp_matter_attr_val(nullable<uint16_t>(test_color_temp));
    esp_err_t err = attribute::set_val(attr, &setable_val);
    TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FINISHED);

    esp_matter_attr_val_t retrieved_val;
    err = attribute::get_val(attr, &retrieved_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_NULLABLE_UINT16, retrieved_val.type);
    nullable<uint16_t> data(retrieved_val.val.u16);
    TEST_ASSERT_FALSE(data.is_null());
    TEST_ASSERT_EQUAL(test_color_temp, data.value());

    setable_val = esp_matter_attr_val(nullable<uint16_t>());
    err = attribute::set_val(attr, &setable_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    err = attribute::get_val(attr, &retrieved_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_NULLABLE_UINT16, retrieved_val.type);
    nullable<uint16_t> data2(retrieved_val.val.u16);
    TEST_ASSERT_TRUE(data2.is_null());

    teardown_for_get_val();
}

// ====================================================================================
// get_val() -> Internally Managed Attributes (by ConnectedHomeIP)
// ====================================================================================

// Arrays - not supported for get_val
TEST_CASE("get_val array not supported", "[get_val][internal_managed][invalid][array]")
{
    setup_for_get_val();

    esp_matter_attr_val_t val;

    esp_err_t err = attribute::get_val(test_endpoint_id, Descriptor::Id, Descriptor::Attributes::ServerList::Id, &val);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, err);

    err = attribute::get_val(test_endpoint_id, Descriptor::Id, Descriptor::Attributes::DeviceTypeList::Id, &val);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, err);

    err = attribute::get_val(root_endpoint_id, Descriptor::Id, Descriptor::Attributes::PartsList::Id, &val);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, err);

    teardown_for_get_val();
}

// Primitive Types - Internally Managed

TEST_CASE("get_val uint16", "[get_val][internal_managed][uint16]")
{
    setup_for_get_val();

    esp_matter_attr_val_t vendor_id_val;
    esp_err_t err = attribute::get_val(root_endpoint_id, BasicInformation::Id, BasicInformation::Attributes::VendorID::Id, &vendor_id_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT16, vendor_id_val.type);
    TEST_ASSERT_EQUAL(CONFIG_DEVICE_VENDOR_ID, vendor_id_val.val.u16);
    esp_matter_attr_val_t product_id_val;
    err = attribute::get_val(root_endpoint_id, BasicInformation::Id, BasicInformation::Attributes::ProductID::Id, &product_id_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT16, product_id_val.type);
    TEST_ASSERT_EQUAL(CONFIG_DEVICE_PRODUCT_ID, product_id_val.val.u16);

    teardown_for_get_val();
}

TEST_CASE("get_val uint32 - SoftwareVersion", "[get_val][internal_managed][uint32]")
{
    setup_for_get_val();

    esp_matter_attr_val_t val;
    esp_err_t err = attribute::get_val(root_endpoint_id, BasicInformation::Id, BasicInformation::Attributes::SoftwareVersion::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT32, val.type);
    TEST_ASSERT_EQUAL(1, val.val.u32);

    teardown_for_get_val();
}

TEST_CASE("get_val uint8 - MaxNetworks", "[get_val][internal_managed][uint8]")
{
    setup_for_get_val();

    esp_matter_attr_val_t val;
    esp_err_t err = attribute::get_val(root_endpoint_id, NetworkCommissioning::Id, NetworkCommissioning::Attributes::MaxNetworks::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_UINT8, val.type);
    TEST_ASSERT_EQUAL(1, val.val.u8);

    teardown_for_get_val();
}

TEST_CASE("get_val bool - SupportsConcurrentConnection", "[get_val][internal_managed][bool]")
{
    setup_for_get_val();

    esp_matter_attr_val_t val;
    esp_err_t err = attribute::get_val(root_endpoint_id, GeneralCommissioning::Id, GeneralCommissioning::Attributes::SupportsConcurrentConnection::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_BOOLEAN, val.type);
    TEST_ASSERT_EQUAL(true, val.val.b);

    teardown_for_get_val();
}

// Strings - Internally Managed

TEST_CASE("get_val char_string", "[get_val][internal_managed][char_string]")
{
    setup_for_get_val();

    esp_matter_attr_val_t vendor_name_val;
    esp_err_t err = attribute::get_val(root_endpoint_id, BasicInformation::Id, BasicInformation::Attributes::VendorName::Id, &vendor_name_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_CHAR_STRING, vendor_name_val.type);
    TEST_ASSERT_NOT_NULL(vendor_name_val.val.a.b);
    TEST_ASSERT_GREATER_THAN(0, vendor_name_val.val.a.s);
    TEST_ASSERT_EQUAL_STRING("TEST_VENDOR", vendor_name_val.val.a.b);
    free(vendor_name_val.val.a.b);
    esp_matter_attr_val_t product_name_val;
    err = attribute::get_val(root_endpoint_id, BasicInformation::Id, BasicInformation::Attributes::ProductName::Id, &product_name_val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_CHAR_STRING, product_name_val.type);
    TEST_ASSERT_NOT_NULL(product_name_val.val.a.b);
    TEST_ASSERT_GREATER_THAN(0, product_name_val.val.a.s);
    TEST_ASSERT_EQUAL_STRING("TEST_PRODUCT", product_name_val.val.a.b);
    free(product_name_val.val.a.b);

    teardown_for_get_val();
}

// Nullable Types - Internally Managed

TEST_CASE("get_val nullable int32 - LastConnectErrorValue", "[get_val][internal_managed][nullable][int32]")
{
    setup_for_get_val();

    esp_matter_attr_val_t val;
    esp_err_t err = attribute::get_val(root_endpoint_id, NetworkCommissioning::Id, NetworkCommissioning::Attributes::LastConnectErrorValue::Id, &val);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(ESP_MATTER_VAL_TYPE_NULLABLE_INT32, val.type);
    nullable<int32_t> nullable_val(val.val.i32);
    TEST_ASSERT_EQUAL(true, nullable_val.is_null());

    teardown_for_get_val();
}
