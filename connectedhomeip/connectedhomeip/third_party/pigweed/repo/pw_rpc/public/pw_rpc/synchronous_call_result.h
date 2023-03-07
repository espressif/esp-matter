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

#include "pw_assert/assert.h"
#include "pw_status/status.h"

namespace pw {
namespace rpc {

// A `SynchronousCallResult<Response>` is an object that contains the result of
// a `SynchronousCall`. When synchronous calls are made, errors could occur for
// multiple reasons. There could have been an error at either the RPC layer or
// Server; or the client-specified timeout might have occurred. A user can query
// this object to determine what type of error occurred, so that they can handle
// it appropriately. If the server responded, the response() and dereference
// operators will provide access to the Response.
//
// Example:
//
//   SynchronousCallResult<MyResponse> result =
//     SynchronousCallFor<MyService::Method>(client, request, timeout);
//   if (result.is_rpc_error()) {
//     ShutdownClient(client);
//   } else if (result.is_timeout()) {
//     RetryCall(client, request);
//   } else if (result.is_server_error()) {
//     return result.status();
//   }
//   PW_ASSERT(result.ok());
//   return std::move(result).response();
//
// For some RPCs, the server could have responded with a non-Ok Status but with
// a valid Response object. For example, if the server was ran out of space in a
// buffer, it might return a Status of ResourceExhausted, but the response
// contains as much data as could fit. In this situation, users should be
// careful not to treat the error as fatal.
//
// Example:
//
//   SynchronousCallResult<BufferResponse> result =
//     SynchronousCall<MyService::Read>(client, request);
//   if (result.is_rpc_error()) {
//     ShutdownClient(client);
//   }
//   PW_ASSERT(result.is_server_response());
//   HandleServerResponse(result.status(), result.response());
//

namespace internal {
enum class SynchronousCallStatus {
  kInvalid,
  kTimeout,
  kRpc,
  kServer,
};
}  // namespace internal

template <typename Response>
class SynchronousCallResult {
 public:
  // Error Constructors
  constexpr static SynchronousCallResult Timeout();
  constexpr static SynchronousCallResult RpcError(Status status);

  // Server Response Constructor
  constexpr explicit SynchronousCallResult(Status status, Response response)
      : call_status_(internal::SynchronousCallStatus::kServer),
        status_(status),
        response_(std::move(response)) {}

  constexpr SynchronousCallResult() = default;
  ~SynchronousCallResult() = default;

  // Copyable if `Response` is copyable.
  constexpr SynchronousCallResult(const SynchronousCallResult&) = default;
  constexpr SynchronousCallResult& operator=(const SynchronousCallResult&) =
      default;

  // Movable if `Response` is movable.
  constexpr SynchronousCallResult(SynchronousCallResult&&) = default;
  constexpr SynchronousCallResult& operator=(SynchronousCallResult&&) = default;

  // Returns true if there was a timeout, an rpc error or the server returned a
  // non-Ok status.
  [[nodiscard]] constexpr bool is_error() const;

  // Returns true if the server returned a response with an Ok status.
  [[nodiscard]] constexpr bool ok() const;

  // Returns true if the server responded with a non-Ok status.
  [[nodiscard]] constexpr bool is_server_error() const;

  [[nodiscard]] constexpr bool is_timeout() const;
  [[nodiscard]] constexpr bool is_rpc_error() const;
  [[nodiscard]] constexpr bool is_server_response() const;

  [[nodiscard]] constexpr Status status() const;

  // SynchronousCallResult<Response>::response()
  // SynchronousCallResult<Response>::operator*()
  // SynchronousCallResult<Response>::operator->()
  //
  // Accessors to the held value if `this->is_server_response()`. Otherwise,
  // terminates the process.
  constexpr const Response& response() const&;
  constexpr Response& response() &;
  constexpr const Response&& response() const&&;
  constexpr Response&& response() &&;

  constexpr const Response& operator*() const&;
  constexpr Response& operator*() &;
  constexpr const Response&& operator*() const&&;
  constexpr Response&& operator*() &&;

  constexpr const Response* operator->() const;
  constexpr Response* operator->();

 private:
  // This constructor is private to protect against invariants that might occur
  // when constructing with a SynchronousCallStatus.
  constexpr explicit SynchronousCallResult(
      internal::SynchronousCallStatus call_status, Status status)
      : call_status_(call_status), status_(status) {}

  internal::SynchronousCallStatus call_status_ =
      internal::SynchronousCallStatus::kInvalid;
  Status status_{};
  Response response_{};
};

// Implementations

template <typename Response>
constexpr SynchronousCallResult<Response>
SynchronousCallResult<Response>::Timeout() {
  return SynchronousCallResult(internal::SynchronousCallStatus::kTimeout,
                               Status::DeadlineExceeded());
}

template <typename Response>
constexpr SynchronousCallResult<Response>
SynchronousCallResult<Response>::RpcError(Status status) {
  return SynchronousCallResult(internal::SynchronousCallStatus::kRpc, status);
}

template <typename Response>
constexpr bool SynchronousCallResult<Response>::is_error() const {
  return !ok();
}

template <typename Response>
constexpr bool SynchronousCallResult<Response>::ok() const {
  return is_server_response() && status_.ok();
}

template <typename Response>
constexpr bool SynchronousCallResult<Response>::is_server_error() const {
  return is_server_response() && !status_.ok();
}

template <typename Response>
constexpr bool SynchronousCallResult<Response>::is_timeout() const {
  return call_status_ == internal::SynchronousCallStatus::kTimeout;
}

template <typename Response>
constexpr bool SynchronousCallResult<Response>::is_rpc_error() const {
  return call_status_ == internal::SynchronousCallStatus::kRpc;
}

template <typename Response>
constexpr bool SynchronousCallResult<Response>::is_server_response() const {
  return call_status_ == internal::SynchronousCallStatus::kServer;
}

template <typename Response>
constexpr Status SynchronousCallResult<Response>::status() const {
  PW_ASSERT(call_status_ != internal::SynchronousCallStatus::kInvalid);
  return status_;
}

template <typename Response>
constexpr const Response& SynchronousCallResult<Response>::response() const& {
  PW_ASSERT(is_server_response());
  return response_;
}

template <typename Response>
constexpr Response& SynchronousCallResult<Response>::response() & {
  PW_ASSERT(is_server_response());
  return response_;
}

template <typename Response>
constexpr const Response&& SynchronousCallResult<Response>::response() const&& {
  PW_ASSERT(is_server_response());
  return std::move(response_);
}

template <typename Response>
constexpr Response&& SynchronousCallResult<Response>::response() && {
  PW_ASSERT(is_server_response());
  return std::move(response_);
}

template <typename Response>
constexpr const Response& SynchronousCallResult<Response>::operator*() const& {
  PW_ASSERT(is_server_response());
  return response_;
}

template <typename Response>
constexpr Response& SynchronousCallResult<Response>::operator*() & {
  PW_ASSERT(is_server_response());
  return response_;
}

template <typename Response>
constexpr const Response&& SynchronousCallResult<Response>::operator*()
    const&& {
  PW_ASSERT(is_server_response());
  return std::move(response_);
}

template <typename Response>
constexpr Response&& SynchronousCallResult<Response>::operator*() && {
  PW_ASSERT(is_server_response());
  return std::move(response_);
}

template <typename Response>
constexpr const Response* SynchronousCallResult<Response>::operator->() const {
  PW_ASSERT(is_server_response());
  return &response_;
}

template <typename Response>
constexpr Response* SynchronousCallResult<Response>::operator->() {
  PW_ASSERT(is_server_response());
  return &response_;
}

}  // namespace rpc

// Conversion functions for usage with PW_TRY and PW_TRY_ASSIGN.
namespace internal {

template <typename T>
constexpr Status ConvertToStatus(const rpc::SynchronousCallResult<T>& result) {
  return result.status();
}

template <typename T>
constexpr T ConvertToValue(rpc::SynchronousCallResult<T>& result) {
  return std::move(result.response());
}

}  // namespace internal
}  // namespace pw
