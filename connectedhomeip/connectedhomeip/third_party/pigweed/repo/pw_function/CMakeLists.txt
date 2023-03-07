# Copyright 2021 The Pigweed Authors
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

pw_add_module_config(pw_function_CONFIG)

pw_add_library(pw_function.config INTERFACE
  HEADERS
    public/pw_function/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    ${pw_function_CONFIG}
)

pw_add_library(pw_function INTERFACE
  HEADERS
    public/pw_function/function.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_function.config
    pw_preprocessor
    pw_third_party.fuchsia.fit
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_FUNCTION)
  zephyr_link_libraries(pw_function)
endif()

pw_add_test(pw_function.function_test
  SOURCES
    function_test.cc
  PRIVATE_DEPS
    pw_compilation_testing._pigweed_only_negative_compilation
    pw_function
    pw_polyfill
  GROUPS
    modules
    pw_function
)
