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

#include "pw_thread_embos/context.h"

namespace pw::thread::backend {

// The native thread is a pointer to a thread's context.
using NativeThread = pw::thread::embos::Context*;

// The native thread handle is the same as the NativeThread.
using NativeThreadHandle = pw::thread::embos::Context*;

}  // namespace pw::thread::backend
