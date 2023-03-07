# Copyright 2022 The Pigweed Authors
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

pw_add_library(pw_intrusive_ptr STATIC
  HEADERS
    public/pw_intrusive_ptr/internal/ref_counted_base.h
    public/pw_intrusive_ptr/intrusive_ptr.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
  SOURCES
    ref_counted_base.cc
)

pw_add_test(pw_intrusive_ptr.intrusive_ptr_test
  SOURCES
    intrusive_ptr_test.cc
  PRIVATE_DEPS
    pw_intrusive_ptr
  GROUPS
    modules
    pw_intrusive_ptr
)
