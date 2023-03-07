// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
#pragma once

// Because FreeRTOS.h includes FreeRTOSConfig.h inside of an extern "C" we must
// wrap the include of check in an extern "C++" for things to work.
#ifdef __cplusplus
extern "C++" {
#endif  // __cplusplus

#if defined(PW_THIRD_PARTY_FREERTOS_NO_STATICS) && \
    PW_THIRD_PARTY_FREERTOS_NO_STATICS == 1
#include "pw_assert/assert.h"
#define configASSERT PW_ASSERT
#else
#include "pw_assert/check.h"
#define configASSERT PW_CHECK
#endif  // PW_THIRD_PARTY_FREERTOS_NO_STATICS == 1

#ifdef __cplusplus
}  // extern "C++"
#endif  // __cplusplus
