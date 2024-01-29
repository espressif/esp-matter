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

#include <esp_matter_controller_utils.h>
#include <string.h>

namespace esp_matter {
namespace controller {

#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
uint8_t s_controller_fabric_index = chip::kUndefinedFabricIndex;

void set_fabric_index(uint8_t fabric_index)
{
    s_controller_fabric_index = fabric_index;
}

uint8_t get_fabric_index(void)
{
    return s_controller_fabric_index;
}
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

} // namespace controller
} // namespace esp_matter
static uint8_t char_to_hex_digit(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else {
        return 0xFF;
    }
}

int oct_str_to_byte_arr(char *oct_str, uint8_t *byte_array)
{
    if (strlen(oct_str) % 2 != 0) {
        return -1;
    }
    size_t byte_array_len = strlen(oct_str) / 2;
    for (size_t idx = 0; idx < byte_array_len; ++idx) {
        uint8_t digit1 = char_to_hex_digit(oct_str[2 * idx]);
        uint8_t digit2 = char_to_hex_digit(oct_str[2 * idx + 1]);
        if (digit1 == 0xFF || digit2 == 0xFF) {
            return -1;
        }
        byte_array[idx] = (digit1 << 4) + digit2;
    }
    return byte_array_len;
}

uint64_t string_to_uint64(char *str)
{
    if (strlen(str) > 2 && str[0] == '0' && str[1] == 'x') {
        return strtoull(&str[2], NULL, 16);
    } else {
        return strtoull(str, NULL, 10);
    }
}

uint32_t string_to_uint32(char *str)
{
    if (strlen(str) > 2 && str[0] == '0' && str[1] == 'x') {
        return strtoul(&str[2], NULL, 16);
    } else {
        return strtoul(str, NULL, 10);
    }
}

uint16_t string_to_uint16(char *str)
{
    return (uint16_t)string_to_uint32(str);
}

uint8_t string_to_uint8(char *str)
{
    return (uint8_t)string_to_uint32(str);
}

int64_t string_to_int64(char *str)
{
    return strtoll(str, NULL, 10);
}

int32_t string_to_int32(char *str)
{
    return strtol(str, NULL, 10);
}

int16_t string_to_int16(char *str)
{
    return (int16_t)string_to_int32(str);
}

int8_t string_to_int8(char *str)
{
    return (int8_t)string_to_int32(str);
}

bool string_to_bool(char *str)
{
    if (strcmp(str, "true") == 0) {
        return true;
    } else if (strcmp(str, "false") == 0) {
        return false;
    } else {
        return string_to_uint8(str);
    }
}
