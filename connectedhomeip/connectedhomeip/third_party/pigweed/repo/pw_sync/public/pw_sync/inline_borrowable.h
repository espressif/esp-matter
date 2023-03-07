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

#include <tuple>
#include <utility>

#include "pw_sync/borrow.h"
#include "pw_sync/internal/borrowable_storage.h"
#include "pw_sync/mutex.h"
#include "pw_sync/virtual_basic_lockable.h"

namespace pw::sync {

// InlineBorrowable holds an object of GuardedType and a Lock that guards
// access to the object. It should be used when an object should be guarded for
// its entire lifecycle by a single lock.
//
// This object should be shared with other componetns as a reference of type
// Borrowable<GuardedType, LockInterface>.
//
template <typename GuardedType,
          typename Lock = pw::sync::VirtualMutex,
          typename LockInterface = pw::sync::VirtualBasicLockable>
class InlineBorrowable : private internal::BorrowableStorage<GuardedType, Lock>,
                         public Borrowable<GuardedType, LockInterface> {
  using Storage = internal::BorrowableStorage<GuardedType, Lock>;
  using Base = Borrowable<GuardedType, LockInterface>;

 public:
  // Construct the guarded object and lock using their default constructors.
  constexpr InlineBorrowable()
      : Storage(std::in_place), Base(Storage::object_, Storage::lock_) {}

  // Construct the guarded object by providing its constructor arguments inline.
  //
  // This constructor supports list initialization for arrays, structs, and
  // other objects such as std::array.
  //
  // Example:
  //
  //   InlineBorrowable<Foo> foo(std::in_place, foo_arg1, foo_arg2);
  //
  //   InlineBorrowable<std::array<int, 2>> foo_array(std::in_place, 1, 2);
  //
  template <typename... Args>
  constexpr explicit InlineBorrowable(std::in_place_t, Args&&... args)
      : Storage(std::in_place, std::forward<Args>(args)...),
        Base(Storage::object_, Storage::lock_) {}

  // Construct the guarded object and lock by providing their construction
  // parameters using separate tuples.
  //
  // Example:
  //
  //   InlineBorrowable<Foo> foo(std::forward_as_tuple(foo_arg1, foo_arg2));
  //
  //   InlineBorrowable<Foo, MyLock> foo_lock(
  //       std::forward_as_tuple(foo_arg1, foo_arg2),
  //       std::forward_as_tuple(lock_arg1, lock_arg2));
  //
  // Note: this constructor only supports list initialization with C++20 or
  // later, because it requires https://wg21.link/p0960.
  //
  template <typename... ObjectArgs, typename... LockArgs>
  constexpr explicit InlineBorrowable(
      std::tuple<ObjectArgs...>&& object_args,
      std::tuple<LockArgs...>&& lock_args = std::make_tuple())
      : Storage(std::forward<std::tuple<ObjectArgs...>>(object_args),
                std::forward<std::tuple<LockArgs...>>(lock_args)),
        Base(Storage::object_, Storage::lock_) {}

  // Construct the guarded object and lock by providing factory functions.
  //
  // Example:
  //
  //    InlineBorrowable<Foo> foo([&]{ return Foo{foo_arg1, foo_arg2}; });
  //
  //    InlineBorrowable<Foo, MyLock> foo_lock(
  //        [&]{ return Foo{foo_arg1, foo_arg2}; }
  //        [&]{ return MyLock{lock_arg1, lock_arg2}; }
  //
  template <typename ObjectConstructor,
            typename LockConstructor = Lock(),
            typename = std::enable_if_t<
                std::is_invocable_r_v<GuardedType&&, ObjectConstructor>>,
            typename = std::enable_if_t<
                std::is_invocable_r_v<Lock&&, LockConstructor>>>
  constexpr explicit InlineBorrowable(
      const ObjectConstructor& object_ctor,
      const LockConstructor& lock_ctor = internal::DefaultConstruct<Lock>)
      : Storage(object_ctor, lock_ctor),
        Base(Storage::object_, Storage::lock_) {}

  template <typename ObjectConstructor,
            typename LockConstructor = Lock(),
            typename = std::enable_if_t<
                std::is_invocable_r_v<GuardedType&&, ObjectConstructor>>,
            typename = std::enable_if_t<
                std::is_invocable_r_v<Lock&&, LockConstructor>>>
  constexpr explicit InlineBorrowable(
      ObjectConstructor& object_ctor,
      const LockConstructor& lock_ctor = internal::DefaultConstruct<Lock>)
      : Storage(object_ctor, lock_ctor),
        Base(Storage::object_, Storage::lock_) {}

  template <typename ObjectConstructor,
            typename LockConstructor = Lock(),
            typename = std::enable_if_t<
                std::is_invocable_r_v<GuardedType&&, ObjectConstructor>>,
            typename = std::enable_if_t<
                std::is_invocable_r_v<Lock&&, LockConstructor>>>
  constexpr explicit InlineBorrowable(const ObjectConstructor& object_ctor,
                                      LockConstructor& lock_ctor)
      : Storage(object_ctor, lock_ctor),
        Base(Storage::object_, Storage::lock_) {}

  template <typename ObjectConstructor,
            typename LockConstructor = Lock(),
            typename = std::enable_if_t<
                std::is_invocable_r_v<GuardedType&&, ObjectConstructor>>,
            typename = std::enable_if_t<
                std::is_invocable_r_v<Lock&&, LockConstructor>>>
  constexpr explicit InlineBorrowable(ObjectConstructor& object_ctor,
                                      LockConstructor& lock_ctor)
      : Storage(object_ctor, lock_ctor),
        Base(Storage::object_, Storage::lock_) {}
};

}  // namespace pw::sync
