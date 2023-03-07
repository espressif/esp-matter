// Copyright 2022 The Pigweed Authors
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
// -----------------------------------------------------------------------------
// File: algorithm.h
// -----------------------------------------------------------------------------
//
// This header file provides Container-based versions of algorithmic functions
// within the C++ standard library, based on a subset of
// absl/algorithm/container.h. The following standard library sets of functions
// are covered within this file:
//
//   * <algorithm> functions
//
// The standard library functions operate on iterator ranges; the functions
// within this API operate on containers, though many return iterator ranges.
//
// All functions within this API are CamelCase instead of their std::
// snake_case counterparts. Calls such as `pw::containers::Foo(container, ...)
// are equivalent to std:: functions such as
// `std::foo(std::begin(cont), std::end(cont), ...)`. Functions that act on
// iterators but not conceptually on iterator ranges (e.g. `std::iter_swap`)
// have no equivalent here.
//
// For template parameter and variable naming, `C` indicates the container type
// to which the function is applied, `Pred` indicates the predicate object type
// to be used by the function and `T` indicates the applicable element type.
//
// This was forked from
// https://cs.opensource.google/abseil/abseil-cpp/+/main:absl/algorithm/algorithm.h;drc=12bc53e0318d80569270a5b26ccbc62b52022b89
#pragma once

#include <algorithm>
#include <utility>

#include "pw_containers/internal/algorithm_internal.h"

namespace pw::containers {

//------------------------------------------------------------------------------
// <algorithm> Non-modifying sequence operations
//------------------------------------------------------------------------------

// AllOf()
//
// Container-based version of the <algorithm> `std::all_of()` function to
// test if all elements within a container satisfy a condition.
template <typename C, typename Pred>
bool AllOf(const C& c, Pred&& pred) {
  return std::all_of(std::begin(c), std::end(c), std::forward<Pred>(pred));
}

// AnyOf()
//
// Container-based version of the <algorithm> `std::any_of()` function to
// test if any element in a container fulfills a condition.
template <typename C, typename Pred>
bool AnyOf(const C& c, Pred&& pred) {
  return std::any_of(std::begin(c), std::end(c), std::forward<Pred>(pred));
}

// NoneOf()
//
// Container-based version of the <algorithm> `std::none_of()` function to
// test if no elements in a container fulfill a condition.
template <typename C, typename Pred>
bool NoneOf(const C& c, Pred&& pred) {
  return std::none_of(std::begin(c), std::end(c), std::forward<Pred>(pred));
}

// ForEach()
//
// Container-based version of the <algorithm> `std::for_each()` function to
// apply a function to a container's elements.
template <typename C, typename Function>
std::decay_t<Function> ForEach(C&& c, Function&& f) {
  return std::for_each(std::begin(c), std::end(c), std::forward<Function>(f));
}

// Find()
//
// Container-based version of the <algorithm> `std::find()` function to find
// the first element containing the passed value within a container value.
template <typename C, typename T>
internal_algorithm::ContainerIter<C> Find(C& c, T&& value) {
  return std::find(std::begin(c), std::end(c), std::forward<T>(value));
}

// FindIf()
//
// Container-based version of the <algorithm> `std::find_if()` function to find
// the first element in a container matching the given condition.
template <typename C, typename Pred>
internal_algorithm::ContainerIter<C> FindIf(C& c, Pred&& pred) {
  return std::find_if(std::begin(c), std::end(c), std::forward<Pred>(pred));
}

// FindIfNot()
//
// Container-based version of the <algorithm> `std::find_if_not()` function to
// find the first element in a container not matching the given condition.
template <typename C, typename Pred>
internal_algorithm::ContainerIter<C> FindIfNot(C& c, Pred&& pred) {
  return std::find_if_not(std::begin(c), std::end(c), std::forward<Pred>(pred));
}

// FindEnd()
//
// Container-based version of the <algorithm> `std::find_end()` function to
// find the last subsequence within a container.
template <typename Sequence1, typename Sequence2>
internal_algorithm::ContainerIter<Sequence1> FindEnd(Sequence1& sequence,
                                                     Sequence2& subsequence) {
  return std::find_end(std::begin(sequence),
                       std::end(sequence),
                       std::begin(subsequence),
                       std::end(subsequence));
}

// Overload of FindEnd() for using a predicate evaluation other than `==` as
// the function's test condition.
template <typename Sequence1, typename Sequence2, typename BinaryPredicate>
internal_algorithm::ContainerIter<Sequence1> FindEnd(Sequence1& sequence,
                                                     Sequence2& subsequence,
                                                     BinaryPredicate&& pred) {
  return std::find_end(std::begin(sequence),
                       std::end(sequence),
                       std::begin(subsequence),
                       std::end(subsequence),
                       std::forward<BinaryPredicate>(pred));
}

// FindFirstOf()
//
// Container-based version of the <algorithm> `std::find_first_of()` function to
// find the first element within the container that is also within the options
// container.
template <typename C1, typename C2>
internal_algorithm::ContainerIter<C1> FindFirstOf(C1& container, C2& options) {
  return std::find_first_of(std::begin(container),
                            std::end(container),
                            std::begin(options),
                            std::end(options));
}

// Overload of FindFirstOf() for using a predicate evaluation other than
// `==` as the function's test condition.
template <typename C1, typename C2, typename BinaryPredicate>
internal_algorithm::ContainerIter<C1> FindFirstOf(C1& container,
                                                  C2& options,
                                                  BinaryPredicate&& pred) {
  return std::find_first_of(std::begin(container),
                            std::end(container),
                            std::begin(options),
                            std::end(options),
                            std::forward<BinaryPredicate>(pred));
}

// AdjacentFind()
//
// Container-based version of the <algorithm> `std::adjacent_find()` function to
// find equal adjacent elements within a container.
template <typename Sequence>
internal_algorithm::ContainerIter<Sequence> AdjacentFind(Sequence& sequence) {
  return std::adjacent_find(std::begin(sequence), std::end(sequence));
}

// Overload of AdjacentFind() for using a predicate evaluation other than
// `==` as the function's test condition.
template <typename Sequence, typename BinaryPredicate>
internal_algorithm::ContainerIter<Sequence> AdjacentFind(
    Sequence& sequence, BinaryPredicate&& pred) {
  return std::adjacent_find(std::begin(sequence),
                            std::end(sequence),
                            std::forward<BinaryPredicate>(pred));
}

// Count()
//
// Container-based version of the <algorithm> `std::count()` function to count
// values that match within a container.
template <typename C, typename T>
internal_algorithm::ContainerDifferenceType<const C> Count(const C& c,
                                                           T&& value) {
  return std::count(std::begin(c), std::end(c), std::forward<T>(value));
}

// CountIOf()
//
// Container-based version of the <algorithm> `std::count_if()` function to
// count values matching a condition within a container.
template <typename C, typename Pred>
internal_algorithm::ContainerDifferenceType<const C> CountIf(const C& c,
                                                             Pred&& pred) {
  return std::count_if(std::begin(c), std::end(c), std::forward<Pred>(pred));
}

// Mismatch()
//
// Container-based version of the <algorithm> `std::mismatch()` function to
// return the first element where two ordered containers differ. Applies `==` to
// the first N elements of `c1` and `c2`, where N = min(size(c1), size(c2)).
template <typename C1, typename C2>
internal_algorithm::ContainerIterPairType<C1, C2> Mismatch(C1& c1, C2& c2) {
  auto first1 = std::begin(c1);
  auto last1 = std::end(c1);
  auto first2 = std::begin(c2);
  auto last2 = std::end(c2);

  for (; first1 != last1 && first2 != last2; ++first1, (void)++first2) {
    // Negates equality because Cpp17EqualityComparable doesn't require clients
    // to overload both `operator==` and `operator!=`.
    if (!(*first1 == *first2)) {
      break;
    }
  }

  return std::make_pair(first1, first2);
}

// Overload of Mismatch() for using a predicate evaluation other than `==` as
// the function's test condition. Applies `pred`to the first N elements of `c1`
// and `c2`, where N = min(size(c1), size(c2)).
template <typename C1, typename C2, typename BinaryPredicate>
internal_algorithm::ContainerIterPairType<C1, C2> Mismatch(
    C1& c1, C2& c2, BinaryPredicate pred) {
  auto first1 = std::begin(c1);
  auto last1 = std::end(c1);
  auto first2 = std::begin(c2);
  auto last2 = std::end(c2);

  for (; first1 != last1 && first2 != last2; ++first1, (void)++first2) {
    if (!pred(*first1, *first2)) {
      break;
    }
  }

  return std::make_pair(first1, first2);
}

// Equal()
//
// Container-based version of the <algorithm> `std::equal()` function to
// test whether two containers are equal.
//
// NOTE: the semantics of Equal() are slightly different than those of
// equal(): while the latter iterates over the second container only up to the
// size of the first container, Equal() also checks whether the container
// sizes are equal.  This better matches expectations about Equal() based on
// its signature.
//
// Example:
//   vector v1 = <1, 2, 3>;
//   vector v2 = <1, 2, 3, 4>;
//   equal(std::begin(v1), std::end(v1), std::begin(v2)) returns true
//   Equal(v1, v2) returns false

template <typename C1, typename C2>
bool Equal(const C1& c1, const C2& c2) {
  return ((std::size(c1) == std::size(c2)) &&
          std::equal(std::begin(c1), std::end(c1), std::begin(c2)));
}

// Overload of Equal() for using a predicate evaluation other than `==` as
// the function's test condition.
template <typename C1, typename C2, typename BinaryPredicate>
bool Equal(const C1& c1, const C2& c2, BinaryPredicate&& pred) {
  return ((std::size(c1) == std::size(c2)) &&
          std::equal(std::begin(c1),
                     std::end(c1),
                     std::begin(c2),
                     std::forward<BinaryPredicate>(pred)));
}

// IsPermutation()
//
// Container-based version of the <algorithm> `std::is_permutation()` function
// to test whether a container is a permutation of another.
template <typename C1, typename C2>
bool IsPermutation(const C1& c1, const C2& c2) {
  using std::begin;
  using std::end;
  return c1.size() == c2.size() &&
         std::is_permutation(begin(c1), end(c1), begin(c2));
}

// Overload of IsPermutation() for using a predicate evaluation other than
// `==` as the function's test condition.
template <typename C1, typename C2, typename BinaryPredicate>
bool IsPermutation(const C1& c1, const C2& c2, BinaryPredicate&& pred) {
  using std::begin;
  using std::end;
  return c1.size() == c2.size() &&
         std::is_permutation(begin(c1),
                             end(c1),
                             begin(c2),
                             std::forward<BinaryPredicate>(pred));
}

// Search()
//
// Container-based version of the <algorithm> `std::search()` function to search
// a container for a subsequence.
template <typename Sequence1, typename Sequence2>
internal_algorithm::ContainerIter<Sequence1> Search(Sequence1& sequence,
                                                    Sequence2& subsequence) {
  return std::search(std::begin(sequence),
                     std::end(sequence),
                     std::begin(subsequence),
                     std::end(subsequence));
}

// Overload of Search() for using a predicate evaluation other than
// `==` as the function's test condition.
template <typename Sequence1, typename Sequence2, typename BinaryPredicate>
internal_algorithm::ContainerIter<Sequence1> Search(Sequence1& sequence,
                                                    Sequence2& subsequence,
                                                    BinaryPredicate&& pred) {
  return std::search(std::begin(sequence),
                     std::end(sequence),
                     std::begin(subsequence),
                     std::end(subsequence),
                     std::forward<BinaryPredicate>(pred));
}

// SearchN()
//
// Container-based version of the <algorithm> `std::search_n()` function to
// search a container for the first sequence of N elements.
template <typename Sequence, typename Size, typename T>
internal_algorithm::ContainerIter<Sequence> SearchN(Sequence& sequence,
                                                    Size count,
                                                    T&& value) {
  return std::search_n(
      std::begin(sequence), std::end(sequence), count, std::forward<T>(value));
}

// Overload of SearchN() for using a predicate evaluation other than
// `==` as the function's test condition.
template <typename Sequence,
          typename Size,
          typename T,
          typename BinaryPredicate>
internal_algorithm::ContainerIter<Sequence> SearchN(Sequence& sequence,
                                                    Size count,
                                                    T&& value,
                                                    BinaryPredicate&& pred) {
  return std::search_n(std::begin(sequence),
                       std::end(sequence),
                       count,
                       std::forward<T>(value),
                       std::forward<BinaryPredicate>(pred));
}

}  // namespace pw::containers
