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

#include <memory>
#include <type_traits>

namespace pw::bluetooth::internal {

// Helper deleter struct to call the OnDestroy template parameter method when
// RaiiPtr<Api, OnDestroy> is destroyed.
template <class Api, void (Api::*OnDestroy)()>
struct RaiiPtrDeleter {
  void operator()(Api* api) { (api->*OnDestroy)(); }
};

// Smart pointer class to expose an API with RAII semantics that calls a
// specific method of the API implementation when it is destroyed, instead of
// calling delete on the API object like the default std::unique_ptr deleter
// behavior. This allows to be used like if it was a std::unique_ptr<API> but
// leaving the memory management implementation details up to the backend
// implementing the API.
//
// Example usage:
//    class SomeApi {
//     public:
//      virtual ~SomeApi() = default;
//
//      virtual void MethodOne() = 0;
//      virtual void MethodTwo(int) = 0;
//
//     private:
//      // Method used to release the resource in the backend.
//      virtual void DeleteResource() = 0;
//
//     public:
//      using Ptr = RaiiPtr<SomeApi, &SomeApi::DeleteResource>;
//    };
//
//    // The concrete backend implementation.
//    class BackendImplementation final : public SomeApi {
//     public:
//      void MethodOne() override { ... }
//      void MethodTwo(int) override { ... }
//      void DeleteResource() override { ... }
//    };
//
//    // Example using a static global resource object. GetResource should check
//    // whether the global resource is in use.
//    BackendImplementation backend_impl;
//    SomeApi::Ptr GetResource() { ...; return &backend_impl; }
//
template <class Api, void (Api::*OnDestroy)()>
using RaiiPtr = std::unique_ptr<Api, RaiiPtrDeleter<Api, OnDestroy>>;

}  // namespace pw::bluetooth::internal
