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

#include <stdint.h>

// The following declares a number of pthread APIs and data types needed to
// compile chromium verifier. But our port doesn't have any referrence to
// them. Thus just declarations are sufficient. The header will only be
// used for targets without a native POSIX thread implementation.

typedef unsigned long int pthread_t;
typedef uint32_t pthread_key_t;
typedef uint32_t pthread_mutex_t;

#if __cplusplus
extern "C" {
#endif

int pthread_mutex_lock(pthread_mutex_t* __mutex);
int pthread_mutex_trylock(pthread_mutex_t*);
int pthread_mutex_unlock(pthread_mutex_t*);

#if __cplusplus
}
#endif
