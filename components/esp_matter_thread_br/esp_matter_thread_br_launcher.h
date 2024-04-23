// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_openthread.h>
#if CONFIG_OPENTHREAD_BR_AUTO_UPDATE_RCP
#include <esp_rcp_update.h>
#endif
#include <openthread/border_agent.h>

#define OPENTHREAD_CLI_BUFFER_LENGTH 255

namespace esp_matter {

typedef struct {
    char buf[OPENTHREAD_CLI_BUFFER_LENGTH + 1];
    uint8_t buf_len;
} ot_cli_buffer_t;

esp_err_t thread_br_init(esp_openthread_platform_config_t *config);

#if CONFIG_OPENTHREAD_BR_AUTO_UPDATE_RCP
esp_err_t thread_rcp_update_init(const esp_rcp_update_config_t *update_config);
#endif

esp_err_t set_thread_enabled(bool enabled);

esp_err_t set_thread_dataset_tlvs(otOperationalDatasetTlvs *dataset_tlvs);

esp_err_t get_thread_dataset_tlvs(otOperationalDatasetTlvs *dataset_tlvs);

esp_err_t get_border_agent_id(otBorderAgentId *border_agent_id);

uint8_t get_thread_role();

esp_err_t cli_transmit_task_post(ot_cli_buffer_t &cli_buf);
}
