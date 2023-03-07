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

// This override header includes the main tokenized logging header and defines
// the PW_LOG macro as the tokenized logging macro.
#pragma once

#include "pw_log_tokenized/config.h"
#include "pw_log_tokenized/log_tokenized.h"

#define PW_HANDLE_LOG PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD

#define PW_LOG_FLAG_BITS PW_LOG_TOKENIZED_FLAG_BITS
