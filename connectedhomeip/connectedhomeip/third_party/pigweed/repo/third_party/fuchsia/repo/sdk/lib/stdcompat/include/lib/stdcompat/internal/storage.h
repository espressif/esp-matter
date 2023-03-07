// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_STORAGE_H_
#define LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_STORAGE_H_

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "utility.h"

namespace cpp17 {
namespace internal {

// Type tag to select overloads based on type T.
template <typename T>
struct type_tag {
  using type = T;
};

// Type tag to select overloads based on index Index.
template <std::size_t Index>
struct index_tag {
  static constexpr std::size_t index = Index;
};

// Type tag to select trivial initialization.
enum trivial_init_t { trivial_init_v };

// Type tag to select default initialization.
enum default_init_t { default_init_v };

// Type tag to select conditional initialization.
enum maybe_init_t { maybe_init_v };

// Represents the pair (T, Index) in the type system.
template <typename T, std::size_t Index>
struct type_index {};

// Represents whether a type is trivially/non-trivially destructible.
enum class destructor_class {
  trivial,
  non_trivial,
};

// Represents whether a type is trivially/non-trivially copyable.
enum class copy_class {
  trivial,
  non_trivial,
};

// Represents whether a type is trivially/non-trivially movable.
enum class move_class {
  trivial,
  non_trivial,
};

// Represents the full complement of move/copy/destruct classes for a type.
template <destructor_class DestructorClass, copy_class CopyClass, move_class MoveClass>
struct storage_class {};

template <typename... Ts>
using make_storage_class =
    storage_class<is_trivially_destructible_v<Ts...> ? destructor_class::trivial
                                                     : destructor_class::non_trivial,
                  is_trivially_copyable_v<Ts...> ? copy_class::trivial : copy_class::non_trivial,
                  is_trivially_movable_v<Ts...> ? move_class::trivial : move_class::non_trivial>;

// A trivial type for the empty alternative of union-based storage.
struct empty_type {};

// Index type used to track the active variant. Tracking uses zero-based
// indices. Empty is denoted by the maximum representable value.
using index_type = std::size_t;

// Index denoting that no user-specified variant is active. Take care not to
// ODR-use this value.
constexpr index_type empty_index = std::numeric_limits<index_type>::max();

#ifdef NDEBUG
#define LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT __builtin_unreachable
#else
#define LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT __builtin_abort
#endif

// Base type for lazy-initialized union storage types. This type implements a
// recursive union of the element types in Ts. Specializations handle the
// recursive and terminal cases, and the different storage requirements for
// trivially/non-trivially destructible types.
template <destructor_class, typename...>
union storage_base;

// Non-trivial terminal case.
template <>
union storage_base<destructor_class::non_trivial, type_index<empty_type, empty_index>> {
  storage_base() : empty{} {}

  template <typename... Args>
  storage_base(type_tag<empty_type>, Args&&...) : empty{} {}
  template <typename... Args>
  storage_base(index_tag<empty_index>, Args&&...) : empty{} {}

  // Non-trivial destructor.
  ~storage_base() {}

  storage_base(const storage_base&) = default;
  storage_base(storage_base&&) = default;
  storage_base& operator=(const storage_base&) = default;
  storage_base& operator=(storage_base&&) = default;

  void construct_at(std::size_t index, const storage_base&) {
    if (index == empty_index) {
      new (&empty) empty_type{};
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }
  void construct_at(std::size_t index, storage_base&&) {
    if (index == empty_index) {
      new (&empty) empty_type{};
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }

  void assign_at(std::size_t index, const storage_base& other) {
    if (index == empty_index) {
      empty = other.empty;
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }
  void assign_at(std::size_t index, storage_base&& other) {
    if (index == empty_index) {
      empty = std::move(other.empty);
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }

  void swap_at(std::size_t index, storage_base& other) {
    if (index == empty_index) {
      using std::swap;
      swap(empty, other.empty);
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }

  template <typename... Args>
  std::size_t construct(type_tag<empty_type>, Args&&...) {
    new (&empty) empty_type{};
    return empty_index;
  }
  template <typename... Args>
  std::size_t construct(index_tag<empty_index>, Args&&...) {
    new (&empty) empty_type{};
    return empty_index;
  }

  void reset(std::size_t index) {
    if (index == empty_index) {
      empty.empty_type::~empty_type();
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }

  empty_type& get(type_tag<empty_type>) { return empty; }
  const empty_type& get(type_tag<empty_type>) const { return empty; }
  empty_type& get(index_tag<empty_index>) { return empty; }
  const empty_type& get(index_tag<empty_index>) const { return empty; }

  std::size_t index(type_tag<empty_type>) const { return empty_index; }

  template <typename V>
  bool visit(std::size_t, V&&) {
    return false;
  }
  template <typename V>
  bool visit(std::size_t, V&&) const {
    return false;
  }

  empty_type empty;
};

// Trivial terminal case.
template <>
union storage_base<destructor_class::trivial, type_index<empty_type, empty_index>> {
  constexpr storage_base() : empty{} {}

  template <typename... Args>
  constexpr storage_base(type_tag<empty_type>, Args&&...) : empty{} {}
  template <typename... Args>
  constexpr storage_base(index_tag<empty_index>, Args&&...) : empty{} {}

  // Trivial destructor.
  ~storage_base() = default;

  constexpr storage_base(const storage_base&) = default;
  constexpr storage_base(storage_base&&) = default;
  constexpr storage_base& operator=(const storage_base&) = default;
  constexpr storage_base& operator=(storage_base&&) = default;

  constexpr void construct_at(std::size_t index, const storage_base&) {
    if (index == empty_index) {
      new (&empty) empty_type{};
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }
  constexpr void construct_at(std::size_t index, storage_base&&) {
    if (index == empty_index) {
      new (&empty) empty_type{};
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }

  constexpr void assign_at(std::size_t index, const storage_base& other) {
    if (index == empty_index) {
      empty = other.empty;
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }
  constexpr void assign_at(std::size_t index, storage_base&& other) {
    if (index == empty_index) {
      empty = std::move(other.empty);
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }

  constexpr void swap_at(std::size_t index, storage_base& other) {
    if (index == empty_index) {
      using std::swap;
      swap(empty, other.empty);
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }

  template <typename... Args>
  constexpr std::size_t construct(type_tag<empty_type>, Args&&...) {
    new (&empty) empty_type{};
    return empty_index;
  }
  template <typename... Args>
  constexpr std::size_t construct(index_tag<empty_index>, Args&&...) {
    new (&empty) empty_type{};
    return empty_index;
  }

  constexpr void reset(std::size_t index) {
    if (index == empty_index) {
      empty.empty_type::~empty_type();
    } else {
      LIB_STDCOMPAT_INTERNAL_UNREACHABLE_OR_ABORT();
    }
  }

  constexpr empty_type& get(type_tag<empty_type>) { return empty; }
  constexpr const empty_type& get(type_tag<empty_type>) const { return empty; }
  constexpr empty_type& get(index_tag<empty_index>) { return empty; }
  constexpr const empty_type& get(index_tag<empty_index>) const { return empty; }

  constexpr std::size_t index(type_tag<empty_type>) const { return empty_index; }

  template <typename V>
  constexpr bool visit(std::size_t, V&&) {
    return false;
  }
  template <typename V>
  constexpr bool visit(std::size_t, V&&) const {
    return false;
  }

  empty_type empty;
};

template <typename T, std::size_t Index, typename... Ts, std::size_t... Is>
union storage_base<destructor_class::non_trivial, type_index<T, Index>, type_index<Ts, Is>...> {
  storage_base() : empty{} {}

  template <typename... Args>
  storage_base(type_tag<T>, Args&&... args) : value(std::forward<Args>(args)...) {}
  template <typename... Args>
  storage_base(index_tag<Index>, Args&&... args) : value(std::forward<Args>(args)...) {}

  template <typename U, typename... Args>
  storage_base(type_tag<U>, Args&&... args) : rest(type_tag<U>{}, std::forward<Args>(args)...) {}
  template <std::size_t OtherIndex, typename... Args>
  storage_base(index_tag<OtherIndex>, Args&&... args)
      : rest(index_tag<OtherIndex>{}, std::forward<Args>(args)...) {}

  // Non-trivial destructor.
  ~storage_base() {}

  // Trival copy/move construction and assignment.
  storage_base(const storage_base&) = default;
  storage_base(storage_base&&) = default;
  storage_base& operator=(const storage_base&) = default;
  storage_base& operator=(storage_base&&) = default;

  void construct_at(std::size_t index, const storage_base& other) {
    if (index == Index) {
      new (&value) T{other.value};
    } else {
      rest.construct_at(index, other.rest);
    }
  }
  void construct_at(std::size_t index, storage_base&& other) {
    if (index == Index) {
      new (&value) T{std::move(other.value)};
    } else {
      rest.construct_at(index, std::move(other.rest));
    }
  }

  void assign_at(std::size_t index, const storage_base& other) {
    if (index == Index) {
      value = other.value;
    } else {
      rest.assign_at(index, other.rest);
    }
  }
  void assign_at(std::size_t index, storage_base&& other) {
    if (index == Index) {
      value = std::move(other.value);
    } else {
      rest.assign_at(index, std::move(other.rest));
    }
  }

  void swap_at(std::size_t index, storage_base& other) {
    if (index == Index) {
      using std::swap;
      swap(value, other.value);
    } else {
      rest.swap_at(index, other.rest);
    }
  }

  template <typename... Args>
  std::size_t construct(type_tag<T>, Args&&... args) {
    new (&value) T(std::forward<Args>(args)...);
    return Index;
  }
  template <typename U, typename... Args>
  std::size_t construct(type_tag<U>, Args&&... args) {
    return rest.construct(type_tag<U>{}, std::forward<Args>(args)...);
  }
  template <typename... Args>
  std::size_t construct(index_tag<Index>, Args&&... args) {
    new (&value) T(std::forward<Args>(args)...);
    return Index;
  }
  template <std::size_t OtherIndex, typename... Args>
  std::size_t construct(index_tag<OtherIndex>, Args&&... args) {
    return rest.construct(index_tag<OtherIndex>{}, std::forward<Args>(args)...);
  }

  void reset(std::size_t index) {
    if (index == Index) {
      value.~T();
    } else {
      rest.reset(index);
    }
  }

  T& get(type_tag<T>) { return value; }
  const T& get(type_tag<T>) const { return value; }
  template <typename U>
  U& get(type_tag<U>) {
    return rest.get(type_tag<U>{});
  }
  template <typename U>
  const U& get(type_tag<U>) const {
    return rest.get(type_tag<U>{});
  }
  T& get(index_tag<Index>) { return value; }
  const T& get(index_tag<Index>) const { return value; }
  template <std::size_t OtherIndex>
  auto& get(index_tag<OtherIndex>) {
    return rest.get(index_tag<OtherIndex>{});
  }
  template <std::size_t OtherIndex>
  const auto& get(index_tag<OtherIndex>) const {
    return rest.get(index_tag<OtherIndex>{});
  }

  std::size_t index(type_tag<T>) const { return Index; }
  template <typename U>
  std::size_t index(type_tag<U>) const {
    return rest.index(type_tag<U>{});
  }

  template <typename V>
  bool visit(std::size_t index, V&& visitor) {
    if (index == Index) {
      std::forward<V>(visitor)(type_tag<T>{}, index_tag<Index>{}, this);
      return true;
    } else {
      return rest.visit(index, std::forward<V>(visitor));
    }
  }
  template <typename V>
  bool visit(std::size_t index, V&& visitor) const {
    if (index == Index) {
      std::forward<V>(visitor)(type_tag<T>{}, index_tag<Index>{}, this);
      return true;
    } else {
      return rest.visit(index, std::forward<V>(visitor));
    }
  }

  empty_type empty;
  T value;
  storage_base<destructor_class::non_trivial, type_index<Ts, Is>...> rest;
};

template <typename T, std::size_t Index, typename... Ts, std::size_t... Is>
union storage_base<destructor_class::trivial, type_index<T, Index>, type_index<Ts, Is>...> {
  constexpr storage_base() : empty{} {}

  template <typename... Args>
  constexpr storage_base(type_tag<T>, Args&&... args) : value(std::forward<Args>(args)...) {}
  template <typename... Args>
  constexpr storage_base(index_tag<Index>, Args&&... args) : value(std::forward<Args>(args)...) {}

  template <typename U, typename... Args>
  constexpr storage_base(type_tag<U>, Args&&... args)
      : rest(type_tag<U>{}, std::forward<Args>(args)...) {}
  template <std::size_t OtherIndex, typename... Args>
  constexpr storage_base(index_tag<OtherIndex>, Args&&... args)
      : rest(index_tag<OtherIndex>{}, std::forward<Args>(args)...) {}

  // Trivial destructor.
  ~storage_base() = default;

  // Trival copy/move construction and assignment.
  constexpr storage_base(const storage_base&) = default;
  constexpr storage_base(storage_base&&) = default;
  constexpr storage_base& operator=(const storage_base&) = default;
  constexpr storage_base& operator=(storage_base&&) = default;

  constexpr void construct_at(std::size_t index, const storage_base& other) {
    if (index == Index) {
      new (&value) T{other.value};
    } else {
      rest.construct_at(index, other.rest);
    }
  }
  constexpr void construct_at(std::size_t index, storage_base&& other) {
    if (index == Index) {
      new (&value) T{std::move(other.value)};
    } else {
      rest.construct_at(index, std::move(other.rest));
    }
  }

  constexpr void assign_at(std::size_t index, const storage_base& other) {
    if (index == Index) {
      value = other.value;
    } else {
      rest.assign_at(index, other.rest);
    }
  }
  constexpr void assign_at(std::size_t index, storage_base&& other) {
    if (index == Index) {
      value = std::move(other.value);
    } else {
      rest.assign_at(index, std::move(other.rest));
    }
  }

  constexpr void swap_at(std::size_t index, storage_base& other) {
    if (index == Index) {
      using std::swap;
      swap(value, other.value);
    } else {
      rest.swap_at(index, other.rest);
    }
  }

  template <typename... Args>
  constexpr std::size_t construct(type_tag<T>, Args&&... args) {
    new (&value) T(std::forward<Args>(args)...);
    return Index;
  }
  template <typename U, typename... Args>
  constexpr std::size_t construct(type_tag<U>, Args&&... args) {
    return rest.construct(type_tag<U>{}, std::forward<Args>(args)...);
  }
  template <typename... Args>
  constexpr std::size_t construct(index_tag<Index>, Args&&... args) {
    new (&value) T(std::forward<Args>(args)...);
    return Index;
  }
  template <std::size_t OtherIndex, typename... Args>
  constexpr std::size_t construct(index_tag<OtherIndex>, Args&&... args) {
    return rest.construct(index_tag<OtherIndex>{}, std::forward<Args>(args)...);
  }

  constexpr void reset(std::size_t) {}

  constexpr T& get(type_tag<T>) { return value; }
  constexpr const T& get(type_tag<T>) const { return value; }
  template <typename U>
  constexpr U& get(type_tag<U>) {
    return rest.get(type_tag<U>{});
  }
  template <typename U>
  constexpr const U& get(type_tag<U>) const {
    return rest.get(type_tag<U>{});
  }
  constexpr T& get(index_tag<Index>) { return value; }
  constexpr const T& get(index_tag<Index>) const { return value; }
  template <std::size_t OtherIndex>
  constexpr auto& get(index_tag<OtherIndex>) {
    return rest.get(index_tag<OtherIndex>{});
  }
  template <std::size_t OtherIndex>
  constexpr const auto& get(index_tag<OtherIndex>) const {
    return rest.get(index_tag<OtherIndex>{});
  }

  constexpr std::size_t index(type_tag<T>) const { return Index; }
  template <typename U>
  constexpr std::size_t index(type_tag<U>) const {
    return rest.index(type_tag<U>{});
  }

  template <typename V>
  constexpr bool visit(std::size_t index, V&& visitor) {
    if (index == Index) {
      std::forward<V>(visitor)(type_tag<T>{}, index_tag<Index>{}, this);
      return true;
    } else {
      return rest.visit(index, std::forward<V>(visitor));
    }
  }
  template <typename V>
  constexpr bool visit(std::size_t index, V&& visitor) const {
    if (index == Index) {
      std::forward<V>(visitor)(type_tag<T>{}, index_tag<Index>{}, this);
      return true;
    } else {
      return rest.visit(index, std::forward<V>(visitor));
    }
  }

  empty_type empty;
  T value;
  storage_base<destructor_class::trivial, type_index<Ts, Is>...> rest;
};

// Lazy-initialized union storage type that tracks the index of the active
// variant.
template <destructor_class, typename...>
class indexed_storage;

template <destructor_class DestructorClass, typename... Ts, std::size_t... Is>
class indexed_storage<DestructorClass, type_index<Ts, Is>...> {
 private:
  using base_type =
      storage_base<DestructorClass, type_index<Ts, Is>..., type_index<empty_type, empty_index>>;

 public:
  static constexpr bool nothrow_default_constructible =
      std::is_nothrow_default_constructible<first_t<Ts...>>::value;
  static constexpr bool nothrow_move_constructible =
      conjunction_v<std::is_nothrow_move_constructible<Ts>...>;
  static constexpr bool nothrow_move_assignable =
      conjunction_v<std::is_nothrow_move_assignable<Ts>...>;

  constexpr indexed_storage() = default;

  constexpr indexed_storage(trivial_init_t) : indexed_storage{} {}

  constexpr indexed_storage(default_init_t) : index_{0}, base_{index_tag<0>{}} {}

  // Only used by trivial copy/move types.
  constexpr indexed_storage(const indexed_storage& other) = default;
  constexpr indexed_storage& operator=(const indexed_storage& other) = default;
  constexpr indexed_storage(indexed_storage&& other) = default;
  constexpr indexed_storage& operator=(indexed_storage&& other) = default;

  template <typename T, typename... Args>
  constexpr indexed_storage(type_tag<T>, Args&&... args)
      : base_(type_tag<T>{}, std::forward<Args>(args)...) {
    index_ = base_.index(type_tag<T>{});
  }
  template <std::size_t Index, typename... Args>
  constexpr indexed_storage(index_tag<Index>, Args&&... args)
      : index_{Index}, base_(index_tag<Index>{}, std::forward<Args>(args)...) {}

  constexpr indexed_storage(maybe_init_t, const indexed_storage& other)
      : index_{other.index()}, base_{} {
    base_.construct_at(other.index(), other.base_);
  }
  constexpr indexed_storage(maybe_init_t, indexed_storage&& other)
      : index_{other.index()}, base_{} {
    base_.construct_at(other.index(), std::move(other.base_));
  }

  ~indexed_storage() = default;

  constexpr index_type index() const { return index_; }
  constexpr bool is_empty() const { return index() == empty_index; }
  template <typename T>
  constexpr bool has_value(type_tag<T>) const {
    return index() == base_.index(type_tag<T>{});
  }
  template <std::size_t Index>
  constexpr bool has_value(index_tag<Index>) const {
    return index() == Index;
  }

  template <typename T>
  constexpr auto& get(type_tag<T>) {
    return base_.get(type_tag<T>{});
  }
  template <typename T>
  constexpr const auto& get(type_tag<T>) const {
    return base_.get(type_tag<T>{});
  }
  template <std::size_t Index>
  constexpr auto& get(index_tag<Index>) {
    return base_.get(index_tag<Index>{});
  }
  template <std::size_t Index>
  constexpr const auto& get(index_tag<Index>) const {
    return base_.get(index_tag<Index>{});
  }

  template <typename T, typename... Args>
  constexpr void construct(type_tag<T>, Args&&... args) {
    index_ = base_.construct(type_tag<T>{}, std::forward<Args>(args)...);
  }
  template <std::size_t Index, typename... Args>
  constexpr void construct(index_tag<Index>, Args&&... args) {
    index_ = base_.construct(index_tag<Index>{}, std::forward<Args>(args)...);
  }

  constexpr void assign(const indexed_storage& other) {
    if (index() == other.index()) {
      base_.assign_at(index_, other.base_);
    } else {
      reset();
      base_.construct_at(other.index_, other.base_);
      index_ = other.index_;
    }
  }
  constexpr void assign(indexed_storage&& other) {
    if (index() == other.index()) {
      base_.assign_at(index_, std::move(other.base_));
    } else {
      reset();
      base_.construct_at(other.index_, std::move(other.base_));
      index_ = other.index_;
    }
  }

  template <typename V>
  constexpr bool visit(V&& visitor) {
    return base_.visit(index_, std::forward<V>(visitor));
  }
  template <typename V>
  constexpr bool visit(V&& visitor) const {
    return base_.visit(index_, std::forward<V>(visitor));
  }

  constexpr void swap(indexed_storage& other) {
    if (index() == other.index()) {
      // Swap directly when the variants are the same, including empty.
      base_.swap_at(index_, other.base_);
    } else {
      // Swap when the variants are different, including one being empty.
      // This approach avoids GCC -Wmaybe-uninitialized warnings by
      // initializing and accessing |temp| unconditionally within a
      // conditional scope. The alternative, using the maybe_init_t
      // constructor confuses GCC because it doesn't understand that the
      // index checks prevent uninitialized access.
      auto do_swap = [](indexed_storage& a, indexed_storage& b) {
        return a.base_.visit(a.index_, [&a, &b](auto, auto index_tag_v, auto* element) {
          indexed_storage temp{index_tag_v, std::move(element->value)};
          a.reset();

          a.base_.construct_at(b.index_, std::move(b.base_));
          a.index_ = b.index_;
          b.reset();

          b.base_.construct_at(temp.index_, std::move(temp.base_));
          b.index_ = temp.index_;
          temp.reset();
        });
      };

      // The visitor above returns false when the first argument is empty
      // and no action is taken. In that case, the other order is tried to
      // complete the half-empty swap.
      do_swap(*this, other) || do_swap(other, *this);
    }
  }

  // Destroys the active variant. Does nothing when already empty.
  constexpr void reset() {
    base_.reset(index_);
    index_ = empty_index;
  }

 private:
  index_type index_{empty_index};
  base_type base_;
};

// Internal variant storage type used by cpp17::optional and cpp17::variant.
// Specializations of this type select trivial vs. non-trivial copy/move
// construction, assignment operators, and destructor based on the storage class
// of the types in Ts.
template <typename StorageClass, typename... Ts>
struct storage;

template <typename... Ts, std::size_t... Is>
struct storage<storage_class<destructor_class::trivial, copy_class::trivial, move_class::trivial>,
               type_index<Ts, Is>...>
    : indexed_storage<destructor_class::trivial, type_index<Ts, Is>...> {
  using base_type = indexed_storage<destructor_class::trivial, type_index<Ts, Is>...>;
  using base_type::base_type;
  constexpr storage() = default;
};

template <typename... Ts, std::size_t... Is>
struct storage<
    storage_class<destructor_class::trivial, copy_class::non_trivial, move_class::trivial>,
    type_index<Ts, Is>...> : indexed_storage<destructor_class::trivial, type_index<Ts, Is>...> {
  using base_type = indexed_storage<destructor_class::trivial, type_index<Ts, Is>...>;
  using base_type::base_type;

  ~storage() = default;
  constexpr storage() = default;

  constexpr storage(const storage& other) : base_type{maybe_init_v, other} {}

  constexpr storage& operator=(const storage& other) {
    this->assign(other);
    return *this;
  }

  constexpr storage(storage&&) = default;
  constexpr storage& operator=(storage&&) = default;
};

template <typename... Ts, std::size_t... Is>
struct storage<
    storage_class<destructor_class::trivial, copy_class::trivial, move_class::non_trivial>,
    type_index<Ts, Is>...> : indexed_storage<destructor_class::trivial, type_index<Ts, Is>...> {
  using base_type = indexed_storage<destructor_class::trivial, type_index<Ts, Is>...>;
  using base_type::base_type;

  ~storage() = default;
  constexpr storage() = default;
  constexpr storage(const storage&) = default;
  constexpr storage& operator=(const storage&) = default;

  constexpr storage(storage&& other) noexcept(base_type::nothrow_move_constructible)
      : base_type{maybe_init_v, std::move(other)} {}

  constexpr storage& operator=(storage&& other) noexcept(base_type::nothrow_move_assignable) {
    this->assign(std::move(other));
    return *this;
  }
};

template <typename... Ts, std::size_t... Is>
struct storage<
    storage_class<destructor_class::trivial, copy_class::non_trivial, move_class::non_trivial>,
    type_index<Ts, Is>...> : indexed_storage<destructor_class::trivial, type_index<Ts, Is>...> {
  using base_type = indexed_storage<destructor_class::trivial, type_index<Ts, Is>...>;
  using base_type::base_type;

  ~storage() = default;
  constexpr storage() = default;

  constexpr storage(const storage& other) : base_type{maybe_init_v, other} {}

  constexpr storage& operator=(const storage& other) {
    this->assign(other);
    return *this;
  }

  constexpr storage(storage&& other) noexcept(base_type::nothrow_move_constructible)
      : base_type{maybe_init_v, std::move(other)} {}

  constexpr storage& operator=(storage&& other) noexcept(base_type::nothrow_move_assignable) {
    this->assign(std::move(other));
    return *this;
  }
};

// Specialization for non-trivially movable/copyable types. Types with a non-
// trivial destructor are always non-trivially movable/copyable.
template <copy_class CopyClass, move_class MoveClass, typename... Ts, std::size_t... Is>
struct storage<storage_class<destructor_class::non_trivial, CopyClass, MoveClass>,
               type_index<Ts, Is>...>
    : indexed_storage<destructor_class::non_trivial, type_index<Ts, Is>...> {
  using base_type = indexed_storage<destructor_class::non_trivial, type_index<Ts, Is>...>;
  using base_type::base_type;

  ~storage() { this->reset(); }

  constexpr storage() = default;

  constexpr storage(const storage& other) : base_type{maybe_init_v, other} {}

  constexpr storage& operator=(const storage& other) {
    this->assign(other);
    return *this;
  }

  constexpr storage(storage&& other) noexcept(base_type::nothrow_move_constructible)
      : base_type{maybe_init_v, std::move(other)} {}

  constexpr storage& operator=(storage&& other) noexcept(base_type::nothrow_move_assignable) {
    this->assign(std::move(other));
    return *this;
  }
};

template <typename... Ts, std::size_t... Is>
constexpr auto make_storage(std::index_sequence<Is...>) {
  return storage<make_storage_class<Ts...>, type_index<Ts, Is>...>{};
}

template <typename... Ts>
using storage_type = decltype(make_storage<Ts...>(std::index_sequence_for<Ts...>{}));

}  // namespace internal
}  // namespace cpp17

#endif  // LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_STORAGE_H_
