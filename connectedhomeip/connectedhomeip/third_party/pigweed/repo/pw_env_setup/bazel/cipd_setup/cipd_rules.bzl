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
"""Bazel rules for downloading CIPD packages."""

load(
    "//pw_env_setup/bazel/cipd_setup/internal:cipd_internal.bzl",
    _cipd_client_impl = "cipd_client_impl",
    _cipd_deps_impl = "cipd_deps_impl",
    _cipd_repository_impl = "cipd_repository_impl",
)

_cipd_client_repository = repository_rule(
    _cipd_client_impl,
    attrs = {
        "_cipd_version_file": attr.label(default = "@pigweed//pw_env_setup:py/pw_env_setup/cipd_setup/.cipd_version"),
        "_cipd_digest_file": attr.label(default = "@pigweed//pw_env_setup:py/pw_env_setup/cipd_setup/.cipd_version.digests"),
    },
    doc = """
Fetches the cipd client.

This rule should not be used directly and instead should be called via
the cipd_client_repository macro.
""",
)

def cipd_client_repository():
    """Fetches the cipd client.

    Fetches the cipd client to the prescribed remote repository target
    prefix 'cipd_client'. This rule should be called before a
    cipd_repository rule is instantiated.
    """
    _cipd_client_repository(
        name = "cipd_client",
    )

cipd_repository = repository_rule(
    _cipd_repository_impl,
    attrs = {
        "_cipd_client": attr.label(default = "@cipd_client//:cipd"),
        "path": attr.string(),
        "tag": attr.string(),
    },
    doc = """
Downloads a singular CIPD dependency to the root of a remote repository.

Example:

    load(
        "//pw_env_setup/bazel/cipd_setup:cipd_rules.bzl",
        "cipd_client_repository",
        "cipd_repository",
    )

    # Must be called before cipd_repository
    cipd_client_repository()

    cipd_repository(
        name = "bloaty",
        path = "pigweed/third_party/bloaty-embedded/${os=linux,mac}-${arch=amd64}",
        tag = "git_revision:2d87d204057b419f5290f8d38b61b9c2c5b4fb52-2",
    )
""",
)

_pigweed_deps = repository_rule(
    _cipd_deps_impl,
    attrs = {
        "_pigweed_packages_json": attr.label(
            default = "@pigweed//pw_env_setup:py/pw_env_setup/cipd_setup/pigweed.json",
        ),
        "_python_packages_json": attr.label(
            default = "@pigweed//pw_env_setup:py/pw_env_setup/cipd_setup/python.json",
        ),
        "_upstream_testing_packages_json": attr.label(
            default = "@pigweed//pw_env_setup:py/pw_env_setup/cipd_setup/testing.json",
        ),
    },
)

def pigweed_deps():
    """Configures Pigweeds Bazel dependencies

    Example:
        load("@pigweed//pw_env_setup:pigweed_deps.bzl", "pigweed_deps")

        pigweed_deps()

        load("@cipd_deps//:cipd_init.bzl", "cipd_init")

        cipd_init()
"""
    _pigweed_deps(
        name = "cipd_deps",
    )
