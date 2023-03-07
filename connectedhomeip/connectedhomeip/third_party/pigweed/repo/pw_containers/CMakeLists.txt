# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

pw_add_library(pw_containers INTERFACE
  PUBLIC_DEPS
    pw_containers.algorithm
    pw_containers.flat_map
    pw_containers.intrusive_list
    pw_containers.vector
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_CONTAINERS)
  zephyr_link_libraries(pw_containers)
endif()

pw_add_library(pw_containers.algorithm INTERFACE
  HEADERS
    public/pw_containers/algorithm.h
    public/pw_containers/internal/algorithm_internal.h
  PUBLIC_INCLUDES
    public
)

pw_add_library(pw_containers.filtered_view INTERFACE
  HEADERS
    public/pw_containers/filtered_view.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_preprocessor
)

pw_add_library(pw_containers.flat_map INTERFACE
  HEADERS
    public/pw_containers/flat_map.h
  PUBLIC_INCLUDES
    public
)

pw_add_library(pw_containers.iterator INTERFACE
  HEADERS
    public/pw_containers/iterator.h
  PUBLIC_INCLUDES
    public
)

pw_add_library(pw_containers.to_array INTERFACE
  HEADERS
    public/pw_containers/to_array.h
  PUBLIC_INCLUDES
    public
)

pw_add_library(pw_containers.vector INTERFACE
  HEADERS
    public/pw_containers/vector.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_polyfill
)

pw_add_library(pw_containers.wrapped_iterator INTERFACE
  HEADERS
    public/pw_containers/wrapped_iterator.h
  PUBLIC_INCLUDES
    public
)

pw_add_library(pw_containers.intrusive_list STATIC
  HEADERS
    public/pw_containers/internal/intrusive_list_impl.h
    public/pw_containers/intrusive_list.h
  PUBLIC_INCLUDES
    public
  SOURCES
    intrusive_list.cc
  PRIVATE_DEPS
    pw_assert
)

pw_add_test(pw_containers.algorithm_test
  SOURCES
    algorithm_test.cc
  PRIVATE_DEPS
    pw_containers.algorithm
    pw_containers.intrusive_list
    pw_containers.vector
    pw_span
  GROUPS
    modules
    pw_containers
)

pw_add_test(pw_containers.filtered_view_test
  SOURCES
    filtered_view_test.cc
  PRIVATE_DEPS
    pw_containers.filtered_view
    pw_containers.intrusive_list
    pw_span
  GROUPS
    modules
    pw_containers
)

pw_add_test(pw_containers.flat_map_test
  SOURCES
    flat_map_test.cc
  PRIVATE_DEPS
    pw_containers.flat_map
  GROUPS
    modules
    pw_containers
)

pw_add_test(pw_containers.to_array_test
  SOURCES
    to_array_test.cc
  PRIVATE_DEPS
    pw_containers.to_array
  GROUPS
    modules
    pw_containers
)

pw_add_test(pw_containers.vector_test
  SOURCES
    vector_test.cc
  PRIVATE_DEPS
    pw_containers.vector
  GROUPS
    modules
    pw_containers
)

pw_add_test(pw_containers.wrapped_iterator_test
  SOURCES
    wrapped_iterator_test.cc
  PRIVATE_DEPS
    pw_containers.wrapped_iterator
  GROUPS
    modules
    pw_containers
)

pw_add_test(pw_containers.intrusive_list_test
  SOURCES
    intrusive_list_test.cc
  PRIVATE_DEPS
    pw_compilation_testing._pigweed_only_negative_compilation
    pw_containers.intrusive_list
    pw_preprocessor
  GROUPS
    modules
    pw_containers
)
