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

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <new>
#include <string_view>
#include <type_traits>
#include <utility>

#include "pw_assert/assert.h"
#include "pw_polyfill/language_feature_macros.h"

namespace pw {
namespace vector_impl {

template <typename I>
using IsIterator = std::negation<
    std::is_same<typename std::iterator_traits<I>::value_type, void>>;

// Used as max_size in the generic-size Vector<T> interface.
PW_INLINE_VARIABLE constexpr size_t kGeneric = size_t(-1);

// The DestructorHelper is used to make Vector<T> trivially destructible if T
// is. This could be replaced with a C++20 constraint.
template <typename VectorClass, bool kIsTriviallyDestructible>
class DestructorHelper;

template <typename VectorClass>
class DestructorHelper<VectorClass, true> {
 public:
  ~DestructorHelper() = default;
};

template <typename VectorClass>
class DestructorHelper<VectorClass, false> {
 public:
  ~DestructorHelper() { static_cast<VectorClass*>(this)->clear(); }
};

}  // namespace vector_impl

// The Vector class is similar to std::vector, except it is backed by a
// fixed-size buffer. Vectors must be declared with an explicit maximum size
// (e.g. Vector<int, 10>) but vectors can be used and referred to without the
// max size template parameter (e.g. Vector<int>).
//
// To allow referring to a pw::Vector without an explicit maximum size, all
// Vector classes inherit from Vector<T, vector_impl::kGeneric>, which stores
// the maximum size in a variable. This allows Vectors to be used without having
// to know their maximum size at compile time. It also keeps code size small
// since function implementations are shared for all maximum sizes.
template <typename T, size_t kMaxSize = vector_impl::kGeneric>
class Vector : public Vector<T, vector_impl::kGeneric> {
 public:
  using typename Vector<T, vector_impl::kGeneric>::value_type;
  using typename Vector<T, vector_impl::kGeneric>::size_type;
  using typename Vector<T, vector_impl::kGeneric>::difference_type;
  using typename Vector<T, vector_impl::kGeneric>::reference;
  using typename Vector<T, vector_impl::kGeneric>::const_reference;
  using typename Vector<T, vector_impl::kGeneric>::pointer;
  using typename Vector<T, vector_impl::kGeneric>::const_pointer;
  using typename Vector<T, vector_impl::kGeneric>::iterator;
  using typename Vector<T, vector_impl::kGeneric>::const_iterator;
  using typename Vector<T, vector_impl::kGeneric>::reverse_iterator;
  using typename Vector<T, vector_impl::kGeneric>::const_reverse_iterator;

  // Construct
  Vector() noexcept : Vector<T, vector_impl::kGeneric>(kMaxSize) {}

  Vector(size_type count, const T& value)
      : Vector<T, vector_impl::kGeneric>(kMaxSize, count, value) {}

  explicit Vector(size_type count)
      : Vector<T, vector_impl::kGeneric>(kMaxSize, count, T()) {}

  template <
      typename Iterator,
      typename...,
      typename = std::enable_if_t<vector_impl::IsIterator<Iterator>::value>>
  Vector(Iterator first, Iterator last)
      : Vector<T, vector_impl::kGeneric>(kMaxSize, first, last) {}

  Vector(const Vector& other)
      : Vector<T, vector_impl::kGeneric>(kMaxSize, other) {}

  template <size_t kOtherMaxSize>
  Vector(const Vector<T, kOtherMaxSize>& other)
      : Vector<T, vector_impl::kGeneric>(kMaxSize, other) {}

  Vector(Vector&& other) noexcept
      : Vector<T, vector_impl::kGeneric>(kMaxSize, std::move(other)) {}

  template <size_t kOtherMaxSize>
  Vector(Vector<T, kOtherMaxSize>&& other) noexcept
      : Vector<T, vector_impl::kGeneric>(kMaxSize, std::move(other)) {}

  Vector(std::initializer_list<T> list)
      : Vector<T, vector_impl::kGeneric>(kMaxSize, list) {}

  static constexpr size_t max_size() { return kMaxSize; }

  // Construct from std::string_view when T is char.
  template <typename U = T,
            typename = std::enable_if_t<std::is_same_v<U, char>>>
  Vector(std::string_view source) : Vector(source.begin(), source.end()) {}

  // Construct from const char* when T is char.
  template <typename U = T,
            typename = std::enable_if_t<std::is_same_v<U, char>>>
  Vector(const char* source) : Vector(std::string_view(source)) {}

  Vector& operator=(const Vector& other) {
    Vector<T>::assign(other.begin(), other.end());
    return *this;
  }

  template <size_t kOtherMaxSize>
  Vector& operator=(const Vector<T, kOtherMaxSize>& other) noexcept {
    Vector<T>::assign(other.begin(), other.end());
    return *this;
  }

  Vector& operator=(Vector&& other) noexcept {
    Vector<T>::operator=(std::move(other));
    return *this;
  }

  template <size_t kOtherMaxSize>
  Vector& operator=(Vector<T, kOtherMaxSize>&& other) noexcept {
    Vector<T>::operator=(std::move(other));
    return *this;
  }

  Vector& operator=(std::initializer_list<T> list) {
    this->assign(list.begin(), list.end());
    return *this;
  }

  // All other vector methods are implemented on the Vector<T> base class.

 private:
  friend class Vector<T, vector_impl::kGeneric>;

  static_assert(kMaxSize <= std::numeric_limits<size_type>::max());

  // Provides access to the underlying array as an array of T.
#ifdef __cpp_lib_launder
  pointer array() { return std::launder(reinterpret_cast<T*>(&array_)); }
  const_pointer array() const {
    return std::launder(reinterpret_cast<const T*>(&array_));
  }
#else
  pointer array() { return reinterpret_cast<T*>(&array_); }
  const_pointer array() const { return reinterpret_cast<const T*>(&array_); }
#endif  // __cpp_lib_launder

  // Vector entries are stored as uninitialized memory blocks aligned correctly
  // for the type. Elements are initialized on demand with placement new.
  //
  // This uses std::array instead of a C array to support zero-length Vectors.
  // Zero-length C arrays are non-standard, but std::array<T, 0> is valid.
  // The alignas specifier is required ensure that a zero-length array is
  // aligned the same as an array with elements.
  alignas(T) std::array<std::aligned_storage_t<sizeof(T), alignof(T)>,
                        kMaxSize> array_;
};

// Defines the generic-sized Vector<T> specialization, which serves as the base
// class for Vector<T> of any maximum size. Except for constructors, all Vector
// methods are implemented on this class.
template <typename T>
class Vector<T, vector_impl::kGeneric>
    : public vector_impl::DestructorHelper<
          Vector<T, vector_impl::kGeneric>,
          std::is_trivially_destructible<T>::value> {
 public:
  using value_type = T;

  // Use unsigned short instead of size_t. Since Vectors are statically
  // allocated, 65535 entries is a reasonable upper limit. This reduces Vector's
  // overhead by packing the size and capacity into 32 bits.
  using size_type = unsigned short;

  using difference_type = ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  // A vector without an explicit maximum size (Vector<T>) cannot be constructed
  // directly. Instead, construct a Vector<T, kMaxSize>. Vectors of any max size
  // can be used through a Vector<T> pointer or reference.

  // Assign

  Vector& operator=(const Vector& other) {
    assign(other.begin(), other.end());
    return *this;
  }

  Vector& operator=(Vector&& other) noexcept {
    clear();
    MoveFrom(other);
    return *this;
  }

  Vector& operator=(std::initializer_list<T> list) {
    assign(list);
    return *this;
  }

  void assign(size_type count, const T& value) {
    clear();
    Append(count, value);
  }

  template <
      typename Iterator,
      typename...,
      typename = std::enable_if_t<vector_impl::IsIterator<Iterator>::value>>
  void assign(Iterator first, Iterator last) {
    clear();
    CopyFrom(first, last);
  }

  void assign(std::initializer_list<T> list) {
    assign(list.begin(), list.end());
  }

  // Access

  reference at(size_type index) {
    PW_ASSERT(index < size());
    return data()[index];
  }
  const_reference at(size_type index) const {
    PW_ASSERT(index < size());
    return data()[index];
  }

  reference operator[](size_type index) {
    PW_DASSERT(index < size());
    return data()[index];
  }
  const_reference operator[](size_type index) const {
    PW_DASSERT(index < size());
    return data()[index];
  }

  reference front() { return data()[0]; }
  const_reference front() const { return data()[0]; }

  reference back() { return data()[size() - 1]; }
  const_reference back() const { return data()[size() - 1]; }

  // The underlying data is not part of the generic-length vector class. It is
  // provided in the derived class from which this instance was constructed. To
  // access the data, down-cast this to a Vector with a known max size, and
  // return a pointer to the start of the array, which is the same for all
  // vectors with explicit max size.
  T* data() noexcept { return static_cast<Vector<T, 0>*>(this)->array(); }
  const T* data() const noexcept {
    return static_cast<const Vector<T, 0>*>(this)->array();
  }

  // Iterate

  iterator begin() noexcept { return &data()[0]; }
  const_iterator begin() const noexcept { return &data()[0]; }
  const_iterator cbegin() const noexcept { return &data()[0]; }

  iterator end() noexcept { return &data()[size()]; }
  const_iterator end() const noexcept { return &data()[size()]; }
  const_iterator cend() const noexcept { return &data()[size()]; }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
  const_reverse_iterator crbegin() const noexcept {
    return reverse_iterator(cend());
  }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return reverse_iterator(begin()); }
  const_reverse_iterator crend() const noexcept {
    return reverse_iterator(cbegin());
  }

  // Size

  [[nodiscard]] bool empty() const noexcept { return size() == 0u; }

  // True if there is no free space in the vector. Operations that add elements
  // (push_back, insert, etc.) will fail if full() is true.
  [[nodiscard]] bool full() const noexcept { return size() == max_size(); }

  // Returns the number of elements in the Vector. Uses size_t instead of
  // size_type for consistency with other containers.
  size_t size() const noexcept { return size_; }

  // Returns the maximum number of elements in this Vector.
  size_t max_size() const noexcept { return max_size_; }

  size_t capacity() const noexcept { return max_size(); }

  // Modify

  void clear() noexcept;

  // TODO(hepler): insert, emplace, and erase are not yet implemented.
  //    Currently, items can only be added to or removed from the end.
  iterator insert(const_iterator index, const T& value);

  iterator insert(const_iterator index, T&& value);

  iterator insert(const_iterator index, size_type count, const T& value);

  template <typename Iterator>
  iterator insert(const_iterator index, Iterator first, Iterator last);

  iterator insert(const_iterator index, std::initializer_list<T> list);

  template <typename... Args>
  iterator emplace(const_iterator index, Args&&... args);

  iterator erase(const_iterator index);

  iterator erase(const_iterator first, const_iterator last);

  void push_back(const T& value) { emplace_back(value); }

  void push_back(T&& value) { emplace_back(std::move(value)); }

  template <typename... Args>
  void emplace_back(Args&&... args);

  void pop_back();

  void resize(size_type new_size) { resize(new_size, T()); }

  void resize(size_type new_size, const T& value);

 protected:
  // Vectors without an explicit size cannot be constructed directly. Instead,
  // the maximum size must be provided.
  explicit constexpr Vector(size_type max_size) noexcept
      : max_size_(max_size) {}

  Vector(size_type max_size, size_type count, const T& value)
      : Vector(max_size) {
    Append(count, value);
  }

  Vector(size_type max_size, size_type count) : Vector(max_size, count, T()) {}

  template <
      typename Iterator,
      typename...,
      typename = std::enable_if_t<vector_impl::IsIterator<Iterator>::value>>
  Vector(size_type max_size, Iterator first, Iterator last) : Vector(max_size) {
    CopyFrom(first, last);
  }

  Vector(size_type max_size, const Vector& other) : Vector(max_size) {
    CopyFrom(other.begin(), other.end());
  }

  Vector(size_type max_size, Vector&& other) noexcept : Vector(max_size) {
    MoveFrom(other);
  }

  Vector(size_type max_size, std::initializer_list<T> list) : Vector(max_size) {
    CopyFrom(list.begin(), list.end());
  }

 private:
  template <typename Iterator>
  void CopyFrom(Iterator first, Iterator last);

  void MoveFrom(Vector& other) noexcept;

  void Append(size_type count, const T& value);

  const size_type max_size_;
  size_type size_ = 0;
};

// Compare

template <typename T, size_t kLhsSize, size_t kRhsSize>
bool operator==(const Vector<T, kLhsSize>& lhs,
                const Vector<T, kRhsSize>& rhs) {
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, size_t kLhsSize, size_t kRhsSize>
bool operator!=(const Vector<T, kLhsSize>& lhs,
                const Vector<T, kRhsSize>& rhs) {
  return !(lhs == rhs);
}

template <typename T, size_t kLhsSize, size_t kRhsSize>
bool operator<(const Vector<T, kLhsSize>& lhs, const Vector<T, kRhsSize>& rhs) {
  return std::lexicographical_compare(
      lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, size_t kLhsSize, size_t kRhsSize>
bool operator<=(const Vector<T, kLhsSize>& lhs,
                const Vector<T, kRhsSize>& rhs) {
  return !(rhs < lhs);
}

template <typename T, size_t kLhsSize, size_t kRhsSize>
bool operator>(const Vector<T, kLhsSize>& lhs, const Vector<T, kRhsSize>& rhs) {
  return rhs < lhs;
}

template <typename T, size_t kLhsSize, size_t kRhsSize>
bool operator>=(const Vector<T, kLhsSize>& lhs,
                const Vector<T, kRhsSize>& rhs) {
  return !(lhs < rhs);
}

// Function implementations

template <typename T>
void Vector<T, vector_impl::kGeneric>::clear() noexcept {
  for (auto& item : *this) {
    item.~T();
  }
  size_ = 0;
}

template <typename T>
template <typename... Args>
void Vector<T, vector_impl::kGeneric>::emplace_back(Args&&... args) {
  if (!full()) {
    new (&data()[size_]) T(std::forward<Args>(args)...);
    size_ += 1;
  }
}

template <typename T>
void Vector<T, vector_impl::kGeneric>::pop_back() {
  if (!empty()) {
    back().~T();
    size_ -= 1;
  }
}

template <typename T>
void Vector<T, vector_impl::kGeneric>::resize(size_type new_size,
                                              const T& value) {
  if (size() < new_size) {
    Append(std::min(max_size(), size_t(new_size)) - size(), value);
  } else {
    while (size() > new_size) {
      pop_back();
    }
  }
}

template <typename T>
template <typename Iterator>
void Vector<T, vector_impl::kGeneric>::CopyFrom(Iterator first, Iterator last) {
  while (first != last) {
    push_back(*first++);
  }
}

template <typename T>
void Vector<T, vector_impl::kGeneric>::MoveFrom(Vector& other) noexcept {
  for (auto&& item : other) {
    emplace_back(std::move(item));
  }
  other.clear();
}

template <typename T>
void Vector<T, vector_impl::kGeneric>::Append(size_type count, const T& value) {
  for (size_t i = 0; i < count; ++i) {
    push_back(value);
  }
}

}  // namespace pw
