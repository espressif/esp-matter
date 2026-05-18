/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unity.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model.h>
#include <unity_test_utils_memory.h>

#include "cluster_lifecycle_common.h"

using namespace esp_matter;

static constexpr uint32_t k_recreate_iterations = 3;
static constexpr chip::ClusterId kManagedDelegateTestClusterId = 0xFFF1FC00;

struct TestManagedDelegateInstance {
    uint16_t endpoint_id;
};

static uint32_t g_managed_delegate_init_count;
static uint32_t g_managed_delegate_allocation_count;
static uint32_t g_managed_delegate_shutdown_count;

static void test_managed_delegate_shutdown_callback(void *managed_instance, uint16_t endpoint_id)
{
    TEST_ASSERT_NOT_NULL(managed_instance);
    auto *instance = static_cast<TestManagedDelegateInstance *>(managed_instance);
    TEST_ASSERT_EQUAL(endpoint_id, instance->endpoint_id);
    delete instance;
    ++g_managed_delegate_shutdown_count;
}

static void test_managed_delegate_init_callback(void *delegate, uint16_t endpoint_id)
{
    TEST_ASSERT_NOT_NULL(delegate);
    ++g_managed_delegate_init_count;

    cluster_t *cluster = cluster::get(endpoint_id, kManagedDelegateTestClusterId);
    TEST_ASSERT_NOT_NULL(cluster);
    if (cluster::get_delegate_managed_instance(cluster) != nullptr) {
        return;
    }

    auto *instance = new TestManagedDelegateInstance{ endpoint_id };
    TEST_ASSERT_NOT_NULL(instance);
    TEST_ASSERT_EQUAL(ESP_OK, cluster::set_delegate_shutdown_callback_and_managed_instance(
                          cluster, test_managed_delegate_shutdown_callback, instance));
    ++g_managed_delegate_allocation_count;
}

static void create_destroy_managed_delegate_endpoint(node_t *node, uint16_t endpoint_id)
{
    endpoint_t *endpoint = endpoint::resume(node, ENDPOINT_FLAG_DESTROYABLE, endpoint_id, nullptr);
    TEST_ASSERT_NOT_NULL(endpoint);

    cluster_t *cluster = cluster::create(endpoint, kManagedDelegateTestClusterId, CLUSTER_FLAG_SERVER);
    TEST_ASSERT_NOT_NULL(cluster);
    TEST_ASSERT_EQUAL(ESP_OK, cluster::set_delegate_and_init_callback(cluster, test_managed_delegate_init_callback,
                                                                      &g_managed_delegate_init_count));

    esp_err_t err = endpoint::enable(endpoint);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(cluster::get_delegate_managed_instance(cluster));

    err = endpoint::destroy(node, endpoint);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NULL(endpoint::get(node, endpoint_id));
}

// -----------------------------------------------------------------------------
// SCOPE
// -----------------------------------------------------------------------------

// Regression coverage for 704ed3dda, which added per-cluster managed delegate instances and endpoint-destroy
// shutdown callbacks. The ownership invariant lives in esp_matter's generic cluster slot, so this test uses a
// synthetic cluster to avoid coupling the leak test to upstream cluster-specific registration behavior.

// -----------------------------------------------------------------------------
// TESTS
// -----------------------------------------------------------------------------

TEST_CASE("endpoint destroy invokes managed delegate shutdown callback", "[cluster_lifecycle][managed_delegate]")
{
    node_t *node = test::get_or_create_node();
    test::start_matter_if_needed();
    uint16_t endpoint_id = test::reserve_destroyable_endpoint_id(node);

    g_managed_delegate_init_count = 0;
    g_managed_delegate_allocation_count = 0;
    g_managed_delegate_shutdown_count = 0;

    unity_utils_record_free_mem();

    for (uint32_t recreate_index = 0; recreate_index < k_recreate_iterations; ++recreate_index) {
        create_destroy_managed_delegate_endpoint(node, endpoint_id);
    }

    TEST_ASSERT_EQUAL(k_recreate_iterations, g_managed_delegate_init_count);
    TEST_ASSERT_EQUAL(k_recreate_iterations, g_managed_delegate_allocation_count);
    TEST_ASSERT_EQUAL(k_recreate_iterations, g_managed_delegate_shutdown_count);
    unity_utils_evaluate_leaks_direct(0);
}
