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
#pragma once

#include "pw_status/internal/config.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// This is the pw_Status enum. pw_Status is used to return the status from an
// operation.
//
// In C++, use the pw::Status class instead of the pw_Status enum. pw_Status and
// Status implicitly convert to one another and can be passed cleanly between C
// and C++ APIs.
//
// pw_Status uses the canonical Google error codes. The following enum was based
// on Abseil's status/status.h. The values are all-caps and prefixed with
// PW_STATUS_ instead of using C++ constant style.
typedef enum {
  // Ok (gRPC code "OK") does not indicate an error; this value is returned on
  // success. It is typical to check for this value before proceeding on any
  // given call across an API or RPC boundary. To check this value, use the
  // `Status::ok()` member function rather than inspecting the raw code.
  PW_STATUS_OK = 0,  // Use OkStatus() in C++

  // Cancelled (gRPC code "CANCELLED") indicates the operation was cancelled,
  // typically by the caller.
  PW_STATUS_CANCELLED = 1,  // Use Status::Cancelled() in C++

  // Unknown (gRPC code "UNKNOWN") indicates an unknown error occurred. In
  // general, more specific errors should be raised, if possible. Errors raised
  // by APIs that do not return enough error information may be converted to
  // this error.
  PW_STATUS_UNKNOWN = 2,  // Use Status::Unknown() in C++

  // InvalidArgument (gRPC code "INVALID_ARGUMENT") indicates the caller
  // specified an invalid argument, such a malformed filename. Note that such
  // errors should be narrowly limited to indicate to the invalid nature of the
  // arguments themselves. Errors with validly formed arguments that may cause
  // errors with the state of the receiving system should be denoted with
  // `FailedPrecondition` instead.
  PW_STATUS_INVALID_ARGUMENT = 3,  // Use Status::InvalidArgument() in C++

  // DeadlineExceeded (gRPC code "DEADLINE_EXCEEDED") indicates a deadline
  // expired before the operation could complete. For operations that may change
  // state within a system, this error may be returned even if the operation has
  // completed successfully. For example, a successful response from a server
  // could have been delayed long enough for the deadline to expire.
  PW_STATUS_DEADLINE_EXCEEDED = 4,  // Use Status::DeadlineExceeded() in C++

  // NotFound (gRPC code "NOT_FOUND") indicates some requested entity (such as
  // a file or directory) was not found.
  //
  // `NotFound` is useful if a request should be denied for an entire class of
  // users, such as during a gradual feature rollout or undocumented allow list.
  // If, instead, a request should be denied for specific sets of users, such as
  // through user-based access control, use `PermissionDenied` instead.
  PW_STATUS_NOT_FOUND = 5,  // Use Status::NotFound() in C++

  // AlreadyExists (gRPC code "ALREADY_EXISTS") indicates the entity that a
  // caller attempted to create (such as file or directory) is already present.
  PW_STATUS_ALREADY_EXISTS = 6,  // Use Status::AlreadyExists() in C++

  // PermissionDenied (gRPC code "PERMISSION_DENIED") indicates that the caller
  // does not have permission to execute the specified operation. Note that this
  // error is different than an error due to an *un*authenticated user. This
  // error code does not imply the request is valid or the requested entity
  // exists or satisfies any other pre-conditions.
  //
  // `PermissionDenied` must not be used for rejections caused by exhausting
  // some resource. Instead, use `ResourceExhausted` for those errors.
  // `PermissionDenied` must not be used if the caller cannot be identified.
  // Instead, use `Unauthenticated` for those errors.
  PW_STATUS_PERMISSION_DENIED = 7,  // Use Status::PermissionDenied() in C++

  // ResourceExhausted (gRPC code "RESOURCE_EXHAUSTED") indicates some resource
  // has been exhausted, perhaps a per-user quota, or perhaps the entire file
  // system is out of space.
  PW_STATUS_RESOURCE_EXHAUSTED = 8,  // Use Status::ResourceExhausted() in C++

  // FailedPrecondition (gRPC code "FAILED_PRECONDITION") indicates that the
  // operation was rejected because the system is not in a state required for
  // the operation's execution. For example, a directory to be deleted may be
  // non-empty, an "rmdir" operation is applied to a non-directory, etc.
  //
  // Some guidelines that may help a service implementer in deciding between
  // `FailedPrecondition`, `Aborted`, and `Unavailable`:
  //
  //  (a) Use `Unavailable` if the client can retry just the failing call.
  //  (b) Use `Aborted` if the client should retry at a higher transaction
  //      level (such as when a client-specified test-and-set fails, indicating
  //      the client should restart a read-modify-write sequence).
  //  (c) Use `FailedPrecondition` if the client should not retry until
  //      the system state has been explicitly fixed. For example, if an "rmdir"
  //      fails because the directory is non-empty, `FailedPrecondition`
  //      should be returned since the client should not retry unless
  //      the files are deleted from the directory.
  PW_STATUS_FAILED_PRECONDITION = 9,  // Use Status::FailedPrecondition() in C++

  // Aborted (gRPC code "ABORTED") indicates the operation was aborted,
  // typically due to a concurrency issue such as a sequencer check failure or a
  // failed transaction.
  //
  // See the guidelines above for deciding between `FailedPrecondition`,
  // `Aborted`, and `Unavailable`.
  PW_STATUS_ABORTED = 10,  // Use Status::Aborted() in C++

  // OutOfRange (gRPC code "OUT_OF_RANGE") indicates the operation was
  // attempted past the valid range, such as seeking or reading past an
  // end-of-file.
  //
  // Unlike `InvalidArgument`, this error indicates a problem that may
  // be fixed if the system state changes. For example, a 32-bit file
  // system will generate `InvalidArgument` if asked to read at an
  // offset that is not in the range [0,2^32-1], but it will generate
  // `OutOfRange` if asked to read from an offset past the current
  // file size.
  //
  // There is a fair bit of overlap between `FailedPrecondition` and
  // `OutOfRange`.  We recommend using `OutOfRange` (the more specific
  // error) when it applies so that callers who are iterating through
  // a space can easily look for an `OutOfRange` error to detect when
  // they are done.
  PW_STATUS_OUT_OF_RANGE = 11,  // Use Status::OutOfRange() in C++

  // Unimplemented (gRPC code "UNIMPLEMENTED") indicates the operation is not
  // implemented or supported in this service. In this case, the operation
  // should not be re-attempted.
  PW_STATUS_UNIMPLEMENTED = 12,  // Use Status::Unimplemented() in C++

  // Internal (gRPC code "INTERNAL") indicates an internal error has occurred
  // and some invariants expected by the underlying system have not been
  // satisfied. This error code is reserved for serious errors.
  PW_STATUS_INTERNAL = 13,  // Use Status::Internal() in C++

  // Unavailable (gRPC code "UNAVAILABLE") indicates the service is currently
  // unavailable and that this is most likely a transient condition. An error
  // such as this can be corrected by retrying with a backoff scheme. Note that
  // it is not always safe to retry non-idempotent operations.
  //
  // See the guidelines above for deciding between `FailedPrecondition`,
  // `Aborted`, and `Unavailable`.
  PW_STATUS_UNAVAILABLE = 14,  // Use Status::Unavailable() in C++

  // DataLoss (gRPC code "DATA_LOSS") indicates that unrecoverable data loss or
  // corruption has occurred. As this error is serious, proper alerting should
  // be attached to errors such as this.
  PW_STATUS_DATA_LOSS = 15,  // Use Status::DataLoss() in C++

  // Unauthenticated (gRPC code "UNAUTHENTICATED") indicates that the request
  // does not have valid authentication credentials for the operation. Correct
  // the authentication and try again.
  PW_STATUS_UNAUTHENTICATED = 16,  // Use Status::Unauthenticated() in C++

  // NOTE: this error code entry should not be used and you should not rely on
  // its value, which may change.
  //
  // The purpose of this enumerated value is to force people who handle status
  // codes with `switch()` statements to *not* simply enumerate all possible
  // values, but instead provide a "default:" case. Providing such a default
  // case ensures that code will compile when new codes are added.
  PW_STATUS_DO_NOT_USE_RESERVED_FOR_FUTURE_EXPANSION_USE_DEFAULT_IN_SWITCH_INSTEAD_,
} pw_Status;  // Use pw::Status in C++

// Returns a null-terminated string representation of the pw_Status.
const char* pw_StatusString(pw_Status status);

#ifdef __cplusplus

}  // extern "C"

namespace pw {

// The Status class is a thin, zero-cost abstraction around the pw_Status enum.
// It initializes to OkStatus() by default and adds ok() and str() methods.
// Implicit conversions are permitted between pw_Status and pw::Status.
class _PW_STATUS_NO_DISCARD Status {
 public:
  using Code = pw_Status;

  // Functions that create a Status with the specified code.
  // clang-format off
  [[nodiscard]] static constexpr Status Cancelled() {
    return PW_STATUS_CANCELLED;
  }
  [[nodiscard]] static constexpr Status Unknown() {
    return PW_STATUS_UNKNOWN;
  }
  [[nodiscard]] static constexpr Status InvalidArgument() {
    return PW_STATUS_INVALID_ARGUMENT;
  }
  [[nodiscard]] static constexpr Status DeadlineExceeded() {
    return PW_STATUS_DEADLINE_EXCEEDED;
  }
  [[nodiscard]] static constexpr Status NotFound() {
    return PW_STATUS_NOT_FOUND;
  }
  [[nodiscard]] static constexpr Status AlreadyExists() {
    return PW_STATUS_ALREADY_EXISTS;
  }
  [[nodiscard]] static constexpr Status PermissionDenied() {
    return PW_STATUS_PERMISSION_DENIED;
  }
  [[nodiscard]] static constexpr Status ResourceExhausted() {
    return PW_STATUS_RESOURCE_EXHAUSTED;
  }
  [[nodiscard]] static constexpr Status FailedPrecondition() {
    return PW_STATUS_FAILED_PRECONDITION;
  }
  [[nodiscard]] static constexpr Status Aborted() {
    return PW_STATUS_ABORTED;
  }
  [[nodiscard]] static constexpr Status OutOfRange() {
    return PW_STATUS_OUT_OF_RANGE;
  }
  [[nodiscard]] static constexpr Status Unimplemented() {
    return PW_STATUS_UNIMPLEMENTED;
  }
  [[nodiscard]] static constexpr Status Internal() {
    return PW_STATUS_INTERNAL;
  }
  [[nodiscard]] static constexpr Status Unavailable() {
    return PW_STATUS_UNAVAILABLE;
  }
  [[nodiscard]] static constexpr Status DataLoss() {
    return PW_STATUS_DATA_LOSS;
  }
  [[nodiscard]] static constexpr Status Unauthenticated() {
    return PW_STATUS_UNAUTHENTICATED;
  }
  // clang-format on

  // Statuses are created with a Status::Code.
  constexpr Status(Code code = PW_STATUS_OK) : code_(code) {}

  constexpr Status(const Status&) = default;
  constexpr Status& operator=(const Status&) = default;

  // Returns the Status::Code (pw_Status) for this Status.
  constexpr Code code() const { return code_; }

  // True if the status is OK.
  [[nodiscard]] constexpr bool ok() const { return code_ == PW_STATUS_OK; }

  // Functions for checking which status this is.
  [[nodiscard]] constexpr bool IsCancelled() const {
    return code_ == PW_STATUS_CANCELLED;
  }
  [[nodiscard]] constexpr bool IsUnknown() const {
    return code_ == PW_STATUS_UNKNOWN;
  }
  [[nodiscard]] constexpr bool IsInvalidArgument() const {
    return code_ == PW_STATUS_INVALID_ARGUMENT;
  }
  [[nodiscard]] constexpr bool IsDeadlineExceeded() const {
    return code_ == PW_STATUS_DEADLINE_EXCEEDED;
  }
  [[nodiscard]] constexpr bool IsNotFound() const {
    return code_ == PW_STATUS_NOT_FOUND;
  }
  [[nodiscard]] constexpr bool IsAlreadyExists() const {
    return code_ == PW_STATUS_ALREADY_EXISTS;
  }
  [[nodiscard]] constexpr bool IsPermissionDenied() const {
    return code_ == PW_STATUS_PERMISSION_DENIED;
  }
  [[nodiscard]] constexpr bool IsResourceExhausted() const {
    return code_ == PW_STATUS_RESOURCE_EXHAUSTED;
  }
  [[nodiscard]] constexpr bool IsFailedPrecondition() const {
    return code_ == PW_STATUS_FAILED_PRECONDITION;
  }
  [[nodiscard]] constexpr bool IsAborted() const {
    return code_ == PW_STATUS_ABORTED;
  }
  [[nodiscard]] constexpr bool IsOutOfRange() const {
    return code_ == PW_STATUS_OUT_OF_RANGE;
  }
  [[nodiscard]] constexpr bool IsUnimplemented() const {
    return code_ == PW_STATUS_UNIMPLEMENTED;
  }
  [[nodiscard]] constexpr bool IsInternal() const {
    return code_ == PW_STATUS_INTERNAL;
  }
  [[nodiscard]] constexpr bool IsUnavailable() const {
    return code_ == PW_STATUS_UNAVAILABLE;
  }
  [[nodiscard]] constexpr bool IsDataLoss() const {
    return code_ == PW_STATUS_DATA_LOSS;
  }
  [[nodiscard]] constexpr bool IsUnauthenticated() const {
    return code_ == PW_STATUS_UNAUTHENTICATED;
  }

  // Updates this Status to the provided Status IF this status is OK. This is
  // useful for tracking the first encountered error, as calls to this helper
  // will not change one error status to another error status.
  constexpr void Update(Status other) {
    if (ok()) {
      code_ = other.code();
    }
  }

  // Ignores any errors. This method does nothing except potentially suppress
  // complaints from any tools that are checking that errors are not dropped on
  // the floor.
  constexpr void IgnoreError() const {}

  // Returns a null-terminated string representation of the Status.
  [[nodiscard]] const char* str() const { return pw_StatusString(code_); }

 private:
  Code code_;
};

// Returns an OK status. Equivalent to Status() or Status(PW_STATUS_OK). This
// function is used instead of a Status::Ok() function, which would be too
// similar to Status::ok().
[[nodiscard]] constexpr Status OkStatus() { return Status(); }

constexpr bool operator==(const Status& lhs, const Status& rhs) {
  return lhs.code() == rhs.code();
}

constexpr bool operator!=(const Status& lhs, const Status& rhs) {
  return lhs.code() != rhs.code();
}

}  // namespace pw

// Create a C++ overload of pw_StatusString so that it supports pw::Status in
// addition to pw_Status.
inline const char* pw_StatusString(pw::Status status) {
  return pw_StatusString(status.code());
}

#endif  // __cplusplus
