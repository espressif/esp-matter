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

#include "pw_thread/thread_info.h"

#include <optional>

#include "gtest/gtest.h"
#include "pw_span/span.h"

namespace pw::thread {
namespace {

TEST(ThreadInfo, ThreadName) {
  ThreadInfo thread_info;
  // Getter.
  EXPECT_EQ(thread_info.thread_name(), std::nullopt);
  char buffer[] = "hello, world";
  span<char, 13> name_string(buffer);
  span<const std::byte> name =
      span(reinterpret_cast<const std::byte*>(name_string.data()), 13);
  // Setter.
  thread_info.set_thread_name(name);
  EXPECT_EQ(thread_info.thread_name().value().data(), name.data());
  // Clear.
  thread_info.clear_thread_name();
  EXPECT_EQ(thread_info.thread_name(), std::nullopt);
}

TEST(ThreadInfo, StackLowAddr) {
  ThreadInfo thread_info;
  // Getter.
  EXPECT_EQ(thread_info.stack_low_addr(), std::nullopt);
  const unsigned int* null_addr = nullptr;

  const unsigned int example_addr = 12345678u;
  const unsigned int* addr = &example_addr;
  // Setter.
  thread_info.set_stack_low_addr(reinterpret_cast<uintptr_t>(null_addr));
  EXPECT_EQ(thread_info.stack_low_addr(),
            reinterpret_cast<uintptr_t>(null_addr));
  thread_info.set_stack_low_addr(reinterpret_cast<uintptr_t>(addr));
  EXPECT_EQ(thread_info.stack_low_addr(), reinterpret_cast<uintptr_t>(addr));
  // Clear.
  thread_info.clear_stack_low_addr();
  EXPECT_EQ(thread_info.stack_low_addr(), std::nullopt);
}

TEST(ThreadInfo, StackHighAddr) {
  ThreadInfo thread_info;
  // Getter.
  EXPECT_EQ(thread_info.stack_high_addr(), std::nullopt);
  const unsigned int* null_addr = nullptr;

  const unsigned int example_addr = 12345678u;
  const unsigned int* addr = &example_addr;
  // Setter.
  thread_info.set_stack_high_addr(reinterpret_cast<uintptr_t>(null_addr));
  EXPECT_EQ(thread_info.stack_high_addr(),
            reinterpret_cast<uintptr_t>(null_addr));
  thread_info.set_stack_high_addr(reinterpret_cast<uintptr_t>(addr));
  EXPECT_EQ(thread_info.stack_high_addr(), reinterpret_cast<uintptr_t>(addr));
  // Clear.
  thread_info.clear_stack_high_addr();
  EXPECT_EQ(thread_info.stack_high_addr(), std::nullopt);
}

TEST(ThreadInfo, PeakAddr) {
  ThreadInfo thread_info;
  // Getter.
  EXPECT_EQ(thread_info.stack_peak_addr(), std::nullopt);
  const unsigned int* null_addr = nullptr;

  const unsigned int example_addr = 12345678u;
  const unsigned int* addr = &example_addr;
  // Setter.
  thread_info.set_stack_peak_addr(reinterpret_cast<uintptr_t>(null_addr));
  EXPECT_EQ(thread_info.stack_peak_addr(),
            reinterpret_cast<uintptr_t>(null_addr));
  thread_info.set_stack_peak_addr(reinterpret_cast<uintptr_t>(addr));
  EXPECT_EQ(thread_info.stack_peak_addr(), reinterpret_cast<uintptr_t>(addr));
  // Clear.
  thread_info.clear_stack_peak_addr();
  EXPECT_EQ(thread_info.stack_peak_addr(), std::nullopt);
}

}  // namespace
}  // namespace pw::thread
