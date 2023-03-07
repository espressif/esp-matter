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

#include <optional>
#include <utility>

#include "pw_assert/assert.h"

namespace pw::bluetooth {

// A Result represents the result of an operation which can fail. If it
// represents an error, it contains an error value. If it represents success, it
// contains zero or one success value.
template <typename E, typename... Ts>
class Result;

// Result specialization for returning OK or an error (E).
template <typename E>
class [[nodiscard]] Result<E> {
 public:
  constexpr Result() = default;
  constexpr Result(E error) : error_(error) {}

  constexpr Result(const Result&) = default;
  constexpr Result& operator=(const Result&) = default;

  constexpr Result(Result&&) = default;
  constexpr Result& operator=(Result&&) = default;

  [[nodiscard]] constexpr E error() const {
    PW_ASSERT(error_.has_value());
    return error_.value();
  }
  [[nodiscard]] constexpr bool ok() const { return !error_.has_value(); }

 private:
  std::optional<E> error_;
};

// Result specialization for returning some data (T) or an error (E).
template <typename E, typename T>
class [[nodiscard]] Result<E, T> {
 public:
  constexpr Result(T&& value) : value_(std::move(value)) {}
  constexpr Result(const T& value) : value_(value) {}

  template <typename... Args>
  constexpr Result(std::in_place_t, Args&&... args)
      : value_(std::forward<Args>(args)...) {}

  constexpr Result(E error) : error_(error) {}

  constexpr Result(const Result&) = default;
  constexpr Result& operator=(const Result&) = default;

  constexpr Result(Result&&) = default;
  constexpr Result& operator=(Result&&) = default;

  [[nodiscard]] constexpr E error() const {
    PW_ASSERT(!value_.has_value());
    return error_;
  }
  [[nodiscard]] constexpr bool ok() const { return value_.has_value(); }

  constexpr T& value() & {
    PW_ASSERT(value_.has_value());
    return value_.value();
  }

  constexpr const T& value() const& {
    PW_ASSERT(value_.has_value());
    return value_.value();
  }

  constexpr T&& value() && {
    PW_ASSERT(value_.has_value());
    return std::move(value_.value());
  }

 private:
  std::optional<T> value_;
  // error_ is only initialized if value_ is empty.
  E error_ = {};
};

}  // namespace pw::bluetooth
