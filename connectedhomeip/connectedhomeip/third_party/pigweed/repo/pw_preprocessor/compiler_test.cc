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

#include "pw_preprocessor/compiler.h"

#include <cstdint>

#include "gtest/gtest.h"

namespace pw::preprocessor {
namespace {

PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Wunused-variable");

int this_variable_is_unused;

PW_MODIFY_DIAGNOSTICS_POP();

class Foo {
  PW_MODIFY_DIAGNOSTICS_PUSH();
  PW_MODIFY_DIAGNOSTIC(ignored, "-Wunused");

  int this_field_is_unused;

  PW_MODIFY_DIAGNOSTICS_POP();
};

TEST(CompilerMacros, ModifyDiagnostics) {
  PW_MODIFY_DIAGNOSTICS_PUSH();
  PW_MODIFY_DIAGNOSTIC(ignored, "-Wunused-variable");

  int this_variable_also_is_unused;

  PW_MODIFY_DIAGNOSTICS_POP();
}

}  // namespace
}  // namespace pw::preprocessor
