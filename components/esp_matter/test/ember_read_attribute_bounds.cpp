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

#include "cluster_lifecycle_common.h"

// The ember read entry point; it calls the converter we fixed.
#include <app/util/attribute-table.h>

using namespace esp_matter;
using chip::Protocols::InteractionModel::Status;

// A custom cluster with one attribute per scalar type, so each type can be read back.
static constexpr uint32_t k_test_cluster_id = 0xFFF1FC20;

struct ember_type_case_t {
    uint32_t attr_id;
    uint16_t width;
    esp_matter_attr_val_t val;
};

static uint16_t s_endpoint_id = 0;

static const ember_type_case_t *get_type_cases(size_t &count)
{
    static const ember_type_case_t cases[] = {
        { 0x0011, sizeof(bool),     esp_matter_bool(true) },
        { 0x0012, sizeof(int8_t),   esp_matter_int8(-42) },
        { 0x0013, sizeof(uint8_t),  esp_matter_uint8(0xAB) },
        { 0x0014, sizeof(int16_t),  esp_matter_int16(-12345) },
        { 0x0015, sizeof(uint16_t), esp_matter_uint16(0xABCD) },
        { 0x0016, sizeof(int32_t),  esp_matter_int32(-123456789) },
        { 0x0017, sizeof(uint32_t), esp_matter_uint32(0xABCDEF01u) },
        { 0x0018, sizeof(int64_t),  esp_matter_int64(-1234567890123LL) },
        { 0x0019, sizeof(uint64_t), esp_matter_uint64(0x1122334455667788ULL) },
    };
    count = sizeof(cases) / sizeof(cases[0]);
    return cases;
}

// Reading an attribute doesn't need the Matter stack started, so setup stays minimal.
static void setup_ember_read_bounds()
{
    static bool done = false;
    if (done) {
        return;
    }

    node_t *node = test::get_or_create_node();
    TEST_ASSERT_NOT_NULL(node);

    endpoint_t *endpoint = endpoint::create(node, ENDPOINT_FLAG_NONE, nullptr);
    TEST_ASSERT_NOT_NULL(endpoint);
    s_endpoint_id = endpoint::get_id(endpoint);

    cluster_t *cluster = cluster::create(endpoint, k_test_cluster_id, CLUSTER_FLAG_SERVER);
    TEST_ASSERT_NOT_NULL(cluster);

    size_t count = 0;
    const ember_type_case_t *cases = get_type_cases(count);
    for (size_t i = 0; i < count; ++i) {
        attribute_t *attr = attribute::create(cluster, cases[i].attr_id, ATTRIBUTE_FLAG_NONE, cases[i].val);
        TEST_ASSERT_NOT_NULL(attr);
    }

    done = true;
}

// A too-small read buffer must be rejected, not written past. The uint64 case used
// to check the wrong size (uint8) and let an 8-byte write through. The buffer here is
// always 8 bytes, so we assert on the returned status, never trigger a real overflow.
TEST_CASE("ember read rejects undersized buffer for every scalar width", "[ember_read_bounds]")
{
    setup_ember_read_bounds();

    size_t count = 0;
    const ember_type_case_t *cases = get_type_cases(count);
    for (size_t i = 0; i < count; ++i) {
        uint8_t buffer[sizeof(uint64_t)] = {};
        uint16_t undersized = cases[i].width - 1; // 0 for one-byte types
        Status status =
            emberAfReadAttribute(s_endpoint_id, k_test_cluster_id, cases[i].attr_id, buffer, undersized);
        TEST_ASSERT_TRUE_MESSAGE(status == Status::ResourceExhausted, "undersized read must be rejected");
    }
}

TEST_CASE("ember read succeeds and round-trips for every scalar width", "[ember_read_bounds]")
{
    setup_ember_read_bounds();

    size_t count = 0;
    const ember_type_case_t *cases = get_type_cases(count);
    for (size_t i = 0; i < count; ++i) {
        uint8_t buffer[sizeof(uint64_t)] = {};
        Status status =
            emberAfReadAttribute(s_endpoint_id, k_test_cluster_id, cases[i].attr_id, buffer, cases[i].width);
        TEST_ASSERT_TRUE_MESSAGE(status == Status::Success, "full-size read must succeed");
    }

    uint8_t buffer[sizeof(uint64_t)] = {};
    Status status = emberAfReadAttribute(s_endpoint_id, k_test_cluster_id, 0x0019, buffer, sizeof(uint64_t));
    TEST_ASSERT_TRUE(status == Status::Success);

    uint64_t read_back = 0;
    memcpy(&read_back, buffer, sizeof(read_back));
    TEST_ASSERT_EQUAL_UINT64(0x1122334455667788ULL, read_back);
}
