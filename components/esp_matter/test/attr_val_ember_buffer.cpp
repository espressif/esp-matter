/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_matter_attr_val_ember_buffer.h>
#include <unity.h>

TEST_CASE("long string ember size includes two-byte length prefix", "[attr_val_ember_buffer]")
{
    constexpr uint16_t payload_size = 300;
    char char_value[payload_size] = {};
    uint8_t octet_value[payload_size] = {};

    esp_matter_attr_val_t long_char = esp_matter_long_char_str(char_value, payload_size);
    esp_matter_attr_val_t long_octet = esp_matter_long_octet_str(octet_value, payload_size);
    TEST_ASSERT_EQUAL_UINT16(payload_size + sizeof(uint16_t), get_ember_attr_size_from_val(long_char));
    TEST_ASSERT_EQUAL_UINT16(payload_size + sizeof(uint16_t), get_ember_attr_size_from_val(long_octet));

    esp_matter_attr_val_t null_long_char = esp_matter_long_char_str(nullptr, UINT16_MAX);
    esp_matter_attr_val_t null_long_octet = esp_matter_long_octet_str(nullptr, UINT16_MAX);
    TEST_ASSERT_EQUAL_UINT16(sizeof(uint16_t), get_ember_attr_size_from_val(null_long_char));
    TEST_ASSERT_EQUAL_UINT16(sizeof(uint16_t), get_ember_attr_size_from_val(null_long_octet));
}
