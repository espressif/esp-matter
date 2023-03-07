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

#include "pw_status/status.h"

#include "gtest/gtest.h"

namespace pw {
namespace {

constexpr Status::Code kInvalidCode = static_cast<Status::Code>(30);

TEST(Status, Default) {
  constexpr Status status;
  static_assert(status.ok());
  static_assert(Status() == status);
}

TEST(Status, ConstructWithStatusCode) {
  constexpr Status status(PW_STATUS_ABORTED);
  static_assert(status.IsAborted());
}

TEST(Status, AssignFromStatusCode) {
  Status status;
  status = PW_STATUS_INTERNAL;
  EXPECT_EQ(Status::Internal(), status);
}

TEST(Status, Ok_OkIsTrue) {
  static_assert(Status().ok());
  static_assert(Status(PW_STATUS_OK).ok());
  static_assert(OkStatus().ok());
}

TEST(Status, NotOk_OkIsFalse) {
  static_assert(!Status::DataLoss().ok());
  static_assert(!Status(kInvalidCode).ok());
}

TEST(Status, Code) {
  // clang-format off
  static_assert(PW_STATUS_OK == Status().code());
  static_assert(PW_STATUS_OK == OkStatus().code());
  static_assert(PW_STATUS_CANCELLED == Status::Cancelled().code());
  static_assert(PW_STATUS_UNKNOWN == Status::Unknown().code());
  static_assert(PW_STATUS_INVALID_ARGUMENT == Status::InvalidArgument().code());
  static_assert(PW_STATUS_DEADLINE_EXCEEDED == Status::DeadlineExceeded().code());
  static_assert(PW_STATUS_NOT_FOUND == Status::NotFound().code());
  static_assert(PW_STATUS_ALREADY_EXISTS == Status::AlreadyExists().code());
  static_assert(PW_STATUS_PERMISSION_DENIED == Status::PermissionDenied().code());
  static_assert(PW_STATUS_RESOURCE_EXHAUSTED == Status::ResourceExhausted().code());
  static_assert(PW_STATUS_FAILED_PRECONDITION == Status::FailedPrecondition().code());
  static_assert(PW_STATUS_ABORTED == Status::Aborted().code());
  static_assert(PW_STATUS_OUT_OF_RANGE == Status::OutOfRange().code());
  static_assert(PW_STATUS_UNIMPLEMENTED == Status::Unimplemented().code());
  static_assert(PW_STATUS_INTERNAL == Status::Internal().code());
  static_assert(PW_STATUS_UNAVAILABLE == Status::Unavailable().code());
  static_assert(PW_STATUS_DATA_LOSS == Status::DataLoss().code());
  static_assert(PW_STATUS_UNAUTHENTICATED == Status::Unauthenticated().code());
  // clang-format on
}

TEST(Status, EqualCodes) {
  static_assert(PW_STATUS_OK == Status());
  static_assert(PW_STATUS_OK == OkStatus());
  static_assert(PW_STATUS_CANCELLED == Status::Cancelled());
  static_assert(PW_STATUS_UNKNOWN == Status::Unknown());
  static_assert(PW_STATUS_INVALID_ARGUMENT == Status::InvalidArgument());
  static_assert(PW_STATUS_DEADLINE_EXCEEDED == Status::DeadlineExceeded());
  static_assert(PW_STATUS_NOT_FOUND == Status::NotFound());
  static_assert(PW_STATUS_ALREADY_EXISTS == Status::AlreadyExists());
  static_assert(PW_STATUS_PERMISSION_DENIED == Status::PermissionDenied());
  static_assert(PW_STATUS_RESOURCE_EXHAUSTED == Status::ResourceExhausted());
  static_assert(PW_STATUS_FAILED_PRECONDITION == Status::FailedPrecondition());
  static_assert(PW_STATUS_ABORTED == Status::Aborted());
  static_assert(PW_STATUS_OUT_OF_RANGE == Status::OutOfRange());
  static_assert(PW_STATUS_UNIMPLEMENTED == Status::Unimplemented());
  static_assert(PW_STATUS_INTERNAL == Status::Internal());
  static_assert(PW_STATUS_UNAVAILABLE == Status::Unavailable());
  static_assert(PW_STATUS_DATA_LOSS == Status::DataLoss());
  static_assert(PW_STATUS_UNAUTHENTICATED == Status::Unauthenticated());
}

TEST(Status, IsError) {
  static_assert(Status::Cancelled().IsCancelled());
  static_assert(Status::Unknown().IsUnknown());
  static_assert(Status::InvalidArgument().IsInvalidArgument());
  static_assert(Status::DeadlineExceeded().IsDeadlineExceeded());
  static_assert(Status::NotFound().IsNotFound());
  static_assert(Status::AlreadyExists().IsAlreadyExists());
  static_assert(Status::PermissionDenied().IsPermissionDenied());
  static_assert(Status::ResourceExhausted().IsResourceExhausted());
  static_assert(Status::FailedPrecondition().IsFailedPrecondition());
  static_assert(Status::Aborted().IsAborted());
  static_assert(Status::OutOfRange().IsOutOfRange());
  static_assert(Status::Unimplemented().IsUnimplemented());
  static_assert(Status::Internal().IsInternal());
  static_assert(Status::Unavailable().IsUnavailable());
  static_assert(Status::DataLoss().IsDataLoss());
  static_assert(Status::Unauthenticated().IsUnauthenticated());
}

TEST(Status, IsNotError) {
  static_assert(!OkStatus().IsCancelled());
  static_assert(!OkStatus().IsUnknown());
  static_assert(!OkStatus().IsInvalidArgument());
  static_assert(!OkStatus().IsDeadlineExceeded());
  static_assert(!OkStatus().IsNotFound());
  static_assert(!OkStatus().IsAlreadyExists());
  static_assert(!OkStatus().IsPermissionDenied());
  static_assert(!OkStatus().IsUnauthenticated());
  static_assert(!OkStatus().IsResourceExhausted());
  static_assert(!OkStatus().IsFailedPrecondition());
  static_assert(!OkStatus().IsAborted());
  static_assert(!OkStatus().IsOutOfRange());
  static_assert(!OkStatus().IsUnimplemented());
  static_assert(!OkStatus().IsInternal());
  static_assert(!OkStatus().IsUnavailable());
  static_assert(!OkStatus().IsDataLoss());
}

TEST(Status, Strings) {
  EXPECT_STREQ("OK", Status().str());
  EXPECT_STREQ("OK", OkStatus().str());
  EXPECT_STREQ("CANCELLED", Status::Cancelled().str());
  EXPECT_STREQ("UNKNOWN", Status::Unknown().str());
  EXPECT_STREQ("INVALID_ARGUMENT", Status::InvalidArgument().str());
  EXPECT_STREQ("DEADLINE_EXCEEDED", Status::DeadlineExceeded().str());
  EXPECT_STREQ("NOT_FOUND", Status::NotFound().str());
  EXPECT_STREQ("ALREADY_EXISTS", Status::AlreadyExists().str());
  EXPECT_STREQ("PERMISSION_DENIED", Status::PermissionDenied().str());
  EXPECT_STREQ("RESOURCE_EXHAUSTED", Status::ResourceExhausted().str());
  EXPECT_STREQ("FAILED_PRECONDITION", Status::FailedPrecondition().str());
  EXPECT_STREQ("ABORTED", Status::Aborted().str());
  EXPECT_STREQ("OUT_OF_RANGE", Status::OutOfRange().str());
  EXPECT_STREQ("UNIMPLEMENTED", Status::Unimplemented().str());
  EXPECT_STREQ("INTERNAL", Status::Internal().str());
  EXPECT_STREQ("UNAVAILABLE", Status::Unavailable().str());
  EXPECT_STREQ("DATA_LOSS", Status::DataLoss().str());
  EXPECT_STREQ("UNAUTHENTICATED", Status::Unauthenticated().str());
}

TEST(Status, UnknownString) {
  EXPECT_STREQ("INVALID STATUS", Status(kInvalidCode).str());
}

TEST(Status, Update) {
  Status status;
  status.Update(Status::Cancelled());
  EXPECT_EQ(status, Status::Cancelled());
  status.Update(OkStatus());
  EXPECT_EQ(status, Status::Cancelled());
  status.Update(Status::NotFound());
  EXPECT_EQ(status, Status::Cancelled());
}

// Functions for executing the C pw_Status tests.
extern "C" {

Status::Code PassStatusFromC(Status status);

Status::Code PassStatusFromCpp(Status status) { return status.code(); }

int TestStatusFromC(void);

int TestStatusStringsFromC(void);

}  // extern "C"

TEST(StatusCLinkage, CallCFunctionWithStatus) {
  EXPECT_EQ(Status::Aborted(), PassStatusFromC(PW_STATUS_ABORTED));
  EXPECT_EQ(Status::Unknown(), PassStatusFromC(Status::Unknown()));

  EXPECT_EQ(Status::NotFound(), PassStatusFromC(PW_STATUS_NOT_FOUND));
  EXPECT_EQ(OkStatus(), PassStatusFromC(OkStatus()));
}

TEST(StatusCLinkage, TestStatusFromC) { EXPECT_EQ(0, TestStatusFromC()); }

TEST(StatusCLinkage, TestStatusStringsFromC) {
  EXPECT_EQ(0, TestStatusStringsFromC());
}

}  // namespace
}  // namespace pw
