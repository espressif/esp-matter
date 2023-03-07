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

#include "pw_stream/seek.h"

#include "gtest/gtest.h"

namespace pw::stream {
namespace {

TEST(ResolveSeekOffset, Beginning) {
  EXPECT_EQ(ResolveSeekOffset(123, Stream::kBeginning, 0, 0), 123);
}

TEST(ResolveSeekOffset, Current) {
  EXPECT_EQ(ResolveSeekOffset(-10, Stream::kCurrent, 0, 100), 90);
}

TEST(ResolveSeekOffset, End) {
  EXPECT_EQ(ResolveSeekOffset(-10, Stream::kEnd, 100, 0), 90);
}

TEST(CalculateSeek, Beginning) {
  size_t position = 2;
  EXPECT_EQ(OkStatus(), CalculateSeek(35, Stream::kBeginning, 100, position));
  EXPECT_EQ(position, 35u);
}

TEST(CalculateSeek, Beginning_SeekToBeginning) {
  size_t position = 99;
  EXPECT_EQ(OkStatus(), CalculateSeek(0, Stream::kBeginning, 100, position));
  EXPECT_EQ(position, 0u);
}

TEST(CalculateSeek, Beginning_SeekToEnd) {
  size_t position = 0;
  EXPECT_EQ(OkStatus(), CalculateSeek(100, Stream::kBeginning, 100, position));
  EXPECT_EQ(position, 100u);
}

TEST(CalculateSeek, Beginning_SeekNegative_OutOfRange) {
  size_t position = 2;
  EXPECT_EQ(Status::OutOfRange(),
            CalculateSeek(-1, Stream::kBeginning, 100, position));
  EXPECT_EQ(position, 2u);
}

TEST(CalculateSeek, Beginning_SeekPastEnd_OutOfRange) {
  size_t position = 2;
  EXPECT_EQ(Status::OutOfRange(),
            CalculateSeek(101, Stream::kBeginning, 100, position));
  EXPECT_EQ(position, 2u);
}

TEST(CalculateSeek, Current) {
  size_t position = 100;
  EXPECT_EQ(OkStatus(), CalculateSeek(1, Stream::kCurrent, 234, position));
  EXPECT_EQ(position, 101u);
}

TEST(CalculateSeek, Current_SeekToBeginning) {
  size_t position = 99;
  EXPECT_EQ(OkStatus(), CalculateSeek(-99, Stream::kCurrent, 100, position));
  EXPECT_EQ(position, 0u);
}

TEST(CalculateSeek, Current_SeekToEnd) {
  size_t position = 10;
  EXPECT_EQ(OkStatus(), CalculateSeek(90, Stream::kCurrent, 1000, position));
  EXPECT_EQ(position, 100u);
}

TEST(CalculateSeek, Current_SeekNegative_OutOfRange) {
  size_t position = 2;
  EXPECT_EQ(Status::OutOfRange(),
            CalculateSeek(-3, Stream::kCurrent, 100, position));
  EXPECT_EQ(position, 2u);
}

TEST(CalculateSeek, Current_SeekPastEnd_OutOfRange) {
  size_t position = 2;
  EXPECT_EQ(Status::OutOfRange(),
            CalculateSeek(99, Stream::kCurrent, 100, position));
  EXPECT_EQ(position, 2u);
}

TEST(CalculateSeek, End) {
  size_t position = 100;
  EXPECT_EQ(OkStatus(), CalculateSeek(-1, Stream::kEnd, 234, position));
  EXPECT_EQ(position, 233u);
}

TEST(CalculateSeek, End_SeekToBeginning) {
  size_t position = 50;
  EXPECT_EQ(OkStatus(), CalculateSeek(-100, Stream::kEnd, 100, position));
  EXPECT_EQ(position, 0u);
}

TEST(CalculateSeek, End_SeekToEnd) {
  size_t position = 10;
  EXPECT_EQ(OkStatus(), CalculateSeek(0, Stream::kEnd, 1000, position));
  EXPECT_EQ(position, 1000u);
}

TEST(CalculateSeek, End_SeekNegative_OutOfRange) {
  size_t position = 2;
  EXPECT_EQ(Status::OutOfRange(),
            CalculateSeek(-101, Stream::kEnd, 100, position));
  EXPECT_EQ(position, 2u);
}

TEST(CalculateSeek, End_SeekPastEnd_OutOfRange) {
  size_t position = 2;
  EXPECT_EQ(Status::OutOfRange(),
            CalculateSeek(1, Stream::kEnd, 100, position));
  EXPECT_EQ(position, 2u);
}

}  // namespace
}  // namespace pw::stream
