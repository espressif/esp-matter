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

#include <atomic>
#include <cstdint>

namespace pw::internal {

// Base class for RefCounted. Separates ref count storage from the private
// API accessible by the IntrusivePtr through the RefCounted.
class RefCountedBase {
 public:
  RefCountedBase(const RefCountedBase&) = delete;
  RefCountedBase(RefCountedBase&&) = delete;
  RefCountedBase& operator=(const RefCountedBase&) = delete;
  RefCountedBase& operator=(RefCountedBase&&) = delete;

 protected:
  constexpr RefCountedBase() = default;
  ~RefCountedBase();

  // Increments reference counter.
  void AddRef() const;

  // Decrements reference count and returns true if the object should be
  // deleted.
  [[nodiscard]] bool ReleaseRef() const;

  // Returns current ref count value.
  [[nodiscard]] int32_t ref_count() const {
    return ref_count_.load(std::memory_order_relaxed);
  }

 private:
  mutable std::atomic_int32_t ref_count_{0};
};

}  // namespace pw::internal
