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

#include <cstring>

#include "pw_assert/check.h"
#include "pw_bloat/bloat_this_binary.h"
#include "pw_kvs/flash_test_partition.h"
#include "pw_log/log.h"

pw::kvs::FlashPartition& test_partition = pw::kvs::FlashTestPartition();

char working_buffer[256];
volatile bool is_set;

volatile bool is_erased;

int volatile* unoptimizable;

int main() {
  pw::bloat::BloatThisBinary();

  // Ensure we are paying the cost for log and assert.
  PW_CHECK_INT_GE(*unoptimizable, 0, "Ensure this CHECK logic stays");
  PW_LOG_INFO("We care about optimizing: %d", *unoptimizable);

  void* result =
      std::memset((void*)working_buffer, 0x55, sizeof(working_buffer));
  is_set = (result != nullptr);

  test_partition.Erase()
      .IgnoreError();  // TODO(b/242598609): Handle Status properly

  std::memset((void*)working_buffer, 0x55, sizeof(working_buffer));

  test_partition.Write(0, pw::as_bytes(pw::span(working_buffer)))
      .IgnoreError();  // TODO(b/242598609): Handle Status properly

  bool tmp_bool;
  test_partition.IsErased(&tmp_bool)
      .IgnoreError();  // TODO(b/242598609): Handle Status properly
  is_erased = tmp_bool;

  test_partition.Read(0, as_writable_bytes(pw::span(working_buffer)))
      .IgnoreError();  // TODO(b/242598609): Handle Status properly

  return 0;
}
