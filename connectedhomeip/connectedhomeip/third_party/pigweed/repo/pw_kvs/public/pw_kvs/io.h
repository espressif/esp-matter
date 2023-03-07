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

#include <cstddef>
#include <type_traits>

#include "pw_span/span.h"
#include "pw_status/status_with_size.h"

namespace pw {
namespace internal {

template <typename T>
struct FunctionTraits;

template <typename T, typename ReturnType, typename... Args>
struct FunctionTraits<ReturnType (T::*)(Args...)> {
  using Class = T;
  using Return = ReturnType;
};

}  // namespace internal

// Writes bytes to an unspecified output. Provides a Write function that takes a
// span of bytes and returns a Status.
class Output {
 public:
  StatusWithSize Write(span<const std::byte> data) { return DoWrite(data); }

  // Convenience wrapper for writing data from a pointer and length.
  StatusWithSize Write(const void* data, size_t size_bytes) {
    return Write(
        span<const std::byte>(static_cast<const std::byte*>(data), size_bytes));
  }

 protected:
  ~Output() = default;

 private:
  virtual StatusWithSize DoWrite(span<const std::byte> data) = 0;
};

class Input {
 public:
  StatusWithSize Read(span<std::byte> data) { return DoRead(data); }

  // Convenience wrapper for reading data from a pointer and length.
  StatusWithSize Read(void* data, size_t size_bytes) {
    return Read(span<std::byte>(static_cast<std::byte*>(data), size_bytes));
  }

 protected:
  ~Input() = default;

 private:
  virtual StatusWithSize DoRead(span<std::byte> data) = 0;
};

// Output adapter that calls a free function.
class OutputToFunction final : public Output {
 public:
  OutputToFunction(StatusWithSize (*function)(span<const std::byte>))
      : function_(function) {}

 private:
  StatusWithSize DoWrite(span<const std::byte> data) override {
    return function_(data);
  }

  StatusWithSize (*function_)(span<const std::byte>);
};

}  // namespace pw
