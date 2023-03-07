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

// The log level to use for this module. Logs below this level are omitted.
#ifndef PW_THREAD_CONFIG_LOG_LEVEL
#define PW_THREAD_CONFIG_LOG_LEVEL PW_LOG_LEVEL_DEBUG
#endif  // PW_THREAD_CONFIG_LOG_LEVEL

// The max number of threads to use by default for thread snapshot service.
#ifndef PW_THREAD_MAXIMUM_THREADS
#define PW_THREAD_MAXIMUM_THREADS 10
#endif  // PW_THREAD_MAXIMUM_THREADS

// The max number of threads to bundle by default for thread snapshot service.
#ifndef PW_THREAD_NUM_BUNDLED_THREADS
#define PW_THREAD_NUM_BUNDLED_THREADS 3
#endif  // PW_THREAD_MAXIMUM_THREADS