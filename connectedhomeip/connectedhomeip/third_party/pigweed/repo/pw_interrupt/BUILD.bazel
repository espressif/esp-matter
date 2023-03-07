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

load(
    "//pw_build:pigweed.bzl",
    "pw_cc_facade",
    "pw_cc_library",
)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

pw_cc_facade(
    name = "context_facade",
    hdrs = [
        "public/pw_interrupt/context.h",
    ],
    includes = ["public"],
)

pw_cc_library(
    name = "context",
    deps = [
        ":context_facade",
        "@pigweed_config//:pw_interrupt_backend",
    ],
)

pw_cc_library(
    name = "backend_multiplexer",
    # Normally this would be done in the backend packages but because there is
    # no host implementation we have to define this here.
    target_compatible_with = select({
        "@platforms//cpu:armv7-m": [],
        "@platforms//cpu:armv7e-m": [],
        "@platforms//cpu:armv8-m": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = ["@pigweed_config//:__pkg__"],
    deps = select({
        "@platforms//cpu:armv7-m": ["//pw_interrupt_cortex_m:context_armv7m"],
        "@platforms//cpu:armv7e-m": ["//pw_interrupt_cortex_m:context_armv7m"],
        "@platforms//cpu:armv8-m": ["//pw_interrupt_cortex_m:context_armv8m"],
        # This is required for this to be a valid select when building for the
        # host i.e. 'bazel build //pw_interrupt/...'. The
        # target_compatible_with attribute is used to skip this target when
        # built with a wildcard. If explicitly depended on for a host build
        # the build will fail.
        "//conditions:default": [],
    }),
)
