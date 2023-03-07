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
"""Utilities for fuzzing."""

load("@rules_fuzzing//fuzzing:cc_defs.bzl", "cc_fuzz_test")
load(
    "//pw_build/bazel_internal:pigweed_internal.bzl",
    _add_cc_and_c_targets = "add_cc_and_c_targets",
    _has_pw_assert_dep = "has_pw_assert_dep",
)

def pw_cc_fuzz_test(**kwargs):
    # TODO(b/234877642): Remove this implicit dependency once we have a better
    # way to handle the facades without introducing a circular dependency into
    # the build.
    if not _has_pw_assert_dep(kwargs["deps"]):
        kwargs["deps"].append("@pigweed//pw_assert")
    _add_cc_and_c_targets(cc_fuzz_test, kwargs)
