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

#include "pw_thread/thread.h"

namespace pw::thread {

// Creates a detached a thread of execution.
//
// This is a very simple helper wrapper around Thread to help the common case of
// creating a Thread which is immediately detached. For example instead of:
//
//   Thread(options, foo).detach();
//
// You can instead use this helper wrapper to:
//
//   DetachedThread(options, foo);
//
// The arguments are directly forwarded to the Thread constructor and ergo
// exactly match the Thread constuctor arguments for creating a thread of
// execution.
template <class... Args>
void DetachedThread(const Options& options, Args&&... args) {
  Thread(options, std::forward<Args>(args)...).detach();
}

}  // namespace pw::thread
