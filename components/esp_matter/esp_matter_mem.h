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

/** ESP Matter Memory Allocations
 * @param[in] n number of elements to be allocated
 * @param[in] size size of elements to be allocated
 */
void *esp_matter_mem_calloc(size_t n, size_t size);

/** ESP Matter Free Memory
 * @param[in] ptr pointer to the memory to be freed.
 */
void esp_matter_mem_free(void *ptr);
