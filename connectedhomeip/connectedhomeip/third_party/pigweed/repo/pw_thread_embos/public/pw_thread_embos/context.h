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

#include <cstring>

#include "RTOS.h"
#include "pw_span/span.h"
#include "pw_string/util.h"
#include "pw_thread_embos/config.h"

namespace pw::thread {

class Thread;  // Forward declare Thread which depends on Context.

}  // namespace pw::thread

namespace pw::thread::embos {

// Static thread context allocation including the TCB, an event group for
// joining if enabled, and an external statically allocated stack.
//
// Example usage:
//
//   std::array<ULONG, kFooStackSizeWords> example_thread_stack;
//   pw::thread::embos::Context example_thread_context(example_thread_stack);
//   void StartExampleThread() {
//      pw::thread::DetachedThread(
//        pw::thread::embos::Options()
//            .set_name("static_example_thread")
//            .set_priority(kFooPriority)
//            .set_context(example_thread_context),
//        example_thread_function);
//   }
class Context {
 public:
  explicit Context(span<OS_UINT> stack_span)
      : tcb_{}, stack_span_(stack_span) {}
  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;

  // Intended for unit test & Thread use only.
  OS_TASK& tcb() { return tcb_; }

 private:
  friend Thread;

  span<OS_UINT> stack() { return stack_span_; }

  bool in_use() const { return in_use_; }
  void set_in_use(bool in_use = true) { in_use_ = in_use; }

  const char* name() const { return name_.data(); }
  void set_name(const char* name) { string::Copy(name, name_); }

  using ThreadRoutine = void (*)(void* arg);
  void set_thread_routine(ThreadRoutine entry, void* arg) {
    user_thread_entry_function_ = entry;
    user_thread_entry_arg_ = arg;
  }

  bool detached() const { return detached_; }
  void set_detached(bool value = true) { detached_ = value; }

  bool thread_done() const { return thread_done_; }
  void set_thread_done(bool value = true) { thread_done_ = value; }

#if PW_THREAD_JOINING_ENABLED
  OS_EVENT& join_event_object() { return event_object_; }
#endif  // PW_THREAD_JOINING_ENABLED

  static void ThreadEntryPoint(void* void_context_ptr);
  static void TerminateThread(Context& context);

  OS_TASK tcb_;
  span<OS_UINT> stack_span_;

  ThreadRoutine user_thread_entry_function_ = nullptr;
  void* user_thread_entry_arg_ = nullptr;
#if PW_THREAD_JOINING_ENABLED
  // Note that the embOS life cycle of this event object is managed together
  // with the thread life cycle, not this object's life cycle.
  OS_EVENT event_object_;
#endif  // PW_THREAD_JOINING_ENABLED
  bool in_use_ = false;
  bool detached_ = false;
  bool thread_done_ = false;

  // The TCB does not have storage for the name, ergo we provide storage for
  // the thread's name which can be truncated down to just a null delimiter.
  std::array<char, config::kMaximumNameLength + 1> name_;
};

// Static thread context allocation including the stack along with the Context.
//
// Example usage:
//
//   pw::thread::embos::ContextWithStack<kFooStackSizeWords>
//       example_thread_context;
//   void StartExampleThread() {
//      pw::thread::DetachedThread(
//        pw::thread::embos::Options()
//            .set_name("static_example_thread")
//            .set_priority(kFooPriority)
//            .set_context(example_thread_context),
//        example_thread_function);
//   }
template <size_t kStackSizeWords = config::kDefaultStackSizeWords>
class ContextWithStack final : public Context {
 public:
  constexpr ContextWithStack() : Context(stack_storage_) {
    static_assert(kStackSizeWords >= config::kMinimumStackSizeWords);
  }

 private:
  std::array<OS_UINT, kStackSizeWords> stack_storage_;
};

}  // namespace pw::thread::embos
