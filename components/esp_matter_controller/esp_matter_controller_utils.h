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

#include <stdint.h>

int oct_str_to_byte_arr(char *oct_str, uint8_t *byte_array);

uint64_t string_to_uint64(char *str);

uint32_t string_to_uint32(char *str);

uint16_t string_to_uint16(char *str);

uint8_t string_to_uint8(char *str);
