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

#include <cstddef>
#include <utility>

#include "pw_intrusive_ptr/internal/ref_counted_base.h"

namespace pw {

// Shared pointer that relies on the stored object for the refcounting.
//
// T should be either a subclass of `RefCounted` (preferred way) or
// implement AddRef()/ReleaseRef() by itself.
//
// IntrusivePtr API follows the std::shared_ptr API, but doesn't provide weak
// pointers and some of the functionality such as reset(), owner_before(),
// operator[] or unique().
//
// Similar to the std::make_shared for the std::shared_ptr, IntrusivePtr
// provides the MakeRefCounted() helper.
//
// IntrusivePtr by itself doesn't provide any thread-safety guarantees but if T
// is a subclass from `RefCounted` - it is guaranteed to have atomic reference
// counter operations.
template <typename T>
class IntrusivePtr final {
 public:
  using element_type = T;

  // Constructs an empty IntrusivePtr.
  constexpr IntrusivePtr() : ptr_(nullptr) {}

  // Constructs an empty IntrusivePtr.
  //
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr IntrusivePtr(std::nullptr_t) : IntrusivePtr() {}

  // Constructs an IntrusivePtr from already allocated pointer.
  //
  // IntrusivePtr owns this pointer after this wrapping. All operations with the
  // pointer should be done through IntrusivePtr after the wrapping or while at
  // least one IntrusivePtr object owning it is in scope.
  //
  // Only heap-allocated pointers should be used with IntrusivePtr. An attempt
  // to wrap the stack-allocated object with the IntrusivePtr will result in a
  // crash on the destruction.
  explicit IntrusivePtr(T* p) : ptr_(p) {
    if (ptr_) {
      ptr_->AddRef();
    }
  }

  IntrusivePtr(const IntrusivePtr& other) : IntrusivePtr(other.ptr_) {}

  IntrusivePtr(IntrusivePtr&& other) noexcept
      : ptr_(std::exchange(other.ptr_, nullptr)) {}

  template <typename U>
  // NOLINTNEXTLINE(google-explicit-constructor)
  IntrusivePtr(const IntrusivePtr<U>& other) : IntrusivePtr(other.ptr_) {
    CheckConversionAllowed<U>();
  }

  template <typename U>
  // NOLINTNEXTLINE(google-explicit-constructor)
  IntrusivePtr(IntrusivePtr<U>&& other)
      : ptr_(std::exchange(other.ptr_, nullptr)) {
    CheckConversionAllowed<U>();
  }

  IntrusivePtr& operator=(const IntrusivePtr& other) {
    if (&other == this) {
      return *this;
    }
    IntrusivePtr(other).swap(*this);
    return *this;
  }

  IntrusivePtr& operator=(IntrusivePtr&& other) noexcept {
    if (&other == this) {
      return *this;
    }
    IntrusivePtr(std::move(other)).swap(*this);
    return *this;
  }

  ~IntrusivePtr() {
    if (ptr_ && ptr_->ReleaseRef()) {
      delete ptr_;
    }
  }

  void swap(IntrusivePtr& other) { std::swap(ptr_, other.ptr_); }

  T* get() const { return ptr_; }

  int32_t use_count() const { return ptr_ ? ptr_->ref_count() : 0; }

  T& operator*() const { return *ptr_; }

  T* operator->() const { return ptr_; }

  explicit operator bool() const { return ptr_; }

 private:
  template <typename U>
  friend class IntrusivePtr;

  // Compilation-time verification that we can convert from IntrusivePtr<U> to
  // IntrusivePtr<T>.
  template <typename U>
  constexpr void CheckConversionAllowed() {
    static_assert(
        std::is_convertible_v<U*, T*> &&
            (std::has_virtual_destructor_v<T> || std::is_same_v<T, const U>),
        "Cannot convert IntrusivePtr<U> to IntrusivePtr<T> unless T has a "
        "virtual destructor or T == const U.");
  }

  T* ptr_;
};

// Base class to be used with the IntrusivePtr. Doesn't provide any public
// methods.
//
// Provides an atomic-based reference counting. Atomics are used irrespective of
// the settings, which makes it different from the std::shared_ptr (that relies
// on the threading support settings to determine if atomics should be used for
// the control block or not).
//
// RefCounted MUST never be used as a pointer type to store derived objects -
// it doesn't provide a virtual destructor.
template <typename T>
class RefCounted : private internal::RefCountedBase {
 public:
  // Type alias for the IntrusivePtr of ref-counted type.
  using Ptr = IntrusivePtr<T>;

 private:
  template <typename U>
  friend class IntrusivePtr;
};

template <typename T, typename U>
inline bool operator==(const IntrusivePtr<T>& lhs, const IntrusivePtr<U>& rhs) {
  return lhs.get() == rhs.get();
}

template <typename T, typename U>
inline bool operator!=(const IntrusivePtr<T>& lhs, const IntrusivePtr<U>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
inline bool operator==(const IntrusivePtr<T>& ptr, std::nullptr_t) {
  return ptr.get() == nullptr;
}

template <typename T>
inline bool operator!=(const IntrusivePtr<T>& ptr, std::nullptr_t) {
  return ptr.get() != nullptr;
}

template <typename T>
inline bool operator==(std::nullptr_t, const IntrusivePtr<T>& ptr) {
  return ptr.get() == nullptr;
}

template <typename T>
inline bool operator!=(std::nullptr_t, const IntrusivePtr<T>& ptr) {
  return ptr.get() != nullptr;
}

// Constructs an IntrusivePtr<T> with a given set of arguments for the T
// constructor.
template <typename T, typename... Args>
IntrusivePtr<T> MakeRefCounted(Args&&... args) {
  return IntrusivePtr(new T(std::forward<Args>(args)...));
}

}  // namespace pw
