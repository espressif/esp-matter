// Copyright 2022 The Pigweed Authors
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

#include "gtest/gtest.h"
#include "pw_log/log.h"
#include "pw_sys_io/sys_io.h"
#include "pw_unit_test/logging_event_handler.h"

int main() {
  pw::unit_test::LoggingEventHandler handler;
  pw::unit_test::RegisterEventHandler(&handler);
  while (true) {
    std::byte b;
    if (pw::sys_io::ReadByte(&b).ok() && b == std::byte(' ')) {
      RUN_ALL_TESTS();
    } else {
      PW_LOG_INFO("Press space to start test");
    }
  }
  return 0;
}
