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

#include <esp_err.h>
#include <app/clusters/identify-server/identify-server.h>

typedef void (*identify_callback)(Identify * identify);

esp_err_t esp_matter_init_identify(chip::EndpointId endpoint, identify_callback start_cb, identify_callback stop_cb,
                                   EmberAfIdentifyIdentifyType type, identify_callback effect_cb);
