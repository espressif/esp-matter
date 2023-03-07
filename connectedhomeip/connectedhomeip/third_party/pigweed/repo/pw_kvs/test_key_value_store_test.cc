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

#include "pw_kvs/test_key_value_store.h"

#include "gtest/gtest.h"
#include "pw_kvs/key_value_store.h"
#include "pw_status/status.h"
#include "pw_sync/borrow.h"

namespace pw::kvs {
namespace {

// Simple test to verify that the TestKvs() does basic function.
TEST(TestKvs, PutGetValue) {
  sync::BorrowedPointer<KeyValueStore> kvs = TestKvs().acquire();
  ASSERT_EQ(OkStatus(), kvs->Put("key", uint32_t(0xfeedbeef)));

  uint32_t value = 0;
  EXPECT_EQ(OkStatus(), kvs->Get("key", &value));
  EXPECT_EQ(uint32_t(0xfeedbeef), value);
}

}  // namespace
}  // namespace pw::kvs
