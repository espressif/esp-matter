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

pw_add_library(pw_metric STATIC
  HEADERS
    public/pw_metric/metric.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_tokenizer.base64
    pw_assert
    pw_containers
    pw_log
    pw_tokenizer
  SOURCES
    metric.cc
  PRIVATE_DEPS
    pw_span
)

pw_add_library(pw_metric.global STATIC
  HEADERS
    public/pw_metric/global.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_metric
    pw_tokenizer
  SOURCES
    global.cc
)

pw_add_test(pw_metric.metric_test
  SOURCES
    metric_test.cc
  PRIVATE_DEPS
    pw_metric
  GROUPS
    modules
    pw_metric
)

pw_add_test(pw_metric.global_test
  SOURCES
    global_test.cc
  PRIVATE_DEPS
    pw_metric.global
  GROUPS
    modules
    pw_metric
)
