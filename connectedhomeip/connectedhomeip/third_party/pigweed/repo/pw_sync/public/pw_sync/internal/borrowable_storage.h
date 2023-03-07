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

namespace pw::sync::internal {

// BorrowableStorage stores an object and associated lock. Both objects are
// constructed in-place.
template <typename ObjectType, typename Lock>
class BorrowableStorage {
 protected:
  // Construct the object in-place using a list of arguments.
  template <typename... Args>
  constexpr explicit BorrowableStorage(std::in_place_t, Args&&... args)
      : object_{std::forward<Args>(args)...}, lock_{} {}

  // Construct the object and lock in-place using the provided parameters.
  template <typename... ObjectArgs, typename... LockArgs>
  constexpr BorrowableStorage(std::tuple<ObjectArgs...>&& object_args,
                              std::tuple<LockArgs...>&& lock_args)
      : object_{std::make_from_tuple<ObjectType>(
            std::forward<std::tuple<ObjectArgs...>>(object_args))},
        lock_{std::make_from_tuple<Lock>(
            std::forward<std::tuple<LockArgs...>>(lock_args))} {}

  // Construct the object and lock in-place using the provided factories.
  template <typename ObjectConstructor,
            typename LockConstructor,
            typename = std::enable_if_t<
                std::is_invocable_r_v<ObjectType&&, ObjectConstructor>>,
            typename = std::enable_if_t<
                std::is_invocable_r_v<Lock&&, LockConstructor>>>
  constexpr BorrowableStorage(const ObjectConstructor& object_ctor,
                              const LockConstructor& lock_ctor)
      : object_{object_ctor()}, lock_{lock_ctor()} {}

  template <typename ObjectConstructor,
            typename LockConstructor,
            typename = std::enable_if_t<
                std::is_invocable_r_v<ObjectType&&, ObjectConstructor>>,
            typename = std::enable_if_t<
                std::is_invocable_r_v<Lock&&, LockConstructor>>>
  constexpr BorrowableStorage(ObjectConstructor& object_ctor,
                              const LockConstructor& lock_ctor)
      : object_{object_ctor()}, lock_{lock_ctor()} {}

  template <typename ObjectConstructor,
            typename LockConstructor,
            typename = std::enable_if_t<
                std::is_invocable_r_v<ObjectType&&, ObjectConstructor>>,
            typename = std::enable_if_t<
                std::is_invocable_r_v<Lock&&, LockConstructor>>>
  constexpr BorrowableStorage(const ObjectConstructor& object_ctor,
                              LockConstructor& lock_ctor)
      : object_{object_ctor()}, lock_{lock_ctor()} {}

  template <typename ObjectConstructor,
            typename LockConstructor,
            typename = std::enable_if_t<
                std::is_invocable_r_v<ObjectType&&, ObjectConstructor>>,
            typename = std::enable_if_t<
                std::is_invocable_r_v<Lock&&, LockConstructor>>>
  constexpr BorrowableStorage(ObjectConstructor& object_ctor,
                              LockConstructor& lock_ctor)
      : object_{object_ctor()}, lock_{lock_ctor()} {}

  ObjectType object_;
  Lock lock_;
};

template <typename T>
T DefaultConstruct() {
  return T();
}

}  // namespace pw::sync::internal
