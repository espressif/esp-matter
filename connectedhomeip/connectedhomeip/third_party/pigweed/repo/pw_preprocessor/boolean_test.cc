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
//
// All of these tests are static asserts. If the test compiles, it has already
// passed. The TEST functions are used for organization only.
#include "pw_preprocessor/boolean.h"

#include "gtest/gtest.h"

namespace pw {
namespace {

#define ONE 1
#define ZERO() 0

TEST(BooleanMacros, And) {
  static_assert(PW_AND(ZERO(), 0) == 0);
  static_assert(PW_AND(ZERO(), ONE) == 0);
  static_assert(PW_AND(1, 0) == 0);
  static_assert(PW_AND(ONE, PW_NOT(ZERO())) == 1);
}

TEST(BooleanMacros, Or) {
  static_assert(PW_OR(ZERO(), 0) == 0);
  static_assert(PW_OR(ZERO(), ONE) == 1);
  static_assert(PW_OR(1, 0) == 1);
  static_assert(PW_OR(ONE, PW_NOT(ZERO())) == 1);
}

TEST(BooleanMacros, Not) {
  static_assert(PW_NOT(0) == 1);
  static_assert(PW_NOT(1) == 0);
  static_assert(PW_NOT(ONE) == 0);
  static_assert(PW_NOT(ZERO()) == 1);
}

TEST(BooleanMacros, Xor) {
  static_assert(PW_XOR(ZERO(), 0) == 0);
  static_assert(PW_XOR(ZERO(), ONE) == 1);
  static_assert(PW_XOR(1, 0) == 1);
  static_assert(PW_XOR(ONE, PW_NOT(ZERO())) == 0);
}

TEST(BooleanMacros, Nand) {
  static_assert(PW_NAND(ZERO(), 0) == 1);
  static_assert(PW_NAND(ZERO(), ONE) == 1);
  static_assert(PW_NAND(1, 0) == 1);
  static_assert(PW_NAND(ONE, PW_NOT(ZERO())) == 0);
}

TEST(BooleanMacros, Nor) {
  static_assert(PW_NOR(ZERO(), 0) == 1);
  static_assert(PW_NOR(ZERO(), ONE) == 0);
  static_assert(PW_NOR(1, 0) == 0);
  static_assert(PW_NOR(ONE, PW_NOT(ZERO())) == 0);
}

TEST(BooleanMacros, Xnor) {
  static_assert(PW_XNOR(ZERO(), 0) == 1);
  static_assert(PW_XNOR(ZERO(), ONE) == 0);
  static_assert(PW_XNOR(1, 0) == 0);
  static_assert(PW_XNOR(ONE, PW_NOT(ZERO())) == 1);
}

TEST(BooleanMacros, Nested) {
  static_assert(PW_AND(1, PW_AND(PW_OR(ZERO(), ONE), PW_XOR(ONE, 0))) == 1);
}

}  // namespace
}  // namespace pw
