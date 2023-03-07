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

#include <tuple>
#include <type_traits>

#include "gtest/gtest.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/raw/internal/method.h"

namespace pw::rpc::internal {

template <typename...>
struct MatchesTypes {};

// This class tests Method implementation classes and MethodTraits
// specializations. It verifies that they provide the expected functions and
// that they correctly identify and construct the various method types.
//
// The TestService class must inherit from Service and provide the following
// methods with valid signatures for RPCs:
//
//   - Unary: synchronous unary RPC member function
//   - StaticUnary: synchronous unary RPC static member function
//   - AsyncUnary: asynchronous unary RPC member function
//   - StaticAsyncUnary: asynchronous unary RPC static member function
//   - ServerStreaming: server streaming RPC member function
//   - StaticServerStreaming: server streaming static RPC member function
//   - ClientStreaming: client streaming RPC member function
//   - StaticClientStreaming: client streaming static RPC member function
//   - BidirectionalStreaming: bidirectional streaming RPC member function
//   - StaticBidirectionalStreaming: bidirectional streaming static RPC
//         member function
//
template <typename MethodImpl, typename TestService>
class MethodImplTests {
 public:
  template <typename... ExtraTypes, typename... CreationArgs>
  constexpr bool Pass(
      const MatchesTypes<ExtraTypes...>& = {},
      const std::tuple<CreationArgs...>& creation_args = {}) const {
    return Matches<ExtraTypes...>().Pass() && Type().Pass() &&
           Creation().Pass(creation_args);
  }

 private:
  template <typename... ExtraTypes>
  struct Matches {
    constexpr bool Pass() const { return true; }

    // Test that the matches() function matches valid signatures.
    static_assert(
        MethodImpl::template matches<&TestService::Unary, ExtraTypes...>());
    static_assert(MethodImpl::template matches<&TestService::StaticUnary,
                                               ExtraTypes...>());

    static_assert(MethodImpl::template matches<&TestService::AsyncUnary,
                                               ExtraTypes...>());
    static_assert(MethodImpl::template matches<&TestService::StaticAsyncUnary,
                                               ExtraTypes...>());

    static_assert(MethodImpl::template matches<&TestService::ServerStreaming,
                                               ExtraTypes...>());
    static_assert(
        MethodImpl::template matches<&TestService::StaticServerStreaming,
                                     ExtraTypes...>());

    static_assert(MethodImpl::template matches<&TestService::ClientStreaming,
                                               ExtraTypes...>());
    static_assert(
        MethodImpl::template matches<&TestService::StaticClientStreaming,
                                     ExtraTypes...>());

    static_assert(
        MethodImpl::template matches<&TestService::BidirectionalStreaming,
                                     ExtraTypes...>());
    static_assert(
        MethodImpl::template matches<&TestService::StaticBidirectionalStreaming,
                                     ExtraTypes...>());

    // Test that the matches() function does not match the wrong method type.
    static_assert(!MethodImpl::template matches<&TestService::UnaryWrongArg,
                                                ExtraTypes...>());
    static_assert(
        !MethodImpl::template matches<&TestService::StaticUnaryVoidReturn,
                                      ExtraTypes...>());

    static_assert(
        !MethodImpl::template matches<&TestService::ServerStreamingBadReturn,
                                      ExtraTypes...>());
    static_assert(!MethodImpl::template matches<
                  &TestService::StaticServerStreamingMissingArg,
                  ExtraTypes...>());

    static_assert(
        !MethodImpl::template matches<&TestService::ClientStreamingBadReturn,
                                      ExtraTypes...>());
    static_assert(!MethodImpl::template matches<
                  &TestService::StaticClientStreamingMissingArg,
                  ExtraTypes...>());

    static_assert(!MethodImpl::template matches<
                  &TestService::BidirectionalStreamingBadReturn,
                  ExtraTypes...>());
    static_assert(!MethodImpl::template matches<
                  &TestService::StaticBidirectionalStreamingMissingArg,
                  ExtraTypes...>());
  };

  // Check that MethodTraits resolves to the correct value for kType.
  struct Type {
    constexpr bool Pass() const { return true; }

    // Don't check kSynchronous for Unary since not all method implementations
    // support synchronous unary.
    static_assert(MethodTraits<decltype(&TestService::Unary)>::kType ==
                  MethodType::kUnary);
    static_assert(MethodTraits<decltype(&TestService::StaticUnary)>::kType ==
                  MethodType::kUnary);
    static_assert(MethodTraits<decltype(&TestService::AsyncUnary)>::kType ==
                  MethodType::kUnary);
    static_assert(
        !MethodTraits<decltype(&TestService::AsyncUnary)>::kSynchronous);
    static_assert(
        MethodTraits<decltype(&TestService::StaticAsyncUnary)>::kType ==
        MethodType::kUnary);
    static_assert(
        !MethodTraits<decltype(&TestService::StaticAsyncUnary)>::kSynchronous);

    static_assert(
        MethodTraits<decltype(&TestService::ServerStreaming)>::kType ==
        MethodType::kServerStreaming);
    static_assert(
        MethodTraits<decltype(&TestService::StaticServerStreaming)>::kType ==
        MethodType::kServerStreaming);

    static_assert(
        MethodTraits<decltype(&TestService::ClientStreaming)>::kType ==
        MethodType::kClientStreaming);
    static_assert(
        MethodTraits<decltype(&TestService::StaticClientStreaming)>::kType ==
        MethodType::kClientStreaming);

    static_assert(
        MethodTraits<decltype(&TestService::BidirectionalStreaming)>::kType ==
        MethodType::kBidirectionalStreaming);
    static_assert(
        MethodTraits<
            decltype(&TestService::StaticBidirectionalStreaming)>::kType ==
        MethodType::kBidirectionalStreaming);
  };

  // Test method creation.
  class Creation {
   public:
    template <typename... Args>
    constexpr bool Pass(const std::tuple<Args...>& args) const {
      return AsyncUnaryMethod(args).id() == 3 &&
             StaticAsyncUnaryMethod(args).id() == 4 &&
             ServerStreamingMethod(args).id() == 5 &&
             StaticServerStreamingMethod(args).id() == 6 &&
             ClientStreamingMethod(args).id() == 7 &&
             StaticClientStreamingMethod(args).id() == 8 &&
             BidirectionalStreamingMethod(args).id() == 9 &&
             StaticBidirectionalStreamingMethod(args).id() == 10 &&
             InvalidMethod().id() == 0;
    }

   private:
    // Do not check synchronous unary since not all method implementations
    // support it.

    template <typename... Args>
    constexpr MethodImpl AsyncUnaryMethod(
        const std::tuple<Args...>& args) const {
      return Call(
          MethodImpl::template AsynchronousUnary<&TestService::AsyncUnary>,
          3,
          args);
    }

    template <typename... Args>
    constexpr MethodImpl StaticAsyncUnaryMethod(
        const std::tuple<Args...>& args) const {
      return Call(MethodImpl::template AsynchronousUnary<
                      &TestService::StaticAsyncUnary>,
                  4,
                  args);
    }

    template <typename... Args>
    constexpr MethodImpl ServerStreamingMethod(
        const std::tuple<Args...>& args) const {
      return Call(
          MethodImpl::template ServerStreaming<&TestService::ServerStreaming>,
          5,
          args);
    }

    template <typename... Args>
    constexpr MethodImpl StaticServerStreamingMethod(
        const std::tuple<Args...>& args) const {
      return Call(MethodImpl::template ServerStreaming<
                      &TestService::StaticServerStreaming>,
                  6,
                  args);
    }

    template <typename... Args>
    constexpr MethodImpl ClientStreamingMethod(
        const std::tuple<Args...>& args) const {
      return Call(
          MethodImpl::template ClientStreaming<&TestService::ClientStreaming>,
          7,
          args);
    }

    template <typename... Args>
    constexpr MethodImpl StaticClientStreamingMethod(
        const std::tuple<Args...>& args) const {
      return Call(MethodImpl::template ClientStreaming<
                      &TestService::StaticClientStreaming>,
                  8,
                  args);
    }

    template <typename... Args>
    constexpr MethodImpl BidirectionalStreamingMethod(
        const std::tuple<Args...>& args) const {
      return Call(MethodImpl::template BidirectionalStreaming<
                      &TestService::BidirectionalStreaming>,
                  9,
                  args);
    }

    template <typename... Args>
    constexpr MethodImpl StaticBidirectionalStreamingMethod(
        const std::tuple<Args...>& args) const {
      return Call(MethodImpl::template BidirectionalStreaming<
                      &TestService::StaticBidirectionalStreaming>,
                  10,
                  args);
    }

    // Test that there is an Invalid method creation function.
    constexpr MethodImpl InvalidMethod() const { return MethodImpl::Invalid(); }

    // Invokes the method creation function with the ID and extra args.
    template <typename Function, typename... Args>
    static constexpr MethodImpl Call(Function&& function,
                                     uint32_t id,
                                     const std::tuple<Args...>& args) {
      return std::apply(function, std::tuple_cat(std::tuple(id), args));
    }
  };
};

}  // namespace pw::rpc::internal
