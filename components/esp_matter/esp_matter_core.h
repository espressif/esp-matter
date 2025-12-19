// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include <app/CASESessionManager.h>
#include <app/DeviceProxy.h>
#include <app/InteractionModelEngine.h>
#include <app/util/af-types.h>
#include <esp_err.h>
#include <app/AttributePathParams.h>
#include <app/CommandPathParams.h>
#include <app/EventPathParams.h>
#include <app/server/Server.h>

using chip::app::ConcreteCommandPath;
using chip::DeviceLayer::ChipDeviceEvent;
using chip::TLV::TLVReader;

namespace esp_matter {

/** Event callback
 *
 * @param[in] event event data pointer.
 * @param[in] arg Pointer to the private data passed while setting the callback.
 */
/** TODO: Change this */
typedef void (*event_callback_t)(const ChipDeviceEvent *event, intptr_t arg);

/** Return whether the Matter is initialized and started
 *
 *  @return true if Matter is started
 *  @return false if Matter is not started
 */
bool is_started();

/**
 * @brief Set the server initialization parameters for Matter.
 *
 * This function must be called before Matter is started. `esp_matter::start()`
 * Calling it after Matter has started will have no effect and will return an error.
 *
 * @note Starting Matter without valid initialization parameters may lead to undefined behavior or startup failure.
 *
 * @note The provided pointer is stored internally and used later during Matter initialization.
 * It is not copied, so the caller must ensure that the memory pointed to by `server_init_params`
 * remains valid and unmodified until Matter is started.
 *
 * @note If this function is called with a `nullptr`, it will still return ESP_OK. The system
 * will proceed with default initialization behavior.
 *
 * @param[in] server_init_params Pointer to the server initialization parameters. May be nullptr.
 *
 * @return ESP_OK Successfully set the server initialization parameters.
 * @return ESP_ERR_INVALID_STATE If called after Matter has already started.
 * @return error in case of failure.
 */

esp_err_t set_server_init_params(chip::CommonCaseDeviceServerInitParams *server_init_params);

/** ESP Matter Start
 *
 * Initialize and start the matter thread.
 *
 * @param[in] callback event callback.
 * @param[in] callback_arg private data to pass to callback function, optional argument, by default set to NULL.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t start(event_callback_t callback, intptr_t callback_arg = static_cast<intptr_t>(NULL));

/** Factory reset
 *
 * Perform factory reset and erase the data stored in the non volatile storage. This also restarts the device.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t factory_reset();

/** Initialize esp-matter nvs partition CONFIG_ESP_MATTER_NVS_PART_NAME
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_matter_nvs_init();

namespace lock {

/** Lock status */
typedef enum status {
    /** Lock failed */
    FAILED,
    /** Lock was already taken */
    ALREADY_TAKEN,
    /** Lock success */
    SUCCESS,
} status_t;

/**
 * @brief RAII-style scoped lock for the Matter (CHIP) stack.
 *
 * This class provides automatic locking and unlocking of the Matter stack using the
 * RAII (Resource Acquisition Is Initialization) pattern. The lock is acquired when
 * the object is constructed and automatically released when it goes out of scope.
 *
 * Use this class whenever you need to access Matter stack APIs from a non-Matter context
 * (e.g., from application tasks, callbacks, or interrupt handlers). The Matter stack is
 * single-threaded and requires synchronization when accessed from external contexts.
 *
 * @section advantages Advantages
 * - **Exception Safety**: The lock is automatically released even if an exception occurs
 *   or the function returns early, preventing deadlocks.
 * - **Simplified Code**: No need to manually call lock/unlock functions or worry about
 *   missing unlock calls in error paths.
 * - **Scope-based Lifetime**: The lock duration is clearly defined by the variable's scope,
 *   making the code easier to read and maintain.
 * - **Copy Prevention**: The class is non-copyable to prevent accidental lock duplication
 *   which could lead to undefined behavior.
 *
 * @note Always use the smallest possible scope for the lock to minimize blocking time.
 */
class ScopedChipStackLock {
public:
    ScopedChipStackLock(uint32_t ticks_to_wait) {
        status = chip_stack_lock(ticks_to_wait);
    }
    ~ScopedChipStackLock() {
        if (status == SUCCESS) {
            chip_stack_unlock();
        }
    }
private:
    ScopedChipStackLock(const ScopedChipStackLock &) = delete;
    ScopedChipStackLock &operator=(const ScopedChipStackLock &) = delete;
    ScopedChipStackLock(ScopedChipStackLock &&) = default;
    ScopedChipStackLock &operator=(ScopedChipStackLock &&) = default;
    status_t status;

    /** Stack lock
    *
    * This API should be called before calling any upstream APIs.
    *
    * @param[in] ticks_to_wait number of ticks to wait for trying to take the lock. Accepted values: 0 to portMAX_DELAY.
    *
    * @return FAILED if the lock was not taken within the specified ticks.
    * @return ALREADY_TAKEN if the lock was already taken by the same task context.
    * @return SUCCESS if the lock was taken successfully.
    */
    status_t chip_stack_lock(uint32_t ticks_to_wait);

    /** Stack unlock
    *
    * This API should be called after the upstream APIs have been done calling.
    *
    * @note: This should only be called if `chip_stack_lock()` returns `SUCCESS`.
    *
    * @return ESP_OK on success.
    * @return error in case of failure.
    */
    esp_err_t chip_stack_unlock();
}; 

} /* lock */

} /* esp_matter */
