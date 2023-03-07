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

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

#include "pw_assert/assert.h"
#include "pw_checksum/crc16_ccitt.h"
#include "pw_preprocessor/compiler.h"
#include "pw_span/span.h"

namespace pw::persistent_ram {

// Behavior to use when attempting to get a handle to the underlying data stored
// in persistent memory.
enum class GetterAction {
  // Default-construct the object before returning a handle.
  kReset,
  // Assert that the object is valid before returning a handle.
  kAssertValid,
};

// The Persistent class intentionally uses uninitialized memory, which triggers
// compiler warnings. Disable those warnings for this file.
PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Wuninitialized");
PW_MODIFY_DIAGNOSTIC_GCC(ignored, "-Wmaybe-uninitialized");

// A simple container for holding a value T with CRC16 integrity checking.
//
// A Persistent is simply a value T plus integrity checking for use in a
// persistent RAM section which is not initialized on boot.
//
// WARNING: Unlike a DoubleBufferedPersistent, a Persistent will be lost if a
// write/set operation is interrupted or otherwise not completed.
//
// TODO(b/235277454): Consider a different integrity check implementation which
// does not use a 512B lookup table.
template <typename T>
class Persistent {
 public:
  // This object provides mutable access to the underlying object of a
  // Persistent<T>.
  //
  // WARNING: This object must remain in scope for any modifications of the
  // Underlying object. If the object is modified after the Mutator goes out
  // of scope, the CRC will not be updated to reflect changes, invalidating the
  // contents of the Persistent<T>!
  //
  // WARNING: Persistent<T>::has_value() will return false if there are
  // in-flight modifications by a Mutator that have not yet been flushed.
  class Mutator {
   public:
    explicit constexpr Mutator(Persistent<T>& persistent)
        : persistent_(persistent) {}
    ~Mutator() { persistent_.crc_ = persistent_.CalculateCrc(); }

    Mutator(const Mutator&) = delete;  // Copy constructor is disabled.

    T* operator->() { return const_cast<T*>(&persistent_.contents_); }

    // Be careful when sharing a reference or pointer to the underlying object.
    // Once the Mutator goes out of scope, any changes to the object will
    // invalidate the checksum. Avoid directly using the underlying object
    // unless you need to pass it to a function.
    T& value() { return persistent_.contents_; }
    T& operator*() { return *const_cast<T*>(&persistent_.contents_); }

   private:
    Persistent<T>& persistent_;
  };

  // Constructor which does nothing, meaning it never sets the value.
  constexpr Persistent() {}

  Persistent(const Persistent&) = delete;  // Copy constructor is disabled.
  Persistent(Persistent&&) = delete;       // Move constructor is disabled.
  ~Persistent() {}                         // The destructor does nothing.

  // Construct the value in-place.
  template <class... Args>
  const T& emplace(Args&&... args) {
    new (const_cast<T*>(&contents_)) T(std::forward<Args>(args)...);
    crc_ = CalculateCrc();
    return const_cast<T&>(contents_);
  }

  // Assignment operator.
  template <typename U = T>
  Persistent& operator=(U&& value) {
    contents_ = std::forward<U>(value);
    crc_ = CalculateCrc();
    return *this;
  }

  // Destroys any contained value.
  void Invalidate() {
    // The trivial destructor is skipped as it's trivial.
    std::memset(const_cast<T*>(&contents_), 0, sizeof(contents_));
    crc_ = 0;
  }

  // This is deprecated, use Invalidate() instead.
  [[deprecated]] void reset() { Invalidate(); }

  // Returns true if a value is held by the Persistent.
  bool has_value() const {
    return crc_ == CalculateCrc();  // There's a value if its CRC matches.
  }

  // Access the value.
  //
  // Precondition: has_value() must be true.
  const T& value() const {
    PW_ASSERT(has_value());
    return const_cast<T&>(contents_);
  }

  // Get a mutable handle to the underlying data.
  //
  // Args:
  //   action: Whether to default-construct the underlying value before
  //           providing a mutator, or to assert that the object is valid
  //           without modifying the underlying data.
  // Precondition: has_value() must be true.
  Mutator mutator(GetterAction action = GetterAction::kAssertValid) {
    if (action == GetterAction::kReset) {
      emplace();
    } else {
      PW_ASSERT(has_value());
    }
    return Mutator(*this);
  }

 private:
  friend class Mutator;

  static_assert(std::is_trivially_copy_constructible<T>::value,
                "If a Persistent persists across reboots, it is effectively "
                "loaded through a trivial copy constructor.");

  static_assert(std::is_trivially_destructible<T>::value,
                "A Persistent's destructor does not invoke the value's "
                "destructor, ergo only trivially destructible types are "
                "supported.");

  uint16_t CalculateCrc() const {
    return checksum::Crc16Ccitt::Calculate(
        as_bytes(span(const_cast<const T*>(&contents_), 1)));
  }

  // Use unions to denote that these members are never initialized by design and
  // on purpose. Volatile is used to ensure that the compiler cannot optimize
  // out operations where it seems like there is no further usage of a
  // Persistent as this may be on the next boot.
  union {
    volatile T contents_;
  };
  union {
    volatile uint16_t crc_;
  };
};

PW_MODIFY_DIAGNOSTICS_POP();

}  // namespace pw::persistent_ram
