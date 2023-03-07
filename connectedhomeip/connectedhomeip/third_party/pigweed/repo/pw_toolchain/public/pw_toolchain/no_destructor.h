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

#include <new>
#include <type_traits>
#include <utility>

namespace pw {

// Helper type to create a function-local static variable of type T when T has a
// non-trivial destructor. Storing a T in a pw::NoDestructor<T> will prevent
// ~T() from running, even when the variable goes out of scope.
//
// This class is useful when a variable has static storage duration but its type
// has a non-trivial destructor. Destructor ordering is not defined and can
// cause issues in multithreaded environments. Additionally, removing destructor
// calls can save code size.
//
// Except in generic code, do not use pw::NoDestructor<T> with trivially
// destructible types. Use the type directly instead. If the variable can be
// constexpr, make it constexpr.
//
// pw::NoDestructor<T> provides a similar API to std::optional. Use * or -> to
// access the wrapped type.
//
// Example usage:
//
//   pw::sync::Mutex& GetMutex() {
//     // Use NoDestructor to avoid running the mutex destructor when exit-time
//     // destructors run.
//     static const pw::NoDestructor<pw::sync::Mutex> global_mutex;
//     return *global_mutex;
//   }
//
// WARNING: Misuse of NoDestructor can cause memory leaks and other problems.
// Only skip destructors when you know it is safe to do so.
//
// In Clang, pw::NoDestructor can be replaced with the [[clang::no_destroy]]
// attribute.
template <typename T>
class NoDestructor {
 public:
  using value_type = T;

  // Initializes a T in place.
  //
  // This overload is disabled when it might collide with copy/move.
  template <typename... Args,
            typename std::enable_if<!std::is_same<void(std::decay_t<Args>&...),
                                                  void(NoDestructor&)>::value,
                                    int>::type = 0>
  explicit constexpr NoDestructor(Args&&... args)
      : storage_(std::forward<Args>(args)...) {}

  // Move or copy from the contained type. This allows for construction from an
  // initializer list, e.g. for std::vector.
  explicit constexpr NoDestructor(const T& x) : storage_(x) {}
  explicit constexpr NoDestructor(T&& x) : storage_(std::move(x)) {}

  NoDestructor(const NoDestructor&) = delete;
  NoDestructor& operator=(const NoDestructor&) = delete;

  ~NoDestructor() = default;

  const T& operator*() const { return *storage_.get(); }
  T& operator*() { return *storage_.get(); }

  const T* operator->() const { return storage_.get(); }
  T* operator->() { return storage_.get(); }

 private:
  class DirectStorage {
   public:
    template <typename... Args>
    explicit constexpr DirectStorage(Args&&... args)
        : value_(std::forward<Args>(args)...) {}

    const T* get() const { return &value_; }
    T* get() { return &value_; }

   private:
    T value_;
  };

  class PlacementStorage {
   public:
    template <typename... Args>
    explicit PlacementStorage(Args&&... args) {
      new (&memory_) T(std::forward<Args>(args)...);
    }

    const T* get() const {
      return std::launder(reinterpret_cast<const T*>(&memory_));
    }
    T* get() { return std::launder(reinterpret_cast<T*>(&memory_)); }

   private:
    alignas(T) char memory_[sizeof(T)];
  };

  // If the type is already trivially destructible, use it directly. Trivially
  // destructible types do not need NoDestructor, but NoDestructor supports them
  // to work better with generic code.
  std::conditional_t<std::is_trivially_destructible<T>::value,
                     DirectStorage,
                     PlacementStorage>
      storage_;
};

}  // namespace pw
