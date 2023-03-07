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
"""Configure Pigweed's backend implementations."""

def _pigweed_config_impl(repository_ctx):
    if repository_ctx.attr.build_file_content and \
       repository_ctx.attr.build_file:
        fail("Attributes 'build_file_content' and 'build_file' cannot both be \
        defined at the same time.")
    if not repository_ctx.attr.build_file_content and \
       not repository_ctx.attr.build_file:
        fail("Either 'build_file_content' or 'build_file' must be defined.")

    if repository_ctx.name != "pigweed_config":
        fail("This repository should be name 'pigweed_config'")

    if repository_ctx.attr.build_file_content:
        repository_ctx.file("BUILD", repository_ctx.attr.build_file_content)

    if repository_ctx.attr.build_file:
        repository_ctx.template("BUILD", repository_ctx.attr.build_file, {})

pigweed_config = repository_rule(
    _pigweed_config_impl,
    attrs = {
        "build_file_content": attr.string(
            doc = "The build file content to configure Pigweed.",
            mandatory = False,
            default = "",
        ),
        "build_file": attr.label(
            doc = "The label for the Pigweed config build file to use.",
            mandatory = False,
            default = "@pigweed//targets/host:host_config.BUILD",
        ),
    },
    doc = """
Configure Pigweeds backend implementations.

Example:
    # WORKSPACE
    pigweed_config(
        # This must use the exact name specified here otherwise this
        # remote repository will not function as expected.
        name = "pigweed_config",
        build_file = "//path/to/config.BUILD",
    )
""",
)
