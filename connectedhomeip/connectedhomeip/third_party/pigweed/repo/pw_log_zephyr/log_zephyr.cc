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

#include <zephyr/logging/log.h>

#include "pw_log_zephyr/config.h"

// Register the Zephyr logging module.
// This must be done exactly ONCE. The header provided in public_overrides
// provides the call to LOG_MODULE_DECLARE for each consumer of this backend
// which will allow the use of this module.
LOG_MODULE_REGISTER(PW_LOG_ZEPHYR_MODULE_NAME);
