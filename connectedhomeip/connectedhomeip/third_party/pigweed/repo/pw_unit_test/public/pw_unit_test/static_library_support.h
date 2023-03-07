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
#pragma once

#include "gtest/gtest.h"

// Ensures tests in a static library are linked and executed. Provide the test
// suite name and test name for one test in the file linked into a static
// library. Any test in the file may be used, but it is recommended to use the
// first for consistency. The test must be in a static library that is a
// dependency of this target. Referring to a test that does not exist causes a
// linker error.
//
// The linker usually ignores tests linked through a static library. This is
// because test registration relies on the test instance's static constructor
// adding itself to a global list of tests. When linking against a static
// library, static constructors in an object file will be ignored unless at
// least one entity in that object file is linked.
//
// This macro works by passing the internal TestInfo instance to a constructor
// defined in a source file. This guarantees that the TestInfo instance is
// referenced, so the linker will link it and the other tests in that file.
#define PW_UNIT_TEST_LINK_FILE_CONTAINING_TEST(suite, name) \
  _PW_UNIT_TEST_LINK_TESTS(_pw_unit_test_Info_##suite##_##name)

#define _PW_UNIT_TEST_LINK_TESTS(info)                              \
  extern "C" {                                                      \
                                                                    \
  extern ::pw::unit_test::internal::TestInfo info;                  \
                                                                    \
  [[maybe_unused]] const ::pw::unit_test::internal::ReferToTestInfo \
      _pw_unit_test_reference_to_ensure_link_##info(info);          \
                                                                    \
  } /* extern "C" */                                                \
                                                                    \
  static_assert(true, "Macros must end with a semicolon")

namespace pw::unit_test::internal {

// Refers to the TestInfo to ensure it is linked in.
class ReferToTestInfo {
 public:
  explicit ReferToTestInfo(const TestInfo& info);
};

}  // namespace pw::unit_test::internal
