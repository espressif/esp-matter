// Copyright 2019 The Pigweed Authors
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

#include "pw_unit_test/simple_printing_event_handler.h"

#include <cstdarg>
#include <cstdio>

namespace pw::unit_test {

void SimplePrintingEventHandler::WriteLine(const char* format, ...) {
  va_list args;

  va_start(args, format);
  std::vsnprintf(buffer_, sizeof(buffer_), format, args);
  va_end(args);

  write_(buffer_, true);
}

}  // namespace pw::unit_test
