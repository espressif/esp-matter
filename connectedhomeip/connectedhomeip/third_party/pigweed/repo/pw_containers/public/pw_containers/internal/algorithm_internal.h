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
#pragma once

#include <cstddef>
#include <iterator>
#include <utility>

namespace pw::containers::internal_algorithm {

// The type of the iterator given by begin(c) (possibly std::begin(c)).
// ContainerIter<const vector<T>> gives vector<T>::const_iterator,
// while ContainerIter<vector<T>> gives vector<T>::iterator.
template <typename C>
using ContainerIter = decltype(std::begin(std::declval<C&>()));

// An MSVC bug involving template parameter substitution requires us to use
// decltype() here instead of just std::pair.
template <typename C1, typename C2>
using ContainerIterPairType =
    decltype(std::make_pair(ContainerIter<C1>(), ContainerIter<C2>()));

template <typename C>
using ContainerDifferenceType = decltype(std::distance(
    std::declval<ContainerIter<C>>(), std::declval<ContainerIter<C>>()));

}  // namespace pw::containers::internal_algorithm
