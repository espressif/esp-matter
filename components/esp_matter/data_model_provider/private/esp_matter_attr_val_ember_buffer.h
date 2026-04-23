// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

// This file provides some APIs which help transferring esp_matter_attr_val_t to ember buffer data.
// This file should be deleted when ember is totally decoupled with Matter Interaction Model.

#pragma once

#include <protocols/interaction_model/StatusCode.h>
#include <esp_matter_attribute_utils.h>

using chip::Protocols::InteractionModel::Status;

Status build_ember_buffer_from_attr_val(const esp_matter_attr_val_t &val, uint8_t *dataPtr, uint16_t bufferSize);

uint8_t  get_ember_attr_type_from_val_type(esp_matter_val_type_t val_type);

uint16_t get_ember_attr_size_from_val(const esp_matter_attr_val_t &val);
