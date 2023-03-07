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

#include "pw_toolchain/no_destructor.h"

#include "gtest/gtest.h"
#include "pw_assert/check.h"

namespace pw {
namespace {

class HasADestructor {
 public:
  HasADestructor(bool& destructor_called_flag)
      : destructor_called_(destructor_called_flag) {
    destructor_called_ = false;
  }

  ~HasADestructor() { destructor_called_ = true; }

 private:
  bool& destructor_called_;
};

class CrashInDestructor {
 public:
  const CrashInDestructor* MyAddress() const { return this; }

  int some_value = 0;

 private:
  ~CrashInDestructor() { PW_CRASH("This destructor should never execute!"); }
};

TEST(NoDestructor, ShouldNotCallDestructor) {
  bool destructor_called = false;

  { HasADestructor should_be_destroyed(destructor_called); }

  EXPECT_TRUE(destructor_called);

  { NoDestructor<HasADestructor> should_not_be_destroyed(destructor_called); }

  EXPECT_FALSE(destructor_called);
}

TEST(NoDestructor, MemberAccess) {
  NoDestructor<CrashInDestructor> no_destructor;

  no_destructor->some_value = 123;
  EXPECT_EQ(123, (*no_destructor).some_value);
  EXPECT_EQ(no_destructor.operator->(), no_destructor->MyAddress());
}

TEST(NoDestructor, TrivialType) {
  NoDestructor<int> no_destructor;

  EXPECT_EQ(*no_destructor, 0);
  *no_destructor = 123;
  EXPECT_EQ(*no_destructor, 123);
}

TEST(NoDestructor, FunctionStatic) {
  static NoDestructor<CrashInDestructor> function_static_no_destructor;
}

NoDestructor<CrashInDestructor> global_no_destructor;

static_assert(!std::is_trivially_destructible<CrashInDestructor>::value,
              "Type should not be trivially destructible");
static_assert(
    std::is_trivially_destructible<NoDestructor<CrashInDestructor>>::value,
    "Wrapper should be trivially destructible");

}  // namespace
}  // namespace pw
