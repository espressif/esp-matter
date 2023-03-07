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
#pragma once

#include <type_traits>

namespace pw::digital_io {
namespace internal {

// A type trait that describes the functionality required by a particular type
// of line, and also the functionality that we assume is always provided by an
// instance of that line. Used by Converter to determine if a conversion should
// be allowed.
//
// Specialize this for each type of DigitalIO line by defining a required
// functionality as `true` and optional functionality as `false`.
//
// Specializations must define the following fields:
// static constexpr bool input;
// static constexpr bool output;
// static constexpr bool interrupt;
//
template <typename T>
struct Requires;

// Concrete struct describing the available functionality of a line at runtime.
struct Provides {
  bool input;
  bool output;
  bool interrupt;
};

// Returns the functionality always provided by the given type.
template <typename T>
constexpr Provides AlwaysProvidedBy() {
  return {
      .input = Requires<T>::input,
      .output = Requires<T>::output,
      .interrupt = Requires<T>::interrupt,
  };
}

// Provides conversion operators between line objects based on the available
// functionality.
template <typename Self, typename CommonBase>
class Conversions {
 private:
  // Static check to enable conversions from `Self` to `T`.
  template <
      typename T,
      typename = std::enable_if_t<std::is_base_of_v<CommonBase, T>>,
      typename = std::enable_if_t<Requires<Self>::input || !Requires<T>::input>,
      typename =
          std::enable_if_t<Requires<Self>::output || !Requires<T>::output>,
      typename = std::enable_if_t<Requires<Self>::interrupt ||
                                  !Requires<T>::interrupt>>
  struct Enabled {};

 public:
  template <typename T, typename = Enabled<T>>
  constexpr operator T&() {
    return as<T>();
  }

  template <typename T, typename = Enabled<T>>
  constexpr operator const T&() const {
    return as<T>();
  }

  template <typename T, typename = Enabled<T>>
  constexpr T& as() {
    return static_cast<T&>(static_cast<CommonBase&>(static_cast<Self&>(*this)));
  }

  template <typename T, typename = Enabled<T>>
  constexpr const T& as() const {
    return static_cast<const T&>(
        static_cast<const CommonBase&>(static_cast<const Self&>(*this)));
  }
};

}  // namespace internal

// Specializations of Requires for each of the line types.
// These live outside the `internal` namespace so that the forward class
// declarations are in the correct namespace.

template <>
struct internal::Requires<class DigitalInterrupt> {
  static constexpr bool input = false;
  static constexpr bool output = false;
  static constexpr bool interrupt = true;
};

template <>
struct internal::Requires<class DigitalIn> {
  static constexpr bool input = true;
  static constexpr bool output = false;
  static constexpr bool interrupt = false;
};

template <>
struct internal::Requires<class DigitalInInterrupt> {
  static constexpr bool input = true;
  static constexpr bool output = false;
  static constexpr bool interrupt = true;
};

template <>
struct internal::Requires<class DigitalOut> {
  static constexpr bool input = false;
  static constexpr bool output = true;
  static constexpr bool interrupt = false;
};

template <>
struct internal::Requires<class DigitalOutInterrupt> {
  static constexpr bool input = false;
  static constexpr bool output = true;
  static constexpr bool interrupt = true;
};

template <>
struct internal::Requires<class DigitalInOut> {
  static constexpr bool input = true;
  static constexpr bool output = true;
  static constexpr bool interrupt = false;
};

template <>
struct internal::Requires<class DigitalInOutInterrupt> {
  static constexpr bool input = true;
  static constexpr bool output = true;
  static constexpr bool interrupt = true;
};

}  // namespace pw::digital_io
