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
"""Utility for tagging a target as compatible with a host OS."""

_RTOS_NONE = "//pw_build/constraints/rtos:none"

# Common select for tagging a target as only compatible with host OS's. This
# select implements the logic '(Windows, macOS, iOS, Linux, Android, or
# Chromium OS) and not RTOS'.  Example usage:
#   load("//pw_build:selects.bzl","TARGET_COMPATIBLE_WITH_HOST_SELECT")
#   pw_cc_library(
#       name = "some_host_only_lib",
#       hdrs = ["host.h"],
#       target_compatible_with = select(TARGET_COMPATIBLE_WITH_HOST_SELECT),
#   )
TARGET_COMPATIBLE_WITH_HOST_SELECT = {
    "@platforms//os:windows": [_RTOS_NONE],
    "@platforms//os:macos": [_RTOS_NONE],
    "@platforms//os:ios": [_RTOS_NONE],
    "@platforms//os:linux": [_RTOS_NONE],
    "@platforms//os:chromiumos": [_RTOS_NONE],
    "@platforms//os:android": [_RTOS_NONE],
    "//conditions:default": ["@platforms//:incompatible"],
}
