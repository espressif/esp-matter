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
//
// This header file contains macro definitions for thread safety annotations
// that allow developers to document the locking policies of multi-threaded
// code. The annotations can also help program analysis tools to identify
// potential thread safety issues.
//
// These annotations are implemented using compiler attributes. Using the macros
// defined here instead of raw attributes allow for portability and future
// compatibility.
//
// The thread safety analysis system is documented at
// http://clang.llvm.org/docs/ThreadSafetyAnalysis.html
//
// When referring to locks in the arguments of the attributes, you should
// use variable names or more complex expressions (e.g. my_object->lock_)
// that evaluate to a concrete lock object whenever possible. If the lock
// you want to refer to is not in scope, you may use a member pointer
// (e.g. &MyClass::lock_) to refer to a lock in some (unknown) object.

#pragma once

#include "pw_preprocessor/compiler.h"

// PW_GUARDED_BY()
//
// Documents if a shared field or global variable needs to be protected by a
// lock. PW_GUARDED_BY() allows the user to specify a particular lock that
// should be held when accessing the annotated variable.
//
// Although this annotation (and PW_PT_GUARDED_BY, below) cannot be applied to
// local variables, a local variable and its associated lock can often be
// combined into a small class or struct, thereby allowing the annotation.
//
// Example:
//
//   class Foo {
//     Mutex mu_;
//     int p1_ PW_GUARDED_BY(mu_);
//     ...
//   };
#if PW_HAVE_ATTRIBUTE(guarded_by)
#define PW_GUARDED_BY(x) __attribute__((guarded_by(x)))
#else
#define PW_GUARDED_BY(x)
#endif

// PW_PT_GUARDED_BY()
//
// Documents if the memory location pointed to by a pointer should be guarded
// by a lock when dereferencing the pointer.
//
// Example:
//   class Foo {
//     Mutex mu_;
//     int *p1_ PW_PT_GUARDED_BY(mu_);
//     ...
//   };
//
// Note that a pointer variable to a shared memory location could itself be a
// shared variable.
//
// Example:
//
//   // `q_`, guarded by `mu1_`, points to a shared memory location that is
//   // guarded by `mu2_`:
//   int *q_ PW_GUARDED_BY(mu1_) PW_PT_GUARDED_BY(mu2_);
#if PW_HAVE_ATTRIBUTE(pt_guarded_by)
#define PW_PT_GUARDED_BY(x) __attribute__((pt_guarded_by(x)))
#else
#define PW_PT_GUARDED_BY(x)
#endif

// PW_ACQUIRED_AFTER() / PW_ACQUIRED_BEFORE()
//
// Documents the acquisition order between locks that can be held
// simultaneously by a thread. For any two locks that need to be annotated
// to establish an acquisition order, only one of them needs the annotation.
// (i.e. You don't have to annotate both locks with both PW_ACQUIRED_AFTER
// and PW_ACQUIRED_BEFORE.)
//
// As with PW_GUARDED_BY, this is only applicable to locks that are shared
// fields or global variables.
//
// Example:
//
//   Mutex m1_;
//   Mutex m2_ PW_ACQUIRED_AFTER(m1_);
#if PW_HAVE_ATTRIBUTE(acquired_after)
#define PW_ACQUIRED_AFTER(...) __attribute__((acquired_after(__VA_ARGS__)))
#else
#define PW_ACQUIRED_AFTER(...)
#endif

#if PW_HAVE_ATTRIBUTE(acquired_before)
#define PW_ACQUIRED_BEFORE(...) __attribute__((acquired_before(__VA_ARGS__)))
#else
#define PW_ACQUIRED_BEFORE(...)
#endif

// PW_EXCLUSIVE_LOCKS_REQUIRED() / PW_SHARED_LOCKS_REQUIRED()
//
// Documents a function that expects a lock to be held prior to entry.
// The lock is expected to be held both on entry to, and exit from, the
// function.
//
// An exclusive lock allows read-write access to the guarded data member(s), and
// only one thread can acquire a lock exclusively at any one time. A shared lock
// allows read-only access, and any number of threads can acquire a shared lock
// concurrently.
//
// Generally, non-const methods should be annotated with
// PW_EXCLUSIVE_LOCKS_REQUIRED, while const methods should be annotated with
// PW_SHARED_LOCKS_REQUIRED.
//
// Example:
//
//   Mutex mu1, mu2;
//   int a PW_GUARDED_BY(mu1);
//   int b PW_GUARDED_BY(mu2);
//
//   void foo() PW_EXCLUSIVE_LOCKS_REQUIRED(mu1, mu2) { ... }
//   void bar() const PW_SHARED_LOCKS_REQUIRED(mu1, mu2) { ... }
#if PW_HAVE_ATTRIBUTE(exclusive_locks_required)
#define PW_EXCLUSIVE_LOCKS_REQUIRED(...) \
  __attribute__((exclusive_locks_required(__VA_ARGS__)))
#else
#define PW_EXCLUSIVE_LOCKS_REQUIRED(...)
#endif

#if PW_HAVE_ATTRIBUTE(shared_locks_required)
#define PW_SHARED_LOCKS_REQUIRED(...) \
  __attribute__((shared_locks_required(__VA_ARGS__)))
#else
#define PW_SHARED_LOCKS_REQUIRED(...)
#endif

// PW_LOCKS_EXCLUDED()
//
// Documents that the caller must not hold the given lock. This annotation is
// often used to prevent deadlocks. Pigweed's mutex implementation is not
// re-entrant, so a deadlock will occur if the function acquires the mutex a
// second time.
#if PW_HAVE_ATTRIBUTE(locks_excluded)
#define PW_LOCKS_EXCLUDED(...) __attribute__((locks_excluded(__VA_ARGS__)))
#else
#define PW_LOCKS_EXCLUDED(...)
#endif

// PW_LOCK_RETURNED()
//
// Documents a function that returns a lock without acquiring it.  For example,
// a public getter method that returns a pointer to a private lock should
// be annotated with PW_LOCK_RETURNED.
#if PW_HAVE_ATTRIBUTE(lock_returned)
#define PW_LOCK_RETURNED(x) __attribute__((lock_returned(x)))
#else
#define PW_LOCK_RETURNED(x)
#endif

// PW_LOCKABLE(name)
//
// Documents if a class/type is a lockable type (such as the `pw::sync::Mutex`
// class). The name is used in the warning messages.
#if PW_HAVE_ATTRIBUTE(capability)
#define PW_LOCKABLE(name) __attribute__((capability(name)))
#elif PW_HAVE_ATTRIBUTE(lockable)
#define PW_LOCKABLE(name) __attribute__((lockable))
#else
#define PW_LOCKABLE(name)
#endif

// PW_SCOPED_LOCKABLE
//
// Documents if a class does RAII locking. The name is used in the warning
// messages.
//
// The constructor should use `LOCK_FUNCTION()` to specify the lock that is
// acquired, and the destructor should use `UNLOCK_FUNCTION()` with no
// arguments; the analysis will assume that the destructor unlocks whatever the
// constructor locked.
#if PW_HAVE_ATTRIBUTE(scoped_lockable)
#define PW_SCOPED_LOCKABLE __attribute__((scoped_lockable))
#else
#define PW_SCOPED_LOCKABLE
#endif

// PW_EXCLUSIVE_LOCK_FUNCTION()
//
// Documents functions that acquire a lock in the body of a function, and do
// not release it.
#if PW_HAVE_ATTRIBUTE(exclusive_lock_function)
#define PW_EXCLUSIVE_LOCK_FUNCTION(...) \
  __attribute__((exclusive_lock_function(__VA_ARGS__)))
#else
#define PW_EXCLUSIVE_LOCK_FUNCTION(...)
#endif

// PW_SHARED_LOCK_FUNCTION()
//
// Documents functions that acquire a shared (reader) lock in the body of a
// function, and do not release it.
#if PW_HAVE_ATTRIBUTE(shared_lock_function)
#define PW_SHARED_LOCK_FUNCTION(...) \
  __attribute__((shared_lock_function(__VA_ARGS__)))
#else
#define PW_SHARED_LOCK_FUNCTION(...)
#endif

// PW_UNLOCK_FUNCTION()
//
// Documents functions that expect a lock to be held on entry to the function,
// and release it in the body of the function.
#if PW_HAVE_ATTRIBUTE(unlock_function)
#define PW_UNLOCK_FUNCTION(...) __attribute__((unlock_function(__VA_ARGS__)))
#else
#define PW_UNLOCK_FUNCTION(...)
#endif

// PW_EXCLUSIVE_TRYLOCK_FUNCTION() / PW_SHARED_TRYLOCK_FUNCTION()
//
// Documents functions that try to acquire a lock, and return success or failure
// (or a non-boolean value that can be interpreted as a boolean).
// The first argument should be `true` for functions that return `true` on
// success, or `false` for functions that return `false` on success. The second
// argument specifies the lock that is locked on success. If unspecified, this
// lock is assumed to be `this`.
#if PW_HAVE_ATTRIBUTE(exclusive_trylock_function)
#define PW_EXCLUSIVE_TRYLOCK_FUNCTION(...) \
  __attribute__((exclusive_trylock_function(__VA_ARGS__)))
#else
#define PW_EXCLUSIVE_TRYLOCK_FUNCTION(...)
#endif

#if PW_HAVE_ATTRIBUTE(shared_trylock_function)
#define PW_SHARED_TRYLOCK_FUNCTION(...) \
  __attribute__((shared_trylock_function(__VA_ARGS__)))
#else
#define PW_SHARED_TRYLOCK_FUNCTION(...)
#endif

// PW_ASSERT_EXCLUSIVE_LOCK() / PW_ASSERT_SHARED_LOCK()
//
// Documents functions that dynamically check to see if a lock is held, and fail
// if it is not held.
#if PW_HAVE_ATTRIBUTE(assert_exclusive_lock)
#define PW_ASSERT_EXCLUSIVE_LOCK(...) \
  __attribute__((assert_exclusive_lock(__VA_ARGS__)))
#else
#define PW_ASSERT_EXCLUSIVE_LOCK(...)
#endif

#if PW_HAVE_ATTRIBUTE(assert_shared_lock)
#define PW_ASSERT_SHARED_LOCK(...) \
  __attribute__((assert_shared_lock(__VA_ARGS__)))
#else
#define PW_ASSERT_SHARED_LOCK(...)
#endif

// PW_NO_LOCK_SAFETY_ANALYSIS
//
// Turns off thread safety checking within the body of a particular function.
// This annotation is used to mark functions that are known to be correct, but
// the locking behavior is more complicated than the analyzer can handle.
#if PW_HAVE_ATTRIBUTE(no_thread_safety_analysis)
#define PW_NO_LOCK_SAFETY_ANALYSIS __attribute__((no_thread_safety_analysis))
#else
#define PW_NO_LOCK_SAFETY_ANALYSIS
#endif
