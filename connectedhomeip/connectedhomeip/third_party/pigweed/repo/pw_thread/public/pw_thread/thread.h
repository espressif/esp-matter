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

#include "pw_thread/id.h"
#include "pw_thread/thread_core.h"

// clang-format off
// The backend's thread_native header must provide PW_THREAD_JOINING_ENABLED.
#include "pw_thread_backend/thread_native.h"
// clang-format on

namespace pw::thread {

// The Options contains the parameters needed for a thread to start.
//
// Options are backend specific and ergo the generic base class cannot be
// directly instantiated.
//
// The attributes which can be set through the options are backend specific
// but may contain things like the thread name, priority, scheduling policy,
// core/processor affinity, and/or an optional reference to a pre-allocated
// Context (the collection of memory allocations needed for a thread to run).
//
// Options shall NOT have an attribute to start threads as detached vs joinable.
// All `pw::thread::Thread` instances must be explicitly `join()`'d or
// `detach()`'d through the run-time Thread API.
//
// Note that if backends set `PW_THREAD_JOINING_ENABLED` to false, backends may
// use native OS specific APIs to create native detached threads because the
// `join()` API would be compiled out. However, users must still explicitly
// invoke `detach()`.
//
// Options must not contain any memory needed for a thread to run (TCB,
// stack, etc.). The Options may be deleted or re-used immediately after
// starting a thread.
class Options {
 protected:
  // We can't use `= default` here, because it allows to create an Options
  // instance in C++17 with `pw::thread::Options{}` syntax.
  constexpr Options() {}
};

// The class Thread can represent a single thread of execution. Threads allow
// multiple functions to execute concurrently.
//
// Threads may begin execution immediately upon construction of the associated
// thread object (pending any OS scheduling delays), starting at the top-level
// function provided as a constructor argument. The return value of the
// top-level function is ignored. The top-level function may communicate its
// return value by modifying shared variables (which may require
// synchronization, see pw_sync and std::atomic)
//
// Thread objects may also be in the state that does not represent any thread
// (after default construction, move from, detach, or join), and a thread of
// execution may be not associated with any thread objects (after detach).
//
// No two Thread objects may represent the same thread of execution; Thread is
// not CopyConstructible or CopyAssignable, although it is MoveConstructible and
// MoveAssignable.
class Thread {
 public:
  using native_handle_type = backend::NativeThreadHandle;

  // Creates a new thread object which does not represent a thread of execution
  // yet.
  Thread();

  // Creates a new thread object which represents a thread of execution.
  //
  // Thread functions are permitted to return and must have the following
  // ThreadRoutine signature:
  //   void example_function(void *arg);
  //
  // To invoke a member method of a class a static lambda closure can be used
  // to ensure the dispatching closure is not destructed before the thread is
  // done executing. For example:
  //   class Foo {
  //    public:
  //     void DoBar() {}
  //   };
  //   Foo foo;
  //
  //   static auto invoke_foo_do_bar = [](void *void_foo_ptr) {
  //       //  If needed, additional arguments could be set here.
  //       static_cast<Foo*>(void_foo_ptr)->DoBar();
  //   };
  //
  //   // Now use the lambda closure as the thread entry, passing the foo's
  //   // this as the argument.
  //   Thread thread(options, invoke_foo_do_bar, &foo);
  //   thread.detach();
  //
  // Alternatively a helper ThreadCore interface can be implemented by an object
  // so that a static lambda closure or function is not needed to dispatch to
  // a member function without arguments. For example:
  //   class Foo : public ThreadCore {
  //    private:
  //     void Run() override {}
  //   };
  //   Foo foo;
  //
  //   // Now create the thread, using foo directly.
  //   Thread(options, foo).detach();
  //
  // Postcondition: The thread get EITHER detached or joined.
  //
  // NOTE: Options have a default constructor, however default options are not
  // portable! Default options can only work if threads are dynamically
  // allocated by default, meaning default options cannot work on backends which
  // require static thread allocations. In addition on some schedulers
  // default options may not work for other reasons.
  using ThreadRoutine = void (*)(void* arg);
  Thread(const Options& options, ThreadRoutine entry, void* arg = nullptr);
  Thread(const Options& options, ThreadCore& thread_core);

  // Postcondition: The other thread no longer represents a thread of execution.
  Thread& operator=(Thread&& other);

  // Precondition: The thread must have been EITHER detached or joined.
  ~Thread();

  Thread(const Thread&) = delete;
  Thread(Thread&&) = delete;
  Thread& operator=(const Thread&) = delete;

  // Returns a value of Thread::id identifying the thread associated with *this.
  // If there is no thread associated, default constructed Thread::id is
  // returned.
  Id get_id() const;

  // Checks if the Thread object identifies an active thread of execution which
  // has not yet been detached. Specifically, returns true if get_id() !=
  // pw::Thread::id() && detach() has NOT been invoked. So a default
  // constructed thread is not joinable and neither is one which was detached.
  //
  // A thread that has not started or has finished executing code which was
  // never detached, but has not yet been joined is still considered an active
  // thread of execution and is therefore joinable.
  bool joinable() const { return get_id() != Id(); }

#if PW_THREAD_JOINING_ENABLED
  // Blocks the current thread until the thread identified by *this finishes its
  // execution.
  //
  // The completion of the thread identified by *this synchronizes with the
  // corresponding successful return from join().
  //
  // No synchronization is performed on *this itself. Concurrently calling
  // join() on the same thread object from multiple threads constitutes a data
  // race that results in undefined behavior.
  //
  // Precondition: The thread must have been NEITHER detached nor joined.
  //
  // Postcondition: After calling detach *this no longer owns any thread.
  void join();
#else
  template <typename kUnusedType = void>
  void join() {
    static_assert(kJoiningEnabled<kUnusedType>,
                  "The selected pw_thread_THREAD backend does not have join() "
                  "enabled (AKA PW_THREAD_JOINING_ENABLED = 1)");
  }
#endif  // PW_THREAD_JOINING_ENABLED

  // Separates the thread of execution from the thread object, allowing
  // execution to continue independently. Any allocated resources will be freed
  // once the thread exits.
  //
  // Precondition: The thread must have been NEITHER detached nor joined.
  //
  // Postcondition: After calling detach *this no longer owns any thread.
  void detach();

  // Exchanges the underlying handles of two thread objects.
  void swap(Thread& other);

  native_handle_type native_handle();

 private:
  template <typename...>
  static constexpr std::bool_constant<PW_THREAD_JOINING_ENABLED>
      kJoiningEnabled = {};

  // Note that just like std::thread, this is effectively just a pointer or
  // reference to the native thread -- this does not contain any memory needed
  // for the thread to execute.
  //
  // This may contain more than the native thread handle to enable functionality
  // which is not always available such as joining, which may require a
  // reference to a binary semaphore, or passing arguments to the thread's
  // function.
  backend::NativeThread native_type_;
};

}  // namespace pw::thread

#include "pw_thread_backend/thread_inline.h"
