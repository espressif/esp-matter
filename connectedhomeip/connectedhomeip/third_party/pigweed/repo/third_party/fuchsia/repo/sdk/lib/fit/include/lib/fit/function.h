// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_FIT_INCLUDE_LIB_FIT_FUNCTION_H_
#define LIB_FIT_INCLUDE_LIB_FIT_FUNCTION_H_

#include <type_traits>

#include "internal/function.h"
#include "internal/utility.h"
#include "traits.h"

namespace fit {

template <size_t inline_target_size, bool require_inline, typename FunctionType>
class function_impl {
  static_assert(std::is_function<FunctionType>::value,
                "fit::function must be instantiated with a function type, such as void() or "
                "int(char*, bool)");
};

template <size_t inline_target_size, bool require_inline, typename FunctionType>
class callback_impl {
  static_assert(std::is_function<FunctionType>::value,
                "fit::callback must be instantiated with a function type, such as void() or "
                "int(char*, bool)");
};

// The default size allowance for storing a target inline within a function
// object, in bytes.  This default allows for inline storage of targets
// as big as two pointers, such as an object pointer and a pointer to a member
// function.
constexpr size_t default_inline_target_size = sizeof(void*) * 2;

// A |fit::function| is a move-only polymorphic function wrapper.
//
// If you need a class with similar characteristics that also ensures
// "run-once" semantics (such as callbacks shared with timeouts, or for
// service requests with redundant, failover, or fallback service providers),
// see |fit::callback|.
//
// |fit::function<T>| behaves like |std::function<T>| except that it is
// move-only instead of copyable, so it can hold targets that cannot be copied,
// such as mutable lambdas, and immutable lambdas that capture move-only
// objects.
//
// Targets of up to |inline_target_size| bytes in size are stored inline within
// the function object without incurring any heap allocation. Larger callable
// objects will be moved to the heap as required. |inline_target_size| is
// rounded up to a multiple of sizeof(void*).
//
// See also |fit::inline_function<T, size>| for more control over allocation
// behavior.
//
// SYNOPSIS
//
// |T| is the function's signature.  e.g. void(int, std::string).
//
// |inline_target_size| is the minimum size of target that is guaranteed to
// fit within a function without requiring heap allocation.
// Defaults to |default_inline_target_size|.
//
// Class members are documented in |fit::function_impl|, below.
//
// EXAMPLES
//
// -
// https://fuchsia.googlesource.com/fuchsia/+/HEAD/sdk/lib/fit/test/examples/function_example1.cc
// -
// https://fuchsia.googlesource.com/fuchsia/+/HEAD/sdk/lib/fit/test/examples/function_example2.cc
//
template <typename T, size_t inline_target_size = default_inline_target_size>
using function = function_impl<internal::RoundUpToWord(inline_target_size),
                               /*require_inline=*/false, T>;

// A move-only callable object wrapper that forces callables to be stored inline
// and never performs heap allocation.
//
// Behaves just like |fit::function<T, inline_target_size>| except that
// attempting to store a target larger than |inline_target_size| will fail to
// compile.
template <typename T, size_t inline_target_size = default_inline_target_size>
using inline_function = function_impl<internal::RoundUpToWord(inline_target_size),
                                      /*require_inline=*/true, T>;

// Synonym for a function which takes no arguments and produces no result.
using closure = function<void()>;

// A |fit::callback| is a move-only polymorphic function wrapper that also
// ensures "run-once" semantics (such as callbacks shared with timeouts, or for
// service requests with redundant, failover, or fallback service providers).
// A |fit::callback| releases it's resources after the first call, and can be
// inspected before calling, so a potential caller can know if it should call
// the function, or skip the call because the target was already called.
//
// If you need a move-only function class with typical function characteristics,
// that permits multiple invocations of the same function, see |fit::function|.
//
// |fit::callback<T>| behaves like |std::function<T>| except:
//
//   1. It is move-only instead of copyable, so it can hold targets that cannot
//      be copied, such as mutable lambdas, and immutable lambdas that capture
//      move-only objects.
//   2. On the first call to invoke a |fit::callback|, the target function held
//      by the |fit::callback| cannot be called again.
//
// When a |fit::callback| is invoked for the first time, the target function is
// released and destructed, along with any resources owned by that function
// (typically the objects captured by a lambda).
//
// A |fit::callback| in the "already called" state has the same state as a
// |fit::callback| that has been assigned to |nullptr|. It can be compared to
// |nullptr| (via "==" or "!=", and its "operator bool()" returns false, which
// provides a convenient way to gate whether or not the |fit::callback| should
// be called. (Note that invoking an empty |fit::callback| or |fit::function|
// will cause a program abort!)
//
// As an example, sharing |fit::callback| between both a service and a timeout
// might look something like this:
//
//  void service_with_timeout(fit::callback<void(bool)> cb, uint timeout_ms) {
//    service_request([cb = cb.share()]() mutable { if (cb) cb(false); });
//    timeout(timeout_ms, [cb = std::move(cb)]() mutable { if (cb) cb(true); });
//  }
//
// Since |fit::callback| objects are move-only, and not copyable, duplicate
// references to the same |fit::callback| can be obtained via share(), as shown
// in the example above. This method converts the |fit::callback| into a
// reference-counted version of the |fit::callback| and returns a copy of the
// reference as another |fit::callback| with the same target function.
//
// What is notable about |fit::callback<T>.share()| is that invoking any shared
// copy will "nullify" all shared copies, as shown in the example.
//
// Note that |fit::callback| is NOT thread-safe by default. If multi-threaded
// support is required, you would need to implement your own mutex, or similar
// guard, before checking and calling a |fit::callback|.
//
// Targets of up to |inline_target_size| bytes in size are stored inline within
// the callback object without incurring any heap allocation. Larger callable
// objects will be moved to the heap as required. |inline_target_size| is
// rounded up to a multiple of sizeof(void*).
//
// See also |fit::inline_callback<T, size>| for more control over allocation
// behavior.
//
// SYNOPSIS
//
// |T| is the callback's signature.  e.g. void(int, std::string).
//
// |inline_target_size| is the minimum size of target that is guaranteed to
// fit within a callback without requiring heap allocation.
// Defaults to |default_inline_target_size|.
//
// Class members are documented in |fit::callback_impl|, below.
//
template <typename T, size_t inline_target_size = default_inline_target_size>
using callback =
    callback_impl<internal::RoundUpToWord(inline_target_size), /*require_inline=*/false, T>;

// A move-only, run-once, callable object wrapper that forces callables to be
// stored inline and never performs heap allocation.
//
// Behaves just like |fit::callback<T, inline_target_size>| except that
// attempting to store a target larger than |inline_target_size| will fail to
// compile.
template <typename T, size_t inline_target_size = default_inline_target_size>
using inline_callback = callback_impl<internal::RoundUpToWord(inline_target_size),
                                      /*require_inline=*/true, T>;

template <size_t inline_target_size, bool require_inline, typename Result, typename... Args>
class function_impl<inline_target_size, require_inline, Result(Args...)> final
    : private ::fit::internal::function_base<inline_target_size, require_inline, Result(Args...)> {
  using base = ::fit::internal::function_base<inline_target_size, require_inline, Result(Args...)>;

  // function_base requires private access during share()
  friend class ::fit::internal::function_base<inline_target_size, require_inline, Result(Args...)>;

  // supports target() for shared functions
  friend const void* ::fit::internal::get_target_type_id<>(
      const function_impl<inline_target_size, require_inline, Result(Args...)>&);

  template <typename U>
  using not_self_type = ::fit::internal::not_same_type<function_impl, U>;

  template <typename... Conditions>
  using requires_conditions = ::fit::internal::requires_conditions<Conditions...>;

  template <typename... Conditions>
  using assignment_requires_conditions =
      ::fit::internal::assignment_requires_conditions<function_impl&, Conditions...>;

 public:
  // The function's result type.
  using typename base::result_type;

  // Initializes an empty (null) function. Attempting to call an empty
  // function will abort the program.
  constexpr function_impl() = default;

  // Creates a function with an empty target (same outcome as the default
  // constructor).
  constexpr function_impl(decltype(nullptr)) : base(nullptr) {}

  // Creates a function bound to the specified function pointer.
  // If target == nullptr, assigns an empty target.
  function_impl(Result (*function_target)(Args...)) : base(function_target) {}

  // Creates a function bound to the specified callable object.
  // If target == nullptr, assigns an empty target.
  //
  // For functors, we need to capture the raw type but also restrict on the
  // existence of an appropriate operator () to resolve overloads and implicit
  // casts properly.
  //
  // Note that specializations of this template method that take fit::callback
  // objects as the target Callable are deleted (see below).
  template <typename Callable,
            requires_conditions<
                std::is_convertible<decltype(std::declval<Callable&>()(std::declval<Args>()...)),
                                    result_type>,
                not_self_type<Callable>> = true>
  function_impl(Callable&& function_target) : base(std::forward<Callable>(function_target)) {}

  // Deletes the specializations of function_impl(Callable) that would allow
  // a |fit::function| to be constructed from a |fit::callback|. This prevents
  // unexpected behavior of a |fit::function| that would otherwise fail after
  // one call. To explicitly allow this, simply wrap the |fit::callback| in a
  // pass-through lambda before passing it to the |fit::function|.
  template <size_t other_inline_target_size, bool other_require_inline>
  function_impl(
      ::fit::callback_impl<other_inline_target_size, other_require_inline, Result(Args...)>) =
      delete;

  // Creates a function with a target moved from another function,
  // leaving the other function with an empty target.
  function_impl(function_impl&& other) noexcept : base(static_cast<base&&>(other)) {}

  // Destroys the function, releasing its target.
  ~function_impl() = default;

  // Assigns the function to an empty target. Attempting to invoke the
  // function will abort the program.
  function_impl& operator=(decltype(nullptr)) {
    base::assign_null();
    return *this;
  }

  // Assigns the function to the specified callable object. If target ==
  // nullptr, assigns an empty target.
  //
  // For functors, we need to capture the raw type but also restrict on the
  // existence of an appropriate operator () to resolve overloads and implicit
  // casts properly.
  //
  // Note that specializations of this template method that take fit::callback
  // objects as the target Callable are deleted (see below).
  template <typename Callable>
  // NOLINTNEXTLINE(misc-unconventional-assign-operator)
  assignment_requires_conditions<
      std::is_convertible<decltype(std::declval<Callable&>()(std::declval<Args>()...)),
                          result_type>,
      not_self_type<Callable>>
  operator=(Callable&& function_target) {
    base::assign_callable(std::forward<Callable>(function_target));
    return *this;
  }

  // Deletes the specializations of operator=(Callable) that would allow
  // a |fit::function| to be assigned from a |fit::callback|. This
  // prevents unexpected behavior of a |fit::function| that would otherwise
  // fail after one call. To explicitly allow this, simply wrap the
  // |fit::callback| in a pass-through lambda before assigning it to the
  // |fit::function|.
  template <size_t other_inline_target_size, bool other_require_inline>
  function_impl& operator=(
      ::fit::callback_impl<other_inline_target_size, other_require_inline, Result(Args...)>) =
      delete;

  // Move assignment
  function_impl& operator=(function_impl&& other) noexcept {
    if (&other == this)
      return *this;
    base::assign_function(static_cast<base&&>(other));
    return *this;
  }

  // Swaps the functions' targets.
  void swap(function_impl& other) { base::swap(other); }

  // Returns a pointer to the function's target.
  using base::target;

  // Returns true if the function has a non-empty target.
  using base::operator bool;

  // Invokes the function's target.
  // Aborts if the function's target is empty.
  Result operator()(Args... args) const { return base::invoke(std::forward<Args>(args)...); }

  // Returns a new function object that invokes the same target.
  // The target itself is not copied; it is moved to the heap and its
  // lifetime is extended until all references have been released.
  //
  // Note: This method is not supported on |fit::inline_function<>|
  //       because it may incur a heap allocation which is contrary to
  //       the stated purpose of |fit::inline_function<>|.
  function_impl share() {
    function_impl copy;
    base::template share_with<function_impl>(copy);
    return copy;
  }
};

template <size_t inline_target_size, bool require_inline, typename FunctionType>
void swap(function_impl<inline_target_size, require_inline, FunctionType>& a,
          function_impl<inline_target_size, require_inline, FunctionType>& b) {
  a.swap(b);
}

template <size_t inline_target_size, bool require_inline, typename FunctionType>
bool operator==(const function_impl<inline_target_size, require_inline, FunctionType>& f,
                decltype(nullptr)) {
  return !f;
}
template <size_t inline_target_size, bool require_inline, typename FunctionType>
bool operator==(decltype(nullptr),
                const function_impl<inline_target_size, require_inline, FunctionType>& f) {
  return !f;
}
template <size_t inline_target_size, bool require_inline, typename FunctionType>
bool operator!=(const function_impl<inline_target_size, require_inline, FunctionType>& f,
                decltype(nullptr)) {
  return !!f;
}
template <size_t inline_target_size, bool require_inline, typename FunctionType>
bool operator!=(decltype(nullptr),
                const function_impl<inline_target_size, require_inline, FunctionType>& f) {
  return !!f;
}

template <size_t inline_target_size, bool require_inline, typename Result, typename... Args>
class callback_impl<inline_target_size, require_inline, Result(Args...)> final
    : private ::fit::internal::function_base<inline_target_size, require_inline, Result(Args...)> {
  using base = ::fit::internal::function_base<inline_target_size, require_inline, Result(Args...)>;

  // function_base requires private access during share()
  friend class ::fit::internal::function_base<inline_target_size, require_inline, Result(Args...)>;

  // supports target() for shared functions
  friend const void* ::fit::internal::get_target_type_id<>(
      const callback_impl<inline_target_size, require_inline, Result(Args...)>&);

  template <typename U>
  using not_self_type = ::fit::internal::not_same_type<callback_impl, U>;

  template <typename... Conditions>
  using requires_conditions = ::fit::internal::requires_conditions<Conditions...>;

  template <typename... Conditions>
  using assignment_requires_conditions =
      ::fit::internal::assignment_requires_conditions<callback_impl&, Conditions...>;

 public:
  // The callback function's result type.
  using typename base::result_type;

  // Initializes an empty (null) callback. Attempting to call an empty
  // callback will abort the program.
  constexpr callback_impl() = default;

  // Creates a callback with an empty target (same outcome as the default
  // constructor).
  constexpr callback_impl(decltype(nullptr)) : base(nullptr) {}

  // Creates a callback bound to the specified function pointer.
  // If target == nullptr, assigns an empty target.
  callback_impl(Result (*callback_target)(Args...)) : base(callback_target) {}

  // Creates a callback bound to the specified callable object.
  // If target == nullptr, assigns an empty target.
  //
  // For functors, we need to capture the raw type but also restrict on the
  // existence of an appropriate operator () to resolve overloads and implicit
  // casts properly.
  template <typename Callable,
            requires_conditions<
                std::is_convertible<decltype(std::declval<Callable&>()(std::declval<Args>()...)),
                                    result_type>,
                not_self_type<Callable>> = true>
  callback_impl(Callable&& callback_target) : base(std::forward<Callable>(callback_target)) {}

  // Creates a callback with a target moved from another callback,
  // leaving the other callback with an empty target.
  callback_impl(callback_impl&& other) noexcept : base(static_cast<base&&>(other)) {}

  // Destroys the callback, releasing its target.
  ~callback_impl() = default;

  // Assigns the callback to an empty target. Attempting to invoke the
  // callback will abort the program.
  callback_impl& operator=(decltype(nullptr)) {
    base::assign_null();
    return *this;
  }

  // Assigns the callback to the specified callable object. If target ==
  // nullptr, assigns an empty target.
  //
  // For functors, we need to capture the raw type but also restrict on the
  // existence of an appropriate operator () to resolve overloads and implicit
  // casts properly.
  template <typename Callable>
  // NOLINTNEXTLINE(misc-unconventional-assign-operator)
  assignment_requires_conditions<
      std::is_convertible<decltype(std::declval<Callable&>()(std::declval<Args>()...)),
                          result_type>,
      not_self_type<Callable>>
  operator=(Callable&& callback_target) {
    base::assign_callable(std::forward<Callable>(callback_target));
    return *this;
  }

  // Move assignment
  callback_impl& operator=(callback_impl&& other) noexcept {
    if (&other == this)
      return *this;
    base::assign_function(static_cast<base&&>(other));
    return *this;
  }

  // Swaps the callbacks' targets.
  void swap(callback_impl& other) { base::swap(other); }

  // Returns a pointer to the callback's target.
  using base::target;

  // Returns true if the callback has a non-empty target.
  using base::operator bool;

  // Invokes the callback's target.
  // Aborts if the callback's target is empty.
  // |fit::callback| must be non-const to invoke. Before the target function
  // is actually called, the fit::callback will be set to the default empty
  // state (== nullptr, and operator bool() will subsequently return |false|).
  // The target function will then be released after the function is called.
  // If the callback was shared, any remaining copies will also be cleared.
  Result operator()(Args... args) {
    auto temp = std::move(*this);
    return temp.invoke(std::forward<Args>(args)...);
  }

  // Returns a new callback object that invokes the same target.
  // The target itself is not copied; it is moved to the heap and its
  // lifetime is extended until all references have been released.
  // For |fit::callback| (unlike fit::function), the first invocation of the
  // callback will release all references to the target. All callbacks
  // derived from the same original callback (via share()) will be cleared,
  // as if set to |nullptr|, and "operator bool()" will return false.
  //
  // Note: This method is not supported on |fit::inline_function<>|
  //       because it may incur a heap allocation which is contrary to
  //       the stated purpose of |fit::inline_function<>|.
  callback_impl share() {
    callback_impl copy;
    base::template share_with<callback_impl>(copy);
    return copy;
  }
};

template <size_t inline_target_size, bool require_inline, typename FunctionType>
void swap(callback_impl<inline_target_size, require_inline, FunctionType>& a,
          callback_impl<inline_target_size, require_inline, FunctionType>& b) {
  a.swap(b);
}

template <size_t inline_target_size, bool require_inline, typename FunctionType>
bool operator==(const callback_impl<inline_target_size, require_inline, FunctionType>& f,
                decltype(nullptr)) {
  return !f;
}
template <size_t inline_target_size, bool require_inline, typename FunctionType>
bool operator==(decltype(nullptr),
                const callback_impl<inline_target_size, require_inline, FunctionType>& f) {
  return !f;
}
template <size_t inline_target_size, bool require_inline, typename FunctionType>
bool operator!=(const callback_impl<inline_target_size, require_inline, FunctionType>& f,
                decltype(nullptr)) {
  return !!f;
}
template <size_t inline_target_size, bool require_inline, typename FunctionType>
bool operator!=(decltype(nullptr),
                const callback_impl<inline_target_size, require_inline, FunctionType>& f) {
  return !!f;
}

// Returns a Callable object that invokes a member function of an object.
// When used in a fit::function, this heap allocates (the returned lambda is
// 3*sizeof(void*)).
//
// Deprecated in favor of the bind_member definition below that will inline into a
// fit::function without heap allocating. The new bind_member definition is only
// supported on C++17 and up. On C++14, a plain lambda should be used instead.
template <typename R, typename T, typename... Args>
auto bind_member(T* instance, R (T::*fn)(Args...)) {
  return [instance, fn](Args... args) { return (instance->*fn)(std::forward<Args>(args)...); };
}

// C++17 due to use of 'auto' template parameters and lambda parameters.
#if __cplusplus >= 201703L
namespace internal {
// Performs the call for bind_member but captures the arguments of the method.
// This ensure that the correct overload of |method| is called.
template <auto method, typename T, typename... Args>
auto make_the_call(T* instance, parameter_pack<Args...>) {
  return [instance](Args... args) {
    return (instance->*method)(std::forward<decltype(args)>(args)...);
  };
}
}  // namespace internal

// Returns a Callable object that invokes a member function of an object.
// In other words, returns a closure 'f' for which calling f(args) is equivalent to
// calling obj.method(args).
//
// Usage: fit::bind_member<&ObjType::MethodName>(&obj)
template <auto method, typename T>
auto bind_member(T* instance) {
  return internal::make_the_call<method>(instance,
                                         typename callable_traits<decltype(method)>::args{});
}
#endif  //  __cplusplus >= 201703L

}  // namespace fit

#endif  // LIB_FIT_INCLUDE_LIB_FIT_FUNCTION_H_
