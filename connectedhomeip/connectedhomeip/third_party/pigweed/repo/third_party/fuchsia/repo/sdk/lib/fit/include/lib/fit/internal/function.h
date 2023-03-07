// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_FUNCTION_H_
#define LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_FUNCTION_H_

#include <lib/stdcompat/bit.h>
#include <stddef.h>
#include <stdlib.h>

#include <algorithm>
#include <cstring>
#include <functional>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include "../nullable.h"
#include "pw_assert/assert.h"
#include "pw_preprocessor/compiler.h"

namespace fit {
namespace internal {

// Rounds the first argument up to a non-zero multiple of the second argument.
constexpr size_t RoundUpToMultiple(size_t value, size_t multiple) {
  return value == 0 ? multiple : (value + multiple - 1) / multiple * multiple;
}

// Rounds up to the nearest word. To avoid unnecessary instantiations, function_base can only be
// instantiated with an inline size that is a non-zero multiple of the word size.
constexpr size_t RoundUpToWord(size_t value) { return RoundUpToMultiple(value, sizeof(void*)); }

// target_ops is the vtable for the function_base class. The base_target_ops struct holds functions
// that are common to all function_base instantiations, regardless of the function's signature.
// The derived target_ops template that adds the signature-specific invoke method.
//
// Splitting the common functions into base_target_ops allows all function_base instantiations to
// share the same vtable for their null function instantiation, reducing code size.
struct base_target_ops {
  const void* (*target_type_id)(void* bits, const void* impl_ops);
  void* (*get)(void* bits);
  void (*move)(void* from_bits, void* to_bits);
  void (*destroy)(void* bits);

 protected:
  // Aggregate initialization isn't supported with inheritance until C++17, so define a constructor.
  constexpr base_target_ops(decltype(target_type_id) target_type_id_func, decltype(get) get_func,
                            decltype(move) move_func, decltype(destroy) destroy_func)
      : target_type_id(target_type_id_func),
        get(get_func),
        move(move_func),
        destroy(destroy_func) {}
};

template <typename Result, typename... Args>
struct target_ops final : public base_target_ops {
  Result (*invoke)(void* bits, Args... args);

  constexpr target_ops(decltype(target_type_id) target_type_id_func, decltype(get) get_func,
                       decltype(move) move_func, decltype(destroy) destroy_func,
                       decltype(invoke) invoke_func)
      : base_target_ops(target_type_id_func, get_func, move_func, destroy_func),
        invoke(invoke_func) {}
};

static_assert(sizeof(target_ops<void>) == sizeof(void (*)()) * 5, "Unexpected target_ops padding");

template <typename Callable, bool is_inline, bool is_shared, typename Result, typename... Args>
struct target;

inline void trivial_target_destroy(void* /*bits*/) {}

inline const void* unshared_target_type_id(void* /*bits*/, const void* impl_ops) {
  return impl_ops;
}

// vtable for nullptr (empty target function)

// All function_base instantiations, regardless of callable type, use the same
// vtable for nullptr functions. This avoids generating unnecessary identical
// vtables, which reduces code size.
//
// The null_target class does not need to be a template. However, if it was not
// a template, the ops variable would need to be defined in a .cc file for C++14
// compatibility. In C++17, null_target::ops could be defined in the class or
// elsewhere in the header as an inline variable.
template <typename Unused = void>
struct null_target {
  static void invoke(void* /*bits*/) { PW_ASSERT(false); }

  static const target_ops<void> ops;

  static_assert(std::is_same<Unused, void>::value, "Only instantiate null_target with void");
};

template <typename Result, typename... Args>
struct target<decltype(nullptr), /*is_inline=*/true, /*is_shared=*/false, Result, Args...> final
    : public null_target<> {};

inline void* null_target_get(void* /*bits*/) { return nullptr; }
inline void null_target_move(void* /*from_bits*/, void* /*to_bits*/) {}

template <typename Unused>
constexpr target_ops<void> null_target<Unused>::ops = {&unshared_target_type_id, &null_target_get,
                                                       &null_target_move, &trivial_target_destroy,
                                                       &null_target::invoke};

// vtable for inline target function

// Trivially movable and destructible types can be moved with a simple memcpy. Use the same function
// for all callable types of a particular size to reduce code size.
template <size_t size_bytes>
inline void inline_trivial_target_move(void* from_bits, void* to_bits) {
  std::memcpy(to_bits, from_bits, size_bytes);
}

template <typename Callable, typename Result, typename... Args>
struct target<Callable,
              /*is_inline=*/true, /*is_shared=*/false, Result, Args...>
    final {
  template <typename Callable_>
  static void initialize(void* bits, Callable_&& target) {
    new (bits) Callable(std::forward<Callable_>(target));
  }
  static Result invoke(void* bits, Args... args) {
    auto& target = *static_cast<Callable*>(bits);
    return target(std::forward<Args>(args)...);
  }
  // Selects which move function to use. Trivially movable and destructible types of a particular
  // size share a single move function.
  static constexpr auto get_move_function() {
    if (std::is_trivially_move_constructible<Callable>::value &&
        std::is_trivially_destructible<Callable>::value) {
      return &inline_trivial_target_move<sizeof(Callable)>;
    }
    return &move;
  }
  // Selects which destroy function to use. Trivially destructible types share a single, empty
  // destroy function.
  static constexpr auto get_destroy_function() {
    return std::is_trivially_destructible<Callable>::value ? &trivial_target_destroy : &destroy;
  }

  static const target_ops<Result, Args...> ops;

 private:
  static void move(void* from_bits, void* to_bits) {
    auto& from_target = *static_cast<Callable*>(from_bits);
    new (to_bits) Callable(std::move(from_target));
    from_target.~Callable();  // NOLINT(bugprone-use-after-move)
  }
  static void destroy(void* bits) {
    auto& target = *static_cast<Callable*>(bits);
    target.~Callable();
  }
};

inline void* inline_target_get(void* bits) { return bits; }

template <typename Callable, typename Result, typename... Args>
constexpr target_ops<Result, Args...> target<Callable,
                                             /*is_inline=*/true,
                                             /*is_shared=*/false, Result, Args...>::ops = {
    &unshared_target_type_id, &inline_target_get, target::get_move_function(),
    target::get_destroy_function(), &target::invoke};

// vtable for pointer to target function

template <typename Callable, typename Result, typename... Args>
struct target<Callable,
              /*is_inline=*/false, /*is_shared=*/false, Result, Args...>
    final {
  template <typename Callable_>
  static void initialize(void* bits, Callable_&& target) {
    auto ptr = static_cast<Callable**>(bits);
    *ptr = new Callable(std::forward<Callable_>(target));
  }
  static Result invoke(void* bits, Args... args) {
    auto& target = **static_cast<Callable**>(bits);
    return target(std::forward<Args>(args)...);
  }
  static void move(void* from_bits, void* to_bits) {
    auto from_ptr = static_cast<Callable**>(from_bits);
    auto to_ptr = static_cast<Callable**>(to_bits);
    *to_ptr = *from_ptr;
  }
  static void destroy(void* bits) {
    auto ptr = static_cast<Callable**>(bits);
    delete *ptr;
  }

  static const target_ops<Result, Args...> ops;
};

inline void* heap_target_get(void* bits) { return *static_cast<void**>(bits); }

template <typename Callable, typename Result, typename... Args>
constexpr target_ops<Result, Args...> target<Callable,
                                             /*is_inline=*/false,
                                             /*is_shared=*/false, Result, Args...>::ops = {
    &unshared_target_type_id, &heap_target_get, &target::move, &target::destroy, &target::invoke};

// vtable for fit::function std::shared_ptr to target function

template <typename SharedFunction>
const void* get_target_type_id(const SharedFunction& function_or_callback) {
  return function_or_callback.target_type_id();
}

// For this vtable,
// Callable by definition will be either a fit::function or fit::callback
template <typename SharedFunction, typename Result, typename... Args>
struct target<SharedFunction,
              /*is_inline=*/false, /*is_shared=*/true, Result, Args...>
    final {
  static void initialize(void* bits, SharedFunction target) {
    new (bits) std::shared_ptr<SharedFunction>(
        std::move(std::make_shared<SharedFunction>(std::move(target))));
  }
  static void copy_shared_ptr(void* from_bits, void* to_bits) {
    auto& from_shared_ptr = *static_cast<std::shared_ptr<SharedFunction>*>(from_bits);
    new (to_bits) std::shared_ptr<SharedFunction>(from_shared_ptr);
  }
  static const void* target_type_id(void* bits, const void* /*impl_ops*/) {
    auto& function_or_callback = **static_cast<std::shared_ptr<SharedFunction>*>(bits);
    return ::fit::internal::get_target_type_id(function_or_callback);
  }
  static void* get(void* bits) {
    auto& function_or_callback = **static_cast<std::shared_ptr<SharedFunction>*>(bits);
    return function_or_callback.template target<SharedFunction>(
        /*check=*/false);  // void* will fail the check
  }
  static Result invoke(void* bits, Args... args) {
    auto& function_or_callback = **static_cast<std::shared_ptr<SharedFunction>*>(bits);
    return function_or_callback(std::forward<Args>(args)...);
  }
  static void move(void* from_bits, void* to_bits) {
    auto from_shared_ptr = std::move(*static_cast<std::shared_ptr<SharedFunction>*>(from_bits));
    new (to_bits) std::shared_ptr<SharedFunction>(std::move(from_shared_ptr));
  }
  static void destroy(void* bits) { static_cast<std::shared_ptr<SharedFunction>*>(bits)->reset(); }

  static const target_ops<Result, Args...> ops;
};

template <typename SharedFunction, typename Result, typename... Args>
constexpr target_ops<Result, Args...> target<SharedFunction,
                                             /*is_inline=*/false,
                                             /*is_shared=*/true, Result, Args...>::ops = {
    &target::target_type_id, &target::get, &target::move, &target::destroy, &target::invoke};

// Calculates the alignment to use for a function of the provided
// inline_target_size. Some platforms use a large alignment for max_align_t, so
// use the minimum of max_align_t and the largest alignment for the inline
// target size.
//
// Alignments must be powers of 2, and alignof(T) <= sizeof(T), so find the
// largest power of 2 <= inline_target_size.
constexpr size_t FunctionAlignment(size_t inline_target_size) {
  return std::min(cpp20::bit_floor(inline_target_size), alignof(max_align_t));
}

// Function implementation details shared by all functions, regardless of
// signature. This class is aligned based on inline_target_size and max_align_t
// so that the target storage (bits_, the first class member) has correct
// alignment.
//
// See |fit::function| and |fit::callback| documentation for more information.
template <size_t inline_target_size>
class alignas(FunctionAlignment(inline_target_size)) generic_function_base {
 public:
  // The inline target size must be a non-zero multiple of sizeof(void*).  Uses
  // of |fit::function_impl| and |fit::callback_impl| may call
  // fit::internal::RoundUpToWord to round to a valid inline size.
  //
  // A multiple of sizeof(void*) is required because it:
  //
  // - Avoids unnecessary duplicate instantiations of the function classes when
  //   working with different inline sizes. This reduces code size.
  // - Prevents creating unnecessarily restrictive functions. Without rounding, a
  //   function with a non-word size would be padded to at least the next word,
  //   but that space would be unusable.
  // - Ensures that the true inline size matches the template parameter, which
  //   could cause confusion in error messages.
  //
  static_assert(inline_target_size >= sizeof(void*),
                "The inline target size must be at least one word");
  static_assert(inline_target_size % sizeof(void*) == 0,
                "The inline target size must be a multiple of the word size");

  // Deleted copy constructor and assign. |generic_function_base|
  // implementations are move-only.
  generic_function_base(const generic_function_base& other) = delete;
  generic_function_base& operator=(const generic_function_base& other) = delete;

  // Move assignment must be provided by subclasses.
  generic_function_base& operator=(generic_function_base&& other) = delete;

 protected:
  constexpr generic_function_base() : null_bits_(), ops_(&null_target<>::ops) {}

  generic_function_base(generic_function_base&& other) noexcept { move_target_from(other); }

  ~generic_function_base() { destroy_target(); }

  // Returns true if the function has a non-empty target.
  explicit operator bool() const { return ops_->get(bits_) != nullptr; }

  // Used by derived "impl" classes to implement operator=().
  // Assigns an empty target.
  void assign_null() {
    destroy_target();
    initialize_null_target();
  }

  // Used by derived "impl" classes to implement operator=().
  // Assigns the function with a target moved from another function,
  // leaving the other function with an empty target.
  void assign_function(generic_function_base&& other) {
    destroy_target();
    move_target_from(other);
  }

  void swap(generic_function_base& other) {
    if (&other == this)
      return;

    const base_target_ops* temp_ops = ops_;
    // temp_bits, which stores the target, must maintain the expected alignment.
    alignas(generic_function_base) uint8_t temp_bits[inline_target_size];
    ops_->move(bits_, temp_bits);

    ops_ = other.ops_;
    other.ops_->move(other.bits_, bits_);

    other.ops_ = temp_ops;
    temp_ops->move(temp_bits, other.bits_);
  }

  // returns an opaque ID unique to the |Callable| type of the target.
  // Used by check_target_type.
  const void* target_type_id() const { return ops_->target_type_id(bits_, ops_); }

  // leaves target uninitialized
  void destroy_target() { ops_->destroy(bits_); }

  // assumes target is uninitialized
  void initialize_null_target() { ops_ = &null_target<>::ops; }

  // Gets a pointer to the function context.
  void* get() const { return ops_->get(bits_); }

  // Allow function_base to directly access bits_ and ops_ when needed.
  void* bits() const { return bits_; }
  const base_target_ops* ops() const { return ops_; }
  void set_ops(const base_target_ops* new_ops) { ops_ = new_ops; }

 private:
  // Implements the move operation, used by move construction and move
  // assignment. Leaves other target initialized to null.
  void move_target_from(generic_function_base& other) {
    ops_ = other.ops_;
    other.ops_->move(other.bits_, bits_);
    other.initialize_null_target();
  }

  struct empty {};

  union {
    // Function context data. The bits_ field requires special alignment, but
    // adding the alignas() at the field declaration increases the padding.
    // Instead, generic_function_base is aligned according to max_align_t and
    // inline_target_size, and bits_ is placed first in the class. Thus, bits_
    // MUST remain first in the class to ensure proper alignment.
    mutable uint8_t bits_[inline_target_size];

    // Empty struct used when initializing the storage in the constexpr
    // constructor.
    empty null_bits_;
  };

  // The target_ops pointer for this function. This field has lower alignment
  // requirement than bits, so placing ops after bits allows for better
  // packing reducing the padding needed in some cases.
  const base_target_ops* ops_;
};

template <size_t inline_target_size, bool require_inline, typename FunctionType>
class function_base;

// Function implementation details that require the function signature.
// See |fit::function| and |fit::callback| documentation for more information.
template <size_t inline_target_size, bool require_inline, typename Result, typename... Args>
class function_base<inline_target_size, require_inline, Result(Args...)>
    : public generic_function_base<inline_target_size> {
  using base = generic_function_base<inline_target_size>;

  // Check alignment and size of the base, which holds the bits_ and ops_ members.
  static_assert(alignof(base) == FunctionAlignment(inline_target_size),
                "Must be aligned as min(alignof(max_align_t), inline_target_size)");
  static_assert(sizeof(base) == RoundUpToMultiple(inline_target_size + sizeof(base_target_ops*),
                                                  FunctionAlignment(inline_target_size)),
                "generic_function_base has unexpected padding and is not minimal in size");

  template <typename Callable>
  using target_type = target<Callable, (sizeof(Callable) <= inline_target_size),
                             /*is_shared=*/false, Result, Args...>;
  template <typename SharedFunction>
  using shared_target_type = target<SharedFunction,
                                    /*is_inline=*/false,
                                    /*is_shared=*/true, Result, Args...>;

  using ops_type = const target_ops<Result, Args...>*;

 protected:
  using result_type = Result;

  constexpr function_base() = default;

  constexpr function_base(decltype(nullptr)) : function_base() {}

  function_base(Result (*function_target)(Args...)) { initialize_target(function_target); }

  template <typename Callable,
            typename = std::enable_if_t<std::is_convertible<
                decltype(std::declval<Callable&>()(std::declval<Args>()...)), result_type>::value>>
  function_base(Callable&& target) {
    initialize_target(std::forward<Callable>(target));
  }

  function_base(function_base&&) noexcept = default;

  // Returns a pointer to the function's target.
  // If |check| is true (the default), the function _may_ abort if the
  // caller tries to assign the target to a varible of the wrong type. (This
  // check is currently skipped for share()d objects.)
  // Note the shared pointer vtable must set |check| to false to assign the
  // target to |void*|.
  template <typename Callable>
  Callable* target(bool check = true) {
    if (check)
      check_target_type<Callable>();
    return static_cast<Callable*>(base::get());
  }

  // Returns a pointer to the function's target (const version).
  // If |check| is true (the default), the function _may_ abort if the
  // caller tries to assign the target to a varible of the wrong type. (This
  // check is currently skipped for share()d objects.)
  // Note the shared pointer vtable must set |check| to false to assign the
  // target to |void*|.
  template <typename Callable>
  const Callable* target(bool check = true) const {
    if (check)
      check_target_type<Callable>();
    return static_cast<Callable*>(base::get());
  }

  // Used by the derived "impl" classes to implement share().
  //
  // The caller creates a new object of the same type as itself, and passes in
  // the empty object. This function first checks if |this| is already shared,
  // and if not, creates a new version of itself containing a |std::shared_ptr|
  // to its original self, and updates |ops_| to the vtable for the shared
  // version.
  //
  // Then it copies its |shared_ptr| to the |bits_| of the given |copy|, and
  // assigns the same shared pointer vtable to the copy's |ops_|.
  //
  // The target itself is not copied; it is moved to the heap and its lifetime
  // is extended until all references have been released.
  //
  // Note: This method is not supported on |fit::inline_function<>|
  //       because it may incur a heap allocation which is contrary to
  //       the stated purpose of |fit::inline_function<>|.
  template <typename SharedFunction>
  void share_with(SharedFunction& copy) {
    static_assert(!require_inline, "Inline functions cannot be shared.");
    if (base::get() != nullptr) {
      // Convert to a shared function if it isn't already.
      if (base::ops() != &shared_target_type<SharedFunction>::ops) {
        shared_target_type<SharedFunction>::initialize(
            base::bits(), std::move(*static_cast<SharedFunction*>(this)));
        base::set_ops(&shared_target_type<SharedFunction>::ops);
      }
      copy_shared_target_to(copy);
    }
  }

  // Used by derived "impl" classes to implement operator()().
  // Invokes the function's target.
  // Note that fit::callback will release the target immediately after
  // invoke() (also affecting any share()d copies).
  // Aborts if the function's target is empty.
  // TODO(b/241567321): Remove "no sanitize" after pw_protobuf is fixed.
  Result invoke(Args... args) const PW_NO_SANITIZE("function") {
    // Down cast the ops to the derived type that this function was instantiated
    // with, which includes the invoke function.
    //
    // NOTE: This abuses the calling convention when invoking a null function
    // that takes arguments! Null functions share a single vtable with a void()
    // invoke function. This is permitted only because invoking a null function
    // is an error that immediately aborts execution. Also, the null invoke
    // function never attempts to access any passed arguments.
    return static_cast<ops_type>(base::ops())->invoke(base::bits(), std::forward<Args>(args)...);
  }

  // Used by derived "impl" classes to implement operator=().
  // Assigns the function's target.
  // If target == nullptr, assigns an empty target.
  template <typename Callable,
            typename = std::enable_if_t<std::is_convertible<
                decltype(std::declval<Callable&>()(std::declval<Args>()...)), result_type>::value>>
  void assign_callable(Callable&& target) {
    base::destroy_target();
    initialize_target(std::forward<Callable>(target));
  }

 private:
  // fit::function and fit::callback are not directly copyable, but share()
  // will create shared references to the original object. This method
  // implements the copy operation for the |std::shared_ptr| wrapper.
  template <typename SharedFunction>
  void copy_shared_target_to(SharedFunction& copy) {
    copy.destroy_target();
    PW_ASSERT(base::ops() == &shared_target_type<SharedFunction>::ops);
    shared_target_type<SharedFunction>::copy_shared_ptr(base::bits(), copy.bits());
    copy.set_ops(base::ops());
  }

  // target may or may not be initialized.
  template <typename Callable>
  void initialize_target(Callable&& target) {
    // Convert function or function references to function pointer.
    using DecayedCallable = std::decay_t<Callable>;
    static_assert(!require_inline || alignof(DecayedCallable) <= alignof(base),
                  "Alignment of Callable must be <= alignment of the function class.");
    static_assert(!require_inline || sizeof(DecayedCallable) <= inline_target_size,
                  "Callable too large to store inline as requested.");
    if (is_null(target)) {
      base::initialize_null_target();
    } else {
      base::set_ops(&target_type<DecayedCallable>::ops);
      target_type<DecayedCallable>::initialize(base::bits(), std::forward<Callable>(target));
    }
  }

  // Called by target() if |check| is true.
  // Checks the template parameter, usually inferred from the context of
  // the call to target(), and aborts the program if it can determine that
  // the Callable type is not compatible with the function's Result and Args.
  template <typename Callable>
  void check_target_type() const {
    if (target_type<Callable>::ops.target_type_id(nullptr, &target_type<Callable>::ops) !=
        base::target_type_id()) {
      PW_ASSERT(false);
    }
  }
};

}  // namespace internal
}  // namespace fit

#endif  // LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_FUNCTION_H_
