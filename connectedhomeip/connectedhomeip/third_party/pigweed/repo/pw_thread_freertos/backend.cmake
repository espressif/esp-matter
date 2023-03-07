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
include_guard(GLOBAL)

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

# Unfortunately FreeRTOS entirely hides the contents of the TCB inside of
# tasks.c but it's necessary for snapshot processing in order to access the
# stack limits. Set this to a pw_source_set which provides the tskTCB struct
# definition for snapshot to work with FreeRTOS. By default, this is
# auto-generated from FreeRTOS sources and shouldn't need to be manually
# modified.
#
# See the pw_thread_freertos docs for more details.
pw_add_backend_variable(pw_thread_freertos.freertos_tsktcb_BACKEND)
