// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_err.h>

#include <app/TestEventTriggerDelegate.h>

namespace esp_matter {
namespace test_event_trigger {

/** Set TestEventTriggerDelegate which can be triggered by TestEventTrigger command of GeneralDiagnostics cluster
 *
 * Note: This function should be called before esp_matter::start()
 *
 * @param[in] test_event_trigger_delegate TestEventTriggerDelegate
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_STATE when calling the function after esp_matter::start()
 */
esp_err_t set_delegate(chip::TestEventTriggerDelegate *test_event_trigger_delegate);

/** Get TestEventTriggerDelegate which can be triggered by TestEventTrigger command of GeneralDiagnostics cluster
 *
 * @return TestEventTriggerDelegate set by set_delegate()
 */
chip::TestEventTriggerDelegate *get_delegate();
} // namespace test_event_trigger
} // namespace esp_matter
