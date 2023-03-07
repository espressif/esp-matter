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

pw_add_library(pw_polyfill INTERFACE
  HEADERS
    public/pw_polyfill/language_feature_macros.h
    public/pw_polyfill/standard.h
  PUBLIC_INCLUDES
    public
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_POLYFILL)
  zephyr_link_libraries(pw_polyfill)
endif()

pw_add_library(pw_polyfill.standard_library INTERFACE
  HEADERS
    standard_library_public/pw_polyfill/standard_library/namespace.h
  PUBLIC_INCLUDES
    standard_library_public
)
