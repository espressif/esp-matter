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

#include "pw_preprocessor/util.h"

#include <cstdint>

#include "gtest/gtest.h"

namespace pw {
namespace {

TEST(Macros, ArraySize) {
  uint32_t hello_there[123];
  static_assert(PW_ARRAY_SIZE(hello_there) == 123);

  char characters[500];
  static_assert(PW_ARRAY_SIZE(characters) == 500);
  static_assert(PW_ARRAY_SIZE("2345") == 5);

  struct Object {
    int a;
    uint64_t array[7];
  };
  Object objects[9];

  static_assert(PW_ARRAY_SIZE(objects) == 9);
  static_assert(PW_ARRAY_SIZE(Object::array) == 7);
  static_assert(PW_ARRAY_SIZE(objects[1].array) == 7);
}

#define HELLO hello
#define WORLD WORLD_IMPL()
#define WORLD_IMPL() WORLD !

TEST(Macros, Stringify) {
  EXPECT_STREQ("", PW_STRINGIFY());
  EXPECT_STREQ("> _ <", PW_STRINGIFY(> _ <));
  EXPECT_STREQ("hello WORLD !", PW_STRINGIFY(HELLO WORLD));
  EXPECT_STREQ("hello, WORLD !", PW_STRINGIFY(HELLO, WORLD));
  EXPECT_STREQ("a, b, c, hello, WORLD ! 2",
               PW_STRINGIFY(a, b, c, HELLO, WORLD 2));
}

}  // namespace
}  // namespace pw
