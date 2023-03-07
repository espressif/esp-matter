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

pw_add_library(pw_preprocessor INTERFACE
  HEADERS
    public/pw_preprocessor/arguments.h
    public/pw_preprocessor/boolean.h
    public/pw_preprocessor/compiler.h
    public/pw_preprocessor/concat.h
    public/pw_preprocessor/util.h
    public/pw_preprocessor/internal/arg_count_impl.h
  PUBLIC_INCLUDES
    public
)

pw_add_library(pw_preprocessor.arch INTERFACE
  HEADERS
    public/pw_preprocessor/arch.h
  PUBLIC_INCLUDES
    public
)

if(Zephyr_FOUND AND CONFIG_PIGWEED_PREPROCESSOR)
  zephyr_link_libraries(pw_preprocessor)
endif()
