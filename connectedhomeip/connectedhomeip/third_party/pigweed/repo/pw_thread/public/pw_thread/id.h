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

#include "pw_thread_backend/id_native.h"

namespace pw::thread {

// The class thread::Id is a lightweight, trivially copyable class that serves
// as a unique identifier of Thread objects.
//
// Instances of this class may also hold the special distinct value that does
// not represent any thread. Once a thread has finished, the value of its
// thread::Id may be reused by another thread.
//
// This class is designed for use as key in associative containers, both
// ordered and unordered.
//
// The backend must ensure that:
// 1) There is a default construct which does not represent a thread.
// 2) Compare operators (==,!=,<,<=,>,>=) are provided to compare and sort IDs.
using Id = backend::NativeId;

}  // namespace pw::thread

namespace pw::this_thread {

// This is thread safe, not IRQ safe. It is implementation defined whether this
// is safe before the scheduler has started.
thread::Id get_id() noexcept;

}  // namespace pw::this_thread

// The backend can opt to include an inline implementation.
#if __has_include("pw_thread_backend/id_inline.h")
#include "pw_thread_backend/id_inline.h"
#endif  // __has_include("pw_thread_backend/id_inline.h")
