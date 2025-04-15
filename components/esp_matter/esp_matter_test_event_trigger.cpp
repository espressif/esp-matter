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

#include <esp_matter_core.h>
#include <esp_matter_test_event_trigger.h>

namespace esp_matter {
namespace test_event_trigger {

static chip::TestEventTriggerDelegate *s_test_event_trigger = nullptr;

esp_err_t set_delegate(chip::TestEventTriggerDelegate *test_event_trigger_delegate)
{
    if (esp_matter::is_started()) {
        return ESP_ERR_INVALID_STATE;
    }
    s_test_event_trigger = test_event_trigger_delegate;
    return ESP_OK;
}

chip::TestEventTriggerDelegate *get_delegate()
{
    return s_test_event_trigger;
}

} // namespace test_event_trigger
} // namespace esp_matter
