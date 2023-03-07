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

#include "pw_pw_cpu_exception_cortex_m/util.h"

#include "gtest/gtest.h"
#include "pw_pw_cpu_exception_cortex_m/cpu_state.h"

namespace pw::pw_cpu_exception::cortex_m {
namespace {

TEST(ActiveProcessorMode, HandlerModeMain) {
  pw_cpu_exception_State cpu_state = {};
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b0001 - 0x1 Handler mode Main
  cpu_state.extended.exc_return = 0b0001;
  EXPECT_EQ(ActiveProcessorMode(cpu_state), ProcessorMode::HandlerMode);
}

TEST(ActiveProcessorMode, ThreadModeMain) {
  pw_cpu_exception_State cpu_state = {};
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b1001 - 0x9 Thread mode Main
  cpu_state.extended.exc_return = 0b1001;
  EXPECT_EQ(ActiveProcessorMode(cpu_state), ProcessorMode::ThreadMode);
}

TEST(ActiveProcessorMode, ThreadModeProcess) {
  pw_cpu_exception_State cpu_state = {};
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b1101 - 0xD Thread mode Process
  cpu_state.extended.exc_return = 0b1001;
  EXPECT_EQ(ActiveProcessorMode(cpu_state), ProcessorMode::ThreadMode);
}

TEST(MainStackActive, HandlerModeMain) {
  pw_cpu_exception_State cpu_state = {};
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b0001 - 0x1 Handler mode Main
  cpu_state.extended.exc_return = 0b0001;
  EXPECT_TRUE(MainStackActive(cpu_state));
}

TEST(MainStackActive, ThreadModeMain) {
  pw_cpu_exception_State cpu_state = {};
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b1001 - 0x9 Thread mode Main
  cpu_state.extended.exc_return = 0b1001;
  EXPECT_TRUE(MainStackActive(cpu_state));
}

TEST(MainStackActive, ThreadModeProcess) {
  pw_cpu_exception_State cpu_state = {};
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b1101 - 0xD Thread mode Process
  cpu_state.extended.exc_return = 0b1001;
  EXPECT_FALSE(MainStackActive(cpu_state));
}

TEST(ProcessStackActive, HandlerModeMain) {
  pw_cpu_exception_State cpu_state = {};
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b0001 - 0x1 Handler mode Main
  cpu_state.extended.exc_return = 0b0001;
  EXPECT_FALSE(ProcessStackActive(cpu_state));
}

TEST(ProcessStackActive, ThreadModeMain) {
  pw_cpu_exception_State cpu_state = {};
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b1001 - 0x9 Thread mode Main
  cpu_state.extended.exc_return = 0b1001;
  EXPECT_FALSE(ProcessStackActive(cpu_state));
}

TEST(ProcessStackActive, ThreadModeProcess) {
  pw_cpu_exception_State cpu_state = {};
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b1101 - 0xD Thread mode Process
  cpu_state.extended.exc_return = 0b1001;
  EXPECT_TRUE(ProcessStackActive(cpu_state));
}

}  // namespace
}  // namespace pw::pw_cpu_exception::cortex_m
