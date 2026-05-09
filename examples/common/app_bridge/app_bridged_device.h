// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <sdkconfig.h>

#include <nvs.h>
#include <esp_matter_bridge.h>
#include <nvs_key_allocator.h>

using esp_matter::node_t;

/* Virtual Class for Bridged Device */
class app_bridged_device_t {
public:
    app_bridged_device_t() : m_dev(nullptr), m_dev_addr_ctx(nullptr), m_next(nullptr), m_priv_data(nullptr) {};

    virtual ~app_bridged_device_t() = default;

    // This function will set device address for this class. It might allocate memory for device address
    // and the allocated memory must be released in delete_dev_addr().
    virtual esp_err_t set_dev_addr(const void *addr_ctx) = 0;
    // This function will compare device address in addr_ctx and dev_addr_ctx. If the two addresses are the
    // same, it should return true.
    virtual bool check_dev_addr(const void *addr_ctx) = 0;
    // This function will release the allocated device address in set_dev_addr().
    virtual esp_err_t delete_dev_addr() = 0;
    // This function will store the device address in NVS.
    virtual esp_err_t store_dev_addr() = 0;
    // This function will restore the device address from NVS, it might allocate memory for device address
    // and the allocated memory must be released in delete_dev_addr().
    virtual esp_err_t restore_dev_addr() = 0;
    // This function will erase the device address from NVS.
    virtual esp_err_t erase_dev_addr() = 0;

    esp_matter_bridge::device_t *get_matter_device() const
    {
        return m_dev;
    }
    void set_matter_device(esp_matter_bridge::device_t *dev)
    {
        m_dev = dev;
    }

    void *get_dev_addr() const
    {
        return m_dev_addr_ctx;
    }

    app_bridged_device_t *get_next() const
    {
        return m_next;
    }
    void set_next(app_bridged_device_t *next)
    {
        m_next = next;
    }

    void *get_priv_data() const
    {
        return m_priv_data;
    }
    void set_priv_data(void *priv_data)
    {
        m_priv_data = priv_data;
    }

protected:
    /** Bridged Device */
    esp_matter_bridge::device_t *m_dev;
    /** Address context of Bridged Device */
    void *m_dev_addr_ctx;
    /** Pointer of Next Bridged Device */
    app_bridged_device_t *m_next;
    /* User initialization data */
    void *m_priv_data;
};

namespace esp_matter_bridge {
namespace nvs_key_allocator {

inline StorageKeyName endpoint_dev_addr(uint16_t endpoint_id)
{
    return StorageKeyName::Formatted("b/%x/da", endpoint_id);
}

} // namespace nvs_key_allocator
} // namespace esp_matter_bridge

esp_err_t map_matter_error(CHIP_ERROR error);

typedef app_bridged_device_t *(*create_device_callback_t)(node_t *node, uint16_t endpoint_id);
typedef void (*free_device_callback_t)(app_bridged_device_t *device);

esp_err_t app_bridge_initialize(node_t *node, esp_matter_bridge::bridge_device_type_callback_t device_type_cb,
                                create_device_callback_t create_cb, free_device_callback_t free_cb);

esp_err_t app_bridge_create_new_device(node_t *node, uint16_t parent_endpoint_id, uint32_t matter_device_type_id,
                                       void *addr_ctx, void *priv_data);

esp_err_t app_bridge_remove_device(app_bridged_device_t *bridged_device);

app_bridged_device_t *app_bridge_get_device(const void *dev_addr);

app_bridged_device_t *app_bridge_get_device(uint16_t endpoint_id);

uint16_t app_bridge_get_endpoint(const void *dev_addr);
