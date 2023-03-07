# Copyright 2019 The Pigweed Authors
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
"""Pigweed build environment for bazel."""

load(
    "//pw_build/bazel_internal:pigweed_internal.bzl",
    _add_cc_and_c_targets = "add_cc_and_c_targets",
    _has_pw_assert_dep = "has_pw_assert_dep",
)

def pw_cc_binary(**kwargs):
    kwargs["deps"] = kwargs.get("deps", [])

    # TODO(b/234877642): Remove this implicit dependency once we have a better
    # way to handle the facades without introducing a circular dependency into
    # the build.
    if not _has_pw_assert_dep(kwargs["deps"]):
        kwargs["deps"] = kwargs["deps"] + ["@pigweed//pw_assert"]
    _add_cc_and_c_targets(native.cc_binary, kwargs)

def pw_cc_library(**kwargs):
    _add_cc_and_c_targets(native.cc_library, kwargs)

def pw_cc_test(**kwargs):
    kwargs["deps"] = kwargs.get("deps", []) + \
                     ["@pigweed//pw_unit_test:simple_printing_main"]

    # TODO(b/234877642): Remove this implicit dependency once we have a better
    # way to handle the facades without introducing a circular dependency into
    # the build.
    if not _has_pw_assert_dep(kwargs["deps"]):
        kwargs["deps"] = kwargs["deps"] + ["@pigweed//pw_assert"]
    _add_cc_and_c_targets(native.cc_test, kwargs)

def pw_cc_perf_test(**kwargs):
    kwargs["deps"] = kwargs.get("deps", []) + \
                     ["@pigweed//pw_perf_test:logging_main"]

    if not _has_pw_assert_dep(kwargs["deps"]):
        kwargs["deps"] = kwargs["deps"] + ["@pigweed//pw_assert"]
    _add_cc_and_c_targets(native.cc_binary, kwargs)

def pw_cc_facade(**kwargs):
    # Bazel facades should be source only cc_library's this is to simplify
    # lazy header evaluation. Bazel headers are not 'precompiled' so the build
    # system does not check to see if the build has the right dependant headers
    # in the sandbox. If a source file is declared here and includes a header
    # file the toolchain will compile as normal and complain about the missing
    # backend headers.
    if "srcs" in kwargs.keys():
        fail("'srcs' attribute does not exist in pw_cc_facade, please use \
        main implementing target.")
    _add_cc_and_c_targets(native.cc_library, kwargs)
