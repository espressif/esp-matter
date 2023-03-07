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

#include "FreeRTOS.h"
#include "task.h"

// The backend is expected to provide this header which defines the tskTCB
// strut which matches FreeRTOS's definition of a task control block which is
// opaquely presented via the public API as the StaticTask_t.
#include "pw_thread_freertos_backend/freertos_tsktcb.h"

static_assert(sizeof(tskTCB) == sizeof(StaticTask_t),
              "The tskTCB mirror of the real task TCB doesn't match FreeRTOS");
