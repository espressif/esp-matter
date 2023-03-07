// Copyright 2020 The Pigweed Authors
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

#include <stdbool.h>
#include <stddef.h>

#include "pw_log_null/log_null.h"

#define PW_LOG_MODULE_NAME "c test!"

static int global;

static int IncrementGlobal(void) { return ++global; }

bool CTest(void) {
  PW_HANDLE_LOG(1, 2, "3");
  PW_HANDLE_LOG(1, 2, "whoa");
  PW_HANDLE_LOG(1, 2, "%s", "hello");
  PW_HANDLE_LOG(1, 2, "%d + %s == %p", 1, "two", NULL);

  global = 0;
  bool local = true;

  PW_HANDLE_LOG(1,
                2,
                "You are number%s %d!",
                (local = false) ? "" : " not",
                IncrementGlobal());

  return global == 1 && !local;
}
