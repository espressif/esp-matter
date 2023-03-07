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

#include "pw_status/status_with_size.h"

#include "gtest/gtest.h"

namespace pw {
namespace {

static_assert(StatusWithSize::max_size() ==
                  (static_cast<size_t>(1) << (sizeof(size_t) * 8 - 5)) - 1,
              "max_size() should use all but the top 5 bits of a size_t.");

TEST(StatusWithSize, Default) {
  StatusWithSize result;
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(Status(), result.status());
  EXPECT_EQ(0u, result.size());
}

TEST(StatusWithSize, ConstructWithSize) {
  StatusWithSize result = StatusWithSize(456);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(Status(), result.status());
  EXPECT_EQ(456u, result.size());
}

TEST(StatusWithSize, ConstructWithError) {
  StatusWithSize result(Status::ResourceExhausted(), 123);
  EXPECT_FALSE(result.ok());
  EXPECT_EQ(Status::ResourceExhausted(), result.status());
  EXPECT_EQ(123u, result.size());
}

TEST(StatusWithSize, ConstructWithOkAndSize) {
  StatusWithSize result(Status(), 99);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(Status(), result.status());
  EXPECT_EQ(99u, result.size());
}

TEST(StatusWithSize, ConstructFromConstant) {
  StatusWithSize result(StatusWithSize::AlreadyExists());

  EXPECT_EQ(Status::AlreadyExists(), result.status());
  EXPECT_EQ(0u, result.size());

  result = StatusWithSize::NotFound();

  EXPECT_EQ(Status::NotFound(), result.status());
  EXPECT_EQ(0u, result.size());
}

TEST(StatusWithSize, AllStatusValues_ZeroSize) {
  for (int i = 0; i < 32; ++i) {
    StatusWithSize result(static_cast<Status::Code>(i), 0);
    EXPECT_EQ(result.ok(), i == 0);
    EXPECT_EQ(i, static_cast<int>(result.status().code()));
    EXPECT_EQ(0u, result.size());
  }
}

TEST(StatusWithSize, AllStatusValues_SameSize) {
  for (int i = 0; i < 32; ++i) {
    StatusWithSize result(static_cast<Status::Code>(i), i);
    EXPECT_EQ(result.ok(), i == 0);
    EXPECT_EQ(i, static_cast<int>(result.status().code()));
    EXPECT_EQ(static_cast<size_t>(i), result.size());
  }
}

TEST(StatusWithSize, AllStatusValues_MaxSize) {
  for (int i = 0; i < 32; ++i) {
    StatusWithSize result(static_cast<Status::Code>(i),
                          StatusWithSize::max_size());
    EXPECT_EQ(result.ok(), i == 0);
    EXPECT_EQ(i, static_cast<int>(result.status().code()));
    EXPECT_EQ(result.max_size(), result.size());
  }
}

TEST(StatusWithSize, Assignment) {
  StatusWithSize result = StatusWithSize(Status::Internal(), 0x123);
  EXPECT_FALSE(result.ok());
  EXPECT_EQ(Status::Internal(), result.status());
  EXPECT_EQ(0x123u, result.size());

  result = StatusWithSize(300);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(Status(), result.status());
  EXPECT_EQ(300u, result.size());
}

TEST(StatusWithSize, Constexpr) {
  constexpr StatusWithSize result(Status::Cancelled(), 1234);
  static_assert(Status::Cancelled() == result.status());
  static_assert(!result.ok());
  static_assert(1234u == result.size());
}

TEST(StatusWithSize, Functions_Status) {
  // clang-format off
  static_assert(StatusWithSize(0).status() == Status());
  static_assert(StatusWithSize::Cancelled().status() == Status::Cancelled());
  static_assert(StatusWithSize::Unknown().status() == Status::Unknown());
  static_assert(StatusWithSize::InvalidArgument().status() == Status::InvalidArgument());
  static_assert(StatusWithSize::DeadlineExceeded().status() == Status::DeadlineExceeded());
  static_assert(StatusWithSize::NotFound().status() == Status::NotFound());
  static_assert(StatusWithSize::AlreadyExists().status() == Status::AlreadyExists());
  static_assert(StatusWithSize::PermissionDenied().status() == Status::PermissionDenied());
  static_assert(StatusWithSize::Unauthenticated().status() == Status::Unauthenticated());
  static_assert(StatusWithSize::ResourceExhausted().status() == Status::ResourceExhausted());
  static_assert(StatusWithSize::FailedPrecondition().status() == Status::FailedPrecondition());
  static_assert(StatusWithSize::Aborted().status() == Status::Aborted());
  static_assert(StatusWithSize::OutOfRange().status() == Status::OutOfRange());
  static_assert(StatusWithSize::Unimplemented().status() == Status::Unimplemented());
  static_assert(StatusWithSize::Internal().status() == Status::Internal());
  static_assert(StatusWithSize::Unavailable().status() == Status::Unavailable());
  static_assert(StatusWithSize::DataLoss().status() == Status::DataLoss());

  static_assert(StatusWithSize(123).status() == Status());
  static_assert(StatusWithSize::Cancelled(123).status() == Status::Cancelled());
  static_assert(StatusWithSize::Unknown(123).status() == Status::Unknown());
  static_assert(StatusWithSize::InvalidArgument(123).status() == Status::InvalidArgument());
  static_assert(StatusWithSize::DeadlineExceeded(123).status() == Status::DeadlineExceeded());
  static_assert(StatusWithSize::NotFound(123).status() == Status::NotFound());
  static_assert(StatusWithSize::AlreadyExists(123).status() == Status::AlreadyExists());
  static_assert(StatusWithSize::PermissionDenied(123).status() == Status::PermissionDenied());
  static_assert(StatusWithSize::Unauthenticated(123).status() == Status::Unauthenticated());
  static_assert(StatusWithSize::ResourceExhausted(123).status() == Status::ResourceExhausted());
  static_assert(StatusWithSize::FailedPrecondition(123).status() == Status::FailedPrecondition());
  static_assert(StatusWithSize::Aborted(123).status() == Status::Aborted());
  static_assert(StatusWithSize::OutOfRange(123).status() == Status::OutOfRange());
  static_assert(StatusWithSize::Unimplemented(123).status() == Status::Unimplemented());
  static_assert(StatusWithSize::Internal(123).status() == Status::Internal());
  static_assert(StatusWithSize::Unavailable(123).status() == Status::Unavailable());
  static_assert(StatusWithSize::DataLoss(123).status() == Status::DataLoss());
  // clang-format on
}

TEST(StatusWithSize, Functions_DefaultSize) {
  static_assert(StatusWithSize::Cancelled().size() == 0u);
  static_assert(StatusWithSize::Unknown().size() == 0u);
  static_assert(StatusWithSize::InvalidArgument().size() == 0u);
  static_assert(StatusWithSize::DeadlineExceeded().size() == 0u);
  static_assert(StatusWithSize::NotFound().size() == 0u);
  static_assert(StatusWithSize::AlreadyExists().size() == 0u);
  static_assert(StatusWithSize::PermissionDenied().size() == 0u);
  static_assert(StatusWithSize::Unauthenticated().size() == 0u);
  static_assert(StatusWithSize::ResourceExhausted().size() == 0u);
  static_assert(StatusWithSize::FailedPrecondition().size() == 0u);
  static_assert(StatusWithSize::Aborted().size() == 0u);
  static_assert(StatusWithSize::OutOfRange().size() == 0u);
  static_assert(StatusWithSize::Unimplemented().size() == 0u);
  static_assert(StatusWithSize::Internal().size() == 0u);
  static_assert(StatusWithSize::Unavailable().size() == 0u);
  static_assert(StatusWithSize::DataLoss().size() == 0u);
}

TEST(StatusWithSize, Functions_SpecifiedSize) {
  static_assert(StatusWithSize(123).size() == 123u);
  static_assert(StatusWithSize::Cancelled(123).size() == 123u);
  static_assert(StatusWithSize::Unknown(123).size() == 123u);
  static_assert(StatusWithSize::InvalidArgument(123).size() == 123u);
  static_assert(StatusWithSize::DeadlineExceeded(123).size() == 123u);
  static_assert(StatusWithSize::NotFound(123).size() == 123u);
  static_assert(StatusWithSize::AlreadyExists(123).size() == 123u);
  static_assert(StatusWithSize::PermissionDenied(123).size() == 123u);
  static_assert(StatusWithSize::Unauthenticated(123).size() == 123u);
  static_assert(StatusWithSize::ResourceExhausted(123).size() == 123u);
  static_assert(StatusWithSize::FailedPrecondition(123).size() == 123u);
  static_assert(StatusWithSize::Aborted(123).size() == 123u);
  static_assert(StatusWithSize::OutOfRange(123).size() == 123u);
  static_assert(StatusWithSize::Unimplemented(123).size() == 123u);
  static_assert(StatusWithSize::Internal(123).size() == 123u);
  static_assert(StatusWithSize::Unavailable(123).size() == 123u);
  static_assert(StatusWithSize::DataLoss(123).size() == 123u);
}

TEST(StatusWithSize, IsError) {
  static_assert(StatusWithSize::Cancelled().IsCancelled());
  static_assert(StatusWithSize::Unknown().IsUnknown());
  static_assert(StatusWithSize::InvalidArgument().IsInvalidArgument());
  static_assert(StatusWithSize::DeadlineExceeded().IsDeadlineExceeded());
  static_assert(StatusWithSize::NotFound().IsNotFound());
  static_assert(StatusWithSize::AlreadyExists().IsAlreadyExists());
  static_assert(StatusWithSize::PermissionDenied().IsPermissionDenied());
  static_assert(StatusWithSize::ResourceExhausted().IsResourceExhausted());
  static_assert(StatusWithSize::FailedPrecondition().IsFailedPrecondition());
  static_assert(StatusWithSize::Aborted().IsAborted());
  static_assert(StatusWithSize::OutOfRange().IsOutOfRange());
  static_assert(StatusWithSize::Unimplemented().IsUnimplemented());
  static_assert(StatusWithSize::Internal().IsInternal());
  static_assert(StatusWithSize::Unavailable().IsUnavailable());
  static_assert(StatusWithSize::DataLoss().IsDataLoss());
  static_assert(StatusWithSize::Unauthenticated().IsUnauthenticated());
}

TEST(StatusWithSize, IsNotError) {
  static_assert(!StatusWithSize(0).IsCancelled());
  static_assert(!StatusWithSize(0).IsUnknown());
  static_assert(!StatusWithSize(0).IsInvalidArgument());
  static_assert(!StatusWithSize(0).IsDeadlineExceeded());
  static_assert(!StatusWithSize(0).IsNotFound());
  static_assert(!StatusWithSize(0).IsAlreadyExists());
  static_assert(!StatusWithSize(0).IsPermissionDenied());
  static_assert(!StatusWithSize(0).IsUnauthenticated());
  static_assert(!StatusWithSize(0).IsResourceExhausted());
  static_assert(!StatusWithSize(0).IsFailedPrecondition());
  static_assert(!StatusWithSize(0).IsAborted());
  static_assert(!StatusWithSize(0).IsOutOfRange());
  static_assert(!StatusWithSize(0).IsUnimplemented());
  static_assert(!StatusWithSize(0).IsInternal());
  static_assert(!StatusWithSize(0).IsUnavailable());
  static_assert(!StatusWithSize(0).IsDataLoss());
}
}  // namespace
}  // namespace pw
