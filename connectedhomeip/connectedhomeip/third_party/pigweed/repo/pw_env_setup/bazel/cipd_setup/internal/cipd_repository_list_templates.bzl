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
"""CIPD rule templates."""

CIPD_REPOSITORY_TEMPLATE = """
{indent}cipd_repository(
{indent}    name = "{name}",
{indent}    path = "{path}",
{indent}    tag = "{tag}",
{indent})"""

CIPD_INIT_BZL_TEMPLATE = """
load("@pigweed//pw_env_setup/bazel/cipd_setup:cipd_rules.bzl",
    "cipd_repository",
    "cipd_client_repository",
)

def cipd_init():
    cipd_client_repository()
{cipd_deps}
"""
