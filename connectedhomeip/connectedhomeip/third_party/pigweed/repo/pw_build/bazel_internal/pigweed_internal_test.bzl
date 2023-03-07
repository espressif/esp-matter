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
"""Unit tests of pigweed_internal.bzl."""

load("@bazel_skylib//lib:unittest.bzl", "asserts", "unittest")
load(":pigweed_internal.bzl", "has_pw_assert_dep")

def _has_pw_assert_dep_test_impl(ctx):
    env = unittest.begin(ctx)
    asserts.equals(env, True, has_pw_assert_dep([":somedep", "//pw_assert"]))
    asserts.equals(env, True, has_pw_assert_dep([":somedep", "//pw_assert:pw_assert"]))
    asserts.equals(env, True, has_pw_assert_dep([":somedep", "@pigweed//pw_assert"]))
    asserts.equals(env, True, has_pw_assert_dep([":somedep", "@pigweed//pw_assert:pw_assert"]))
    asserts.equals(env, False, has_pw_assert_dep([":somedep", ":someotherdep"]))
    return unittest.end(env)

has_pw_assert_dep_test = unittest.make(_has_pw_assert_dep_test_impl)

def pigweed_internal_test_suite(name):
    unittest.suite(
        name,
        has_pw_assert_dep_test,
    )
